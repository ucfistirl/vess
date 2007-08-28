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
//    VESS Module:  vsFPSMotion.c++
//
//    Description:  Motion model for typical first-person shooter motion
//                  control.  Works with either dual analog stick 
//                  controllers, or a single analog stick and a mouse.
//                  Using vsButtonAxis, four keys on the keyboard (up, 
//                  down, left, right or w, s, a, d) can be converted to 
//                  two movement axes, allowing the typical keyboard/mouse 
//                  shooter controls.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsFPSMotion.h++"
#include <stdio.h>
#include "vsMatrix.h++"
#include "vsTimer.h++"

// ------------------------------------------------------------------------
// Constructs an FPS motion model using the given input axes
// ------------------------------------------------------------------------
vsFPSMotion::vsFPSMotion(vsInputAxis *forwardAxis, 
                         vsInputAxis *strafeAxis, 
                         vsInputAxis *headingAxis, 
                         vsInputAxis *pitchAxis, 
                         vsKinematics *rootKin,
                         vsKinematics *viewKin)
{
    // Initialize class variables
    forward = forwardAxis;
    strafe = strafeAxis;
    heading = headingAxis;
    pitch = pitchAxis;
    rootKinematics = rootKin;
    viewKinematics = viewKin;

    // Make sure all the axes passed in are in normalized mode
    if (((forward != NULL) && (!forward->isNormalized())) ||
        ((strafe != NULL) && (!strafe->isNormalized())) ||
        ((heading != NULL) && (!heading->isNormalized())) ||
        ((pitch != NULL) && (!pitch->isNormalized())))
    {
        printf("vsFPSMotion::vsFPSMotion:  One or more axes are not "
            "normalized\n");
    }

    // Set the motion parameters to defaults
    maxForwardSpeed = VS_FPSM_DEFAULT_MAX_SPEED;
    maxReverseSpeed = VS_FPSM_DEFAULT_MAX_SPEED;
    maxStrafeSpeed = VS_FPSM_DEFAULT_MAX_SPEED;
    headingRate = VS_FPSM_DEFAULT_HEADING_RATE;
    pitchRate = VS_FPSM_DEFAULT_PITCH_RATE;
    minPitch = -VS_FPSM_DEFAULT_PITCH_LIMIT;
    maxPitch = VS_FPSM_DEFAULT_PITCH_LIMIT;

    // Set the heading and pitch modes to incremental, since this constructor
    // is more likely to be used by a joystick
    headingMode = VS_FPSM_MODE_INCREMENTAL;
    pitchMode = VS_FPSM_MODE_INCREMENTAL;
}

