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

#include "atVector.h++"
#include "atMatrix.h++"
#include "atQuat.h++"
#include "vsIODevice.h++"
#include "vsInputAxis.h++"

#define VS_6DINPUT_NUM_AXES 3

class VESS_SYM vs6DInputDevice : public vsIODevice
{
protected:

    // 3 axes for position values
    vsInputAxis    position[VS_6DINPUT_NUM_AXES];

    // Quaternion for orientation values
    atQuat         orientation;   

public:

                           vs6DInputDevice(void);
    virtual                ~vs6DInputDevice(void);

    // Inherited methods
    virtual int            getNumAxes(void);
    virtual vsInputAxis    *getAxis(int index);

    // Accessors
    virtual atVector       getPositionVec();
    virtual atVector       getOrientationVec(atMathEulerAxisOrder axisOrder);
    virtual atMatrix       getOrientationMat();
    virtual atQuat         getOrientationQuat();
};

#endif
