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
//    VESS Module:  vsJoystick.h++
//
//    Description:  Class to store data for all joystick-type input
//                  devices
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_SIMPLE_JOYSTICK_HPP
#define VS_SIMPLE_JOYSTICK_HPP

#include "vsInputDevice.h++"

#define VS_JS_MAX_AXES    20
#define VS_JS_MAX_BUTTONS 30

enum VS_IO_DLL
{
    VS_JS_X_AXIS = 0,
    VS_JS_Y_AXIS = 1,
    VS_JS_Z_AXIS = 2,
    VS_JS_T_AXIS = 3
};

class VS_IO_DLL vsJoystick : public vsInputDevice
{
protected:

    // Input axes
    int              numAxes;
    vsInputAxis      *axis[VS_JS_MAX_AXES];

    // Input buttons
    int              numButtons;
    vsInputButton    *button[VS_JS_MAX_BUTTONS];

public:

                vsJoystick(int nAxes, int nButtons, 
                           double axisMin, double axisMax);
                vsJoystick(int nAxes, int nButtons);
    virtual     ~vsJoystick();

    // Inherited methods
    virtual const char       *getClassName();

    // Input device methods
    virtual int              getNumAxes();
    virtual int              getNumButtons();

    virtual vsInputAxis      *getAxis(int index);
    virtual vsInputButton    *getButton(int index);

    // Simple calibration routine
    void        setIdlePosition();

    // Sets up a "sweet spot" at the joystick center by setting
    // the threshold value for all axes
    void        setThreshold(double newThreshold);
};

#endif
