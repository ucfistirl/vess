#include "vsInputButton.h++"
#include <sys/time.h>

// ------------------------------------------------------------------------
// Sets up a basic vsInputButton
// ------------------------------------------------------------------------
vsInputButton::vsInputButton(void)
{
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

    gettimeofday(&tv, NULL);

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

    pressed = VS_TRUE;

    currentTime = getTime();
    if ((currentTime - lastPressedTime) <= doubleClickInterval)
    {
        doubleClicked = VS_TRUE;
    }
    else
    {
        doubleClicked = VS_FALSE;
    }

    lastPressedTime = currentTime;
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
