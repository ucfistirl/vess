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
//    VESS Module:  vsSpaceball.c++
//
//    Description:  A class for storing and returning the state of a
//                  spaceball
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsSpaceball.h++"
#include <stdio.h>

// ------------------------------------------------------------------------
// Find and initialize the Spaceball on the given window
// ------------------------------------------------------------------------
vsSpaceball::vsSpaceball(int nButtons)
           : vs6DInputDevice()
{
    int i;
  
    numButtons = nButtons;

    if (numButtons > VS_SB_MAX_BUTTONS)
        numButtons = VS_SB_MAX_BUTTONS;

    // Construct the buttons
    for (i = 0; i < numButtons; i++)
    {
        button[i] = new vsInputButton();
    }
}

// ------------------------------------------------------------------------
// Release the spaceball device 
// ------------------------------------------------------------------------
vsSpaceball::~vsSpaceball(void)
{
    int i;

    for (i = 0; i < numButtons; i++)
    {
        if (button[i])
            delete button[i];
    }
}

// ------------------------------------------------------------------------
// Set the position of the Spaceball
// ------------------------------------------------------------------------
void vsSpaceball::setPosition(vsVector posVec)
{
    int i;

    for (i = 0; i < 3; i++)
    {
        position[i].setPosition(posVec.getValue(i));
    }
}

// ------------------------------------------------------------------------
// Set the orientation of the Spaceball
// ------------------------------------------------------------------------
void vsSpaceball::setOrientation(vsVector ornVec, 
                                 vsMathEulerAxisOrder axisOrder)
{
    orientation.setEulerRotation(axisOrder, ornVec.getValue(0),
                                 ornVec.getValue(1), ornVec.getValue(2));
}

// ------------------------------------------------------------------------
// Set the orientation of the Spaceball
// ------------------------------------------------------------------------
void vsSpaceball::setOrientation(vsMatrix ornMat)
{
    orientation.setMatrixRotation(ornMat);
}

// ------------------------------------------------------------------------
// Set the orientation of the Spaceball
// ------------------------------------------------------------------------
void vsSpaceball::setOrientation(vsQuat ornQuat)
{
    orientation = ornQuat;
}

// ------------------------------------------------------------------------
// Return the number of buttons on the spaceball
// ------------------------------------------------------------------------
int vsSpaceball::getNumButtons()
{
    return numButtons;
}

// ------------------------------------------------------------------------
// Return the button at the given index
// ------------------------------------------------------------------------
vsInputButton *vsSpaceball::getButton(int index)
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
