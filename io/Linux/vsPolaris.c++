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
//    VESS Module:  vsPolaris.c++
//
//    Description:  Support for the Northern Digital, Inc. POLARIS optical
//                  tracking system.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#include "vsPolaris.h++"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

// Static class variable for instructing the server (child) process to exit
bool vsPolaris::serverDone;

// ------------------------------------------------------------------------
// Creates a vsPolaris object on the given serial port using the given
// baud rate and number of trackers.  Note that only wired trackers are
// counted by default.  Any wireless trackers must be added separately,
// since the system cannot detect them automatically.
// ------------------------------------------------------------------------
vsPolaris::vsPolaris(int portNumber, long baud, int nTrackers)
{
    int i;
    char portDevice[30];
    vsQuat quat1, quat2;

    // Initialize data members
    memset(tracker, 0, sizeof(tracker));
    memset(portHandle, 0, sizeof(portHandle));

    // Test for endianness
    bigEndian = isBigEndian();

    // Create the serial port device name
    sprintf(portDevice, "/dev/ttyS%d", portNumber - 1);

    // Set up coordinate conversion
    quat1.setAxisAngleRotation(0, 1, 0, 90.0);
    quat2.setAxisAngleRotation(1, 0, 0, 90.0);
    coordXform = quat1 * quat2;
    coordXformInv = coordXform.getConjugate();

    // Initialize the reference frame to identity.  This assumes that
    // the tracker's cameras are mounted facing forward
    referenceFrame.set(0.0, 0.0, 0.0, 1.0);

    // Open the port
    port = new vsSerialPort(portDevice, 9600, 8, 'N', 1);

    // Initialize the Polaris hardware
    initializeSystem(baud);

    // Count the active trackers attached to the system
    numTrackers = enumerateTrackers();

    // Print a warning if we have too few trackers, or an informational
    // message if we're not using all available trackers
    if (numTrackers < nTrackers)
    {
        printf("vsPolaris::vsPolaris:  WARNING -- Only %d trackers found,"
            " expecting %d.\n", numTrackers, nTrackers);
    }
    else if ((numTrackers > nTrackers) && (nTrackers != 0))
    {
        printf("vsPolaris::vsPolaris:  Configuring %d of %d trackers.\n",
            numTrackers, nTrackers);
    }

    // Test the environment for infrared interference.  Print a warning
    // if stray infrared light is detected.
    if (testIR() == false)
    {
        printf("vsPolaris::vsPolaris:  WARNING -- Infrared interference "
            "detected!\n");
    }

    // Start tracking the markers
    startTracking();

    // Ping 10 times to "warm up" the tracking system (recommended procedure
    // according to NDI techs)
    for (i = 0; i < 10; i++)
    {
        ping();
        getBinaryReply();
    }

    // Issue one final ping to prepare the first set of tracker data
    ping();
}

