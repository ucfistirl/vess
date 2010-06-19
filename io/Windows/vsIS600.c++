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
//    VESS Module:  vsIS600.c++
//
//    Description:  Class supporting the InterSense IS-600 Mark 2 motion
//                  tracking system. This class supports a single IS-600
//                  running over an RS-232 interface with up to
//                  VS_IS_MAX_TRACKERS receivers.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsIS600.h++"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    atQuat quat1, quat2;

    // Determine the platform-dependent serial device
    // name
    sprintf(portDevice, "COM%d", portNumber);

    // Initialize variables
    port = NULL;
    numTrackers = 0;
    forked = false;
    serverThreadID = 0;
    streaming = false;

    // Set up a coordinate conversion quaternion that will convert
    // Polhemus (Intersense) coordinates to VESS coordinates
    quat1.setAxisAngleRotation(0, 0, 1, 90);
    quat2.setAxisAngleRotation(0, 1, 0, 180);
    coordXform = quat2 * quat1;

    // Initialize the motion tracker array
    for (i = 0; i < VS_IS_MAX_TRACKERS; i++)
    {
        tracker[i] = NULL;
    }

    // Open serial port at the given baud rate
    port = new vsSerialPort(portDevice, baud, 8, 'N', 1);

    // Make sure the serial port opened properly
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

        // Print a status message if we're using fewer trackers than available
        if ((numTrackers > nTrackers) && (nTrackers > 0))
        {
            printf("vsIS600::vsIS600: Configuring %d of %d trackers\n",
                nTrackers, numTrackers);

            numTrackers = nTrackers;
        }

        // Check the endianness of the host machine
        bigEndian = isBigEndian();

        // Use binary output mode
        setBinaryOutput();

        // Initialize the output format
        initOutputFormat();

        // Print status
        printf("vsIS600::vsIS600: IS-600 running on %s "
            "with %d tracker(s)\n", portDevice, numTrackers);

        // Request the first data packet
        ping();
    }
    else
    {
        printf("vsIS600::vsIS600: "
            "Unable to open serial port %s", portDevice);
    }

}

