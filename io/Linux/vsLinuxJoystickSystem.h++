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

#include "vsInputSystem.h++"
#include "vsJoystick.h++"

#include <linux/joystick.h>

#define VS_LINUX_JS_AXIS_MIN       -32767
#define VS_LINUX_JS_AXIS_MAX        32767

#define VS_LINUX_JS_BUTTON_PRESSED  1
#define VS_LINUX_JS_BUTTON_RELEASED 0

class vsLinuxJoystickSystem : public vsInputSystem
{
protected:

    // Joystick port name and file descriptor
    char               portName[256];
    int                portFileDescriptor;

    // Structure to hold the current joystick data
    // (struct js_event is defined in linux/joystick.h)
    struct js_event    joystickEvent;

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
