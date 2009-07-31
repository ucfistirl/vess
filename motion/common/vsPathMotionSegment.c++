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
//    VESS Module:  vsPathMotionSegment.c++
//
//    Description:  Stores the data for a point on a vsPathMotion path
//
//    Author(s):    Bryan Kline, Jason Daly
//
//------------------------------------------------------------------------

#include "vsPathMotionSegment.h++"

// ------------------------------------------------------------------------
// Constructor.  Initializes all data members to zero
// ------------------------------------------------------------------------
vsPathMotionSegment::vsPathMotionSegment()
{
    position = atVector(0.0, 0.0, 0.0);
    orientation = atQuat(0.0, 0.0, 0.0, 1.0);
    travelTime = 0.0;
    pauseTime = 0.0;
}

// ------------------------------------------------------------------------
// Destructor.  Does nothing
// ------------------------------------------------------------------------
vsPathMotionSegment::~vsPathMotionSegment()
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsPathMotionSegment::getClassName()
{
    return "vsPathMotionSegmentSegment";
}

// ------------------------------------------------------------------------
// Returns a duplicate of this segment
// ------------------------------------------------------------------------
vsPathMotionSegment *vsPathMotionSegment::clone()
{
    vsPathMotionSegment *newSeg;

    // Create a new segment and copy over our data
    newSeg = new vsPathMotionSegment();
    newSeg->setPosition(position);
    newSeg->setOrientation(orientation);
    newSeg->setTravelTime(travelTime);
    newSeg->setPauseTime(pauseTime);

    // Return the new segment
    return newSeg;
}

// ------------------------------------------------------------------------
// Sets the position of the point
// ------------------------------------------------------------------------
void vsPathMotionSegment::setPosition(atVector pos)
{
    // Set the position, forcing the vector size to stay 3
    position.clearCopy(pos);
}

// ------------------------------------------------------------------------
// Gets the position of one of the key points of the path
// ------------------------------------------------------------------------
atVector vsPathMotionSegment::getPosition()
{
    // Get the position
    return position;
}

// ------------------------------------------------------------------------
// Sets the orientation of the point
// ------------------------------------------------------------------------
void vsPathMotionSegment::setOrientation(atQuat orient)
{
    // Set the orientation
    orientation = orient;
}

// ------------------------------------------------------------------------
// Gets the orientation of the of the key points of the path
// ------------------------------------------------------------------------
atQuat vsPathMotionSegment::getOrientation()
{
    // Get the orientation
    return orientation;
}

// ------------------------------------------------------------------------
// Sets the traversal time for the segment of the path located between
// this point and the next point on the path
// ------------------------------------------------------------------------
void vsPathMotionSegment::setTravelTime(double seconds)
{
    // Set the travel time
    travelTime = seconds;
}

// ------------------------------------------------------------------------
// Gets the traversal time for the segment of the path located between
// this point and the next point on the path
// ------------------------------------------------------------------------
double vsPathMotionSegment::getTravelTime()
{
    // Get the travel time
    return travelTime;
}

// ------------------------------------------------------------------------
// Sets the amount of time to wait at this point before continuing on to
// traverse the next segment of the path. Specifying the constant
// VS_PATH_WAIT_FOREVER for the time causes the path to go into PAUSED
// mode, to wait at this point until a resume call is made.
// ------------------------------------------------------------------------
void vsPathMotionSegment::setPauseTime(double seconds)
{
    // Set the pause time
    pauseTime = seconds;
}

// ------------------------------------------------------------------------
// Gets the amount of time to wait at this point before continuing on to
// traverse the next segment of the path. The constant
// VS_PATH_WAIT_FOREVER indicates the path should go into PAUSED mode, to
// wait at this point until a resume call is made.
// ------------------------------------------------------------------------
double vsPathMotionSegment::getPauseTime()
{
    // Set the pause time
    return pauseTime;
}

