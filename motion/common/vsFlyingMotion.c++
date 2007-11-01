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
#include "atMatrix.h++"
#include "vsTimer.h++"

// ------------------------------------------------------------------------
// Constructs a flying motion model using a mouse and the default button
// configuration.
// ------------------------------------------------------------------------
vsFlyingMotion::vsFlyingMotion(vsMouse *mouse, vsKinematics *kin)
              : vsMotionModel()
{
    // Initialize class variables
    headingAxis = mouse->getAxis(0);
    pitchAxis = mouse->getAxis(1);
    throttleAxis = NULL;

    // Use the default mouse button configuration
    accelButton = mouse->getButton(0);
    decelButton = mouse->getButton(2);
    stopButton = mouse->getButton(1);
    kinematics = kin;

    // Print an error message if any axes are not normalized
    if (((headingAxis != NULL) && (!headingAxis->isNormalized())) ||
        ((pitchAxis != NULL) && (!pitchAxis->isNormalized())) ||
        ((throttleAxis != NULL) && (!throttleAxis->isNormalized())))
    {
        printf("vsFlyingMotion::vsFlyingMotion:  One or more axes are not "
               "normalized!\n");
    }

    // Set motion defaults
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
    // Initialize class variables
    headingAxis = mouse->getAxis(0);
    pitchAxis = mouse->getAxis(1);
    throttleAxis = NULL;
    accelButton = mouse->getButton(accelButtonIndex);
    decelButton = mouse->getButton(decelButtonIndex);
    stopButton = mouse->getButton(stopButtonIndex);
    kinematics = kin;

    // Print an error message if any axes are not normalized
    if (((headingAxis != NULL) && (!headingAxis->isNormalized())) ||
        ((pitchAxis != NULL) && (!pitchAxis->isNormalized())) ||
        ((throttleAxis != NULL) && (!throttleAxis->isNormalized())))
    {
        printf("vsFlyingMotion::vsFlyingMotion:  One or more axes are not "
               "normalized!\n");
    }

    // Set motion defaults
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
    // Initialize class variables
    headingAxis = headingAx;
    pitchAxis = pitchAx;
    throttleAxis = throttleAx;
    accelButton = NULL;
    decelButton = NULL;
    stopButton = NULL;
    kinematics = kin;

    // Print an error message if any axes are not normalized
    if (((headingAxis != NULL) && (!headingAxis->isNormalized())) ||
        ((pitchAxis != NULL) && (!pitchAxis->isNormalized())) ||
        ((throttleAxis != NULL) && (!throttleAxis->isNormalized())))
    {
        printf("vsFlyingMotion::vsFlyingMotion:  One or more axes are not "
               "normalized!\n");
    }

    // Set motion defaults
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
    // Initialize class variables
    headingAxis = headingAx;
    pitchAxis = pitchAx;
    throttleAxis = NULL;
    accelButton = accelBtn;
    decelButton = decelBtn;
    stopButton = stopBtn;
    kinematics = kin;

    // Print an error message if any axes are not normalized
    if (((headingAxis != NULL) && (!headingAxis->isNormalized())) ||
        ((pitchAxis != NULL) && (!pitchAxis->isNormalized())))
    {
        printf("vsFlyingMotion::vsFlyingMotion:  One or more axes are not "
               "normalized!\n");
    }

    // Set motion defaults
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
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsFlyingMotion::getClassName()
{
    return "vsFlyingMotion";
}

// ------------------------------------------------------------------------
// Returns the current mode setting of each axis.  NULL may be safely
// passed in for any values that aren't needed.
// ------------------------------------------------------------------------
void vsFlyingMotion::getAxisModes(vsFlyingAxisMode *heading,
                                  vsFlyingAxisMode *pitch,
                                  vsFlyingAxisMode *throttle)
{
    // Return the heading axis mode if a valid pointer was passed
    if (heading != NULL)
        *heading = headingMode;

    // Return the pitch axis mode if a valid pointer was passed
    if (pitch != NULL)
        *pitch = pitchMode;

    // Return the throttle axis mode if a valid pointer was passed
    if (throttle != NULL)
        *throttle = throttleMode;
}

// ------------------------------------------------------------------------
// Chanages the axis modes
// ------------------------------------------------------------------------
void vsFlyingMotion::setAxisModes(vsFlyingAxisMode newHeadingMode,
                                  vsFlyingAxisMode newPitchMode,
                                  vsFlyingAxisMode newThrottleMode)
{
    // Change the heading axis mode unless NO_CHANGE is specified
    if (newHeadingMode != VS_FM_MODE_NO_CHANGE)
        headingMode = newHeadingMode;

    // Change the pitch axis mode unless NO_CHANGE is specified
    if (newPitchMode != VS_FM_MODE_NO_CHANGE)
        pitchMode = newPitchMode;

    // Change the throttle axis mode unless NO_CHANGE is specified
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
    atQuat              orn, quat1, quat2;
    atQuat              currentRot;
    double              h, p, r;
    atVector            v;
    double              newH, newP;

    // Get the frame time from the system timer
    interval = vsTimer::getSystemTimer()->getInterval();

    // Get the current rotation
    currentRot = kinematics->getOrientation();   
    currentRot.getEulerRotation(AT_EULER_ANGLES_ZXY_R, &h, &p, &r);

    // Maintain the same heading and pitch, unless a control dictates
    // otherwise
    newH = h;
    newP = p;

    // Handle the heading axis
    if (headingAxis != NULL)
    {
        // Compute the new heading based on the axis mode
        if (headingMode == VS_FM_MODE_INCREMENTAL)
        {
            // Compute the change in heading as a product of the axis value,
            // current turning rate and last frame time interval
            dHeading = -(headingAxis->getPosition()) * turningRate * interval;

            // Apply the change in heading to the current heading
            newH = h + dHeading;
        }
        else
        {
            // Set the new heading directly based on the axis value.  The
            // heading can range between -180 and 180 degrees.
            dHeading = 0;
            newH = (-(headingAxis->getPosition()) * 180.0);
        }
    }

    // Handle the pitch axis
    if (pitchAxis != NULL)
    {
        // Compute the new pitch based on the axis mode
        if (pitchMode == VS_FM_MODE_INCREMENTAL)
        {
            // Compute the change in pitch based on the product of the axis
            // value, current turning rate and time interval of the last
            // frame.
            dPitch = -(pitchAxis->getPosition()) * turningRate * interval;

            // Apply the change in pitch to the current heading
            newP = p + dPitch;
        }
        else
        {
            // Compute the pitch directly from the axis value.  The pitch
            // can range between -89.9 and 89.9 degrees
            dPitch = 0;
            newP = (-(pitchAxis->getPosition()) * 89.9);
        }

        // Make sure the new pitch doesn't reach 90 degrees.  This avoids
        // Euler angle singularity problems
        if (newP > 89.9)
            newP = 89.9;
        if (newP < -89.9)
            newP = -89.9;
    }

    // Combine the heading and pitch to update the orientation
    quat1.setAxisAngleRotation(0, 0, 1, newH);
    quat2.setAxisAngleRotation(1, 0, 0, newP);
    orn = quat1 * quat2;
    kinematics->setOrientation(orn);

    // If we have a throttle axis...
    if (throttleAxis != NULL)
    {
        // Get the new speed from the throttle axis
        if (throttleMode == VS_FM_MODE_INCREMENTAL)
        {
            // Calculate a scalar speed adjustment from the axis value,
            // current acceleration rate, and time interval of the last
            // frame.
            dSpeed = throttleAxis->getPosition() * accelerationRate * interval;

            // Add the speed adjustment to the current speed
            currentSpeed += dSpeed;
        }
        else
        {
            // Compute a new forward speed directly from the axis value and
            // current maximum speed
            currentSpeed = throttleAxis->getPosition() * maxSpeed;
        }
    }

    // If the acceleration button is pressed
    if ((accelButton != NULL) && (accelButton->isPressed()))
    {
        // Increase the speed
        if (throttleMode == VS_FM_MODE_INCREMENTAL)
        {
            // Calculate a scalar speed adjustment from the current 
            // acceleration rate, and time interval of the last frame.
            dSpeed = accelerationRate * interval;

            // Add the speed adjustment to the current speed
            currentSpeed += dSpeed;
        }
        else
        {
            // Absolute throttle mode
            if ((decelButton != NULL) && (decelButton->isPressed()))
            {
                // If both buttons are pressed, treat as a stop
                currentSpeed = 0.0;
            }
            else
            {
                // Only accelerate button pressed.  In absolute mode, this
                // produces maximum speed going forward.
                currentSpeed = maxSpeed;
            }
        }
    }

    // If the deceleration button is pressed
    if ((decelButton != NULL) && (decelButton->isPressed()))
    {
        // Decrease the speed if the accelerate button is pressed
        if (throttleMode == VS_FM_MODE_INCREMENTAL)
        {
            // Calculate a scalar speed adjustment from the current 
            // acceleration rate, and time interval of the last frame.
            // Negate the acceleration rate to produce deceleration.
            dSpeed = -accelerationRate * interval;

            // Add the speed adjustment to the current speed
            currentSpeed += dSpeed;
        }
        else
        {
            // Absolute throttle mode
            if ((accelButton != NULL) && (accelButton->isPressed()))
            {
                // If both buttons are pressed, treat as a stop
                currentSpeed = 0.0;
            }
            else
            {
                // Only decelerate button pressed.  In absolute mode, this
                // means maximum speed in reverse.
                currentSpeed = -maxSpeed;
            }
        }
    }

    // If the stop button is pressed
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
    // orientation.  First, create a velocity vector with the current
    // speed going straight forward.
    v.set(0.0, currentSpeed, 0.0);

    // Now, rotate the velocity vector by the current orientation
    v = orn.rotatePoint(v);

    // Update the linear velocity
    kinematics->modifyVelocity(v);
}
