#include "vsIS600.h++"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

// Static class data member to instruct the server process to exit
int vsIS600::serverDone;

// ------------------------------------------------------------------------
// Constructs a vsIS600 on the specified port with the given number of
// trackers.  If nTrackers is zero, the class attempts to determine the 
// number automatically
// ------------------------------------------------------------------------
vsIS600::vsIS600(int portNumber, long baud, int nTrackers)
         : vsTrackingSystem()
{
    char   portDevice[20];
    int    i;
    vsQuat quat1, quat2;

    // Determine the platform-dependent serial device
    // name

#ifdef IRIX
    sprintf(portDevice, "/dev/ttyd%d", portNumber);
#endif

#ifdef IRIX64
    sprintf(portDevice, "/dev/ttyd%d", portNumber);
#endif

#ifdef __linux__
    sprintf(portDevice, "/dev/ttyS%d", portNumber - 1);
#endif

    // Initialize variables
    port = NULL;
    numTrackers = 0;
    forked = VS_FALSE;
    serverPID = 0;

    quat1.setAxisAngleRotation(0, 0, 1, 90);
    quat2.setAxisAngleRotation(0, 1, 0, 180);
    coordXform = quat2 * quat1;

    for (i = 0; i < VS_IS_MAX_TRACKERS; i++)
    {
        tracker[i] = NULL;
    }

    // Open serial port at the given baud rate
    port = new vsSerialPort(portDevice, baud, 8, 'N', 1);

    if (port)
    {
        // Determine the number of available trackers
        enumerateTrackers();

        // Check the number of expected trackers with the number found
        if (numTrackers < nTrackers)
        {
            printf("vsIS600::vsIS600: "
                "WARNING -- Only %d trackers found, expecting %d\n",
                numTrackers, nTrackers);
        }

        if ((numTrackers > nTrackers) && (nTrackers > 0))
        {
            printf("vsIS600::vsIS600: Configuring %d of %d trackers\n",
                nTrackers, numTrackers);

            numTrackers = nTrackers;
        }

        // Set some default configurations
        setBinaryOutput();

        initOutputFormat();

        streaming = VS_FALSE;

        printf("vsIS600::vsIS600: IS-600 running on %s "
            "with %d tracker(s)\n", portDevice, numTrackers);

        ping();
    }
    else
    {
        printf("vsIS600::vsIS600: "
            "Unable to open serial port %s", portDevice);
    }

}

// ------------------------------------------------------------------------
// Destructs the vsMotionTrackers and closes the serial port.
// ------------------------------------------------------------------------
vsIS600::~vsIS600()
{
    int           i;
    unsigned char buf;

    // Delete the motion trackers
    for (i = 0; i < VS_IS_MAX_TRACKERS; i++)
    {
        if (tracker[i] != NULL)
            delete tracker[i];
    }

    // Terminate the server process
    if (forked)
    {
        printf("vsIS600::~vsIS600:  Notifying server process to quit\n");
        kill(serverPID, SIGUSR1);
    }

    // Shut down the IS-600
    if ((port != NULL) && (!forked))
    {
        printf("vsIS600::~vsIS600:  Shutting down IS-600\n");

        // Reset the IS-600
        buf = VS_IS_CMD_STOP_CONTINUOUS;
        port->writePacket(&buf, 1);
        sleep(1);
        port->flushPort();

        delete port;
    }
}

// ------------------------------------------------------------------------
// Loop for the server portion of this class when operating in multiple
// processes
// ------------------------------------------------------------------------
void vsIS600::serverLoop()
{
    int               i;
    vsVector          posVec;
    vsQuat            ornQuat;
    unsigned char     buf;

    // Set up the signal handler
    signal(SIGUSR1, vsIS600::quitServer);

    vsIS600::serverDone = VS_FALSE;

    // Start streaming data
    startStream();

    posVec.setSize(3);
    posVec.clear();
    ornQuat.clear();

    // Constantly update the shared data
    while (!vsIS600::serverDone)
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

    // Reset the default signal handler
    signal(SIGUSR1, SIG_DFL);

    // Detach from shared memory
    delete sharedData;

    // Clean up
    if (port != NULL)
    {
        printf("vsIS600::serverLoop:  Shutting down IS-600\n");

        // Reset the IS-600
        buf = VS_IS_CMD_STOP_CONTINUOUS;
        port->writePacket(&buf, 1);
        sleep(1);
        port->flushPort();

        delete port;
    }

    exit(0);
}

