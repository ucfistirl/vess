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
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "vsAscensionSerialTrackingSystem.h++"

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

#ifdef WIN32
    sprintf(portDevice, "COM%d", portNumber);
#endif

    // Initialize variables
    multiSerial = false;
    forked = false;
    serverThread = NULL;
    serverThreadID = 0;
    configuration = mode;
    addressMode = 0;
    ercAddress = 0;
    numTrackers = 0;
    streaming = false;

    // Initialize all trackers and ports to NULL
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

    // Check to see if the port is valid
    if (port[0])
    {
        // Drop the RTS line to put the flock into FLY mode
        port[0]->setRTS(false);

        // Set the DTR line to make sure the flock knows the host is ready
        port[0]->setDTR(true);

        // Wait for the bird to wake up
        Sleep(1000);

        // Check the configuration flag
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

            // Check to see if we have as many trackers as we should,
            // and print an error message if necessary (a value of zero
            // for the parameter means "use all trackers" so this can't 
            // produce an error).
            if ((numTrackers > nTrackers) && (nTrackers > 0))
            {
                printf("vsAscensionSerialTrackingSystem::"
                    "vsAscensionSerialTrackingSystem:\n");
                printf("   Configuring %d of %d sensors\n", 
                    nTrackers, numTrackers);

                numTrackers = nTrackers;
            }

            // Set the requested data format on all trackers
            setDataFormat(dFormat);

            // Attempt to start the system
            result = initializeFlock();

            // Check the initialization result and print the status
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
        // Couldn't open the serial port
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

    // This constructor shouldn't be used if only one tracker exists
    if (nTrackers > 1)
    {
        // Open a serial port for each tracker
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

#ifdef WIN32
    sprintf(portDevice, "COM%d", portNumbers[i]);
#endif

            port[i] = new vsSerialPort(portDevice, baud, 8, 'N', 1);

            // Drop the RTS line to put the flock into FLY mode
            port[i]->setRTS(false);

            // Set the DTR line to make sure the flock knows the host is ready
            port[i]->setDTR(true);
        }

        // Initialize variables
        multiSerial = true;
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

        // Check to see if we have all the trackers requested, and print
        // an error message if not.
        if (numTrackers < nTrackers)
        {
            printf("vsAscensionSerialTrackingSystem::"
                "vsAscensionSerialTrackingSystem:\n");
            printf("   Incorrect number of sensors specified\n");
        }

        // Also check if we have more trackers than requested, and print
        // a status message if so.
        if (numTrackers > nTrackers) 
        {
            printf("vsAscensionSerialTrackingSystem::"
                "vsAscensionSerialTrackingSystem:\n");
            printf("   Configuring %d of %d sensors\n", nTrackers, numTrackers);

            numTrackers = nTrackers;
        }

        // Set all trackers to the requested data format
        setDataFormat(dFormat);

        // Attempt to start the flock
        result = initializeFlock();

        // Check the initialization result and print a status message.
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
    else
    {
        // Tried to use the multi-serial constructor on a single tracker
        // system
        printf("vsAscensionSerialTrackingSystem::"
            "vsAscensionSerialTrackingSystem:\n");
        printf("   Can't use multi-serial mode on a single tracker system.\n");
    }
}

// ------------------------------------------------------------------------
// Destructs all vsMotionTrackers and puts the flock to sleep
// ------------------------------------------------------------------------
vsAscensionSerialTrackingSystem::~vsAscensionSerialTrackingSystem()
{
    int i;

    // Print status info for the shutdown
    printf("vsAscensionSerialTrackingSystem::"
        "~vsAscensionSerialTrackingSystem:\n");

    // Exit the server thread if we've forked
    if (forked)
    {
        // Set the done flag to true.  The server thread will exit
        // when its done with the current update.
        serverDone = true;
    }

    // Delete motion trackers
    printf("  Deleting vsMotionTrackers\n");
    for (i = 0; i < VS_AS_MAX_TRACKERS; i++)
        if (tracker[i] != NULL)
            delete tracker[i];

    // If we haven't forked a server thread, stop the flock and close 
    // the serial port(s).  The server thread will handle this if we 
    // have forked.
    if (!forked)
    {
        printf("  Putting flock to sleep\n");
        sleepFlock();
        Sleep(100);

        printf("  Closing serial port(s)\n");
        for (i = 0; i < VS_AS_MAX_TRACKERS; i++)
            if (port[i] != NULL)
                delete port[i];
    }
}

