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
//    Modified :    David Smith (July 21, 2005)
//
//------------------------------------------------------------------------

#include "vsWalkInPlace.h++"
#include <stdio.h>
#include <math.h>
#include "vsTimer.h++"

#define MM_TO_INCHES 0.039370079

// ------------------------------------------------------------------------
// Constructor for vsWalkInPlace, using the positions of the feet and
// the orientation of the back.
// ------------------------------------------------------------------------
vsWalkInPlace::vsWalkInPlace(vsMotionTracker *back, vsMotionTracker *left, 
                             vsMotionTracker *right, vsKinematics *kin)
             : vsMotionModel()
{
    // Save the tracker and kinematics objects
    backTracker = back;
    lFootTracker = left;
    rFootTracker = right;
    lHipTracker = NULL;
    rHipTracker = NULL;
    kinematics = kin;

    // Complain if any trackers are invalid
    if ((backTracker == NULL) || (lFootTracker == NULL) || 
        (rFootTracker == NULL))
    {
        printf("vsWalkInPlace::vsWalkInPlace:  WARNING -- NULL motion "
            "tracker(s) specified!\n");
    }

    // Set movement allowed flags
    forwardAllowed = true;
    backwardAllowed = true;
    sideStepAllowed = true;

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
// Constructor for vsWalkInPlace using the positions of the feet and the
// hips.  The hip positions are used to compute the orientation of the
// body.  This provides support for point-based tracking systems.
// ------------------------------------------------------------------------
vsWalkInPlace::vsWalkInPlace(vsMotionTracker *lHip, vsMotionTracker *rHip, 
                             vsMotionTracker *left, vsMotionTracker *right, 
                             vsKinematics *kin)
             : vsMotionModel()
{
    // Save the tracker and kinematics objects
    backTracker = NULL;
    lFootTracker = left;
    rFootTracker = right;
    lHipTracker = lHip;
    rHipTracker = rHip;
    kinematics = kin;

    // Complain if any trackers are invalid
    if ((lHip == NULL) || (rHip == NULL) || 
        (lFootTracker == NULL) || (rFootTracker ==  NULL))
    {
        printf("vsWalkInPlace::vsWalkInPlace:  WARNING -- NULL motion "
            "tracker(s) specified!\n");
    }

    // Set movement allowed flags
    forwardAllowed = true;
    backwardAllowed = true;
    sideStepAllowed = true;

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
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsWalkInPlace::getClassName()
{
    return "vsWalkInPlace";
}

// ------------------------------------------------------------------------
// Enables forward motion
// ------------------------------------------------------------------------
void vsWalkInPlace::enableForward()
{
    forwardAllowed = true;
}

// ------------------------------------------------------------------------
// Disables forward motion
// ------------------------------------------------------------------------
void vsWalkInPlace::disableForward()
{
    forwardAllowed = false;
}

// ------------------------------------------------------------------------
// Enables backward motion
// ------------------------------------------------------------------------
void vsWalkInPlace::enableBackward()
{
    backwardAllowed = true;
}

// ------------------------------------------------------------------------
// Disables forward motion
// ------------------------------------------------------------------------
void vsWalkInPlace::disableBackward()
{
    backwardAllowed = false;
}

// ------------------------------------------------------------------------
// Enables side step motion
// ------------------------------------------------------------------------
void vsWalkInPlace::enableSideStep()
{
    sideStepAllowed = true;
}

// ------------------------------------------------------------------------
// Disables side step motion
// ------------------------------------------------------------------------
void vsWalkInPlace::disableSideStep()
{
    sideStepAllowed = false;
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
    movementLimited = true;
}

// ------------------------------------------------------------------------
// Disables the movement allowance check
// ------------------------------------------------------------------------
void vsWalkInPlace::disableMovementLimit()
{
    movementLimited = false;
}

// ------------------------------------------------------------------------
// Updates the motion model
// ------------------------------------------------------------------------
void vsWalkInPlace::update()
{
    atVector             backOrient;
    atVector             lHipPos, rHipPos, lHipFloor;
    atVector             forwardVector;
    atVector             leftFoot, rightFoot;
    double               trackerHeading;
    double               deltaHeading;
    atQuat               headingQuat;
    atVector             separationVec;
    double               deltaX, deltaY, deltaZ;
    double               deltaTime;
    double               p, r;
    atVector             v;
    bool                 motionFlag;
    double               moveSpeed;
    double               moveDistance;
    atQuat               currentOrientation;

    // See whether we're using the hips' positions or the back's orientation,
    // to determine the body's orientation
    if (lHipTracker && rHipTracker)
    {
        // Grab tracker data, including the feet
        lHipPos = lHipTracker->getPositionVec().getScaled(MM_TO_INCHES);
        rHipPos = rHipTracker->getPositionVec().getScaled(MM_TO_INCHES);
        leftFoot = lFootTracker->getPositionVec().getScaled(MM_TO_INCHES);
        rightFoot = rFootTracker->getPositionVec().getScaled(MM_TO_INCHES);
        
        // Get projection on floor of left hip position
        lHipFloor = lHipPos;
        lHipFloor[AT_Z] = leftFoot[AT_Z];
    
        // Calculate the forward vector and get the body's heading
        forwardVector = 
            (rHipPos - lHipPos).getCrossProduct(lHipFloor - lHipPos);
        forwardVector.normalize();
        headingQuat.setVecsRotation(atVector(0.0, 1.0, 0.0),
            atVector(0.0, 0.0, 1.0), forwardVector, atVector(0.0, 0.0, 1.0));
        headingQuat.getEulerRotation(
            AT_EULER_ANGLES_ZXY_R, &trackerHeading, &p, &r);
    }
    else
    {
        // Grab tracker data, including the feet
        leftFoot = lFootTracker->getPositionVec();
        rightFoot = rFootTracker->getPositionVec();
        backOrient = backTracker->getOrientationVec(AT_EULER_ANGLES_ZXY_R);
        
        // Get the body's heading using the back tracker data
        trackerHeading = backOrient[AT_H];
        headingQuat.setAxisAngleRotation(0, 0, 1, trackerHeading);
    }


    // Compute the separation distance of the feet in all three axes
    // and rotate the separation vector to align it with the back
    // heading.  This allows us to easily determine precisely how the
    // feet are positioned with respect to the body.
    separationVec.setSize(3);
    separationVec = rightFoot - leftFoot;
    separationVec = headingQuat.getConjugate().rotatePoint(separationVec);

    // Extract the feet separation distances to their x, y, and z
    // scalar components
    deltaX = separationVec[AT_X];
    deltaY = separationVec[AT_Y];
    deltaZ = separationVec[AT_Z];

    // Compute the current heading relative to last frame's heading
    deltaHeading = trackerHeading - lastTrackerHeading;
    lastTrackerHeading = trackerHeading;
    headingQuat.setAxisAngleRotation(0, 0, 1, deltaHeading);

    // Get the difference in time from last frame to this one
    deltaTime = vsTimer::getSystemTimer()->getInterval();

    // Initialize speed, velocity, and the motion flag
    moveSpeed = 0.0;
    motionFlag = false;
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
        motionFlag = true;
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
        motionFlag = true;
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
        motionFlag = true;
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
