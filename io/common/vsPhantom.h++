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
//    VESS Module:  vsPhantom.c++
//
//    Description:  A class for storing and returning the state of a
//                  PHANToM
//
//    Author(s):    Duvan Cope, Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_PHANTOM_DEVICE_HPP
#define VS_PHANTOM_DEVICE_HPP

#include "vs6DInputDevice.h++"

#define VS_PHANTOM_BUTTONS  1

class vsPhantom: public vs6DInputDevice
{
protected:

    vsInputButton    *button[VS_PHANTOM_BUTTONS];

VS_INTERNAL:

    void        setPosition(vsVector posVec);
    void        setOrientation(vsVector ornVec, vsMathEulerAxisOrder axisOrder);
    void        setOrientation(vsMatrix ornMat);
    void        setOrientation(vsQuat ornQuat);

public:

                             vsPhantom(void);
    virtual                  ~vsPhantom(void);

    virtual const char       *getClassName(void);

    virtual int               getNumButtons(void);
    virtual vsInputButton    *getButton(int index);
};

#endif
