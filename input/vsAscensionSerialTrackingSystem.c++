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
//    VESS Module:  vsAscensionSerialTrackingSystem.c++
//
//    Description:  Base class to handle input from Ascension serial-based
//                  Motion Tracking systems that use Ascension's RS-232
//                  command set.  This includes the Flock of Birds and
//                  MotionStar systems.  Do not instantiate this class
//                  directly. Instead, use the vsFlockOfBirds or
//                  vsSerialMotionStar classes.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "vsAscensionSerialTrackingSystem.h++"

// Static class variable for instructing the server (child) process to exit
int vsAscensionSerialTrackingSystem::serverDone;

// ------------------------------------------------------------------------
// Constructs a tracking system on the specified port with the given number
// of FBB devices.  If nTrackers is zero, the class attempts to determine
// the number automatically.
// ------------------------------------------------------------------------
vsAscensionSerialTrackingSystem::vsAscensionSerialTrackingSystem(
    int portNumber, int nTrackers, int dFormat, long baud, int mode)
    : vsTrackingSystem()
{
    char   portDevice[20];
    int    i;
    int    result;
    vsQuat quat1, quat2, xformQuat;

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
    multiSerial = VS_FALSE;
    forked = VS_FALSE;
    serverPID = 0;
    configuration = mode;
    addressMode = 0;
    ercAddress = 0;
    numTrackers = 0;
    streaming = VS_FALSE;

    for (i = 0; i < VS_AS_MAX_TRACKERS; i++)
    {
        tracker[i] = NULL;
        port[i] = NULL;
    }

    // Set up a coordinate conversion quaternion that converts from native
    // Ascension coordinates to VESS coordinates
    quat1.setAxisAngleRotation(0, 0, 1, 90);
    quat2.setAxisAngleRotation(0, 1, 0, 180);
    coordXform = quat2 * quat1;

    // Open serial port
    port[0] = new vsSerialPort(portDevice, baud, 8, 'N', 1);

    if (port[0])
    {
        if (configuration == VS_AS_MODE_STANDALONE)
        {
            // Standalone configuration, tracker number is 0
            // no initialization needed
            numTrackers = 1;
            tracker[0] = new vsMotionTracker(0);

            setDataFormat(dFormat);
        }
        else
        {
            // Get the system configuration from the master bird and
            // create motion trackers
            enumerateTrackers();
             
            // Check the number of expected trackers with the number
            // found
            if (numTrackers < nTrackers)
            {
                printf("vsAscensionSerialTrackingSystem::"
                    "vsAscensionSerialTrackingSystem:\n");
                printf("    WARNING -- Only %d sensors found, expecting %d\n",
                    numTrackers, nTrackers);
            }

            if ((numTrackers > nTrackers) && (nTrackers > 0))
            {
                printf("vsAscensionSerialTrackingSystem::"
                    "vsAscensionSerialTrackingSystem:\n");
                printf("   Configuring %d of %d sensors\n", 
                    nTrackers, numTrackers);

                numTrackers = nTrackers;
            }

            setDataFormat(dFormat);

            // Attempt to start the system
            result = initializeFlock();

            if (result != 0)
            {
                printf("vsAscensionSerialTrackingSystem::"
                    "vsAscensionSerialTrackingSystem:\n");
                printf("   Flock running on %s with %d sensors\n", 
                    portDevice, numTrackers);
            }
            else
            {
                printf("vsAscensionSerialTrackingSystem::"
                    "vsAscensionSerialTrackingSystem:\n");
                printf("   Flock did not initialize properly.\n");
            }
        }
    }
    else
    {
        printf("vsAscensionSerialTrackingSystem::"
            "vsAscensionSerialTrackingSystem:\n");
        printf("   Unable to open serial port %s", portDevice);
    }
}

// ------------------------------------------------------------------------
// Constructs a tracking system on the specified ports with the given 
// number of trackers.  The nTrackers parameter must be correctly specified 
// (a value of zero is not valid in multi-serial configurations).
// ------------------------------------------------------------------------
vsAscensionSerialTrackingSystem::vsAscensionSerialTrackingSystem(
    int portNumbers[], int nTrackers, int dFormat, long baud)
    : vsTrackingSystem()
{
    char   portDevice[20];
    int    result;
    int    i;
    vsQuat quat1, quat2, xformQuat;

    if (nTrackers > 1)
    {
        for (i = 0; i < nTrackers; i++)
        {

#ifdef IRIX
            sprintf(portDevice, "/dev/ttyd%d", portNumbers[i]);
#endif

#ifdef IRIX64
            sprintf(portDevice, "/dev/ttyd%d", portNumbers[i]);
#endif

#ifdef __linux__
            sprintf(portDevice, "/dev/ttyS%d", portNumbers[i] - 1);
#endif

            port[i] = new vsSerialPort(portDevice, baud, 8, 'N', 1);
        }

        // Initialize variables
        multiSerial = VS_TRUE;
        configuration = VS_AS_MODE_FLOCK;
        addressMode = 0;
        ercAddress = 0;
        numTrackers = 0;

        // Set up the coordinate conversion matrix that converts from native
        // Ascension coordinates to VESS coordinates
        quat1.setAxisAngleRotation(0, 0, 1, 90);
        quat2.setAxisAngleRotation(0, 1, 0, 180);
        coordXform = quat2 * quat1;

        // Get the system configuration from the first bird and create
        // the motion trackers
        enumerateTrackers();

        if (numTrackers < nTrackers)
        {
            printf("vsAscensionSerialTrackingSystem::"
                "vsAscensionSerialTrackingSystem:\n");
            printf("   Incorrect number of sensors specified\n");
        }

        if (numTrackers > nTrackers) 
        {
            printf("vsAscensionSerialTrackingSystem::"
                "vsAscensionSerialTrackingSystem:\n");
            printf("   Configuring %d of %d sensors\n", nTrackers, numTrackers);

            numTrackers = nTrackers;
        }

        setDataFormat(dFormat);

        // Attempt to start the flock
        result = initializeFlock();

        if (result != 0)
        {
            printf("vsAscensionSerialTrackingSystem::"
                "vsAscensionSerialTrackingSystem:\n");
            printf("   System running on multiple ports with %d sensors\n",
                numTrackers);
        }
        else
        {
            printf("vsAscensionSerialTrackingSystem::"
                "vsAscensionSerialTrackingSystem:\n");
            printf("   System did not initialize properly.\n");
        }
    }
}

