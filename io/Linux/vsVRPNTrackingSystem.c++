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
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------


#include "vsVRPNTrackingSystem.h++"


vsVRPNTrackingSystem::vsVRPNTrackingSystem(atString hostName,
    atList * trackerNames, atList * buttonNames)
{
    // Store the hostname in a local variable.
    remoteHostname = hostName;

    // No remote connection is required under this constructor.
    localHostname.setString("0.0.0.0");
    remoteConnection = NULL;

    // Now initialize the remote objects.
    createRemoteObjects(hostName, trackerNames, buttonNames);
}


vsVRPNTrackingSystem::vsVRPNTrackingSystem(atString hostName,
    atString localName, atList * trackerNames, atList * buttonNames)
{
    // Store the hostnames in a local variable.
    remoteHostname = hostName;
    localHostname = localName;

    // Forge a special connection. If the local machine has more than one
    // adapter, this is necessary to differentiate between them.
    remoteConnection = vrpn_get_connection_by_name(remoteHostname.getString(),
        NULL, NULL, NULL, NULL, 1.0, 3, localHostname.getString());

    // Now initialize the remote objects.
    createRemoteObjects(hostName, trackerNames, buttonNames);
}


void vsVRPNTrackingSystem::createRemoteObjects(atString hostName,
    atList *trackerNames, atList *buttonNames)
{
    atString    *curName;
    char        hostVRPNAddress[256];
    int         i;

    // Create the appropriate number of trackers.
    if (trackerNames != NULL)
    {
        // Determine the number of trackers requested and cap that value if it
        // exceeds the maximum number supported.
        numRemoteTrackers = trackerNames->getNumEntries();
        if (numRemoteTrackers > VS_VRPN_MAX_REMOTE_TRACKERS)
        {
            numRemoteTrackers = VS_VRPN_MAX_REMOTE_TRACKERS;
        }

        // Create each remote tracker by name.
        curName = (atString *)trackerNames->getFirstEntry();
        for (i = 0; i < numRemoteTrackers; i++)
        {
            // Indicate that a connection to the tracker is being attempted.
            notify(AT_INFO, "Connecting to tracker %s.\n",
                curName->getString());

            // Generate the full address from the host and tracker names.
            sprintf(hostVRPNAddress,
                "%s@%s", curName->getString(), remoteHostname.getString());

            // ALLOCATE DATA FOOL.
            remoteTrackers[i] = 
                (vsVRPNRemoteTracker *)malloc(sizeof(vsVRPNRemoteTracker));

            // Create the remote tracker connection.
            remoteTrackers[i]->vrpnTracker =
                new vrpn_Tracker_Remote(hostVRPNAddress);
            remoteTrackers[i]->vrpnTracker->register_change_handler(
                (void *)(remoteTrackers[i]), remoteTrackerChangeHandler);

            // Move on to the next tracker name.
            curName = (atString *)trackerNames->getNextEntry();
        }
    }
    else
    {
       // No remote trackers are to be used by this tracking system.
       numRemoteTrackers = 0;
    }

    // Create the appropriate number of buttons.
    if (buttonNames != NULL)
    {
        // Determine the number of buttons requested and cap that value if it
        // exceeds the maximum number supported.
        numRemoteButtons = buttonNames->getNumEntries();
        if (numRemoteButtons > VS_VRPN_MAX_REMOTE_BUTTONS)
        {
            numRemoteButtons = VS_VRPN_MAX_REMOTE_BUTTONS;
        }

        // Create each remote button by name.
        curName = (atString *)buttonNames->getFirstEntry();
        for (i = 0; i < numRemoteButtons; i++)
        {
            // Indicate that a connection to the button is being attempted.
            notify(AT_INFO, "Connecting to button %s.\n",
                curName->getString());

            // Generate the full address from the host and button names.
            sprintf(hostVRPNAddress,
                "%s@%s", curName->getString(), remoteHostname.getString());

            // Create the remote button connection.
            remoteButtons[i] = 
                (vsVRPNRemoteButton *)malloc(sizeof(vsVRPNRemoteButton));

            remoteButtons[i]->vrpnButton =
                new vrpn_Button_Remote(hostVRPNAddress);
            remoteButtons[i]->vrpnButton->register_change_handler(
                (void *)(remoteButtons[i]), remoteButtonChangeHandler);

            // Move on to the next button name.
            curName = (atString *)buttonNames->getNextEntry();
        }
    }
    else
    {
       // No remote buttons are to be used by this tracking system.
       numRemoteButtons = 0;
    }
}


vsVRPNTrackingSystem::~vsVRPNTrackingSystem()
{
    // FIXME: Actually clean up memory here.
}


void vsVRPNTrackingSystem::update()
{
    int i;

    // Perform an update on the connection if it exists.
    if (remoteConnection != NULL)
        remoteConnection->mainloop();

    // Perform an update on each of the trackers.
    for (i = 0; i < numRemoteTrackers; i++)
    {
        // Tell the tracker to cycle its main loop.
        remoteTrackers[i]->vrpnTracker->mainloop();
    }

    // Perform an update on each of the buttons.
    for (i = 0; i < numRemoteButtons; i++)
    {
        // Tell the button to cycle its main loop.
        remoteButtons[i]->vrpnButton->mainloop();
    }
}


void vsVRPNTrackingSystem::remoteTrackerChangeHandler(void *userData,
    const vrpn_TRACKERCB tracker)
{
    vsVRPNRemoteTracker *trackerData;

    // TODO: This structure needs to be protected by a semaphore.

    // Fetch the tracker structure from the user data.
    trackerData = (vsVRPNRemoteTracker *)userData;

    // Store the most recent data from the tracker into the structure.
    trackerData->trackerPosition.set(
        tracker.pos[0], tracker.pos[1], tracker.pos[2]);
    trackerData->trackerOrientation.set(
        tracker.quat[0], tracker.quat[1], tracker.quat[2], tracker.quat[3]);
}


void vsVRPNTrackingSystem::remoteButtonChangeHandler(void *userData,
    const vrpn_BUTTONCB button)
{
    vsVRPNRemoteButton *buttonData;

    // TODO: This structure needs to be protected by a semaphore.

    // Fetch the button structure from the user data.
    buttonData = (vsVRPNRemoteButton *)userData;

    // Store the most recent data from the button into the structure.
    if (button.state == 1)
    {
        buttonData->buttonState = true;
    }
    else
    {
        buttonData->buttonState = false;
    }
}


