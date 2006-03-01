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
//    VESS Module:  vsIntersenseTrackingSystem.h++
//
//    Description:  Generic driver for all Intersense tracking systems.
//                  Uses the API provided by Intersense to communicate
//                  with hardware.  No threading is carried out in this
//                  class because the InterSense library handles all the
//                  buffering for us.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#include "vsIntersenseTrackingSystem.h++"
#include "isense.h"
#include <stdio.h>
#include <string.h>

#ifndef _MSC_VER
    #include <unistd.h>
#endif

// ------------------------------------------------------------------------
// Constructor.  Creates and initializes the vsIntersenseTrackingSystem
// object.  The "portNumber" parameter typically refers to either a
// serial port, or UDP network port (when receiving UDP broadcast traffic).
// However, because of the way the InterSense API works, if you have a file
// called isports.ini in the local directory, the port numbers defined in
// that file will be used instead.  This may be necessary if your 
// InterSense box is connected via a USB->Serial converter on Linux 
// (/dev/ttyUSB0), for example.  This is also necessary in order to use a
// TCP connection with the Ethernet option of the IS-900.
//
// Unfortunately, there is no way to allow the user to directly specify a 
// port device string with the InterSense library.
// ------------------------------------------------------------------------
vsIntersenseTrackingSystem::vsIntersenseTrackingSystem(int portNumber)
{
    int i;
    vsQuat quat1, quat2;

    // Clear all of our status structures first
    systemHandle = 0;
    memset(&systemConfig, 0, sizeof(systemConfig));
    memset(trackerConfig, 0, sizeof(trackerConfig));
    memset(&systemInfo, 0, sizeof(systemInfo));
    memset(trackerInfo, 0, sizeof(trackerConfig));
    memset(tracker, 0, sizeof(tracker));
    memset(joystick, 0, sizeof(joystick));
    numTrackers = 0;
    valid = false;
    port = portNumber;

    // Set up quaternions for transforming Intersense coordinates into VESS
    // coordinates
    quat1.setAxisAngleRotation(0, 0, 1, 90);
    quat2.setAxisAngleRotation(0, 1, 0, 180);
    coordXform = quat2 * quat1;

    // Initialize the tracker to station index and station to tracker index
    // maps
    for (i = 0; i <= ISD_MAX_STATIONS; i++)
        trackerToStation[i] = -1;
    for (i = 0; i < VS_ITS_MAX_TRACKERS + 1; i++)
        stationToTracker[i] = -1;

    // Open the connection to the tracking system
    systemHandle = ISD_OpenTracker(0, portNumber, 0, 0);

    // Bail if we didn't connect to the tracking system.  A systemHandle
    // of -1 indicates an error returned by ISD_OpenTracker.  A systemHandle
    // of 0 is a special handle in the InterSense API, indicating all available
    // trackers.  So, anything less than zero indicates something bad
    // probably happened.
    if (systemHandle < 0)
    {
        printf("vsIntersenseTrackingSystem::vsIntersenseTrackingSystem:\n");
        printf("    Unable to connect to the tracking system!\n");

        return;
    }

    // Try to configure the system
    if (configureSystem())
    {
        // Configuration succeeded, so enumerate the trackers (stations) 
        // attached to the tracking system and set up the required data
        // structures to handle their data
        enumerateTrackers();

        // See if we set up any trackers
        if (numTrackers > 0)
        {
            printf("vsIntersenseTrackingSystem::vsIntersenseTrackingSystem:\n");
            printf("    %d tracker(s) found.\n", numTrackers);
        }
        else
        {
            printf("vsIntersenseTrackingSystem::vsIntersenseTrackingSystem:\n");
            printf("    WARNING: No trackers found.  No data will be "
                "available.\n");
        }

        // Mark the object as valid, so the rest of the function calls will
        // work
        valid = true;
    }
}

// ------------------------------------------------------------------------
// Destructor.  Disconnects from the hardware and cleans up allocated
// resources.
// ------------------------------------------------------------------------
vsIntersenseTrackingSystem::~vsIntersenseTrackingSystem()
{
}

