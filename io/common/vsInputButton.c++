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
    pressed = false;
    pressedState = VS_IB_STABLE;
    releasedState = VS_IB_STABLE;
    doubleClicked = false;
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
// Update function called by this vsInputButton's vsIODevice every time
// the vsIODevice is updated
// ------------------------------------------------------------------------
void vsInputButton::update()
{
    // Update the button press temporal state
    if (pressedState == VS_IB_THIS_FRAME)
    {
        // Update the state to "last frame", meaning that the button
        // was pressed during the frame prior to this update
        pressedState = VS_IB_LAST_FRAME;
    }
    else
    {
        // Update the state to stable, meaning that the button
        // state hasn't changed since the previous update
        pressedState = VS_IB_STABLE;
    }

    // Update the button release temporal state (note that both a press and
    // a release can occur during the same frame)
    if (releasedState == VS_IB_THIS_FRAME)
    {
        // Update the state to "last frame", meaning that the button
        // was released during the frame prior to this update
        releasedState = VS_IB_LAST_FRAME;
    }
    else
    {
        // Update the state to stable, meaning that the button
        // state hasn't changed since the previous update
        releasedState = VS_IB_STABLE;
    }
}

// ------------------------------------------------------------------------
// Returns whether or not the button is pressed
// ------------------------------------------------------------------------
bool vsInputButton::isPressed(void)
{
    return pressed;
}

// ------------------------------------------------------------------------
// Returns whether or not the button was pressed at some time during the
// previous frame (update() must be called once before this method will
// return the correct value)
// ------------------------------------------------------------------------
bool vsInputButton::wasPressed(void)
{
    return (pressedState == VS_IB_LAST_FRAME);
}

// ------------------------------------------------------------------------
// Returns whether or not the button was released at some time during the
// previous frame (like wasPressed(), update() must be called once before
// this method will return the correct value)
// ------------------------------------------------------------------------
bool vsInputButton::wasReleased(void)
{
    return (releasedState == VS_IB_LAST_FRAME);
}

// ------------------------------------------------------------------------
// Returns whether or not the last press was a double-click
// ------------------------------------------------------------------------
bool vsInputButton::wasDoubleClicked(void)
{
    return doubleClicked;
}

// ------------------------------------------------------------------------
// Sets the button to the pressed state
// ------------------------------------------------------------------------
void vsInputButton::setPressed(void)
{
    // Don't count this as a press if the button is already pressed
    // (polled devices often send constant "setPressed" messages as long as
    // the button is pressed)
    if (!pressed)
    {
        // Set the button to pressed
        pressed = true;

        // Set the button pressed state to "this frame" (a press just occured)
        pressedState = VS_IB_THIS_FRAME;

        // Mark the button press time
        buttonTimer->mark();
        
        // Calculate the time interval between this and the last press,
        // and flag a double-click if the interval is small enough
        if (buttonTimer->getInterval() <= doubleClickInterval)
        {
            doubleClicked = true;
        }
        else
        {
            doubleClicked = false;
        }
    }
}

// ------------------------------------------------------------------------
// Sets the button to the released (not pressed) state
// ------------------------------------------------------------------------
void vsInputButton::setReleased(void)
{
    // Don't count this as a release if the button is already released
    // (polled devices often send constant "setReleased" messages as long as
    // the button is released)
    if (pressed)
    {
        // Set the button to not pressed
        pressed = false;

        // Set the button released state to "this frame" (a release
        // just occured)
        releasedState = VS_IB_THIS_FRAME;
    }
}

// ------------------------------------------------------------------------
// Sets the maximum amount of time between two consecutive presses that
// will be considered a double-click
// ------------------------------------------------------------------------
void vsInputButton::setDoubleClickInterval(double interval)
{
    doubleClickInterval = interval;
}
