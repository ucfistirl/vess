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
//    VESS Module:  vsWalkInPlace.c++
//
//    Description:  Motion model for walking action
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsWalkInPlace.h++"
#include <stdio.h>
#include <math.h>
#include "vsSystem.h++"

// ------------------------------------------------------------------------
// Constructor for vsWalkInPlace
// ------------------------------------------------------------------------
vsWalkInPlace::vsWalkInPlace(vsMotionTracker *back, vsMotionTracker *left, 
                             vsMotionTracker *right, vsKinematics *kin)
             : vsMotionModel()
{
    // Save the tracker and kinematics objects
    backTracker = back;
    lFootTracker = left;
    rFootTracker = right;
    kinematics = kin;

    // Complain if any trackers are invalid
    if ((backTracker == NULL) || (lFootTracker == NULL) || 
        (rFootTracker == NULL))
    {
        printf("vsWalkInPlace::vsWalkInPlace:  WARNING -- NULL motion "
            "tracker(s) specified!\n");
    }

    // Set movement allowed flags
    forwardAllowed = VS_TRUE;
    backwardAllowed = VS_TRUE;
    sideStepAllowed = VS_TRUE;

    // Set movement speed values
    forwardSpeed = VS_WIP_DEFAULT_FWD_SPD;
    backwardSpeed = VS_WIP_DEFAULT_BCK_SPD;
    sideStepSpeed = VS_WIP_DEFAULT_SS_SPD;

    // Set motion trigger distance thresholds (how far the foot trackers
    // need to be separated in a particular direction for motion to
    // occur)
    forwardThresh = VS_WIP_DEFAULT_FWD_THRESH;
    backwardThresh = VS_WIP_DEFAULT_BCK_THRESH;
    sideStepThresh = VS_WIP_DEFAULT_SS_THRESH;

    // Set movement distance allowance values
    maxAllowance = VS_WIP_DEFAULT_ALLOWANCE;
    moveAllowance = maxAllowance;
    movementLimited = VS_WIP_DEFAULT_LIMIT_STATE;

    // Initialize the heading to default
    lastTrackerHeading = 0.0;
}

// ------------------------------------------------------------------------
// Destructor for vsWalkInPlace
// ------------------------------------------------------------------------
vsWalkInPlace::~vsWalkInPlace()
{
}

// ------------------------------------------------------------------------
// Enables forward motion
// ------------------------------------------------------------------------
void vsWalkInPlace::enableForward()
{
    forwardAllowed = VS_TRUE;
}

// ------------------------------------------------------------------------
// Disables forward motion
// ------------------------------------------------------------------------
void vsWalkInPlace::disableForward()
{
    forwardAllowed = VS_FALSE;
}

// ------------------------------------------------------------------------
// Enables backward motion
// ------------------------------------------------------------------------
void vsWalkInPlace::enableBackward()
{
    backwardAllowed = VS_TRUE;
}

// ------------------------------------------------------------------------
// Disables forward motion
// ------------------------------------------------------------------------
void vsWalkInPlace::disableBackward()
{
    backwardAllowed = VS_FALSE;
}

// ------------------------------------------------------------------------
// Enables side step motion
// ------------------------------------------------------------------------
void vsWalkInPlace::enableSideStep()
{
    sideStepAllowed = VS_TRUE;
}

// ------------------------------------------------------------------------
// Disables side step motion
// ------------------------------------------------------------------------
void vsWalkInPlace::disableSideStep()
{
    sideStepAllowed = VS_FALSE;
}

// ------------------------------------------------------------------------
// Retrieves the velocity of forward motion
// ------------------------------------------------------------------------
double vsWalkInPlace::getForwardSpeed()
{
    return forwardSpeed;
}

// ------------------------------------------------------------------------
// Retrieves the velocity of backward motion
// ------------------------------------------------------------------------
double vsWalkInPlace::getBackwardSpeed()
{
    return backwardSpeed;
}

// ------------------------------------------------------------------------
// Retrieves the velocity of side step motion
// ------------------------------------------------------------------------
double vsWalkInPlace::getSideStepSpeed()
{
    return sideStepSpeed;
}

