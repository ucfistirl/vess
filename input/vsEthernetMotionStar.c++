//------------------------------------------------------------------------
//
//    VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//    Copyright (c) 2001, University of Central Florida
//
//       See the file LICENSE for license information
//
//    E-mail:  vess@ist.ucf.edu
//    WWW:     http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//    VESS Module:  vsEthernetMotionStar.h++
//
//    Description:  Class to handle input from an Ascension MotionStar
//                  motion capture system.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "vsEthernetMotionStar.h++"

// Static class variable for instructing the server (child) process to exit
int vsEthernetMotionStar::serverDone;

// ------------------------------------------------------------------------
// Constructs a MotionStar for the server at the given host and port.
// The number of trackers specified by the nTrackers parameter will be 
// used. If nTrackers is zero, then all available trackers will be used.  
// The masterFlag parameter determines whether or not this instance is 
// responsible for controlling the MotionStar.
// ------------------------------------------------------------------------
vsEthernetMotionStar::vsEthernetMotionStar(char *serverName, int port, 
    int nTrackers, int masterFlag, int dFormat)
    : vsTrackingSystem()
{
    int      i;
    int      result;
    vsQuat   quat1; 
    vsQuat   quat2;
    vsQuat   xformQuat;
    vsMatrix headingMat;

    // Initialize variables
    addressMode = 0;
    numTrackers = 0;
    forked = VS_FALSE;
    serverPID = 0;
    sharedData = NULL;
    master = masterFlag;

    streaming = VS_FALSE;
    configured = VS_FALSE;

    posScale = VS_MSTAR_SCALE_DEFAULT_POS;

    for (i = 0; i < VS_MSTAR_MAX_TRACKERS; i++)
    {
        tracker[i] = NULL;
    }

    xmtrAddress = 0;

    // Set up the coordinate conversion quaternion that converts from native
    // Ascension coordinates to VESS coordinates.  
    quat1.setAxisAngleRotation(0, 0, 1, 90);
    quat2.setAxisAngleRotation(0, 1, 0, 180);
    coordXform = quat2 * quat1;

    // Open ethernet link
    net = new vsUDPUnicastNetworkInterface(serverName, port, VS_TRUE);

    if (net)
    {
        // If we're the master client . . .
        if (master)
        {
            // Get the system configuration from the MotionStar server
            result = configureSystem();

            // Check the number of expected trackers with the number
            // found
            if (numTrackers < nTrackers)
            {
                printf("vsEthernetMotionStar::"
                    "vsEthernetMotionStar:\n");
                printf("    WARNING -- Only %d sensors found, expecting %d\n",
                    numTrackers, nTrackers);
            }
    
            if ((numTrackers > nTrackers) && (nTrackers > 0))
            {
                printf("vsEthernetMotionStar::vsEthernetMotionStar:\n");
                printf("   Configuring %d of %d sensors\n", 
                    nTrackers, numTrackers);
    
                numTrackers = nTrackers;
            }
    
            // Change the data format for all the birds to the one
            // requested
            setDataFormat(VS_MSTAR_ALL_TRACKERS, dFormat);
    
            if (result != VS_FALSE)
            {
                printf("vsEthernetMotionStar::vsEthernetMotionStar:\n");
                printf("   MotionStar running on %s:%d with %d sensors\n", 
                    serverName, port, numTrackers);
            }
            else
            {
                printf("vsEthernetMotionStar::vsEthernetMotionStar:\n");
                printf("   MotionStar did not initialize properly.\n");
            }
        }
        else 
        {
            printf("vsEthernetMotionStar::vsEthernetMotionStar:\n");
            printf("    Listening on %s:%d for MotionStar data\n",
                serverName, port);
        }
    }
    else
    {
        printf("vsEthernetMotionStar::"
            "vsEthernetMotionStar:\n");
        printf("   Unable to open network connection at %s:%d", serverName,
            port);
    }
}

// ------------------------------------------------------------------------
// Destructs all vsMotionTrackers and puts the MotionStar to sleep
// ------------------------------------------------------------------------
vsEthernetMotionStar::~vsEthernetMotionStar()
{
    int i;

    // Delete motion trackers
    printf("vsEthernetMotionStar::"
        "~vsEthernetMotionStar:\n");
    printf("  Deleting vsMotionTrackers\n");
    for (i = 0; i < VS_MSTAR_MAX_TRACKERS; i++)
        if (tracker[i] != NULL)
            delete tracker[i];

    if (forked)
    {
        // Stop the server process if we've forked
        kill(serverPID, SIGUSR1);

        // Detach from shared memory
        delete sharedData;
    }

    // Shut down the MotionStar (if it's our job)
    if (!forked)
    {
        if (master)
        {
            printf("  Shutting down MotionStar\n");
            shutdownMStar();
            usleep(100000);
        }

        printf("  Closing network connection\n");
        if (net != NULL)
            delete net;
    }
}

// ------------------------------------------------------------------------
// Loop for the server portion of this class when operating in forked mode
// ------------------------------------------------------------------------
void vsEthernetMotionStar::serverLoop()
{
    int      i;
    vsVector posVec;
    vsQuat   ornQuat;

    // Set up the signal handler
    signal(SIGUSR1, vsEthernetMotionStar::quitServer);

    vsEthernetMotionStar::serverDone = VS_FALSE;

    // Start streaming data
    if (master)
    {
        startStream();
    }

    posVec.setSize(3);
    posVec.clear();
    ornQuat.clear();

    // Constantly update the shared data from hardware
    while (!vsEthernetMotionStar::serverDone)
    {
        updateSystem();

        for (i = 0; i < numTrackers; i++)
        {
            posVec = tracker[i]->getPositionVec();
            ornQuat = tracker[i]->getOrientationQuat();

            sharedData->storeVectorData(i, posVec);
            sharedData->storeQuatData(i, ornQuat);
        }
    }

    // Restore the default signal handler
    signal(SIGUSR1, SIG_DFL);

    // Detach from shared memory
    delete sharedData;

    // Clean up
    if (master)
    {
        printf("  Shutting down MotionStar\n");
        shutdownMStar();
        usleep(100000);
    }

    printf("  Closing network connection(s)\n");
    if (net != NULL)
        delete net;

    printf("vsEthernetMotionStar server process exiting...\n");

    exit(0);
}

