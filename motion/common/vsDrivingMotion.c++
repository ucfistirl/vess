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
//    VESS Module:  vsDrivingMotion.c++
//
//    Description:  Motion model for simple driving action
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsDrivingMotion.h++"
#include <stdio.h>
#include "atMatrix.h++"
#include "vsTimer.h++"

// ------------------------------------------------------------------------
// Constructs a driving motion model using the given input axes
// ------------------------------------------------------------------------
vsDrivingMotion::vsDrivingMotion(vsInputAxis *steeringAxis, 
                                 vsInputAxis *throttleAxis, 
                                 vsKinematics *kin)
{
    // Initialize class variables
    steering = steeringAxis;
    throttle = throttleAxis;
    accelButton = NULL;
    decelButton = NULL;
    stopButton = NULL;
    kinematics = kin;

    // Make sure all the axes passed in are in normalized mode
    if (((steering != NULL) && (!steering->isNormalized())) ||
        ((throttle != NULL) && (!throttle->isNormalized())))
    {
        printf("vsDrivingMotion::vsDrivingMotion:  One or more axes are not "
            "normalized\n");
    }

    // Set the motion parameters to defaults
    accelerationRate = VS_DM_DEFAULT_ACCEL_RATE;
    steeringRate = VS_DM_DEFAULT_STEER_RATE;
    maxForwardSpeed = VS_DM_DEFAULT_MAX_SPEED;
    maxReverseSpeed = VS_DM_DEFAULT_MAX_SPEED;
    currentSpeed = 0.0;
    throttleMode = VS_DM_DEFAULT_THROTTLE_MODE;
    steeringMode = VS_DM_DEFAULT_STEERING_MODE;
}

