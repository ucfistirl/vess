#include "vsInputButton.h++"

// ------------------------------------------------------------------------
// Sets up a basic vsInputButton
// ------------------------------------------------------------------------
vsInputButton::vsInputButton(void)
{
    pressed = VS_FALSE;
    lastPressedTime = 0.0;
    doubleClickInterval = VS_IB_DBLCLICK_INTERVAL;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsInputButton::~vsInputButton(void)
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
    return VS_FALSE;
}

// ------------------------------------------------------------------------
// Sets the button to the pressed state
// ------------------------------------------------------------------------
void vsInputButton::setPressed(void)
{
    pressed = VS_TRUE;
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