// ------------------------------------------------------------------------
// Signal handler for the server process
// ------------------------------------------------------------------------
void vsEthernetMotionStar::quitServer(int arg)
{
    vsEthernetMotionStar::serverDone = VS_TRUE;
}


// ------------------------------------------------------------------------
// Package the given command into a MotionStar-friendly packet and send it
// Returns VS_TRUE if a valid response is available in the response 
// parameter, otherwise VS_FALSE is returned.
// ------------------------------------------------------------------------
int vsEthernetMotionStar::sendCommand(unsigned char command, 
                                      unsigned char xtype, 
                                      vsBirdnetPacket *response)
{
    vsBirdnetPacket commandPacket;

    commandPacket.header.sequence = htons(currentSequence++);
    commandPacket.header.type = command;
    commandPacket.header.xtype = xtype;
    commandPacket.header.protocol = VS_BN_PROTOCOL_VERSION;
    commandPacket.header.numBytes = 0;
    commandPacket.header.errorCode = 0;
    commandPacket.header.extErrorCode = 0;

    return sendPacket(&commandPacket, 16, response);
}

// ------------------------------------------------------------------------
// Send the given packet to the MotionStar and check for the proper 
// response.  Returns VS_TRUE if a valid response is available in the
// response parameter, otherwise VS_FALSE is returned.
// ------------------------------------------------------------------------
int vsEthernetMotionStar::sendPacket(vsBirdnetPacket *packet, int pktLength, 
                                     vsBirdnetPacket *response)
{
    unsigned short  commandType;
    int             packetLength;
    int             result;
    int             responseRequired;
    int             responseReceived;
    vsBirdnetPacket responsePacket;

    responseRequired = VS_TRUE;
    responseReceived = VS_FALSE;

    if (pktLength == 0)
    {
        packetLength = sizeof(*packet);
    }
    else
    {
        packetLength = pktLength;
    }

    commandType = packet->header.type;

    result = 0;

    // Loop until we've got a valid response or we determine we don't 
    // need one
    while (responseRequired)
    {
        result = net->writePacket((unsigned char *)packet, packetLength);

        usleep(10000);

        // If we're expecting a data packet back, don't wait for the 
        // response, so the updateSystem() function will receive it.
        // Also, the MotionStar server never seems to send a response to the 
        // shutdown command (even though it's supposed to), so we won't 
        // wait for one
        if ((commandType == VS_BN_MSG_SHUT_DOWN) || 
            (commandType == VS_BN_MSG_SINGLE_SHOT))
        {
            responseRequired = VS_FALSE;
            responseReceived = VS_FALSE;
        }

        if (responseRequired)
        {
            result = net->readPacket((unsigned char *)&responsePacket,
                 sizeof(vsBirdnetPacket));

            if ((result >= 16) &&
                (responsePacket.header.type != VS_BN_DATA_PACKET_MULTI))
            {
                responseRequired = VS_FALSE;
                responseReceived = VS_TRUE;
            }
        }
    }

    // Check the response for validity
    if (responseReceived)
    {
        currentSequence = ntohs(responsePacket.header.sequence);

        switch(responsePacket.header.type)
        {
            case VS_BN_RSP_ILLEGAL:
                printf("vsEthernetMotionStar::sendPacket:  "
                    "Packet type sent at the wrong time.\n");
                return VS_FALSE;
            case VS_BN_RSP_UNKNOWN:
                printf("vsEthernetMotionStar::sendPacket:  "
                    "Unknown command sent.\n");
                return VS_FALSE;
            case VS_BN_RSP_WAKE_UP:
                if (commandType == VS_BN_MSG_WAKE_UP)
                {
                    if (response != NULL)
                    {
                        memcpy(response, &responsePacket, 
                            sizeof(vsBirdnetHeader));
                        memcpy(&(response->buffer[0]), responsePacket.buffer, 
                            ntohs(responsePacket.header.numBytes));
                    }
    
                    return VS_TRUE;
                }
                else
                {
                    printf("vsEthernetMotionStar::sendPacket:  "
                        "Invalid response received: %d\n", 
                        responsePacket.header.type);
    
                    return VS_FALSE;
                }
            case VS_BN_RSP_SHUT_DOWN:
                if (commandType == VS_BN_MSG_SHUT_DOWN)
                {
                    if (response != NULL)
                    {
                        memcpy(response, &responsePacket, 
                            sizeof(vsBirdnetHeader));
                        memcpy(&(response->buffer[0]), responsePacket.buffer, 
                            ntohs(responsePacket.header.numBytes));
                    }
    
                    return VS_TRUE;
                }
                else
                {
                    printf("vsEthernetMotionStar::sendPacket:  "
                        "Invalid response received: %d\n",
                        responsePacket.header.type);
    
                    return VS_FALSE;
                }
            case VS_BN_RSP_GET_STATUS:
                if (commandType == VS_BN_MSG_GET_STATUS)
                {
                    if (response != NULL)
                    {
                        memcpy(response, &responsePacket, 
                            sizeof(vsBirdnetHeader));
                        memcpy(&(response->buffer[0]), responsePacket.buffer, 
                            ntohs(responsePacket.header.numBytes));
                    }
    
                    return VS_TRUE;
                }
                else
                {
                    printf("vsEthernetMotionStar::sendPacket:  "
                        "Invalid response received: %d\n",
                        responsePacket.header.type);
    
                    return VS_FALSE;
                }
            case VS_BN_RSP_SEND_SETUP:
                if (commandType == VS_BN_MSG_SEND_SETUP)
                {
                    if (response != NULL)
                    {
                        memcpy(response, &responsePacket, 
                            sizeof(vsBirdnetHeader));
                        memcpy(&(response->buffer[0]), responsePacket.buffer, 
                            ntohs(responsePacket.header.numBytes));
                    }
    
                    return VS_TRUE;
                }
                else
                {
                    printf("vsEthernetMotionStar::sendPacket:  "
                        "Invalid response received: %d\n",
                        responsePacket.header.type);
    
                    return VS_FALSE;
                }
            case VS_BN_RSP_RUN_CONTINUOUS:
                if (commandType == VS_BN_MSG_RUN_CONTINUOUS)
                {
                    if (response != NULL)
                    {
                        memcpy(response, &responsePacket, 
                            sizeof(vsBirdnetHeader));
                        memcpy(&(response->buffer[0]), responsePacket.buffer, 
                            ntohs(responsePacket.header.numBytes));
                    }
    
                    return VS_TRUE;
                }
                else
                {
                    printf("vsEthernetMotionStar::sendPacket:  "
                        "Invalid response received: %d\n",
                        responsePacket.header.type);
    
                    return VS_FALSE;
                }
            case VS_BN_RSP_STOP_DATA:
                if (commandType == VS_BN_MSG_STOP_DATA)
                {
                    if (response != NULL)
                    {
                        memcpy(response, &responsePacket, 
                            sizeof(vsBirdnetHeader));
                        memcpy(&(response->buffer[0]), responsePacket.buffer, 
                            ntohs(responsePacket.header.numBytes));
                    }
    
                    return VS_TRUE;
                }
                else
                {
                    printf("vsEthernetMotionStar::sendPacket:  "
                        "Invalid response received: %d\n",
                        responsePacket.header.type);
    
                    return VS_FALSE;
                }
            case VS_BN_RSP_SEND_DATA:
                if (commandType == VS_BN_MSG_SEND_DATA)
                {
                    if (response != NULL)
                    {
                        memcpy(response, &responsePacket, 
                            sizeof(vsBirdnetHeader));
                        memcpy(&(response->buffer[0]), responsePacket.buffer, 
                            ntohs(responsePacket.header.numBytes));
                    }
    
                    return VS_TRUE;
                }
                else
                {
                    if (response != NULL)
                    {
                        printf("vsEthernetMotionStar::sendPacket:  "
                            "Invalid response received: %d\n",
                            responsePacket.header.type);
                    }
    
                    return VS_FALSE;
                }
            case VS_BN_RSP_SYNC_SEQUENCE:
                if (commandType == VS_BN_MSG_SYNC_SEQUENCE)
                {
                    if (response != NULL)
                    {
                        memcpy(response, &responsePacket, 
                            sizeof(vsBirdnetHeader));
                        memcpy(&(response->buffer[0]), responsePacket.buffer, 
                            ntohs(responsePacket.header.numBytes));
                    }

                    return VS_TRUE;
                }
                else
                {
                    printf("vsEthernetMotionStar::sendPacket:  "
                        "Invalid response received: %d\n",
                        responsePacket.header.type);
    
                    return VS_FALSE;
                }
            case VS_BN_DATA_PACKET_MULTI:
                return VS_FALSE;
            case VS_BN_DATA_PACKET_SINGLE:
                return VS_FALSE;
            default:
                printf("vsEthernetMotionStar::sendPacket:  "
                    "Unknown response received: %d\n",
                    responsePacket.header.type);
                return VS_FALSE;
        }
    }

    return VS_FALSE;
}
    
