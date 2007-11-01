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
//    VESS Module:  vsFastrak.c++
//
//    Description:  Class supporting the Polhemus FASTRAK motion tracking
//                  system.  This class supports a single FASTRAK running
//                  over an RS-232 interface with up to VS_FT_MAX_TRACKERS
//                  receivers.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsFastrak.h++"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef _MSC_VER
    #include <unistd.h>
#endif

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
    atQuat quat1, quat2;

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

#ifdef WIN32
    sprintf(portDevice, "COM%d", portNumber);
#endif

    // Initialize variables
    port = NULL;
    numTrackers = 0;
    forked = false;
    serverThreadID = 0;
    streaming = false;

    // Set up a coordinate conversion quaternion that will convert
    // Polhemus coordinates to VESS coordinates
    quat1.setAxisAngleRotation(0, 0, 1, 90);
    quat2.setAxisAngleRotation(0, 1, 0, 180);
    coordXform = quat2 * quat1;

    // Initialize the motion trackers
    for (i = 0; i < VS_FT_MAX_TRACKERS; i++)
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
            printf("vsFastrak::vsFastrak: "
                "WARNING -- Only %d trackers found, expecting %d\n",
                numTrackers, nTrackers);
        }

        // Print a status message if we're using fewer trackers than available
        if ((numTrackers > nTrackers) && (nTrackers > 0))
        {
            printf("vsFastrak::vsFastrak: Configuring %d of %d trackers\n",
                nTrackers, numTrackers);

            numTrackers = nTrackers;
        }

        // Check the byte ordering of the local machine
        bigEndian = isBigEndian();

        // Use binary data
        setBinaryOutput();

        // Initialize the output format
        initOutputFormat();

        // Print status
        printf("vsFastrak::vsFastrak: Fastrak running on %s "
            "with %d tracker(s)\n", portDevice, numTrackers);

        // Request the first data packet
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
    DWORD exitCode;
    int timeOut;

    // Delete the motion trackers
    for (i = 0; i < VS_FT_MAX_TRACKERS; i++)
    {
        if (tracker[i] != NULL)
            delete tracker[i];
    }

    // Terminate the server thread if we've forked
    if (forked)
    {
        printf("vsFastrak::~vsFastrak:  Notifying server thread to quit\n");
        serverDone = true;
        
        // Wait (up to 3 seconds) for the thread to quit
        GetExitCodeThread(serverThread, &exitCode);
        timeOut = 30;
        while ((exitCode == STILL_ACTIVE) && (timeOut > 0))
        {
            // Get the current exit code of the thread
            GetExitCodeThread(serverThread, &exitCode);
            
            // Wait another tenth of a second
            usleep(100000);
            timeOut--;
        }
    }

    // Shut down the FASTRAK if the serial port is valid and we haven't forked
    if ((port != NULL) && (!forked))
    {
        printf("vsFastrak::~vsFastrak:  Shutting down Fastrak\n");

        // Reset the FASTRAK
        buf = VS_FT_CMD_REINIT_SYSTEM;
        port->writePacket(&buf, 1);
        Sleep(1000);

        // Flush and delete the serial port
        port->flushPort();
        delete port;
    }
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsFastrak::getClassName()
{
    return "vsFastrak";
}

