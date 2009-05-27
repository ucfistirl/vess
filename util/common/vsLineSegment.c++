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
//    VESS Module:  vsLineSegment.c++
//
//    Description:  This class represents a line segment as a start point
//                  and an end point.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsLineSegment.h++"

//------------------------------------------------------------------------
// Constructor - Default
//------------------------------------------------------------------------
vsLineSegment::vsLineSegment()
{
    startPoint.clear();
    startPoint.setSize(3);
    endPoint.clear();
    endPoint.setSize(3);
}

//------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------
vsLineSegment::vsLineSegment(const atVector &start, const atVector &end)
{
    // Copy the values into the two local vectors.
    startPoint.clearCopy(start);
    startPoint.setSize(3);
    endPoint.clearCopy(end);
    endPoint.setSize(3);
}

//------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------
vsLineSegment::~vsLineSegment()
{
}

//------------------------------------------------------------------------
// Virtual function
//------------------------------------------------------------------------
const char *vsLineSegment::getClassName()
{
    return "vsLineSegment";
}

//------------------------------------------------------------------------
// Sets the start point.
//------------------------------------------------------------------------
void vsLineSegment::setStartPoint(const atVector &start)
{
    startPoint.clearCopy(start);
    startPoint.setSize(3);
}

//------------------------------------------------------------------------
// Sets the end point.
//------------------------------------------------------------------------
void vsLineSegment::setEndPoint(const atVector &end)
{
    endPoint.clearCopy(end);
    endPoint.setSize(3);
}

//------------------------------------------------------------------------
// Returns the start point.
//------------------------------------------------------------------------
atVector vsLineSegment::getStartPoint() const
{
    return startPoint;
}

//------------------------------------------------------------------------
// Returns the end point.
//------------------------------------------------------------------------
atVector vsLineSegment::getEndPoint() const
{
    return endPoint;
}