// ------------------------------------------------------------------------
// Adjusts the velocity of forward motion
// ------------------------------------------------------------------------
void vsWalkInPlace::setForwardSpeed(double speed)
{
    forwardSpeed = speed;
}

// ------------------------------------------------------------------------
// Adjusts the velocity of backward motion
// ------------------------------------------------------------------------
void vsWalkInPlace::setBackwardSpeed(double speed)
{
    backwardSpeed = speed;
}

// ------------------------------------------------------------------------
// Adjusts the velocity of side step motion
// ------------------------------------------------------------------------
void vsWalkInPlace::setSideStepSpeed(double speed)
{
    sideStepSpeed = speed;
}

// ------------------------------------------------------------------------
// Retrieves the tracker threshold for forward motion
// ------------------------------------------------------------------------
double vsWalkInPlace::getForwardThreshold()
{
    return forwardThresh;
}

// ------------------------------------------------------------------------
// Retrieves the tracker threshold for backward motion
// ------------------------------------------------------------------------
double vsWalkInPlace::getBackwardThreshold()
{
    return backwardThresh;
}

// ------------------------------------------------------------------------
// Retrieves the tracker threshold for side step motion
// ------------------------------------------------------------------------
double vsWalkInPlace::getSideStepThreshold()
{
    return sideStepThresh;
}

// ------------------------------------------------------------------------
// Adjusts the tracker threshold for forward motion
// ------------------------------------------------------------------------
void vsWalkInPlace::setForwardThreshold(double threshold)
{
    forwardThresh = threshold;
}

// ------------------------------------------------------------------------
// Adjusts the tracker threshold for backward motion
// ------------------------------------------------------------------------
void vsWalkInPlace::setBackwardThreshold(double threshold)
{
    backwardThresh = threshold;
}

// ------------------------------------------------------------------------
// Adjusts the tracker threshold for side step motion
// ------------------------------------------------------------------------
void vsWalkInPlace::setSideStepThreshold(double threshold)
{
    sideStepThresh = threshold;
}

// ------------------------------------------------------------------------
// Returns the movement allowance (the maximum allowed distance per step)
// ------------------------------------------------------------------------
double vsWalkInPlace::getMovementAllowance()
{
    return maxAllowance;
}

// ------------------------------------------------------------------------
// Set the movement allowance
// ------------------------------------------------------------------------
void vsWalkInPlace::setMovementAllowance(double distance)
{
    maxAllowance = distance;
}

// ------------------------------------------------------------------------
// Enables the movement allowance check
// ------------------------------------------------------------------------
void vsWalkInPlace::enableMovementLimit()
{
    movementLimited = VS_TRUE;
}

// ------------------------------------------------------------------------
// Disables the movement allowance check
// ------------------------------------------------------------------------
void vsWalkInPlace::disableMovementLimit()
{
    movementLimited = VS_FALSE;
}

