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
//    VESS Module:  vs4TrackerHead.c++
//
//    Description:  Class to handle viewpoint head tracking.
//                  Suited for use with motion tracking systems which
//                  do not provide orientation of trackers, such as the
//                  PhaseSpace Motion Digitizer.
//
//    Author(s):    David Smith
//
//------------------------------------------------------------------------

#include "vs4TrackerHead.h++"
#include "vsVector.h++"
#include "vsQuat.h++"

// ------------------------------------------------------------------------
// Constructor.  Creates a 4-tracker head motion model using two position
// markers on the head (front and back), and two position markers on the
// shoulders (left and right).
// ------------------------------------------------------------------------
vs4TrackerHead::vs4TrackerHead(vsMotionTracker *headRear, 
                               vsMotionTracker *headFront, 
                               vsMotionTracker *lShoulder, 
                               vsMotionTracker *rShoulder, 
                               vsKinematics *kin)
{
    int i;

    headTrackerRear = headRear;
    headTrackerFront = headFront;
    lShoulderTracker = lShoulder;
    rShoulderTracker = rShoulder;
    kinematics = kin;

    // Warn if any of the trackers aren't given
    if ((headTrackerRear == NULL) || (headTrackerFront == NULL) ||
        (lShoulderTracker == NULL) || (rShoulderTracker == NULL))
    {
        printf("vs4TrackerHead::vs4TrackerHead:\n"
               "    WARNING -- NULL motion tracker(s) specified!\n");
    }

    // Warn if there is no kinematics to control
    if (kinematics == NULL)
    {
        printf("vs4TrackerHead::vs4TrackerHead:\n"
               "    WARNING -- NULL kinematics specified!\n");
    }
}

// ------------------------------------------------------------------------
// Destructor.  Does nothing.
// ------------------------------------------------------------------------
vs4TrackerHead::~vs4TrackerHead()
{
}

// ------------------------------------------------------------------------
// Return the name of this class.
// ------------------------------------------------------------------------
const char *vs4TrackerHead::getClassName()
{
    return "vs4TrackerHead";
}

// ------------------------------------------------------------------------
// Update method.  Compares the vector created by the two head markers and
// the vector created by the two shoulder markers and adjusts the head
// kinematics orientation to match.
// ------------------------------------------------------------------------
void vs4TrackerHead::update()
{
    vsVector torsoForward;
    vsVector lShoulderToChest, shoulderLeftToRight;
    vsVector headForward, headForwardXY, headForwardYZ;
    vsVector globalUp;
    double headZRot, headXRot;
    vsQuat torsoOrient, headOrient;
    vsQuat headZQuat, headXQuat;
    
    // Get orientation of torso
    shoulderLeftToRight = 
        rShoulderTracker->getPositionVec() - lShoulderTracker->getPositionVec();
    shoulderLeftToRight.normalize();
    globalUp.set(0.0, 0.0, 1.0);
    torsoForward = 
        globalUp.getCrossProduct(shoulderLeftToRight);

    // Get the head forward vector
    headForward = 
        headTrackerFront->getPositionVec() - headTrackerRear->getPositionVec();
    headForward.normalize();

    // Get the z-axis rotation of the back to front head vector
    headForwardXY = headForward;
    headForwardXY[VS_Z] = 0.0;
    headZRot = headForwardXY.getAngleBetween(torsoForward);    

    // Is the head turned left or right?
    if (torsoForward.getCrossProduct(headForwardXY)[VS_Z] < 0.0)
        headZRot = -headZRot;

    // Convert the Z rotation to a quaternion
    headZQuat.setAxisAngleRotation(0, 0, 1, headZRot);
    
    // Get the pitch (x-axis) rotation of the head, start by projecting the
    // head's direction vector onto the YZ plane, then rotating
    // that vector by the inverse of the heading rotation that we computed 
    // above
    headForwardYZ = headForward;
    headForwardYZ = headZQuat.getConjugate().rotatePoint(headForwardYZ);

    // Get the pitch of the head vector
    headXRot = torsoForward.getAngleBetween(headForwardYZ);

    // Is the head turned down or up?
    if (globalUp.getDotProduct(headForward) < 0.0)
        headXRot = -headXRot; 

    // Convert the X rotation to a quaternion
    headXQuat.setAxisAngleRotation(1, 0, 0, headXRot);

    // Combine the two rotations to get the orientation of the head... 
    headOrient = headXQuat * headZQuat;
    kinematics->setOrientation(headOrient);     
}