// ------------------------------------------------------------------------
// Constructs an FPS motion model using a mouse, plus the given input axes
// ------------------------------------------------------------------------
vsFPSMotion::vsFPSMotion(vsInputAxis *forwardAxis, 
                         vsInputAxis *strafeAxis, 
                         vsMouse *mouse, 
                         vsKinematics *rootKin,
                         vsKinematics *viewKin)
{
    // Initialize class variables
    forward = forwardAxis;
    strafe = strafeAxis;
    heading = mouse->getAxis(0);
    pitch = mouse->getAxis(1);
    rootKinematics = rootKin;
    viewKinematics = viewKin;

    // Make sure all the axes passed in are in normalized mode
    if (((forward != NULL) && (!forward->isNormalized())) ||
        ((strafe != NULL) && (!strafe->isNormalized())) ||
        ((heading != NULL) && (!heading->isNormalized())) ||
        ((pitch != NULL) && (!pitch->isNormalized())))
    {
        printf("vsFPSMotion::vsFPSMotion:  One or more axes are not "
            "normalized\n");
    }

    // Set the motion parameters to defaults
    maxForwardSpeed = VS_FPSM_DEFAULT_MAX_SPEED;
    maxReverseSpeed = VS_FPSM_DEFAULT_MAX_SPEED;
    maxStrafeSpeed = VS_FPSM_DEFAULT_MAX_SPEED;
    headingRate = VS_FPSM_DEFAULT_HEADING_RATE;
    pitchRate = VS_FPSM_DEFAULT_PITCH_RATE;
    minPitch = -VS_FPSM_DEFAULT_PITCH_LIMIT;
    maxPitch = VS_FPSM_DEFAULT_PITCH_LIMIT;

    // Set the heading and pitch modes to absolute.  This constructor
    // is used by a mouse, which works better with absolute heading/pitch
    // control.
    headingMode = VS_FPSM_MODE_ABSOLUTE;
    pitchMode = VS_FPSM_MODE_ABSOLUTE;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsFPSMotion::~vsFPSMotion()
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsFPSMotion::getClassName()
{
    return "vsFPSMotion";
}

// ------------------------------------------------------------------------
// Returns the current maximum velocity
// ------------------------------------------------------------------------
double vsFPSMotion::getMaxForwardSpeed()
{
    return maxForwardSpeed;
}

// ------------------------------------------------------------------------
// Changes the current maximum velocity
// ------------------------------------------------------------------------
void vsFPSMotion::setMaxForwardSpeed(double max)
{
    maxForwardSpeed = max;
}

// ------------------------------------------------------------------------
// Returns the current maximum reverse velocity
// ------------------------------------------------------------------------
double vsFPSMotion::getMaxReverseSpeed()
{
    return maxReverseSpeed;
}

// ------------------------------------------------------------------------
// Changes the current maximum reverse velocity
// ------------------------------------------------------------------------
void vsFPSMotion::setMaxReverseSpeed(double max)
{
    maxReverseSpeed = max;
}

// ------------------------------------------------------------------------
// Returns the current maximum strafe velocity
// ------------------------------------------------------------------------
double vsFPSMotion::getMaxStrafeSpeed()
{
    return maxStrafeSpeed;
}

// ------------------------------------------------------------------------
// Changes the current maximum strafe velocity
// ------------------------------------------------------------------------
void vsFPSMotion::setMaxStrafeSpeed(double max)
{
    maxStrafeSpeed = max;
}

// ------------------------------------------------------------------------
// Returns the current heading change rate
// ------------------------------------------------------------------------
double vsFPSMotion::getHeadingRate()
{
    return headingRate;
}

// ------------------------------------------------------------------------
// Changes the current heading change rate
// ------------------------------------------------------------------------
void vsFPSMotion::setHeadingRate(double rate)
{
    headingRate = rate;
}

// ------------------------------------------------------------------------
// Retreive the current heading axis mode (incremental or absolute)
// ------------------------------------------------------------------------
vsFPSMAxisMode vsFPSMotion::getHeadingAxisMode()
{
    return headingMode;
}

// ------------------------------------------------------------------------
// Change the current heading axis mode
// ------------------------------------------------------------------------
void vsFPSMotion::setHeadingAxisMode(vsFPSMAxisMode newMode)
{
    headingMode = newMode;
}

// ------------------------------------------------------------------------
// Returns the current pitch change rate
// ------------------------------------------------------------------------
double vsFPSMotion::getPitchRate()
{
    return pitchRate;
}

// ------------------------------------------------------------------------
// Changes the current pitch change rate
// ------------------------------------------------------------------------
void vsFPSMotion::setPitchRate(double rate)
{
    pitchRate = rate;
}

// ------------------------------------------------------------------------
// Retrieve the current minimum and maximum pitch
// ------------------------------------------------------------------------
void vsFPSMotion::getPitchLimits(double *min, double *max)
{
    // Return the pitch limits, if the variable parameters provided are 
    // valid
    if (min != NULL)
        *min = minPitch;
    if (max != NULL)
        *max = maxPitch;
}

// ------------------------------------------------------------------------
// Changes the current minimum and maximum pitch
// ------------------------------------------------------------------------
void vsFPSMotion::setPitchLimits(double min, double max)
{
    minPitch = min;
    maxPitch = max;
}

// ------------------------------------------------------------------------
// Retreive the current pitch axis mode (incremental or absolute)
// ------------------------------------------------------------------------
vsFPSMAxisMode vsFPSMotion::getPitchAxisMode()
{
    return pitchMode;
}

// ------------------------------------------------------------------------
// Change the current pitch axis mode
// ------------------------------------------------------------------------
void vsFPSMotion::setPitchAxisMode(vsFPSMAxisMode newMode)
{
    pitchMode = newMode;
}

// ------------------------------------------------------------------------
// Updates the motion model
// ------------------------------------------------------------------------
void vsFPSMotion::update()
{
    double   interval;
    double   dHeading, dPitch;
    vsQuat   headingQuat, pitchQuat;
    double   currentPitch, newPitch;
    vsVector deltaV;
    vsQuat   orn;
    vsQuat   inverseOrn;
    vsVector dPos;
    vsMatrix transMat, rotMat;
    vsMatrix movement;

    // Get elapsed time
    interval = vsTimer::getSystemTimer()->getInterval();

    // If we have a valid heading axis
    if (heading != NULL)
    {
        // Compute the desired change in heading, based on the axis mode
        if (headingMode == VS_FPSM_MODE_INCREMENTAL)
            dHeading = -(heading->getPosition()) * headingRate * interval;
        else
            dHeading = -(heading->getDelta()) * headingRate;

        // Update the orientation
        headingQuat.setAxisAngleRotation(0, 0, 1, dHeading);
        rootKinematics->preModifyOrientation(headingQuat);
    }

    // If we have a valid pitch axis
    if (pitch != NULL)
    {
        // Compute the desired change in pitch, based on the axis mode
        if (pitchMode == VS_FPSM_MODE_INCREMENTAL)
            dPitch = -(pitch->getPosition()) * pitchRate * interval;
        else
            dPitch = -(pitch->getDelta()) * pitchRate;


        // Get the current viewKinematics pitch, so we can enforce the pitch
        // restrictions.  First, decompose the orientation quaternion, and get 
        // the quaternion that represents current rotation around the X axis.
        pitchQuat = viewKinematics->getOrientation().
            getDecomposition(vsVector(1,0,0));

        // Now, we can retrieve the degrees rotated around the X axis
        pitchQuat.getAxisAngleRotation(NULL, NULL, NULL, &currentPitch);
        if (currentPitch > 180.0)
            currentPitch -= 360.0;
        if (currentPitch < -180.0)
            currentPitch += 360.0;

        // See if the current pitch change would cause the pitch to exceed
        // a limit, and adjust dPitch to enforce the limits if necessary
        newPitch = currentPitch + dPitch;
        if (newPitch > maxPitch)
            dPitch = maxPitch - currentPitch;
        if (newPitch < minPitch)
            dPitch = minPitch - currentPitch;

        // Redefine the pitch quaternion to hold the new change in pitch, and
        // use it to modify the viewpoint kinematics.
        pitchQuat.setAxisAngleRotation(1, 0, 0, dPitch);
        viewKinematics->postModifyOrientation(pitchQuat);
    }

    // Initialize the linear velocity change to zero
    deltaV.setSize(3);
    deltaV.clear();

    // Handle the forward axis
    if (forward != NULL)
    {
        // Compute the new speed from the axis position and maximum
        // speed values
        if (forward->getPosition() > 0.0)
            deltaV[VS_Y] = forward->getPosition() * maxForwardSpeed;
        else
            deltaV[VS_Y] = forward->getPosition() * maxReverseSpeed;
    }

    // Handle the strafe axis
    if (strafe != NULL)
    {
        // Compute the new speed from the axis position and maximum
        // speed value
        deltaV[VS_X] = strafe->getPosition() * maxStrafeSpeed;
    }

    // Get the current heading
    orn = rootKinematics->getOrientation();
    headingQuat = orn.getDecomposition(vsVector(0,0,1));

    // Rotate the velocity change vector, based on the kinematics current
    // heading
    deltaV = headingQuat.rotatePoint(deltaV);

    // Set the kinematics velocity
    rootKinematics->modifyVelocity(deltaV);
}