// ------------------------------------------------------------------------
// Signal handler for the server process
// ------------------------------------------------------------------------
void vsIS600::quitServer(int arg)
{
    vsIS600::serverDone = VS_TRUE;
}

// ------------------------------------------------------------------------
// Examines the 32 possible tracker "stations" on the IS-600 system, and
// constructs a vsMotionTracker for each one that is reported active.
// These trackers are numbered in the order found starting at 0.
// 
// NOTE:  Hereafter, the term "tracker number" or "tracker index" refers
//        to the number given a tracker by this function.  The term 
//        "station number" or "station index" refers to the tracking 
//        stations referenced by the hardware.  See the InterSense manual 
//        for more information on using and configuring tracking stations.
// ------------------------------------------------------------------------
void vsIS600::enumerateTrackers()
{
    unsigned char buf[VS_IS_SIZE_CMD_PACKET];
    int           result;
    int           i;

    // Stop the IS-600 from streaming (if it is)
    stopStream();
    port->flushPort();

    printf("vsIS600::enumerateTrackers:\n");

    buf[0] = VS_IS_CMD_STATION_STATE;
    buf[1] = '*';
    buf[2] = '\r';

    port->writePacket(buf, 3);

    result = port->readPacket(buf, 37);

    if (result < 37)
    {
        printf("   Error reading active station state (%d of 37 bytes)\n",
            result);
        port->flushPort();
    }

    buf[37] = 0;

    numTrackers = 0;

    for (i = 3; i < 35; i++)
    {
        // Report each active station
        if (buf[i] == '1')
        {
            tracker[numTrackers] = new vsMotionTracker(numTrackers);
            station[i-2] = numTrackers;
            numTrackers++;
            printf("    Station %d is active\n", i-2);
        }
        else
        {
            station[i-2] = -1;
        }
    }
}

// ------------------------------------------------------------------------
// Initialize the formatArray variable to the current output format at
// station 1.  All stations are then set to this format.
// ------------------------------------------------------------------------
void vsIS600::initOutputFormat()
{
    unsigned char buf[VS_IS_SIZE_CMD_PACKET];
    int           result;
    char          *token;

    // Stop streaming data and flush the serial port
    stopStream();
    port->flushPort();

    // Get the data format from station 1
    buf[0] = VS_IS_CMD_OUTPUT_LIST;
    buf[1] = '1';
    buf[2] = '\r';

    port->writePacket(buf, 3);

    result = port->readPacket(buf, VS_IS_SIZE_CMD_PACKET);
    buf[result] = 0;

    // Parse the format string into formatArray[]
    token = strtok((char *)(&buf[4]), " \n");
    formatNum = 0;
    while (token != NULL)
    {
        // Since we only support items 5, 6, and 7 (directional cosines)
        // together as a group, we need to throw out any stray 6's and 7's
        if ((atoi(token) != 6) && (atoi(token) != 7))
        {
            formatArray[formatNum] = atoi(token);
            formatNum++;
            token = strtok(NULL, " \n");
        }
    }

    // Set all stations to this format
    setOutputFormat(formatArray, formatNum);
}

// ------------------------------------------------------------------------
// Convert a little-endian 32-bit floating point number to big-endian (or 
// vice versa).  On Linux systems, this function simply returns the number
// unchanged.
// ------------------------------------------------------------------------
void vsIS600::endianSwap(float *inFloat, float *outFloat)
{
#ifdef __linux__
    *outFloat = *inFloat;
#else
    ((unsigned char *)outFloat)[0] = ((unsigned char *)inFloat)[3];
    ((unsigned char *)outFloat)[1] = ((unsigned char *)inFloat)[2];
    ((unsigned char *)outFloat)[2] = ((unsigned char *)inFloat)[1];
    ((unsigned char *)outFloat)[3] = ((unsigned char *)inFloat)[0];
#endif
}

// ------------------------------------------------------------------------
// Set the IS-600 to binary output mode
// ------------------------------------------------------------------------
void vsIS600::setBinaryOutput()
{
    unsigned char buf;

    buf = VS_IS_CMD_BINARY_OUTPUT;

    printf("vsIS600::setBinaryOutput: Switching to binary output\n");
    port->writePacket(&buf, 1);

    port->flushPort();
}