// ------------------------------------------------------------------------
// Constructs a vsIS600 on the specified port with the given number of
// trackers.  If nTrackers is zero, the class attempts to determine the 
// number automatically
// ------------------------------------------------------------------------
vsIS600::vsIS600(char *portDev, long baud, int nTrackers)
         : vsTrackingSystem()
{
    int    i;
    atQuat quat1, quat2;

    // Initialize variables
    port = NULL;
    numTrackers = 0;
    forked = false;
    serverThreadID = 0;
    streaming = false;

    // Set up a coordinate conversion quaternion that will convert
    // Polhemus (Intersense) coordinates to VESS coordinates
    quat1.setAxisAngleRotation(0, 0, 1, 90);
    quat2.setAxisAngleRotation(0, 1, 0, 180);
    coordXform = quat2 * quat1;

    // Initialize the motion tracker array
    for (i = 0; i < VS_IS_MAX_TRACKERS; i++)
    {
        tracker[i] = NULL;
    }

    // Open serial port at the given baud rate
    port = new vsSerialPort(portDev, baud, 8, 'N', 1);

    // Make sure the serial port opened properly
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

        // Print a status message if we're using fewer trackers than available
        if ((numTrackers > nTrackers) && (nTrackers > 0))
        {
            printf("vsIS600::vsIS600: Configuring %d of %d trackers\n",
                nTrackers, numTrackers);

            numTrackers = nTrackers;
        }

        // Check the endianness of the host machine
        bigEndian = isBigEndian();

        // Use binary output mode
        setBinaryOutput();

        // Initialize the output format
        initOutputFormat();

        // Print status
        printf("vsIS600::vsIS600: IS-600 running on %s "
            "with %d tracker(s)\n", portDev, numTrackers);

        // Request the first data packet
        ping();
    }
    else
    {
        printf("vsIS600::vsIS600: "
            "Unable to open serial port %s", portDev);
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

    // Terminate the server thread if we're forked
    if (forked)
    {
        printf("vsIS600::~vsIS600:  Notifying server thread to quit\n");
        serverDone = true;
    }

    // Shut down the IS-600 if we're not forked (the server thread will
    // handle it if we are)
    if ((port != NULL) && (!forked))
    {
        printf("vsIS600::~vsIS600:  Shutting down IS-600\n");

        // Reset the IS-600
        buf = VS_IS_CMD_STOP_CONTINUOUS;
        port->writePacket(&buf, 1);
        Sleep(1000);

        // Flush and close the serial port
        port->flushPort();
        delete port;
    }
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsIS600::getClassName()
{
    return "vsIS600";
}

// ------------------------------------------------------------------------
// Loop for the server portion of this class when operating in multiple
// threads
// ------------------------------------------------------------------------
DWORD WINAPI vsIS600::serverLoop(void *parameter)
{
    vsIS600           *instance;
    int               i;
    unsigned char     buf;

    // Get the vsIS600 instance from the parameter
    instance = (vsIS600 *)parameter;
    
    // Initialize the done flag
    instance->serverDone = false;

    // Start streaming data
    instance->startStream();

    // Constantly update the shared data
    while (!instance->serverDone)
    {
        // Update the hardware
        instance->updateSystem();
    }
    
    // Delete the private tracker objects
    for (i = 0; i < instance->numTrackers; i++)
    {
        delete instance->privateTracker[i];
    }

    // Delete the critical section object
    DeleteCriticalSection(&(instance->criticalSection));

    // Clean up
    if (instance->port != NULL)
    {
        printf("vsIS600::serverLoop:  Shutting down IS-600\n");

        // Reset the IS-600
        buf = VS_IS_CMD_STOP_CONTINUOUS;
        instance->port->writePacket(&buf, 1);
        Sleep(1000);

        // Flush and close the serial port
        instance->port->flushPort();
        delete instance->port;
    }

    // Return from the thread (this calls ExitThread() implicitly)
    return 0;
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

    // Print status as we go
    printf("vsIS600::enumerateTrackers:\n");

    // Request the station state information
    buf[0] = VS_IS_CMD_STATION_STATE;
    buf[1] = '*';
    buf[2] = '\r';
    port->writePacket(buf, 3);

    // Read the result.  See the IS-600 manual for the data format for the
    // response to an active station state command.
    result = port->readPacket(buf, 37);

    // Make sure we got a valid response.  Should be 37 bytes.  A 3-byte
    // header, one byte per station (for 32 stations), and a CR/LF pair
    // at the end.
    if (result < 37)
    {
        printf("   Error reading active station state (%d of 37 bytes)\n",
            result);
        port->flushPort();
    }

    // Terminate the string in the result packet
    buf[37] = 0;

    // Initialize the number of trackers to zero
    numTrackers = 0;

    // Read the packet string to check if each station is active.
    // The active station state begins at byte 3 and consists of one
    // byte per station for 32 stations.
    for (i = 3; i < 35; i++)
    {
        // Report each active station and configure a motion tracker for it
        if (buf[i] == '1')
        {
            // Configure a motion tracker for this station
            tracker[numTrackers] = new vsMotionTracker(numTrackers);

            // Map the station number to the current tracker number
            station[i-2] = numTrackers;

            // Increment the number of trackers
            numTrackers++;

            // Report the station is active
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

    // Read the result and terminate the string in the result packet
    result = port->readPacket(buf, VS_IS_SIZE_CMD_PACKET);
    buf[result] = 0;

    // Initialize the number of data format items to zero
    formatNum = 0;

    // Parse the format string into formatArray[]
    token = strtok((char *)(&buf[4]), " \n");
    while (token != NULL)
    {
        // Since we only support items 5, 6, and 7 (directional cosines)
        // together as a group, we need to throw out any stray 6's and 7's
        if ((atoi(token) != 6) && (atoi(token) != 7))
        {
            // Add the item to the format array
            formatArray[formatNum] = atoi(token);

            // Increment the number of items in the format array
            formatNum++;

            // Get the next token
            token = strtok(NULL, " \n");
        }
    }

    // Set all stations to this format
    setOutputFormat(formatArray, formatNum);
}

// ------------------------------------------------------------------------
// From Harbinson&Steele.  Determines whether this machine is big- or
// little-endian.
// ------------------------------------------------------------------------
bool vsIS600::isBigEndian()
{
    // Union of a long and four bytes used to perform the test
    union
    {
        long          l;
        unsigned char c[sizeof (long)];
    } u;

    // Set the long to 1
    u.l = 1;

    // If the last byte of the character array corresponding to the long
    // is 1, we're big-endian
    return (u.c[sizeof (long) - 1] == 1);
}

// ------------------------------------------------------------------------
// Convert a little-endian 32-bit floating point number to big-endian (or 
// vice versa).  The IS-600 returns its numbers in little-endian format, so
// on little-endian systems, this function simply returns the number 
// unchanged.
// ------------------------------------------------------------------------
void vsIS600::endianSwap(float *inFloat, float *outFloat)
{
    if (bigEndian)
    {
        // Perform the swap
        ((unsigned char *)outFloat)[0] = ((unsigned char *)inFloat)[3];
        ((unsigned char *)outFloat)[1] = ((unsigned char *)inFloat)[2];
        ((unsigned char *)outFloat)[2] = ((unsigned char *)inFloat)[1];
        ((unsigned char *)outFloat)[3] = ((unsigned char *)inFloat)[0];
    }
    else
    {
        // Just copy the input to the output
        *outFloat = *inFloat;
    }
}

// ------------------------------------------------------------------------
// Set the IS-600 to binary output mode
// ------------------------------------------------------------------------
void vsIS600::setBinaryOutput()
{
    unsigned char buf;


    // Send the binary output format command
    buf = VS_IS_CMD_BINARY_OUTPUT;
    printf("vsIS600::setBinaryOutput: Switching to binary output\n");
    port->writePacket(&buf, 1);
    port->flushPort();
}

// ------------------------------------------------------------------------
// Update the given tracker's position with the given atVector
// ------------------------------------------------------------------------
void vsIS600::updatePosition(int trackerNum, atVector positionVec)
{
    // Validate the tracker number
    if (trackerNum < numTrackers)
    {
        // Convert to VESS coordinates
        positionVec = coordXform.rotatePoint(positionVec);

        // Update the tracker.  Use the private copy of the tracker
        // data and ensure mutual exclusion with the critical section
        // object if we're multithreading.
        if (forked)
        {
            EnterCriticalSection(&criticalSection);
            privateTracker[trackerNum]->setPosition(positionVec);
            LeaveCriticalSection(&criticalSection);
        }
        else
        {
            // Not multithreading, just update the public tracker object
            tracker[trackerNum]->setPosition(positionVec);
        }
    }
}

// ------------------------------------------------------------------------
// Update the given tracker's orientation with the given atVector of Euler
// angles
// ------------------------------------------------------------------------
void vsIS600::updateRelativePosition(int trackerNum, atVector deltaVec)
{
    atVector currentPosVec;

    // Validate the tracker number
    if (trackerNum < numTrackers)
    {
        // Get the tracker's current position
        currentPosVec[AT_X] = tracker[trackerNum]->getAxis(AT_X)->getPosition();
        currentPosVec[AT_Y] = tracker[trackerNum]->getAxis(AT_Y)->getPosition();
        currentPosVec[AT_Z] = tracker[trackerNum]->getAxis(AT_Z)->getPosition();

        // Convert deltaVec to VESS coordinates before adding
        deltaVec = coordXform.rotatePoint(deltaVec);

        // Add deltaVec to currentPosition
        currentPosVec.add(deltaVec);

        // Update the tracker.  Use the private copy of the tracker
        // data and ensure mutual exclusion with the critical section
        // object if we're multithreading.
        if (forked)
        {
            EnterCriticalSection(&criticalSection);
            privateTracker[trackerNum]->setPosition(currentPosVec);
            LeaveCriticalSection(&criticalSection);
        }
        else
        {
            // Not multithreading, just update the public tracker object
            tracker[trackerNum]->setPosition(currentPosVec);
        }
    }
}

// ------------------------------------------------------------------------
// Update the given tracker's orientation with the given atVector of Euler
// angles
// ------------------------------------------------------------------------
void vsIS600::updateAngles(int trackerNum, atVector orientationVec)
{
    atQuat ornQuat;

    // Validate the tracker number
    if (trackerNum < numTrackers)
    {
        // Convert to VESS coordinates
        ornQuat.setEulerRotation(AT_EULER_ANGLES_ZYX_R, orientationVec[AT_H],
            orientationVec[AT_P], orientationVec[AT_R]);
        ornQuat = coordXform * ornQuat * coordXform;

        // Update the tracker.  Use the private copy of the tracker
        // data and ensure mutual exclusion with the critical section
        // object if we're multithreading.
        if (forked)
        {
            EnterCriticalSection(&criticalSection);
            privateTracker[trackerNum]->setOrientation(ornQuat);
            LeaveCriticalSection(&criticalSection);
        }
        else
        {
            // Not multithreading, just update the public tracker object
            tracker[trackerNum]->setOrientation(ornQuat);
        }
    }
}

// ------------------------------------------------------------------------
// Update the given tracker's orientation with the given atMatrix
// ------------------------------------------------------------------------
void vsIS600::updateMatrix(int trackerNum, atMatrix orientationMat)
{
    atQuat ornQuat;

    // Validate the tracker number
    if (trackerNum < numTrackers)
    {
        // Convert to VESS coordinates
        ornQuat.setMatrixRotation(orientationMat);
        ornQuat = coordXform * ornQuat * coordXform;

        // Update the tracker.  Use the private copy of the tracker
        // data and ensure mutual exclusion with the critical section
        // object if we're multithreading.
        if (forked)
        {
            EnterCriticalSection(&criticalSection);
            privateTracker[trackerNum]->setOrientation(ornQuat);
            LeaveCriticalSection(&criticalSection);
        }
        else
        {
            // Not multithreading, just update the public tracker object
            tracker[trackerNum]->setOrientation(ornQuat);
        }
    }
}

// ------------------------------------------------------------------------
// Update the given tracker's orientation with the given atQuat
// ------------------------------------------------------------------------
void vsIS600::updateQuat(int trackerNum, atQuat quat)
{
    atQuat ornQuat;

    // Validate the tracker number
    if (trackerNum < numTrackers)
    {
        // Convert to VESS coordinates
        ornQuat = coordXform * quat * coordXform;

        // Update the tracker.  Use the private copy of the tracker
        // data and ensure mutual exclusion with the critical section
        // object if we're multithreading.
        if (forked)
        {
            EnterCriticalSection(&criticalSection);
            privateTracker[trackerNum]->setOrientation(ornQuat);
            LeaveCriticalSection(&criticalSection);
        }
        else
        {
            // Not multithreading, just update the public tracker object
            tracker[trackerNum]->setOrientation(ornQuat);
        }
    }
}

// ------------------------------------------------------------------------
// Requests an update packet from the IS-600
// ------------------------------------------------------------------------
void vsIS600::ping()
{
    unsigned char buf;

    // Send a ping command to the IS-600
    buf = VS_IS_CMD_PING;
    port->writePacket(&buf, 1);
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
    atVector      tempVec;
    atVector      deltaVec;
    atMatrix      tempMat;
    atQuat        tempQuat;

    // Check to see if the IS-600 is streaming
    if (streaming)
    {
        // Initialize the counters
        bytesRead = 0;
        errorRetry = 100;

        // Read in (outputSize * numTrackers) bytes
        while ((bytesRead < (outputSize * numTrackers)) && (errorRetry > 0))
        {
            // Read one byte
            result = port->readPacket(&buf[bytesRead], 1);

            // Check to see if we read anything
            if (result != 0)
            {
                // If we're reading the first byte...
                if (bytesRead == 0)
                {
                    // Check to make sure we're starting at the beginning of
                    // a data record (first byte = '0'), otherwise ignore
                    // this byte
                    if (buf[0] == '0')
                    {
                        // Increment the number of bytes read
                        bytesRead++;
                    }
                }
                else
                {
                    // Increment the number of bytes read
                    bytesRead++;
                }
            }
            else
            {
                // Failed to read, decrement retry count
                errorRetry--;
            }
        }
 
        // Print an error and flush the port if we failed to read the
        // tracker data
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
        // IS-600 is not in streaming mode, read the whole packet at once
        bytesRead = port->readPacket(buf, outputSize * numTrackers);

        // Check to see that we read the correct amount of data, and that
        // the first byte is the beginning of a data record
        if ((bytesRead != (outputSize * numTrackers)) || (buf[0] != '0'))
        {
            // Error happened, print a message and flush the port
            printf("vsIS-600::updateSystem: "
                "Error reading IS-600 data (%d of %d bytes)\n",
                bytesRead, outputSize * numTrackers);
            port->flushPort();
        }
    }

    // If we managed to read the correct amount of data, process the data
    if (bytesRead == (outputSize * numTrackers))
    {
        // Process each tracker's data
        for (i = 0; i < numTrackers; i++)
        {
            // Compute the current station number from the data stream
            currentStation = buf[(i * outputSize) + 1] - '0';

            // Get the tracker number from the station number
            currentTracker = station[currentStation];

            // Check for valid tracker number and motion tracker object
            if ((currentTracker < 0) || 
                (currentTracker > VS_IS_MAX_TRACKERS) ||
                (tracker[currentTracker] == NULL))
            {
                // Tracker is invalid, print an error and flush the serial 
                // port
                printf("vsIS600::updateSystem: "
                    "Data received for an invalid tracker\n");
                printf("vsIS600::updateSystem: "
                    "   Station Number:  %d   numTrackers:  %d\n",
                    currentStation, numTrackers);
                port->flushPort();
            }
            else
            {
                // Compute the index of this tracker's data in the data
                // buffer (add three bytes to account for the data packet's
                // header)
                bufIndex = (i * outputSize) + 3;

                // Initialize temporary variables
                tempShort = 0;
                tempVec.setSize(3);
                tempVec.clear();
                tempMat.setIdentity();
                tempQuat.clear();

                // Initialize the output item counter to zero
                outputItem = 0;

                // Process each data item according to the data format
                // array.  Stop when we've processed the correct amount of 
                // data
                while (bufIndex < ((i + 1) * outputSize))
                {
                    // Check the data type for the current output item
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
                            // Extract the position elements and form a
                            // atVector
                            for (j = 0; j < 3; j++)
                            {
                                endianSwap((float *)(&buf[bufIndex]),
                                    &tempFloat);
                                tempVec[j] = tempFloat;
                                bufIndex += sizeof(float);
                            }

                            // Update the position
                            updatePosition(currentTracker, tempVec);
                            break;

                        case VS_IS_FORMAT_REL_POS:
                            // Extract the position elements and form a
                            // atVector
                            for (j = 0; j < 3; j++)
                            {
                                endianSwap((float *)(&buf[bufIndex]),
                                    &tempFloat);
                                tempVec[j] = tempFloat;
                                bufIndex += sizeof(float);
                            }

                            // Update the position
                            updateRelativePosition(currentTracker, tempVec);
                            break;

                        case VS_IS_FORMAT_ANGLES:
                            // Extract the Euler angles and form a atVector
                            for (j = 0; j < 3; j++)
                            {
                                endianSwap((float *)(&buf[bufIndex]), 
                                    &tempFloat);
                                tempVec[j] = tempFloat;
                                bufIndex += sizeof(float);
                            }

                            // Update the orientation
                            updateAngles(currentTracker, tempVec);
                            break;

                        case VS_IS_FORMAT_MATRIX:
                            // Extract the matrix elements and form a
                            // atMatrix
                            for (j = 0; j < 9; j++)
                            {
                                endianSwap((float *)(&buf[bufIndex]), 
                                    &tempFloat);
                                tempMat[j/3][j%3] = tempFloat;
                                bufIndex += sizeof(float);
                            }

                            // Update the orientation
                            updateMatrix(currentTracker, tempMat);
                            break;

                        case VS_IS_FORMAT_QUAT:
                            // Extract the quaternion elements and form a
                            // atQuat
                            for (j = 0; j < 4; j++)
                            {
                                endianSwap((float *)&buf[bufIndex], 
                                    &tempFloat);

                                // The IS-600 sends the scalar part first,
                                // but the atQuat expects it last, so we have
                                // to account for this by shifting the indices
                                tempQuat[(j + 1) % 4] = tempFloat;

                                bufIndex += sizeof(float);
                            }

                            // Update the orientation
                            updateQuat(currentTracker, tempQuat);
                            break;

                        case VS_IS_FORMAT_16BIT_POS:
                            // Extract the position elements and form a
                            // atVector.  The method for extracting the
                            // 16-bit data is described in the IS-600
                            // manual.
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

                            // Update the position
                            updatePosition(currentTracker, tempVec);
                            break;

                        case VS_IS_FORMAT_16BIT_ANGLES:
                            // Extract the Euler angles and form a
                            // atVector.  The method for extracting the
                            // 16-bit data is described in the IS-600
                            // manual.
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

                            // Update the orientation
                            updateAngles(currentTracker, tempVec);
                            break;

                        case VS_IS_FORMAT_16BIT_QUAT:
                            // Extract the quaternion elements and form a
                            // atQuat.  The method for extracting the
                            // 16-bit data is described in the IS-600
                            // manual.
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

                            // Update the orientation
                            updateQuat(currentTracker, tempQuat);
                            break;
                    }
                  
                    // Go to the next output item
                    outputItem++;
                }
            }
        }
    }

    // If we're not streaming, request the next data packet
    if (!streaming)
        ping();
}

// ------------------------------------------------------------------------
// Spawn a separate (server) thread that continuously reads the IS-600
// and updates the vsMotionTraker data
// ------------------------------------------------------------------------
void vsIS600::forkTracking()
{
    int i;
    
    // Initialize the critical section for thread synchronization
    InitializeCriticalSection(&criticalSection);
    
    // Create the private tracker objects.  The data in these objects 
    // will be copied to the publically-accessible tracker objects in
    // a thread-safe manner when the application calls update().
    for (i = 0; i < numTrackers; i++)
    {
        privateTracker[i] = new vsMotionTracker(i);
    }
    
    // Create the server thread.  Pass this object to the thread, so we
    // can access this instance's variables from the static method.
    serverThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)serverLoop,
        this, 0, &serverThreadID);
    
    // Print the thread ID
    printf("vsIS600::forkTracking:\n");
    printf("    Server Thread ID is %d\n", serverThreadID);
    
    // Set the forked flag to indicate we've started running multithreaded
    forked = true;
}