// ------------------------------------------------------------------------
// Requests the MotionStar system status from the master bird, then constructs
// a vsMotionTracker for every bird with a contiguous address starting at
// 1.  Each bird with a sensor is enumerated with indices starting at 0,
// matching the tracker[] array. The function sets the instance variable
// numTrackers to the number of trackers available.
// ------------------------------------------------------------------------
int vsEthernetMotionStar::configureSystem()
{
    vsBirdnetPacket             response;
    vsBirdnetSystemStatusPacket status;
    int                         result;


    printf("vsEthernetMotionStar::configureSystem:\n");

    // First, wake the master server up
    sendCommand(VS_BN_MSG_WAKE_UP, 0, NULL);

    // Next, get the general system status from the master server
    result = sendCommand(VS_BN_MSG_GET_STATUS, 0, &response);
    
    // If we have a valid response
    if (result)
    {
        // Copy the status (it has all the info we need)

        memcpy(&status, &(response.buffer[0]), ntohs(response.header.numBytes));

        // Print some of the vital information

        printf("  MotionStar Server software revision:  %d.%d\n", 
            status.softwareRevision[0], status.softwareRevision[1]);
        printf("  Number of devices in system:          %d\n",
            status.flockNumber);
        printf("  Number of chassis in system:          %d\n",
            status.serverNumber);
        printf("  ID Number of this chassis:            %d\n",
            status.serverNumber);
        printf("  First FBB address in this chassis:    %d\n",
            status.firstAddress);
        printf("  Number of devices in this chassis:    %d\n",
            status.chassisDevices);
        printf("  Measurement rate:                     %c%c%c.%c%c%c Hz\n",
            status.measurementRate[0], status.measurementRate[1], 
            status.measurementRate[2], status.measurementRate[3], 
            status.measurementRate[4], status.measurementRate[5]);

        // Report any error conditions

        if (status.all & 0x40)
        {
            printf("  WARNING -- System error detected\n");
        }

        if (status.all & 0x20)
        {
            printf("  WARNING -- FBB error detected\n");
        }
        
        if (status.all & 0x10)
        {
            printf("  WARNING -- Local chassis error detected\n");
        }

        if (status.all & 0x08)
        {
            printf("  WARNING -- Local power status error detected\n");
        }

        if (status.serverNumber > 1)
        {
            printf("  WARNING -- Multiple chassis not supported\n");
        }

        // Create vsMotionTrackers for the devices with receivers
        enumerateTrackers(&status);

        return VS_TRUE;
    }
    else 
    {
        printf("  Error reading the MotionStar's status\n");
    }

    return VS_FALSE;
}

