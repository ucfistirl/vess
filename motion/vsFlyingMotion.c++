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
//    VESS Module:  vsFlyingMotion.c++
//
//    Description:  Motion model for simple flying action (not true
//		    aerodynamic flying)
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsFlyingMotion.h++"
#include <stdio.h>
#include "vsMatrix.h++"
#include "vsSystem.h++"

// ------------------------------------------------------------------------
// Constructs a flying motion model using a mouse and the default button
// configuration.
// ------------------------------------------------------------------------
vsFlyingMotion::vsFlyingMotion(vsMouse *mouse, vsKinematics *kin)
              : vsMotionModel()
{
    headingAxis = mouse->getAxis(0);
    pitchAxis = mouse->getAxis(1);
    throttleAxis = NULL;
    accelButton = mouse->getButton(0);
    decelButton = mouse->getButton(2);
    stopButton = mouse->getButton(1);
    kinematics = kin;

    if (((headingAxis != NULL) && (!headingAxis->isNormalized())) ||
        ((pitchAxis != NULL) && (!pitchAxis->isNormalized())) ||
        ((throttleAxis != NULL) && (!throttleAxis->isNormalized())))
    {
        printf("vsFlyingMotion::vsFlyingMotion:  One or more axes are not "
               "normalized!\n");
    }

    accelerationRate = VS_FM_DEFAULT_ACCEL_RATE;
    turningRate = VS_FM_DEFAULT_TURNING_RATE;
    maxSpeed = VS_FM_DEFAULT_MAX_SPEED;

    currentSpeed = 0.0;

    headingMode = VS_FM_DEFAULT_HEADING_MODE;
    pitchMode  = VS_FM_DEFAULT_PITCH_MODE;
    throttleMode = VS_FM_DEFAULT_THROTTLE_MODE;
}

// ------------------------------------------------------------------------
// Constructs a flying motion model with the given control axes
// ------------------------------------------------------------------------
vsFlyingMotion::vsFlyingMotion(vsMouse *mouse, int accelButtonIndex, 
                               int decelButtonIndex, int stopButtonIndex,
                               vsKinematics *kin)
              : vsMotionModel()
{
    headingAxis = mouse->getAxis(0);
    pitchAxis = mouse->getAxis(1);
    throttleAxis = NULL;
    accelButton = mouse->getButton(accelButtonIndex);
    decelButton = mouse->getButton(decelButtonIndex);
    stopButton = mouse->getButton(stopButtonIndex);
    kinematics = kin;

    if (((headingAxis != NULL) && (!headingAxis->isNormalized())) ||
        ((pitchAxis != NULL) && (!pitchAxis->isNormalized())) ||
        ((throttleAxis != NULL) && (!throttleAxis->isNormalized())))
    {
        printf("vsFlyingMotion::vsFlyingMotion:  One or more axes are not "
               "normalized!\n");
    }

    accelerationRate = VS_FM_DEFAULT_ACCEL_RATE;
    turningRate = VS_FM_DEFAULT_TURNING_RATE;
    maxSpeed = VS_FM_DEFAULT_MAX_SPEED;

    currentSpeed = 0.0;

    headingMode = VS_FM_DEFAULT_HEADING_MODE;
    pitchMode  = VS_FM_DEFAULT_PITCH_MODE;
    throttleMode = VS_FM_DEFAULT_THROTTLE_MODE;
}

// ------------------------------------------------------------------------
// Constructs a flying motion model with the given control axes
// ------------------------------------------------------------------------
vsFlyingMotion::vsFlyingMotion(vsInputAxis *headingAx, vsInputAxis *pitchAx,
                               vsInputAxis *throttleAx, vsKinematics *kin)
              : vsMotionModel()
{
    headingAxis = headingAx;
    pitchAxis = pitchAx;
    throttleAxis = throttleAx;
    accelButton = NULL;
    decelButton = NULL;
    stopButton = NULL;
    kinematics = kin;

    if (((headingAxis != NULL) && (!headingAxis->isNormalized())) ||
        ((pitchAxis != NULL) && (!pitchAxis->isNormalized())) ||
        ((throttleAxis != NULL) && (!throttleAxis->isNormalized())))
    {
        printf("vsFlyingMotion::vsFlyingMotion:  One or more axes are not "
               "normalized!\n");
    }

    accelerationRate = VS_FM_DEFAULT_ACCEL_RATE;
    turningRate = VS_FM_DEFAULT_TURNING_RATE;
    maxSpeed = VS_FM_DEFAULT_MAX_SPEED;

    currentSpeed = 0.0;

    headingMode = VS_FM_DEFAULT_HEADING_MODE;
    pitchMode  = VS_FM_DEFAULT_PITCH_MODE;
    throttleMode = VS_FM_DEFAULT_THROTTLE_MODE;
}