// ------------------------------------------------------------------------
// Updates the motion model
// ------------------------------------------------------------------------
void vsWalkInPlace::update()
{
    vsVector             backOrient;
    vsVector             leftFoot, rightFoot;
    double               trackerHeading;
    double               deltaHeading;
    vsQuat               headingQuat;
    vsVector             separationVec;
    double               deltaX, deltaY, deltaZ;
    double               deltaTime;
    vsVector             v;
    int                  motionFlag;
    double               moveSpeed;
    double               moveDistance;
    vsQuat               currentOrientation;

    // Grab tracker data
    backOrient = backTracker->getOrientationVec(VS_EULER_ANGLES_ZXY_R);
    leftFoot = lFootTracker->getPositionVec();
    rightFoot = rFootTracker->getPositionVec();

    // Get the heading of the back tracker
    trackerHeading = backOrient[VS_H];
    headingQuat.setAxisAngleRotation(0, 0, 1, -trackerHeading);

    // Compute the separation distance of the feet in all three axes
    // and rotate the separation vector to align it with the back
    // heading.  This allows us to easily determine precisely how the
    // feet are positioned with respect to the body.
    separationVec.setSize(3);
    separationVec[VS_X] = rightFoot[VS_X] - leftFoot[VS_X];
    separationVec[VS_Y] = rightFoot[VS_Y] - leftFoot[VS_Y];
    separationVec[VS_Z] = rightFoot[VS_Z] - leftFoot[VS_Z];
    separationVec = headingQuat.rotatePoint(separationVec);

    // Extract the feet separation distances to their x, y, and z
    // scalar components
    deltaX = separationVec[VS_X];
    deltaY = separationVec[VS_Y];
    deltaZ = separationVec[VS_Z];

    // Compute the current heading relative to last frame's heading
    deltaHeading = trackerHeading - lastTrackerHeading;
    lastTrackerHeading = trackerHeading;
    headingQuat.setAxisAngleRotation(0, 0, 1, deltaHeading);

    // Get the difference in time from last frame to this one
    deltaTime = vsSystem::systemObject->getFrameTime();

    // Initialize speed, velocity, and the motion flag
    moveSpeed = 0.0;
    motionFlag = VS_FALSE;
    v.setSize(3);
    v.clear();

    // Figure out what kind of motion we should carry out by looking at the
    // foot tracker separation distances.  Check for sidestep, backward
    // and then forward motion in that order.
    if ((deltaX < sideStepThresh) && (sideStepAllowed))
    {
        // Feet are crossed, therefore sidestep motion should happen.
        moveSpeed = sideStepSpeed;

        // Figure direction.  The the Y separation indicates the direction 
        // to travel, i.e if the right foot is in front of the left, we 
        // should sidestep left)
        if (deltaY < 0)
        {
            v.set(moveSpeed, 0.0, 0.0);
        }
        else
        {
            v.set(-moveSpeed, 0.0, 0.0);
        }

        // Signal that motion is happening this frame
        motionFlag = VS_TRUE;
    }
    else if ((fabs(deltaY) > backwardThresh) && (backwardAllowed))
    {
        // Feet are separated in the forward/back direction, therefore
        // backward motion should happen.
        moveSpeed = backwardSpeed;

        // Set the velocity vector to the movement speed in the negative Y
        // (backward) direction
        v.set(0.0, -moveSpeed, 0.0);

        // Signal that motion is happening this frame
        motionFlag = VS_TRUE;
    }
    else if ((fabs(deltaZ) > forwardThresh) && (forwardAllowed))
    {
        // One foot is raised above the other, so forward motion should
        // happen
        moveSpeed = forwardSpeed;

        // Set the velocity vector to the movement speed in the positive Y
        // (forward) direction
        v.set(0.0, moveSpeed, 0.0);

        // Signal that motion is happening this frame
        motionFlag = VS_TRUE;
    }

    // Compute the distance that will be moved this frame
    moveDistance = moveSpeed * deltaTime;

    // If motion is happening, see if we need to check our movement
    // allowance
    if ((motionFlag) && (movementLimited))
    {
        // See if the current movement distance will exceed the movement
        // allowance.
        if ((moveSpeed > 0.0) && (moveDistance > moveAllowance))
        {
            // The current amount of motion (this frame) will exceed the 
            // movement allowance, so we need to restrict the movement.
            // Normalize the velocity vector and scale it to the remaining
            // movement allowance.
            v.normalize();
            v.scale(moveAllowance);

            // Reduce the movement distance to the movement allowance
            moveDistance = moveAllowance;

            // Set the movement allowance to zero.  No more motion is
            // possible until the feet return to an idle position.
            moveAllowance = 0.0;
        }
        else
        {
            // The current amount of motion is within the movement allowance
            // reduce the movement allowance by the current movement distance.
            if (moveAllowance > 0.0)
                moveAllowance -= moveDistance;
        }
    }
    else
    {
        // We've stopped moving, so reset the movement allowance.
        moveAllowance = maxAllowance;
    }

    // Modify the orientation by premultiplying this frame's heading change
    // by the current kinematics orientation
    kinematics->preModifyOrientation(headingQuat);

    // Get the new orientation (after applying this frame's heading
    // change) from the kinematics object
    currentOrientation = kinematics->getOrientation();

    // Rotate the movement velocity vector to match the kinematics'
    // orientation
    v = currentOrientation.rotatePoint(v);

    // Add the movement velocity vector to the current kinematics velocity
    kinematics->modifyVelocity(v);
}