// ------------------------------------------------------------------------
// Update the given tracker's position with the given vsVector
// ------------------------------------------------------------------------
void vsIS600::updatePosition(int trackerNum, vsVector positionVec)
{
    if (trackerNum < numTrackers)
    {
        // Convert to VESS coordinates
        positionVec = coordXform.rotatePoint(positionVec);

        tracker[trackerNum]->setPosition(positionVec);
    }
}

// ------------------------------------------------------------------------
// Update the given tracker's orientation with the given vsVector of Euler
// angles
// ------------------------------------------------------------------------
void vsIS600::updateRelativePosition(int trackerNum, vsVector deltaVec)
{
    vsVector currentPosVec;

    if (trackerNum < numTrackers)
    {
        // Get the tracker's current position
        currentPosVec[VS_X] = tracker[trackerNum]->getAxis(VS_X)->getPosition();
        currentPosVec[VS_Y] = tracker[trackerNum]->getAxis(VS_Y)->getPosition();
        currentPosVec[VS_Z] = tracker[trackerNum]->getAxis(VS_Z)->getPosition();

        // Convert deltaVec to VESS coordinates before adding
        deltaVec = coordXform.rotatePoint(deltaVec);

        // Add deltaVec to currentPosition
        currentPosVec.add(deltaVec);

        tracker[trackerNum]->setPosition(currentPosVec);
    }
}

// ------------------------------------------------------------------------
// Update the given tracker's orientation with the given vsVector of Euler
// angles
// ------------------------------------------------------------------------
void vsIS600::updateAngles(int trackerNum, vsVector orientationVec)
{
    vsQuat ornQuat;

    if (trackerNum < numTrackers)
    {
        // Convert to VESS coordinates
        ornQuat.setEulerRotation(VS_EULER_ANGLES_ZYX_R, orientationVec[VS_H],
            orientationVec[VS_P], orientationVec[VS_R]);
        ornQuat = coordXform * ornQuat * coordXform;

        tracker[trackerNum]->setOrientation(ornQuat);
    }
}

// ------------------------------------------------------------------------
// Update the given tracker's orientation with the given vsMatrix
// ------------------------------------------------------------------------
void vsIS600::updateMatrix(int trackerNum, vsMatrix orientationMat)
{
    vsQuat ornQuat;

    if (trackerNum < numTrackers)
    {
        // Convert to VESS coordinates
        ornQuat.setMatrixRotation(orientationMat);
        ornQuat = coordXform * ornQuat * coordXform;

        tracker[trackerNum]->setOrientation(ornQuat);
    }
}

// ------------------------------------------------------------------------
// Update the given tracker's orientation with the given vsQuat
// ------------------------------------------------------------------------
void vsIS600::updateQuat(int trackerNum, vsQuat quat)
{
    vsQuat ornQuat;

    if (trackerNum < numTrackers)
    {
        // Convert to VESS coordinates
        ornQuat = coordXform * quat * coordXform;

        tracker[trackerNum]->setOrientation(ornQuat);
    }
}

