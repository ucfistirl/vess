#include "vsDrivingMotion.h++"
#include <stdio.h>
#include "vsMatrix.h++"
#include "vsSystem.h++"

// ------------------------------------------------------------------------
// Constructs a driving motion model using the given input axes
// ------------------------------------------------------------------------
vsDrivingMotion::vsDrivingMotion(vsInputAxis *steeringAxis, 
                                 vsInputAxis *throttleAxis, 
                                 vsKinematics *kin)
{
    steering = steeringAxis;
    throttle = throttleAxis;
    accelButton = NULL;
    decelButton = NULL;
    stopButton = NULL;
    kinematics = kin;

    if (((steering != NULL) && (!steering->isNormalized())) ||
        ((throttle != NULL) && (!throttle->isNormalized())))
    {
        printf("vsDrivingMotion::vsDrivingMotion:  One or more axes are not "
            "normalized\n");
    }

    accelerationRate = VS_DM_DEFAULT_ACCEL_RATE;
    steeringRate = VS_DM_DEFAULT_STEER_RATE;
    maxSpeed = VS_DM_DEFAULT_MAX_SPEED;
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
    steering = steeringAxis;
    throttle = NULL;
    accelButton = accelBtn;
    decelButton = decelBtn;
    stopButton = stopBtn;
    kinematics = kin;

    if (((steering != NULL) && (!steering->isNormalized())) ||
        ((throttle != NULL) && (!throttle->isNormalized())))
    {
        printf("vsDrivingMotion::vsDrivingMotion:  One or more axes are not "
            "normalized\n");
    }

    lastSteeringVal = 0.0;
    lastThrottleVal = 0.0;

    accelerationRate = VS_DM_DEFAULT_ACCEL_RATE;
    steeringRate = VS_DM_DEFAULT_STEER_RATE;
    maxSpeed = VS_DM_DEFAULT_MAX_SPEED;
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
    steering = mouse->getAxis(0);
    throttle = NULL;
    accelButton = mouse->getButton(0);
    decelButton = mouse->getButton(2);
    stopButton = mouse->getButton(1);
    kinematics = kin;

    if (((steering != NULL) && (!steering->isNormalized())) ||
        ((throttle != NULL) && (!throttle->isNormalized())))
    {
        printf("vsDrivingMotion::vsDrivingMotion:  One or more axes are not "
            "normalized\n");
    }

    accelerationRate = VS_DM_DEFAULT_ACCEL_RATE;
    steeringRate = VS_DM_DEFAULT_STEER_RATE;
    maxSpeed = VS_DM_DEFAULT_MAX_SPEED;
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
    steering = mouse->getAxis(0);
    throttle = NULL;
    accelButton = mouse->getButton(accelButtonIndex);
    decelButton = mouse->getButton(decelButtonIndex);
    stopButton = mouse->getButton(stopButtonIndex);
    kinematics = kin;

    if (((steering != NULL) && (!steering->isNormalized())) ||
        ((throttle != NULL) && (!throttle->isNormalized())))
    {
        printf("vsDrivingMotion::vsDrivingMotion:  One or more axes are not "
            "normalized\n");
    }

    accelerationRate = VS_DM_DEFAULT_ACCEL_RATE;
    steeringRate = VS_DM_DEFAULT_STEER_RATE;
    maxSpeed = VS_DM_DEFAULT_MAX_SPEED;
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
double vsDrivingMotion::getMaxSpeed()
{
    return maxSpeed;
}

// ------------------------------------------------------------------------
// Changes the current maximum velocity
// ------------------------------------------------------------------------
void vsDrivingMotion::setMaxSpeed(double max)
{
    maxSpeed = max;
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
    vsVector            steeringAxis;
    vsVector            v;
    vsVector            tempV;
    vsQuat              orn;
    vsQuat              inverseOrn;
    vsVector            dPos;
    vsMatrix            transMat, rotMat;
    vsMatrix            movement;

    // Get elapsed time
    interval = vsSystem::systemObject->getFrameTime();

    // Adjust heading according to the current axis mode
    if (steering != NULL)
    {
        if (steeringMode == VS_DM_STEER_RELATIVE)
        {
            dHeading = -(steering->getPosition()) * 45.0 * currentSpeed;
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
        if (throttleMode == VS_DM_THROTTLE_ACCELERATION)
        {
            currentSpeed += throttle->getPosition() * accelerationRate *
                interval;
        }
        else
        {
            currentSpeed = throttle->getPosition() * maxSpeed;
        }
    }

    // Handle the buttons
    if ((accelButton != NULL) && (accelButton->isPressed()))
    {
        if (throttleMode == VS_DM_THROTTLE_ACCELERATION)
        {
            currentSpeed += accelerationRate * interval;
        }
        else
        {
            if ((decelButton != NULL) && (decelButton->isPressed()))
            {
                currentSpeed = 0;
            }
            else
            {
                currentSpeed = maxSpeed;
            }
        }
    }

    if ((decelButton != NULL) && (decelButton->isPressed()))
    {
        if (throttleMode == VS_DM_THROTTLE_ACCELERATION)
        {
            currentSpeed -= accelerationRate * interval;
        }
        else
        {
            if ((accelButton != NULL) && (accelButton->isPressed()))
            {
                currentSpeed = 0;
            }
            else
            {
                currentSpeed = -maxSpeed;
            }
        }
    }

    if ((stopButton != NULL) && (stopButton->isPressed()))
    {
        currentSpeed = 0;
    }

    // Clamp the velocity to the maximum velocity
    if (currentSpeed > maxSpeed)
    {
        currentSpeed = maxSpeed;
    }

    if (currentSpeed < -maxSpeed)
    {
        currentSpeed = -maxSpeed;
    }

    // Factor in the adjusted velocity
    tempV[VS_Y] = currentSpeed;
    v = orn.rotatePoint(tempV);

    // Modify the kinematics velocity
    kinematics->setVelocity(v);
}
