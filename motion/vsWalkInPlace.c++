#include "vsWalkInPlace.h++"
#include <stdio.h>
#include <math.h>

// ------------------------------------------------------------------------
// Constructor for vsWalkInPlace
// ------------------------------------------------------------------------
vsWalkInPlace::vsWalkInPlace(vsMotionTracker *back, vsMotionTracker *left, 
                             vsMotionTracker *right, vsKinematics *kin)
             : vsMotionModel()
{
    backTracker = back;
    lFootTracker = left;
    rFootTracker = right;
    kinematics = kin;

    if ((backTracker == NULL) || (lFootTracker == NULL) || 
        (rFootTracker == NULL))
    {
        printf("vsWalkInPlace::vsWalkInPlace:  WARNING -- NULL motion "
            "tracker(s) specified!\n");
    }

    forwardAllowed = VS_TRUE;
    backwardAllowed = VS_TRUE;
    sideStepAllowed = VS_TRUE;

    forwardSpeed = VS_WIP_DEFAULT_FWD_SPD;
    backwardSpeed = VS_WIP_DEFAULT_BCK_SPD;
    sideStepSpeed = VS_WIP_DEFAULT_SS_SPD;

    forwardThresh = VS_WIP_DEFAULT_FWD_THRESH;
    backwardThresh = VS_WIP_DEFAULT_BCK_THRESH;
    sideStepThresh = VS_WIP_DEFAULT_SS_THRESH;

    maxAllowance = VS_WIP_DEFAULT_ALLOWANCE;
    moveAllowance = maxAllowance;
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
// Updates the motion model
// ------------------------------------------------------------------------
void vsWalkInPlace::update()
{
    vsVector             backOrient;
    vsVector             leftFoot, rightFoot;
    double               trackerHeading;
    double               deltaHeading;
    vsQuat               headingQuat;
    vsVector             separationVec, unitVec;
    double               deltaX, deltaY, deltaZ;
    double               deltaTime;
    vsVector             transVec;
    int                  motionFlag;
    double               moveDistance;
    vsMatrix             newTranslation;
    vsMatrix             newRotation;

    // Grab tracker data
    backOrient = backTracker->getOrientationVec(VS_EULER_ANGLES_ZXY_R);
    leftFoot = lFootTracker->getPositionVec();
    rightFoot = rFootTracker->getPositionVec();

    printf("Left foot :  %0.2lf %0.2lf %0.2lf\n", leftFoot[VS_X],
        leftFoot[VS_Y], leftFoot[VS_Z]);
    printf("Right foot:  %0.2lf %0.2lf %0.2lf\n", rightFoot[VS_X],
        rightFoot[VS_Y], rightFoot[VS_Z]);
    printf("Back hdg  :  %0.2lf\n", backOrient[VS_H]);

    // Extract the essential data
    trackerHeading = backOrient[VS_H];
    headingQuat.setAxisAngleRotation(0, 0, 1, -trackerHeading);
    separationVec.setSize(3);
    separationVec[VS_X] = rightFoot[VS_X] - leftFoot[VS_X];
    separationVec[VS_Y] = rightFoot[VS_Y] - leftFoot[VS_Y];
    separationVec[VS_Z] = rightFoot[VS_Z] - leftFoot[VS_Z];
    separationVec = headingQuat.rotatePoint(separationVec);
    deltaX = separationVec[VS_X];
    deltaY = separationVec[VS_Y];
    deltaZ = separationVec[VS_Z];

    // Compute the current heading relative to last frame's heading
    deltaHeading = trackerHeading - lastTrackerHeading;
    lastTrackerHeading = trackerHeading;

    headingQuat.setAxisAngleRotation(1, 0, 0, deltaHeading);

    printf("heading = %0.2f  deltaX = %0.2f  deltaY = %0.2f  deltaZ = %0.2f\n",
           trackerHeading, deltaX, deltaY, deltaZ);

    // Get the difference in time from last frame to this one
    deltaTime = getTimeInterval();

    moveDistance = 0.0;
    motionFlag = VS_FALSE;
    transVec.setSize(3);
    transVec.clear();

    // Figure out what kind of motion we want
    if ((deltaX < sideStepThresh) && (sideStepAllowed))
    {
        // Feet are crossed, therefore sidestep motion
        moveDistance = deltaTime * sideStepSpeed;

        // Figure direction.  The the Y separation indicates the direction 
        // to travel, i.e if the right foot is in front of the left, we 
        // should sidestep left)
        if (deltaY < 0)
        {
            transVec.set(moveDistance, 0.0, 0.0);
            printf("Stepping RIGHT %0.2lf\n", moveDistance);
        }
        else
        {
            transVec.set(-moveDistance, 0.0, 0.0);
            printf("Stepping LEFT %0.2lf\n", moveDistance);
        }

        motionFlag = VS_TRUE;
    }
    else if ((fabs(deltaY) > backwardThresh) && (backwardAllowed))
    {
        // Backward motion
        moveDistance = deltaTime * backwardSpeed;

        transVec.set(0.0, -moveDistance, 0.0);

        printf("Stepping BACK %0.2lf\n", moveDistance);
        motionFlag = VS_TRUE;
    }
    else if ((fabs(deltaZ) > forwardThresh) && (forwardAllowed))
    {
        // Forward motion
        moveDistance = deltaTime * forwardSpeed;

        transVec.set(0.0, moveDistance, 0.0);

        printf("Stepping FORWARD %0.2lf\n", moveDistance);
        motionFlag = VS_TRUE;
    }

    // Clamp the distance to travel to the movement allowance
    if ((moveDistance > 0.0) && (moveDistance > moveAllowance))
    {
        transVec.normalize();
        transVec.scale(moveAllowance);

        moveAllowance = 0.0;
    }

    // Reset the movement allowance if we stop moving
    if (!motionFlag)
    {
        printf("Not moving\n");
        moveAllowance = maxAllowance;
    }
    else
    {
        moveAllowance -= moveDistance;
    }

    kinematics->modifyPosition(transVec);
    kinematics->preModifyOrientation(headingQuat);
}