// ------------------------------------------------------------------------
// Update the motion tracker data with fresh data from the IS-600
// ------------------------------------------------------------------------
void vsIS600::updateSystem()
{
    unsigned char buf[VS_IS_SIZE_DATA_PACKET];
    int           bytesRead;
    int           result;
    int           errorRetry;
    int           currentStation;
    int           currentTracker;
    int           i,j;
    int           outputItem;
    int           bufIndex;
    short         lsb, msb;
    short         tempShort;
    float         tempFloat;
    vsVector      tempVec;
    vsVector      deltaVec;
    vsMatrix      tempMat;
    vsQuat        tempQuat;

    if (streaming)
    {
        // Read in (outputSize * numTrackers) bytes
        bytesRead = 0;
        errorRetry = 100;

        while ((bytesRead < (outputSize * numTrackers)) && (errorRetry > 0))
        {
            result = port->readPacket(&buf[bytesRead], 1);

            if (result != 0)
            {
                if (bytesRead == 0)
                {
                    // Check to make sure we're starting at the beginning of
                    // a data record
                    if (buf[0] == '0')
                    {
                        bytesRead++;
                    }
                }
                else
                {
                    bytesRead++;
                }
            }
            else
            {
                errorRetry--;
            }
        }
 
        if (errorRetry <= 0)
        {
            printf("vsIS600::updateSystem: "
                "Error reading IS-600 data (%d of %d bytes)\n",
                bytesRead, outputSize * numTrackers);
            port->flushPort();
        }
    }
    else
    {
        // Read the whole packet at once
        bytesRead = port->readPacket(buf, outputSize * numTrackers);

        if ((bytesRead != (outputSize * numTrackers)) || (buf[0] != '0'))
        {
            printf("vsIS-600::updateSystem: "
                "Error reading IS-600 data (%d of %d bytes)\n",
                bytesRead, outputSize * numTrackers);
            port->flushPort();
        }
    }

    if (bytesRead == (outputSize * numTrackers))
    {
        for (i = 0; i < numTrackers; i++)
        {
            currentStation = buf[(i * outputSize) + 1] - '0';
            currentTracker = station[currentStation];

            // Check for valid tracker number
            if ((currentTracker < 0) || 
                (currentTracker > VS_IS_MAX_TRACKERS) ||
                (tracker[currentTracker] == NULL))
            {
                printf("vsIS600::updateSystem: "
                    "Data received for an invalid tracker\n");
                printf("vsIS600::updateSystem: "
                    "   Station Number:  %d   numTrackers:  %d\n",
                    currentStation, numTrackers);
                port->flushPort();
            }
            else
            {
                bufIndex = (i * outputSize) + 3;
                tempShort = 0;
                tempVec.setSize(3);
                tempVec.clear();
                tempMat.setIdentity();
                tempQuat.clear();
                outputItem = 0;
                while (bufIndex < ((i + 1) * outputSize))
                {
                    switch (formatArray[outputItem])
                    {
                        case VS_IS_FORMAT_SPACE:
                            // Not useful for updating tracker data
                            bufIndex++;
                            break;
                        case VS_IS_FORMAT_CRLF:
                            // Not useful for updating tracker data
                            bufIndex += 2;
                            break;
                        case VS_IS_FORMAT_POSITION:
                            for (j = 0; j < 3; j++)
                            {
                                endianSwap((float *)(&buf[bufIndex]),
                                    &tempFloat);
                                tempVec[j] = tempFloat;
                                bufIndex += sizeof(float);
                            }
                            updatePosition(currentTracker, tempVec);
                            break;
                        case VS_IS_FORMAT_REL_POS:
                            for (j = 0; j < 3; j++)
                            {
                                endianSwap((float *)(&buf[bufIndex]),
                                    &tempFloat);
                                tempVec[j] = tempFloat;
                                bufIndex += sizeof(float);
                            }
                            updateRelativePosition(currentTracker, tempVec);
                            break;
                        case VS_IS_FORMAT_ANGLES:
                            for (j = 0; j < 3; j++)
                            {
                                endianSwap((float *)(&buf[bufIndex]), 
                                    &tempFloat);
                                tempVec[j] = tempFloat;
                                bufIndex += sizeof(float);
                            }
                            updateAngles(currentTracker, tempVec);
                            break;
                        case VS_IS_FORMAT_MATRIX:
                            for (j = 0; j < 9; j++)
                            {
                                endianSwap((float *)(&buf[bufIndex]), 
                                    &tempFloat);
                                tempMat[j/3][j%3] = tempFloat;
                                bufIndex += sizeof(float);
                            }
                            updateMatrix(currentTracker, tempMat);
                            break;
                        case VS_IS_FORMAT_QUAT:
                            for (j = 0; j < 4; j++)
                            {
                                endianSwap((float *)&buf[bufIndex], 
                                    &tempFloat);

                                // The IS-600 sends the scalar part first,
                                // but the vsQuat expects it last, so we have
                                // to account for this by shifting the indices
                                tempQuat[(j + 1) % 4] = tempFloat;

                                bufIndex += sizeof(float);
                            }
                            updateQuat(currentTracker, tempQuat);
                            break;
                        case VS_IS_FORMAT_16BIT_POS:
                            for (j = 0; j < 3; j++)
                            {
                                lsb = buf[bufIndex];
                                msb = buf[bufIndex+1];
                                lsb = lsb & 0x7F;
                                lsb = lsb << 1;
                                msb = msb << 8;
                                tempShort = 0x3FFF & ((msb | lsb) >> 1);

                                // Multiply by the scale factor
                                if (outputUnits == VS_IS_UNITS_CENTIMETERS)
                                    tempVec[j] = 
                                        tempShort * VS_IS_SCALE_POS_CM;
                                else
                                    tempVec[j] = 
                                        tempShort * VS_IS_SCALE_POS_INCHES;

                                bufIndex += 2;
                            }
                            updatePosition(currentTracker, tempVec);
                            break;
                        case VS_IS_FORMAT_16BIT_ANGLES:
                            for (j = 0; j < 3; j++)
                            {
                                lsb = buf[bufIndex];
                                msb = buf[bufIndex+1];
                                lsb = lsb & 0x7F;
                                lsb = lsb << 1;
                                msb = msb << 8;
                                tempShort = 0x3FFF & ((msb | lsb) >> 1);

                                // Multiply by the scale factor
                                tempVec[j] = tempShort * VS_IS_SCALE_ANGLES;

                                bufIndex += 2;
                            }
                            updateAngles(currentTracker, tempVec);
                            break;
                        case VS_IS_FORMAT_16BIT_QUAT:
                            for (j = 0; j < 4; j++)
                            {
                                lsb = buf[bufIndex];
                                msb = buf[bufIndex+1];
                                lsb = lsb & 0x7F;
                                lsb = lsb << 1;
                                msb = msb << 8;
                                tempShort = 0x3FFF & ((msb | lsb) >> 1);

                                // Multiply by the scale factor
                                tempQuat[j] = tempShort * VS_IS_SCALE_QUAT;

                                bufIndex += 2;
                            }
                            updateQuat(currentTracker, tempQuat);
                            break;
                    }
                  
                    outputItem++;
                }
            }
        }
    }

    if (!streaming)
        ping();
}

