#include "vsFlyingMotion.h++"

// ------------------------------------------------------------------------
// Constructs a flying motion model using a mouse and the default button
// configuration.
// ------------------------------------------------------------------------
vsFlyingMotion::vsFlyingMotion(vsMouse *mouse)
              : vsMotionModel()
{
    headingAxis = mouse->getAxis(0);
    pitchAxis = mouse->getAxis(1);
    throttleAxis = NULL;
    accelButton = mouse->getButton(0);
    decelButton = mouse->getButton(2);
    stopButton = mouse->getButton(1);

    if (((headingAxis != NULL) && (!headingAxis->isNormalized())) ||
        ((pitchAxis != NULL) && (!pitchAxis->isNormalized())) ||
        ((throttleAxis != NULL) && (!throttleAxis->isNormalized())))
    {
        printf("vsFlyingMotion::vsFlyingMotion:  One or more axes are not "
               "normalized!\n");
    }

    lastHeadingAxisVal = 0.0;
    lastPitchAxisVal = 0.0;

    velocity = 0.0;

    accelerationRate = VS_FM_DEFAULT_ACCEL_RATE;
    turningRate = VS_FM_DEFAULT_TURNING_RATE;
    maxVelocity = VS_FM_DEFAULT_MAX_VELOCITY;

    headingMode = VS_FM_DEFAULT_HEADING_MODE;
    pitchMode  = VS_FM_DEFAULT_PITCH_MODE;
    throttleMode = VS_FM_DEFAULT_THROTTLE_MODE;
}

// ------------------------------------------------------------------------
// Constructs a flying motion model with the given control axes
// ------------------------------------------------------------------------
vsFlyingMotion::vsFlyingMotion(vsMouse *mouse, int accelButtonIndex, 
                               int decelButtonIndex, int stopButtonIndex)
              : vsMotionModel()
{
    headingAxis = mouse->getAxis(0);
    pitchAxis = mouse->getAxis(1);
    throttleAxis = NULL;
    accelButton = mouse->getButton(accelButtonIndex);
    decelButton = mouse->getButton(decelButtonIndex);
    stopButton = mouse->getButton(stopButtonIndex);

    if (((headingAxis != NULL) && (!headingAxis->isNormalized())) ||
        ((pitchAxis != NULL) && (!pitchAxis->isNormalized())) ||
        ((throttleAxis != NULL) && (!throttleAxis->isNormalized())))
    {
        printf("vsFlyingMotion::vsFlyingMotion:  One or more axes are not "
               "normalized!\n");
    }

    lastHeadingAxisVal = 0.0;
    lastPitchAxisVal = 0.0;

    velocity = 0.0;

    accelerationRate = VS_FM_DEFAULT_ACCEL_RATE;
    turningRate = VS_FM_DEFAULT_TURNING_RATE;
    maxVelocity = VS_FM_DEFAULT_MAX_VELOCITY;

    headingMode = VS_FM_DEFAULT_HEADING_MODE;
    pitchMode  = VS_FM_DEFAULT_PITCH_MODE;
    throttleMode = VS_FM_DEFAULT_THROTTLE_MODE;
}

// ------------------------------------------------------------------------
// Constructs a flying motion model with the given control axes
// ------------------------------------------------------------------------
vsFlyingMotion::vsFlyingMotion(vsInputAxis *headingAx, vsInputAxis *pitchAx,
                               vsInputAxis *throttleAx)
              : vsMotionModel()
{
    headingAxis = headingAx;
    pitchAxis = pitchAx;
    throttleAxis = throttleAx;
    accelButton = NULL;
    decelButton = NULL;
    stopButton = NULL;

    if (((headingAxis != NULL) && (!headingAxis->isNormalized())) ||
        ((pitchAxis != NULL) && (!pitchAxis->isNormalized())) ||
        ((throttleAxis != NULL) && (!throttleAxis->isNormalized())))
    {
        printf("vsFlyingMotion::vsFlyingMotion:  One or more axes are not "
               "normalized!\n");
    }

    lastHeadingAxisVal = 0.0;
    lastPitchAxisVal = 0.0;

    velocity = 0.0;

    accelerationRate = VS_FM_DEFAULT_ACCEL_RATE;
    turningRate = VS_FM_DEFAULT_TURNING_RATE;
    maxVelocity = VS_FM_DEFAULT_MAX_VELOCITY;

    headingMode = VS_FM_DEFAULT_HEADING_MODE;
    pitchMode  = VS_FM_DEFAULT_PITCH_MODE;
    throttleMode = VS_FM_DEFAULT_THROTTLE_MODE;
}