// ------------------------------------------------------------------------
// Starts continuous data output from the IS-600
// ------------------------------------------------------------------------
void vsIS600::startStream()
{
    unsigned char buf;

    // Send the command to start continuous data streaming
    buf = VS_IS_CMD_START_CONTINUOUS;
    port->writePacket(&buf, 1);

    // Set the streaming flag so we know that the IS-600 is now streaming 
    // data
    streaming = true;
}

// ------------------------------------------------------------------------
// Stops continuous data output from the IS-600
// ------------------------------------------------------------------------
void vsIS600::stopStream()
{
    unsigned char buf;

    // Send the command to start continuous data streaming
    buf = VS_IS_CMD_STOP_CONTINUOUS;
    port->writePacket(&buf, 1);

    // Clear the streaming flag so we know that the IS-600 is not streaming 
    // data
    streaming = false;
}

// ------------------------------------------------------------------------
// Removes all SoniDiscs from the given station
// ------------------------------------------------------------------------
void vsIS600::clearStation(int stationNum)
{
    unsigned char buf[20];
    int           index;

    // Set up the clear station packet
    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_CONFIGURE;
    buf[2] = VS_IS_CMD_CLEAR_STATION;
 
    // Append the station number
    index = 3;
    index += sprintf((char *)&buf[index], "%d\r", stationNum);

    // Send the packet
    port->writePacket(buf, index);
}