// ------------------------------------------------------------------------
// Loop for the server portion of this class when operating in multiple
// threads
// ------------------------------------------------------------------------
DWORD WINAPI vsFastrak::serverLoop(void *parameter)
{
    vsFastrak     *instance;
    int           i;
    atVector      posVec;
    atQuat        ornQuat;
    unsigned char buf;

    // Get the vsFastrak object from the parameter
    instance = (vsFastrak *)parameter;

    // Initialize the done flag
    instance->serverDone = false;

    // Start streaming data
    instance->startStream();
    
    // Constantly update the data in the private tracker objects
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

    // Shut down the tracking system if the serial port is valid
    if (instance->port != NULL)
    {
        printf("vsFastrak::serverLoop:  Shutting down Fastrak\n");

        // Reset the FASTRAK
        buf = VS_FT_CMD_REINIT_SYSTEM;
        instance->port->writePacket(&buf, 1);
        Sleep(1000);

        // Flush and delete the serial port
        instance->port->flushPort();
        delete instance->port;
    }

    // Return from the thread (this calls ExitThread() implicitly)
    return 0;
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

    // Clear any garbage that might have sneaked onto the
    // serial port
    buf[0] = '\r';
    port->writePacket(buf, 1);
    Sleep(100);
    port->flushPort();

    // Stop the FASTRAK from streaming (if it is)
    stopStream();
    port->flushPort();

    // Print status info as we go
    printf("vsFastrak::enumerateTrackers:\n");

    // Initialize the number of trackers to zero
    numTrackers = 0;

    // Request the state of all stations in the system
    buf[0] = VS_FT_CMD_STATION_STATE;
    buf[1] = '1';
    buf[2] = '\r';
    port->writePacket(buf, 3);

    // Wait for hardware
    Sleep(1000);

    // Check the result
    result = port->readPacket(buf, 9);
    if (result < 9)
    {
        printf("   Error reading active station state (%d of 9 bytes)\n",
            result);
        port->flushPort();
    }

    // Terminate the string in the buffer
    buf[9] = 0;

    // Report each station's status and configure a tracker if it is
    // active

    // Station 1
    if (buf[3] == '1')
    {
        // Create tracker for station 1
        tracker[numTrackers] = new vsMotionTracker(numTrackers);

        // Map the current tracker number to station 1
        station[1] = numTrackers;

        // Increment the number of trackers
        numTrackers++;
        printf("    Station 1 is active\n");
    }
    else
    {
        station[1] = -1;
        printf("    Station 1 is not active\n");
    }

    // Station 2
    if (buf[4] == '1')
    {
        // Create tracker for station 2
        tracker[numTrackers] = new vsMotionTracker(numTrackers);

        // Map the current tracker number to station 2
        station[2] = numTrackers;

        // Increment the number of trackers
        numTrackers++;
        printf("    Station 2 is active\n");
    }
    else
    {
        station[2] = -1;
        printf("    Station 2 is not active\n");
    }

    // Station 3
    if (buf[5] == '1')
    {
        // Create tracker for station 3
        tracker[numTrackers] = new vsMotionTracker(numTrackers);

        // Map the current tracker number to station 3
        station[3] = numTrackers;

        // Increment the number of trackers
        numTrackers++;
        printf("    Station 3 is active\n");
    }
    else
    {
        station[3] = -1;
        printf("    Station 3 is not active\n");
    }

    // Station 4
    if (buf[6] == '1')
    {
        // Create tracker for station 4
        tracker[numTrackers] = new vsMotionTracker(numTrackers);

        // Map the current tracker number to station 4
        station[4] = numTrackers;

        // Increment the number of trackers
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

    // Read the result
    result = port->readPacket(buf, VS_FT_SIZE_CMD_PACKET);

    // Terminate the string in the result packet
    buf[result] = 0;

    // Parse the format string into formatArray[]
    token = strtok((char *)(&buf[4]), " \n");
    formatNum = 0;
    while (token != NULL)
    {
        // Since we only support items 5, 6, and 7 (directional cosines)
        // together as a group (rotation matrix), we need to throw out any 
        // stray 6's and 7's
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
// From Harbinson&Steele.  Determines whether this machine is big- or
// little-endian.
// ------------------------------------------------------------------------
bool vsFastrak::isBigEndian()
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
// vice versa)  The FASTRAK uses little-endian floats, so we'll need to
// convert if the machine is big-endian
// ------------------------------------------------------------------------
void vsFastrak::endianSwap(float *inFloat, float *outFloat)
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
// Set the FASTRAK to binary output mode
// ------------------------------------------------------------------------
void vsFastrak::setBinaryOutput()
{
    unsigned char buf;

    // Send the command to switch to binary output
    printf("vsFastrak::setBinaryOutput: Switching to binary output\n");
    buf = VS_FT_CMD_BINARY_OUTPUT;
    port->writePacket(&buf, 1);
    port->flushPort();
}

// ------------------------------------------------------------------------
// Update the given tracker's position with the given atVector
// ------------------------------------------------------------------------
void vsFastrak::updatePosition(int trackerNum, atVector positionVec)
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
void vsFastrak::updateRelativePosition(int trackerNum, atVector deltaVec)
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
void vsFastrak::updateAngles(int trackerNum, atVector orientationVec)
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
void vsFastrak::updateMatrix(int trackerNum, atMatrix orientationMat)
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
void vsFastrak::updateQuat(int trackerNum, atQuat quat)
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
// Requests an update packet from the FASTRAK
// ------------------------------------------------------------------------
void vsFastrak::ping()
{
    unsigned char buf;

    // Send a ping command to the FASTRAK
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
    atVector      tempVec;
    atVector      deltaVec;
    atMatrix      tempMat;
    atQuat        tempQuat;

    // Check to see if the FASTRAK streaming
    if (streaming)
    {
        // Initialize the counters
        bytesRead = 0;
        errorRetry = 10;

        // Read in (outputSize * numTrackers) bytes
        while ((bytesRead < (outputSize * numTrackers)) && (errorRetry > 0))
        {
            // Read one byte
            result = port->readPacket(&buf[bytesRead], 1);

            // Check to see if we got anything
            if (result != 0)
            {
                // If we're reading the first byte...
                if (bytesRead == 0)
                {
                    // Check to make sure we're starting at the beginning of
                    // a data record (first byte = '0'), otherwise ignore this 
                    // byte
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
                // Decrement retry count
                errorRetry--;
                
                // Flush the serial port and try again
                port->flushPort();
                bytesRead = 0;
            }
        }
 
        // If we failed to read the tracker data, report an error
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
        // Not streaming, read the whole packet at once
        bytesRead = port->readPacket(buf, outputSize * numTrackers);

        // Make sure we read the right number of bytes, and that the first
        // byte is the correct value ('0').  Report an error and flush
        // the serial port if not.
        if ((bytesRead != (outputSize * numTrackers)) || (buf[0] != '0'))
        {
            printf("vsFastrack::updateSystem: "
                "Error reading FASTRAK data (%d of %d bytes)\n",
                bytesRead, outputSize * numTrackers);
            port->flushPort();
        }
    }

    // If we managed to get all the data, process it
    if (bytesRead == (outputSize * numTrackers))
    {
        // Process each tracker's data
        for (i = 0; i < numTrackers; i++)
        {
            // Get the station number from the tracker data
            currentStation = buf[(i * outputSize) + 1] - '0';

            // Get the tracker number from the station/tracker mapping
            // created earlier
            currentTracker = station[currentStation];

            // Check for valid tracker number, print an error and
            // flush the port if invalid
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
                // Compute the index into the data buffer
                bufIndex = (i * outputSize) + 3;

                // Initialize the temporary data structures
                tempShort = 0;
                tempVec.setSize(3);
                tempVec.clear();
                tempMat.setIdentity();
                tempQuat.clear();

                // Start with the first output item
                outputItem = 0;

                // Read each data element according to the current data
                // format.  Stop when we've processed the correct amount of
                // data
                while (bufIndex < ((i + 1) * outputSize))
                {
                    // Check the data type for the current output item
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

                        case VS_FT_FORMAT_REL_POS:
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

                        case VS_FT_FORMAT_ANGLES:
                            // Extract the angles into an Euler angle vector
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

                        case VS_FT_FORMAT_MATRIX:
                            // Extract the matrix elements into a atMatrix
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

                        case VS_FT_FORMAT_QUAT:
                            // Extract the quaternion elements into a atQuat
                            for (j = 0; j < 4; j++)
                            {
                                endianSwap((float *)&buf[bufIndex], 
                                    &tempFloat);

                                // The Fastrak sends the scalar part first,
                                // but the atQuat expects it last, so we have
                                // to account for this by shifting the indices
                                tempQuat[(j+1) % 4] = tempFloat;

                                bufIndex += sizeof(float);
                            }

                            // Update the orientation
                            updateQuat(currentTracker, tempQuat);
                            break;

                        case VS_FT_FORMAT_16BIT_POS:
                            // Extract the coordinates into a atVector
                            // The method is described in the FASTRAK
                            // manual
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

                            // Update the orientation
                            updatePosition(currentTracker, tempVec);
                            break;

                        case VS_FT_FORMAT_16BIT_ANGLES:
                            // Extract the matrix elements into a atVector
                            // The method is described in the FASTRAK
                            // manual
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

                            // Update the orientation
                            updateAngles(currentTracker, tempVec);
                            break;

                        case VS_FT_FORMAT_16BIT_QUAT:
                            // Extract the elements into a atQuat
                            // The method is described in the FASTRAK
                            // manual
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

    // Request the next update if we're not streaming
    if (!streaming)
        ping();
}

// ------------------------------------------------------------------------
// Spawn a separate (server) thread that continuously reads the FASTRAK
// and updates the vsMotionTraker data
// ------------------------------------------------------------------------
void vsFastrak::forkTracking()
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
    printf("vsFastrak::forkTracking:\n");
    printf("    Server Thread ID is %d\n", serverThreadID);
    
    // Set the forked flag to indicate we've started running multithreaded
    forked = true;
}

// ------------------------------------------------------------------------
// Starts continuous data output from the FASTRAK
// ------------------------------------------------------------------------
void vsFastrak::startStream()
{
    unsigned char buf;

    // Send the stream command to the FASTRAK
    buf = VS_FT_CMD_START_CONTINUOUS;
    port->writePacket(&buf, 1);

    // Set the streaming flag to true, so we know the FASTRAK is now 
    // streaming data
    streaming = true;
}

// ------------------------------------------------------------------------
// Stops continuous data output from the FASTRAK
// ------------------------------------------------------------------------
void vsFastrak::stopStream()
{
    unsigned char buf;

    // Send the stop continuous stream command to the FASTRAK
    buf = VS_FT_CMD_STOP_CONTINUOUS;
    port->writePacket(&buf, 1);

    // Clear the streaming flag, so we know that the FASTRAK is not
    // streaming data
    streaming = false;
}

// ------------------------------------------------------------------------
// Adjust the alignment frame for the specified station
// ------------------------------------------------------------------------
void vsFastrak::setAlignment(int station, atVector origin, atVector positiveX, 
                             atVector positiveY)
{
    unsigned char buf[VS_FT_SIZE_CMD_PACKET];
    int           index;

    // Reset the alignment frame to the identity matrix because the FASTRAK
    // expects an alteration to the alignment frame and not a replacement
    // alignment frame.  Resetting the alignment frame allows us to specify
    // an entirely new one.
    buf[0] = VS_FT_CMD_RESET_ALIGNMENT;
    buf[1] = station + '0';
    port->writePacket(buf, 2);

    // Construct the new alignment frame packet
    buf[0] = VS_FT_CMD_SET_ALIGNMENT;
    buf[1] = station;

    // Set the new alignment frame
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
void vsFastrak::resetAlignment(int station)
{
    unsigned char buf[4];

    // Set up the reset alignment frame command for the given station
    buf[0] = VS_FT_CMD_RESET_ALIGNMENT;
    buf[1] = station + '0';
    buf[2] = '\r';

    // Send the command
    port->writePacket(buf, 3);
}

// ------------------------------------------------------------------------
// Adjust the transmitter mounting frame for the given station to the given 
// orientation.
// ------------------------------------------------------------------------
void vsFastrak::setMountingFrame(int station, atVector orientation)
{
    unsigned char buf[VS_FT_SIZE_CMD_PACKET];
    int           index;

    // Set up the packet
    buf[0] = VS_FT_CMD_XMTR_MOUNT_FRAME;
    buf[1] = station + '0';

    // Set up the mounting frame
    index = 2;
    index += sprintf((char *)(&buf[index]), ",%0.2lf", orientation[AT_H]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", orientation[AT_P]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", orientation[AT_R]);
    buf[index] = '\r';

    // Send the command
    port->writePacket(buf, index + 1);
}

// ------------------------------------------------------------------------
// Set the sync mode to the given sync mode.  Be careful with this command.
// See the WARNING in section 6.5 of the manual.
// ------------------------------------------------------------------------
void vsFastrak::setSyncMode(int syncMode)
{
    unsigned char buf[3];

    // Set up the sync mode command with the given mode parameter
    buf[0] = VS_FT_CMD_SYNC_MODE;
    buf[1] = syncMode + '0';
    buf[2] = '\r';

    // Send the command
    port->writePacket(buf, 3);
}

// ------------------------------------------------------------------------
// Set the active hemisphere of the given station to the one specified by 
// the given vector
// ------------------------------------------------------------------------
void vsFastrak::setActiveHemisphere(int station, atVector zenithVec)
{
    unsigned char buf[VS_FT_SIZE_CMD_PACKET];
    int           index;

    // Set up the command packet
    buf[0] = VS_FT_CMD_HEMISPHERE;
    buf[1] = station + '0';

    // Set the zenith vector of the new hemisphere
    index = 2;
    index += sprintf((char *)(&buf[index]), ",%0.2lf", zenithVec[AT_X]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", zenithVec[AT_Y]);
    index += sprintf((char *)(&buf[index]), ",%0.2lf", zenithVec[AT_Z]);
    buf[index] = '\r';

    // Send the command
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

    // Print status as we go
    printf("vsFastrak::setOutputFormat:\n");

    // If the array is too big, clip it 
    if (newFormatNum > VS_FT_MAX_OUTPUT_ITEMS)
        formatNum = VS_FT_MAX_OUTPUT_ITEMS;
    else
        formatNum = newFormatNum;

    // Initialize the data size and the format array index
    size = 0;
    index = 0;

    // For each element in the new data format array, validate it and
    // copy it into the internal format array.  Also, calculate the new
    // output packet size as we go.
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
                break;
        }
    }

    // Total size:  3-byte header plus the data size
    outputSize = 3 + size;
    printf("   Total output size per tracker is %d bytes\n", outputSize);

    // Construct the new output list command

    // First the header
    buf[0] = VS_FT_CMD_OUTPUT_LIST;
    index = 2;

    // Iterate through the data format array and build the data format
    // packet
    for (i = 0; i < formatNum; i++)
    {
        // Matrix output is handled specially
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

    // Terminate the packet
    buf[index] = '\r';

    // Set each tracker to output the new output list
    for (i = 0; i < VS_FT_MAX_TRACKERS; i++)
    {
        buf[1] = i + 1 + '0';

        port->writePacket(buf, index + 1);
    }

    // Flush the port of any extraneous data
    port->flushPort();

    // Ping for a new data packet if we're not streaming data
    if (!streaming)
        ping();
}

// ------------------------------------------------------------------------
// Adjust the baud rate of the FASTRAK
// ------------------------------------------------------------------------
void vsFastrak::setBaudRate(long baud)
{
    char buf[20];
    int  baudCode;
    int  length;
    bool  wasStreaming;

    // Remember if we were streaming data
    wasStreaming = streaming;

    // Stop streaming
    stopStream();
    Sleep(100);
    port->flushPort();

    // Get the proper identifier for the baud rate
    baudCode = baud/100;

    // Send the new communications format
    length = sprintf(buf, "o%d,N,8,0\r", baudCode);
    port->writePacket((unsigned char *)buf, length);
    Sleep(100);

    // Set the baud rate on the serial port
    port->setBaudRate(baud);

    // Resume streaming or ping for a new packet
    if (wasStreaming)
        startStream();
    else
        ping();
}

// ------------------------------------------------------------------------
// Set the units for position output (inches or centimeters)
// ------------------------------------------------------------------------
void vsFastrak::setUnits(int units)
{
    unsigned char buf;

    // Select the appropriate units command based on the units parameter
    if (units == VS_FT_UNITS_CENTIMETERS)
        buf = VS_FT_CMD_UNITS_CM;
    else
        buf = VS_FT_CMD_UNITS_INCHES;

    // Send the command
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
