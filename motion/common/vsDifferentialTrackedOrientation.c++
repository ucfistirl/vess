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
//    VESS Module:  vsDifferentialTrackedOrientation.c++
//
//    Description:  Class intended to handle orientation tracking measured
//                  by a motion tracker, but also subject to a reference
//                  orientation measured by a second motion tracker.
//                  A common example would be viewpoint head tracking
//                  where the overall orientation of the user's body also
//                  affects the global orientation of the head.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsDifferentialTrackedOrientation.h++"

// ------------------------------------------------------------------------
// Creates a vsDifferentialTrackedOrientation motion model using the given 
// component and trackers
// ------------------------------------------------------------------------
vsDifferentialTrackedOrientation::vsDifferentialTrackedOrientation(
    vsMotionTracker *ref, vsMotionTracker *diff, vsKinematics *kinObject)
{
    // Store the provided trackers and kinematics object
    refTracker = ref;   
    diffTracker = diff;   
    kinematics = kinObject;

    // Make sure both trackers are valid, complain if not
    if ((refTracker == NULL) || (diffTracker == NULL))
    {
        printf("vsDifferentialTrackedOrientation::"
               "vsDifferentialTrackedOrientation:\n");
        printf("   WARNING -- NULL motion tracker(s) specified!\n");
    }

    // Set the user-specified orientation offset to no rotation
    orientationOffset.set(0.0, 0.0, 0.0, 1.0);

    // Set both tracker reset positions to no rotation
    resetRef.set(0.0, 0.0, 0.0, 1.0);
    resetDiff.set(0.0, 0.0, 0.0, 1.0);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsDifferentialTrackedOrientation::~vsDifferentialTrackedOrientation()
{
}

// ------------------------------------------------------------------------
// Returns the class name
// ------------------------------------------------------------------------
const char *vsDifferentialTrackedOrientation::getClassName()
{
    return "vsDifferentialTrackedOrientation.h++";
}

// ------------------------------------------------------------------------
// Sets the orientation post-offset (applies to the final differential
// orientation after being adjusted by the reference orientation)
// ------------------------------------------------------------------------
void vsDifferentialTrackedOrientation::setOrientationOffset(vsQuat newOffset)
{
    orientationOffset = newOffset;
}

// ------------------------------------------------------------------------
// Gets the orientation post-offset
// ------------------------------------------------------------------------
vsQuat vsDifferentialTrackedOrientation::getOrientationOffset()
{
    return orientationOffset;
}

// ------------------------------------------------------------------------
// Updates the motion model
// ------------------------------------------------------------------------
void vsDifferentialTrackedOrientation::update()
{
    vsQuat refOrn;
    vsQuat diffOrn;

    // Get tracker data
    refOrn = refTracker->getOrientationQuat();
    diffOrn = diffTracker->getOrientationQuat();

    // Factor in reset orientations
    refOrn = resetRef * refOrn;
    diffOrn = resetDiff * diffOrn;

    // Subtract the reference tracker's orientation from the differential
    // tracker's orientation
    refOrn.conjugate();
    diffOrn = refOrn * diffOrn;
    
    // Factor in user-specified offsets
    diffOrn = orientationOffset * diffOrn;
    
    // Apply the data to the kinematics object
    kinematics->setOrientation(diffOrn);
}

// ------------------------------------------------------------------------
// Sets the reset position and orientation of the motion model to the
// current position and orientation
// ------------------------------------------------------------------------
void vsDifferentialTrackedOrientation::reset()
{
    vsQuat refOrn;
    vsQuat diffOrn;

    // Get current tracker orientations
    refOrn = refTracker->getOrientationQuat();
    diffOrn = diffTracker->getOrientationQuat();

    // Set the reset data as the conjugates of the current tracker data
    resetRef = refOrn.getConjugate();
    resetDiff = diffOrn.getConjugate();
}
