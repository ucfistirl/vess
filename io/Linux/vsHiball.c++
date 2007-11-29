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
//    VESS Module:  vsHiball.c++
//
//    Description:  Class to handle input from a Hiball tracking system.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsHiball.h++"

vsHiball::vsHiball(atString hostName, atList * trackerNames,
    atList * buttonNames) :
    vsVRPNTrackingSystem(hostName, trackerNames, buttonNames)
{
    // Create local objects corresponding to those created by the base class.
    createLocalObjects();
}


vsHiball::vsHiball(atString hostName, atString localName,
    atList * trackerNames, atList * buttonNames) :
    vsVRPNTrackingSystem(hostName, localName, trackerNames, buttonNames)
{
    // Create local objects corresponding to those created by the base class.
    createLocalObjects();
}


void vsHiball::createLocalObjects()
{
    int    i;

    // For each VRPN remote tracker created in the base class, create a
    // corresponding vsMotionTracker.
    for (i = 0; i < numRemoteTrackers; i++)
    {
        // Create the motion tracker and reference it.
        motionTrackers[i] = new vsMotionTracker(i);
        motionTrackers[i]->ref();
    }

    // For each VRPN remote button created in the base class, create a
    // corresponding vsInputButton.
    for (i = 0; i < numRemoteButtons; i++)
    {
        // Create the motion tracker and reference it.
        inputButtons[i] = new vsInputButton();
        inputButtons[i]->ref();
    }
}


vsHiball::~vsHiball()
{
    int i;

    // TODO: Confirm that this scheme will actually cause the base class
    // destructor to be called.

    // Free each of the vsMotionTrackers, free the local reference and
    // potentially delete the object.
    for (i = 0; i < numRemoteTrackers; i++)
    {
        vsObject::unrefDelete(motionTrackers[i]);
    }

    // Free each of the vsInputButtons, free the local reference and
    // potentially delete the object.
    for (i = 0; i < numRemoteButtons; i++)
    {
        vsObject::unrefDelete(inputButtons[i]);
    }
}


const char * vsHiball::getClassName()
{
    return "vsHiball";
}


int vsHiball::getNumTrackers()
{
    return numRemoteTrackers;
}


vsMotionTracker *vsHiball::getTracker(int index)
{
    // Confirm that the index of the tracker is within array bounds.
    if ((index >= 0) && (index < numRemoteTrackers))
        return motionTrackers[index];

    // Return NULL by default.
    return NULL;
}


int vsHiball::getNumButtons()
{
    return numRemoteButtons;
}


vsInputButton *vsHiball::getButton(int index)
{
    // Confirm that the index of the button is within array bounds.
    if ((index >= 0) && (index < numRemoteButtons))
        return inputButtons[index];

    // Return NULL by default.
    return NULL;
}


void vsHiball::update()
{
    int i;

    // Update the base class to update each of the VRPN trackers.
    vsVRPNTrackingSystem::update();

    // Now copy the states of each of the remote trackers into the
    // corresponding vsMotionTracker.
    for (i = 0; i < numRemoteTrackers; i++)
    {
        motionTrackers[i]->setPosition(remoteTrackers[i]->trackerPosition);
        motionTrackers[i]->
            setOrientation(remoteTrackers[i]->trackerOrientation);
    }

    // Also copy the states of each of the remote buttons into the
    // corresponding vsInputButton.
    for (i = 0; i < numRemoteButtons; i++)
    {
        // See if the remote button state and the input button state differ.
        if ((remoteButtons[i]->buttonState == true) &&
            (inputButtons[i]->isPressed() == false))
        {
            // Set that the button was just pressed.
            inputButtons[i]->setPressed();
        }
        else if ((remoteButtons[i]->buttonState == false) &&
            (inputButtons[i]->isPressed() == true))
        {
            // Set that the button was just released.
            inputButtons[i]->setReleased();
        }
    }
}

