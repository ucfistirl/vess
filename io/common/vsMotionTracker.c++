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
//    VESS Module:  vsMotionTracker.h++
//
//    Description:  Class for storing and returning the state of a motion
//                  tracker
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include "vsMotionTracker.h++"

// ------------------------------------------------------------------------
// Set up a new vsMotionTracker with the given tracker number.  No buttons
// are created.
// ------------------------------------------------------------------------
vsMotionTracker::vsMotionTracker(int trackerNum)
{
    // Set the tracker number
    trackerNumber = trackerNum;

    // Initialize button data to zero and NULL
    numButtons = 0;
    memset(button, 0, sizeof(button));
}

// ------------------------------------------------------------------------
// Set up a new vsMotionTracker with the given tracker number and number
// of buttons
// ------------------------------------------------------------------------
vsMotionTracker::vsMotionTracker(int trackerNum, int nButtons)
{
    int i;

    // Set the tracker number
    trackerNumber = trackerNum;

    // Initialize the buttons
    numButtons = nButtons;
    memset(button, 0, sizeof(button));
    for (i = 0; i < numButtons; i++)
        button[i] = new vsInputButton();
}

// ------------------------------------------------------------------------
// Set up a new vsMotionTracker with a tracker number of 0.  No buttons
// are created.
// ------------------------------------------------------------------------
vsMotionTracker::vsMotionTracker()
{
    // Set the tracker number to zero
    trackerNumber = 0;

    // Initialize button data to zero and NULL
    numButtons = 0;
    memset(button, 0, sizeof(button));
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsMotionTracker::~vsMotionTracker()
{
    int i;

    // Delete any buttons that were created
    for (i = 0; i < VS_MT_MAX_BUTTONS; i++)
    {
        if (button[i] != NULL)
            delete button[i];
    }
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsMotionTracker::getClassName()
{
    return "vsMotionTracker";
}

// ------------------------------------------------------------------------
// Set the position of this motion tracker
// ------------------------------------------------------------------------
void vsMotionTracker::setPosition(atVector posVec)
{
    int i;

    // Set each input axis's position to the corresponding value in the
    // specified position vector
    for (i = 0; i < 3; i++)
    {
        position[i].setPosition(posVec.getValue(i));
    }
}

// ------------------------------------------------------------------------
// Set the orientation of this motion tracker using Euler angles
// ------------------------------------------------------------------------
void vsMotionTracker::setOrientation(atVector ornVec, 
                                     atMathEulerAxisOrder axisOrder)
{
    orientation.setEulerRotation(axisOrder, ornVec.getValue(0),
                                 ornVec.getValue(1), ornVec.getValue(2));
}

// ------------------------------------------------------------------------
// Set the orientation of this motion tracker using a rotation matrix
// ------------------------------------------------------------------------
void vsMotionTracker::setOrientation(atMatrix ornMat)
{
    orientation.setMatrixRotation(ornMat);
}

// ------------------------------------------------------------------------
// Set the orientation of this motion tracker using a quaternion
// ------------------------------------------------------------------------
void vsMotionTracker::setOrientation(atQuat ornQuat)
{
    orientation = ornQuat;
}

// ------------------------------------------------------------------------
// Return the number of buttons on the motion tracker (often zero, but
// some trackers do have buttons)
// ------------------------------------------------------------------------
int vsMotionTracker::getNumButtons()
{
    return numButtons;
}

// ------------------------------------------------------------------------
// Return the requested button, or NULL if the index does not specify a
// valid button
// ------------------------------------------------------------------------
vsInputButton *vsMotionTracker::getButton(int index)
{
    if ((index < 0) || (index >= numButtons))
        return button[index];
    else
        return NULL;
}

// ------------------------------------------------------------------------
// Return the number of this tracker
// ------------------------------------------------------------------------
int vsMotionTracker::getTrackerNumber(void)
{
    return trackerNumber;
}

// ------------------------------------------------------------------------
// Assign a new number to this tracker
// ------------------------------------------------------------------------
void vsMotionTracker::setTrackerNumber(int newNumber)
{
    trackerNumber = newNumber;
}
