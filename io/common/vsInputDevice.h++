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
//    VESS Module:  vsInputDevice.h++
//
//    Description:  Abstract base class for all VESS input devices
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_INPUT_DEVICE_HPP
#define VS_INPUT_DEVICE_HPP

#include "vsGlobals.h++"
#include "vsUpdatable.h++"
#include "vsInputAxis.h++"
#include "vsInputButton.h++"

class vsInputDevice : public vsUpdatable
{
VS_INTERNAL:

    virtual void             update();

public:

                             vsInputDevice();
    virtual                  ~vsInputDevice();

    virtual int              getNumAxes() = 0;
    virtual int              getNumButtons() = 0;

    virtual vsInputAxis      *getAxis(int index) = 0;
    virtual vsInputButton    *getButton(int index) = 0;
};

#endif
