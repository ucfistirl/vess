#ifndef VS_JOYSTICK_BOX_HPP
#define VS_JOYSTICK_BOX_HPP

// Abstract base class for all joystick interface boxes

#include "vsInputSystem.h++"
#include "vsJoystick.h++"

class vsJoystickBox : public vsInputSystem
{
public:

                          vsJoystickBox();
                          ~vsJoystickBox();

    virtual int           getNumJoysticks() = 0;
    virtual vsJoystick    *getJoystick() = 0;
    virtual vsJoystick    *getJoystick(int index) = 0;
};

#endif
