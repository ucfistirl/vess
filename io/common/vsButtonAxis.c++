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
//    VESS Module:  vsButtonAxis.h++
//
//    Description:  Class for emulating the behavior of an input axis
//                  based on the input of some number of vsInputButtons
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsButtonAxis.h++"
#include <stdio.h>
#include <math.h>

//------------------------------------------------------------------------
// Constructor
// Sets up the button axis to use the specified buttons. Any of the
// vsInputButton parameters may be NULL. Assumes a default range for the
// axis limits.
//------------------------------------------------------------------------
vsButtonAxis::vsButtonAxis(vsInputButton *positiveBtn,
    vsInputButton *negativeBtn, vsInputButton *centerBtn)
    : vsIODevice()
{
    // Save the button pointers
    positiveButton = positiveBtn;
    if (positiveButton)
        positiveButton->ref();

    negativeButton = negativeBtn;
    if (negativeButton)
        negativeButton->ref();

    centerButton = centerBtn;
    if (centerButton)
        centerButton->ref();

    // Create the output axis
    outputAxis = new vsInputAxis();
    outputAxis->ref();

    // Set the axis parameters to default values
    outputAxis->setRange(VS_AXIS_DEFAULT_MIN, VS_AXIS_DEFAULT_MAX);
    outputAxis->setIdlePosition((VS_AXIS_DEFAULT_MIN + VS_AXIS_DEFAULT_MAX) / 2.0);

    // Set the current position to the idle position
    setPosition(outputAxis->getIdlePosition());

    // Set the button parameters to defaults
    positiveSpeed = -1.0;
    negativeSpeed = -1.0;
    centerSpeed = -1.0;
    idleSpeed = -1.0;
}

//------------------------------------------------------------------------
// Constructor
// Sets up the button axis to use the specified buttons. Any of the
// vsInputButton parameters may be NULL. Uses the given values for the
// axis limits.
//------------------------------------------------------------------------
vsButtonAxis::vsButtonAxis(vsInputButton *positiveBtn,
    vsInputButton *negativeBtn, vsInputButton *centerBtn,
    double axisMin, double axisMax) : vsIODevice()
{
    // Save the button pointers
    positiveButton = positiveBtn;
    if (positiveButton)
        positiveButton->ref();

    negativeButton = negativeBtn;
    if (negativeButton)
        negativeButton->ref();

    centerButton = centerBtn;
    if (centerButton)
        centerButton->ref();

    // Create the output axis
    outputAxis = new vsInputAxis();
    outputAxis->ref();

    // Set the axis parameters to specified values
    if (axisMin < axisMax)
    {
        outputAxis->setRange(axisMin, axisMax);
        outputAxis->setIdlePosition((axisMin + axisMax) / 2.0);
    }
    else
    {
        printf("vsButtonAxis::vsButtonAxis: axisMax must be greater than"
            " axisMin; using default values instead\n");
        outputAxis->setRange(VS_AXIS_DEFAULT_MIN, VS_AXIS_DEFAULT_MAX);
        outputAxis->setIdlePosition((VS_AXIS_DEFAULT_MIN + VS_AXIS_DEFAULT_MAX) / 2.0);
    }

    // Set the current position to the idle position
    setPosition(outputAxis->getIdlePosition());

    // Set the button axis parameters to defaults
    positiveSpeed = -1.0;
    negativeSpeed = -1.0;
    centerSpeed = -1.0;
    idleSpeed = -1.0;
}

