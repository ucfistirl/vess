#include "vsFastrak.h++"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

// Static class data member to instruct the server process to exit
int vsFastrak::serverDone;

// ------------------------------------------------------------------------
// Constructs a vsFastrak on the specified port with the given number of
// trackers.  If nTrackers is zero, the class attempts to determine the 
// number automatically
// ------------------------------------------------------------------------
vsFastrak::vsFastrak(int portNumber, long baud, int nTrackers)
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

    for (i = 0; i < VS_FT_MAX_TRACKERS; i++)
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
            printf("vsFastrak::vsFastrak: "
                "WARNING -- Only %d trackers found, expecting %d\n",
                numTrackers, nTrackers);
        }

        if ((numTrackers > nTrackers) && (nTrackers > 0))
        {
            printf("vsFastrak::vsFastrak: Configuring %d of %d trackers\n",
                nTrackers, numTrackers);

            numTrackers = nTrackers;
        }

        // Set some default configurations
        setBinaryOutput();

        initOutputFormat();

        streaming = VS_FALSE;

        printf("vsFastrak::vsFastrak: Fastrak running on %s "
            "with %d tracker(s)\n", portDevice, numTrackers);

        ping();
    }
    else
    {
        printf("vsFastrak::vsFastrak: "
            "Unable to open serial port %s", portDevice);
    }

}

