//------------------------------------------------------------------------
//
//    VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//    Copyright (c) 2002, University of Central Florida
//
//       See the file LICENSE for license information
//
//    E-mail:  vess@ist.ucf.edu
//    WWW:     http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//    VESS Module:  vsRelativeMouseMotion.c++
//
//    Description:  Motion model to move based on relative moments of the
//                  given input axes. For example, this gives "Quake-like"
//                  motion to a kinematics object when combined with the
//                  vsMouse from vsWindowSystem.
//
//    Author(s):    Ryan Wilson, Jason Daly
//
//------------------------------------------------------------------------

#include "vsRelativeMouseMotion.h++"
#include <math.h>
#include "vsTimer.h++"
// #include "vsSystem.h++"

vsRelativeMouseMotion::vsRelativeMouseMotion(vsMouse * mouse,
        vsKinematics * kinObj)
{
    kinematics = kinObj;

    // For a mouse, axis 0 is the x axis. axis 1 is the y axis.
    inputAxis[ 0 ] = mouse->getAxis( 0 );
    inputAxis[ 1 ] = mouse->getAxis( 1 );

    // Axis scaling - how many degrees a half-screen movement of the mouse
    // rotates the viewpoint
    axisChange[ 0 ] = 100.0;
    axisChange[ 1 ] = 90.0;

    // Axis limits
    axisLimits[ 0 ] = false;
    axisLimits[ 1 ] = true;
    kinMin[ 1 ] = -90.0;
    kinMax[ 1 ] = 90.0;

    // which axis to rotate around?
    rotationAxis[ 0 ] = VS_Z;
    rotationAxis[ 1 ] = VS_X;

    // see header files - these reflect on the way that the calculated
    // orientation is combined with the current orientation on the kinematics
    // object
    prePost[ 0 ] = 0;
    prePost[ 1 ] = 1;
    
    // Initialize class variables
    throttleAxis = NULL;

    // Use the default mouse button configuration
    accelButton = mouse->getButton(0);
    decelButton = mouse->getButton(2);
    stopButton = mouse->getButton(1);

    // Set motion defaults
    accelerationRate = VS_FM_DEFAULT_ACCEL_RATE;
    maxSpeed = VS_FM_DEFAULT_MAX_SPEED;
    currentSpeed = 0.0;
    throttleMode = VS_FM_DEFAULT_THROTTLE_MODE;

    // Reset the state of the motion model
    reset();
}

vsRelativeMouseMotion::~vsRelativeMouseMotion( )
{
}

const char * vsRelativeMouseMotion::getClassName()
{
    return "vsRelativeMouseMotion";
}

void vsRelativeMouseMotion::setThrottleAxisMode( vsFlyingAxisMode axisMode )
{
    throttleMode = axisMode;
}

vsFlyingAxisMode vsRelativeMouseMotion::getThrottleAxisMode( )
{
    return throttleMode;
}

// ------------------------------------------------------------------------
// Returns the current acceleration rate for the speed control
// ------------------------------------------------------------------------
double vsRelativeMouseMotion::getAccelerationRate()
{
    return accelerationRate;
}

// ------------------------------------------------------------------------
// Adjusts the acceleration rate
// ------------------------------------------------------------------------
void vsRelativeMouseMotion::setAccelerationRate(double newRate)
{
    accelerationRate = newRate;
}

// ------------------------------------------------------------------------
// Returns the current maximum forward velocity
// ------------------------------------------------------------------------
double vsRelativeMouseMotion::getMaxSpeed()
{
    return maxSpeed;
}

// ------------------------------------------------------------------------
// Adjusts the maximum forward velocity
// ------------------------------------------------------------------------
void vsRelativeMouseMotion::setMaxSpeed(double newMax)
{
    maxSpeed = newMax;
}

// ------------------------------------------------------------------------
// Updates the orientation and velocity based on the input axes and buttons
// ------------------------------------------------------------------------
void vsRelativeMouseMotion::update()
{
    updateOrientation();
    updateVelocity();
}

