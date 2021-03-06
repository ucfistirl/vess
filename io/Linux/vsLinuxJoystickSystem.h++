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
//    VESS Module:  vsLinuxJoystickSystem.h++
//
//    Description:  Support for Linux-based joysticks. See
//                  joystick-api.txt in the Linux kernel documentation for
//                  implementation details.
//
//    Author(s):    Carlos Rosas-Anderson
//
//------------------------------------------------------------------------

#ifndef VS_LINUX_JOYSTICK_SYSTEM_HPP
#define VS_LINUX_JOYSTICK_SYSTEM_HPP

#include "vsIOSystem.h++"
#include "vsJoystick.h++"

#include <linux/input.h>

#define VS_LINUX_JS_AXIS_MIN       -128
#define VS_LINUX_JS_AXIS_MAX        127


class vsLinuxJoystickSystem : public vsIOSystem
{
protected:

    // Joystick port name and file descriptor
    char               portName[256];
    int                portFD;

    // Axis and button mappings
    int                firstButton;
    int                axisMap[ABS_MAX];

    // Joystick connected to this system
    vsJoystick         *joystick;

public:

                          vsLinuxJoystickSystem(char *joystickPortName);
    virtual               ~vsLinuxJoystickSystem();

    virtual const char    *getClassName();

    virtual char          *getPortName();
    virtual vsJoystick    *getJoystick();

    virtual void          update();
};

#endif