// ------------------------------------------------------------------------
// Spawn a separate (server) process that continuously reads the IS-600
// and updates the vsMotionTraker data
// ------------------------------------------------------------------------
void vsIS600::forkTracking()
{
    key_t  theKey;
    time_t tod;

    // Use a portion of the time of day for the second half of the shared
    // memory key
    tod = time(NULL);
    tod &= 0x0000FFFF;

    theKey = VS_IS_SHM_KEY_BASE | tod;

    serverPID = fork();

    switch(serverPID)
    {
        case -1:
            printf("vsIS600::forkTracking: "
                "fork() failed, continuing in single-process mode\n");
            break;
        case 0:
            sharedData = new vsSharedInputData(theKey, numTrackers, VS_TRUE);
            serverLoop();
            break;
        default:
            sharedData = new vsSharedInputData(theKey, numTrackers, VS_FALSE);
            forked = VS_TRUE;
            printf("vsIS600::forkTracking: Server PID is %d\n", serverPID);
    }
}

// ------------------------------------------------------------------------
// Requests an update packet from the IS-600
// ------------------------------------------------------------------------
void vsIS600::ping()
{
    unsigned char buf;

    buf = VS_IS_CMD_PING;

    port->writePacket(&buf, 1);
}

// ------------------------------------------------------------------------
// Starts continuous data output from the IS-600
// ------------------------------------------------------------------------
void vsIS600::startStream()
{
    unsigned char buf;

    buf = VS_IS_CMD_START_CONTINUOUS;

    port->writePacket(&buf, 1);

    streaming = VS_TRUE;
}

// ------------------------------------------------------------------------
// Stops continuous data output from the IS-600
// ------------------------------------------------------------------------
void vsIS600::stopStream()
{
    unsigned char buf;

    buf = VS_IS_CMD_STOP_CONTINUOUS;

    port->writePacket(&buf, 1);

    streaming = VS_FALSE;
}

// ------------------------------------------------------------------------
// Removes all SoniDiscs from the given station
// ------------------------------------------------------------------------
void vsIS600::clearStation(int stationNum)
{
    unsigned char buf[20];
    int           index;

    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_CONFIGURE;
    buf[2] = VS_IS_CMD_CLEAR_STATION;

    index = 3;
 
    index += sprintf((char *)&buf[index], "%d\r", stationNum);

    port->writePacket(buf, index);
}

