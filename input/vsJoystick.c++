#include "vsJoystick.h++"
#include <stdio.h>

// ------------------------------------------------------------------------
// Construct a vsJoystick with the specified number of axes and buttons and 
// the give axis extents
// ------------------------------------------------------------------------
vsJoystick::vsJoystick(int nAxes, int nButtons, double axisMin, double axisMax)
          : vsInputDevice()
{
    int i;

    // Initialize variables
    numAxes = nAxes;
    numButtons = nButtons;

    // Create axes and buttons
    for (i = 0; i < numAxes; i++)
    {
        if (axisMin < axisMax)
            axis[i] = new vsInputAxis(axisMin, axisMax);
        else
            axis[i] = new vsInputAxis();
    }

    for (i = 0; i < numButtons; i++)
    {
        button[i] = new vsInputButton();
    }
}

// ------------------------------------------------------------------------
// Construct a vsJoystick with the specified number of axes and buttons 
// with the axes in non-normalized mode
// ------------------------------------------------------------------------
vsJoystick::vsJoystick(int nAxes, int nButtons)
          : vsInputDevice()
{
    int i;

    // Initialize variables
    numAxes = nAxes;
    numButtons = nButtons;

    // Create axes and buttons
    for (i = 0; i < numAxes; i++)
    {
        axis[i] = new vsInputAxis();
    }

    for (i = 0; i < numButtons; i++)
    {
        button[i] = new vsInputButton();
    }
}

// ------------------------------------------------------------------------
// Destroy all axes and buttons
// ------------------------------------------------------------------------
vsJoystick::~vsJoystick()
{
    int i;

    // Delete axes and buttons
    for (i = 0; i < VS_JS_MAX_AXES; i++)
    {
        if (axis[i])
            delete axis[i];
    }

    for (i = 0; i < VS_JS_MAX_BUTTONS; i++)
    {
        if (button[i])
            delete button[i];
    }
}

// ------------------------------------------------------------------------
// Return the number of axes
// ------------------------------------------------------------------------
int vsJoystick::getNumAxes()
{
    return numAxes;
}

// ------------------------------------------------------------------------
// Return the specified axis if it exists
// ------------------------------------------------------------------------
vsInputAxis *vsJoystick::getAxis(int index)
{
    if (index < numAxes)
    {
        return axis[index];
    }
    else
    {
        return NULL;
    }
}

// ------------------------------------------------------------------------
// Return the number of buttons
// ------------------------------------------------------------------------
int vsJoystick::getNumButtons()
{
    return numButtons;
}

// ------------------------------------------------------------------------
// Return the specified button if it exists
// ------------------------------------------------------------------------
vsInputButton *vsJoystick::getButton(int index)
{
    if (index < numButtons)
    {
        return button[index];
    }
    else
    {
        return NULL;
    }
}

// ------------------------------------------------------------------------
// Use the current axis values to set the idle position of the axes
// ------------------------------------------------------------------------
void vsJoystick::setIdlePosition()
{
    int i;

    for (i = 0; i < numAxes; i++)
    {
        axis[i]->setIdlePosition();
    }
}