// ------------------------------------------------------------------------
// Obtains the current configuration and sets the system up for use
// ------------------------------------------------------------------------
bool vsIntersenseTrackingSystem::configureSystem()
{
    // Get the system configuration
    ISD_GetTrackerConfig(systemHandle, &systemConfig, 0);

    // Show the configuration process
    printf("vsIntersenseTrackingSystem::configureSystem:\n");

    // Print out the InterSense library version
    printf("  Intersense Library v.%0.2lf\n", systemConfig.LibVersion);

    // Figure out which series this Intersense system is in
    if (systemConfig.TrackerType == ISD_PRECISION_SERIES)
    {
        printf("  System is an InterSense Precision Series ");
    }
    else if (systemConfig.TrackerType == ISD_INTERTRAX_SERIES)
    {
        printf("  System is an InterSense InterTrax Series ");
    }
    else
    {
        printf("vsIntersenseTrackingSystem::vsIntersenseTrackingsystem:\n");
        printf("    InterSense device not initialized properly!\n");
        return false;
    }

    // Figure out the model
    switch (systemConfig.TrackerModel)
    {
        case ISD_IS300:
            printf("(IS-300)\n");
            break;
        case ISD_IS600:
            printf("(IS-900)\n");
            break;
        case ISD_IS900:
            printf("(IS-900)\n");
            break;
        case ISD_IS1200:
            printf("(IS-1200)\n");
            break;
        case ISD_INTERTRAX:
            printf("(InterTrax)\n");
            break;
        case ISD_INTERTRAX_2:
            printf("(InterTrax 2)\n");
            break;
        case ISD_INTERTRAX_LS:
            printf("(InterTraxLS)\n");
            break;
        case ISD_INTERTRAX_LC:
            printf("(InterTraxLC)\n");
            break;
        case ISD_INTERTRAX_3:
            printf("(InterTrax3)\n");
            break;
        case ISD_ICUBE2:
            printf("(InertiaCube2)\n");
            break;
        case ISD_ICUBE2_PRO:
            printf("(InertiaCube2 Pro)\n");
            break;
        case ISD_ICUBE3:
            printf("(InertiaCube3)\n");
            break;
        case ISD_ICUBE4:
            printf("(InertiaCube4)\n");
            break;
        default:
            printf("(Unknown)\n");
            printf("  WARNING:  Unknown Tracker Model.  Results may be "
                "unreliable.\n");
            break;
    };

    // Report the type of interface in use
    printf("  Connected via port #%d ", port);
    switch (systemConfig.Interface)
    {
        case ISD_INTERFACE_SERIAL:
            printf("(serial port)\n");
            break;
        case ISD_INTERFACE_USB:
            printf("(USB port)\n");
            break;
        case ISD_INTERFACE_ETHERNET_UDP:
            printf("(UDP)\n");
            break;
        case ISD_INTERFACE_ETHERNET_TCP:
            printf("(TCP)\n");
            break;
        case ISD_INTERFACE_IOCARD:
            printf("(IO card)\n");
            break;
        case ISD_INTERFACE_PCMCIA:
            printf("(PCMCIA)\n");
            break;
        case ISD_INTERFACE_FILE:
            printf("(file)\n");
            break;
        default:
            printf("(unknown interface!)\n");
            printf("  WARNING:  Unknown interface method.  Results may be "
                "unreliable.\n");
            break;
    };

    // Get the system hardware info, so we know the system's capabilities
    if ((!ISD_GetSystemHardwareInfo(systemHandle, &systemInfo)) ||
        (!systemInfo.Valid))
    {
        printf("  WARNING:  Unable to retrieve system capabilities\n");
        return false;
    }

    // If we got this far, we should have a working configuration, so return
    // true
    return true;
}