// ------------------------------------------------------------------------
// Counts and numbers the MotionStar's trackers
// ------------------------------------------------------------------------
void vsEthernetMotionStar::enumerateTrackers(
    vsBirdnetSystemStatusPacket *status)
{
    vsBirdnetPacket             response;
    vsBirdnetBirdStatusPacket   *birdStatus;
    vsBirdnetRefAlignmentPacket *refTable;
    int                         tableIndex;
    int                         result;
    int                         address;
    char                        description[7][11] = { "6DFOB     ", 
                                                       "6DERC     ", 
                                                       "6DBOF     ",
                                                       "PCBIRD    ", 
                                                       "SPACEPAD  ", 
                                                       "MOTIONSTAR",
                                                       "WIRELESS  "};

    printf("vsEthernetMotionStar::enumerateTrackers:\n");
 
    numTrackers = 0;

    // Walk the FBB in the chassis and configure all the devices
    for (address = 1; address <= status->chassisDevices; address++)
    {
        result = sendCommand(VS_BN_MSG_GET_STATUS, address, &response);
        
        if (result)
        {
    
            birdStatus = (vsBirdnetBirdStatusPacket *)&(response.buffer[0]);

            if ((birdStatus->id > 0) && (birdStatus->id < 8))
            {
                // Display the type of device
                printf("  Bird %d is a %s\t(id = %d)\n", address,
                    description[birdStatus->id - 1], birdStatus->id);

                // If it has a receiver, configure a vsMotionTracker for it
                if (birdStatus->status & VS_BN_FLOCK_RECEIVERPRESENT)
                {
                    // Construct tracker
                    tracker[numTrackers] = new vsMotionTracker(numTrackers);
 
                    // Set the FBB address
                    fbbAddress[numTrackers] = address;

                    // Initialize the configuration info
                    trackerConfig[numTrackers].dataFormat = 
                        birdStatus->dataFormat;
                    trackerConfig[numTrackers].hemisphere = 
                        birdStatus->hemisphere;
                    
                    tableIndex = sizeof(vsBirdnetBirdStatusPacket) + 
                        3 * sizeof(vsBirdnetFilterTablePacket);

                    refTable = (vsBirdnetRefAlignmentPacket *)
                        &(response.buffer[tableIndex]);
                    trackerConfig[numTrackers].refH = 
                        ntohs(refTable->azimuth);
                    trackerConfig[numTrackers].refP = 
                        ntohs(refTable->elevation);
                    trackerConfig[numTrackers].refR = 
                        ntohs(refTable->roll);

                    // Increment tracker count
                    numTrackers++;
                }
                else
                {
                    if (birdStatus->status & VS_BN_FLOCK_TRANSMITTERRUNNING)
                    {
                        // Store the address of the active transmitter
                        xmtrAddress = address;
                        printf("    Transmitter is active\n");

                        // Check the id field to set the position scaling
                        if (birdStatus->id == VS_BN_ERC)
                        {
                            // Set position scaling to 140" (ERT range)
                            posScale = VS_MSTAR_SCALE_ERT_POS;
                        }
                        else
                        {
                            // Default to standard short range transmitter
                            // range of 36"
                            posScale = VS_MSTAR_SCALE_SRT1_POS;
                        }
                    }
                }
            }
            else
            {
                // For some reason, our ERT returns 115 as its ID number, 
                // and reports that it has a sensor.  Even more odd, it
                // also reports that it has no transmitter.  I don't know 
                // why, so I'll make this a special case until I find out.
                if (birdStatus->id == 115)
                {
                    printf("  Bird %d is a 6DERC     \t(id = %d)\n", 
                        address, birdStatus->id);

                    posScale = VS_MSTAR_SCALE_ERT_POS;
                }
                else
                {
                    printf("  Bird %d is an unknown device\t(id = %d)\n", 
                        address, birdStatus->id);
                }
            }
        }
        else
        {
            printf("  No response from Bird %d!!\n", address);
        }
    }
}

// ------------------------------------------------------------------------
// Changes the MotionStar trackers' configuration to match the
// configuration information stored in the trackerConfig[] array.
//
// NOTE:  This must be called BEFORE startStream(), or the MotionStar 
//        server may hang when the stream is stopped and then restarted.
// ------------------------------------------------------------------------
void vsEthernetMotionStar::updateConfiguration()
{
    int                         stoppedStream;
    int                         index;
    int                         result;
    vsBirdnetPacket             response;
    vsBirdnetBirdStatusPacket   *bStatus;
    vsBirdnetRefAlignmentPacket *refTable;
    int                         tableIndex;

    // If we're streaming, we need to stop the stream
    if (streaming)
    {
        stopStream();
        stoppedStream = VS_TRUE;
    }
    else
    {
        stoppedStream = VS_FALSE;
    }

    // Talk to each tracker and send the new configuration info
    for (index = 0; index < numTrackers; index++)
    {
        result = sendCommand(VS_BN_MSG_GET_STATUS, fbbAddress[index], 
            &response);

        if (result)
        {
            response.header.sequence = htons(currentSequence++);
            response.header.type = VS_BN_MSG_SEND_SETUP;

            bStatus = (vsBirdnetBirdStatusPacket *)response.buffer;
            if (bStatus->FBBaddress == fbbAddress[index])
            {
                bStatus->dataFormat = trackerConfig[index].dataFormat;
                bStatus->hemisphere = trackerConfig[index].hemisphere;

                tableIndex = sizeof(vsBirdnetBirdStatusPacket) + 
                    3 * sizeof(vsBirdnetFilterTablePacket);

                refTable = (vsBirdnetRefAlignmentPacket *)
                    &(response.buffer[tableIndex]);

                refTable->azimuth = htons(trackerConfig[index].refH);
                refTable->elevation = htons(trackerConfig[index].refP);
                refTable->roll = htons(trackerConfig[index].refR);

                sendPacket(&response, 16 + ntohs(response.header.numBytes), 
                           NULL);
            }
        }
        else
        {
            printf("vsEthernetMotionStar::updateConfiguration: \n");
            printf("  Unable to get configuration data for Bird %d\n",
                fbbAddress[index]);
        }
    }

    // Pause for a bit
    usleep(100000);
    configured = VS_TRUE;

    // Restart the data stream if we stopped it
    if (stoppedStream)
    {
        startStream();
    }
}