// ------------------------------------------------------------------------
// Destructor.  Shuts down the Polaris and closes the serial port
// ------------------------------------------------------------------------
vsPolaris::~vsPolaris()
{
    int i;

    // Print status info for the shutdown
    printf("vsPolaris::~vsPolaris:\n");

    // Kill the server process if we've forked
    if (forked)
    {
        printf("  Notifying server process to quit\n");
        kill(serverPID, SIGUSR1);

        // Disconnect from shared memory
        delete sharedData;
    }

    // Delete the motion trackers
    printf("  Deleting vsMotionTrackers\n");
    for (i = 0; i < VS_PL_MAX_TRACKERS; i++)
        if (tracker[i] != NULL)
            delete tracker[i];

    // If we haven't forked a server process, stop tracking and reset
    // the Polaris now.
    if (!forked)
    {
        printf("  Resetting Polaris\n");
        stopTracking();
        resetSystem();

        // Close the serial port
        printf("  Closing serial port\n");
        delete port;
    }
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsPolaris::getClassName()
{
    return "vsPolaris";
}

// ------------------------------------------------------------------------
// Initializes the Polaris hardware to prepare it for tracking
// ------------------------------------------------------------------------
void vsPolaris::initializeSystem(long baud)
{
    int result;

    // Do a reset to make sure we can communicate with the hardware
    resetSystem();

    // Set the Polaris to the desired baud rate
    setBaudRate(baud);

    // Get the version information from the polaris
    sendCommand("VER:4");
    
    // Get the response
    result = getReply();
    if (result != VS_PL_ERR_NONE)
    {
        // Error happened, print an error message
        printError("initializeSystem", "Error getting firmware revision",
            result);
    }
    else
    {
        // Print the firmware information
        printf("Polaris control firmware:\n");
        printf("%s\n", (char *)dataBuffer);
    }

    // Send the initialization command
    sendCommand("INIT:");

    // Print an error if initialization fails
    result = getReply();
    if (result != VS_PL_ERR_NONE)
    {
        printError("initializeSystem", "Error initializing Polaris", result);
    }
}

// ------------------------------------------------------------------------
// Performs a background infrared test to determine if too much IR light
// is present for effective tracking
// ------------------------------------------------------------------------
bool vsPolaris::testIR()
{
    int result;
    bool irResult;
    unsigned char buf[100];

    // Set the test result flag to true (no IR interference) to start
    irResult = true;

    // Put the Polaris in diagnostic mode
    sendCommand("DSTART:");

    // Check the command response
    result = getReply();
    if (result != VS_PL_ERR_NONE)
    {
        // Print an error message on a bad response
        printError("testIR", "Error entering diagnostic mode", result);
    }

    // Initialize the IR emitters for the test
    sendCommand("IRINIT:");

    // Check the command response
    result = getReply();
    if (result != VS_PL_ERR_NONE)
    {
        // Print an error message on a bad response
        printError("testIR", "Error initializing IR test", result);
    }

    // Perform the IR diagnostic
    sendCommand("IRCHK:");

    // Check the command response
    result = getReply();
    if (result != VS_PL_ERR_NONE)
    {
        // Print an error message on a bad response
        printError("testIR", "Error performing IR test", result);
    }
    else
    {
        // Print a warning if stray IR is detected
        if (dataBuffer[0] == '1')
        {
            printf("vsPolaris::testIR:  WARNING -- Infrared interference "
                "detected!\n");
            printf("    Tracking results may not be reliable\n");

            // Interference detected, set the result to false
            irResult = false;
        }
    }

    // Exit from diagnostic mode
    sendCommand("DSTOP:");

    // Check the command response
    result = getReply();
    if (result != VS_PL_ERR_NONE)
    {
        // Print an error message on a bad response
        printError("testIR", "Error exiting diagnostic mode", result);
    }

    return irResult;
}

// ------------------------------------------------------------------------
// Counts the number of active tools attached to the Polaris, and prepares
// them for use.  This procedure comes directly from the Polaris API
// manual.
// ------------------------------------------------------------------------
int vsPolaris::enumerateTrackers()
{
    int trackerCount;
    bool doneEnumerating, doneInitializing;
    char *replyPtr;
    char tmpStr[10];
    char cmdStr[10];
    char portNumber[3];
    int portsToFree, portsToInitialize, portsToEnable;
    int result;
    int i;

    // Start with zero trackers
    trackerCount = 0;

    // Print status information as we go
    printf("vsPolaris::enumerateTrackers:\n");

    // Initialize the "done" flag to false (not done)
    doneEnumerating = false;

    // Keep searching for ports until no more are found
    while (!doneEnumerating)
    {
        // See if any old port handles need to be freed
        sendCommand("PHSR:01");

        // Check the command response
        result = getReply();
        if (result != VS_PL_ERR_NONE)
        {
            // Print an error message on a bad response
            printError("enumerateTrackers", "Error querying port status", 
                result);
        }
        else
        {
            // Get the number of port handles to be freed
            replyPtr = (char *)&dataBuffer[0];
            strncpy(tmpStr, replyPtr, 2);
            tmpStr[2] = 0;
            portsToFree = atoi(tmpStr);

            // Free each port handle
            replyPtr += 2;
            for (i = 0; i < portsToFree; i++)
            {
                // Parse the port handle number
                strncpy(tmpStr, replyPtr, 2);
                tmpStr[2] = 0;
                
                // Send the free command for the given port handle
                sprintf(cmdStr, "PHF:%s", tmpStr);
                sendCommand(cmdStr);

                // Check the response from the Polaris
                result = getReply();
                if (result != VS_PL_ERR_NONE)
                {
                    // Print an error message on a bad response
                    printError("enumerateTrackers", 
                        "Error freeing port handle", result);
                }

                // Move to the next port handle
                replyPtr += 5;
            }
        }

        // Next, check for ports to initialize  
        doneInitializing = false;
        while (!doneInitializing)
        {
            // Search for port handles that need initializing
            sendCommand("PHSR:02");

            // Check the response from the Polaris
            result = getReply();
            if (result != VS_PL_ERR_NONE)
            {
                // Print an error message on a bad response
                printError("enumerateTrackers", 
                    "Error querying port status", result);

                // We've hit an error, so stop trying to initialize ports
                doneInitializing = true;
            }
            else
            {
                // Get the number of port handles to be initialized
                replyPtr = (char *)&dataBuffer[0];
                strncpy(tmpStr, replyPtr, 2);
                tmpStr[2] = 0;
                portsToInitialize = atoi(tmpStr);

                // Set the done flag if there are no ports to initialize
                if (portsToInitialize == 0)
                    doneInitializing = true;
            
                // Initialize each port handle
                replyPtr += 2;
                for (i = 0; i < portsToInitialize; i++)
                {
                    // Parse the port handle number
                    strncpy(tmpStr, replyPtr, 2);
                    tmpStr[2] = 0;
                
                    // Send the initialize command for the given port handle
                    sprintf(cmdStr, "PINIT:%s", tmpStr);
                    sendCommand(cmdStr);

                    // Initialization can take a few seconds, so wait before
                    // checking for a reply
                    sleep(3);

                    // Check the response from the Polaris
                    result = getReply();
                    if (result != VS_PL_ERR_NONE)
                    {
                        // Print an error message on a bad response
                        printError("enumerateTrackers", 
                            "Error initializing port handle", result);
                    }

                    // Move to the next port handle
                    replyPtr += 5;
                }
            }
        }

        // Finally, check for ports to enable
        sendCommand("PHSR:03");

        // Check the command response
        result = getReply();
        if (result != VS_PL_ERR_NONE)
        {
            // Print an error message on a bad response
            printError("enumerateTrackers", "Error querying port status", 
                result);
        }
        else
        {
            // Get the number of port handles to be freed
            replyPtr = (char *)&dataBuffer[0];
            strncpy(tmpStr, replyPtr, 2);
            tmpStr[2] = 0;
            portsToEnable = atoi(tmpStr);

            // Set the done flag if there are no ports left to enable
            if (portsToEnable == 0)
                doneEnumerating = true;
            
            // Enable each port handle, use dynamic tracking for all.
            // Upon enabling, create a vsMotionTracker for each port
            // handle.
            replyPtr += 2;
            for (i = 0; i < portsToEnable; i++)
            {
                // Parse the port handle number
                strncpy(tmpStr, replyPtr, 2);
                tmpStr[2] = 0;

                // Send the free command for the given port handle
                sprintf(cmdStr, "PENA:%sD", tmpStr);
                sendCommand(cmdStr);

                // Check the response from the Polaris
                result = getReply();
                if (result != VS_PL_ERR_NONE)
                {
                    // Print an error message on a bad response
                    printError("enumerateTrackers", 
                        "Error freeing port handle", result);
                }
                else
                {
                    // Remember this port handle
                    sscanf(tmpStr, "%02X", &portHandle[trackerCount]);

                    // Get the physical port location for this port
                    // handle so we can print it out
                    sprintf(cmdStr, "PHINF:%02X0020", portHandle[trackerCount]);
                    sendCommand(cmdStr);

                    // Get the reply, and print information for this tracker
                    result = getReply();
                    if (result == VS_PL_ERR_NONE)
                    {
                        // Get the port number
                        portNumber[0] = dataBuffer[10];
                        portNumber[1] = dataBuffer[11];
                        portNumber[2] = 0;
                       
                        printf("    Tracker %d: port handle %s, physical "
                            "port %s\n", trackerCount, tmpStr, portNumber);
                    }
                    else
                    {
                        // We couldn't get the physical port number, so
                        // print what we know
                        printf("    Tracker %d: port handle %s, physical "
                            "port ??\n", trackerCount, tmpStr);
                    }

                    // Polaris trackers can have up to three buttons.  We
                    // can't tell ahead of time how many buttons there are
                    // so we'll just create the tracker with support for
                    // three buttons
                    tracker[trackerCount] = 
                        new vsMotionTracker(trackerCount, 3);

                    // Increment the tracker count
                    trackerCount++;
                }

                // Move to the next port handle
                replyPtr += 5;
            }
        }
    }

    return trackerCount;
}

// ------------------------------------------------------------------------
// Returns true if the local machine is a big-endian machine
// ------------------------------------------------------------------------
bool vsPolaris::isBigEndian()
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
// Swap the two bytes in a short value
// ------------------------------------------------------------------------
short vsPolaris::swapShort(short input)
{
    short output;

    // Swap the two input bytes and store them in output
    ((unsigned char *)(&output))[0] = ((unsigned char *)(&input))[1];
    ((unsigned char *)(&output))[1] = ((unsigned char *)(&input))[0];

    // Return the byte-swapped short
    return output;
}

// ------------------------------------------------------------------------
// Swap the two bytes in a float value
// ------------------------------------------------------------------------
float vsPolaris::swapFloat(float input)
{
    float output;

    // Swap the four input bytes and store them in output
    ((unsigned char *)(&output))[0] = ((unsigned char *)(&input))[3];
    ((unsigned char *)(&output))[1] = ((unsigned char *)(&input))[2];
    ((unsigned char *)(&output))[2] = ((unsigned char *)(&input))[1];
    ((unsigned char *)(&output))[3] = ((unsigned char *)(&input))[0];

    // Return the byte-swapped float
    return output;
}

// ------------------------------------------------------------------------
// Calculates a 16-bit Cyclic Redundancy Check (CRC) 
// This is used in error detection for the serial port communications
// between the host and the Polaris hardware.  This routine comes from
// the Polaris API manual directly.
//
// This particular CRC method uses the form (x^16 + x^15 + x^2 + 1).
// ------------------------------------------------------------------------
unsigned int vsPolaris::calculateCRC(unsigned char *string, int length)
{
    unsigned int crc, data;
    unsigned char ch, *chPtr;
    int i;

    static int oddParity[16] =
    {
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0
    };

    // Start at the beginning of the command string with a zero CRC
    crc = 0;
    chPtr = string;

    // Process each character in the command
    for (i = 0; i < length; i++)
    {
        // Factor the new character into the CRC value
        ch = *chPtr;
        data = (ch ^ (crc & 0xff)) & 0xff;
        crc >>= 8;
        if (oddParity[data & 0x0f] ^ oddParity[data >> 4])
        {
            crc ^= 0xc001;
        }
        data <<= 6;
        crc ^= data;
        data <<= 1;
        crc ^= data;

        // Advance to the next character
        chPtr++;
    }

    return crc;
}

// ------------------------------------------------------------------------
// Sends the given command to the Polaris, after attaching the appropriate
// CRC check to the end.
// ------------------------------------------------------------------------
void vsPolaris::sendCommand(char *command)
{
    short crc;
    char *chPtr;
    char fullCommand[50];
    char cmdLength;

    // Bail if the command string is NULL
    if (command == NULL)
    {
        printf("vsPolaris::sendCommand:  NULL command string specified\n");
        return;
    }

    // Bail if the serial port pointer is NULL
    if (port == NULL)
    {
        printf("vsPolaris::sendCommand:  Serial port not open\n");
        return;
    }

    // Get the crc value for the command
    crc = calculateCRC((unsigned char *)command, strlen(command));

    // Append the CRC and a return character to the end of the command 
    // string
    cmdLength = sprintf(fullCommand, "%s%04hX\r", command, crc);

    // Write the command to the serial port
    port->writePacket((unsigned char *)fullCommand, cmdLength);
}

// ------------------------------------------------------------------------
// Retrieves the reply from the Polaris and checks for errors in both the
// reply and the CRC value.  This version expects a text reply, and reads
// the port until a \r character is found.
// ------------------------------------------------------------------------
int vsPolaris::getReply()
{
    int replyIdx;
    int goodByte;
    int timeout;
    unsigned char inCh;
    char crcStr[5];
    short givenCRC, compCRC;

    // Clear the data buffer
    memset(dataBuffer, 0, sizeof(dataBuffer));

    // Start at the beginning of the buffer and read each character into
    // the data buffer.  Stop when we see a carriage return ('\r')
    replyIdx = 0;
    inCh = 0;
    while (inCh != '\r')
    {
        // Read the next character
        goodByte = port->readPacket(&inCh, 1);

        // Check to see if data was received
        if (!goodByte)
        {
            // Flush the serial port and return a no reply error
            port->flushPort();

            return VS_PL_ERR_NO_REPLY;
        }
        else
        {
            // Copy the input character to the data buffer
            dataBuffer[replyIdx] = inCh;
        }

        // Increment the index
        replyIdx++;
    }

    // Reply must be at least big enough to hold a CRC
    if (replyIdx < 5)
    {
        return VS_PL_ERR_NO_REPLY;
    }

    // NULL-terminate the reply string (write over the \r character)
    dataBuffer[replyIdx-1] = 0;

    // Get the CRC value returned by the Polaris
    strncpy(crcStr, (char *)&dataBuffer[replyIdx - 5], 5);
    crcStr[4] = 0;
    sscanf(crcStr, "%04hX", &givenCRC);

    // Remove the CRC from the reply string and calculate our own CRC
    dataBuffer[replyIdx - 5] = 0;
    compCRC = calculateCRC(dataBuffer, strlen((char *)dataBuffer));

    // Check the two CRC values against each other
    if (compCRC != givenCRC)
    {
        printf("givenCRC = 0x%04hX  compCRC = 0x%04hX\n", givenCRC, compCRC);
        return VS_PL_ERR_BAD_CRC;
    }

    // Now, check to see if the returned value is an error message
    if (strncmp((char *)dataBuffer, "ERROR", 5) == 0)
    {
        // The Polaris returned an error code
        return VS_PL_ERR_ERROR_MSG;
    }

    // If we get this far, we have good data
    return VS_PL_ERR_NONE;
}

// ------------------------------------------------------------------------
// Retrieves the reply from the Polaris and checks for errors in both the
// reply and the CRC value.  This version (used only for the BX command)
// expects a binary reply, and uses the header and length information in 
// the reply packet to determine how much data to read.
// ------------------------------------------------------------------------
int vsPolaris::getBinaryReply()
{
    int replyIdx;
    int length;
    int i;
    int timeout;
    char crcStr[5];
    short givenCRC, compCRC;
    short packetLength;

    // Clear the data buffer
    memset(dataBuffer, 0, sizeof(dataBuffer));

    // Read the reply header, look for the first header byte
    while (dataBuffer[0] != VS_PL_BX_REPLY_2)
    {
        length = port->readPacket(&dataBuffer[0], 1);

        // Check to see if data was received
        if (length == 0)
        {
            // No data, flush the serial port and bail out
            port->flushPort();
            return VS_PL_ERR_NO_REPLY;
        }

        // See if an error code is being returned
        if (dataBuffer[0] == 'E')
        {
            length = port->readPacket(&dataBuffer[1], 10);

            // Check the length of data received
            if (length < 9)
            {
                // Incomplete message, flush the serial port and bail out
                port->flushPort();
                return VS_PL_ERR_NO_REPLY;
            }
            else if (strncmp((char *)dataBuffer, "ERROR", 5) == 0)
            {
                // NULL-terminate the error string
                dataBuffer[12] = 0;

                // Get the CRC to verify the error code
                sscanf((char *)&dataBuffer[7], "%04X", givenCRC);

                // Remove the CRC from the error string and calculate our
                // own CRC
                dataBuffer[8] = 0;
                compCRC = calculateCRC(dataBuffer, strlen((char *)dataBuffer));

                // Check the CRC's
                if (givenCRC == compCRC)
                {
                    // Bad CRC match, flush the port and bail out
                    port->flushPort();
                    return VS_PL_ERR_BAD_CRC;
                }
                else
                {
                    // The Polaris returned an error code
                    return VS_PL_ERR_ERROR_MSG;
                }
            }
        }
    }

    // Read the second header byte, packet length, header CRC, and number
    // of port handles
    length = port->readPacket(&dataBuffer[1], 5);

    // Check the reply length and look for the second header byte
    if ((length < 5) || (dataBuffer[1] != VS_PL_BX_REPLY_1))
    {
        // Data is incomplete, flush the serial port and bail out
        port->flushPort();
        return VS_PL_ERR_NO_REPLY;
    }

    // Get the header CRC from the data (the Polaris uses little-endian
    // format)
    memcpy(&givenCRC, &dataBuffer[4], 2);
    if (bigEndian)
    {
        givenCRC = swapShort(givenCRC);
    }

    // Compute a CRC for the header
    compCRC = calculateCRC(dataBuffer, 4);

    // Check the two CRC values
    if (compCRC != givenCRC)
    {
        // No match, flush the port and bail out
        port->flushPort();
        return VS_PL_ERR_BAD_CRC;
    }

    // Get the packet length from the header
    memcpy(&packetLength, &dataBuffer[2], 2);
    if (bigEndian)
    {
        packetLength = swapShort(packetLength);
    }

    // Read the rest of the packet, including the two-byte CRC at the end
    length = port->readPacket(&dataBuffer[6], packetLength + 2);

    // Check the length of data read
    if (length < (packetLength + 2))
    {
        // Not enough data, flush the port and bail out
        port->flushPort();
        return VS_PL_ERR_NO_REPLY;
    }

    // Calculate the CRC for the packet
    compCRC = calculateCRC(&dataBuffer[6], packetLength);

    // Get the CRC from the end of the packet
    memcpy(&givenCRC, &dataBuffer[6+packetLength], 2);
    if (bigEndian)
    {
        givenCRC = swapShort(givenCRC);
    }

    // Check the two CRC values
    if (compCRC != givenCRC)
    {
        // No match, flush the port and bail out
        port->flushPort();
        return VS_PL_ERR_BAD_CRC;
    }

    // If we get here, we have a complete data packet
    return VS_PL_ERR_NONE;
}

// ------------------------------------------------------------------------
// Prints a formatted error message given the method, error header, and
// error code.
// ------------------------------------------------------------------------
void vsPolaris::printError(char *method, char *header, int code)
{
    short errorCode;

    // Print the first line using the given method and header message
    printf("vsPolaris::%s:  %s\n", method, header);
    
    // Print the text description of the error code
    switch (code)
    {
        case VS_PL_ERR_NONE:
            printf("    No error.\n");
            break;

        case VS_PL_ERR_NO_REPLY:
            printf("    Missing or incomplete reply from Polaris.\n");
            break;

        case VS_PL_ERR_BAD_CRC:
            printf("    CRC-16 check failed.\n");
            break;

        case VS_PL_ERR_ERROR_MSG:

            // Polaris returned an error code, translate and print it
            // First, we have to get the error code from the string
            sscanf((char *)dataBuffer, "ERROR%04X", &errorCode);

            // Print the error code information first
            printf("    ERROR%02X: ", errorCode);

            // Now print the corresponding error message
            if (errorCode < 0x33)
            {
                switch (errorCode)
                {
                     case 0x01: 
                         printf("Invalid command.\n");
                         break;

                     case 0x02: 
                         printf("Command too long.\n");
                         break;

                     case 0x03: 
                         printf("Command too short.\n");
                         break;

                     case 0x04: 
                         printf("Invalid CRC calculated for command.\n");
                         break;

                     case 0x05: 
                         printf("Time-out on command execution.\n");
                         break;

                     case 0x06: 
                         printf("Unable to set up new communications "
                             "parameters.\n");
                         break;

                     case 0x07: 
                         printf("Incorrect number of command parameters.\n");
                         break;

                     case 0x08: 
                         printf("Invalid port handle selected.\n");
                         break;

                     case 0x09: 
                         printf("Invalid tracking priority selected.\n");
                         break;

                     case 0x0A: 
                         printf("Invalid LED selected.\n");
                         break;

                     case 0x0B:
                         printf("Invalid LED state selected.\n");
                         break;

                     case 0x0C: 
                         printf("Command is invalid while in the current "
                             "mode.\n");
                         break;

                     case 0x0D: 
                         printf("No tool assigned to the selected port "
                             "handle.\n");
                         break;

                     case 0x0E: 
                         printf("Selected port handle not initialized.\n");
                         break;

                     case 0x0F: 
                         printf("Selected port handle not enabled.\n");
                         break;

                     case 0x10: 
                         printf("System not initialized.\n");
                         break;

                     case 0x11: 
                         printf("Unable to stop tracking.\n");
                         break;

                     case 0x12: 
                         printf("Unable to start tracking.\n");
                         break;

                     case 0x13: 
                         printf("Unable to initialize Tool-in-port.\n");
                         break;

                     case 0x14: 
                         printf("Invalid Position Sensor characterization "
                             "parameters.\n");
                         break;

                     case 0x15: 
                         printf("Unable to initialize the Measurement "
                             "System.\n");
                         break;

                     case 0x16: 
                         printf("Unable to start diagnostic mode.\n");
                         break;

                     case 0x17: 
                         printf("Unable to stop diagnostic mode.\n");
                         break;

                     case 0x18: 
                         printf("Unable to determine environmental infrared "
                             "interference.\n");
                         break;

                     case 0x19: 
                         printf("Unable to read device's firmware revision "
                             "information.\n");
                         break;

                     case 0x1A: 
                         printf("Internal Measurement System error.\n");
                         break;

                     case 0x1B: 
                         printf("Unable to initialize for evironmental "
                             "infrared diagnostics.\n");
                         break;

                     case 0x1C: 
                         printf("Unable to set marker firing signature.\n");
                         break;

                     case 0x1D: 
                         printf("Unable to search for SROM device IDs.\n");
                         break;

                     case 0x1E: 
                         printf("Unable to read SROM device data.\n");
                         break;

                     case 0x1F: 
                         printf("Unable to write SROM device data.\n");
                         break;

                     case 0x20: 
                         printf("Unable to select SROM device.\n");
                         break;

                     case 0x21: 
                         printf("Unable to test electrical current on tool.\n");
                         break;

                     case 0x22: 
                         printf("Enabled tools not supported by selected "
                             "volume parameters.\n");
                         break;

                     case 0x23: 
                         printf("Command parameter out of range.\n");
                         break;

                     case 0x24: 
                         printf("Unable to select parameters by volume.\n");
                         break;

                     case 0x25: 
                         printf("Unable to determine Measurement System "
                             "supported features list.\n");
                         break;

                     case 0x28: 
                         printf("SCU hardware has changed state; a card "
                             "has been removed or added.\n");
                         break;

                     case 0x29: 
                         printf("Main processor firmware corrupt.\n");
                         break;

                     case 0x2A: 
                         printf("No memory available for dynamic allocation "
                             "(heap is full).\n");
                         break;

                     case 0x2B: 
                         printf("Requested handle has not been allocated.\n");
                         break;

                     case 0x2C: 
                         printf("Requested handle has become unoccupied.\n");
                         break;

                     case 0x2D: 
                         printf("All handles have been allocated.\n");
                         break;

                     case 0x2E: 
                         printf("Incompatible firmware revisions.\n");
                         break;

                     case 0x2F: 
                         printf("Invalid port description.\n");
                         break;

                     case 0x30: 
                         printf("Requested port already assigned to a port "
                             "handle.\n");
                         break;

                     case 0x31: 
                         printf("Invalid input or output state.\n");
                         break;

                     case 0x32: 
                         printf("Invalid operation for device associated with "
                             "specified port handle.\n");
                         break;

                     case 0x33: 
                         printf("Feature not available.\n");
                         break;

                     default:
                         printf("Unknown error message from Polaris.\n");
                         break;
                }
            }
            else if (errorCode == 0xA2)
            {
                 printf("General purpose I/O access on external SYNC port "
                     "failed.\n");
            }
            else if (errorCode == 0xF1)
            {
                 printf("Too much environmental infrared.\n");
            }
            else if (errorCode == 0xF4)
            {
                 printf("Unable to erase Flash SROM device.\n");
            }
            else if (errorCode == 0xF5)
            {
                 printf("Unable to write Flash SROM device.\n");
            }
            else if (errorCode == 0xF6)
            {
                 printf("Unable to read Flash SROM device.\n");
            }
            else
            {
                printf("Unknown error message from Polaris.\n");
            }
            break;

        default:
            printf("Unknown error.\n");
            break;
    }
}

// ------------------------------------------------------------------------
// Method used by asynchronous server process to continuously poll the
// Polaris and update the associated motion trackers.
// ------------------------------------------------------------------------
void vsPolaris::serverLoop()
{
    int      i;
    vsVector posVec;
    vsQuat   ornQuat;

    // Set up the signal handler
    signal(SIGUSR1, vsPolaris::quitServer);

    vsPolaris::serverDone = false;

    // Initialize the data structures
    posVec.setSize(3);
    posVec.clear();
    ornQuat.clear();

    // Continuously update the shared data while we're running
    while (!vsPolaris::serverDone)
    {
        // Update the hardware
        updateSystem();

        // Read the tracker data and store it in shared memory
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

    // Remove the shared memory segment
    delete sharedData;

    // Reset the tracking system
    printf("  Resetting Polaris\n");
    stopTracking();
    resetSystem();

    printf("  Closing serial port\n");
    if (port != NULL)
        delete port;

    // Exit the forked process
    exit(0);
}

// ------------------------------------------------------------------------
// Static protected method.  Signals the server process to quit
// ------------------------------------------------------------------------
void vsPolaris::quitServer(int arg)
{
    vsPolaris::serverDone = true;
}

// ------------------------------------------------------------------------
// Instructs the Polaris to start tracking the markers
// ------------------------------------------------------------------------
void vsPolaris::startTracking()
{
    int result;

    // Send the start tracking command
    sendCommand("TSTART:");

    // Check the reply
    result = getReply();
    if (result != VS_PL_ERR_NONE)
    {
        // Something bad happened, print the error message
        printError("startTracking", "Error entering tracking mode", result);
    }
}

// ------------------------------------------------------------------------
// Instructs the Polaris to stop tracking the markers
// ------------------------------------------------------------------------
void vsPolaris::stopTracking()
{
    int result;

    // Wait for a short time and then flush the serial port.  This
    // will clear out the data that is coming in from the last ping()
    // command.
    usleep(100000);
    port->flushPort();

    // Send the stop tracking command
    sendCommand("TSTOP:");

    // Check the reply
    result = getReply();
    if (result != VS_PL_ERR_NONE)
    {
        // Something bad happened, print the error message
        printError("stopTracking", "Error stopping tracking mode", result);
    }
}

// ------------------------------------------------------------------------
// Requests the latest tracker data from the Polaris
// ------------------------------------------------------------------------
void vsPolaris::ping()
{
    // Request a tracker update in binary format.  Specify that we only
    // want transformation and port status (0x0001), and that we want
    // out-of-volume trackers to be reported as well (0x0800).
    sendCommand("BX:0801");
}

// ------------------------------------------------------------------------
// Parses the tracker data from the Polaris.  ping(), followed by
// getBinaryReply() must be called before this function can work properly.
// ------------------------------------------------------------------------
void vsPolaris::processTrackerData()
{
    int dataIndex;
    int numHandles;
    int handleNum;
    int handleStatus;
    int i, j;
    float qx, qy, qz, qw;
    float tx, ty, tz;
    float error;
    int trackerIndex;
    vsVector translation;
    vsQuat rotation;
    vsQuat translationXform;
    bool foundIt;
    unsigned char statusByte;
    vsInputButton *button;

    // First, get the number of port handles returned by the Polaris
    numHandles = dataBuffer[6];

    // The data starts at byte 7 of the data, after the two header
    // bytes, two length bytes, two header CRC bytes, and one "number 
    // of handles" byte
    dataIndex = 7;

    // For each port handle, get the transformation data.  Take note
    // of any important status bits
    for (i = 0; i < numHandles; i++)
    {
        // Get the handle number
        handleNum = dataBuffer[dataIndex];
        dataIndex++;

        // Find the tracker corresponding to the given port handle
        trackerIndex = 0;
        while ((trackerIndex < numTrackers) && 
            (portHandle[trackerIndex] != handleNum))
        {
            trackerIndex++;
        }

        // Get the handle status
        handleStatus = dataBuffer[dataIndex];
        dataIndex++;

        // Check the port handle status to see if it's valid
        if (handleStatus & 0x00000001)
        {
            // Port handle is valid, so transformation and error info
            // are available.  Read them now.
            memcpy(&qw, &dataBuffer[dataIndex], 4);
            dataIndex += 4;
            memcpy(&qx, &dataBuffer[dataIndex], 4);
            dataIndex += 4;
            memcpy(&qy, &dataBuffer[dataIndex], 4);
            dataIndex += 4;
            memcpy(&qz, &dataBuffer[dataIndex], 4);
            dataIndex += 4;
            memcpy(&tx, &dataBuffer[dataIndex], 4);
            dataIndex += 4;
            memcpy(&ty, &dataBuffer[dataIndex], 4);
            dataIndex += 4;
            memcpy(&tz, &dataBuffer[dataIndex], 4);
            dataIndex += 4;
            memcpy(&error, &dataBuffer[dataIndex], 4);
            dataIndex += 4;
            
            // Swap each quantity if we're on a big-endian machine
            if (bigEndian)
            {
                tx = swapFloat(tx);
                ty = swapFloat(ty);
                tz = swapFloat(tz);
                qx = swapFloat(qx);
                qy = swapFloat(qy);
                qz = swapFloat(qz);
                qw = swapFloat(qw);
                error = swapFloat(error);
            }

            // Construct the translation vector and rotation quaternion
            translation.set(tx, ty, tz);
            rotation.set(qx, qy, qz, qw);

            // Transform to VESS coordinates, accounting for any adjustments
            // to the reference frame and scaling from millimeters to meters
            translationXform = referenceFrame * coordXform;
            translation = translationXform.rotatePoint(translation);
            rotation = referenceFrame * coordXform * rotation * coordXformInv;
            translation.scale(0.001);

            // Pass the new measurements to the motion tracker object
            if (tracker[trackerIndex] != NULL)
            {
                tracker[trackerIndex]->setPosition(translation);
                tracker[trackerIndex]->setOrientation(rotation);
            }

            // Remember the measurement error (in case anyone's interested)
            trackingError[trackerIndex] = error;
        }

        // The remainder of the packet is reported even if the port
        // is not in a valid state (missing or disabled).  Process the 
        // first status byte.
        statusByte = dataBuffer[dataIndex];
        dataIndex++;

        // This byte contains the button state bits.  Update the button
        // state for button 0
        button = tracker[trackerIndex]->getButton(0);
        if (button != NULL)
        {
            if (statusByte & 0x02)
                button->setPressed();
            else
                button->setPressed();
        }

        // Update the button state for button 1
        button = tracker[trackerIndex]->getButton(1);
        if (button != NULL)
        {
            if (statusByte & 0x04)
                button->setPressed();
            else
                button->setPressed();
        }
            
        // Update the button state for button 2
        button = tracker[trackerIndex]->getButton(2);
        if (button != NULL)
        {
            if (statusByte & 0x08)
                button->setPressed();
            else
                button->setPressed();
        }

        // Skip the other three status bytes and the frame number (we're 
        // not interested in these at this point)
        dataIndex += 4;
    }
}

// ------------------------------------------------------------------------
// Updates the current state of the hardware
// ------------------------------------------------------------------------
void vsPolaris::updateSystem()
{
    int result;

    // Get the reply from the previous ping() call
    result = getBinaryReply();

    // Check the result
    if (result == VS_PL_ERR_NONE)
    {
        // Process the tracker data and update the vsMotionTrackers
        processTrackerData();
    }
    else
    {
        // Couldn't get the tracker data, print an error
        printError("updateSystem", "Error updating Polaris", result);
    }

    // Request the next set of data
    ping();
}

// ------------------------------------------------------------------------
// Spawns a separate server process to handle retrieving data from the
// Polaris hardware.
// ------------------------------------------------------------------------
void vsPolaris::forkTracking()
{
    key_t  theKey;
    time_t tod;

    // Use a portion of the time of day for the second half of the shared
    // memory key.  This helps prevent multiple shared memory segments with
    // the same key.
    tod = time(NULL);
    tod &= 0x0000FFFF;
    theKey = VS_PL_SHM_KEY_BASE | tod;

    // Fork the server process
    serverPID = fork();

    // Branch based which process we're now running
    switch(serverPID)
    {
        case -1:
            // Oops, the fork() call failed
            printf("vsPolaris::forkTracking:  fork() failed!\n");
            printf("    Continuing in single-process mode\n");
            break;

        case 0:
            // Create the shared memory area and enter the server loop
            sharedData = new vsSharedInputData(theKey, numTrackers, true);
            serverLoop();
            break;

        default:
            // Connect to the shared memory area (don't create it) and
            // continue with the application
            sharedData = new vsSharedInputData(theKey, numTrackers, false);
            forked = true;
            printf("vsPolaris::forkTracking:  Server PID is %d\n", serverPID);
            break;
    }
}

// ------------------------------------------------------------------------
// Changes the baud rate between the host and the Polaris system
// ------------------------------------------------------------------------
void vsPolaris::setBaudRate(long baud)
{
    int baudCode;
    int result;
    char baudCmd[15];

    // Get the Polaris code for the requested baud rate
    switch (baud)
    {
        case 9600:
            baudCode = 0;
            break;
        case 14400:
            baudCode = 1;
            break;
        case 19200:
            baudCode = 2;
            break;
        case 38400:
            baudCode = 3;
            break;
        case 57600:
            baudCode = 4;
            break;
        case 115200:
            baudCode = 5;
            break;
        default:
            baudCode = -1;
            break;
    }

    // Check the baud rate code for validity
    if (baudCode < 0)
    {
        printf("vsPolaris::setBaudRate:  Unsupported baud rate requested\n");
        return;
    }

    // Send the Polaris a request to change communications parameters
    sprintf(baudCmd, "COMM:%d0000", baudCode);
    sendCommand(baudCmd);

    // Check the response
    result = getReply();
    if (result == VS_PL_ERR_NONE)
    {
        // Wait 100ms before changing the host baud rate
        usleep(100000);
        port->setBaudRate(baud);
    }
    else
    {
        printError("setBaudRate", "Unable to change baud rate", result);
    }
}

// ------------------------------------------------------------------------
// Loads a tool description image from a file into the Polaris's memory
// This is necessary for development tools without a permanent SROM image,
// or for passive wireless tools.  The image file is created with NDI's
// software.
// ------------------------------------------------------------------------
void vsPolaris::loadToolImage(int trackerNum, char *fileName)
{
    FILE *toolImage;

    // Stop tracking the tools
    stopTracking();

    // Open the image file
    toolImage = fopen(fileName, "rb");

    // Write the file to the Polaris (maximum of 64 bytes per command)

    // Reinitialize the port

    // Close the image file
    fclose(toolImage);

    // Return to tracking mode
    startTracking();
}

// ------------------------------------------------------------------------
// Changes the characteristic volume of the tracking area.  The Polaris
// must be aware of the parameters of the given volume number.
// ------------------------------------------------------------------------
void vsPolaris::setTrackingVolume(int volumeNumber)
{
    char cmdStr[10];
    int result;

    // Validate the volume number (only 0-9 are possible)
    if ((volumeNumber < 0) || (volumeNumber > 9))
    {
        printf("vsPolaris::setTrackingVolume:  Invalid volume number\n");
        return;
    }

    // Create the command string using the given volume parameter
    sprintf(cmdStr, "VSEL:%d", volumeNumber);

    // Get the Polaris's reply
    result = getReply();
    if (result != VS_PL_ERR_NONE)
    {
        printError("setTrackingVolume", "Unable to change tracking volume",
            result);
    }
}

// ------------------------------------------------------------------------
// Changes the state of a visible LED on the given tracking tool
// ------------------------------------------------------------------------
void vsPolaris::setLED(int tracker, int led, int ledState)
{
    char cmdStr[10];
    int result;

    // Validate the tracker number
    if ((tracker < 0) || (tracker >= numTrackers))
    {
        printf("vsPolaris::setLED:  Invalid tracker number specified\n");
        return;
    }

    // Validate the LED number (only 1-3 are acceptable)
    if ((led < 1) || (led > 3))
    {
        printf("vsPolaris::setLED:  Invalid LED number specified\n");
        return;
    }

    // Validate the LED state parameter
    if ((ledState != VS_PL_LED_OFF) &&  (ledState != VS_PL_LED_FLASH) &&
        (ledState != VS_PL_LED_ON))
    {
        printf("vsPolaris::setLED:  Invalid LED state specified\n");
        return;
    }

    // Set up the LED command
    sprintf(cmdStr, "LED:%02X%d%c", portHandle[tracker], led, ledState);
}

// ------------------------------------------------------------------------
// Retrieves the amount of measurement error in the last update for the
// given tracker.  If no tracker exists at the given index, 0.0 is returned
// ------------------------------------------------------------------------
double vsPolaris::getTrackingError(int index)
{
    if ((index < 0) || (index >= numTrackers))
        return 0.0;
    else
        return trackingError[index];
}

// ------------------------------------------------------------------------
// Allows adjustment to the reference coordinate frame of the tracking
// system.  This allows the Polaris' camera unit to be mounted in 
// directions other than straight forward.
// ------------------------------------------------------------------------
void vsPolaris::setReferenceFrame(double h, double p, double r)
{
    // Set up a quaternion representing the given Euler angles
    referenceFrame.setEulerRotation(VS_EULER_ANGLES_ZXY_R, h, p, r);
}

// ------------------------------------------------------------------------
// Resets the Polaris system.  This command works in any mode.
// ------------------------------------------------------------------------
void vsPolaris::resetSystem()
{
    int result;

    // Send a serial break signal to the Polaris to reset it.
    port->sendBreakSignal();

    // Make sure our serial port is at 9600 baud before cheking the 
    // reply, otherwise we won't get the reply properly.
    port->setBaudRate(9600);

    // Wait for the Polaris to finish resetting
    sleep(3);

    // Get the reply
    result = getReply();
    if (result != VS_PL_ERR_NONE)
    {
        // Error condition, print the error message
        printError("resetSystem", "Error resetting Polaris", result);
    }
}

// ------------------------------------------------------------------------
// Returns the number of trackers available
// ------------------------------------------------------------------------
int vsPolaris::getNumTrackers()
{
    return numTrackers;
}

// ------------------------------------------------------------------------
// Returns the requested tracker, if it is available
// ------------------------------------------------------------------------
vsMotionTracker *vsPolaris::getTracker(int index)
{
    // Validate the tracker number
    if ((index < 0) || (index > numTrackers))
    {
        // Invalid tracker, return NULL
        return NULL;
    }

    // Return the requested tracker
    return tracker[index];
}

// ------------------------------------------------------------------------
// Update the motion tracker data, either from the hardware or from shared
// memory
// ------------------------------------------------------------------------
void vsPolaris::update()
{
    int      i;
    vsVector posVec;
    vsQuat   ornQuat;

    // Check to see if we're using a forked server process
    if (forked)
    {
        // Get the latest tracker data from shared memory for all trackers
        for (i = 0; i < numTrackers; i++)
        {
            // Copy the data from shared memory
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
        // Get the data directly from hardware
        updateSystem();
    }

    // Update all input devices
    for (i = 0; i < numTrackers; i++)
    {
        if (tracker[i] != NULL)
            tracker[i]->update();
    }
}