// ------------------------------------------------------------------------
// Destructs all vsMotionTrackers and puts the flock to sleep
// ------------------------------------------------------------------------
vsAscensionSerialTrackingSystem::~vsAscensionSerialTrackingSystem()
{
    int i;

    printf("vsAscensionSerialTrackingSystem::"
        "~vsAscensionSerialTrackingSystem:\n");

    // Kill the server process if we've forked
    if (forked)
    {
        printf("  Notifying server process to quit\n");
        kill(serverPID, SIGUSR1);

        delete sharedData;
    }

    // Delete motion trackers
    printf("  Deleting vsMotionTrackers\n");
    for (i = 0; i < VS_AS_MAX_TRACKERS; i++)
        if (tracker[i] != NULL)
            delete tracker[i];

    // Stop the flock and close the serial port(s)
    if (!forked)
    {
        printf("  Putting flock to sleep\n");
        sleepFlock();
        usleep(100000);

        printf("  Closing serial port(s)\n");
        for (i = 0; i < VS_AS_MAX_TRACKERS; i++)
            if (port[i] != NULL)
                delete port[i];
    }
}

// ------------------------------------------------------------------------
// Loop for the server portion of this class when operating in forked mode
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::serverLoop()
{
    int      i;
    vsVector posVec;
    vsQuat   ornQuat;

    // Set up the signal handler
    signal(SIGUSR1, vsAscensionSerialTrackingSystem::quitServer);

    vsAscensionSerialTrackingSystem::serverDone = VS_FALSE;

    // Start the flock streaming data
    startStream();

    posVec.setSize(3);
    posVec.clear();
    ornQuat.clear();

    // Continuously update the shared data while we're running
    while (!vsAscensionSerialTrackingSystem::serverDone)
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

    // Remove the shared memory segment
    delete sharedData;

    // Shut down the tracking system
    printf("  Putting flock to sleep\n");
    sleepFlock();
    usleep(100000);

    printf("  Closing serial port(s)\n");
    for (i = 0; i < VS_AS_MAX_TRACKERS; i++)
        if (port[i] != NULL)
            delete port[i];

    exit(0);
}

// ------------------------------------------------------------------------
// Signal handler for the server process
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::quitServer(int arg)
{
    vsAscensionSerialTrackingSystem::serverDone = VS_TRUE;
}

// ------------------------------------------------------------------------
// Requests the Flock system status from the master bird, then constructs
// a vsMotionTracker for every bird with a contiguous address starting at
// 1.  Each bird with a sensor is enumerated with indices starting at 0,
// matching the tracker[] array. The function sets the instance variable
// numTrackers to the number of trackers available.
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::enumerateTrackers()
{
    int           i;
    int           result;
    unsigned char inBuf[VS_AS_CMD_PACKET_SIZE];
    unsigned char outBuf[VS_AS_CMD_PACKET_SIZE];
    unsigned char data;
    unsigned char statusBuf[VS_AS_CMD_PACKET_SIZE];
    char          modelID[11];
    int           firmwareMajorRev, firmwareMinorRev;
   

    printf("vsAscensionSerialTrackingSystem::enumerateTrackers:\n");

    port[0]->flushPort();

    // First, determine the firmware revision and crystal speed
    outBuf[0] = VS_AS_CMD_EXAMINE_VALUE;
    outBuf[1] = VS_AS_VAL_SW_REV;
    
    port[0]->writePacket(outBuf, 2);
    result = port[0]->readPacket(inBuf, 2);

    if (result == 2)
    {
        firmwareMajorRev = inBuf[0];
        firmwareMinorRev = inBuf[1];
        printf("  Master Bird firmware revision:  %d.%d\n", firmwareMajorRev,
            firmwareMinorRev);
    }

    outBuf[0] = VS_AS_CMD_EXAMINE_VALUE;
    outBuf[1] = VS_AS_VAL_CRYSTAL_SPEED;
    
    port[0]->writePacket(outBuf, 2);
    result = port[0]->readPacket(inBuf, 2);
    
    if (result == 2)
    {
        printf("  Master Bird crystal speed:      %d MHz\n", inBuf[0]);
    }

    // Next, we need to know the model ID of the master bird.  The firmware's
    // major revision number is always one higher for ERC's, so we need to 
    // check whether the master is an ERC or not before we can determine the
    // addressing mode
    outBuf[0] = VS_AS_CMD_EXAMINE_VALUE;
    outBuf[1] = VS_AS_VAL_SYSTEM_MODEL_ID;

    port[0]->writePacket(outBuf, 2);
    result = port[0]->readPacket((unsigned char *)modelID, 10);

    if (result != 10)
    {
        printf("  Can't read master bird's model ID\n");
    }
    else
    {
        modelID[10] = 0;

        if (strcmp(modelID, "6DERC     ") == 0)
            firmwareMajorRev--;
    }

    addressMode = -1;

    // Next, determine the addressing mode.  This is complicated because
    // there are two different ways to do this, depending on the firmware
    // revision.

    // If the firmware revision is >= 3.67, we need to check for 
    // super-expanded address mode
    if ((firmwareMajorRev > 3) || 
        ((firmwareMajorRev == 3) && (firmwareMinorRev >= 67)))
    {
        outBuf[0] = VS_AS_CMD_EXAMINE_VALUE;
        outBuf[1] = VS_AS_VAL_ADDRESS_MODE;

        result = port[0]->readPacket(inBuf, 1);
        
        if (result == 1)
        { 
            addressMode = inBuf[0] - '0';
            switch(addressMode)
            {
                case VS_AS_ADDR_STANDARD:
                    printf("  Flock running in normal address mode\n");
                    break;
                case VS_AS_ADDR_EXPANDED:
                    printf("  Flock running in expanded address mode\n");
                    break;
                case VS_AS_ADDR_SUPER_EXP:
                    printf("  Flock running in super-expanded address mode\n");
                    break;
            }
        }
    }

    outBuf[0] = VS_AS_CMD_EXAMINE_VALUE;
    outBuf[1] = VS_AS_VAL_BIRD_STATUS;

    port[0]->writePacket(outBuf, 2);
    
    result = port[0]->readPacket(inBuf, 2);

    if (result == 2)
    {
        // If the flock is running, put it back to sleep
        if (inBuf[1] & 0x10)
        {
            outBuf[0] = VS_AS_CMD_SLEEP;
            port[0]->writePacket(outBuf, 1);
        }

        // Read the address mode if we haven't already
        if (addressMode == -1)
        {
            if (inBuf[1] & 0x04)
            {
                // Expanded address mode
                printf("  Flock running in expanded address mode\n");

                addressMode = VS_AS_ADDR_EXPANDED;
            }
            else
            {
                // Normal address mode
                printf("  Flock running in normal address mode\n");

                addressMode = VS_AS_ADDR_STANDARD;
            }
        }
    }
    else
    {
        printf("  Error reading master bird status (%d of 2 bytes)\n", result);
    }

    // Next, ask the master for the system status
    outBuf[0] = VS_AS_CMD_EXAMINE_VALUE;
    outBuf[1] = VS_AS_VAL_FLOCK_STATUS;

    result = port[0]->writePacket(outBuf, 2);
    for (i = 0; i < VS_AS_CMD_PACKET_SIZE; i++)
    {
        inBuf[i] = 0;
    }

    if (addressMode == VS_AS_ADDR_SUPER_EXP)
    {
        result = port[0]->readPacket(statusBuf, 126);

        if (result != 126)
            printf("  Error getting flock status (%d of 126 bytes)\n", result);
    }
    else if (addressMode == VS_AS_ADDR_EXPANDED)
    {
        result = port[0]->readPacket(statusBuf, 30);

        if (result != 30)
            printf("  Error getting flock status (%d of 30 bytes)\n", result);
    }
    else
    {
        result = port[0]->readPacket(statusBuf, 14);

        if (result != 14)
            printf("  Error getting flock status (%d of 14 bytes)\n", result);
    }

    // Examine each tracker until one is found inaccessible.  Sensors must
    // be configured with continuous FBB addresses; this is a hardware 
    // requirement.
    i = 1; 
    numTrackers = 0;
    while (statusBuf[i-1] & 0x80)
    {
        // Ask the bird for its model ID
        data = VS_AS_VAL_SYSTEM_MODEL_ID;
        fbbCommand(i, VS_AS_CMD_EXAMINE_VALUE, &data, 1);

        memset(modelID, 0, 11);

        if (multiSerial)
            port[i - 1]->readPacket(inBuf, 10);
        else
            port[0]->readPacket(inBuf, 10);

        memcpy(modelID, inBuf, 10);

        printf("  Bird %d is a %s\n", i, modelID);

        if (strcmp(modelID, "6DERC     ") == 0)
        {
            // Only one ERC allowed
            if (ercAddress != 0)
            {
                printf("  WARNING -- Multiple ERC's not supported\n");
            }

            ercAddress = i;
        }
        
        // Construct a tracker if this is a bird with a receiver
        // (i.e.: not an ERC)
        if ((strcmp(modelID, "6DFOB     ") == 0) || 
            (strcmp(modelID, "6DBOF     ") == 0) || 
            (strcmp(modelID, "MOTIONSTAR") == 0) ||
            (strcmp(modelID, "WIRELESS  ") == 0) ||
            (strcmp(modelID, "PCBIRD    ") == 0))
        {
            tracker[numTrackers] = new vsMotionTracker(numTrackers);
            numTrackers++;
        }

        i++;
    }

    // Finally, report the status
    if (numTrackers > 0)
    {
        printf("  Flock has %d available sensors, ", numTrackers);
  
        if (ercAddress != 0)
        {
            printf("ERC is at address %d\n", ercAddress);
            posScale = VS_AS_SCALE_ERT_POS;
        }
        else
        {
            printf("no ERC present\n");
            posScale = VS_AS_SCALE_SRT1_POS;
        }
    }
}