// ------------------------------------------------------------------------
// Interprets the MotionStar data as a position record and sets the given
// tracker's data accordingly.  Orientation is set to zero.
// ------------------------------------------------------------------------
void vsEthernetMotionStar::updatePosition(int trackerIndex, short flockData[])
{
    vsVector posVec;
    vsQuat   ornQuat;

    posVec.setSize(3);
    posVec[VS_X] = flockData[0] * posScale;
    posVec[VS_Y] = flockData[1] * posScale;
    posVec[VS_Z] = flockData[2] * posScale;

    // Convert position to VESS coordinates
    posVec = coordXform.rotatePoint(posVec);

    ornQuat.setAxisAngleRotation(0.0, 0.0, 0.0, 1.0);

    tracker[trackerIndex]->setPosition(posVec);
    tracker[trackerIndex]->setOrientation(ornQuat);
}

// ------------------------------------------------------------------------
// Interprets the MotionStar data as an Euler angle record (heading, pitch,
// roll) and sets the given tracker's data accordingly.  Position is set to 
// zero.
// ------------------------------------------------------------------------
void vsEthernetMotionStar::updateAngles(int trackerIndex, short flockData[])
{
    vsVector posVec;
    double   h, p, r;
    vsQuat   ornQuat;

    posVec.setSize(3);
    posVec.clear();

    h = flockData[0] * VS_MSTAR_SCALE_ANGLE;
    p = flockData[1] * VS_MSTAR_SCALE_ANGLE;
    r = flockData[2] * VS_MSTAR_SCALE_ANGLE;

    // Convert orientation to VESS coordinates
    ornQuat.setEulerRotation(VS_EULER_ANGLES_ZYX_R, h, p, r);
    ornQuat = coordXform * ornQuat * coordXform;

    tracker[trackerIndex]->setPosition(posVec);
    tracker[trackerIndex]->setOrientation(ornQuat);
}

// ------------------------------------------------------------------------
// Interprets the MotionStar data as a rotation matrix record and sets the 
// given tracker's data accordingly.  Position is set to zero
// ------------------------------------------------------------------------
void vsEthernetMotionStar::updateMatrix(int trackerIndex, short flockData[])
{
    vsVector posVec;
    vsMatrix ornMat;
    int      i, j;
    vsQuat   ornQuat;

    posVec.setSize(3);
    posVec.clear();

    // Data is a 3x3 matrix stored in column-major order
    ornMat.setIdentity();
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            ornMat[j][i] = flockData[(i * 3) + j] * VS_MSTAR_SCALE_MATRIX;
        }
    }

    // The MotionStar's matrix is the transpose of what VESS expects
    ornMat.transpose();

    // Convert orientation to VESS coordinates
    ornQuat.setMatrixRotation(ornMat);
    ornQuat = coordXform * ornQuat * coordXform;

    tracker[trackerIndex]->setPosition(posVec);
    tracker[trackerIndex]->setOrientation(ornQuat);
}

// ------------------------------------------------------------------------
// Interprets the MotionStar data as a quaternion record and sets the given
// tracker's data accordingly.  Position is set to zero
// ------------------------------------------------------------------------
void vsEthernetMotionStar::updateQuaternion(int trackerIndex, short flockData[])
{
    vsVector posVec;
    vsQuat   ornQuat;

    posVec.setSize(3);
    posVec.clear();

    // Quaternion returned by the MotionStar has the scalar portion in front
    ornQuat[VS_X] = flockData[1] * VS_MSTAR_SCALE_QUAT;
    ornQuat[VS_Y] = flockData[2] * VS_MSTAR_SCALE_QUAT;
    ornQuat[VS_Z] = flockData[3] * VS_MSTAR_SCALE_QUAT;
    ornQuat[VS_W] = flockData[0] * VS_MSTAR_SCALE_QUAT;

    // The MotionStar's quaternion is the conjugate of what VESS expects
    ornQuat.conjugate();

    // Convert orientation to VESS coordinates
    ornQuat = coordXform * ornQuat * coordXform;

    tracker[trackerIndex]->setPosition(posVec);
    tracker[trackerIndex]->setOrientation(ornQuat);
}

// ------------------------------------------------------------------------
// Interprets the MotionStar data as a position/angles record and sets the 
// given tracker's data accordingly. 
// ------------------------------------------------------------------------
void vsEthernetMotionStar::updatePosAngles(int trackerIndex, short flockData[])
{
    vsVector posVec;
    double   h, p, r;
    vsQuat   ornQuat;

    posVec.setSize(3);
    posVec[VS_X] = flockData[0] * posScale;
    posVec[VS_Y] = flockData[1] * posScale;
    posVec[VS_Z] = flockData[2] * posScale;

    // Convert position to VESS coordinates
    posVec = coordXform.rotatePoint(posVec);

    h = flockData[3] * VS_MSTAR_SCALE_ANGLE;
    p = flockData[4] * VS_MSTAR_SCALE_ANGLE;
    r = flockData[5] * VS_MSTAR_SCALE_ANGLE;

    // Convert orientation to VESS coordinates
    ornQuat.setEulerRotation(VS_EULER_ANGLES_ZYX_R, h, p, r);
    ornQuat = coordXform * ornQuat * coordXform;

    tracker[trackerIndex]->setPosition(posVec);
    tracker[trackerIndex]->setOrientation(ornQuat);
}

