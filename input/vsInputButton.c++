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
#include <sys/time.h>
#include <stdio.h>

// ------------------------------------------------------------------------
// Sets up a basic vsInputButton
// ------------------------------------------------------------------------
vsInputButton::vsInputButton(void)
{
    // Initialize variables
    pressed = VS_FALSE;
    lastPressedTime = 0.0;
    doubleClicked = VS_FALSE;
    doubleClickInterval = VS_IB_DBLCLICK_INTERVAL;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsInputButton::~vsInputButton(void)
{
}

// ------------------------------------------------------------------------
// Returns the current system time in seconds
// ------------------------------------------------------------------------
double vsInputButton::getTime()
{
    struct timeval tv;
    double currentTime;

    // Query the system time
    gettimeofday(&tv, NULL);

    // Convert the timeval struct to floating-point seconds
    currentTime = tv.tv_sec + (tv.tv_usec / 1000000.0);

    return currentTime;
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
    double currentTime;

    // Don't count this as a press if the button is already pressed
    if (!pressed)
    {
        // Set the button to pressed
        pressed = VS_TRUE;

        // Calculate the time interval between this and the last press,
        // and flag a double-click if the interval is small enough
        currentTime = getTime();
        if ((currentTime - lastPressedTime) <= doubleClickInterval)
        {
            doubleClicked = VS_TRUE;
        }
        else
        {
            doubleClicked = VS_FALSE;
        }
    
        // Remember the time of this button press
        lastPressedTime = currentTime;
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