// ------------------------------------------------------------------------
// Removes all ReceiverPods from the system
// ------------------------------------------------------------------------
void vsIS600::clearConstellation()
{
    unsigned char buf[4];

    // Set up the clear constellation command
    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_CONFIGURE;
    buf[2] = VS_IS_CMD_CLEAR_CONST;
    buf[3] = '\r';

    // Send the command
    port->writePacket(buf, 4);
}

// ------------------------------------------------------------------------
// Adds the given InertiaCube to the given station
// ------------------------------------------------------------------------
void vsIS600::addInertiaCube(int stationNum, int cubeNum)
{
    unsigned char buf[20];
    int           index;

    // Set up the add InertiaCube packet
    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_CONFIGURE;
    buf[2] = VS_IS_CMD_ADD_ICUBE;

    // Append the station number
    index = 3;
    index += sprintf((char *)&buf[index], "%d,%d\r", stationNum, cubeNum);

    // Send the packet
    port->writePacket(buf, index);
}


// ------------------------------------------------------------------------
// Removes the given InertiaCube from the given station
// ------------------------------------------------------------------------
void vsIS600::removeInertiaCube(int stationNum, int cubeNum)
{
    unsigned char buf[20];
    int           index;

    // Set up the remove InertiaCube packet
    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_CONFIGURE;
    buf[2] = VS_IS_CMD_DEL_ICUBE;

    // Append the station number
    index = 3;
    index += sprintf((char *)&buf[index], "%d,%d\r", stationNum, cubeNum);

    port->writePacket(buf, index);
}