// ------------------------------------------------------------------------
// Loop for the server portion of this class when operating in forked mode
// ------------------------------------------------------------------------
DWORD WINAPI vsAscensionSerialTrackingSystem::serverLoop(void *parameter)
{
    vsAscensionSerialTrackingSystem *instance;
    int i;
    
    // Get the tracking system instance from the parameter
    instance = (vsAscensionSerialTrackingSystem *)parameter;

    // Set the flag to indicate we're now "forked" (threaded)
    instance->forked = true;
  
    // Initialize the done flag to false  
    instance->serverDone = false;

    // Start the flock streaming data
    instance->startStream();

    // Continuously update the private tracker data while we're running
    while (!instance->serverDone)
    {
        // Update the hardware
        instance->updateSystem();
    }
    
    // Delete the private motion trackers
    for (i = 0; i < instance->numTrackers; i++)
    {
        delete instance->privateTracker[i];
    }
    
    // Delete the critical section object
    DeleteCriticalSection(&(instance->criticalSection));

    // Shut down the tracking system
    printf("  Putting flock to sleep\n");
    instance->sleepFlock();
    Sleep(100);

    printf("  Closing serial port(s)\n");
    for (i = 0; i < VS_AS_MAX_TRACKERS; i++)
        if (instance->port[i] != NULL)
            delete instance->port[i];

    // Return from the thread (this calls ExitThread() implicitly)
    return 0;
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

    // Print status info as we go
    printf("vsAscensionSerialTrackingSystem::enumerateTrackers:\n");

    // Flush the serial port
    port[0]->flushPort();

    // First, determine the firmware revision and crystal speed, this
    // determines how we request the addressing mode later

    // Ask for the firmware revision
    outBuf[0] = VS_AS_CMD_EXAMINE_VALUE;
    outBuf[1] = VS_AS_VAL_SW_REV;
    port[0]->writePacket(outBuf, 2);

    // Read the result
    result = port[0]->readPacket(inBuf, 2);

    // Print the result if valid
    if (result == 2)
    {
        firmwareMajorRev = inBuf[0];
        firmwareMinorRev = inBuf[1];
        printf("  Master Bird firmware revision:  %d.%d\n", firmwareMajorRev,
            firmwareMinorRev);
    }

    // Ask for the crystal speed
    outBuf[0] = VS_AS_CMD_EXAMINE_VALUE;
    outBuf[1] = VS_AS_VAL_CRYSTAL_SPEED;
    port[0]->writePacket(outBuf, 2);

    // Read the result
    result = port[0]->readPacket(inBuf, 2);
    
    // Print the result if valid
    if (result == 2)
    {
        printf("  Master Bird crystal speed:      %d MHz\n", inBuf[0]);
    }

    // Next, we need to know the model ID of the master bird.  The firmware's
    // major revision number is always one higher for ERC's, so we need to 
    // check whether the master is an ERC or not before we can determine the
    // addressing mode

    // Ask for the model ID
    outBuf[0] = VS_AS_CMD_EXAMINE_VALUE;
    outBuf[1] = VS_AS_VAL_SYSTEM_MODEL_ID;
    port[0]->writePacket(outBuf, 2);

    // Read the result
    result = port[0]->readPacket((unsigned char *)modelID, 10);

    // Print an error if not valid
    if (result != 10)
    {
        printf("  Can't read master bird's model ID\n");
    }
    else
    {
        // Terminate the string we received
        modelID[10] = 0;

        // If the bird is an ERC, then decrement its major revision by one
        if (strcmp(modelID, "6DERC     ") == 0)
            firmwareMajorRev--;
    }

    // Next, determine the addressing mode.  This is complicated because
    // there are two different ways to do this, depending on the firmware
    // revision.

    // Initialize the address mode value
    addressMode = -1;

    // If the firmware revision is >= 3.67, we need to check for 
    // super-expanded address mode
    if ((firmwareMajorRev > 3) || 
        ((firmwareMajorRev == 3) && (firmwareMinorRev >= 67)))
    {
        // Ask for the address mode
        outBuf[0] = VS_AS_CMD_EXAMINE_VALUE;
        outBuf[1] = VS_AS_VAL_ADDRESS_MODE;
        port[0]->writePacket(outBuf, 2);

        // Read the result
        result = port[0]->readPacket(inBuf, 1);
        
        // If we got an answer, print the results or an error message
        // as appropriate
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
                default:
                    printf("  Invalid address mode returned from flock!\n");
                    break;
            }
        }
    }

    // Next, read the status of the master bird (including the address
    // mode if we don't already have it)

    // Ask for the master bird's status
    outBuf[0] = VS_AS_CMD_EXAMINE_VALUE;
    outBuf[1] = VS_AS_VAL_BIRD_STATUS;
    port[0]->writePacket(outBuf, 2);
    
    // Read the result
    result = port[0]->readPacket(inBuf, 2);

    // Examine the results if we got an answer
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

    // Initialize the storage for the answer
    for (i = 0; i < VS_AS_CMD_PACKET_SIZE; i++)
    {
        inBuf[i] = 0;
        statusBuf[i] = 0;
    }

    // Wait three seconds for the hardware to catch up
    Sleep(3000);

    // The length of the response depends on the address mode
    if (addressMode == VS_AS_ADDR_SUPER_EXP)
    {
        // Read 126 bytes for super-expanded mode
        result = port[0]->readPacket(statusBuf, 126);

        // Check the result to make sure it's the correct length
        if (result != 126)
            printf("  Error getting flock status (%d of 126 bytes)\n", result);
    }
    else if (addressMode == VS_AS_ADDR_EXPANDED)
    {
        // Read 30 bytes for expanded mode
        result = port[0]->readPacket(statusBuf, 30);

        // Check the result to make sure it's the correct length
        if (result != 30)
            printf("  Error getting flock status (%d of 30 bytes)\n", result);
    }
    else
    {
        // Read 14 bytes for standard mode
        result = port[0]->readPacket(statusBuf, 14);

        // Check the result to make sure it's the correct length
        if (result != 14)
            printf("  Error getting flock status (%d of 14 bytes)\n", result);
    }

    // Now, we can enumerate the trackers, initialize counters
    i = 1; 
    numTrackers = 0;

    // Examine each tracker until one is found inaccessible.  Sensors must
    // be configured with contiguous FBB addresses (this is a hardware 
    // requirement), so we can safely assume that the first inaccessible
    // tracker means we have found all available trackers.
    // (The high bit of each byte signifies whether or not the corresponding
    // tracker is accessible)
    while (statusBuf[i-1] & 0x80)
    {
        // Ask the bird for its model ID
        data = VS_AS_VAL_SYSTEM_MODEL_ID;
        fbbCommand(i, VS_AS_CMD_EXAMINE_VALUE, &data, 1);

        // Clear the result field
        memset(modelID, 0, 11);

        // Read the result from the appropriate serial port
        if (multiSerial)
            port[i - 1]->readPacket(inBuf, 10);
        else
            port[0]->readPacket(inBuf, 10);

        // Copy the result into the modelID string (this terminates
        // the string as well)
        memcpy(modelID, inBuf, 10);

        // Print the result
        printf("  Bird %d is a %s\n", i, modelID);

        // If the bird is an ERC, keep track of its address
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
  
        // Print the ERC's location on the bus if one is present
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
    bool          errorFlag;
    int           i;

    // Print status as we go
    printf("vsAscensionSerialTrackingSystem::initializeFlock:\n");

    // Stop the system from streaming (if it is)
    ping();

    // Set the maximum FBB address as appropriate (a flock with an ERC
    // will have a maximum address one higher than the number of trackers)
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
    Sleep(1000);

    // Auto-configure the system
    outBuf[0] = VS_AS_CMD_CHANGE_VALUE;
    outBuf[1] = VS_AS_VAL_FBB_AUTOCONFIG;
    outBuf[2] = highAddress;
    printf("  Auto-configuring flock . . .\n");
    port[0]->writePacket(outBuf, 3);

    // Pause again after auto-configuring
    Sleep(2000);

    // Flush the serial port
    port[0]->flushPort();

    // Check all birds for errors
    errorFlag = false;
    for (address = 1; address <= highAddress; address++)
    {
        // We'll have to handle this differently depending on if we're 
        // using multiple serial ports or not
        if (multiSerial)
        {
            // Send the bird status command
            data = VS_AS_VAL_BIRD_STATUS;
            fbbCommand(address, VS_AS_CMD_EXAMINE_VALUE, &data, 1);

            // Read the response from the correct serial port
            port[address - 1]->readPacket(inBuf, 2);

            // If the error flag is set...
            if (inBuf[1] & 0x20)
            {
                // Request the error code
                data = VS_AS_VAL_EXP_ERROR_CODE;
                fbbCommand(address, VS_AS_CMD_EXAMINE_VALUE, &data, 1);

                // Read the result
                port[address - 1]->readPacket(inBuf, 2);
   
                // Print the error
                getErrorString(inBuf[0], inBuf[1]);
                printf("  Bird %d reports an error:\n", address);
                printf("    %s\n", errorString);
    
                // Set the error flag to true
                errorFlag = true;
            }
        }
        else
        {
            // Send the bird status command
            data = VS_AS_VAL_BIRD_STATUS;
            fbbCommand(address, VS_AS_CMD_EXAMINE_VALUE, &data, 1);

            // Read the result
            port[0]->readPacket(inBuf, 2);

            // If the error flag is set...
            if (inBuf[1] & 0x20)
            {
                // Request the error code
                data = VS_AS_VAL_EXP_ERROR_CODE;
                fbbCommand(address, VS_AS_CMD_EXAMINE_VALUE, &data, 1);

                // Read the result
                port[0]->readPacket(inBuf, 2);
   
                // Print the error
                getErrorString(inBuf[0], inBuf[1]);
                printf("  Bird %d reports an error:\n", address);
                printf("    %s\n", errorString);
    
                // Set the error flag to true
                errorFlag = true;
            }
        }
    }

    // Finish initializing if no errors reported
    if (errorFlag == false)
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

        // Initialization successful
        return true;
    }
 
    // Problem with initialization
    return false;
}

