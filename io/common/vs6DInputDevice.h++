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
//    VESS Module:  vs6DInputDevice.h++
//
//    Description:  Abstract base class for all 6 DOF input devices
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_6D_INPUT_DEVICE_HPP
#define VS_6D_INPUT_DEVICE_HPP

#include "vsVector.h++"
#include "vsMatrix.h++"
#include "vsQuat.h++"
#include "vsInputDevice.h++"
#include "vsInputAxis.h++"

#define VS_6DINPUT_NUM_AXES 3

class VS_IO_DLL vs6DInputDevice : public vsInputDevice
{
protected:

    // 3 axes for position values
    vsInputAxis    position[VS_6DINPUT_NUM_AXES];

    // Quaternion for orientation values
    vsQuat         orientation;   

public:

                           vs6DInputDevice(void);
    virtual                ~vs6DInputDevice(void);

    // Inherited methods
    virtual int            getNumAxes(void);
    virtual vsInputAxis    *getAxis(int index);

    // Accessors
    virtual vsVector       getPositionVec();
    virtual vsVector       getOrientationVec(vsMathEulerAxisOrder axisOrder);
    virtual vsMatrix       getOrientationMat();
    virtual vsQuat         getOrientationQuat();
};

#endif
