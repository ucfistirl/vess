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
//    VESS Module:  vsMouse.c++
//
//    Description:  Class to handle the state of the mouse
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsMouse.h++"
#include <stdio.h>

// ------------------------------------------------------------------------
// Create a vsMouse with the given number of axes and buttons.
// ------------------------------------------------------------------------
vsMouse::vsMouse(int nAxes, int nButtons)
{
    int i;

    // Construct axes and buttons
    numAxes = nAxes;
    for (i = 0; i < VS_MOUSE_MAX_AXES; i++)
    {
        if (i < numAxes)
            axis[i] = new vsInputAxis();
        else
            axis[i] = NULL;
    }
    numButtons = nButtons;
    for (i = 0; i < numButtons; i++)
    {
        if (i < numButtons)
            button[i] = new vsInputButton();
        else
            axis[i] = NULL;
    }
}

// ------------------------------------------------------------------------
// Create a vsMouse with the given number of axes and buttons.  Normalize
// the first two axes based on the given window size
// ------------------------------------------------------------------------
vsMouse::vsMouse(int nAxes, int nButtons, int xSize, int ySize)
{
    int i;

    // Construct axes and buttons
    numAxes = nAxes;
    for (i = 0; i < VS_MOUSE_MAX_AXES; i++)
    {
        // Construct the specified number of axes, initialize the rest
        // of the axes in the axis array to NULL
        if (i < numAxes)
        {
            if (i == 0)
            {
                // Construct the x axis normalized between 0 and xSize
                axis[i] = new vsInputAxis(0, xSize);
            }
            else if (i == 1)
            {
                // Construct the y axis normalized between 0 and ySize
                axis[i] = new vsInputAxis(0, ySize);
            } 
            else
                // Construct additional axes non-normalized
                axis[i] = new vsInputAxis();
        }
        else
            axis[i] = NULL;
    }
    numButtons = nButtons;
    for (i = 0; i < numButtons; i++)
    {
        // Construct the specified number of buttons, initialize the rest
        // of the buttons in the button array to NULL
        if (i < numButtons)
            button[i] = new vsInputButton();
        else
            button[i] = NULL;
    }
}

// ------------------------------------------------------------------------
// Delete the vsInputAxis and vsInputButtons objects.
// ------------------------------------------------------------------------
vsMouse::~vsMouse()
{
    int i;

    // Delete all the axes we created
    for (i = 0; i < numAxes; i++)
    {
        if (axis[i])
            delete axis[i];
    }

    // Delete all the button we created
    for (i = 0; i < numButtons; i++)
    {
        if (button[i])
            delete button[i];
    }
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsMouse::getClassName()
{
    return "vsMouse";
}

// ------------------------------------------------------------------------
// Change the value of the first two vsInputAxes to the given positions.
// ------------------------------------------------------------------------
void vsMouse::moveTo(int xPos, int yPos)
{
    // This function makes no sense unless we have at least two axes
    // (x and y)
    if (numAxes >= 2)
    {
        axis[0]->setPosition(xPos);
        axis[1]->setPosition(yPos);
    }
}

// ------------------------------------------------------------------------
// Return the number of axes on this vsMouse.
// ------------------------------------------------------------------------
int vsMouse::getNumAxes()
{
    return numAxes;
}

// ------------------------------------------------------------------------
// Return the number of buttons on this vsMouse.
// ------------------------------------------------------------------------
int vsMouse::getNumButtons()
{
    return numButtons;
}

// ------------------------------------------------------------------------
// Return the specified vsInputAxis if it exists.
// ------------------------------------------------------------------------
vsInputAxis *vsMouse::getAxis(int index)
{
    if (index < numAxes)
        return axis[index];
    else
        return NULL;
}

// ------------------------------------------------------------------------
// Return the specified vsInputButton if it exists.
// ------------------------------------------------------------------------
vsInputButton *vsMouse::getButton(int index)
{
    if (index < numButtons)
        return button[index];
    else
        return NULL;
}