// ------------------------------------------------------------------------
// Removes all ReceiverPods from the system
// ------------------------------------------------------------------------
void vsIS600::clearConstellation()
{
    unsigned char buf[4];

    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_CONFIGURE;
    buf[2] = VS_IS_CMD_CLEAR_STATION;
    buf[3] = '\r';

    port->writePacket(buf, 4);
}

// ------------------------------------------------------------------------
// Adds the given InertiaCube to the given station
// ------------------------------------------------------------------------
void vsIS600::addInertiaCube(int stationNum, int cubeNum)
{
    unsigned char buf[20];
    int           index;

    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_CONFIGURE;
    buf[2] = VS_IS_CMD_ADD_ICUBE;

    index = 3;

    index += sprintf((char *)&buf[index], "%d,%d\r", stationNum, cubeNum);

    port->writePacket(buf, index);
}


// ------------------------------------------------------------------------
// Removes the given InertiaCube from the given station
// ------------------------------------------------------------------------
void vsIS600::removeInertiaCube(int stationNum, int cubeNum)
{
    unsigned char buf[20];
    int           index;

    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_CONFIGURE;
    buf[2] = VS_IS_CMD_DEL_ICUBE;

    index = 3;

    index += sprintf((char *)&buf[index], "%d,%d\r", stationNum, cubeNum);

    port->writePacket(buf, index);
}

// ------------------------------------------------------------------------
// Adds the given SoniDisc to the given station with the offset and normal
// provided.
// ------------------------------------------------------------------------
void vsIS600::addSoniDisc(int stationNum, int discNum, vsVector pos, 
                          vsVector normal, int discID)
{
    unsigned char buf[40];
    int           index;

    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_CONFIGURE;
    buf[2] = VS_IS_CMD_ADD_MOBILE_PSE;

    index = 3;

    index += sprintf((char *)&buf[index], "%d,%d,", stationNum, discNum);

    index += sprintf((char *)&buf[index], "%0.4lf,%0.4lf,%0.4lf,", pos[VS_X],
                     pos[VS_Y], pos[VS_Z]);

    index += sprintf((char *)&buf[index], "%0.4lf,%0.4lf,%0.4lf,", 
                     normal[VS_X], normal[VS_Y], normal[VS_Z]);

    index += sprintf((char *)&buf[index], "%d\r", discID);

    port->writePacket(buf, index);
}

// ------------------------------------------------------------------------
// Removes the given SoniDisc from the given station
// ------------------------------------------------------------------------
void vsIS600::removeSoniDisc(int stationNum, int discNum, int discID)
{
    unsigned char buf[40];
    int           index;

    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_CONFIGURE;
    buf[2] = VS_IS_CMD_DEL_MOBILE_PSE;

    index = 3;

    index += sprintf((char *)&buf[index], "%d,%d,%d\r", stationNum, discNum, discID);

    port->writePacket(buf, index);
}

// ------------------------------------------------------------------------
// Adds a ReceiverPod to the system at the given position and orientation
// ------------------------------------------------------------------------
void vsIS600::addReceiverPod(int podNum, vsVector pos, vsVector normal,
                             int podID)
{
    unsigned char buf[40];
    int           index;

    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_CONFIGURE;
    buf[2] = VS_IS_CMD_ADD_FIXED_PSE;

    index = 3;

    index += sprintf((char *)&buf[index], "%d,", podNum);

    index += sprintf((char *)&buf[index], "%0.4lf,%0.4lf, %0.4lf,", pos[VS_X], 
                     pos[VS_Y], pos[VS_Z]);

    index += sprintf((char *)&buf[index], "%0.4lf,%0.4lf, %0.4lf,",
                     normal[VS_X], normal[VS_Y], normal[VS_Z]);

    index += sprintf((char *)&buf[index], "%d\r", podID);

    port->writePacket(buf, index);
}

// ------------------------------------------------------------------------
// Removes the given ReceiverPod from the system
// ------------------------------------------------------------------------
void vsIS600::removeReceiverPod(int podNum, int podID)
{
    unsigned char buf[40];
    int           index;

    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_CONFIGURE;
    buf[2] = VS_IS_CMD_DEL_FIXED_PSE;

    index = 3;

    index += sprintf((char *)&buf[index], "%d,%d\r", podNum, podID);

    port->writePacket(buf, index);
}