// ------------------------------------------------------------------------
// Constructs a flying motion model with the given control axes and buttons
// ------------------------------------------------------------------------
vsFlyingMotion::vsFlyingMotion(vsInputAxis *headingAx, vsInputAxis *pitchAx,
                               vsInputButton *accelBtn, 
                               vsInputButton *decelBtn, vsInputButton *stopBtn)
              : vsMotionModel()
{
    headingAxis = headingAx;
    pitchAxis = pitchAx;
    throttleAxis = NULL;
    accelButton = accelBtn;
    decelButton = decelBtn;
    stopButton = stopBtn;

    if (((headingAxis != NULL) && (!headingAxis->isNormalized())) ||
        ((pitchAxis != NULL) && (!pitchAxis->isNormalized())))
    {
        printf("vsFlyingMotion::vsFlyingMotion:  One or more axes are not "
               "normalized!\n");
    }

    lastHeadingAxisVal = 0.0;
    lastPitchAxisVal = 0.0;

    velocity = 0.0;

    accelerationRate = VS_FM_DEFAULT_ACCEL_RATE;
    turningRate = VS_FM_DEFAULT_TURNING_RATE;
    maxVelocity = VS_FM_DEFAULT_MAX_VELOCITY;

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
// Returns the current maximum velocity
// ------------------------------------------------------------------------
double vsFlyingMotion::getMaxVelocity()
{
    return maxVelocity;
}

// ------------------------------------------------------------------------
// Adjusts the maximum velocity
// ------------------------------------------------------------------------
void vsFlyingMotion::setMaxVelocity(double newMax)
{
    maxVelocity = newMax;
}

// ------------------------------------------------------------------------
// Updates the motion model
// ------------------------------------------------------------------------
vsMatrix vsFlyingMotion::update()
{
    double              interval;
    double              dHeading;
    double              dPitch;
    vsVector            dPos;
    vsQuat              dOrn, quat1, quat2;
    vsMatrix            transMat, rotMat;
    vsMatrix            movement;

    interval = getTimeInterval();

    // Adjust heading according to the current axis mode
    if (headingAxis != NULL)
    {
        if (headingMode == VS_FM_MODE_INCREMENTAL)
        {
            dHeading = -(headingAxis->getPosition()) * turningRate * interval;
        }
        else
        {
            dHeading = 
                (-(headingAxis->getPosition() - lastHeadingAxisVal) * 180.0);
        }
    }

    // Adjust pitch according to the current axis mode
    if (pitchAxis != NULL)
    {
        if (pitchMode == VS_FM_MODE_INCREMENTAL)
        {
            dPitch = -(pitchAxis->getPosition()) * turningRate * interval;
        }
        else
        {
            dPitch = (-(pitchAxis->getPosition() - lastPitchAxisVal) * 89.9);
        }
    }

    // Handle the throttle axis
    if (throttleAxis != NULL)
    {
        if (throttleMode == VS_FM_MODE_INCREMENTAL)
        {
            velocity += throttleAxis->getPosition() * accelerationRate * 
                interval;
        }
        else
        {
            velocity = throttleAxis->getPosition() * maxVelocity;
        }
    }

    // Handle the buttons
    if ((accelButton != NULL) && (accelButton->isPressed()))
    {
        if (throttleMode == VS_FM_MODE_INCREMENTAL)
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
        if (throttleMode == VS_FM_MODE_INCREMENTAL)
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
    }
    
    if (velocity < -maxVelocity)
    {
        velocity = -maxVelocity;
    }

    // Update the stored axis values
    lastHeadingAxisVal = headingAxis->getPosition();
    lastPitchAxisVal = pitchAxis->getPosition();

    // Update the orientation
    quat1.setAxisAngleRotation(0, 0, 1, dHeading);
    quat2.setAxisAngleRotation(1, 0, 0, dPitch);
    dOrn = quat1 * quat2;
    rotMat.setQuatRotation(dOrn);

    // Update the position
    transMat.setTranslation(0.0, velocity * interval, 0.0);

    return rotMat * transMat;   
}