//------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------
vsButtonAxis::~vsButtonAxis()
{
    // Release the buttons
    if (positiveButton)
        vsObject::unrefDelete(positiveButton);
    if (negativeButton)
        vsObject::unrefDelete(negativeButton);
    if (centerButton)
        vsObject::unrefDelete(centerButton);

    // Dispose of the output axis
    vsObject::unrefDelete(outputAxis);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsButtonAxis::getClassName()
{
    return "vsButtonAxis";
}

//------------------------------------------------------------------------
// Updates this object by using the data in the associated vsInputButtons
// to fabricate a current position on this input axis
//------------------------------------------------------------------------
void vsButtonAxis::update()
{
    bool posBtnPressed, negBtnPressed, cenBtnPressed;
    double moveDirection, moveDistance, moveSpeed, moveCap;
    double axisMin, axisMax, offset;
    double deltaTime;

    // Get some of the parameters from the output axis object
    outputAxis->getRange(&axisMin, &axisMax);
    offset = outputAxis->getIdlePosition();

    // Get the amount of time that passed last frame
    deltaTime = (vsTimer::getSystemTimer())->getInterval();

    // Determine which of the buttons are currently pressed
    // * A button that was not specified is never considered to be pressed
    posBtnPressed = ((positiveButton != NULL) && positiveButton->isPressed());
    negBtnPressed = ((negativeButton != NULL) && negativeButton->isPressed());
    cenBtnPressed = ((centerButton != NULL) && centerButton->isPressed());

    // The 'positive' and 'negative' buttons cancel each other out; if both
    // are pressed, then treat it as if neither is pressed.
    if (posBtnPressed && negBtnPressed)
    {
        posBtnPressed = false;
        negBtnPressed = false;
    }

    // Determine the direction and speed of movement, as well as the maximum
    // distance to travel, based on which button(s) are currently pressed

    if (cenBtnPressed)
    {
        // Pressing the center button overrides both other buttons

        // Move towards the idle position
        if (position > offset)
            moveDirection = -1.0;
        else
            moveDirection = 1.0;

        // Use the center button speed
        moveSpeed = centerSpeed;

        // Determine distance to the idle position
        moveDistance = fabs(position - offset);
    }
    else if (posBtnPressed)
    {
        // Move in the positive direction
        moveDirection = 1.0;

        // Use the positive button speed
        moveSpeed = positiveSpeed;

        // Determine distance to the maximum axis value
        moveDistance = axisMax - position;
    }
    else if (negBtnPressed)
    {
        // Move in the negative direction
        moveDirection = -1.0;

        // Use the negative button speed
        moveSpeed = negativeSpeed;

        // Determine distance to the minimum axis value
        moveDistance = position - axisMin;
    }
    else
    {
        // Move towards the idle position
        if (position > offset)
            moveDirection = -1.0;
        else
            moveDirection = 1.0;

        // Use the idle speed
        moveSpeed = idleSpeed;

        // Determine distance to the idle position
        moveDistance = fabs(position - offset);
    }

    // If the movement speed is non-negative, then use it as a cap on the
    // amount of movement we can make this update
    if (moveSpeed >= 0.0)
    {
        moveCap = moveSpeed * deltaTime;
        if (moveDistance > moveCap)
            moveDistance = moveCap;
    }

    // Move the current axis position by the specified distance and in the
    // specified direction
    setPosition(position + (moveDistance * moveDirection));
}

//------------------------------------------------------------------------
// Return the number of input axes
//------------------------------------------------------------------------
int vsButtonAxis::getNumAxes()
{
    return 1;
}

//------------------------------------------------------------------------
// Return the number of input buttons (zero, since this device does not
// pass on any of the data from its buttons).
//------------------------------------------------------------------------
int vsButtonAxis::getNumButtons()
{
    return 0;
}

//------------------------------------------------------------------------
// Return the requested input axis
//------------------------------------------------------------------------
vsInputAxis *vsButtonAxis::getAxis(int index)
{
    // If the first axis is requested, return that. Otherwise, return NULL,
    // as there is no other axis to return.
    if (index == 0)
        return outputAxis;
    else
        return NULL;
}

//------------------------------------------------------------------------
// Return the requested input button (NULL, since this device does not
// pass on any of the data from its buttons).
//------------------------------------------------------------------------
vsInputButton *vsButtonAxis::getButton(int index)
{
    return NULL;
}

//------------------------------------------------------------------------
// Sets the speed (in units/sec) at which the 'positive' button moves the
// axis position towards the maximum axis value. A speed of zero means
// that the button does not move the positon at all (although it still
// prevents the 'idle' movement from occurring), and a negative speed
// means that the position is set directly to the maximum value, with no
// transition time.
//------------------------------------------------------------------------
void vsButtonAxis::setPositiveButtonSpeed(double speed)
{
    positiveSpeed = speed;
}

//------------------------------------------------------------------------
// Gets the speed (in units/sec) at which the 'positive' button moves the
// axis position towards the maximum axis value. A speed of zero means
// that the button does not move the positon at all (although it still
// prevents the 'idle' movement from occurring), and a negative speed
// means that the position is set directly to the maximum value, with no
// transition time.
//------------------------------------------------------------------------
double vsButtonAxis::getPositiveButtonSpeed()
{
    return positiveSpeed;
}

//------------------------------------------------------------------------
// Sets the speed (in units/sec) at which the 'negative' button moves the
// axis position towards the minimum axis value. A speed of zero means
// that the button does not move the positon at all (although it still
// prevents the 'idle' movement from occurring), and a negative speed
// means that the position is set directly to the minimum value, with no
// transition time.
//------------------------------------------------------------------------
void vsButtonAxis::setNegativeButtonSpeed(double speed)
{
    negativeSpeed = speed;
}

//------------------------------------------------------------------------
// Gets the speed (in units/sec) at which the 'negative' button moves the
// axis position towards the minimum axis value. A speed of zero means
// that the button does not move the positon at all (although it still
// prevents the 'idle' movement from occurring), and a negative speed
// means that the position is set directly to the minimum value, with no
// transition time.
//------------------------------------------------------------------------
double vsButtonAxis::getNegativeButtonSpeed()
{
    return negativeSpeed;
}

//------------------------------------------------------------------------
// Sets the speed (in units/sec) at which the 'center' button moves the
// axis position towards the idle position axis value. A speed of zero
// means that the button does not move the positon at all (although it
// still prevents the 'idle' movement from occurring), and a negative
// speed means that the position is set directly to the idle position
// value, with no transition time.
//------------------------------------------------------------------------
void vsButtonAxis::setCenterButtonSpeed(double speed)
{
    centerSpeed = speed;
}

//------------------------------------------------------------------------
// Gets the speed (in units/sec) at which the 'center' button moves the
// axis position towards the idle position axis value. A speed of zero
// means that the button does not move the positon at all (although it
// still prevents the 'idle' movement from occurring), and a negative
// speed means that the position is set directly to the idle position
// value, with no transition time.
//------------------------------------------------------------------------
double vsButtonAxis::getCenterButtonSpeed()
{
    return centerSpeed;
}

//------------------------------------------------------------------------
// Sets the speed (in units/sec) at which the axis position moves towards
// the idle position axis value when no buttons are pressed. A speed of
// zero means that idling does not move the positon at all, and a negative
// speed means that the position is set directly to the idle position
// value, with no transition time.
//------------------------------------------------------------------------
void vsButtonAxis::setIdleSpeed(double speed)
{
    idleSpeed = speed;
}

//------------------------------------------------------------------------
// Gets the speed (in units/sec) at which the axis position moves towards
// the idle position axis value when no buttons are pressed. A speed of
// zero means that idling does not move the positon at all, and a negative
// speed means that the position is set directly to the idle position
// value, with no transition time.
//------------------------------------------------------------------------
double vsButtonAxis::getIdleSpeed()
{
    return idleSpeed;
}

//------------------------------------------------------------------------
// Private function
// Sets the value of the output axis. Also stores the position value.
// This axis position needs to be stored locally because getting the
// position back out of the axis object is not always trivial; if the
// axis is set to auto-normalize, the value we get out might not be the
// same as the value we put in.
//------------------------------------------------------------------------
void vsButtonAxis::setPosition(double newPos)
{
    // Store the value locally
    position = newPos;

    // Store the value in the output axis
    outputAxis->setPosition(newPos);
}
