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
#include "vsMotionTracker.h++"

// ------------------------------------------------------------------------
// Set up a new vsMotionTracker with the given tracker number
// ------------------------------------------------------------------------
vsMotionTracker::vsMotionTracker(int trackerNum)
{
    trackerNumber = trackerNum;
}

// ------------------------------------------------------------------------
// Set up a new vsMotionTracker with a tracker number of 0
// ------------------------------------------------------------------------
vsMotionTracker::vsMotionTracker()
{
    trackerNumber = 0;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsMotionTracker::~vsMotionTracker()
{
}

// ------------------------------------------------------------------------
// Set the position of this motion tracker
// ------------------------------------------------------------------------
void vsMotionTracker::setPosition(vsVector posVec)
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
void vsMotionTracker::setOrientation(vsVector ornVec, 
                                     vsMathEulerAxisOrder axisOrder)
{
    orientation.setEulerRotation(axisOrder, ornVec.getValue(0),
                                 ornVec.getValue(1), ornVec.getValue(2));
}

// ------------------------------------------------------------------------
// Set the orientation of this motion tracker using a rotation matrix
// ------------------------------------------------------------------------
void vsMotionTracker::setOrientation(vsMatrix ornMat)
{
    orientation.setMatrixRotation(ornMat);
}

// ------------------------------------------------------------------------
// Set the orientation of this motion tracker using a quaternion
// ------------------------------------------------------------------------
void vsMotionTracker::setOrientation(vsQuat ornQuat)
{
    orientation = ornQuat;
}

// ------------------------------------------------------------------------
// Return the number of buttons on a vsMotionTracker (zero, trackers don't
// have buttons)
// ------------------------------------------------------------------------
int vsMotionTracker::getNumButtons()
{
    return 0;
}

// ------------------------------------------------------------------------
// Return NULL for any button requests (trackers don't have buttons)
// ------------------------------------------------------------------------
vsInputButton *vsMotionTracker::getButton(int index)
{
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