// ------------------------------------------------------------------------
// Initializes the tracking system with the current configuration 
// information.  Each bird is checked for errors after the flock is 
// initialized.  Any error conditions are reported.  The function returns
// non-zero if the flock is initialized without errors.
// ------------------------------------------------------------------------
int vsAscensionSerialTrackingSystem::initializeFlock()
{
    int           highAddress;
    unsigned char outBuf[VS_AS_CMD_PACKET_SIZE];
    unsigned char inBuf[VS_AS_CMD_PACKET_SIZE];
    unsigned char data;
    int           address;
    int           errorFlag;
    int           i;

    printf("vsAscensionSerialTrackingSystem::initializeFlock:\n");

    // Stop the system from streaming (if it is)
    ping();

    if (ercAddress == 0)
        highAddress = numTrackers;
    else
        highAddress = numTrackers + 1;

    // Place the system in group mode if we're not using a serial port
    // for each bird
    if (!multiSerial)
    {
        printf("  Enabling group mode\n");
        outBuf[0] = VS_AS_CMD_CHANGE_VALUE;
        outBuf[1] = VS_AS_VAL_GROUP_MODE;
        outBuf[2] = 1;

        port[0]->writePacket(outBuf, 3);
    }

    // Pause before sending the auto-configure command
    sleep(1);

    // Auto-configure the system
    outBuf[0] = VS_AS_CMD_CHANGE_VALUE;
    outBuf[1] = VS_AS_VAL_FBB_AUTOCONFIG;
    outBuf[2] = highAddress;

    printf("  Auto-configuring flock . . .\n");
    port[0]->writePacket(outBuf, 3);

    // Pause again after auto-configuring
    sleep(2);

    // Flush the serial port
    port[0]->flushPort();

    // Check all birds for errors
    errorFlag = VS_FALSE;

    for (address = 1; address <= highAddress; address++)
    {
        if (multiSerial)
        {
            data = VS_AS_VAL_BIRD_STATUS;
            fbbCommand(address, VS_AS_CMD_EXAMINE_VALUE, &data, 1);

            port[address - 1]->readPacket(inBuf, 2);

            if (inBuf[1] & 0x20)
            {
                // Get the error code
                data = VS_AS_VAL_EXP_ERROR_CODE;
                fbbCommand(address, VS_AS_CMD_EXAMINE_VALUE, &data, 1);

                port[address - 1]->readPacket(inBuf, 2);
   
                getErrorString(inBuf[0], inBuf[1]);
                printf("  Bird %d reports an error:\n", address);
                printf("    %s\n", errorString);
    
                errorFlag = VS_TRUE;
            }
        }
        else
        {
            data = VS_AS_VAL_BIRD_STATUS;
            fbbCommand(address, VS_AS_CMD_EXAMINE_VALUE, &data, 1);

            port[0]->readPacket(inBuf, 2);

            if (inBuf[1] & 0x20)
            {
                // Get the error code
                data = VS_AS_VAL_EXP_ERROR_CODE;
                fbbCommand(address, VS_AS_CMD_EXAMINE_VALUE, &data, 1);

                port[0]->readPacket(inBuf, 2);
   
                getErrorString(inBuf[0], inBuf[1]);
                printf("  Bird %d reports an error:\n", address);
                printf("    %s\n", errorString);
    
                errorFlag = VS_TRUE;
            }
        }
    }

    if (errorFlag == VS_FALSE)
    {
        printf("  Flock initialized\n");

        // Flush the serial port(s)
        if (multiSerial)
        {
            for (i = 0; i < numTrackers; i++)
            {
                if (port[i])
                    port[i]->flushPort();
            }
        }
        else
            port[0]->flushPort();

        // Ping the system to get the first data report ready
        ping();

        return 1;
    }
 
    return 0;
}