// ------------------------------------------------------------------------
// Interprets the MotionStar data as a position/matrix record and sets the 
// given tracker's data accordingly. 
// ------------------------------------------------------------------------
void vsEthernetMotionStar::updatePosMatrix(int trackerIndex, short flockData[])
{
    vsVector posVec;
    vsMatrix ornMat;
    int      i,j;
    vsQuat   ornQuat;

    posVec.setSize(3);
    posVec[VS_X] = flockData[0] * posScale;
    posVec[VS_Y] = flockData[1] * posScale;
    posVec[VS_Z] = flockData[2] * posScale;

    // Convert position to VESS coordinates
    posVec = coordXform.rotatePoint(posVec);

    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            ornMat[j][i] = flockData[(i * 3) + j + 3] * VS_MSTAR_SCALE_MATRIX;
        }
    }

    // The MotionStar's matrix is the transpose of what VESS expects
    ornMat.transpose();

    // Convert orientation to VESS coordinates
    ornQuat.setMatrixRotation(ornMat);
    ornQuat = coordXform * ornQuat * coordXform;

    tracker[trackerIndex]->setPosition(posVec);
    tracker[trackerIndex]->setOrientation(ornQuat);
}

// ------------------------------------------------------------------------
// Interprets the MotionStar data as a position record and sets the given
// tracker's data accordingly.  Orientation is set to zero
// ------------------------------------------------------------------------
void vsEthernetMotionStar::updatePosQuat(int trackerIndex, short flockData[])
{
    vsVector posVec;
    vsQuat   ornQuat;

    posVec.setSize(3);
    posVec[VS_X] = flockData[0] * posScale;
    posVec[VS_Y] = flockData[1] * posScale;
    posVec[VS_Z] = flockData[2] * posScale;

    // Convert position to VESS coordinates
    posVec = coordXform.rotatePoint(posVec);

    ornQuat[VS_X] = flockData[4] * VS_MSTAR_SCALE_QUAT;
    ornQuat[VS_Y] = flockData[5] * VS_MSTAR_SCALE_QUAT;
    ornQuat[VS_Z] = flockData[6] * VS_MSTAR_SCALE_QUAT;
    ornQuat[VS_W] = flockData[3] * VS_MSTAR_SCALE_QUAT;

    // The MotionStar's quaternion is the conjugate of what VESS expects
    ornQuat.conjugate();

    // Convert orientation to VESS coordinates
    ornQuat = coordXform * ornQuat * coordXform;

    tracker[trackerIndex]->setPosition(posVec);
    tracker[trackerIndex]->setOrientation(ornQuat);
}

// ------------------------------------------------------------------------
// Spawn a separate (server) process that continuously reads the device
// and updates the vsMotionTracker data
// ------------------------------------------------------------------------
void vsEthernetMotionStar::forkTracking()
{
    key_t  theKey;
    time_t tod;

    // Use a portion of the time of day as the second half of the shared
    // memory key
    tod = time(NULL);
    tod &= 0x0000FFFF;

    theKey = VS_MSTAR_SHM_KEY_BASE | tod;

    // If we don't yet know the number of trackers, wait until we do before
    // forking
    while (numTrackers == 0)
        updateSystem();

    // Fork off the server process
    serverPID = fork();

    switch(serverPID)
    {
        case -1:
            printf("vsEthernetMotionStar::forkTracking:\n");
            printf("    fork() failed, continuing in single-process mode\n");
            break;
        case 0:
            sharedData = new vsSharedInputData(theKey, numTrackers, VS_TRUE);
            serverLoop();
            break;
        default:
            sharedData = new vsSharedInputData(theKey, numTrackers, VS_FALSE);
            forked = VS_TRUE;
            printf("vsEthernetMotionStar::forkTracking:\n");
            printf("    Server PID is %d\n", serverPID);
    }
}

// ------------------------------------------------------------------------
// Request a data packet from the MotionStar.  This command is ignored if
// the MotionStar is already streaming data.
// ------------------------------------------------------------------------
void vsEthernetMotionStar::ping()
{
    if ((!streaming) && (master))
    {
        // Update the system configuration if it has changed
        if (!configured)
        {
            updateConfiguration();
        }

        sendCommand(VS_BN_MSG_SINGLE_SHOT, 0, NULL);
    }
}

