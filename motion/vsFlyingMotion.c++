#include "vsFlyingMotion.h++"
#include <stdio.h>
#include "vsMatrix.h++"

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
    maxVelocity = VS_FM_DEFAULT_MAX_VELOCITY;

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
    maxVelocity = VS_FM_DEFAULT_MAX_VELOCITY;

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
void vsFlyingMotion::update()
{
    double              interval;
    double              dHeading;
    double              dPitch;
    double              dSpeed;
    vsVector            dPos;
    vsQuat              orn, quat1, quat2;
    vsQuat              currentRot;
    double              h, p, r;
    vsVector            v, dv;
    double              newH, newP, newSpd;
    vsMatrix            transMat, rotMat;
    vsMatrix            movement;

    interval = getTimeInterval();

    // Get the current rotation
    currentRot = kinematics->getOrientation();   
    currentRot.getEulerRotation(VS_EULER_ANGLES_ZXY_R, &h, &p, &r);

    // Get the current velocity
    v = kinematics->getVelocity();

    // "Unrotate" the current velocity by the current orientation
    orn = currentRot;
    orn.conjugate();
    v = orn.rotatePoint(v);

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

            if (p > 89.9)
                newP = 89.9;
            if (p < -89.9)
                newP = -89.9;
        }
        else
        {
            dPitch = 0;
            newP = (-(pitchAxis->getPosition()) * 89.9);
        }
    }

    // Update the orientation
    quat1.setAxisAngleRotation(0, 0, 1, newH);
    quat2.setAxisAngleRotation(1, 0, 0, newP);
    orn = quat1 * quat2;
    kinematics->setOrientation(orn);

    // Now, "rerotate" the velocity to the new orientation.
    // This will redirect the velocity by the same amount as the
    // change in orientation.
    v = orn.rotatePoint(v);

    // Get the new speed from the throttle axis
    if (throttleAxis != NULL)
    {
        if (throttleMode == VS_FM_MODE_INCREMENTAL)
        {
            // Calculate a scalar speed adjustment
            dSpeed = throttleAxis->getPosition() * accelerationRate * interval;

            // Compute a velocity adjustment vector from the speed 
            // adjustment
            dv.set(0.0, dSpeed, 0.0);
            dv = orn.rotatePoint(dv);

            // Add the velocity adjustment vector to the current velocity
            // vector
            v += dv;
        }
        else
        {
            // Compute a new velocity vector directly from the axis value
            // and current rotation
            newSpd = throttleAxis->getPosition() * maxVelocity;
            v.set(0.0, newSpd, 0.0);
            v = orn.rotatePoint(v);
        }
    }

    // Get the new speed from the throttle buttons
    if ((accelButton != NULL) && (accelButton->isPressed()))
    {
        if (throttleMode == VS_FM_MODE_INCREMENTAL)
        {
            // Calculate a scalar speed adjustment
            dSpeed = accelerationRate * interval;

            // Compute a velocity adjustment vector from the speed 
            // adjustment
            dv.set(0.0, dSpeed, 0.0);
            dv = orn.rotatePoint(dv);

            // Add the velocity adjustment vector to the current velocity
            // vector
            v += dv;
        }
        else
        {
            if ((decelButton != NULL) && (decelButton->isPressed()))
            {
                // If both buttons are pressed, treat as a stop
                v.clear();
            }
            else
            {
                // Set velocity to max
                v.set(0.0, maxVelocity, 0.0);
                v = orn.rotatePoint(v);
            }
        }
    }

    if ((decelButton != NULL) && (decelButton->isPressed()))
    {
        if (throttleMode == VS_FM_MODE_INCREMENTAL)
        {
            // Calculate a scalar speed adjustment
            dSpeed = -accelerationRate * interval;

            // Compute a velocity adjustment vector from the speed 
            // adjustment
            dv.set(0.0, dSpeed, 0.0);
            dv = orn.rotatePoint(dv);

            // Add the velocity adjustment vector to the current velocity
            // vector
            v += dv;
        }
        else
        {
            if ((accelButton != NULL) && (accelButton->isPressed()))
            {
                // If both buttons are pressed, treat as a stop
                v.clear();
            }
            else
            {
                // Set velocity to negative max
                v.set(0.0, maxVelocity, 0.0);
                v = orn.rotatePoint(v);
            }
        }
    }

    if ((stopButton != NULL) && (stopButton->isPressed()))
    {
        // Set velocity to zero
        v.clear();
    }

    // Clamp the velocity to maximum
    if (v.getMagnitude() > maxVelocity)
    {
        v.normalize();
        v.scale(maxVelocity);
    }

    // Update the linear velocity
    kinematics->setVelocity(v);

    // Clear the angular velocity
    v.clear();
    kinematics->setAngularVelocity(v, 0.0);
}
