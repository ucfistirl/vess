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
//    VESS Module:  vsTrackedMotion.c++
//
//    Description:  Class intended to take motion data from a motion
//		    tracker and apply the movements directly to the
//		    component
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsTrackedMotion.h++"

// ------------------------------------------------------------------------
// Creates a vsTrackedMotion motion model using the given component and
// tracker
// ------------------------------------------------------------------------
vsTrackedMotion::vsTrackedMotion(vsMotionTracker *theTracker,
    vsKinematics *kinObject)
{
    // Get the tracker and kinematics objects
    tracker = theTracker;   
    kinematics = kinObject;

    // Complain if the tracker object is invalid
    if (tracker == NULL)
    {
        printf("vsTrackedMotion::vsTrackedMotion:  WARNING -- NULL motion "
            "tracker specified!\n");
    }

    // Enable position and orientation tracking by default
    positionEnabled = VS_TRUE;
    orientationEnabled = VS_TRUE;
    
    // Set the offsets to identity
    positionOffset.set(0.0, 0.0, 0.0);
    orientationOffset.set(0.0, 0.0, 0.0, 1.0);
    resetPosition.set(0.0, 0.0, 0.0);
    resetOrientation.set(0.0, 0.0, 0.0, 1.0);

    // Initialize position scaling to identity as well
    positionScale = 1.0;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsTrackedMotion::~vsTrackedMotion()
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsTrackedMotion::getClassName()
{
    return "vsTrackedMotion";
}

// ------------------------------------------------------------------------
// Enables positional motion
// ------------------------------------------------------------------------
void vsTrackedMotion::enablePositionTracking()
{
    positionEnabled = VS_TRUE;
}

// ------------------------------------------------------------------------
// Disables positional motion
// ------------------------------------------------------------------------
void vsTrackedMotion::disablePositionTracking()
{
    positionEnabled = VS_FALSE;
}

// ------------------------------------------------------------------------
// Enables rotational motion
// ------------------------------------------------------------------------
void vsTrackedMotion::enableOrientationTracking()
{
    orientationEnabled = VS_TRUE;
}

// ------------------------------------------------------------------------
// Disables rotational motion
// ------------------------------------------------------------------------
void vsTrackedMotion::disableOrientationTracking()
{
    orientationEnabled = VS_FALSE;
}

// ------------------------------------------------------------------------
// Sets the position offset
// ------------------------------------------------------------------------
void vsTrackedMotion::setPositionOffset(vsVector newOffset)
{
    // Copy the new position offset and make sure the vector is
    // the correct size
    positionOffset.clearCopy(newOffset);
    positionOffset.setSize(3);
}

// ------------------------------------------------------------------------
// Gets the position offset
// ------------------------------------------------------------------------
vsVector vsTrackedMotion::getPositionOffset()
{
    return positionOffset;
}

// ------------------------------------------------------------------------
// Sets the orientation post-offset
// ------------------------------------------------------------------------
void vsTrackedMotion::setOrientationOffset(vsQuat newOffset)
{
    orientationOffset = newOffset;
}

// ------------------------------------------------------------------------
// Gets the orientation post-offset
// ------------------------------------------------------------------------
vsQuat vsTrackedMotion::getOrientationOffset()
{
    return orientationOffset;
}

// ------------------------------------------------------------------------
// Sets the scale factor for position data
// ------------------------------------------------------------------------
void vsTrackedMotion::setPositionScale(double scale)
{
    positionScale = scale;
}

// ------------------------------------------------------------------------
// Returns the scale factor for position data
// ------------------------------------------------------------------------
double vsTrackedMotion::getPositionScale()
{
    return positionScale;
}

// ------------------------------------------------------------------------
// Updates the motion model
// ------------------------------------------------------------------------
void vsTrackedMotion::update()
{
    vsVector trackerPos;
    vsQuat trackerOrn;

    // Get tracker data
    trackerPos = tracker->getPositionVec();
    trackerOrn = tracker->getOrientationQuat();

    // Factor in reset position and offsets
    trackerPos += resetPosition;
    trackerOrn = resetOrientation * trackerOrn;

    // Scale the position
    trackerPos.scale(positionScale);
    
    // Factor in the user-specified offsets
    trackerPos += positionOffset;
    trackerOrn = orientationOffset * trackerOrn;

    // Apply the data to the kinematics object
    if (positionEnabled)
        kinematics->setPosition(trackerPos);
    if (orientationEnabled)
        kinematics->setOrientation(trackerOrn);
}

// ------------------------------------------------------------------------
// Sets the reset position and orientation of the motion model to the
// current position and orientation
// ------------------------------------------------------------------------
void vsTrackedMotion::reset()
{
    vsVector trackerPos;
    vsQuat trackerOrn;

    // Get tracker data
    trackerPos = tracker->getPositionVec();  
    trackerOrn = tracker->getOrientationQuat();

    // Set the reset data
    resetPosition = trackerPos * -1.0;
    resetOrientation = trackerOrn.getConjugate();
}