// ------------------------------------------------------------------------
// Adds the given SoniDisc to the given station with the offset and normal
// provided.
// ------------------------------------------------------------------------
void vsIS600::addSoniDisc(int stationNum, int discNum, atVector pos, 
                          atVector normal, int discID)
{
    unsigned char buf[40];
    int           index;

    // Set up the add SoniDisc packet
    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_CONFIGURE;
    buf[2] = VS_IS_CMD_ADD_MOBILE_PSE;

    // Append the station number and disc number
    index = 3;
    index += sprintf((char *)&buf[index], "%d,%d,", stationNum, discNum);

    // Append the SoniDisc parameters
    index += sprintf((char *)&buf[index], "%0.4lf,%0.4lf,%0.4lf,", pos[AT_X],
                     pos[AT_Y], pos[AT_Z]);
    index += sprintf((char *)&buf[index], "%0.4lf,%0.4lf,%0.4lf,", 
                     normal[AT_X], normal[AT_Y], normal[AT_Z]);
    index += sprintf((char *)&buf[index], "%d\r", discID);

    // Send the packet
    port->writePacket(buf, index);
}

// ------------------------------------------------------------------------
// Removes the given SoniDisc from the given station
// ------------------------------------------------------------------------
void vsIS600::removeSoniDisc(int stationNum, int discNum, int discID)
{
    unsigned char buf[40];
    int           index;

    // Set up the remove SoniDisc packet
    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_CONFIGURE;
    buf[2] = VS_IS_CMD_DEL_MOBILE_PSE;

    // Append the station number and disc parameters
    index = 3;
    index += sprintf((char *)&buf[index], "%d,%d,%d\r", stationNum, discNum, 
        discID);

    // Send the packet
    port->writePacket(buf, index);
}