// ------------------------------------------------------------------------
// Update the motion tracker data with fresh data from the MotionStar
// ------------------------------------------------------------------------
void vsEthernetMotionStar::updateSystem()
{
    vsBirdnetPacket dataPacket;
    int             dataBytes;
    int             currentByte;
    int             birdDataFormat;
    int             birdButtonFlag;
    int             birdDataSize;
    short           birdData[30];
    short           tempWord;
    int             j;
    int             currentAddress;
    int             currentTracker;

    // Ping for a packet if we're not streaming
    if ((master) && (!streaming))
    {
        ping();
    }

    // Read the data packet
    net->readPacket((unsigned char *)&dataPacket,
        sizeof(vsBirdnetPacket));

    if ((dataPacket.header.type == VS_BN_DATA_PACKET_MULTI) ||
        (dataPacket.header.type == VS_BN_DATA_PACKET_SINGLE))
    {
        dataBytes = ntohs(dataPacket.header.numBytes);
        currentByte = 0;

        while (currentByte < dataBytes)
        {
            // The bird's address is in the first byte of the data record
            // (the high bit is the flag indicating button data is present)
            currentAddress = dataPacket.buffer[currentByte] & 0x7F;
            birdButtonFlag = (dataPacket.buffer[currentByte] & 0x80) >> 7;
            currentByte++;
    
            // The bird's data format and data size in words are in the next 
            // byte
            birdDataFormat = (dataPacket.buffer[currentByte] >> 4) & 0x0F;
            birdDataSize = (dataPacket.buffer[currentByte] & 0x0F) * 2;
            currentByte++;
        
            // Find the right vsMotionTracker based on the FBB address
            currentTracker = 0;
            while ((currentTracker < numTrackers) &&
                (fbbAddress[currentTracker] != currentAddress))
                currentTracker++;

            // Make sure we have a valid tracker
            if ((master) && 
                ((currentTracker >= numTrackers) || 
                 (tracker[currentTracker] == NULL))) 
            {
                printf("vsEthernetMotionStar::updateSystem:\n");
                printf("   Data received for an invalid tracker\n");
                printf("   FBB Address:  %d   numTrackers:  %d\n",
                    currentAddress, numTrackers);
            }
            else
            {
                // If we're a slave instance, and we don't have a 
                // vsMotionTracker yet created for this data, create it
                // now
                if ((currentTracker >= numTrackers) || 
                    (tracker[currentTracker] == NULL))
                {
                    printf("Creating tracker #%d at FBB Address %d\n",
                        numTrackers, currentAddress);

                    tracker[numTrackers] = new vsMotionTracker(numTrackers);
                    fbbAddress[numTrackers] = currentAddress;
                    numTrackers++;
                }

                // Extract the tracker's data from the packet
                for (j = 0; j < birdDataSize - 1; j += 2)
                {
                    tempWord = 
                        (dataPacket.buffer[currentByte + j] << 8) & 0xFF00;
                    tempWord |= 
                        dataPacket.buffer[currentByte + j + 1] & 0x00FF;

                    birdData[j/2] = tempWord;
                }
     
                switch (birdDataFormat)
                {
                    case VS_BN_FLOCK_POSITION:
                        updatePosition(currentTracker, birdData);
                        break;
                    case VS_BN_FLOCK_ANGLES:
                        updateAngles(currentTracker, birdData);
                        break;
                    case VS_BN_FLOCK_MATRIX:
                        updateMatrix(currentTracker, birdData);
                        break;
                    case VS_BN_FLOCK_QUATERNION:
                        updateQuaternion(currentTracker, birdData);
                        break;
                    case VS_BN_FLOCK_POSITIONANGLES: 
                        updatePosAngles(currentTracker, birdData);
                        break;
                    case VS_BN_FLOCK_POSITIONMATRIX:
                        updatePosMatrix(currentTracker, birdData);
                        break;
                    case VS_BN_FLOCK_POSITIONQUATERNION:
                        updatePosQuat(currentTracker, birdData);
                        break;
                }
    
                // Advance the currentByte index by birdDataSize bytes
                currentByte += birdDataSize;
    
                // Skip the button data if present (we don't support buttons
                // yet)
                if (birdButtonFlag)
                    currentByte += 2;
            }
        }
    }
}

// ------------------------------------------------------------------------
// Start the MotionStar continuously streaming data.  The MotionStar should 
// be run in a separate process when using this mode.
// ------------------------------------------------------------------------
void vsEthernetMotionStar::startStream()
{
    if (master)
    {
        // Update the system configuration if it has changed
        if (!configured)
        {
            printf("Updating MotionStar configuration\n");
            updateConfiguration();
        }

        sendCommand(VS_BN_MSG_RUN_CONTINUOUS, 0, NULL);

        streaming = VS_TRUE;
    }
}

// ------------------------------------------------------------------------
// Stop the continuous data stream from the MotionStar
// ------------------------------------------------------------------------
void vsEthernetMotionStar::stopStream()
{
    vsBirdnetPacket trashPacket;
    int             retryCount;
    int             result;

    if (master)
    {
        // Read excess packets until we receive a data packet
        memset(&trashPacket, 0, sizeof(vsBirdnetPacket));

        while ((trashPacket.header.type != VS_BN_DATA_PACKET_SINGLE) &&
            (trashPacket.header.type != VS_BN_DATA_PACKET_MULTI))
        {
            net->readPacket((unsigned char *)&trashPacket, 
                sizeof(vsBirdnetPacket));

            currentSequence = trashPacket.header.sequence + 1;
        }

        // Attempt to stop the data stream
        if (streaming)
        {
            retryCount = 0;

            while ((streaming) && (retryCount < 10))
            {
                printf("    Sending MSG_STOP_DATA...");
                result = sendCommand(VS_BN_MSG_STOP_DATA, 0, NULL);

                if (result)
                {
                    streaming = VS_FALSE;
                    printf("success!!\n");
                }
                else
                {
                    retryCount++;
                    printf("failed\n");
                }

                usleep(100000);
            }
        }
    }
}

// ------------------------------------------------------------------------
// Change the data format to the one specified
// ------------------------------------------------------------------------
void vsEthernetMotionStar::setDataFormat(int trackerNum, int format)
{
    int index;
    int dataFormat;

    if (master)
    {
        configured = VS_FALSE;

        // Set the data format to the requested format
        switch (format)
        {
            case VS_BN_FLOCK_NOBIRDDATA:
                printf("  Setting data format to NOBIRDDATA\n");
                dataFormat = 0 | (VS_BN_FLOCK_NOBIRDDATA & 0x0F);
                break;
            case VS_BN_FLOCK_POSITION:
                printf("  Setting data format to POSITION\n");
                dataFormat = (3 << 4) | (VS_BN_FLOCK_POSITION & 0x0F);
                break;
            case VS_BN_FLOCK_ANGLES:
                printf("  Setting data format to ANGLES\n");
                dataFormat = (3 << 4) | (VS_BN_FLOCK_ANGLES & 0x0F);
                break;
            case VS_BN_FLOCK_MATRIX:
                printf("  Setting data format to MATRIX\n");
                dataFormat = (9 << 4) | (VS_BN_FLOCK_MATRIX & 0x0F);
                break;
            case VS_BN_FLOCK_QUATERNION:
                printf("  Setting data format to QUATERNION\n");
                dataFormat = (4 << 4) | (VS_BN_FLOCK_QUATERNION & 0x0F);
                break;
            case VS_BN_FLOCK_POSITIONANGLES:
                printf("  Setting data format to POSITIONANGLES\n");
                dataFormat = (6 << 4) | (VS_BN_FLOCK_POSITIONANGLES & 0x0F);
                break;
            case VS_BN_FLOCK_POSITIONMATRIX:
                printf("  Setting data format to POSITIONMATRIX\n");
                dataFormat = (12 << 4) | (VS_BN_FLOCK_POSITIONMATRIX & 0x0F);
                break;
            case VS_BN_FLOCK_POSITIONQUATERNION:
                printf("  Setting data format to POSITIONQUATERNION\n");
                dataFormat = (7 << 4) | (VS_BN_FLOCK_POSITIONQUATERNION & 0x0F);
                break;
            default:
                printf("   Invalid data format %d, "
                    "assuming POSITIONQUATERNION\n", dataFormat);
                dataFormat = VS_BN_FLOCK_POSITIONQUATERNION;
        }

        // Change the configuration for the appropriate bird(s)
        if (trackerNum == VS_MSTAR_ALL_TRACKERS)
        {
            for (index = 0; index <= numTrackers; index++)
            {
                trackerConfig[index].dataFormat = dataFormat;
            }
        }
        else
        {
            if ((trackerNum >= 0) && (trackerNum < numTrackers))
            {
                trackerConfig[trackerNum].dataFormat = dataFormat;
            }
        }
    }
}