// ------------------------------------------------------------------------
// Updates the orientation and velocity based on the input axes
// ------------------------------------------------------------------------
void vsRelativeMouseMotion::updateOrientation()
{
    // Deal with each axis separetly
    for( int axis=0; axis<NUMBER_OF_AXES; axis++ )
    {
        double delta;
        double currentRotation[3];
        vsQuat q;

        if( inputAxis[axis]==NULL )
            continue;

        // Get the change in the axis
        delta = inputAxis[axis]->getDelta();

        // Scale the relative movement of the axis (getDelta() will return a
        // small number. It is then multiplied by axisChange, a user-definable
        // value)
        delta *= -(axisChange[axis]);

        // Get the current orientation so we can enforce axis limits
        q = kinematics->getOrientation();
        q.getEulerRotation( VS_EULER_ANGLES_XYZ_S,
                &currentRotation[VS_X],
                &currentRotation[VS_Y],
                &currentRotation[VS_Z] );

        // enforce axis limits
        if( axisLimits[axis] )
        {
            double newRotation =
                currentRotation[ rotationAxis[axis] ] + delta;
            if( newRotation >= kinMax[axis] )
                delta = 0.0;
            else if( newRotation <= kinMin[axis] )
                delta = 0.0;
        }

        // set up the rotation for the given axis
        switch( rotationAxis[ axis ] )
        {
            case VS_X:
                q.setAxisAngleRotation( 1.0, 0.0, 0.0, delta );
                break;
            case VS_Y:
                q.setAxisAngleRotation( 0.0, 1.0, 0.0, delta );
                break;
            case VS_Z:
                q.setAxisAngleRotation( 0.0, 0.0, 1.0, delta );
                break;
        }

        // do we apply this axis with pre or post multiplication?
        if( prePost[axis] ) //post
            q = kinematics->getOrientation() * q;
        else //pre
            q = q * kinematics->getOrientation();
       
        // Finally, set the new orientation
        kinematics->setOrientation( q );
    }
}

// ------------------------------------------------------------------------
// If the state needed to be reset, we would do it here.
// ------------------------------------------------------------------------
void vsRelativeMouseMotion::reset( )
{
}

// ------------------------------------------------------------------------
// Used for forcibly setting axis limits (i.e. not allowing the rotation
// around the x axis to go more than -60 degrees to 60 degrees so to limit
// head movement).
// ------------------------------------------------------------------------
void vsRelativeMouseMotion::setAxisLimits( int axis,
        double minLimit, double maxLimit )
{
    if( axis>=0 && axis<NUMBER_OF_AXES )
    {
        if( maxLimit <= minLimit )
            axisLimits[axis] = false;
        else
        {
            axisLimits[axis] = true;
            kinMin[axis] = minLimit;
            kinMax[axis] = maxLimit;
        }
    }
}

// ------------------------------------------------------------------------
// Set How the orientation transformation is applied (see header file)
// ------------------------------------------------------------------------
void vsRelativeMouseMotion::setAxisPrePost( int axis, int isPost )
{
    if( axis>=0 && axis<NUMBER_OF_AXES )
        prePost[axis] = isPost;
}

// ------------------------------------------------------------------------
// Get How the orientation transformation is applied (see header file)
// ------------------------------------------------------------------------
int vsRelativeMouseMotion::getAxisPrePost( int axis )
{
    if( axis>=0 && axis<NUMBER_OF_AXES )
        return prePost[axis];
    else
        return 0;
}

// ------------------------------------------------------------------------
// Set the scaling factor. For every 1/2-axis movement of the mouse (0.0 to
// 1.0), we rotate this many degrees around the given axis.
// ------------------------------------------------------------------------
void vsRelativeMouseMotion::setAxisChange( int axis, double scaleFactor )
{
    if( axis>=0 && axis<NUMBER_OF_AXES )
        axisChange[axis] = scaleFactor;
}

// ------------------------------------------------------------------------
// Get the scaling factor. For every 1/2-axis movement of the mouse (0.0 to
// 1.0), we rotate this many degrees around the given axis.
// ------------------------------------------------------------------------
double vsRelativeMouseMotion::getAxisChange( int axis )
{
    if( axis>=0 && axis<NUMBER_OF_AXES )
        return axisChange[axis];
    else
        return 0.0;
}

// ------------------------------------------------------------------------
// Set the axis which we will rotate around (VS_X, VS_Y, VS_Z)
// ------------------------------------------------------------------------
void vsRelativeMouseMotion::setRotationAxis( int axis, int newRotationAxis )
{
    if( axis>=0 && axis<NUMBER_OF_AXES )
        rotationAxis[axis] = newRotationAxis;
}

// ------------------------------------------------------------------------
// Get the axis which we will rotate around (VS_X, VS_Y, VS_Z)
// ------------------------------------------------------------------------
int vsRelativeMouseMotion::getRotationAxis( int axis )
{
    if( axis>=0 && axis<NUMBER_OF_AXES )
        return rotationAxis[axis];
    else
        return 0;
}

// ------------------------------------------------------------------------
// Just update the velocity
// ------------------------------------------------------------------------
void vsRelativeMouseMotion::updateVelocity()
{
    double              interval;
    double              dSpeed;
    vsQuat              currentRot;
    vsVector            v;

    // Get the frame time from the vsTimer object
    // interval = (vsSystem::systemObject)->getFrameTime();
    interval = vsTimer::getSystemTimer()->getInterval();

    // Get the current rotation
    currentRot = kinematics->getOrientation();   

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
    v = currentRot.rotatePoint(v);

    // Update the linear velocity
    kinematics->modifyVelocity(v);
}
