#include "vsDrivingMotion.h++"

// ------------------------------------------------------------------------
// Constructs a driving motion model using the given input axes
// ------------------------------------------------------------------------
vsDrivingMotion::vsDrivingMotion(vsInputAxis *steeringAxis, 
                                 vsInputAxis *throttleAxis)
{
    steering = steeringAxis;
    throttle = throttleAxis;
    accelButton = NULL;
    decelButton = NULL;
    stopButton = NULL;

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
    maxVelocity = VS_DM_DEFAULT_MAX_VELOCITY;
    
    throttleMode = VS_DM_DEFAULT_THROTTLE_MODE;
    steeringMode = VS_DM_DEFAULT_STEERING_MODE;
}

// ------------------------------------------------------------------------
// Constructs a driving motion model using the given input axis and buttons
// ------------------------------------------------------------------------
vsDrivingMotion::vsDrivingMotion(vsInputAxis *steeringAxis, 
                                 vsInputButton *accelBtn,
                                 vsInputButton *decelBtn,
                                 vsInputButton *stopBtn)
{
    steering = steeringAxis;
    throttle = NULL;
    accelButton = accelBtn;
    decelButton = decelBtn;
    stopButton = stopBtn;

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
    maxVelocity = VS_DM_DEFAULT_MAX_VELOCITY;
    
    throttleMode = VS_DM_DEFAULT_THROTTLE_MODE;
    steeringMode = VS_DM_DEFAULT_STEERING_MODE;
}
// ------------------------------------------------------------------------
// Constructs a driving motion model using a mouse with the default axis
// and button configuration
// ------------------------------------------------------------------------
vsDrivingMotion::vsDrivingMotion(vsMouse *mouse)
{
    steering = mouse->getAxis(0);
    throttle = NULL;
    accelButton = mouse->getButton(0);
    decelButton = mouse->getButton(2);
    stopButton = mouse->getButton(1);

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
    maxVelocity = VS_DM_DEFAULT_MAX_VELOCITY;
    
    throttleMode = VS_DM_DEFAULT_THROTTLE_MODE;
    steeringMode = VS_DM_DEFAULT_STEERING_MODE;
}
// ------------------------------------------------------------------------
// Constructs a driving motion model using a mouse with the given button
// configuration
// ------------------------------------------------------------------------
vsDrivingMotion::vsDrivingMotion(vsMouse *mouse, int accelButtonIndex,
                                 int decelButtonIndex, int stopButtonIndex)
{
    steering = mouse->getAxis(0);
    throttle = NULL;
    accelButton = mouse->getButton(accelButtonIndex);
    decelButton = mouse->getButton(decelButtonIndex);
    stopButton = mouse->getButton(stopButtonIndex);

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
    maxVelocity = VS_DM_DEFAULT_MAX_VELOCITY;
    
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
double vsDrivingMotion::getMaxVelocity()
{
    return maxVelocity;
}

// ------------------------------------------------------------------------
// Changes the current maximum velocity
// ------------------------------------------------------------------------
void vsDrivingMotion::setMaxVelocity(double max)
{
    maxVelocity = max;
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
vsMatrix vsDrivingMotion::update()
{
    double              interval;
    double              dHeading;
    vsVector            dPos;
    vsQuat              dOrn;
    vsMatrix            transMat, rotMat;
    vsMatrix            movement;

    interval = getTimeInterval();

    // Adjust heading according to the current axis mode
    if (steering != NULL)
    {
        if (steeringMode == VS_DM_STEER_RELATIVE)
        {
            dHeading = -(steering->getPosition()) * steeringRate * interval *
                velocity / maxVelocity;
        }
        else
        {
            dHeading = -(steering->getPosition()) * steeringRate * interval;
        }
    }

    // Handle the throttle axis
    if (throttle != NULL)
    {
        if (throttleMode == VS_DM_THROTTLE_ACCELERATION)
        {
            velocity += throttle->getPosition() * accelerationRate *
                interval;
        }
        else
        {
            velocity = throttle->getPosition() * maxVelocity;
        }
    }

    // Handle the buttons
    if ((accelButton != NULL) && (accelButton->isPressed()))
    {
        if (throttleMode == VS_DM_THROTTLE_ACCELERATION)
        {
            velocity += accelerationRate * interval;
        }
        else
        {
            if ((decelButton != NULL) && (decelButton->isPressed()))
            {
                velocity = 0;
            }
            else
            {
                velocity = maxVelocity;
            }
        }
    }

    if ((decelButton != NULL) && (decelButton->isPressed()))
    {
        if (throttleMode == VS_DM_THROTTLE_ACCELERATION)
        {
            velocity -= accelerationRate * interval;
        }
        else
        {
            if ((accelButton != NULL) && (accelButton->isPressed()))
            {
                velocity = 0;
            }
            else
            {
                velocity = -maxVelocity;
            }
        }
    }

    if ((stopButton != NULL) && (stopButton->isPressed()))
    {
        velocity = 0;
    }

    // Clamp the velocity to the maximum velocity
    if (velocity > maxVelocity)
    {
        velocity = maxVelocity;
        velocity = maxVelocity;
    }

    if (velocity < -maxVelocity)
    {
        velocity = -maxVelocity;
    }

    // Update the stored axis values
    if (steering)
        lastSteeringVal = steering->getPosition();
    if (throttle)
        lastThrottleVal = throttle->getPosition();

    // Update the orientation
    dOrn.setAxisAngleRotation(0, 0, 1, dHeading);
    rotMat.setQuatRotation(dOrn);

    // Update the position
    transMat.setTranslation(0.0, velocity * interval, 0.0);

    return rotMat * transMat;
}