// ------------------------------------------------------------------------
// Fills the class data member errorString with a user-readable string
// explaining the given error code
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::getErrorString(unsigned char errorNum, 
                                                     unsigned char errorAddr)
{
    // Convert the given error number (and possibly address information) into
    // a legible error message
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
            // The format of the address field for this error message will
            // vary according to the current addressing mode.  Use the
            // appropriate bit-mask to decipher the address of the bird
            // causing the error.
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

    // If we're in standalone mode, this is easy, we only have one tracker
    // and one possible location it can be
    if (configuration == VS_AS_MODE_STANDALONE)
    {
        // Set up the command and its arguments (if any) into a packet
        outBuf[0] = command;
        if (dataSize > 0)
            memcpy(&outBuf[1], data, dataSize);

        // Write the packet
        port[0]->writePacket(outBuf, dataSize + 1);
    }
    else 
    {
        // Should this go to every tracker?
        if (address == VS_AS_ALL_TRACKERS)
        {

            // Figure out the range of addresses, the high address will
            // be the number of trackers or one larger if an ERC is present
            if (ercAddress == 0)
                highAddress = numTrackers;
            else
                highAddress = numTrackers + 1;

            // Check whether we're using multiple serial ports or not
            if (multiSerial)
            {
                // Set up the command
                outBuf[0] = command;
                if (dataSize > 0)
                    memcpy(&outBuf[1], data, dataSize);

                // Send the basic command to every port
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
                // appropriate RS232 TO FBB command and address (details
                // in the Ascension manual)
                for (address = 1; address <= highAddress; address++)
                {
                    // Skip the ERC address
                    if (address != ercAddress)
                    {
                        // The command has different formats, depending
                        // on the address mode
                        if (addressMode == VS_AS_ADDR_SUPER_EXP)
                        {
                            // Set up the command
                            outBuf[0] = VS_AS_CMD_RS232_TO_FBB_SUP;
                            outBuf[1] = address;
                            outBuf[2] = command;

                            // Copy the data
                            if (dataSize > 0)
                                memcpy(&outBuf[3], data, dataSize);
            
                            // Send it to the flock
                            port[0]->writePacket(outBuf, 3 + dataSize);
                        }
                        else if (addressMode == VS_AS_ADDR_EXPANDED)
                        {
                            // Set up the command (In this mode, format 
                            // further varies according to the FBB address 
                            // of the target)
                            if (address > 15)
                                outBuf[0] = VS_AS_CMD_RS232_TO_FBB_EXP + 
                                    address - 0x10;
                            else
                                outBuf[0] = VS_AS_CMD_RS232_TO_FBB_STD + 
                                    address;
                    
                            outBuf[1] = command;
            
                            // Copy the data
                            if (dataSize > 0)
                                memcpy(&outBuf[2], data, dataSize);
    
                            // Send the command to the flock
                            port[0]->writePacket(outBuf, 2 + dataSize);
                        }
                        else
                        {
                            // Standard address mode; set up the command
                            outBuf[0] = VS_AS_CMD_RS232_TO_FBB_STD + address;
                            outBuf[1] = command;
                
                            // Copy the data
                            if (dataSize > 0)
                                memcpy(&outBuf[2], data, dataSize);
            
                            // Send it to the flock
                            port[0]->writePacket(outBuf, 2 + dataSize);
                        }
                    }
                }
            }
        }
        else
        {
            // Command is just for one tracker (not all trackers)
            // Again, the proper method varies according to serial
            // port configuration
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
                // appropriate RS232 TO FBB command and address.  The 
                // proper format varies according to address mode.
                if (addressMode == VS_AS_ADDR_SUPER_EXP)
                {
                    // Set up the command
                    outBuf[0] = VS_AS_CMD_RS232_TO_FBB_SUP;
                    outBuf[1] = address;
                    outBuf[2] = command;

                    // Copy the data
                    if (dataSize > 0)
                        memcpy(&outBuf[3], data, dataSize);
    
                    // Send to the flock
                    port[0]->writePacket(outBuf, 3 + dataSize);
                }
                else if (addressMode == VS_AS_ADDR_EXPANDED)
                {
                    // Set up the command.  In expanded mode, the format
                    // further varies according to the FBB address of
                    // the target.
                    if (address > 15)
                        outBuf[0] = VS_AS_CMD_RS232_TO_FBB_EXP + 
                            address - 0x10;
                    else
                        outBuf[0] = VS_AS_CMD_RS232_TO_FBB_STD + address;
    
                    // Copy the command and data
                    outBuf[1] = command;
                    if (dataSize > 0)
                        memcpy(&outBuf[2], data, dataSize);
    
                    // Send the command to the flock
                    port[0]->writePacket(outBuf, 2 + dataSize);
                }
                else
                {
                    // Standard address mode; set up the command
                    outBuf[0] = VS_AS_CMD_RS232_TO_FBB_STD + address;
    
                    // Copy the command and data
                    outBuf[1] = command;
                    if (dataSize > 0)
                        memcpy(&outBuf[2], data, dataSize);
    
                    // Send it to the flock
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

    // Update the tracker data.  Use the private tracker data and 
    // synchronize with the critical section if running multi-threaded.
    if (forked)
    {
        EnterCriticalSection(&criticalSection);
        privateTracker[trackerIndex]->setPosition(posVec);
        privateTracker[trackerIndex]->setOrientation(ornQuat);
        LeaveCriticalSection(&criticalSection);
    }
    else
    {
        // Not multithreaded, just update the main tracker data.
        tracker[trackerIndex]->setPosition(posVec);
        tracker[trackerIndex]->setOrientation(ornQuat);
    }
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

    // Clear the position vector to zero
    posVec.setSize(3);
    posVec.clear();

    // Extract the Euler angles
    h = flockData[0] * VS_AS_SCALE_ANGLE;
    p = flockData[1] * VS_AS_SCALE_ANGLE;
    r = flockData[2] * VS_AS_SCALE_ANGLE;

    // Convert orientation to VESS coordinates
    ornQuat.setEulerRotation(VS_EULER_ANGLES_ZYX_R, h, p, r);
    ornQuat = coordXform * ornQuat * coordXform;

    // Update the tracker data.  Use the private tracker data and 
    // synchronize with the critical section if running multi-threaded.
    if (forked)
    {
        EnterCriticalSection(&criticalSection);
        privateTracker[trackerIndex]->setPosition(posVec);
        privateTracker[trackerIndex]->setOrientation(ornQuat);
        LeaveCriticalSection(&criticalSection);
    }
    else
    {
        // Not multithreaded, just update the main tracker data.
        tracker[trackerIndex]->setPosition(posVec);
        tracker[trackerIndex]->setOrientation(ornQuat);
    }
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

    // Clear the position vector to zero
    posVec.setSize(3);
    posVec.clear();

    // Data is a 3x3 matrix stored in column-major order
    ornMat.setIdentity();
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            ornMat[j][i] = flockData[(i * 3) + j] * VS_AS_SCALE_MATRIX;
        }
    }

    // The flock's matrix is the transpose of what VESS expects
    ornMat.transpose();

    // Covert orientation to VESS coordinates
    ornQuat.setMatrixRotation(ornMat);
    ornQuat = coordXform * ornQuat * coordXform;

    // Update the tracker data.  Use the private tracker data and 
    // synchronize with the critical section if running multi-threaded.
    if (forked)
    {
        EnterCriticalSection(&criticalSection);
        privateTracker[trackerIndex]->setPosition(posVec);
        privateTracker[trackerIndex]->setOrientation(ornQuat);
        LeaveCriticalSection(&criticalSection);
    }
    else
    {
        // Not multithreaded, just update the main tracker data.
        tracker[trackerIndex]->setPosition(posVec);
        tracker[trackerIndex]->setOrientation(ornQuat);
    }
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

    // Clear the position vector to zero
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

    // Update the tracker data.  Use the private tracker data and 
    // synchronize with the critical section if running multi-threaded.
    if (forked)
    {
        EnterCriticalSection(&criticalSection);
        privateTracker[trackerIndex]->setPosition(posVec);
        privateTracker[trackerIndex]->setOrientation(ornQuat);
        LeaveCriticalSection(&criticalSection);
    }
    else
    {
        // Not multithreaded, just update the main tracker data.
        tracker[trackerIndex]->setPosition(posVec);
        tracker[trackerIndex]->setOrientation(ornQuat);
    }
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

    // Copy the position data
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

    // Update the tracker data.  Use the private tracker data and 
    // synchronize with the critical section if running multi-threaded.
    if (forked)
    {
        EnterCriticalSection(&criticalSection);
        privateTracker[trackerIndex]->setPosition(posVec);
        privateTracker[trackerIndex]->setOrientation(ornQuat);
        LeaveCriticalSection(&criticalSection);
    }
    else
    {
        // Not multithreaded, just update the main tracker data.
        tracker[trackerIndex]->setPosition(posVec);
        tracker[trackerIndex]->setOrientation(ornQuat);
    }
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

    // Copy the position data
    posVec.setSize(3);
    posVec[VS_X] = flockData[0] * posScale;
    posVec[VS_Y] = flockData[1] * posScale;
    posVec[VS_Z] = flockData[2] * posScale;

    // Convert position to VESS coordinates
    posVec = coordXform.rotatePoint(posVec);

    // Convert the flock's matrix to a vsMatrix
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
    
    // Update the tracker data.  Use the private tracker data and 
    // synchronize with the critical section if running multi-threaded.
    if (forked)
    {
        EnterCriticalSection(&criticalSection);
        privateTracker[trackerIndex]->setPosition(posVec);
        privateTracker[trackerIndex]->setOrientation(ornQuat);
        LeaveCriticalSection(&criticalSection);
    }
    else
    {
        // Not multithreaded, just update the main tracker data.
        tracker[trackerIndex]->setPosition(posVec);
        tracker[trackerIndex]->setOrientation(ornQuat);
    }
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

    // Copy the position data
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

    // Update the tracker data.  Use the private tracker data and 
    // synchronize with the critical section if running multi-threaded.
    if (forked)
    {
        EnterCriticalSection(&criticalSection);
        privateTracker[trackerIndex]->setPosition(posVec);
        privateTracker[trackerIndex]->setOrientation(ornQuat);
        LeaveCriticalSection(&criticalSection);
    }
    else
    {
        // Not multithreaded, just update the main tracker data.
        tracker[trackerIndex]->setPosition(posVec);
        tracker[trackerIndex]->setOrientation(ornQuat);
    }
}

// ------------------------------------------------------------------------
// Request a data packet from the flock.  
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::ping()
{
    unsigned char buf;
    int           trackerNum;

    // Simple one-byte command
    buf = VS_AS_CMD_POINT;

    // Check the serial configuration
    if (multiSerial)
    {
        // Send the ping to each bird
        for (trackerNum = 0; trackerNum < numTrackers; trackerNum++)
        {
            // If the flock contains an ERC, skip the bird with the ERC
            // connected and make sure we send each ping to the correct
            // FBB address
            if (((trackerNum + 1) < ercAddress) || (ercAddress == 0))
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
        // Initialize byte and retry counters
        bytesRead = 0;
        errorRetry = 100;

        // Try up to 100 times to read the appropriate number of bytes
        while ((bytesRead < dataSize) && (errorRetry > 0))
        {
            // Read the serial port
            result = port[0]->readPacket(&buf[bytesRead], 1);
            
            // If we get any data, try to process it
            if (result != 0)
            {
                // If we haven't read any data yet...
                if (bytesRead == 0)
                { 
                    // Check phase bit (high bit of each byte) so we start 
                    // reading at the beginning of a valid data record
                    if (buf[0] & 0x80)
                    {
                        bytesRead++;
                    }
                }
                else
                {
                    // Accumulate the data we've read
                    bytesRead++;
                }
            }
            else
            {
                // Decrement the retry counter
                errorRetry--;
            }
        }

        // Print an error message if we failed to read the correct amount
        // of data
        if (errorRetry <= 0)
        {
            printf("vsAscensionSerialTrackingSystem::updateSystem:\n");
            printf("   Error reading data (%d of %d bytes)\n",
                 bytesRead, dataSize);

            // Flush the input buffer and try again with fresh data
            port[0]->flushPort();
        }
    }
    else
    {
        // We're in single-shot mode

        // Check the serial port configuration
        if (multiSerial)
        {
            // Multi-serial configuration, read a data packet from each bird
            for (trackerNum = 0; trackerNum < numTrackers; trackerNum++)
            {
                // The FBB address for the current tracker will be one greater 
                // than the current tracker number, unless we are reading a 
                // tracker located after the ERC on the FBB, in which case it
                // will be two greater.
                if (((trackerNum + 1) < ercAddress) || (ercAddress == 0))
                    bytesRead = 
                        port[trackerNum + 1]->
                            readPacket(&buf[trackerNum * birdDataSize], 1);
                else
                    bytesRead = 
                        port[trackerNum + 2]->
                            readPacket(&buf[trackerNum * birdDataSize], 1);

                // Print an error if we fail to read the correct amount of
                // data
                if (bytesRead != birdDataSize)
                {
                    printf("vsAscensionSerialTrackingSystem::updateSystem:\n");
                    printf("   Error reading data from Bird %d "
                           "(%d of %d bytes)\n",
                         trackerNum, bytesRead, dataSize);

                    // Flush the input buffer and try again with fresh data
                    port[trackerNum]->flushPort();
                }
            }
        }
        else
        {
            // Read the entire data packet at once
            bytesRead = port[0]->readPacket(buf, dataSize);

            // Print an error if we fail to read the correct amount of data
            if (bytesRead != dataSize)
            {
                printf("vsAscensionSerialTrackingSystem::updateSystem:\n");
                printf("   Error reading data (%d of %d bytes)\n",
                     bytesRead, dataSize);

                // Flush the input buffer and try again with fresh data
                port[0]->flushPort();
            }
    
            // Check the phase bit of the first byte to make sure it is set
            if (!(buf[0] & 0x80))
            {
                printf("vsAscensionSerialTrackingSystem::updateSystem:\n");
                printf("   Error reading data, packet out of phase\n");
                bytesRead = 0;

                // Flush the input buffer and try again with fresh data
                port[0]->flushPort();
            }
        }
    }

    // If we succeeded in receiving the required amount of data, process it
    if (bytesRead == dataSize)
    {
        // Process the data for each tracker
        for (i = 0; i < numTrackers; i++)
        {
            // Figure out the vsMotionTracker array index for this segment of 
            // tracker data
            if (configuration == VS_AS_MODE_FLOCK)
            {
                if (multiSerial)
                {
                    // The tracker index is the same as the serial port index
                    currentTracker = i;
                }
                else
                {
                    // The bird's address is the last byte of each data record
                    currentAddress = 
                        buf[ (i * birdDataSize) + (birdDataSize - 1)];

                    // Translate the address to an index into the tracker 
                    // array.  The index will be one less than the FBB address
                    // or two less if the tracker is located after the ERC
                    // on the bus.
                    if ((currentAddress > ercAddress) && (ercAddress != 0))
                        currentTracker = currentAddress - 2;
                    else
                        currentTracker = currentAddress - 1;
                }
            }
            else
            {
                // In standalone mode, there is only one tracker so the
                // index is zero
                currentTracker = 0;
            }

            // Make sure the tracker index is valid (this is primarily
            // an additional check on the validity of the data)
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
                // Convert the 7-bit data into 8-bit numbers.
                // This method is described in the Ascension documentation.
                for (j = 0; j < birdDataSize - 1; j += 2)
                {
                    lsb = buf[(i * birdDataSize) + j];
                    msb = buf[(i * birdDataSize) + j + 1];
                    lsb = lsb & 0x7F;
                    lsb = lsb << 1;
                    msb = msb << 8;
                    flockData[j/2] = (msb | lsb) << 1;
                }

                // Update the tracker data depending on the current
                // data format
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

    // Request the next update if we're not continuously streaming
    if (!streaming)
    {
        ping();
    }
}

// ------------------------------------------------------------------------
// Spawn a separate (server) thread that continuously reads the device
// and updates the vsMotionTracker data
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::forkTracking()
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
    printf("vsAscensionSerialTrackingSystem::forkTracking:\n");
    printf("    Server Thread ID is %d\n", serverThreadID);
    
    // Set the forked flag to indicate we've started running multithreaded
    forked = true;
}


// ------------------------------------------------------------------------
// Start the flock continuously streaming data.  The flock should be run in
// a separate thread when using this mode.  This command is invalid in a
// multiple serial port configuration.
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::startStream()
{
    unsigned char buf;

    // Ignore this command if we're using multiple serial ports
    if (!multiSerial)
    {
        // Send the stream command
        buf = VS_AS_CMD_STREAM;
        port[0]->writePacket(&buf, 1);

        // Set the stream flag to true
        streaming = true;
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
        // don't bother checking for multiSerial.  Just send the point
        // command
        ping();

        // Set the stream flag to false
        streaming = false;
    }
}

// ------------------------------------------------------------------------
// Change the data format to the one specified
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::setDataFormat(int format)
{
    unsigned char dataCommand;

    // Set the new dataFormat value
    dataFormat = format;

    // Set each bird's data format to the requested format and calculate
    // the new data size
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
            break;
    }

    // Add one to the size for the group mode address byte
    if ((!multiSerial) && (configuration == VS_AS_MODE_FLOCK))
        birdDataSize += 1;

    // Compute the total data size per update
    dataSize = birdDataSize * numTrackers;

    // Set the data format on all trackers
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
    int           address;

    // memcpy() of the hSphere parameter doesn't work on little-endian
    // machines.  To account for this, we set each byte of the hemisphere
    // command explicitly.  See the Ascension hardware documentation for
    // info and parameters for the Hemisphere hardware command.
    switch (hSphere)
    {
        case VS_AS_HSPH_FORWARD:
            buf[0] = 0x00;
            buf[1] = 0x00;
            break;

        case VS_AS_HSPH_AFT:
            buf[0] = 0x00;
            buf[1] = 0x01;
            break;

        case VS_AS_HSPH_UPPER:
            buf[0] = 0x0C;
            buf[1] = 0x01;
            break;

        case VS_AS_HSPH_LOWER:
            buf[0] = 0x0C;
            buf[1] = 0x00;
            break;

        case VS_AS_HSPH_LEFT:
            buf[0] = 0x06;
            buf[1] = 0x01;
            break;

        case VS_AS_HSPH_RIGHT:
            buf[0] = 0x06;
            buf[1] = 0x00;
            break;
    }

    // Send the command (procedure varies based on standalone or flock mode)
    if (configuration == VS_AS_MODE_STANDALONE)
    {
        // Validate the tracker number
        if (trackerNum == 0)
        {
            // Send the command
            address = 0;
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
        // Validate the tracker number
        if (trackerNum < numTrackers)
        {
            // Compute the FBB address.  The address will be one greater
            // than the tracker number, unless the tracker is behind an
            // ERC on the bus, in which case the address will be two greater.
            if (((trackerNum + 1) < ercAddress) || (ercAddress == 0))
                address = trackerNum + 1;
            else
                address = trackerNum + 2;

            // Send the command
            fbbCommand(address, VS_AS_CMD_HEMISPHERE, buf, 2);
        }
        else if (trackerNum == VS_AS_ALL_TRACKERS)
        {
            // Send the command to all trackers
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

    // Set up the reference frame command.  Note that we use the Reference
    // Frame 2 command which requires only the h, p, r orientation of the
    // tracker's reference frame.
    buf[0] = VS_AS_CMD_REF_FRAME2;
   
    // Convert the angles into Ascension-friendly format
    az = (short)(h / VS_AS_SCALE_ANGLE);
    pt = (short)(p / VS_AS_SCALE_ANGLE);
    rl = (short)(r / VS_AS_SCALE_ANGLE);

    // Pack the angles into the proper data format
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

    // Pack the angles into the right data format
    data[0] = (unsigned char)(az & 0x00FF);
    data[1] = (unsigned char)(az >> 8);
    data[2] = (unsigned char)(pt & 0x00FF);
    data[3] = (unsigned char)(pt >> 8);
    data[4] = (unsigned char)(rl & 0x00FF);
    data[5] = (unsigned char)(rl >> 8);

    // Issue the command with the converted data
    if (configuration == VS_AS_MODE_STANDALONE)
    {
        // Validate the tracker number
        if (trackerNum == 0)
        {
            // Send the command
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
        // Validate the tracker number
        if (trackerNum < numTrackers)
        {
            // Compute the FBB address.  The address will be one greater
            // than the tracker number, unless the tracker is behind an
            // ERC on the bus, in which case the address will be two greater.
            if (((trackerNum + 1) < ercAddress) || (ercAddress == 0))
                address = trackerNum + 1;
            else
                address = trackerNum + 2;

            // Send the command
            fbbCommand(address, VS_AS_CMD_ANGLE_ALIGN2, data, 6);
        }
        else if (trackerNum == VS_AS_ALL_TRACKERS)
        {
            // Send the command to all trackers
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

    // Send the sleep command to the master bird
    buf = VS_AS_CMD_SLEEP;
    port[0]->writePacket(&buf, 1);
}

// ------------------------------------------------------------------------
// Start the flock running (does not perform initialization)
// ------------------------------------------------------------------------
void vsAscensionSerialTrackingSystem::runFlock()
{
    unsigned char buf;
   
    // Send the run command to the master bird
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

    // Send the sync command and its argument to the master bird
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

    // Set up the command and pack the arguments as necessary
    buf[0] = VS_AS_CMD_NEXT_XMTR;
    buf[1] = ((address & 0x0F) << 4) | (number & 0x03);

    // Send the command to the master bird
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
    // Validate the tracker number (return NULL if invalid)
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
