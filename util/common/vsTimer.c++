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
//    VESS Module:  vsTimer.h++
//
//    Description:  Object for measuring elapsed (real) time
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsTimer.h++"

#include <stdio.h>
#include <sys/time.h>

//------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------
vsTimer::vsTimer()
{
    // Record the reference time when constructed
    mark();
}

//------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------
vsTimer::~vsTimer()
{
}

//------------------------------------------------------------------------
// Records the current time
//------------------------------------------------------------------------
void vsTimer::mark()
{
    struct timeval timeStruct;

    // Get the current time from the system
    gettimeofday(&timeStruct, NULL);

    // Record the current time in seconds
    markTime = timeStruct.tv_sec + (timeStruct.tv_usec / 1000000.0);
}

//------------------------------------------------------------------------
// Returns the amount of time (in seconds) since the last time the mark
// function was called
//------------------------------------------------------------------------
double vsTimer::getElapsed()
{
    struct timeval timeStruct;
    double currentTime;

    // Get the current time in seconds from the system
    gettimeofday(&timeStruct, NULL);
    currentTime = timeStruct.tv_sec + (timeStruct.tv_usec / 1000000.0);
    
    // Return the difference between the current time and the last mark()
    return (currentTime - markTime);
}