// ------------------------------------------------------------------------
// Executes any pending configuration commands
// ------------------------------------------------------------------------
void vsIS600::applyConfig()
{
    unsigned char buf[4];

    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_CONFIGURE;
    buf[2] = VS_IS_CMD_APPLY_CONFIG;
    buf[3] = '\r';

    port->writePacket(buf, 4);
}

// ------------------------------------------------------------------------
// Cancels any pending configuration commands
// ------------------------------------------------------------------------
void vsIS600::cancelConfig()
{
    unsigned char buf[4];

    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_CONFIGURE;
    buf[2] = VS_IS_CMD_CANCEL_CONFIG;
    buf[3] = '\r';

    port->writePacket(buf, 4);
}

// ------------------------------------------------------------------------
// Adjust the alignment frame for the specified station
// ------------------------------------------------------------------------
void vsIS600::setAlignment(int station, vsVector origin, vsVector positiveX, 
                             vsVector positiveY)
{
    unsigned char buf[VS_IS_SIZE_CMD_PACKET];
    int           index;

    // Reset the alignment frame to the identity matrix
    buf[0] = VS_IS_CMD_RESET_ALIGNMENT;
    buf[1] = station + '0';
    
    port->writePacket(buf, 2);

   
    // Set the new alignment frame
    buf[0] = VS_IS_CMD_SET_ALIGNMENT;
    buf[1] = station;

    index = 2;
    index += sprintf((char *)(&buf[index]), ",%0.2lf", origin[VS_X]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", origin[VS_Y]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", origin[VS_Z]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", positiveX[VS_X]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", positiveX[VS_Y]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", positiveX[VS_Z]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", positiveY[VS_X]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", positiveY[VS_Y]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", positiveY[VS_Z]);
    buf[index] = '\r';

    port->writePacket(buf, index + 1);
}

// ------------------------------------------------------------------------
// Reset the alignment frame of the given station to the default (identity 
// matrix).
// ------------------------------------------------------------------------
void vsIS600::resetAlignment(int station)
{
    unsigned char buf[4];

    buf[0] = VS_IS_CMD_RESET_ALIGNMENT;
    buf[1] = station + '0';
    buf[2] = '\r';

    port->writePacket(buf, 3);
}

// ------------------------------------------------------------------------
// Set the genlock to the given mode.
// ------------------------------------------------------------------------
void vsIS600::setGenlock(int syncMode, int rate)
{
    unsigned char buf[20];
    int           index;

    if ((syncMode < 0) || (syncMode > 3))
    {
        printf("vsIS600::setGenlock:  Invalid mode specified\n");
        return;
    }

    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_GENLOCK;
    buf[2] = syncMode + '0';

    index = 3;
    if (rate >= 2)
    {
        index += sprintf((char *)&buf[index], ",%d", rate);
    }

    buf[index] = '\r';

    port->writePacket(buf, index + 1);
}

// ------------------------------------------------------------------------
// Adjusts the genlock phase to the given percentage.
// ------------------------------------------------------------------------
void vsIS600::setGenlockPhase(int phase)
{
    unsigned char buf[20];
    int           index;

    if ((phase < 0) || (phase > 100))
    {
        printf("vsIS600::setGenlockPhase:  Invalid parameter\n");
        return;
    }
 
    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_GENLOCK;
    buf[1] = VS_IS_CMD_GENLOCK_PHASE;
    
    index = 3;
    index += sprintf((char *)&buf[index], "%d", phase);

    buf[index] = '\r';

    port->writePacket(buf, index + 1);
}

