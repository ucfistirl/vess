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
//    VESS Module:  vsHeadMotion.c++
//
//    Description:  Class intended to handle viewpoint head tracking
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsHeadMotion.h++"

// ------------------------------------------------------------------------
// Creates a vsHeadMotion motion model using the given component and
// tracker
// ------------------------------------------------------------------------
vsHeadMotion::vsHeadMotion(vsMotionTracker *back, vsMotionTracker *head,
    vsKinematics *kinObject)
{
    // Save the tracker and kinematics objects
    backTracker = back;   
    headTracker = head;   
    kinematics = kinObject;

    // Print an error if either tracker is not valid
    if ((backTracker == NULL) || (headTracker == NULL))
    {
        printf("vsHeadMotion::vsHeadMotion:  WARNING -- NULL motion "
            "tracker(s) specified!\n");
    }

    // Initialize the offset quaternion
    orientationOffset.set(0.0, 0.0, 0.0, 1.0);

    // Initialize the user-reset quaternions
    resetBack.set(0.0, 0.0, 0.0, 1.0);
    resetHead.set(0.0, 0.0, 0.0, 1.0);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsHeadMotion::~vsHeadMotion()
{
}

// ------------------------------------------------------------------------
// Sets the orientation post-offset (applies to the final head orientation
// after being adjusted by the reference (e.g.: back or waist) orientation)
// ------------------------------------------------------------------------
void vsHeadMotion::setOrientationOffset(vsQuat newOffset)
{
    orientationOffset = newOffset;
}

// ------------------------------------------------------------------------
// Gets the orientation post-offset
// ------------------------------------------------------------------------
vsQuat vsHeadMotion::getOrientationOffset()
{
    return orientationOffset;
}

// ------------------------------------------------------------------------
// Updates the motion model
// ------------------------------------------------------------------------
void vsHeadMotion::update()
{
    vsQuat backOrn;
    vsQuat headOrn;

    // Get tracker data
    backOrn = backTracker->getOrientationQuat();
    headOrn = headTracker->getOrientationQuat();

    // Factor in reset position and offsets
    backOrn = resetBack * backOrn;
    headOrn = resetHead * headOrn;

    // Subtract the back orientation from the head
    backOrn.conjugate();
    headOrn = backOrn * headOrn;
    
    // Factor in specified offsets
    headOrn = orientationOffset * headOrn;
    
    // Apply the data to the kinematics object
    kinematics->setOrientation(headOrn);
}

// ------------------------------------------------------------------------
// Sets the reset position and orientation of the motion model to the
// current position and orientation
// ------------------------------------------------------------------------
void vsHeadMotion::reset()
{
    vsQuat backOrn;
    vsQuat headOrn;

    // Get current tracker data
    backOrn = backTracker->getOrientationQuat();
    headOrn = headTracker->getOrientationQuat();

    // Set the reset data
    resetBack = backOrn.getConjugate();
    resetHead = headOrn.getConjugate();
}
