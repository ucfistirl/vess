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
//    VESS Module:  vsJoystick.c++
//
//    Description:  Class to store data for all joystick-type input
//                  devices
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsJoystick.h++"
#include <stdio.h>
#include <string.h>

// ------------------------------------------------------------------------
// Construct a vsJoystick with the specified number of axes and buttons and 
// the give axis extents
// ------------------------------------------------------------------------
vsJoystick::vsJoystick(int nAxes, int nButtons, double axisMin, double axisMax)
          : vsIODevice()
{
    int i;

    // Initialize variables
    numAxes = nAxes;
    numButtons = nButtons;

    // Zero the axis and button arrays
    memset(axis, 0, sizeof(axis));
    memset(button, 0, sizeof(button));

    // Create axes and buttons
    for (i = 0; i < numAxes; i++)
    {
        if (axisMin < axisMax)
            axis[i] = new vsInputAxis(axisMin, axisMax);
        else
            axis[i] = new vsInputAxis();

        axis[i]->ref();
    }
    for (i = 0; i < numButtons; i++)
    {
        button[i] = new vsInputButton();
        button[i]->ref();
    }
}

// ------------------------------------------------------------------------
// Construct a vsJoystick with the specified number of axes and buttons 
// with the axes in non-normalized mode
// ------------------------------------------------------------------------
vsJoystick::vsJoystick(int nAxes, int nButtons)
          : vsIODevice()
{
    int i;

    // Initialize variables
    numAxes = nAxes;
    numButtons = nButtons;

    // Zero the axis and button arrays
    memset(axis, 0, sizeof(axis));
    memset(button, 0, sizeof(button));

    // Create axes and buttons
    for (i = 0; i < numAxes; i++)
    {
        axis[i] = new vsInputAxis();
        axis[i]->ref();
    }
    for (i = 0; i < numButtons; i++)
    {
        button[i] = new vsInputButton();
        button[i]->ref();
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
            vsObject::unrefDelete(axis[i]);
    }
    for (i = 0; i < VS_JS_MAX_BUTTONS; i++)
    {
        if (button[i])
            vsObject::unrefDelete(button[i]);
    }
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsJoystick::getClassName()
{
    return "vsJoystick";
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
    // Validate the index, make sure it refers to one of the joystick's
    // axes
    if ((index >= 0) && (index < numAxes))
    {
        // Return the requested axis
        return axis[index];
    }
    else
    {
        // Invalid axis index
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
    // Validate the index, making sure it refers to one of the joystick's
    // buttons
    if ((index >= 0) && (index < numButtons))
    {
        // Return the requested button
        return button[index];
    }
    else
    {
        // Invalid button index
        return NULL;
    }
}

// ------------------------------------------------------------------------
// Use the current axis values to set the idle position of the axes
// ------------------------------------------------------------------------
void vsJoystick::setIdlePosition()
{
    int i;

    // Call the setIdlePosition() method on each joystick axis
    // to set the joystick's current position as the idle position
    for (i = 0; i < numAxes; i++)
    {
        axis[i]->setIdlePosition();
    }
}

// ------------------------------------------------------------------------
// Sets the axis threshold for all axes of this joystick
// ------------------------------------------------------------------------
void vsJoystick::setThreshold(double newThreshold)
{
    int i;

    // Set the specified threshold value on each joystick axis
    for (i = 0; i < numAxes; i++)
    {
        axis[i]->setThreshold(newThreshold);
    }
}