// ------------------------------------------------------------------------
// Adds a ReceiverPod to the system at the given position and orientation
// ------------------------------------------------------------------------
void vsIS600::addReceiverPod(int podNum, atVector pos, atVector normal,
                             int podID)
{
    unsigned char buf[40];
    int           index;

    // Set up the add ReceiverPod packet
    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_CONFIGURE;
    buf[2] = VS_IS_CMD_ADD_FIXED_PSE;

    // Append the pod number
    index = 3;
    index += sprintf((char *)&buf[index], "%d,", podNum);

    // Append the pod parameters
    index += sprintf((char *)&buf[index], "%0.4lf,%0.4lf, %0.4lf,", pos[AT_X], 
        pos[AT_Y], pos[AT_Z]);
    index += sprintf((char *)&buf[index], "%0.4lf,%0.4lf, %0.4lf,",
        normal[AT_X], normal[AT_Y], normal[AT_Z]);
    index += sprintf((char *)&buf[index], "%d\r", podID);

    // Send the packet
    port->writePacket(buf, index);
}

// ------------------------------------------------------------------------
// Removes the given ReceiverPod from the system
// ------------------------------------------------------------------------
void vsIS600::removeReceiverPod(int podNum, int podID)
{
    unsigned char buf[40];
    int           index;

    // Set up the remove ReceiverPod packet
    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_CONFIGURE;
    buf[2] = VS_IS_CMD_DEL_FIXED_PSE;

    // Append the pod number and ID
    index = 3;
    index += sprintf((char *)&buf[index], "%d,%d\r", podNum, podID);

    // Send the packet
    port->writePacket(buf, index);
}