// ------------------------------------------------------------------------
// Constructs a flying motion model with the given control axes and buttons
// ------------------------------------------------------------------------
vsFlyingMotion::vsFlyingMotion(vsInputAxis *headingAx, vsInputAxis *pitchAx,
                               vsInputButton *accelBtn, 
                               vsInputButton *decelBtn, vsInputButton *stopBtn,
                               vsKinematics *kin)
              : vsMotionModel()
{
    headingAxis = headingAx;
    pitchAxis = pitchAx;
    throttleAxis = NULL;
    accelButton = accelBtn;
    decelButton = decelBtn;
    stopButton = stopBtn;
    kinematics = kin;

    if (((headingAxis != NULL) && (!headingAxis->isNormalized())) ||
        ((pitchAxis != NULL) && (!pitchAxis->isNormalized())))
    {
        printf("vsFlyingMotion::vsFlyingMotion:  One or more axes are not "
               "normalized!\n");
    }

    accelerationRate = VS_FM_DEFAULT_ACCEL_RATE;
    turningRate = VS_FM_DEFAULT_TURNING_RATE;
    maxSpeed = VS_FM_DEFAULT_MAX_SPEED;

    currentSpeed = 0.0;

    headingMode = VS_FM_DEFAULT_HEADING_MODE;
    pitchMode  = VS_FM_DEFAULT_PITCH_MODE;
    throttleMode = VS_FM_DEFAULT_THROTTLE_MODE;
}

// ------------------------------------------------------------------------
// Destructor for the flying motion model class
// ------------------------------------------------------------------------
vsFlyingMotion::~vsFlyingMotion()
{

}

// ------------------------------------------------------------------------
// Returns the current mode setting of each axis
// ------------------------------------------------------------------------
void vsFlyingMotion::getAxisModes(vsFlyingAxisMode *heading,
                                  vsFlyingAxisMode *pitch,
                                  vsFlyingAxisMode *throttle)
{
    *heading = headingMode;
    *pitch = pitchMode;
    *throttle = throttleMode;
}

// ------------------------------------------------------------------------
// Chanages the axis modes
// ------------------------------------------------------------------------
void vsFlyingMotion::setAxisModes(vsFlyingAxisMode newHeadingMode,
                                  vsFlyingAxisMode newPitchMode,
                                  vsFlyingAxisMode newThrottleMode)
{
    if (newHeadingMode != VS_FM_MODE_NO_CHANGE)
        headingMode = newHeadingMode;

    if (newPitchMode != VS_FM_MODE_NO_CHANGE)
        pitchMode = newPitchMode;

    if (newThrottleMode != VS_FM_MODE_NO_CHANGE)
        throttleMode = newThrottleMode;
}

// ------------------------------------------------------------------------
// Returns the current acceleration rate for the speed control
// ------------------------------------------------------------------------
double vsFlyingMotion::getAccelerationRate()
{
    return accelerationRate;
}

// ------------------------------------------------------------------------
// Adjusts the acceleration rate
// ------------------------------------------------------------------------
void vsFlyingMotion::setAccelerationRate(double newRate)
{
    accelerationRate = newRate;
}

// ------------------------------------------------------------------------
// Returns the current turning rate for the orientation controls
// ------------------------------------------------------------------------
double vsFlyingMotion::getTurningRate()
{
    return turningRate;
}

// ------------------------------------------------------------------------
// Adjusts the acceleration rate
// ------------------------------------------------------------------------
void vsFlyingMotion::setTurningRate(double newRate)
{
    turningRate = newRate;
}

// ------------------------------------------------------------------------
// Returns the current maximum forward velocity
// ------------------------------------------------------------------------
double vsFlyingMotion::getMaxSpeed()
{
    return maxSpeed;
}

// ------------------------------------------------------------------------
// Adjusts the maximum forward velocity
// ------------------------------------------------------------------------
void vsFlyingMotion::setMaxSpeed(double newMax)
{
    maxSpeed = newMax;
}

