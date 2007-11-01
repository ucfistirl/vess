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
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "vsEthernetMotionStar.h++"

// Static class variable for instructing the server (child) process to exit
bool vsEthernetMotionStar::serverDone;

// ------------------------------------------------------------------------
// Constructs a MotionStar for the server at the given host and port.
// The number of trackers specified by the nTrackers parameter will be 
// used. If nTrackers is zero, then all available trackers will be used.  
// The masterFlag parameter determines whether or not this instance is 
// responsible for controlling the MotionStar.
// ------------------------------------------------------------------------
vsEthernetMotionStar::vsEthernetMotionStar(char *serverName, int port, 
    int nTrackers, bool masterFlag, int dFormat)
    : vsTrackingSystem()
{
    int      i;
    bool     result;
    atQuat   quat1; 
    atQuat   quat2;
    atQuat   xformQuat;
    atMatrix headingMat;

    // Initialize state variables
    addressMode = 0;
    numTrackers = 0;
    forked = false;
    serverPID = 0;
    sharedData = NULL;
    master = masterFlag;
    streaming = false;
    configured = false;
    posScale = VS_MSTAR_SCALE_DEFAULT_POS;
    xmtrAddress = 0;

    // Initialize tracker array
    for (i = 0; i < VS_MSTAR_MAX_TRACKERS; i++)
    {
        tracker[i] = NULL;
    }

    // Set up the coordinate conversion quaternion that converts from native
    // Ascension coordinates to VESS coordinates.  
    quat1.setAxisAngleRotation(0, 0, 1, 90);
    quat2.setAxisAngleRotation(0, 1, 0, 180);
    coordXform = quat2 * quat1;

    // Open ethernet link
    net = new vsUDPNetworkInterface(serverName, port);
    net->enableBlocking();

    // Make sure it opened, print an error if not
    if (net)
    {
        // Configure the MotionStar if we're the master client (slaves 
        // don't do any configuration)
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
    
            // Print a status/warning message if we are using fewer trackers
            // than available
            if ((numTrackers > nTrackers) && (nTrackers > 0))
            {
                printf("vsEthernetMotionStar::vsEthernetMotionStar:\n");
                printf("   Configuring %d of %d sensors\n", 
                    nTrackers, numTrackers);
    
                // Update the number of trackers
                numTrackers = nTrackers;
            }
    
            // Change the data format for all the birds to the one
            // requested
            setDataFormat(VS_MSTAR_ALL_TRACKERS, dFormat);
    
            // Print a message indicating whether or not we've properly
            // configured everything
            if (result != false)
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
            // This is a slave instance, print a message that we're
            // listening for tracker/configuration data
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

    // Check if we've spawned a server process
    if (forked)
    {
        // Stop the server process if we've forked
        kill(serverPID, SIGUSR1);

        // Detach from shared memory
        delete sharedData;
    }

    // Shut down the MotionStar (the server process will do this if we've
    // forked)
    if (!forked)
    {
        // Don't do anything with the MotionStar if we're a slave instace
        if (master)
        {
            printf("  Shutting down MotionStar\n");
            shutdownMStar();
            usleep(100000);
        }

        // Close the network connection
        printf("  Closing network connection\n");
        if (net != NULL)
            delete net;
    }
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsEthernetMotionStar::getClassName()
{
    return "vsEthernetMotionStar";
}

// ------------------------------------------------------------------------
// Loop for the server portion of this class when operating in forked mode
// ------------------------------------------------------------------------
void vsEthernetMotionStar::serverLoop()
{
    int      i;
    atVector posVec;
    atQuat   ornQuat;

    // Set up the signal handler
    signal(SIGUSR1, vsEthernetMotionStar::quitServer);

    // Initialize the done flag to false
    vsEthernetMotionStar::serverDone = false;

    // Start streaming data
    if (master)
    {
        startStream();
    }

    // Initialize the data structures
    posVec.setSize(3);
    posVec.clear();
    ornQuat.clear();

    // Constantly update the shared data from hardware
    while (!vsEthernetMotionStar::serverDone)
    {
        // Update the hardware
        updateSystem();

        // Collect and store tracker data in shared memory
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

    // Close the socket
    printf("  Closing network connection(s)\n");
    if (net != NULL)
        delete net;

    // Exit the server process
    printf("vsEthernetMotionStar server process exiting...\n");
    exit(0);
}

// ------------------------------------------------------------------------
// Signal handler for the server process
// ------------------------------------------------------------------------
void vsEthernetMotionStar::quitServer(int arg)
{
    vsEthernetMotionStar::serverDone = true;
}


// ------------------------------------------------------------------------
// Package the given command into a MotionStar-friendly packet and send it
// Returns true if a valid response is available in the response 
// parameter, otherwise false is returned.
// ------------------------------------------------------------------------
bool vsEthernetMotionStar::sendCommand(unsigned char command, 
                                       unsigned char xtype, 
                                       vsBirdnetPacket *response)
{
    vsBirdnetPacket commandPacket;

    // Set up a standard BirdNet command packet
    commandPacket.header.sequence = htons(currentSequence++);
    commandPacket.header.type = command;
    commandPacket.header.xtype = xtype;
    commandPacket.header.protocol = VS_BN_PROTOCOL_VERSION;
    commandPacket.header.numBytes = 0;
    commandPacket.header.errorCode = 0;
    commandPacket.header.extErrorCode = 0;

    // Call the other sendPacket function to handle the command
    return sendPacket(&commandPacket, 16, response);
}

// ------------------------------------------------------------------------
// Send the given packet to the MotionStar and check for the proper 
// response.  Returns true if a valid response is available in the
// response parameter, otherwise false is returned.
// ------------------------------------------------------------------------
bool vsEthernetMotionStar::sendPacket(vsBirdnetPacket *packet, int pktLength, 
                                      vsBirdnetPacket *response)
{
    unsigned short  commandType;
    int             packetLength;
    int             result;
    bool            responseRequired;
    bool            responseReceived;
    vsBirdnetPacket responsePacket;

    // Initialize the response flags
    responseRequired = true;
    responseReceived = false;

    // If the packet length is specified, use this value, otherwise
    // use the size of the packet in memory
    if (pktLength == 0)
    {
        packetLength = sizeof(*packet);
    }
    else
    {
        packetLength = pktLength;
    }

    // Get the command type
    commandType = packet->header.type;

    // Initialize the result value
    result = 0;

    // Loop until we've got a valid response or we determine we don't 
    // need one
    while (responseRequired)
    {
        // Send the packet
        result = net->write((unsigned char *)packet, packetLength);

        // Wait for hardware
        usleep(10000);

        // If we're expecting a data packet back, don't wait for the 
        // response, so the updateSystem() function will receive it.
        // Also, the MotionStar server never seems to send a response to the 
        // shutdown command (even though it's supposed to), so we won't 
        // wait for one
        if ((commandType == VS_BN_MSG_SHUT_DOWN) || 
            (commandType == VS_BN_MSG_SINGLE_SHOT))
        {
            responseRequired = false;
            responseReceived = false;
        }

        // If after the above check we still need a response, try to 
        // read it here
        if (responseRequired)
        {
            // Read the response
            result = net->read((unsigned char *)&responsePacket,
                 sizeof(vsBirdnetPacket));

            // Make sure the response is at least as big as a BirdNet
            // header, and is not a data packet
            if ((result >= sizeof(vsBirdnetHeader)) &&
                (responsePacket.header.type != VS_BN_DATA_PACKET_MULTI))
            {
                responseRequired = false;
                responseReceived = true;
            }
        }
    }

    // Check the response for validity
    if (responseReceived)
    {
        // Byte swap the sequence value
        currentSequence = ntohs(responsePacket.header.sequence);

        // Check the header type with the command issued and make sure
        // they match.  If they do, copy and return the response if a 
        // response buffer is provided.  Print an error if they don't match 
        // or if the response is itself an error message.
        switch(responsePacket.header.type)
        {
            case VS_BN_RSP_ILLEGAL:
                printf("vsEthernetMotionStar::sendPacket:  "
                    "Packet type sent at the wrong time.\n");
                return false;

            case VS_BN_RSP_UNKNOWN:
                printf("vsEthernetMotionStar::sendPacket:  "
                    "Unknown command sent.\n");
                return false;

            case VS_BN_RSP_WAKE_UP:
                // Make sure we get the proper response
                if (commandType == VS_BN_MSG_WAKE_UP)
                {
                    // See if we need to return the response data
                    if (response != NULL)
                    {
                        // Copy the response packet's header
                        memcpy(response, &responsePacket, 
                            sizeof(vsBirdnetHeader));

                        // Copy the remaining data
                        memcpy(&(response->buffer[0]), responsePacket.buffer, 
                            ntohs(responsePacket.header.numBytes));
                    }
    
                    return true;
                }
                else
                {
                    printf("vsEthernetMotionStar::sendPacket:  "
                        "Invalid response received: %d\n", 
                        responsePacket.header.type);
    
                    return false;
                }

            case VS_BN_RSP_SHUT_DOWN:
                // Make sure we get the proper response
                if (commandType == VS_BN_MSG_SHUT_DOWN)
                {
                    // See if we need to return the response data
                    if (response != NULL)
                    {
                        // Copy the response packet's header
                        memcpy(response, &responsePacket, 
                            sizeof(vsBirdnetHeader));

                        // Copy the remaining data
                        memcpy(&(response->buffer[0]), responsePacket.buffer, 
                            ntohs(responsePacket.header.numBytes));
                    }
    
                    return true;
                }
                else
                {
                    printf("vsEthernetMotionStar::sendPacket:  "
                        "Invalid response received: %d\n",
                        responsePacket.header.type);
    
                    return false;
                }

            case VS_BN_RSP_GET_STATUS:
                // Make sure we get the proper response
                if (commandType == VS_BN_MSG_GET_STATUS)
                {
                    // See if we need to return the response data
                    if (response != NULL)
                    {
                        // Copy the response packet's header
                        memcpy(response, &responsePacket, 
                            sizeof(vsBirdnetHeader));

                        // Copy the remaining data
                        memcpy(&(response->buffer[0]), responsePacket.buffer, 
                            ntohs(responsePacket.header.numBytes));
                    }
    
                    return true;
                }
                else
                {
                    printf("vsEthernetMotionStar::sendPacket:  "
                        "Invalid response received: %d\n",
                        responsePacket.header.type);
    
                    return false;
                }

            case VS_BN_RSP_SEND_SETUP:
                // Make sure we get the proper response
                if (commandType == VS_BN_MSG_SEND_SETUP)
                {
                    // See if we need to return the response data
                    if (response != NULL)
                    {
                        // Copy the response packet's header
                        memcpy(response, &responsePacket, 
                            sizeof(vsBirdnetHeader));

                        // Copy the remaining data
                        memcpy(&(response->buffer[0]), responsePacket.buffer, 
                            ntohs(responsePacket.header.numBytes));
                    }
    
                    return true;
                }
                else
                {
                    printf("vsEthernetMotionStar::sendPacket:  "
                        "Invalid response received: %d\n",
                        responsePacket.header.type);
    
                    return false;
                }

            case VS_BN_RSP_RUN_CONTINUOUS:
                // Make sure we get the proper response
                if (commandType == VS_BN_MSG_RUN_CONTINUOUS)
                {
                    // See if we need to return the response data
                    if (response != NULL)
                    {
                        // Copy the response packet's header
                        memcpy(response, &responsePacket, 
                            sizeof(vsBirdnetHeader));

                        // Copy the remaining data
                        memcpy(&(response->buffer[0]), responsePacket.buffer, 
                            ntohs(responsePacket.header.numBytes));
                    }
    
                    return true;
                }
                else
                {
                    printf("vsEthernetMotionStar::sendPacket:  "
                        "Invalid response received: %d\n",
                        responsePacket.header.type);
    
                    return false;
                }

            case VS_BN_RSP_STOP_DATA:
                // Make sure we get the proper response
                if (commandType == VS_BN_MSG_STOP_DATA)
                {
                    // See if we need to return the response data
                    if (response != NULL)
                    {
                        // Copy the response packet's header
                        memcpy(response, &responsePacket, 
                            sizeof(vsBirdnetHeader));

                        // Copy the remaining data
                        memcpy(&(response->buffer[0]), responsePacket.buffer, 
                            ntohs(responsePacket.header.numBytes));
                    }
    
                    return true;
                }
                else
                {
                    printf("vsEthernetMotionStar::sendPacket:  "
                        "Invalid response received: %d\n",
                        responsePacket.header.type);
    
                    return false;
                }

            case VS_BN_RSP_SEND_DATA:
                // Make sure we get the proper response
                if (commandType == VS_BN_MSG_SEND_DATA)
                {
                    // See if we need to return the response data
                    if (response != NULL)
                    {
                        // Copy the response packet's header
                        memcpy(response, &responsePacket, 
                            sizeof(vsBirdnetHeader));

                        // Copy the remaining data
                        memcpy(&(response->buffer[0]), responsePacket.buffer, 
                            ntohs(responsePacket.header.numBytes));
                    }
    
                    return true;
                }
                else
                {
                    // Only print an error if we're expecting a response
                    // and didn't get what we wanted.
                    if (response != NULL)
                    {
                        printf("vsEthernetMotionStar::sendPacket:  "
                            "Invalid response received: %d\n",
                            responsePacket.header.type);
                    }
    
                    return false;
                }

            case VS_BN_RSP_SYNC_SEQUENCE:
                // Make sure we get the proper response
                if (commandType == VS_BN_MSG_SYNC_SEQUENCE)
                {
                    // See if we need to return the response data
                    if (response != NULL)
                    {
                        // Copy the response packet's header
                        memcpy(response, &responsePacket, 
                            sizeof(vsBirdnetHeader));

                        // Copy the remaining data
                        memcpy(&(response->buffer[0]), responsePacket.buffer, 
                            ntohs(responsePacket.header.numBytes));
                    }

                    return true;
                }
                else
                {
                    printf("vsEthernetMotionStar::sendPacket:  "
                        "Invalid response received: %d\n",
                        responsePacket.header.type);
    
                    return false;
                }

            case VS_BN_DATA_PACKET_MULTI:
                // This is never a valid command response
                return false;

            case VS_BN_DATA_PACKET_SINGLE:
                // This is never a valid command response
                return false;

            default:
                printf("vsEthernetMotionStar::sendPacket:  "
                    "Unknown response received: %d\n",
                    responsePacket.header.type);
                return false;
        }
    }

    // No response received (whether by error or by design)
    return false;
}
    
// ------------------------------------------------------------------------
// Requests the MotionStar system status from the master bird, then constructs
// a vsMotionTracker for every bird with a contiguous address starting at
// 1.  Each bird with a sensor is enumerated with indices starting at 0,
// matching the tracker[] array. The function sets the instance variable
// numTrackers to the number of trackers available.
// ------------------------------------------------------------------------
bool vsEthernetMotionStar::configureSystem()
{
    vsBirdnetPacket             response;
    vsBirdnetSystemStatusPacket status;
    bool                        result;

    // Print status information as we go
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

        return true;
    }
    else 
    {
        printf("  Error reading the MotionStar's status\n");
    }

    return false;
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
    bool                        result;
    int                         address;
    char                        description[7][11] = { "6DFOB     ", 
                                                       "6DERC     ", 
                                                       "6DBOF     ",
                                                       "PCBIRD    ", 
                                                       "SPACEPAD  ", 
                                                       "MOTIONSTAR",
                                                       "WIRELESS  "};

    // Print status information as we go
    printf("vsEthernetMotionStar::enumerateTrackers:\n");
 
    // Initialize the tracker counter
    numTrackers = 0;

    // Walk the FBB in the chassis and configure all the devices
    for (address = 1; address <= status->chassisDevices; address++)
    {
        // Send the get status request
        result = sendCommand(VS_BN_MSG_GET_STATUS, address, &response);
        
        // If we get a valid response
        if (result)
        {
            // Get the bird status packet
            birdStatus = (vsBirdnetBirdStatusPacket *)&(response.buffer[0]);

            // ID should be strictly between 0 and 8
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

                    // Initialize the data format and hemisphere
                    trackerConfig[numTrackers].dataFormat = 
                        birdStatus->dataFormat;
                    trackerConfig[numTrackers].hemisphere = 
                        birdStatus->hemisphere;
                    
                    // Skip to the alignment table in the status packet
                    tableIndex = sizeof(vsBirdnetBirdStatusPacket) + 
                        3 * sizeof(vsBirdnetFilterTablePacket);

                    // Read the angle alignment table
                    refTable = (vsBirdnetRefAlignmentPacket *)
                        &(response.buffer[tableIndex]);

                    // Store the alignment angles
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
                    // See if this is a transmitter controller
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
                // For some reason, our ERC returns 115 as its ID number, 
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
    bool                        stoppedStream;
    int                         index;
    bool                        result;
    vsBirdnetPacket             response;
    vsBirdnetBirdStatusPacket   *bStatus;
    vsBirdnetRefAlignmentPacket *refTable;
    int                         tableIndex;

    // If we're streaming, we need to stop the stream
    if (streaming)
    {
        stopStream();
        stoppedStream = true;
    }
    else
    {
        // Set this to false so we know not to start streaming again
        stoppedStream = false;
    }

    // Talk to each tracker and send the new configuration info
    for (index = 0; index < numTrackers; index++)
    {
        // Get the current status from the MotionStar
        result = sendCommand(VS_BN_MSG_GET_STATUS, fbbAddress[index], 
            &response);

        // Make sure we get a good result
        if (result)
        {
            // Change the sequence number and change the type to a setup 
            // request
            response.header.sequence = htons(currentSequence++);
            response.header.type = VS_BN_MSG_SEND_SETUP;

            // Get the bird status packet from the response
            bStatus = (vsBirdnetBirdStatusPacket *)response.buffer;
            if (bStatus->FBBaddress == fbbAddress[index])
            {
                // Update the data format and hemisphere
                bStatus->dataFormat = trackerConfig[index].dataFormat;
                bStatus->hemisphere = trackerConfig[index].hemisphere;

                // Find the angle alignment table in the packet
                tableIndex = sizeof(vsBirdnetBirdStatusPacket) + 
                    3 * sizeof(vsBirdnetFilterTablePacket);

                // Get the table
                refTable = (vsBirdnetRefAlignmentPacket *)
                    &(response.buffer[tableIndex]);

                // Update the values in the table
                refTable->azimuth = htons(trackerConfig[index].refH);
                refTable->elevation = htons(trackerConfig[index].refP);
                refTable->roll = htons(trackerConfig[index].refR);

                // Send the new values to the MotionStar
                sendPacket(&response, sizeof(vsBirdnetHeader) + 
                    ntohs(response.header.numBytes), NULL);
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
    configured = true;

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
    atVector posVec;
    atQuat   ornQuat;

    // Get the position data
    posVec.setSize(3);
    posVec[AT_X] = flockData[0] * posScale;
    posVec[AT_Y] = flockData[1] * posScale;
    posVec[AT_Z] = flockData[2] * posScale;

    // Convert position to VESS coordinates
    posVec = coordXform.rotatePoint(posVec);

    // Set the orientation to identity
    ornQuat.setAxisAngleRotation(0.0, 0.0, 0.0, 1.0);

    // Update the motion tracker
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
    atVector posVec;
    double   h, p, r;
    atQuat   ornQuat;

    // Initialize the position vector to zero
    posVec.setSize(3);
    posVec.clear();

    // Get the Euler angle data
    h = flockData[0] * VS_MSTAR_SCALE_ANGLE;
    p = flockData[1] * VS_MSTAR_SCALE_ANGLE;
    r = flockData[2] * VS_MSTAR_SCALE_ANGLE;

    // Convert orientation to VESS coordinates
    ornQuat.setEulerRotation(AT_EULER_ANGLES_ZYX_R, h, p, r);
    ornQuat = coordXform * ornQuat * coordXform;

    // Update the motion tracker
    tracker[trackerIndex]->setPosition(posVec);
    tracker[trackerIndex]->setOrientation(ornQuat);
}

// ------------------------------------------------------------------------
// Interprets the MotionStar data as a rotation matrix record and sets the 
// given tracker's data accordingly.  Position is set to zero
// ------------------------------------------------------------------------
void vsEthernetMotionStar::updateMatrix(int trackerIndex, short flockData[])
{
    atVector posVec;
    atMatrix ornMat;
    int      i, j;
    atQuat   ornQuat;

    // Clear the position vector
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

    // Update the motion tracker
    tracker[trackerIndex]->setPosition(posVec);
    tracker[trackerIndex]->setOrientation(ornQuat);
}

// ------------------------------------------------------------------------
// Interprets the MotionStar data as a quaternion record and sets the given
// tracker's data accordingly.  Position is set to zero
// ------------------------------------------------------------------------
void vsEthernetMotionStar::updateQuaternion(int trackerIndex, short flockData[])
{
    atVector posVec;
    atQuat   ornQuat;

    // Clear the position vector
    posVec.setSize(3);
    posVec.clear();

    // Quaternion returned by the MotionStar has the scalar portion in front
    ornQuat[AT_X] = flockData[1] * VS_MSTAR_SCALE_QUAT;
    ornQuat[AT_Y] = flockData[2] * VS_MSTAR_SCALE_QUAT;
    ornQuat[AT_Z] = flockData[3] * VS_MSTAR_SCALE_QUAT;
    ornQuat[AT_W] = flockData[0] * VS_MSTAR_SCALE_QUAT;

    // The MotionStar's quaternion is the conjugate of what VESS expects
    ornQuat.conjugate();

    // Convert orientation to VESS coordinates
    ornQuat = coordXform * ornQuat * coordXform;

    // Update the motion tracker
    tracker[trackerIndex]->setPosition(posVec);
    tracker[trackerIndex]->setOrientation(ornQuat);
}

// ------------------------------------------------------------------------
// Interprets the MotionStar data as a position/angles record and sets the 
// given tracker's data accordingly. 
// ------------------------------------------------------------------------
void vsEthernetMotionStar::updatePosAngles(int trackerIndex, short flockData[])
{
    atVector posVec;
    double   h, p, r;
    atQuat   ornQuat;

    // Get the position data
    posVec.setSize(3);
    posVec[AT_X] = flockData[0] * posScale;
    posVec[AT_Y] = flockData[1] * posScale;
    posVec[AT_Z] = flockData[2] * posScale;

    // Convert position to VESS coordinates
    posVec = coordXform.rotatePoint(posVec);

    // Get the Euler angle data
    h = flockData[3] * VS_MSTAR_SCALE_ANGLE;
    p = flockData[4] * VS_MSTAR_SCALE_ANGLE;
    r = flockData[5] * VS_MSTAR_SCALE_ANGLE;

    // Convert orientation to VESS coordinates
    ornQuat.setEulerRotation(AT_EULER_ANGLES_ZYX_R, h, p, r);
    ornQuat = coordXform * ornQuat * coordXform;

    // Update the motion tracker
    tracker[trackerIndex]->setPosition(posVec);
    tracker[trackerIndex]->setOrientation(ornQuat);
}

// ------------------------------------------------------------------------
// Interprets the MotionStar data as a position/matrix record and sets the 
// given tracker's data accordingly. 
// ------------------------------------------------------------------------
void vsEthernetMotionStar::updatePosMatrix(int trackerIndex, short flockData[])
{
    atVector posVec;
    atMatrix ornMat;
    int      i,j;
    atQuat   ornQuat;

    // Get the position data
    posVec.setSize(3);
    posVec[AT_X] = flockData[0] * posScale;
    posVec[AT_Y] = flockData[1] * posScale;
    posVec[AT_Z] = flockData[2] * posScale;

    // Convert position to VESS coordinates
    posVec = coordXform.rotatePoint(posVec);

    // Get the rotation matrix
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

    // Update the motion tracker
    tracker[trackerIndex]->setPosition(posVec);
    tracker[trackerIndex]->setOrientation(ornQuat);
}

// ------------------------------------------------------------------------
// Interprets the MotionStar data as a position record and sets the given
// tracker's data accordingly.  Orientation is set to zero
// ------------------------------------------------------------------------
void vsEthernetMotionStar::updatePosQuat(int trackerIndex, short flockData[])
{
    atVector posVec;
    atQuat   ornQuat;

    // Get the position data
    posVec.setSize(3);
    posVec[AT_X] = flockData[0] * posScale;
    posVec[AT_Y] = flockData[1] * posScale;
    posVec[AT_Z] = flockData[2] * posScale;

    // Convert position to VESS coordinates
    posVec = coordXform.rotatePoint(posVec);

    // Get the quaternion
    ornQuat[AT_X] = flockData[4] * VS_MSTAR_SCALE_QUAT;
    ornQuat[AT_Y] = flockData[5] * VS_MSTAR_SCALE_QUAT;
    ornQuat[AT_Z] = flockData[6] * VS_MSTAR_SCALE_QUAT;
    ornQuat[AT_W] = flockData[3] * VS_MSTAR_SCALE_QUAT;

    // The MotionStar's quaternion is the conjugate of what VESS expects
    ornQuat.conjugate();

    // Convert orientation to VESS coordinates
    ornQuat = coordXform * ornQuat * coordXform;

    // Update the motion tracker
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

    // Use a portion of the time of day for the second half of the shared
    // memory key.  This helps prevent multiple shared memory segments with
    // the same key.
    tod = time(NULL);
    tod &= 0x0000FFFF;
    theKey = VS_MSTAR_SHM_KEY_BASE | tod;

    // If we don't yet know the number of trackers, wait until we do before
    // forking
    while (numTrackers == 0)
        updateSystem();

    // Fork off the server process
    serverPID = fork();

    // Branch according to what process we're in now
    switch(serverPID)
    {
        case -1:
            // Oops, the fork() failed
            printf("vsEthernetMotionStar::forkTracking:\n");
            printf("    fork() failed, continuing in single-process mode\n");
            break;

        case 0:
            // Server process, create the shared memory and start the server
            // loop
            sharedData = new vsSharedInputData(theKey, numTrackers, true);
            serverLoop();
            break;

        default:
            // Application process, connect to (don't create) the shared 
            // memory and begin reading data from the server process
            sharedData = new vsSharedInputData(theKey, numTrackers, false);
            forked = true;
            printf("vsEthernetMotionStar::forkTracking:\n");
            printf("    Server PID is %d\n", serverPID);
            break;
    }
}

// ------------------------------------------------------------------------
// Request a data packet from the MotionStar.  This command is ignored if
// the MotionStar is already streaming data.
// ------------------------------------------------------------------------
void vsEthernetMotionStar::ping()
{
    // Only actually send the ping if we're not streaming and we're the
    // master instance.  Otherwise, silently ignore this command.
    if ((!streaming) && (master))
    {
        // Update the system configuration if it has changed
        if (!configured)
        {
            updateConfiguration();
        }

        // Send the ping
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
    bool            birdButtonFlag;
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
    net->read((unsigned char *)&dataPacket, sizeof(vsBirdnetPacket));

    // Make sure the packet is the correct type
    if ((dataPacket.header.type == VS_BN_DATA_PACKET_MULTI) ||
        (dataPacket.header.type == VS_BN_DATA_PACKET_SINGLE))
    {
        // Read the data size, and initialize the byte counter
        dataBytes = ntohs(dataPacket.header.numBytes);
        currentByte = 0;

        // Keep processing until we run out of data
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

                // Extract the tracker's data from the packet (this method
                // is described in the Ascension manual)
                for (j = 0; j < birdDataSize - 1; j += 2)
                {
                    tempWord = 
                        (dataPacket.buffer[currentByte + j] << 8) & 0xFF00;
                    tempWord |= 
                        dataPacket.buffer[currentByte + j + 1] & 0x00FF;

                    birdData[j/2] = tempWord;
                }
     
                // Update the tracker data depending on the current data
                // format
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

        // Send the run continuous command to the MotionStar
        sendCommand(VS_BN_MSG_RUN_CONTINUOUS, 0, NULL);

        // Set the streaming flag so we know we're now in streaming mode
        streaming = true;
    }
}

// ------------------------------------------------------------------------
// Stop the continuous data stream from the MotionStar
// ------------------------------------------------------------------------
void vsEthernetMotionStar::stopStream()
{
    vsBirdnetPacket trashPacket;
    int             retryCount;
    bool            result;

    // The MotionStar is finicky when it comes to shutting down.  This
    // is why this code is so convoluted.

    // Only try this if we're the master instance
    if (master)
    {
        // Initialize a packet
        memset(&trashPacket, 0, sizeof(vsBirdnetPacket));

        // Read excess packets until we receive a data packet
        while ((trashPacket.header.type != VS_BN_DATA_PACKET_SINGLE) &&
            (trashPacket.header.type != VS_BN_DATA_PACKET_MULTI))
        {
            net->read((unsigned char *)&trashPacket, sizeof(vsBirdnetPacket));

            // Increment the sequence number
            currentSequence = trashPacket.header.sequence + 1;
        }

        // Attempt to stop the data stream
        if (streaming)
        {
            // Initialize the retry counter
            retryCount = 0;

            // Keep trying up to 10 times until the MotionStar acknowledges
            // it has stopped streaming
            while ((streaming) && (retryCount < 10))
            {
                printf("    Sending MSG_STOP_DATA...");
                result = sendCommand(VS_BN_MSG_STOP_DATA, 0, NULL);

                // Check the result
                if (result)
                {
                    streaming = false;
                    printf("success!!\n");
                }
                else
                {
                    // Increment retry
                    retryCount++;
                    printf("failed\n");
                }

                // Wait a moment for the hardware to catch up
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

    // Only the master instance can do this
    if (master)
    {
        // Indicate we have changed the configuration
        configured = false;

        // Set the data format to the requested format.  The most significant
        // nybble indicates the data size, and the least significant nybble
        //  is the actual data format
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
                    "assuming POSITIONQUATERNION\n", format);
                dataFormat = VS_BN_FLOCK_POSITIONQUATERNION;
                break;
        }

        // Change the configuration for the appropriate bird(s)
        if (trackerNum == VS_MSTAR_ALL_TRACKERS)
        {
            // Change for all birds
            for (index = 0; index <= numTrackers; index++)
            {
                trackerConfig[index].dataFormat = dataFormat;
            }
        }
        else
        {
            // Validate the tracker index and change the appropriate bird
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

    // Check to make sure we're the master instance and the parameter is
    // valid.  Valid values for the hSphere parameter range from 0 to 5,
    // each integer representing a possible hemisphere of the transmitter.
    if ((master) && (hSphere >= 0) && (hSphere <= 5))
    {
        // Indicate the configuration has changed
        configured = false;

        // Change the configuration for the appropriate bird(s)
        if (trackerNum == VS_MSTAR_ALL_TRACKERS)
        {
            // Change all birds
            for (index = 0; index <= numTrackers; index++)
            {
                trackerConfig[index].hemisphere = hSphere;
            }
        }
        else
        {
            // Change the given bird, if the index is valid
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

    // Only the master instance can do this
    if (master)
    {
        // Validate and convert the parameters.  Valid parameter values
        // are [-180.0, 180.0) for heading and roll, and [-90.0, 90.0) for
        // pitch.  These are hardware restrictions.
        if ((h >= -180.0) && (h <= 179.99) &&
            (p >= -90.0) && (p <= 89.99) &&
            (r >= -180.0) && (r <= 179.99)) 
        {
            // Indicate we've changed the configuration
            configured = false;

            // Convert the angles to Ascension-friendly format
            az = (short)(h / VS_MSTAR_SCALE_ANGLE);
            pt = (short)(p / VS_MSTAR_SCALE_ANGLE);
            rl = (short)(r / VS_MSTAR_SCALE_ANGLE);

            // Change the configuration for the appropriate bird(s)
            if (trackerNum == VS_MSTAR_ALL_TRACKERS)
            {
                // Change all birds
                for (index = 0; index < numTrackers; index++)
                {
                    trackerConfig[index].refH = az;
                    trackerConfig[index].refP = pt;
                    trackerConfig[index].refR = rl;
                }
            }
            else
            {
                // Change the given bird if the index is valid
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
    bool    result;

    // Initialize the packet sequence number to 1
    currentSequence = 1;

    // Only the master instance can command the MotionStar
    if (master)
    {
        // Send the wake up command
        result = sendCommand(VS_BN_MSG_WAKE_UP, 0, NULL);

        // Print an error if no response is received
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
    // Only the master instance can command the MotionStar
    if (master)
    {
        // If we're currently streaming data, stop the stream now
        printf("    Halting data stream\n");
        stopStream();

        // Send the shutdown command to the MotionStar
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
    atVector posVec;
    atQuat   ornQuat;

    // Check to see if we've forked a server process
    if (forked)
    {
        // Get the latest tracker data from shared memory for all trackers
        for (i = 0; i < numTrackers; i++)
        {
            // Copy tracker data from shared memory
            posVec.setSize(3);
            sharedData->retrieveVectorData(i, &posVec);
            sharedData->retrieveQuatData(i, &ornQuat);

            // Apply the new data to the vsMotionTracker
            tracker[i]->setPosition(posVec);
            tracker[i]->setOrientation(ornQuat);
        }
    }
    else
    {
        // Get tracker data from hardware
        updateSystem();
    }

    // Update all input devices
    for( i=0; i<numTrackers; i++ )
    {
        if( tracker[i] )
            tracker[i]->update();
    }
}