// ------------------------------------------------------------------------
// Checks the given station number for analog (joystick) controls and 
// buttons, and creates
// ------------------------------------------------------------------------
void vsIntersenseTrackingSystem::configureJoystick(int trackerNum)
{
    ISD_STATION_HARDWARE_INFO_TYPE stationHwInfo;
    int stationNum;
    int numAxes, numButtons;
    ISD_STATION_INFO_TYPE stationInfo;

    // Get the station number we're working with
    stationNum = trackerToStation[trackerNum];

    // Get this stations hardware and capabilities.  If this call fails, 
    // just assume we don't have any and move on
    if (ISD_GetStationHardwareInfo(systemHandle,
            &stationHwInfo, stationNum))
    {
        // Get the number of analog axes and buttons supported
        numAxes = stationHwInfo.Capability.NumChannels;
        numButtons = stationHwInfo.Capability.NumButtons;

        // See if there are any axes and/or buttons supported
        if ((numAxes > 0) || (numButtons > 0))
        {
            // Create a vsJoystick with this tracker to
            // handle the analog channels and buttons
            joystick[trackerNum] = 
                new vsJoystick(numAxes, numButtons, -32768, 32767);

            // Try to get the information structure for this station
            if (ISD_GetStationConfig(systemHandle, &stationInfo, stationNum, 0))
            {
                // Alter the structure to indicate that we want the joystick
                // and button data
                stationInfo.GetInputs = 1;

                // Send the new configuration to the system
                ISD_SetStationConfig(systemHandle, &stationInfo, stationNum, 0);
            }
            else
            {
                // Report that there was a problem configuring the joystick
                // controls
                printf("vsIntersenseTrackingSystem::configureJoystick:\n");
                printf("    Unable to configure joystick controls on "
                    "tracker %d\n", trackerNum);
            }
        }
    }
}