// ------------------------------------------------------------------------
// Set the output format to the items specified in the newFormat parameter.
// The newFormatNum parameter specifies the size of the newFormat array.
// ------------------------------------------------------------------------
void vsIS600::setOutputFormat(int newFormat[], int newFormatNum)
{
    unsigned char buf[VS_IS_SIZE_CMD_PACKET];
    int           size;
    int           index;
    int           i;

    printf("vsIS600::setOutputFormat:\n");

    // If the array is too big, clip it 
    if (newFormatNum > VS_IS_MAX_OUTPUT_ITEMS)
        formatNum = VS_IS_MAX_OUTPUT_ITEMS;
    else
        formatNum = newFormatNum;

    // Validate the new array and calculate the new output packet size
    size = 0;
    index = 0;
    for (i = 0; i < formatNum; i++)
    {
        switch (newFormat[i])
        {
            case VS_IS_FORMAT_SPACE:
                formatArray[index] = newFormat[i];
                index++;
                size += 1;
                printf("   Output item %d is a SPACE\n", i);
                break;
            case VS_IS_FORMAT_CRLF:
                formatArray[index] = newFormat[i];
                index++;
                size += 2;
                printf("   Output item %d is a CR/LF\n", i);
                break;
            case VS_IS_FORMAT_POSITION:
                formatArray[index] = newFormat[i];
                index++;
                size += 12;
                printf("   Output item %d is POSITION\n", i);
                break;
            case VS_IS_FORMAT_REL_POS:
                formatArray[index] = newFormat[i];
                index++;
                size += 12;
                printf("   Output item %d is RELATIVE POSITION\n", i);
                break;
            case VS_IS_FORMAT_ANGLES:
                formatArray[index] = newFormat[i];
                index++;
                size += 12;
                printf("   Output item %d is ANGLES\n", i);
                break;
            case VS_IS_FORMAT_MATRIX:
                formatArray[index] = newFormat[i];
                index++;
                size += 36;
                printf("   Ouput item %d is MATRIX\n", i);
                break;
            case VS_IS_FORMAT_QUAT:
                formatArray[index] = newFormat[i];
                index++;
                size += 16;
                printf("   Output item %d is a QUATERNION\n", i);
                break;
            case VS_IS_FORMAT_16BIT_POS:
                formatArray[index] = newFormat[i];
                index++;
                size += 6;
                printf("   Output item %d is 16-BIT POSITION\n", i);
                break;
            case VS_IS_FORMAT_16BIT_ANGLES:
                formatArray[index] = newFormat[i];
                index++;
                size += 6;
                printf("   Output item %d is 16-BIT ANGLES\n", i);
                break;
            case VS_IS_FORMAT_16BIT_QUAT:
                formatArray[index] = newFormat[i];
                index++;
                size += 8;
                printf("   Output item %d is a 16-BIT QUATERNION\n", i);
                break;
            default:
                printf("Ouput item type %d not supported, ignoring\n", 
                    newFormat[i]);
        }
    }

    // Total size:  3-byte header plus the data size
    outputSize = 3 + size;
    printf("   Total output size per tracker is %d bytes\n", outputSize);

    // Construct the new output list command
    buf[0] = VS_IS_CMD_OUTPUT_LIST;

    index = 2;

    for (i = 0; i < formatNum; i++)
    {
        if (formatArray[i] == VS_IS_FORMAT_MATRIX)
        {
            // Request all three directional cosine vectors if MATRIX is
            // selected (see pp. 98-101 of the manual for details)
            index += sprintf((char *)(&buf[index]), ",5,6,7");
        }
        else
        {
            // Request the next item
            index += sprintf((char *)(&buf[index]), ",%d", formatArray[i]);
        }
    }

    buf[index] = '\r';

    // Set each tracker to output the new output list
    for (i = 0; i < VS_IS_MAX_TRACKERS; i++)
    {
        buf[1] = i + 1 + '0';

        port->writePacket(buf, index + 1);
    }

    // Flush the port and ping for a new packet if necessary
    port->flushPort();

    if (!streaming)
        ping();
}

// ------------------------------------------------------------------------
// Set the units for position output (inches or centimeters)
// ------------------------------------------------------------------------
void vsIS600::setUnits(int units)
{
    unsigned char buf;

    if (units == VS_IS_UNITS_CENTIMETERS)
        buf = VS_IS_CMD_UNITS_CM;
    else
        buf = VS_IS_CMD_UNITS_INCHES;

    port->writePacket(&buf, 1);
}

// ------------------------------------------------------------------------
// Return the number of active trackers
// ------------------------------------------------------------------------
int vsIS600::getNumTrackers()
{
    return numTrackers;
}

// ------------------------------------------------------------------------
// Return the specified tracker, if available
// ------------------------------------------------------------------------
vsMotionTracker *vsIS600::getTracker(int index)
{
    if (index < numTrackers)
        return tracker[index];
    else
        return NULL;
}

// ------------------------------------------------------------------------
// Update the motion tracker data, either from hardware or from shared
// memory
// ------------------------------------------------------------------------
void vsIS600::update()
{
    int i;
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