// ------------------------------------------------------------------------
// Executes any pending configuration commands
// ------------------------------------------------------------------------
void vsIS600::applyConfig()
{
    unsigned char buf[4];

    // Set up the apply configurantion command
    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_CONFIGURE;
    buf[2] = VS_IS_CMD_APPLY_CONFIG;
    buf[3] = '\r';

    // Send the command
    port->writePacket(buf, 4);
}

// ------------------------------------------------------------------------
// Cancels any pending configuration commands
// ------------------------------------------------------------------------
void vsIS600::cancelConfig()
{
    unsigned char buf[4];

    // Set up the cancel configuration command
    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_CONFIGURE;
    buf[2] = VS_IS_CMD_CANCEL_CONFIG;
    buf[3] = '\r';

    // Send the command
    port->writePacket(buf, 4);
}

// ------------------------------------------------------------------------
// Adjust the alignment frame for the specified station
// ------------------------------------------------------------------------
void vsIS600::setAlignment(int station, atVector origin, atVector positiveX, 
                             atVector positiveY)
{
    unsigned char buf[VS_IS_SIZE_CMD_PACKET];
    int           index;

    // Reset the alignment frame to the identity matrix.  This is necessary
    // because the IS-600  expects an alteration to the current alignment
    // frame instead of a replacement for it.  Resetting the alignment
    // frame to identity allows us to specify a new one.
    buf[0] = VS_IS_CMD_RESET_ALIGNMENT;
    buf[1] = station + '0';
    port->writePacket(buf, 2);

    // Construct the new alignment frame packet
    buf[0] = VS_IS_CMD_SET_ALIGNMENT;
    buf[1] = station;

    // Append the new alignment frame
    index = 2;
    index += sprintf((char *)(&buf[index]), ",%0.2lf", origin[AT_X]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", origin[AT_Y]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", origin[AT_Z]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", positiveX[AT_X]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", positiveX[AT_Y]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", positiveX[AT_Z]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", positiveY[AT_X]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", positiveY[AT_Y]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", positiveY[AT_Z]);
    buf[index] = '\r';

    // Send the packet
    port->writePacket(buf, index + 1);
}

// ------------------------------------------------------------------------
// Reset the alignment frame of the given station to the default (identity 
// matrix).
// ------------------------------------------------------------------------
void vsIS600::resetAlignment(int station)
{
    unsigned char buf[4];

    // Set up the reset alignment command
    buf[0] = VS_IS_CMD_RESET_ALIGNMENT;
    buf[1] = station + '0';
    buf[2] = '\r';

    // Send the command
    port->writePacket(buf, 3);
}

