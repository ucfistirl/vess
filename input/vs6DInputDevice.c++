#include "vs6DInputDevice.h++"
#include <stdlib.h>

// ------------------------------------------------------------------------
// Default constructor
// ------------------------------------------------------------------------
vs6DInputDevice::vs6DInputDevice(void)
{
    orientation.setAxisAngleRotation(1.0, 0.0, 0.0, 0.0);
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
    return VS_6DINPUT_NUM_AXES;
}

// ------------------------------------------------------------------------
// Returns a pointer to the vsInputAxis specified by the parameter
// ------------------------------------------------------------------------
vsInputAxis *vs6DInputDevice::getAxis(int index)
{
    if (index < VS_6DINPUT_NUM_AXES)
        return &position[index];
    else
        return NULL;
}

// ------------------------------------------------------------------------
// Returns the position of the device in a vsVector
// ------------------------------------------------------------------------
vsVector vs6DInputDevice::getPositionVec()
{
    vsVector vec;
   
    vec.setSize(3);
    vec.set(position[0].getPosition(), position[1].getPosition(),
             position[2].getPosition());

    return vec;
}

// ------------------------------------------------------------------------
// Returns the orientation of the device represented as Euler Angles in a
// vsVector
// ------------------------------------------------------------------------
vsVector vs6DInputDevice::getOrientationVec(vsMathEulerAxisOrder axisOrder)
{
    double   h, p, r;
    vsVector vec;
    orientation.getEulerRotation(axisOrder, &h, &p, &r);

    vec.setSize(3);
    vec.set(h, p, r);
   
    return vec;
}

// ------------------------------------------------------------------------
// Returns the orientation of the device as a vsMatrix
// ------------------------------------------------------------------------
vsMatrix vs6DInputDevice::getOrientationMat()
{
    vsMatrix mat;

    mat.setQuatRotation(orientation);

    return mat;
}

// ------------------------------------------------------------------------
// Returns the orientation of the device as a vsQuat
// ------------------------------------------------------------------------
vsQuat vs6DInputDevice::getOrientationQuat()
{
    return orientation;
}