// ------------------------------------------------------------------------
// Counts the trackers attached to the tracking system, and creates a
// vsMotionTracker for each active tracker.  Active stations are given
// tracker numbers in increasing order, starting at zero.
// ------------------------------------------------------------------------
void vsIntersenseTrackingSystem::enumerateTrackers()
{
    int stationNum;
    vsTimer dataTimer;

    // Collect tracker data for 5 seconds at regular intervals to allow 
    // the InterSense library to gather data on which trackers are available
    printf("  Collecting station data...\n");
    while (dataTimer.getElapsed() < 5.0)
    {
        // Call update() to force the tracking library to collect data from
        // the tracker
        update();
        usleep(100000);
    }

    // If this is an InterTrax device, we know there is only one active
    // station (if we try to get the station configuration from an
    // InterTrax, it will fail)
    if (systemConfig.TrackerType == ISD_INTERTRAX_SERIES)
    {
        // Set up the system for a single tracker
        numTrackers = 1;
        tracker[0] = new vsMotionTracker(0);
        trackerToStation[0] = 1;
        stationToTracker[1] = 0;
    }
    else if (systemConfig.TrackerType == ISD_PRECISION_SERIES)
    {
        // Test each station to see if it's active
        for (stationNum = 1; 
            stationNum <= systemInfo.Capability.MaxStations; 
            stationNum++)
        {
            // Try to get the configuration of this station
            if (ISD_GetStationConfig(systemHandle, 
                    &(trackerConfig[numTrackers]), stationNum, false))
            {
                // If the station is active, create a tracker for it
                if (trackerConfig[numTrackers].State)
                {
                    printf("  Configuring station %d\n", stationNum);
                    tracker[numTrackers] = 
                        new vsMotionTracker(numTrackers);
                    trackerToStation[numTrackers] = stationNum;
                    stationToTracker[stationNum] = numTrackers;
                
                    // Check to see if this station includes analog and/or
                    // button controls, and set them up if necessary
                    configureJoystick(numTrackers);

                    // Increment the number of trackers configured
                    numTrackers++;
                }
                else
                {
                    // We failed to get a configuration for this station, so 
                    // just assume it's not there
                    printf("  Station %d is not a valid station\n", stationNum);
                }
            }
        }
    }
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsIntersenseTrackingSystem::getClassName()
{
    return "vsIntersenseTrackingSystem";
}

// ------------------------------------------------------------------------
// Return the number of active trackers on this system
// ------------------------------------------------------------------------
int vsIntersenseTrackingSystem::getNumTrackers()
{
    // Return the number of active trackers if this object is valid, zero
    // otherwise
    if (valid)
        return numTrackers;
    else
        return 0;
}

// ------------------------------------------------------------------------
// Return the requested motion tracker object
// ------------------------------------------------------------------------
vsMotionTracker *vsIntersenseTrackingSystem::getTracker(int index)
{
    // Bail (returning NULL) if this object is not in a valid state
    if (!valid)
        return NULL;

    // Check the index to see if it specifies a valid tracker.  Return the
    // tracker if so, or NULL if not.
    if ((index < 0) || (index >= numTrackers))
        return NULL;
    else
        return tracker[index];
}

// ------------------------------------------------------------------------
// Return whether or not the given motion tracker has additional analog
// or button controls associated with it (in the form of a vsJoystick 
// object)
// ------------------------------------------------------------------------
bool vsIntersenseTrackingSystem::hasJoystick(int index)
{
    // Bail if this object is not in a valid state
    if (!valid)
        return false;

    // Check the index to see if it specifies a valid tracker.  Return NULL
    // if it does not.  If it does, return whether or not the associated 
    // joystick object exists.
    if ((index < 0) || (index >= numTrackers))
        return false;
    else
        return (joystick[index] != NULL);
}

// ------------------------------------------------------------------------
// Return the joystick object associated with the requested motion tracker
// ------------------------------------------------------------------------
vsJoystick *vsIntersenseTrackingSystem::getJoystick(int index)
{
    // Bail (returning NULL) if this object is not in a valid state
    if (!valid)
        return NULL;

    // Check the index to see if it specifies a valid tracker.  Return the
    // tracker's associated joystick object if so, or NULL if not.
    if ((index < 0) || (index >= numTrackers))
        return NULL;
    else
        return joystick[index];
}

// ------------------------------------------------------------------------
// Adjusts the orientation reports of the given tracker to be relative
// to the specified Euler angles.  Note that for early InterTrax models,
// this function will simply reset the heading value to zero.
// ------------------------------------------------------------------------
void vsIntersenseTrackingSystem::setAngleAlignment(int tracker, float h,
                                                   float p, float r)
{
    // Bail (returning NULL) if this object is not in a valid state
    if (!valid)
        return;

    // Check the index to see if it specifies a valid tracker.  Bail if
    // it does not
    if ((tracker < 0) || (tracker >= numTrackers))
        return;

    // Call the library function to set the current angle alignment for
    // this tracker's associated station number
    ISD_BoresightReferenced(systemHandle, trackerToStation[tracker], h, p, r);
}

// ------------------------------------------------------------------------
// Adjusts the orientation reports of the given tracker to be relative
// to the current orientation.  Note that for early InterTrax models,
// this function will simply reset the heading value to zero.
// ------------------------------------------------------------------------
void vsIntersenseTrackingSystem::setAngleAlignment(int tracker)
{
    // Bail (returning NULL) if this object is not in a valid state
    if (!valid)
        return;

    // Check the index to see if it specifies a valid tracker.  Bail if
    // it does not
    if ((tracker < 0) || (tracker >= numTrackers))
        return;

    // Call the library function to set the current angle alignment for
    // this tracker's associated station number
    ISD_Boresight(systemHandle, trackerToStation[tracker], 1);
}

// ------------------------------------------------------------------------
// Clears any previously set angle alignments.  Returns the tracker to
// reporting it's native orientation.
// ------------------------------------------------------------------------
void vsIntersenseTrackingSystem::clearAngleAlignment(int tracker)
{
    // Bail (returning NULL) if this object is not in a valid state
    if (!valid)
        return;

    // Check the index to see if it specifies a valid tracker.  Bail if
    // it does not
    if ((tracker < 0) || (tracker >= numTrackers))
        return;

    // Call the library function to clear the current angle alignment for
    // this tracker's associated station number
    ISD_Boresight(systemHandle, trackerToStation[tracker], 0);
}

// ------------------------------------------------------------------------
// For IS-900 models, enables the LED's on the SoniStrips to provide a
// visual reference of what transducers are active.
// ------------------------------------------------------------------------
void vsIntersenseTrackingSystem::enableLEDs()
{
    // Bail out if this object is invalid or is not associated with an 
    // IS-900 system
    if ((!valid) || (systemConfig.TrackerModel != ISD_IS900))
        return;

    // Don't bother enabling the LED's if they're already enabled
    if (systemConfig.LedEnable)
        return;

    // Adjust the system configuration and send the new configuration to
    // the system
    systemConfig.LedEnable = 1;
    ISD_SetTrackerConfig(systemHandle, &systemConfig, 0);
}

// ------------------------------------------------------------------------
// For IS-900 models, disables the LED's on the SoniStrips.
// ------------------------------------------------------------------------
void vsIntersenseTrackingSystem::disableLEDs()
{
    // Bail out if this object is invalid or is not associated with an 
    // IS-900 system
    if ((!valid) || (systemConfig.TrackerModel != ISD_IS900))
        return;

    // Don't bother disabling the LED's if they're already disabled
    if (!systemConfig.LedEnable)
        return;

    // Adjust the system configuration and send the new configuration to
    // the system
    systemConfig.LedEnable = 0;
    ISD_SetTrackerConfig(systemHandle, &systemConfig, 0);
}

// ------------------------------------------------------------------------
// Retrieves fresh data from the InterSense device and updates all 
// vsMotionTrackers and vsJoysticks (if any)
// ------------------------------------------------------------------------
void vsIntersenseTrackingSystem::update()
{
    ISD_TRACKER_DATA_TYPE trackerData;
    int trackerNum;
    vsVector position;
    vsQuat orientation;
    vsInputAxis *axis;
    int i, j;

    // Get the latest data from the hardware
    ISD_GetData(systemHandle, &trackerData);

    // Extract the relevant data from the tracker data structure
    for (i = 1; i <= systemInfo.Capability.MaxStations; i++)
    {
        // See if this record matches a valid tracker
        trackerNum = stationToTracker[i];
        if ((trackerNum >= 0) && (trackerNum < numTrackers))
        {
            // Extract and set the position for this tracker
            position.set(trackerData.Station[i-1].Position[VS_X],
                trackerData.Station[i-1].Position[VS_Y], 
                trackerData.Station[i-1].Position[VS_Z]);
            position = coordXform.rotatePoint(position);
            tracker[trackerNum]->setPosition(position);

            // Extract and set the orientation for this tracker
            if (trackerConfig[i].AngleFormat == ISD_QUATERNION)
            {
                orientation.set(trackerData.Station[i-1].Orientation[1],
                    trackerData.Station[i-1].Orientation[2], 
                    trackerData.Station[i-1].Orientation[3], 
                    trackerData.Station[i-1].Orientation[0]);
                tracker[trackerNum]->setOrientation(orientation);
            }
            else
            {
                orientation.setEulerRotation(VS_EULER_ANGLES_ZXY_R,
                    -trackerData.Station[i-1].Orientation[0],
                    trackerData.Station[i-1].Orientation[1],
                    trackerData.Station[i-1].Orientation[2]);
                tracker[trackerNum]->setOrientation(orientation);
            }
            orientation = coordXform * orientation * coordXform;

            // See if this tracker has a joystick associated with it
            if (hasJoystick(trackerNum))
            {
                // Iterate over the joystick's axes
                for (j = 0; j < joystick[trackerNum]->getNumAxes(); j++)
                {
                    // Update this axis with the latest state
                    axis = joystick[trackerNum]->getAxis(j);
                    axis->setPosition(trackerData.Station[i-1].AnalogData[j]);
                }

                // Iterate over the joystick's buttons
                for (j = 0; j < joystick[trackerNum]->getNumButtons(); j++)
                {
                    // Update this button with the latest state
                    if (trackerData.Station[i-1].ButtonState[j])
                        joystick[trackerNum]->getButton(j)->setPressed();
                    else
                        joystick[trackerNum]->getButton(j)->setReleased();
                }
            }
        }
    }
}