// ------------------------------------------------------------------------
// Set the genlock to the given mode.
// ------------------------------------------------------------------------
void vsIS600::setGenlock(int syncMode, int rate)
{
    unsigned char buf[20];
    int           index;

    // Validate the syncMode parameter
    if ((syncMode < 0) || (syncMode > 3))
    {
        printf("vsIS600::setGenlock:  Invalid mode specified\n");
        return;
    }

    // Set up the genlock packet
    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_GENLOCK;
    buf[2] = syncMode + '0';

    // Append the rate parameter, if the mode requires it
    index = 3;
    if (syncMode >= 2)
    {
        index += sprintf((char *)&buf[index], ",%d", rate);
    }

    // Terminate the packet
    buf[index] = '\r';

    // Send the packet
    port->writePacket(buf, index + 1);
}

// ------------------------------------------------------------------------
// Adjusts the genlock phase to the given percentage.
// ------------------------------------------------------------------------
void vsIS600::setGenlockPhase(int phase)
{
    unsigned char buf[20];
    int           index;

    // Validate the phase parameter
    if ((phase < 0) || (phase > 100))
    {
        printf("vsIS600::setGenlockPhase:  Invalid parameter\n");
        return;
    }
 
    // Set up the genlock phase packet
    buf[0] = VS_IS_CMD_MFR_SPECIFIC;
    buf[1] = VS_IS_CMD_GENLOCK;
    buf[1] = VS_IS_CMD_GENLOCK_PHASE;
    
    // Append the phase argument
    index = 3;
    index += sprintf((char *)&buf[index], "%d", phase);

    // Terminate the packet
    buf[index] = '\r';

    // Send the packet
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

    // Print status as we go
    printf("vsIS600::setOutputFormat:\n");

    // If the array is too big, clip it 
    if (newFormatNum > VS_IS_MAX_OUTPUT_ITEMS)
        formatNum = VS_IS_MAX_OUTPUT_ITEMS;
    else
        formatNum = newFormatNum;

    // Initialize the data size and format array index
    size = 0;
    index = 0;

    // For each element in the new data format array, validate it and
    // copy it into the internal format array.  Also, calculate the new
    // output packet size as we go.
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

    // First, the header
    buf[0] = VS_IS_CMD_OUTPUT_LIST;
    index = 2;

    // Iterate through the data format array and build the data format
    // packet
    for (i = 0; i < formatNum; i++)
    {
        // Matrix output is handled in a special way
        if (formatArray[i] == VS_IS_FORMAT_MATRIX)
        {
            // Request all three directional cosine vectors if MATRIX is
            // selected (see pp. 98-101 of the FASTRAK manual for details)
            index += sprintf((char *)(&buf[index]), ",5,6,7");
        }
        else
        {
            // Request the next item
            index += sprintf((char *)(&buf[index]), ",%d", formatArray[i]);
        }
    }

    // Terminate the packet
    buf[index] = '\r';

    // Set each tracker to output the new output list
    for (i = 0; i < VS_IS_MAX_TRACKERS; i++)
    {
        buf[1] = i + 1 + '0';

        port->writePacket(buf, index + 1);
    }

    // Flush the port of any stale data
    port->flushPort();

    // Ping for a new packet if we're not streaming data
    if (!streaming)
        ping();
}

// ------------------------------------------------------------------------
// Set the units for position output (inches or centimeters)
// ------------------------------------------------------------------------
void vsIS600::setUnits(int units)
{
    unsigned char buf;

    // Select the appropriate data units command according to the parameter
    if (units == VS_IS_UNITS_CENTIMETERS)
        buf = VS_IS_CMD_UNITS_CM;
    else
        buf = VS_IS_CMD_UNITS_INCHES;

    // Send the units command
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
    int      i;
    
    // Check to see if we're using a forked server thread
    if (forked)
    {
        // Copy the latest tracker data from the server thread.  Ensure
        // mutual exclusion using the critical section object.
        EnterCriticalSection(&criticalSection);
        for (i = 0; i < numTrackers; i++)
        {
            // Copy the private tracker data to the public trackers
            tracker[i]->setPosition(privateTracker[i]->getPositionVec());
            tracker[i]->setOrientation(
                privateTracker[i]->getOrientationQuat());
        }
        LeaveCriticalSection(&criticalSection);
    }
    else
    {
        // Get the data directly from hardware
        updateSystem();
    }
}
