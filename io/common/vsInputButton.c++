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
//    VESS Module:  vsInputButton.c++
//
//    Description:  Class for storing and returning the state of an input
//                  device's button
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsInputButton.h++"
#include <stdio.h>

// ------------------------------------------------------------------------
// Sets up a basic vsInputButton
// ------------------------------------------------------------------------
vsInputButton::vsInputButton(void)
{
    // Initialize variables
    pressed = VS_FALSE;
    doubleClicked = VS_FALSE;
    doubleClickInterval = VS_IB_DBLCLICK_INTERVAL;
    
    // Create the button timer
    buttonTimer = new vsTimer();
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsInputButton::~vsInputButton(void)
{
    delete buttonTimer;
}

// ------------------------------------------------------------------------
// Returns the class name
// ------------------------------------------------------------------------
const char * vsInputButton::getClassName()
{
    return "vsInputButton";
}

// ------------------------------------------------------------------------
// Update function called by this vsInputButton's vsInputDevice every time
// the vsInputDevice is updated
// ------------------------------------------------------------------------
void vsInputButton::update()
{
}

// ------------------------------------------------------------------------
// Returns whether or not the button is pressed
// ------------------------------------------------------------------------
int vsInputButton::isPressed(void)
{
    return pressed;
}

// ------------------------------------------------------------------------
// Returns whether or not the last press was a double-click
// ------------------------------------------------------------------------
int vsInputButton::wasDoubleClicked(void)
{
    return doubleClicked;
}

// ------------------------------------------------------------------------
// Sets the button to the pressed state
// ------------------------------------------------------------------------
void vsInputButton::setPressed(void)
{
    // Don't count this as a press if the button is already pressed
    if (!pressed)
    {
        // Set the button to pressed
        pressed = VS_TRUE;

        // Mark the button press time
        buttonTimer->mark();
        
        // Calculate the time interval between this and the last press,
        // and flag a double-click if the interval is small enough
        if (buttonTimer->getInterval() <= doubleClickInterval)
        {
            doubleClicked = VS_TRUE;
        }
        else
        {
            doubleClicked = VS_FALSE;
        }
    }
}

// ------------------------------------------------------------------------
// Sets the button to the released (not pressed) state
// ------------------------------------------------------------------------
void vsInputButton::setReleased(void)
{
    pressed = VS_FALSE;
}

// ------------------------------------------------------------------------
// Sets the maximum amount of time between two consecutive presses that
// will be considered a double-click
// ------------------------------------------------------------------------
void vsInputButton::setDoubleClickInterval(double interval)
{
    doubleClickInterval = interval;
}