// ------------------------------------------------------------------------
// Destructs the vsMotionTrackers and closes the serial port.
// ------------------------------------------------------------------------
vsFastrak::~vsFastrak()
{
    int           i;
    unsigned char buf;

    // Delete the motion trackers
    for (i = 0; i < VS_FT_MAX_TRACKERS; i++)
    {
        if (tracker[i] != NULL)
            delete tracker[i];
    }

    // Terminate the server process
    if (forked)
    {
        printf("vsFastrak::~vsFastrak:  Notifying server process to quit\n");
        kill(serverPID, SIGUSR1);
    }

    // Shut down the FASTRAK
    if ((port != NULL) && (!forked))
    {
        printf("vsFastrak::~vsFastrak:  Shutting down Fastrak\n");

        // Reset the FASTRAK
        buf = VS_FT_CMD_REINIT_SYSTEM;
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
void vsFastrak::serverLoop()
{
    int               i;
    vsVector          posVec;
    vsQuat            ornQuat;
    unsigned char     buf;

    // Set up the signal handler
    signal(SIGUSR1, vsFastrak::quitServer);

    vsFastrak::serverDone = VS_FALSE;

    // Start streaming data
    startStream();

    posVec.setSize(3);
    posVec.clear();
    ornQuat.clear();

    // Constantly update the shared data
    while (!vsFastrak::serverDone)
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
        printf("vsFastrak::serverLoop:  Shutting down Fastrak\n");

        // Reset the FASTRAK
        buf = VS_FT_CMD_REINIT_SYSTEM;
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
void vsFastrak::quitServer(int arg)
{
    vsFastrak::serverDone = VS_TRUE;
}

// ------------------------------------------------------------------------
// Examines the four possible tracker "stations" on the FASTRAK system, and
// constructs a vsMotionTracker for each one that is reported active.
// These trackers are numbered in the order found starting at 0.
// 
// NOTE:  Hereafter, the term "tracker number" or "tracker index" refers
//        to the number given a tracker by this function.  The term 
//        "station number" or "station index" refers to the physical port
//        to which a tracker is plugged in.
// ------------------------------------------------------------------------
void vsFastrak::enumerateTrackers()
{
    unsigned char buf[VS_FT_SIZE_CMD_PACKET];
    int           result;

    // Stop the FASTRAK from streaming (if it is)
    stopStream();
    port->flushPort();

    printf("vsFastrak::enumerateTrackers:\n");

    buf[0] = VS_FT_CMD_STATION_STATE;
    buf[1] = '1';
    buf[2] = '\r';

    port->writePacket(buf, 3);

    result = port->readPacket(buf, 9);

    if (result < 9)
    {
        printf("   Error reading active station state (%d of 9 bytes)\n",
            result);
        port->flushPort();
    }

    buf[9] = 0;

    numTrackers = 0;

    // Report each station's status
    if (buf[3] == '1')
    {
        tracker[numTrackers] = new vsMotionTracker(numTrackers);
        station[1] = numTrackers;
        numTrackers++;
        printf("    Station 1 is active\n");
    }
    else
    {
        station[1] = -1;
        printf("    Station 1 is not active\n");
    }
    if (buf[4] == '1')
    {
        tracker[numTrackers] = new vsMotionTracker(numTrackers);
        station[2] = numTrackers;
        numTrackers++;
        printf("    Station 2 is active\n");
    }
    else
    {
        station[2] = -1;
        printf("    Station 2 is not active\n");
    }
    if (buf[5] == '1')
    {
        tracker[numTrackers] = new vsMotionTracker(numTrackers);
        station[3] = numTrackers;
        numTrackers++;
        printf("    Station 3 is active\n");
    }
    else
    {
        station[3] = -1;
        printf("    Station 3 is not active\n");
    }
    if (buf[6] == '1')
    {
        tracker[numTrackers] = new vsMotionTracker(numTrackers);
        station[4] = numTrackers;
        numTrackers++;
        printf("    Station 4 is active\n");
    }
    else
    {
        station[4] = -1;
        printf("    Station 4 is not active\n");
    }
}

// ------------------------------------------------------------------------
// Initialize the formatArray variable to the current output format at
// station 1.  All stations are then set to this format.
// ------------------------------------------------------------------------
void vsFastrak::initOutputFormat()
{
    unsigned char buf[VS_FT_SIZE_CMD_PACKET];
    int           result;
    char          *token;

    // Stop streaming data and flush the serial port
    stopStream();
    port->flushPort();

    // Get the data format from station 1
    buf[0] = VS_FT_CMD_OUTPUT_LIST;
    buf[1] = '1';
    buf[2] = '\r';

    port->writePacket(buf, 3);

    result = port->readPacket(buf, VS_FT_SIZE_CMD_PACKET);
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
// vice versa)
// ------------------------------------------------------------------------
void vsFastrak::endianSwap(float *inFloat, float *outFloat)
{
    *outFloat = *inFloat;
/*
    ((unsigned char *)outFloat)[0] = ((unsigned char *)inFloat)[3];
    ((unsigned char *)outFloat)[1] = ((unsigned char *)inFloat)[2];
    ((unsigned char *)outFloat)[2] = ((unsigned char *)inFloat)[1];
    ((unsigned char *)outFloat)[3] = ((unsigned char *)inFloat)[0];
*/
}

// ------------------------------------------------------------------------
// Set the FASTRAK to binary output mode
// ------------------------------------------------------------------------
void vsFastrak::setBinaryOutput()
{
    unsigned char buf;

    buf = VS_FT_CMD_BINARY_OUTPUT;

    printf("vsFastrak::setBinaryOutput: Switching to binary output\n");
    port->writePacket(&buf, 1);

    port->flushPort();
}

// ------------------------------------------------------------------------
// Update the given tracker's position with the given vsVector
// ------------------------------------------------------------------------
void vsFastrak::updatePosition(int trackerNum, vsVector positionVec)
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
void vsFastrak::updateRelativePosition(int trackerNum, vsVector deltaVec)
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
void vsFastrak::updateAngles(int trackerNum, vsVector orientationVec)
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
void vsFastrak::updateMatrix(int trackerNum, vsMatrix orientationMat)
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
void vsFastrak::updateQuat(int trackerNum, vsQuat quat)
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
// Requests an update packet from the FASTRAK
// ------------------------------------------------------------------------
void vsFastrak::ping()
{
    unsigned char buf;

    buf = VS_FT_CMD_PING;

    port->writePacket(&buf, 1);
}

// ------------------------------------------------------------------------
// Update the motion tracker data with fresh data from the FASTRAK
// ------------------------------------------------------------------------
void vsFastrak::updateSystem()
{
    unsigned char buf[VS_FT_SIZE_DATA_PACKET];
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
            printf("vsFastrak::updateSystem: "
                "Error reading FASTRAK data (%d of %d bytes)\n",
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
            printf("vsFastrack::updateSystem: "
                "Error reading FASTRAK data (%d of %d bytes)\n",
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
                (currentTracker > VS_FT_MAX_TRACKERS) ||
                (tracker[currentTracker] == NULL))
            {
                printf("vsFastrak::updateSystem: "
                    "Data received for an invalid tracker\n");
                printf("vsFastrak::updateSystem: "
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
                        case VS_FT_FORMAT_SPACE:
                            // Not useful for updating tracker data
                            bufIndex++;
                            break;
                        case VS_FT_FORMAT_CRLF:
                            // Not useful for updating tracker data
                            bufIndex += 2;
                            break;
                        case VS_FT_FORMAT_POSITION:
                            for (j = 0; j < 3; j++)
                            {
                                endianSwap((float *)(&buf[bufIndex]),
                                    &tempFloat);
                                tempVec[j] = tempFloat;
                                bufIndex += sizeof(float);
                            }
                            updatePosition(currentTracker, tempVec);
                            break;
                        case VS_FT_FORMAT_REL_POS:
                            for (j = 0; j < 3; j++)
                            {
                                endianSwap((float *)(&buf[bufIndex]),
                                    &tempFloat);
                                tempVec[j] = tempFloat;
                                bufIndex += sizeof(float);
                            }
                            updateRelativePosition(currentTracker, tempVec);
                            break;
                        case VS_FT_FORMAT_ANGLES:
                            for (j = 0; j < 3; j++)
                            {
                                endianSwap((float *)(&buf[bufIndex]), 
                                    &tempFloat);
                                tempVec[j] = tempFloat;
                                bufIndex += sizeof(float);
                            }
                            updateAngles(currentTracker, tempVec);
                            break;
                        case VS_FT_FORMAT_MATRIX:
                            for (j = 0; j < 9; j++)
                            {
                                endianSwap((float *)(&buf[bufIndex]), 
                                    &tempFloat);
                                tempMat[j/3][j%3] = tempFloat;
                                bufIndex += sizeof(float);
                            }
                            updateMatrix(currentTracker, tempMat);
                            break;
                        case VS_FT_FORMAT_QUAT:
                            for (j = 0; j < 4; j++)
                            {
                                endianSwap((float *)&buf[bufIndex], 
                                    &tempFloat);

                                // The Fastrak sends the scalar part first,
                                // but the vsQuat expects it last, so we have
                                // to account for this by shifting the indices
                                tempQuat[(j + 1) % 4] = tempFloat;

                                bufIndex += sizeof(float);
                            }
                            updateQuat(currentTracker, tempQuat);
                            break;
                        case VS_FT_FORMAT_16BIT_POS:
                            for (j = 0; j < 3; j++)
                            {
                                lsb = buf[bufIndex];
                                msb = buf[bufIndex+1];
                                lsb = lsb & 0x7F;
                                lsb = lsb << 1;
                                msb = msb << 8;
                                tempShort = 0x3FFF & ((msb | lsb) >> 1);

                                // Multiply by the scale factor
                                if (outputUnits == VS_FT_UNITS_CENTIMETERS)
                                    tempVec[j] = 
                                        tempShort * VS_FT_SCALE_POS_CM;
                                else
                                    tempVec[j] = 
                                        tempShort * VS_FT_SCALE_POS_INCHES;

                                bufIndex += 2;
                            }
                            updatePosition(currentTracker, tempVec);
                            break;
                        case VS_FT_FORMAT_16BIT_ANGLES:
                            for (j = 0; j < 3; j++)
                            {
                                lsb = buf[bufIndex];
                                msb = buf[bufIndex+1];
                                lsb = lsb & 0x7F;
                                lsb = lsb << 1;
                                msb = msb << 8;
                                tempShort = 0x3FFF & ((msb | lsb) >> 1);

                                // Multiply by the scale factor
                                tempVec[j] = tempShort * VS_FT_SCALE_ANGLES;

                                bufIndex += 2;
                            }
                            updateAngles(currentTracker, tempVec);
                            break;
                        case VS_FT_FORMAT_16BIT_QUAT:
                            for (j = 0; j < 4; j++)
                            {
                                lsb = buf[bufIndex];
                                msb = buf[bufIndex+1];
                                lsb = lsb & 0x7F;
                                lsb = lsb << 1;
                                msb = msb << 8;
                                tempShort = 0x3FFF & ((msb | lsb) >> 1);

                                // Multiply by the scale factor
                                tempQuat[j] = tempShort * VS_FT_SCALE_QUAT;

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
// Spawn a separate (server) process that continuously reads the FASTRAK
// and updates the vsMotionTraker data
// ------------------------------------------------------------------------
void vsFastrak::forkTracking()
{
    key_t  theKey;
    time_t tod;

    // Use a portion of the time of day for the second half of the shared
    // memory key
    tod = time(NULL);
    tod &= 0x0000FFFF;

    theKey = VS_FT_SHM_KEY_BASE | tod;

    serverPID = fork();

    switch(serverPID)
    {
        case -1:
            printf("vsFastrak::forkTracking: "
                "fork() failed, continuing in single-process mode\n");
            break;
        case 0:
            sharedData = new vsSharedInputData(theKey, numTrackers, VS_TRUE);
            serverLoop();
            break;
        default:
            sharedData = new vsSharedInputData(theKey, numTrackers, VS_FALSE);
            forked = VS_TRUE;
            printf("vsFastrak::forkTracking: Server PID is %d\n", serverPID);
    }
}

// ------------------------------------------------------------------------
// Starts continuous data output from the FASTRAK
// ------------------------------------------------------------------------
void vsFastrak::startStream()
{
    unsigned char buf;

    buf = VS_FT_CMD_START_CONTINUOUS;

    port->writePacket(&buf, 1);

    streaming = VS_TRUE;
}

// ------------------------------------------------------------------------
// Stops continuous data output from the FASTRAK
// ------------------------------------------------------------------------
void vsFastrak::stopStream()
{
    unsigned char buf;

    buf = VS_FT_CMD_STOP_CONTINUOUS;

    port->writePacket(&buf, 1);

    streaming = VS_FALSE;
}

// ------------------------------------------------------------------------
// Adjust the alignment frame for the specified station
// ------------------------------------------------------------------------
void vsFastrak::setAlignment(int station, vsVector origin, vsVector positiveX, 
                             vsVector positiveY)
{
    unsigned char buf[VS_FT_SIZE_CMD_PACKET];
    int           index;

    // Reset the alignment frame to the identity matrix
    buf[0] = VS_FT_CMD_RESET_ALIGNMENT;
    buf[1] = station + '0';
    
    port->writePacket(buf, 2);

   
    // Set the new alignment frame
    buf[0] = VS_FT_CMD_SET_ALIGNMENT;
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
void vsFastrak::resetAlignment(int station)
{
    unsigned char buf[4];

    buf[0] = VS_FT_CMD_RESET_ALIGNMENT;
    buf[1] = station + '0';
    buf[2] = '\r';

    port->writePacket(buf, 3);
}

// ------------------------------------------------------------------------
// Adjust the transmitter mounting frame for the given station to the given 
// orientation.
// ------------------------------------------------------------------------
void vsFastrak::setMountingFrame(int station, vsVector orientation)
{
    unsigned char buf[VS_FT_SIZE_CMD_PACKET];
    int           index;

    buf[0] = VS_FT_CMD_XMTR_MOUNT_FRAME;
    buf[1] = station + '0';

    index = 2;

    index += sprintf((char *)(&buf[index]), ",%0.2lf", orientation[VS_H]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", orientation[VS_P]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", orientation[VS_R]);
    buf[index] = '\r';

    port->writePacket(buf, index + 1);
}

// ------------------------------------------------------------------------
// Set the sync mode to the given sync mode.  Be careful with this command.
// See the WARNING in section 6.5 of the manual.
// ------------------------------------------------------------------------
void vsFastrak::setSyncMode(int syncMode)
{
    unsigned char buf[3];

    buf[0] = VS_FT_CMD_SYNC_MODE;
    buf[1] = syncMode + '0';
    buf[2] = '\r';

    port->writePacket(buf, 3);
}

// ------------------------------------------------------------------------
// Set the active hemisphere of the given station to the one specified by 
// the given vector
// ------------------------------------------------------------------------
void vsFastrak::setActiveHemisphere(int station, vsVector zenithVec)
{
    unsigned char buf[VS_FT_SIZE_CMD_PACKET];
    int           index;

    buf[0] = VS_FT_CMD_HEMISPHERE;
    buf[1] = station + '0';

    index = 2;

    index += sprintf((char *)(&buf[index]), ",%0.2lf", zenithVec[VS_X]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", zenithVec[VS_Y]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", zenithVec[VS_Z]);

    buf[index] = '\r';

    port->writePacket(buf, index + 1);
}

// ------------------------------------------------------------------------
// Set the output format to the items specified in the newFormat parameter.
// The newFormatNum parameter specifies the size of the newFormat array.
// ------------------------------------------------------------------------
void vsFastrak::setOutputFormat(int newFormat[], int newFormatNum)
{
    unsigned char buf[VS_FT_SIZE_CMD_PACKET];
    int           size;
    int           index;
    int           i;

    printf("vsFastrak::setOutputFormat:\n");

    // If the array is too big, clip it 
    if (newFormatNum > VS_FT_MAX_OUTPUT_ITEMS)
        formatNum = VS_FT_MAX_OUTPUT_ITEMS;
    else
        formatNum = newFormatNum;

    // Validate the new array and calculate the new output packet size
    size = 0;
    index = 0;
    for (i = 0; i < formatNum; i++)
    {
        switch (newFormat[i])
        {
            case VS_FT_FORMAT_SPACE:
                formatArray[index] = newFormat[i];
                index++;
                size += 1;
                printf("   Output item %d is a SPACE\n", i);
                break;
            case VS_FT_FORMAT_CRLF:
                formatArray[index] = newFormat[i];
                index++;
                size += 2;
                printf("   Output item %d is a CR/LF\n", i);
                break;
            case VS_FT_FORMAT_POSITION:
                formatArray[index] = newFormat[i];
                index++;
                size += 12;
                printf("   Output item %d is POSITION\n", i);
                break;
            case VS_FT_FORMAT_REL_POS:
                formatArray[index] = newFormat[i];
                index++;
                size += 12;
                printf("   Output item %d is RELATIVE POSITION\n", i);
                break;
            case VS_FT_FORMAT_ANGLES:
                formatArray[index] = newFormat[i];
                index++;
                size += 12;
                printf("   Output item %d is ANGLES\n", i);
                break;
            case VS_FT_FORMAT_MATRIX:
                formatArray[index] = newFormat[i];
                index++;
                size += 36;
                printf("   Ouput item %d is MATRIX\n", i);
                break;
            case VS_FT_FORMAT_QUAT:
                formatArray[index] = newFormat[i];
                index++;
                size += 16;
                printf("   Output item %d is a QUATERNION\n", i);
                break;
            case VS_FT_FORMAT_16BIT_POS:
                formatArray[index] = newFormat[i];
                index++;
                size += 6;
                printf("   Output item %d is 16-BIT POSITION\n", i);
                break;
            case VS_FT_FORMAT_16BIT_ANGLES:
                formatArray[index] = newFormat[i];
                index++;
                size += 6;
                printf("   Output item %d is 16-BIT ANGLES\n", i);
                break;
            case VS_FT_FORMAT_16BIT_QUAT:
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
    buf[0] = VS_FT_CMD_OUTPUT_LIST;

    index = 2;

    for (i = 0; i < formatNum; i++)
    {
        if (formatArray[i] == VS_FT_FORMAT_MATRIX)
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
    for (i = 0; i < VS_FT_MAX_TRACKERS; i++)
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
void vsFastrak::setUnits(int units)
{
    unsigned char buf;

    if (units == VS_FT_UNITS_CENTIMETERS)
        buf = VS_FT_CMD_UNITS_CM;
    else
        buf = VS_FT_CMD_UNITS_INCHES;

    port->writePacket(&buf, 1);
}

// ------------------------------------------------------------------------
// Return the number of active trackers
// ------------------------------------------------------------------------
int vsFastrak::getNumTrackers()
{
    return numTrackers;
}

// ------------------------------------------------------------------------
// Return the specified tracker, if available
// ------------------------------------------------------------------------
vsMotionTracker *vsFastrak::getTracker(int index)
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
void vsFastrak::update()
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
