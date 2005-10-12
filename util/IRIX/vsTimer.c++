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

vsTimer *vsTimer::systemTimer = NULL;

//------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------
vsTimer::vsTimer()
{
    // Set markTime to 0.0
    markTime = 0.0;

    // Call mark() twice to initialize the mark interval time
    // and set a sane markTime
    mark();
    mark();
}

//------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------
vsTimer::~vsTimer()
{
}

// ------------------------------------------------------------------------
// Returns the global system timer, measuring the time between frames
// ------------------------------------------------------------------------
vsTimer *vsTimer::getSystemTimer()
{
    // Create the system timer if it doesn't yet exist
    if (systemTimer == NULL)
        systemTimer = new vsTimer();

    // Return the timer
    return systemTimer;
}

// ------------------------------------------------------------------------
// Deletes the global system timer 
// ------------------------------------------------------------------------
void vsTimer::deleteSystemTimer()
{
    // Delete the system timer instance if it exists and reset the pointer
    // to NULL
    if (systemTimer != NULL)
    {
        delete systemTimer;
        systemTimer = NULL;
    }
}

//------------------------------------------------------------------------
// Records the current time
//------------------------------------------------------------------------
void vsTimer::mark()
{
    struct timeval timeStruct;
    double newMark;

    // Get the current time from the system
    gettimeofday(&timeStruct, NULL);

    // Record the current time in seconds
    newMark = timeStruct.tv_sec + (timeStruct.tv_usec / 1000000.0);

    // Compute the new mark interval
    markInterval = newMark - markTime;

    // Save the new mark time
    markTime = newMark;
}

//------------------------------------------------------------------------
// Records the time at which the given interval had elapsed since the last
// mark.  If the given interval is less than the actual elapsed interval
// since the last mark, this function behaves exactly like the mark()
// function.
//------------------------------------------------------------------------
void vsTimer::markAtInterval(double intervalTime)
{
    struct timeval timeStruct;
    double newMark;

    // Get the current time from the system
    gettimeofday(&timeStruct, NULL);

    // Record the current time in seconds
    newMark = timeStruct.tv_sec + (timeStruct.tv_usec / 1000000.0);

    // Compute the new mark interval
    markInterval = newMark - markTime;

    // See if the given interval is less than the elapsed interval
    if (markInterval > intervalTime)
    {
        // Use the given interval, and set the new mark time to the time
        // that the given interval would have expired
        markTime = newMark - (markInterval - intervalTime);
        markInterval = intervalTime;
    }
    else
    {
        // Use the actual elapsed interval and set the new mark time as if
        // mark() had been called
        markTime = newMark;
    }
}

// ------------------------------------------------------------------------
// Returns the amount of time (in seconds) between the last two calls to
// mark()
// ------------------------------------------------------------------------
double vsTimer::getInterval()
{
    return markInterval;
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