// ------------------------------------------------------------------------
// Fills the class data member errorString with a user-readable string
// explaining the given error code
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::getErrorString(unsigned char errorNum, 
                                                     unsigned char errorAddr)
{
    switch (errorNum)
    {
        case 0:
            sprintf(errorString, "No error");
            break;
        case 1:
            sprintf(errorString, "System RAM failure");
            break;
        case 2:
            sprintf(errorString, "Non-volatile storage write failure");
            break;
        case 3:
            sprintf(errorString, "PCB configuration data corrupt");
            break;
        case 4:
            sprintf(errorString, "Bird transmitter calibration data corrupt or"
                "not connected");
            break;
        case 5:
            sprintf(errorString, "Bird receiver calibration data corrupt or"
                "not connected");
            break;
        case 6:
            sprintf(errorString, "Invalid RS232 command");
            break;
        case 7:
            sprintf(errorString, "Not an FBB master");
            break;
        case 8:
            sprintf(errorString, "No birds accessible in device list");
            break;
        case 9:
            sprintf(errorString, "Bird is not initialized");
            break;
        case 10:
            sprintf(errorString, "FBB serial port receive error - "
                "intra bird bus");
            break;
        case 11:
            sprintf(errorString, "RS232 serial port receive error");
            break;
        case 12:
            sprintf(errorString, "FBB serial port receive error - "
                "FBB host bus");
            break;
        case 13:
            if (addressMode == VS_AS_ADDR_SUPER_EXP)
                errorAddr = (errorAddr & 0x7F);
            else if (addressMode == VS_AS_ADDR_EXPANDED)
                errorAddr = (errorAddr & 0x1F);
            else
                errorAddr = (errorAddr & 0x0F);

            sprintf(errorString, "No FBB command response - bird %d", 
                errorAddr);
            break;
        case 14:
            sprintf(errorString, "Invalid FBB host command");
            break;
        case 15:
            sprintf(errorString, "FBB run time error");
            break;
        case 16:
            sprintf(errorString, "Invalid CPU speed");
            break;
        case 17:
            sprintf(errorString, "No FBB data");
            break;
        case 18:
            sprintf(errorString, "Illegal baud rate");
            break;
        case 19:
            sprintf(errorString, "Slave acknowledge error");
            break;
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
            sprintf(errorString, "Intel 80186 CPU error - #%d", errorNum);
            break;
        case 28:
            sprintf(errorString, "CRT synchronization error");
            break;
        case 29:
            sprintf(errorString, "Transmitter not accessible");
            break;
        case 30:
            sprintf(errorString, "ERT not attached");
            break;
        case 31:
            sprintf(errorString, "CPU time overflow");
            break;
        case 32:
            sprintf(errorString, "Receiver saturated");
            break;
        case 33:
            sprintf(errorString, "Slave configuration error");
            break;
        case 34:
            sprintf(errorString, "Watch dog timer error");
            break;
        case 35:
            sprintf(errorString, "Over temperature");
            break;
    }
}

// ------------------------------------------------------------------------
// Sends the given command to the specified FBB device using the 
// RS232 TO FBB command (see the RS232 Command Reference in the Ascension
// documentation for details).
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::fbbCommand(int address,
                                                 unsigned char command, 
                                                 unsigned char data[],
                                                 int dataSize)
{
    unsigned char outBuf[VS_AS_CMD_PACKET_SIZE];
    int           highAddress;

    // If we're in standalone mode, this is easy
    if (configuration == VS_AS_MODE_STANDALONE)
    {
        outBuf[0] = command;

        if (dataSize > 0)
            memcpy(&outBuf[1], data, dataSize);

        port[0]->writePacket(outBuf, dataSize + 1);
    }
    else 
    {
        // Should this go to every tracker?
        if (address == VS_AS_ALL_TRACKERS)
        {

            // Figure out the range of addresses
            if (ercAddress == 0)
                highAddress = numTrackers;
            else
                highAddress = numTrackers + 1;

            if (multiSerial)
            {
                // Send the basic command to every port
                outBuf[0] = command;

                if (dataSize > 0)
                    memcpy(&outBuf[1], data, dataSize);

                for (address = 1; address <= highAddress; address++)
                {
                    // Skip the ERC port
                    if (address != ercAddress)
                    {
                        port[address - 1]->writePacket(outBuf, dataSize + 1);
                    }
                }
            }
            else
            {
                // Send the given command to the master, prefixed by the 
                // appropriate RS232 TO FBB command and address

                for (address = 1; address <= highAddress; address++)
                {
                    if (address != ercAddress)
                    {
                        if (addressMode == VS_AS_ADDR_SUPER_EXP)
                        {
                            outBuf[0] = VS_AS_CMD_RS232_TO_FBB_SUP;
                            outBuf[1] = address;
                            outBuf[2] = command;
            
                            if (dataSize > 0)
                                memcpy(&outBuf[3], data, dataSize);
            
                            port[0]->writePacket(outBuf, 3 + dataSize);
                        }
                        else if (addressMode == VS_AS_ADDR_EXPANDED)
                        {
                            if (address > 15)
                                outBuf[0] = VS_AS_CMD_RS232_TO_FBB_EXP + 
                                    address - 0x10;
                            else
                                outBuf[0] = VS_AS_CMD_RS232_TO_FBB_STD + 
                                    address;
                    
                            outBuf[1] = command;
            
                            if (dataSize > 0)
                                memcpy(&outBuf[2], data, dataSize);
    
                            port[0]->writePacket(outBuf, 2 + dataSize);
                        }
                        else
                        {
                            outBuf[0] = VS_AS_CMD_RS232_TO_FBB_STD + address;
            
                            outBuf[1] = command;
                
                            if (dataSize > 0)
                                memcpy(&outBuf[2], data, dataSize);
            
                            port[0]->writePacket(outBuf, 2 + dataSize);
                        }
                    }
                }
            }
        }
        else
        {
            // Just one tracker

            if (multiSerial)
            {
                // Send the given command to the right port
                outBuf[0] = command;
            
                if (dataSize > 0)
                    memcpy(&outBuf[1], data, dataSize);
    
                    port[address - 1]->writePacket(outBuf, 1 + dataSize);
            }
            else
            {

                // Send the given command to the master, prefixed by the
                // appropriate RS232 TO FBB command and address

                if (addressMode == VS_AS_ADDR_SUPER_EXP)
                {
                    outBuf[0] = VS_AS_CMD_RS232_TO_FBB_SUP;
                    outBuf[1] = address;
                    outBuf[2] = command;

                    if (dataSize > 0)
                        memcpy(&outBuf[3], data, dataSize);
    
                    port[0]->writePacket(outBuf, 3 + dataSize);
                }
                else if (addressMode == VS_AS_ADDR_EXPANDED)
                {
                    if (address > 15)
                        outBuf[0] = VS_AS_CMD_RS232_TO_FBB_EXP + 
                            address - 0x10;
                    else
                        outBuf[0] = VS_AS_CMD_RS232_TO_FBB_STD + address;
    
                    outBuf[1] = command;
    
                    if (dataSize > 0)
                        memcpy(&outBuf[2], data, dataSize);
    
                    port[0]->writePacket(outBuf, 2 + dataSize);
                }
                else
                {
                    outBuf[0] = VS_AS_CMD_RS232_TO_FBB_STD + address;
    
                    outBuf[1] = command;
        
                    if (dataSize > 0)
                        memcpy(&outBuf[2], data, dataSize);
    
                    port[0]->writePacket(outBuf, 2 + dataSize);
                }
            }
        }
    }
}