// ------------------------------------------------------------------------
// Constructs a driving motion model using the given input axis and buttons
// ------------------------------------------------------------------------
vsDrivingMotion::vsDrivingMotion(vsInputAxis *steeringAxis, 
                                 vsInputButton *accelBtn,
                                 vsInputButton *decelBtn,
                                 vsInputButton *stopBtn,
                                 vsKinematics *kin)
{
    // Initialize class variables
    steering = steeringAxis;
    throttle = NULL;
    accelButton = accelBtn;
    decelButton = decelBtn;
    stopButton = stopBtn;
    kinematics = kin;

    // Make sure all the axes passed in are in normalized mode
    if (((steering != NULL) && (!steering->isNormalized())) ||
        ((throttle != NULL) && (!throttle->isNormalized())))
    {
        printf("vsDrivingMotion::vsDrivingMotion:  One or more axes are not "
            "normalized\n");
    }

    // Set the motion parameters to defaults
    accelerationRate = VS_DM_DEFAULT_ACCEL_RATE;
    steeringRate = VS_DM_DEFAULT_STEER_RATE;
    maxForwardSpeed = VS_DM_DEFAULT_MAX_SPEED;
    maxReverseSpeed = VS_DM_DEFAULT_MAX_SPEED;
    currentSpeed = 0.0;
    throttleMode = VS_DM_DEFAULT_THROTTLE_MODE;
    steeringMode = VS_DM_DEFAULT_STEERING_MODE;
}
// ------------------------------------------------------------------------
// Constructs a driving motion model using a mouse with the default axis
// and button configuration
// ------------------------------------------------------------------------
vsDrivingMotion::vsDrivingMotion(vsMouse *mouse, vsKinematics *kin)
{
    // Initialize class variables
    steering = mouse->getAxis(0);
    throttle = NULL;

    // Use a default button configuration
    accelButton = mouse->getButton(0);
    decelButton = mouse->getButton(2);
    stopButton = mouse->getButton(1);
    kinematics = kin;

    // Make sure all the axes passed in are in normalized mode
    if (((steering != NULL) && (!steering->isNormalized())) ||
        ((throttle != NULL) && (!throttle->isNormalized())))
    {
        printf("vsDrivingMotion::vsDrivingMotion:  One or more axes are not "
            "normalized\n");
    }

    // Set the motion parameters to defaults
    accelerationRate = VS_DM_DEFAULT_ACCEL_RATE;
    steeringRate = VS_DM_DEFAULT_STEER_RATE;
    maxForwardSpeed = VS_DM_DEFAULT_MAX_SPEED;
    maxReverseSpeed = VS_DM_DEFAULT_MAX_SPEED;
    currentSpeed = 0.0;
    throttleMode = VS_DM_DEFAULT_THROTTLE_MODE;
    steeringMode = VS_DM_DEFAULT_STEERING_MODE;
}
// ------------------------------------------------------------------------
// Constructs a driving motion model using a mouse with the given button
// configuration
// ------------------------------------------------------------------------
vsDrivingMotion::vsDrivingMotion(vsMouse *mouse, int accelButtonIndex,
                                 int decelButtonIndex, int stopButtonIndex,
                                 vsKinematics *kin)
{
    // Initialize class variables
    steering = mouse->getAxis(0);
    throttle = NULL;

    // Use the given button index
    accelButton = mouse->getButton(accelButtonIndex);
    decelButton = mouse->getButton(decelButtonIndex);
    stopButton = mouse->getButton(stopButtonIndex);
    kinematics = kin;

    // Make sure all the axes passed in are in normalized mode
    if (((steering != NULL) && (!steering->isNormalized())) ||
        ((throttle != NULL) && (!throttle->isNormalized())))
    {
        printf("vsDrivingMotion::vsDrivingMotion:  One or more axes are not "
            "normalized\n");
    }

    // Set the motion parameters to defaults
    accelerationRate = VS_DM_DEFAULT_ACCEL_RATE;
    steeringRate = VS_DM_DEFAULT_STEER_RATE;
    maxForwardSpeed = VS_DM_DEFAULT_MAX_SPEED;
    maxReverseSpeed = VS_DM_DEFAULT_MAX_SPEED;
    currentSpeed = 0.0;
    throttleMode = VS_DM_DEFAULT_THROTTLE_MODE;
    steeringMode = VS_DM_DEFAULT_STEERING_MODE;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsDrivingMotion::~vsDrivingMotion()
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsDrivingMotion::getClassName()
{
    return "vsDrivingMotion";
}

// ------------------------------------------------------------------------
// Returns the current throttle control type (velocity or acceleration)
// ------------------------------------------------------------------------
vsDMThrottleMode vsDrivingMotion::getThrottleMode()
{
    return throttleMode;
}

// ------------------------------------------------------------------------
// Changes the current throttle control type
// ------------------------------------------------------------------------
void vsDrivingMotion::setThrottleMode(vsDMThrottleMode mode)
{
    throttleMode = mode;
}

// ------------------------------------------------------------------------
// Returns the current maximum acceleration rate
// ------------------------------------------------------------------------
double vsDrivingMotion::getAccelerationRate()
{
    return accelerationRate;
}

// ------------------------------------------------------------------------
// Changes the current maximum acceleration rate
// ------------------------------------------------------------------------
void vsDrivingMotion::setAccelerationRate(double rate)
{
    accelerationRate = rate;
}

// ------------------------------------------------------------------------
// Returns the current maximum velocity
// ------------------------------------------------------------------------
double vsDrivingMotion::getMaxForwardSpeed()
{
    return maxForwardSpeed;
}

// ------------------------------------------------------------------------
// Changes the current maximum velocity
// ------------------------------------------------------------------------
void vsDrivingMotion::setMaxForwardSpeed(double max)
{
    maxForwardSpeed = max;
}

// ------------------------------------------------------------------------
// Returns the current maximum velocity
// ------------------------------------------------------------------------
double vsDrivingMotion::getMaxReverseSpeed()
{
    return maxReverseSpeed;
}

// ------------------------------------------------------------------------
// Changes the current maximum velocity
// ------------------------------------------------------------------------
void vsDrivingMotion::setMaxReverseSpeed(double max)
{
    maxReverseSpeed = max;
}

// ------------------------------------------------------------------------
// Returns the current steering mode (relative to velocity or absolute)
// ------------------------------------------------------------------------
vsDMSteeringMode vsDrivingMotion::getSteeringMode()
{
     return steeringMode;
}

// ------------------------------------------------------------------------
// Changes the current steering mode (relative to velocity or absolute)
// ------------------------------------------------------------------------
void vsDrivingMotion::setSteeringMode(vsDMSteeringMode mode)
{
     steeringMode = mode;
}

// ------------------------------------------------------------------------
// Returns the current maximum steering rate
// ------------------------------------------------------------------------
double vsDrivingMotion::getSteeringRate()
{
    return steeringRate;
}

// ------------------------------------------------------------------------
// Changes the current maximum steering rate
// ------------------------------------------------------------------------
void vsDrivingMotion::setSteeringRate(double rate)
{
    steeringRate = rate;
}

// ------------------------------------------------------------------------
// Updates the motion model
// ------------------------------------------------------------------------
void vsDrivingMotion::update()
{
    double              interval;
    double              dHeading;
    atVector            steeringAxis;
    atVector            v;
    atVector            tempV;
    atQuat              orn;
    atQuat              inverseOrn;
    atVector            dPos;
    atMatrix            transMat, rotMat;
    atMatrix            movement;

    // Get elapsed time
    interval = vsTimer::getSystemTimer()->getInterval();

    // If we have a valid steering axis
    if (steering != NULL)
    {
        // Adjust heading according to the current axis mode
        if (steeringMode == VS_DM_STEER_RELATIVE)
        {
            dHeading = -(steering->getPosition()) * steeringRate * 
                currentSpeed;
        }
        else
        {
            dHeading = -(steering->getPosition()) * steeringRate;
        }
    }

    // Update the angular velocity
    steeringAxis.set(0, 0, 1);
    kinematics->modifyAngularVelocity(steeringAxis, dHeading);

    // Get the current orientation
    orn = kinematics->getOrientation();

    // Handle the throttle axis
    if (throttle != NULL)
    {
        // Adjust speed according to the current throttle axis mode
        if (throttleMode == VS_DM_THROTTLE_ACCELERATION)
        {
            // Compute the change in speed from the axis position, time 
            // interval and current acceleration rate.  Add the change
            // to the current speed.
            currentSpeed += throttle->getPosition() * accelerationRate *
                interval;
        }
        else
        {
            // Compute the new speed from the axis position and maximum
            // speed values
            if (throttle->getPosition() > 0.0)
                currentSpeed = throttle->getPosition() * maxForwardSpeed;
            else
                currentSpeed = throttle->getPosition() * maxReverseSpeed;
        }
    }

    // Handle the buttons
    if ((accelButton != NULL) && (accelButton->isPressed()))
    {
        // Adjust speed according to the button state and the current
        // throttle axis mode
        if (throttleMode == VS_DM_THROTTLE_ACCELERATION)
        {
            // Scale the acceleration rate by the frame time interval and add
            // this value to the current speed.
            currentSpeed += accelerationRate * interval;
        }
        else
        {
            if ((decelButton != NULL) && (decelButton->isPressed()))
            {
                // Set speed to zero if both accelerate and decelerate
                // buttons are pressed
                currentSpeed = 0;
            }
            else
            {
                // Only the acceleration button pressed.  In velocity mode
                // this sets the speed to maximum
                currentSpeed = maxForwardSpeed;
            }
        }
    }

    if ((decelButton != NULL) && (decelButton->isPressed()))
    {
        // Adjust speed according to the button state and the current
        // throttle axis mode
        if (throttleMode == VS_DM_THROTTLE_ACCELERATION)
        {
            // Scale the acceleration rate by the frame time interval and
            // subtract this value from the current speed
            currentSpeed -= accelerationRate * interval;
        }
        else
        {
            if ((accelButton != NULL) && (accelButton->isPressed()))
            {
                // Set speed to zero if both accelerate and decelerate
                // buttons are pressed
                currentSpeed = 0;
            }
            else
            {
                // Only the deceleration button pressed.  In velocity mode,
                // this sets the speed to maximum in reverse.
                currentSpeed = -maxReverseSpeed;
            }
        }
    }

    // Stop if the stop button is pressed
    if ((stopButton != NULL) && (stopButton->isPressed()))
    {
        currentSpeed = 0;
    }

    // Clamp the velocity to the maximum velocity
    if (currentSpeed > maxForwardSpeed)
    {
        currentSpeed = maxForwardSpeed;
    }
    if (currentSpeed < -maxReverseSpeed)
    {
        currentSpeed = -maxReverseSpeed;
    }

    // Factor in the adjusted velocity
    tempV[AT_Y] = currentSpeed;
    v = orn.rotatePoint(tempV);

    // Modify the kinematics velocity
    kinematics->setVelocity(v);
}
