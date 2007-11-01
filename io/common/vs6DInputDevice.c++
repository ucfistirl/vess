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
//    VESS Module:  vs6DInputDevice.c++
//
//    Description:  Abstract base class for all 6 DOF input devices
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vs6DInputDevice.h++"
#include <stdlib.h>

// ------------------------------------------------------------------------
// Default constructor
// ------------------------------------------------------------------------
vs6DInputDevice::vs6DInputDevice(void)
{
    // Initialize orientation to identity
    orientation.setAxisAngleRotation(0.0, 0.0, 0.0, 1.0);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vs6DInputDevice::~vs6DInputDevice(void)
{
}

// ------------------------------------------------------------------------
// Returns the number of vsInputAxis instances in this object
// ------------------------------------------------------------------------
int vs6DInputDevice::getNumAxes(void)
{
    // Every 6D input device has three axes to represent position
    return VS_6DINPUT_NUM_AXES;
}

// ------------------------------------------------------------------------
// Returns a pointer to the vsInputAxis specified by the parameter
// ------------------------------------------------------------------------
vsInputAxis *vs6DInputDevice::getAxis(int index)
{
    // Validate and return the requested axis, or NULL if the index is
    // out of bounds
    if (index < VS_6DINPUT_NUM_AXES)
        return &position[index];
    else
        return NULL;
}

// ------------------------------------------------------------------------
// Returns the position of the device in a atVector
// ------------------------------------------------------------------------
atVector vs6DInputDevice::getPositionVec()
{
    atVector vec;
   
    // Create a atVector with the current device position as read from
    // the input axes
    vec.setSize(3);
    vec.set(position[0].getPosition(), position[1].getPosition(),
             position[2].getPosition());

    // Return the position vector
    return vec;
}

// ------------------------------------------------------------------------
// Returns the orientation of the device represented as Euler Angles in a
// atVector
// ------------------------------------------------------------------------
atVector vs6DInputDevice::getOrientationVec(atMathEulerAxisOrder axisOrder)
{
    double   h, p, r;
    atVector vec;
    orientation.getEulerRotation(axisOrder, &h, &p, &r);

    // Create a atVector with the current device orientation represented
    // as Euler angles
    vec.setSize(3);
    vec.set(h, p, r);
   
    // Return the orientation vector
    return vec;
}

// ------------------------------------------------------------------------
// Returns the orientation of the device as a atMatrix
// ------------------------------------------------------------------------
atMatrix vs6DInputDevice::getOrientationMat()
{
    atMatrix mat;

    // Create a atMatrix with the current device orientation
    mat.setQuatRotation(orientation);

    // Return the orientation matrix
    return mat;
}

// ------------------------------------------------------------------------
// Returns the orientation of the device as a atQuat
// ------------------------------------------------------------------------
atQuat vs6DInputDevice::getOrientationQuat()
{
    return orientation;
}