// ------------------------------------------------------------------------
// Interprets the flock data as a position record and sets the given
// tracker's data accordingly.  Orientation is set to zero.
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::updatePosition(int trackerIndex, 
                                                     short flockData[])
{
    vsVector posVec;
    vsQuat   ornQuat;

    // Extract the position
    posVec.setSize(3);
    posVec[VS_X] = flockData[0] * posScale;
    posVec[VS_Y] = flockData[1] * posScale;
    posVec[VS_Z] = flockData[2] * posScale;

    // Transform the position to the VESS coordinate system
    posVec = coordXform.rotatePoint(posVec);

    ornQuat.setAxisAngleRotation(0.0, 0.0, 0.0, 1.0);

    // Update the tracker data
    tracker[trackerIndex]->setPosition(posVec);
    tracker[trackerIndex]->setOrientation(ornQuat);
}

// ------------------------------------------------------------------------
// Interprets the flock data as an Euler angle record (heading, pitch,roll)
// and sets the given tracker's data accordingly.  Position is set to zero.
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::updateAngles(int trackerIndex, 
                                                   short flockData[])
{
    vsVector posVec;
    double   h, p, r;
    vsQuat   ornQuat;

    posVec.setSize(3);
    posVec.clear();

    // Extract the Euler angles
    h = flockData[0] * VS_AS_SCALE_ANGLE;
    p = flockData[1] * VS_AS_SCALE_ANGLE;
    r = flockData[2] * VS_AS_SCALE_ANGLE;

    // Convert orientation to VESS coordinates
    ornQuat.setEulerRotation(VS_EULER_ANGLES_ZYX_R, h, p, r);
    ornQuat = coordXform * ornQuat * coordXform;

    // Update the tracker data
    tracker[trackerIndex]->setPosition(posVec);
    tracker[trackerIndex]->setOrientation(ornQuat);
}

// ------------------------------------------------------------------------
// Interprets the flock data as a rotation matrix record and sets the given
// tracker's data accordingly.  Position is set to zero
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::updateMatrix(int trackerIndex, 
                                                   short flockData[])
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
            ornMat[j][i] = flockData[(i * 4) + j] * VS_AS_SCALE_MATRIX;
        }
    }

    // The flock's matrix is the transpose of what VESS expects
    ornMat.transpose();

    // Covert orientation to VESS coordinates
    ornQuat.setMatrixRotation(ornMat);
    ornQuat = coordXform * ornQuat * coordXform;

    // Update the tracker data
    tracker[trackerIndex]->setPosition(posVec);
    tracker[trackerIndex]->setOrientation(ornQuat);
}

// ------------------------------------------------------------------------
// Interprets the flock data as a quaternion record and sets the given
// tracker's data accordingly.  Position is set to zero
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::updateQuaternion(int trackerIndex, 
                                                       short flockData[])
{
    vsVector posVec;
    vsQuat   ornQuat;

    posVec.setSize(3);
    posVec.clear();

    // Quaternion returned by the Flock has the scalar portion in front
    ornQuat[VS_X] = flockData[1] * VS_AS_SCALE_QUAT;
    ornQuat[VS_Y] = flockData[2] * VS_AS_SCALE_QUAT;
    ornQuat[VS_Z] = flockData[3] * VS_AS_SCALE_QUAT;
    ornQuat[VS_W] = flockData[0] * VS_AS_SCALE_QUAT;

    // The flock's quaternion is the conjugate of what VESS expects
    ornQuat.conjugate();

    // Convert orientation to VESS coordinates
    ornQuat = coordXform * ornQuat * coordXform;

    // Update the tracker data
    tracker[trackerIndex]->setPosition(posVec);
    tracker[trackerIndex]->setOrientation(ornQuat);
}

// ------------------------------------------------------------------------
// Interprets the flock data as a position/angles record and sets the given
// tracker's data accordingly. 
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::updatePosAngles(int trackerIndex, 
                                                      short flockData[])
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

    // Extract the Euler angles
    h = flockData[3] * VS_AS_SCALE_ANGLE;
    p = flockData[4] * VS_AS_SCALE_ANGLE;
    r = flockData[5] * VS_AS_SCALE_ANGLE;

    // Convert orientation to VESS coordinates
    ornQuat.setEulerRotation(VS_EULER_ANGLES_ZYX_R, h, p, r);
    ornQuat = coordXform * ornQuat * coordXform;

    // Update the tracker data
    tracker[trackerIndex]->setPosition(posVec);
    tracker[trackerIndex]->setOrientation(ornQuat);
}

