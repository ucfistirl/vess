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
//    VESS Module:  vsVRPNTrackingSystem.c++
//
//    Description:  Base class to handle communication for motion tracking
//                  systems based on the VRPN communication scheme.
//
//    Author(s):    Casey Thurston, Jason Daly
//
//------------------------------------------------------------------------


#include "vsVRPNTrackingSystem.h++"
#include "atTimer.h++"


// ------------------------------------------------------------------------
// Simple constructor for a VRPN tracking system connection, takes the
// remote host name, the name of the tracker server (if any), and the name
// of the button server (if any).  An empty strings for the server name
// means that we don't want that server
// ------------------------------------------------------------------------
vsVRPNTrackingSystem::vsVRPNTrackingSystem(atString serverHostname,
    atString trackerServerName, atString buttonServerName)
{
    // Store the hostname in a local variable
    remoteHostname = serverHostname;

    // No remote connection is required under this constructor
    localHostname.setString("0.0.0.0");
    remoteConnection = NULL;

    // Now initialize the tracking system
    init(remoteHostname, trackerServerName, buttonServerName);
}

// ------------------------------------------------------------------------
// Slightly more complex constructor for a VRPN tracking system connection.
// This one also takes the remote host name and the names for the tracker
// and button servers, but it also takes a local host name, for use when
// the local host has more than one network interface.
// ------------------------------------------------------------------------
vsVRPNTrackingSystem::vsVRPNTrackingSystem(atString serverHostname,
    atString localName, atString trackerServerName, atString buttonServerName)
{
    // Store the hostnames in a local variable
    remoteHostname = serverHostname;
    localHostname = localName;

    // Forge a special connection. If the local machine has more than one
    // interface, this is necessary to differentiate between them
    remoteConnection = vrpn_get_connection_by_name(remoteHostname.getString(),
        NULL, NULL, NULL, NULL, 1.0, 3, localHostname.getString());

    // Now initialize the tracking system
    init(remoteHostname, trackerServerName, buttonServerName);
}

// ------------------------------------------------------------------------
// Destructor.  Cleans up any IO objects we've created and closes all
// VRPN connections.
// ------------------------------------------------------------------------
vsVRPNTrackingSystem::~vsVRPNTrackingSystem()
{
    // Delete the trackers, if we have any
    if (motionTrackers != NULL)
        delete motionTrackers;

    // Delete the buttons, if we have any
    if (trackerButtons != NULL)
        delete trackerButtons;

    // Delete the remote connections (if open)
    if (remoteTrackerConnection != NULL)
        delete remoteTrackerConnection;
    if (remoteButtonConnection != NULL)
        delete remoteButtonConnection;
    if (remoteConnection != NULL)
        delete remoteConnection;
}

// ------------------------------------------------------------------------
// Initializes the VRPN tracking system by creating connections to the
// tracker and/or button servers on the remote host, and counting the
// sensors and/or buttons that we get back
// ------------------------------------------------------------------------
void vsVRPNTrackingSystem::init(atString hostName, atString trackerServerName,
                                atString buttonServerName)
{
    char        hostVRPNAddress[256];
    int         i;
    atTimer     initTimer;

    // See if we should set up motion trackers
    if (trackerServerName.getLength() > 0)
    {
        // Indicate that a connection to the tracker is being attempted
        notify(AT_INFO, "Connecting to tracker server %s@%s...\n",
            trackerServerName.getString(), remoteHostname.getString());

        // Generate the full address from the host and tracker names
        sprintf(hostVRPNAddress, "%s@%s", trackerServerName.getString(),
            remoteHostname.getString());

        // Create the remote tracker connection
        remoteTrackerConnection = new vrpn_Tracker_Remote(hostVRPNAddress);

        // Set up the tracker data callback
        remoteTrackerConnection->register_change_handler(this,
           remoteTrackerChangeHandler);

        // Create the array of motion trackers
        motionTrackers = new atArray();
    }
    else
    {
        // No remote trackers are to be used by this tracking system
        remoteTrackerConnection = NULL;
        motionTrackers = NULL;
    }

    // See if we should set up a connection to a button server
    if (buttonServerName.getLength() > 0)
    {
        // Indicate that a connection to the button is being attempted
        notify(AT_INFO, "Connecting to button server %s@%s\n",
            buttonServerName.getString(), remoteHostname.getString());

        // Generate the full address from the host and button names
        sprintf(hostVRPNAddress, "%s@%s", buttonServerName.getString(),
            remoteHostname.getString());

        // Create the connection to the remote button server
        remoteButtonConnection = new vrpn_Button_Remote(hostVRPNAddress);

        // Set up the button callback
        remoteButtonConnection->register_change_handler(this,
            remoteButtonChangeHandler);

        // Create the button array
        trackerButtons = new atArray();
    }
    else
    {
        // No remote buttons are to be used by this tracking system
        remoteButtonConnection = NULL;
        trackerButtons = NULL;
    }

    // Now, update the system for a couple of seconds to count the number of
    // trackers and/or buttons we have
    notify(AT_INFO, "Counting trackers and buttons...\n");
    initTimer.mark();
    while (initTimer.getElapsed() < 2.0)
    {
        update();
        usleep(100000);
    }
    notify(AT_INFO, "   found %d trackers and %d buttons\n",
       getNumTrackers(), getNumButtons());
}