// ------------------------------------------------------------------------
// Updates the motion model
// ------------------------------------------------------------------------
void vsFlyingMotion::update()
{
    double              interval;
    double              dHeading, dPitch, dSpeed;
    vsQuat              orn, quat1, quat2;
    vsQuat              currentRot;
    double              h, p, r;
    vsVector            v;
    double              newH, newP;

    // Get the frame time from the vsSystem object
    interval = vsSystem::systemObject->getFrameTime();

    // Get the current rotation
    currentRot = kinematics->getOrientation();   
    currentRot.getEulerRotation(VS_EULER_ANGLES_ZXY_R, &h, &p, &r);

    // Maintain the same heading and pitch, unless a control dictates
    // otherwise
    newH = h;
    newP = p;

    // Get the new heading
    if (headingAxis != NULL)
    {
        if (headingMode == VS_FM_MODE_INCREMENTAL)
        {
            dHeading = -(headingAxis->getPosition()) * turningRate * interval;
            newH = h + dHeading;
        }
        else
        {
            dHeading = 0;
            newH = (-(headingAxis->getPosition()) * 180.0);
        }
    }

    // Get the new pitch
    if (pitchAxis != NULL)
    {
        if (pitchMode == VS_FM_MODE_INCREMENTAL)
        {
            dPitch = -(pitchAxis->getPosition()) * turningRate * interval;
            newP = p + dPitch;
        }
        else
        {
            dPitch = 0;
            newP = (-(pitchAxis->getPosition()) * 89.9);
        }

        // Make sure the new pitch doesn't reach 90 degrees
        if (newP > 89.9)
            newP = 89.9;
        if (newP < -89.9)
            newP = -89.9;
    }

    // Update the orientation
    quat1.setAxisAngleRotation(0, 0, 1, newH);
    quat2.setAxisAngleRotation(1, 0, 0, newP);
    orn = quat1 * quat2;
    kinematics->setOrientation(orn);

    // Get the new speed from the throttle axis
    if (throttleAxis != NULL)
    {
        if (throttleMode == VS_FM_MODE_INCREMENTAL)
        {
            // Calculate a scalar speed adjustment and add it to the current
            // speed
            dSpeed = throttleAxis->getPosition() * accelerationRate * interval;
            currentSpeed += dSpeed;
        }
        else
        {
            // Compute a new forward speed directly from the axis value
            currentSpeed = throttleAxis->getPosition() * maxSpeed;
        }
    }

    // Get the new speed from the throttle buttons
    if ((accelButton != NULL) && (accelButton->isPressed()))
    {
        if (throttleMode == VS_FM_MODE_INCREMENTAL)
        {
            // Calculate a scalar speed adjustment add add it to the current
            // speed
            dSpeed = accelerationRate * interval;
            currentSpeed += dSpeed;
        }
        else
        {
            if ((decelButton != NULL) && (decelButton->isPressed()))
            {
                // If both buttons are pressed, treat as a stop
                currentSpeed = 0.0;
            }
            else
            {
                // Set speed to max
                currentSpeed = maxSpeed;
            }
        }
    }

    if ((decelButton != NULL) && (decelButton->isPressed()))
    {
        if (throttleMode == VS_FM_MODE_INCREMENTAL)
        {
            // Calculate a scalar speed adjustment and add it to the current
            // speed
            dSpeed = -accelerationRate * interval;
            currentSpeed += dSpeed;
        }
        else
        {
            if ((accelButton != NULL) && (accelButton->isPressed()))
            {
                // If both buttons are pressed, treat as a stop
                currentSpeed = 0.0;
            }
            else
            {
                // Set speed to negative max
                currentSpeed = -maxSpeed;
            }
        }
    }

    if ((stopButton != NULL) && (stopButton->isPressed()))
    {
        // Set speed to zero
        currentSpeed = 0.0;
    }

    // Clamp the velocity to maximum (or negative max)
    if (currentSpeed > maxSpeed)
    {
        currentSpeed = maxSpeed;
    }
    if (currentSpeed < -maxSpeed)
    {
        currentSpeed = -maxSpeed;
    }

    // Calculate the current velocity vector from the current speed and
    // orientation
    v.set(0.0, currentSpeed, 0.0);
    v = orn.rotatePoint(v);

    // Update the linear velocity
    kinematics->modifyVelocity(v);
}