// ------------------------------------------------------------------------
// Change the transmitter hemisphere in which the given sensor is located.
// Use one of the VS_BN_X_HEMISPHERE constants defined in the header file 
// as the value for the hSphere parameter.
// ------------------------------------------------------------------------
void vsEthernetMotionStar::setActiveHemisphere(int trackerNum, short hSphere)
{
    int index;

    // Check for valid parameter
    if ((master) && (hSphere > 0) && (hSphere <= 5))
    {
        configured = VS_FALSE;

        // Change the configuration for the appropriate bird(s)
        if (trackerNum == VS_MSTAR_ALL_TRACKERS)
        {
            for (index = 0; index <= numTrackers; index++)
            {
                trackerConfig[index].hemisphere = hSphere;
            }
        }
        else
        {
            if ((trackerNum >= 0) && (trackerNum < numTrackers))
            {
                trackerConfig[trackerNum].hemisphere = hSphere;
            }
        }
    }
}

// ------------------------------------------------------------------------
// Change the reference frame of the MotionStar sensors.  Subsequent 
// orientation measurements will be offset by the amount specified.
// ------------------------------------------------------------------------
void vsEthernetMotionStar::setReferenceFrame(int trackerNum, float h, float p, 
                                             float r)
{
    short         az, pt, rl;
    int           index;

    if (master)
    {
        // Validate and convert the parameters
        if ((h >= -180.0) && (h <= 179.99) &&
            (p >= -90.0) && (p <= 89.99) &&
            (r >= -180.0) && (r <= 179.99)) 
        {
            configured = VS_FALSE;

            az = (short)(h / VS_MSTAR_SCALE_ANGLE);
            pt = (short)(p / VS_MSTAR_SCALE_ANGLE);
            rl = (short)(r / VS_MSTAR_SCALE_ANGLE);

            // Change the configuration for the appropriate bird(s)
            if (trackerNum == VS_MSTAR_ALL_TRACKERS)
            {
                for (index = 0; index < numTrackers; index++)
                {
                    trackerConfig[index].refH = az;
                    trackerConfig[index].refP = pt;
                    trackerConfig[index].refR = rl;
                }
            }
            else
            {
                if ((trackerNum >= 0) && (trackerNum < numTrackers))
                {
                    trackerConfig[trackerNum].refH = az;
                    trackerConfig[trackerNum].refP = pt;
                    trackerConfig[trackerNum].refR = rl;
                }
            }
        }
        else
        {
            printf("vsEthernetMotionStar::setReferenceFrame: "
                "Value out of range\n");
        }
    }
}

// ------------------------------------------------------------------------
// Open a connection to the MotionStar server
// ------------------------------------------------------------------------
void vsEthernetMotionStar::wakeMStar()
{
    int result;

    currentSequence = 1;

    if (master)
    {
        result = sendCommand(VS_BN_MSG_WAKE_UP, 0, NULL);

        if (!result)
        {
            printf("vsEthernetMotionStar::wakeMStar: "
                "ERROR -- Unable to wake MotionStar\n");
        }
    }
}

// ------------------------------------------------------------------------
// Shut down the connection to the MotionStar server
// ------------------------------------------------------------------------
void vsEthernetMotionStar::shutdownMStar()
{
    if (master)
    {
        printf("    Halting data stream\n");
        stopStream();

        printf("    Sending MSG_SHUT_DOWN\n");
        sendCommand(VS_BN_MSG_SHUT_DOWN, 0, NULL);
    }
}

// ------------------------------------------------------------------------
// Return the number of trackers currently running
// ------------------------------------------------------------------------
int vsEthernetMotionStar::getNumTrackers()
{
    return numTrackers;
}

// ------------------------------------------------------------------------
// Return the tracker at the specified index
// ------------------------------------------------------------------------
vsMotionTracker *vsEthernetMotionStar::getTracker(int index)
{
    if (index < numTrackers)
        return tracker[index];
    else
        return NULL;
}

// ------------------------------------------------------------------------
// Update the motion trackers with fresh data, either from the MotionStar,
// or from shared memory
// ------------------------------------------------------------------------
void vsEthernetMotionStar::update()
{
    int      i;
    vsVector posVec;
    vsQuat   ornQuat;

    if (forked)
    {
        // Copy tracker data from shared memory
        for (i = 0; i < numTrackers; i++)
        {
            posVec.setSize(3);
            sharedData->retrieveVectorData(i, &posVec);
            sharedData->retrieveQuatData(i, &ornQuat);

            tracker[i]->setPosition(posVec);
            tracker[i]->setOrientation(ornQuat);
        }
    }
    else
    {
        // Get tracker data from hardware
        updateSystem();
    }
}
