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
//    VESS Module:  vsJoystickBox.h++
//
//    Description:  Abstract base class for all joystick interface boxes
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_JOYSTICK_BOX_HPP
#define VS_JOYSTICK_BOX_HPP

#include "vsInputSystem.h++"
#include "vsJoystick.h++"

class VS_IO_DLL vsJoystickBox : public vsInputSystem
{
public:

                          vsJoystickBox();
    virtual               ~vsJoystickBox();

    virtual int           getNumJoysticks() = 0;
    virtual vsJoystick    *getJoystick() = 0;
    virtual vsJoystick    *getJoystick(int index) = 0;
};

#endif
