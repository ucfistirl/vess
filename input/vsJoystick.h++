#ifndef VS_SIMPLE_JOYSTICK_HPP
#define VS_SIMPLE_JOYSTICK_HPP

// Class to store data for all joystick-type input devices

#include "vsInputDevice.h++"

#define VS_JS_MAX_AXES    4
#define VS_JS_MAX_BUTTONS 4

enum
{
    VS_JS_X_AXIS = 0,
    VS_JS_Y_AXIS = 1,
    VS_JS_Z_AXIS = 2,
    VS_JS_T_AXIS = 3
};

class vsJoystick : public vsInputDevice
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
                ~vsJoystick();

    // Input device methods
    virtual int              getNumAxes();
    virtual int              getNumButtons();

    virtual vsInputAxis      *getAxis(int index);
    virtual vsInputButton    *getButton(int index);

    // Simple calibration routine
    void        setIdlePosition();
};

#endif