// ------------------------------------------------------------------------
// VRPN callback for updating tracker data
// ------------------------------------------------------------------------
void vsVRPNTrackingSystem::remoteTrackerChangeHandler(
                                              void *userData,
                                              const vrpn_TRACKERCB vrpnTracker)
{
    vsVRPNTrackingSystem *instance;
    int sensor;
    vsMotionTracker *vessTracker;
    atVector position;
    atQuat orientation;

    // Get the class instance from the user data
    instance = (vsVRPNTrackingSystem *)userData;

    // See which tracker (VRPN calls them "sensors") this update is for
    sensor = vrpnTracker.sensor;

    // Try to get the corresponding tracker
    vessTracker = instance->getTracker(sensor);
    if (vessTracker == NULL)
    {
       // Create the tracker and add it to the array at the correct index
       vessTracker = new vsMotionTracker(sensor);
       instance->motionTrackers->setEntry(sensor, vessTracker);
    }

    // Store the most recent data from the VRPN sensor into the VESS tracker
    position.set(vrpnTracker.pos[0], vrpnTracker.pos[1], vrpnTracker.pos[2]);
    orientation.set(vrpnTracker.quat[0], vrpnTracker.quat[1],
       vrpnTracker.quat[2], vrpnTracker.quat[3]);
    vessTracker->setPosition(position);
    vessTracker->setOrientation(orientation);
}

// ------------------------------------------------------------------------
// VRPN callback for updating button data
// ------------------------------------------------------------------------
void vsVRPNTrackingSystem::remoteButtonChangeHandler(
                                                void *userData,
                                                const vrpn_BUTTONCB vrpnButton)
{
    vsVRPNTrackingSystem *instance;
    int buttonNum;
    vsInputButton *vessButton;

    // Fetch the class instance from user data
    instance = (vsVRPNTrackingSystem *)userData;

    // See which button this update is for
    buttonNum = vrpnButton.button;

    // Try to get the corresponding VESS button
    vessButton = instance->getButton(buttonNum);
    if (vessButton == NULL)
    {
       // Create the button
       vessButton = new vsInputButton();

       // Add the button to the array
       instance->trackerButtons->setEntry(buttonNum, vessButton);
    }

    // Update the button's state
    if ((vrpnButton.state == 1) && (!vessButton->isPressed()))
        vessButton->setPressed();
    else if ((vrpnButton.state != 1) && (vessButton->isPressed()))
        vessButton->setReleased();
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsVRPNTrackingSystem::getClassName()
{
    return "vsVRPNTrackingSystem";
}

// ------------------------------------------------------------------------
// Return the number of trackers in this tracking system
// ------------------------------------------------------------------------
int vsVRPNTrackingSystem::getNumTrackers()
{
    // Return the number of trackers we have (or zero if we don't have a
    // tracker server connection)
    if (motionTrackers != NULL)
        return motionTrackers->getNumEntries();
    else
        return 0;
}

// ------------------------------------------------------------------------
// Return the requested tracker
// ------------------------------------------------------------------------
vsMotionTracker *vsVRPNTrackingSystem::getTracker(int index)
{
    // Return the tracker corresponding to the specified index, or NULL
    // if we don't have any trackers
    if (motionTrackers != NULL)
        return (vsMotionTracker *)(motionTrackers->getEntry(index));
    else
        return NULL;
}

// ------------------------------------------------------------------------
// Return the number of buttons in this tracking system
// ------------------------------------------------------------------------
int vsVRPNTrackingSystem::getNumButtons()
{
    // Return the number of buttons we have (or zero if we don't have a
    // button server connection)
    if (trackerButtons != NULL)
        return trackerButtons->getNumEntries();
    else
        return 0;
}

// ------------------------------------------------------------------------
// Return the requested button
// ------------------------------------------------------------------------
vsInputButton *vsVRPNTrackingSystem::getButton(int index)
{
    // Return the button corresponding to the specified index, or NULL
    // if we don't have any buttons
    if (trackerButtons != NULL)
        return (vsInputButton *)(trackerButtons->getEntry(index));
    else
        return NULL;
}

// ------------------------------------------------------------------------
// Main update method.  Simply calls the mainloop() method on our remote
// connections (VRPN handles the rest).
// ------------------------------------------------------------------------
void vsVRPNTrackingSystem::update()
{
    int i;

    // Perform an update on the connection if it exists
    if (remoteConnection != NULL)
        remoteConnection->mainloop();

    // Perform an update on the trackers
    remoteTrackerConnection->mainloop();

    // Perform an update on the buttons
    remoteButtonConnection->mainloop();
}