// ------------------------------------------------------------------------
// Interprets the flock data as a position/matrix record and sets the given
// tracker's data accordingly. 
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::updatePosMatrix(int trackerIndex, 
                                                      short flockData[])
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
            ornMat[j][i] = flockData[(i * 3) + j + 3] * VS_AS_SCALE_MATRIX;
        }
    }

    // The flock's matrix is the transpose of what VESS expects
    ornMat.transpose();

    // Convert orientation to VESS coordinates
    ornQuat.setMatrixRotation(ornMat);
    ornQuat = coordXform * ornQuat * coordXform;
    
    // Update the tracker data
    tracker[trackerIndex]->setPosition(posVec);
    tracker[trackerIndex]->setOrientation(ornQuat);
}

// ------------------------------------------------------------------------
// Interprets the flock data as a position record and sets the given
// tracker's data accordingly.  Orientation is set to zero
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::updatePosQuat(int trackerIndex, 
                                                    short flockData[])
{
    vsVector posVec;
    vsQuat   ornQuat;

    posVec.setSize(3);
    posVec[VS_X] = flockData[0] * posScale;
    posVec[VS_Y] = flockData[1] * posScale;
    posVec[VS_Z] = flockData[2] * posScale;

    // Convert position to VESS coordinates
    posVec = coordXform.rotatePoint(posVec);

    ornQuat[VS_X] = flockData[4] * VS_AS_SCALE_QUAT;
    ornQuat[VS_Y] = flockData[5] * VS_AS_SCALE_QUAT;
    ornQuat[VS_Z] = flockData[6] * VS_AS_SCALE_QUAT;
    ornQuat[VS_W] = flockData[3] * VS_AS_SCALE_QUAT;

    // The flock's quaternion is the conjugate of what VESS expects
    ornQuat.conjugate();

    // Convert orientation to VESS coordinates
    ornQuat = coordXform * ornQuat * coordXform;

    // Update the tracker data
    tracker[trackerIndex]->setPosition(posVec);
    tracker[trackerIndex]->setOrientation(ornQuat);
}

// ------------------------------------------------------------------------
// Request a data packet from the flock.  
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::ping()
{
    unsigned char buf;
    int           trackerNum;

    buf = VS_AS_CMD_POINT;

    if (multiSerial)
    {
        // Send the ping to each bird
        for (trackerNum = 0; trackerNum < numTrackers; trackerNum++)
        {
            if ((trackerNum + 1 < ercAddress) || (ercAddress == 0))
                port[trackerNum + 1]->writePacket(&buf, 1);
            else
                port[trackerNum + 2]->writePacket(&buf, 1);
        }
    }
    else
        // Send the ping to the master bird
        port[0]->writePacket(&buf, 1);
}

