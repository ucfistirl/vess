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
    int    i;

    // For each VRPN remote tracker created in the base class, create a
    // corresponding vsMotionTracker. Determine the number of trackers to be
    // created.
    numTrackers = trackerNames->getNumEntries();
    if (numTrackers > VS_VRPN_MAX_REMOTE_TRACKERS)
        numTrackers = VS_VRPN_MAX_REMOTE_TRACKERS;

    // Create a the vsMotionTrackers.
    for (i = 0; i < numTrackers; i++)
    {
        // Create the motion tracker and reference it.
        motionTrackers[i] = new vsMotionTracker(i);
        motionTrackers[i]->ref();
    }
}


vsHiball::~vsHiball()
{
    int i;

    // TODO: Confirm that this scheme will actually cause the base class
    // destructor to be called.

    // Free each of the vsMotionTrackers, free the local reference and
    // potentially delete the object.
    for (i = 0; i < numTrackers; i++)
    {
        vsObject::unrefDelete(motionTrackers[i]);
    }
}


const char * vsHiball::getClassName()
{
    return "vsHiball";
}


int vsHiball::getNumTrackers()
{
    return numTrackers;
}


vsMotionTracker *vsHiball::getTracker(int index)
{
    // Confirm that the index of the tracker is within array bounds.
    if ((index >= 0) && (index < numTrackers))
        return motionTrackers[index];

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
    for (i = 0; i < numTrackers; i++)
    {
        motionTrackers[i]->setPosition(remoteTrackers[i]->trackerPosition);
        motionTrackers[i]->
            setOrientation(remoteTrackers[i]->trackerOrientation);
    }
}