// ------------------------------------------------------------------------
// Update the motion tracker data with fresh data from the flock
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::updateSystem()
{
    unsigned char buf[VS_AS_DATA_PACKET_SIZE];
    short         flockData[VS_AS_DATA_POS_MATRIX_SIZE];
    int           trackerNum;
    int           result;
    int           bytesRead;
    int           errorRetry;
    int           i, j;
    int           currentAddress;
    int           currentTracker;
    short         lsb, msb;
    
    // If we're streaming, read the data one byte at a time
    if (streaming)
    {
        bytesRead = 0;
        errorRetry = 100;
        while ((bytesRead < dataSize) && (errorRetry > 0))
        {
            result = port[0]->readPacket(&buf[bytesRead], 1);
            
            if (result != 0)
            {
                // Check phase bit so we start at the beginning
                // of a data record
                if (bytesRead == 0)
                { 
                    if (buf[0] & 0x80)
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
            printf("vsAscensionSerialTrackingSystem::updateSystem:\n");
            printf("   Error reading data (%d of %d bytes)\n",
                 bytesRead, dataSize);

            port[0]->flushPort();
        }
    }
    else
    {
        if (multiSerial)
        {
            // Read a data packet from each bird
            for (trackerNum = 0; trackerNum < numTrackers; trackerNum++)
            {
                if ((trackerNum + 1 < ercAddress) || (ercAddress == 0))
                    bytesRead = 
                        port[trackerNum + 1]->
                        readPacket(&buf[trackerNum * birdDataSize], 1);
                else
                    bytesRead = 
                        port[trackerNum + 2]->
                        readPacket(&buf[trackerNum * birdDataSize], 1);

                if (bytesRead != birdDataSize)
                {
                    printf("vsAscensionSerialTrackingSystem::updateSystem:\n");
                    printf("   Error reading data from Bird %d "
                           "(%d of %d bytes)\n",
                         trackerNum, bytesRead, dataSize);

                    port[trackerNum]->flushPort();
                }
            }
        }
        else
        {
            // Read the entire data packet at once
            bytesRead = port[0]->readPacket(buf, dataSize);

            if (bytesRead != dataSize)
            {
                printf("vsAscensionSerialTrackingSystem::updateSystem:\n");
                printf("   Error reading data (%d of %d bytes)\n",
                     bytesRead, dataSize);
                port[0]->flushPort();
            }
    
            if (!(buf[0] & 0x80))
            {
                printf("vsAscensionSerialTrackingSystem::updateSystem:\n");
                printf("   Error reading data, packet out of phase\n");
                bytesRead = 0;
                port[0]->flushPort();
            }
        }
    }

    if (bytesRead == dataSize)
    {
        for (i = 0; i < numTrackers; i++)
        {
            if (configuration == VS_AS_MODE_FLOCK)
            {
                if (multiSerial)
                {
                    currentTracker = i;
                }
                else
                {
                    // The bird's address is the last byte of each data record
                    currentAddress = 
                        buf[ (i * birdDataSize) + (birdDataSize - 1)];

                    // Translate the address to an index into the tracker array
                    if (currentAddress > ercAddress)
                        currentTracker = currentAddress - 2;
                    else
                        currentTracker = currentAddress - 1;
                }
            }
            else
            {
                currentTracker = 0;
            }

            if ((currentTracker >= numTrackers) || 
                (currentAddress < 0) || 
                ((currentAddress == 0) && 
                    (configuration == VS_AS_MODE_FLOCK)) ||
                (tracker[currentTracker] == NULL))
            {
                printf("vsAscensionSerialTrackingSystem::updateSystem:\n");
                printf("   Data received for an invalid tracker\n");
                printf("   FBB Address:  %d   numTrackers:  %d\n",
                    currentAddress, numTrackers);
            }
            else
            {
                // Convert the 7-bit data into 8-bit numbers
                // See the Ascension documentation for details
                for (j = 0; j < birdDataSize - 1; j += 2)
                {
                    lsb = buf[(i * birdDataSize) + j];
                    msb = buf[(i * birdDataSize) + j + 1];
                    lsb = lsb & 0x7F;
                    lsb = lsb << 1;
                    msb = msb << 8;
                    flockData[j/2] = (msb | lsb) << 1;
                }
    
                switch (dataFormat)
                {
                    case VS_AS_DATA_POSITION:
                        updatePosition(currentTracker, flockData);
                        break;
                    case VS_AS_DATA_ANGLES:
                        updateAngles(currentTracker, flockData);
                        break;
                    case VS_AS_DATA_MATRIX:
                        updateMatrix(currentTracker, flockData);
                        break;
                    case VS_AS_DATA_QUATERNION:
                        updateQuaternion(currentTracker, flockData);
                        break;
                    case VS_AS_DATA_POS_ANGLES: 
                        updatePosAngles(currentTracker, flockData);
                        break;
                    case VS_AS_DATA_POS_MATRIX:
                        updatePosMatrix(currentTracker, flockData);
                        break;
                    case VS_AS_DATA_POS_QUAT:
                        updatePosQuat(currentTracker, flockData);
                        break;
                }
            }
        }
    }

    if (!streaming)
    {
        ping();
    }
}

// ------------------------------------------------------------------------
// Spawn a separate (server) process that continuously reads the device
// and updates the vsMotionTracker data
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::forkTracking()
{
    key_t  theKey;
    time_t tod;

    // Use a portion of the time of day for the second half of the 
    // shared memory key
    tod = time(NULL);
    tod &= 0x0000FFFF;

    theKey = VS_AS_SHM_KEY_BASE | tod;

    serverPID = fork();

    switch(serverPID)
    {
        case -1:
            printf("vsAscensionSerialTrackingSystem::forkTracking:\n");
            printf("    fork() failed, continuing in single-process mode\n");
            break;
        case 0:
            sharedData = new vsSharedInputData(theKey, numTrackers, VS_TRUE);
            serverLoop();
            break;
        default:
            sharedData = new vsSharedInputData(theKey, numTrackers, VS_FALSE);
            forked = VS_TRUE;
            printf("vsAscensionSerialTrackingSystem::forkTracking:\n");
            printf("    Server PID is %d\n", serverPID);
    }
}


// ------------------------------------------------------------------------
// Start the flock continuously streaming data.  The flock should be run in
// a separate process when using this mode.  This command is invalid in a
// multiple serial port configuration.
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::startStream()
{
    unsigned char buf;

    // Ignore this command if we're using multiple serial ports
    if (!multiSerial)
    {
        buf = VS_AS_CMD_STREAM;
        port[0]->writePacket(&buf, 1);

        streaming = VS_TRUE;
    }
}

// ------------------------------------------------------------------------
// Stop the flock from streaming data.  
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::stopStream()
{
    if (streaming)
    {
        // If we're streaming, we must be using a single serial port, so
        // don't bother checking for multiSerial
        ping();

        streaming = VS_FALSE;
    }
}

// ------------------------------------------------------------------------
// Change the data format to the one specified
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::setDataFormat(int format)
{
    unsigned char dataCommand;

    dataFormat = format;

    // Set each bird's data format to the requested format
    // Add one to the size for the group mode address byte
    switch (dataFormat)
    {
        case VS_AS_DATA_POSITION:
            dataCommand = VS_AS_CMD_POSITION;
            birdDataSize = VS_AS_DATA_POSITION_SIZE;
            printf("  Setting data format to POSITION\n");
            break;
        case VS_AS_DATA_ANGLES:
            dataCommand = VS_AS_CMD_ANGLES;
            birdDataSize = VS_AS_DATA_ANGLES_SIZE;
            printf("  Setting data format to ANGLES\n");
            break;
        case VS_AS_DATA_MATRIX:
            dataCommand = VS_AS_CMD_MATRIX;
            birdDataSize = VS_AS_DATA_MATRIX_SIZE;
            printf("  Setting data format to MATRIX\n");
            break;
        case VS_AS_DATA_QUATERNION:
            dataCommand = VS_AS_CMD_QUATERNION;
            birdDataSize = VS_AS_DATA_QUATERNION_SIZE;
            printf("  Setting data format to QUATERNION\n");
            break;
        case VS_AS_DATA_POS_ANGLES:
            dataCommand = VS_AS_CMD_POS_ANGLES;
            birdDataSize = VS_AS_DATA_POS_ANGLES_SIZE;
            printf("  Setting data format to POS_ANGLES\n");
            break;
        case VS_AS_DATA_POS_MATRIX:
            dataCommand = VS_AS_CMD_POS_MATRIX;
            birdDataSize = VS_AS_DATA_POS_MATRIX_SIZE;
            printf("  Setting data format to POS_MATRIX\n");
            break;
        case VS_AS_DATA_POS_QUAT:
            dataCommand = VS_AS_CMD_POS_QUAT;
            birdDataSize = VS_AS_DATA_POS_QUAT_SIZE;
            printf("  Setting data format to POS_QUAT\n");
            break;
        default:
            printf("   Invalid data format %d, assuming POS_QUAT\n", 
                dataFormat);
            dataFormat = VS_AS_DATA_POS_QUAT;
            birdDataSize = VS_AS_DATA_POS_QUAT_SIZE;
            dataCommand = VS_AS_CMD_POS_QUAT;
    }

    if ((!multiSerial) && (configuration == VS_AS_MODE_FLOCK))
        birdDataSize += 1;

    // Compute the total data size per update
    dataSize = birdDataSize * numTrackers;

    fbbCommand(VS_AS_ALL_TRACKERS, dataCommand, NULL, 0);
}

// ------------------------------------------------------------------------
// Change the transmitter hemisphere in which the sensors are located.
// Use one of the VS_AS_HSPH_X constants defined in the header file as the 
// value for the hSphere parameter.
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::setActiveHemisphere(int trackerNum, 
                                                          short hSphere)
{
    unsigned char buf[3];
    short         hemisphere;
    int           address;

    hemisphere = hSphere;

    if (configuration == VS_AS_MODE_STANDALONE)
    {
        if (trackerNum == 0)
        {
            address = 0;
            memcpy(buf, &hemisphere, 2);
            fbbCommand(address, VS_AS_CMD_HEMISPHERE, buf, 2);
        }
        else 
        {
            printf("vsAscensionSerialTrackingSystem::setActiveHemisphere:\n");
            printf("    Invalid tracker number specified\n");
        }
    }
    else
    {
        if (trackerNum < numTrackers)
        {
            if ((trackerNum + 1) < ercAddress)
                address = trackerNum + 1;
            else
                address = trackerNum + 2;

            memcpy(buf, &hemisphere, 2);

            fbbCommand(address, VS_AS_CMD_HEMISPHERE, buf, 2);
        }
        else if (trackerNum == VS_AS_ALL_TRACKERS)
        {
            memcpy(buf, &hemisphere, 2);

            fbbCommand(VS_AS_ALL_TRACKERS, VS_AS_CMD_HEMISPHERE, buf, 2);
        }
        else 
        {
            printf("vsAscensionSerialTrackingSystem::setActiveHemisphere:\n");
            printf("    Invalid tracker number specified\n");
        }
    }
}

// ------------------------------------------------------------------------
// Change the reference frame of the transmitter.  Subsequent orientation 
// measurements will be offset by the amount specified.
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::setReferenceFrame(float h, float p, 
                                                        float r)
{
    unsigned char buf[7];
    short         az, pt, rl;

    buf[0] = VS_AS_CMD_REF_FRAME2;
   
    // Convert the angles into Ascension-friendly format
    az = (short)(h / VS_AS_SCALE_ANGLE);
    pt = (short)(p / VS_AS_SCALE_ANGLE);
    rl = (short)(r / VS_AS_SCALE_ANGLE);

    buf[1] = (unsigned char)(az & 0x00FF);
    buf[2] = (unsigned char)(az >> 8);
    buf[3] = (unsigned char)(pt & 0x00FF);
    buf[4] = (unsigned char)(pt >> 8);
    buf[5] = (unsigned char)(rl & 0x00FF);
    buf[6] = (unsigned char)(rl >> 8);

    // Issue the command with the converted data
    port[0]->writePacket(buf, 7);
}

// ------------------------------------------------------------------------
// Adjust the angle alignment of the given tracker.  The angle alignment
// offsets the orientation of the tracker by the specified amount
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::setAngleAlignment(int trackerNum, 
                                                        float h, float p,
                                                        float r)
{
    unsigned char data[6];
    int           address;
    short         az, pt, rl;

    // Convert the angles into Ascension-friendly format
    az = (short)(h / VS_AS_SCALE_ANGLE);
    pt = (short)(p / VS_AS_SCALE_ANGLE);
    rl = (short)(r / VS_AS_SCALE_ANGLE);

    data[0] = (unsigned char)(az & 0x00FF);
    data[1] = (unsigned char)(az >> 8);
    data[2] = (unsigned char)(pt & 0x00FF);
    data[3] = (unsigned char)(pt >> 8);
    data[4] = (unsigned char)(rl & 0x00FF);
    data[5] = (unsigned char)(rl >> 8);


    // Issue the command with the converted data
    if (configuration == VS_AS_MODE_STANDALONE)
    {
        if (trackerNum == 0)
        {
            address = 0;

            fbbCommand(address, VS_AS_CMD_ANGLE_ALIGN2, data, 6);
        }
        else 
        {
            printf("vsAscensionSerialTrackingSystem::setAngleAlignment:\n");
            printf("    Invalid tracker number specified\n");
        }
    }
    else
    {
        if (trackerNum < numTrackers)
        {
            if ((trackerNum + 1) < ercAddress)
                address = trackerNum + 1;
            else
                address = trackerNum + 2;

            fbbCommand(address, VS_AS_CMD_ANGLE_ALIGN2, data, 6);
        }
        else if (trackerNum == VS_AS_ALL_TRACKERS)
        {
            fbbCommand(VS_AS_ALL_TRACKERS, VS_AS_CMD_ANGLE_ALIGN2, data, 6);
        }
        else 
        {
            printf("vsAscensionSerialTrackingSystem::setAngleAlignment:\n");
            printf("    Invalid tracker number specified\n");
        }
    }
}

// ------------------------------------------------------------------------
// Put the flock to sleep
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::sleepFlock()
{
    unsigned char buf;

    buf = VS_AS_CMD_SLEEP;

    port[0]->writePacket(&buf, 1);
}

// ------------------------------------------------------------------------
// Start the flock running (does not perform initialization)
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::runFlock()
{
    unsigned char buf;
   
    buf = VS_AS_CMD_RUN;

    port[0]->writePacket(&buf, 1);
}

// ------------------------------------------------------------------------
// Changes the synchronization mode (CRT sync pickup or TTL pulse input 
// must be connected to the master bird).  CRT sync pickups should be 
// positioned and calibrated with other software (e.g.: Cbird from 
// Ascension)
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::setSyncMode(int syncType)
{
    unsigned char buf[2];

    buf[0] = VS_AS_CMD_SYNC;
    buf[1] = syncType;

    port[0]->writePacket(buf, 2);
}

// ------------------------------------------------------------------------
// Changes the active transmitter to the transmitter specified.  The 
// specified transmitter must be connected, or the command will be ignored.
// 
// Valid values for address are 1-14, valid values for number are 0-3.
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::setTransmitter(int address, int number)
{
    unsigned char buf[2];

    buf[0] = VS_AS_CMD_NEXT_XMTR;
    buf[1] = ((address & 0x0F) << 4) | (number & 0x03);

    port[0]->writePacket(buf, 2);
}

// ------------------------------------------------------------------------
// Return the number of trackers currently running
// ------------------------------------------------------------------------
int vsAscensionSerialTrackingSystem::getNumTrackers()
{
    return numTrackers;
}

// ------------------------------------------------------------------------
// Return the tracker at the specified index (if it exists)
// ------------------------------------------------------------------------
vsMotionTracker *vsAscensionSerialTrackingSystem::getTracker(int index)
{
    if (index < numTrackers)
        return tracker[index];
    else
        return NULL;
}

// ------------------------------------------------------------------------
// Update the motion tracker data, either from the hardware or from shared 
// memory
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::update()
{
    int      i;
    vsVector posVec;
    vsQuat   ornQuat;

    if (forked)
    {
        // Copy the data from shared memory
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
        // Get the data directly from hardware
        updateSystem();
    }
}
