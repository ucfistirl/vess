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
#include <mmsystem.h>
#include <limits.h>

vsTimer *vsTimer::systemTimer = NULL;

//------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------
vsTimer::vsTimer()
{
    TIMECAPS tc;
    
    // Determine the maximum timer resolution we can get (in milliseconds)
    // start with a value of 1 (the optimal resolution)
    timerResolution = 1;
    
    // Get the capabilities of the system timer, if we can't do this, we'll
    // just use the default resolution
    if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) == TIMERR_NOERROR) 
    {
        // Select the minimum resolution the timer can handle
        timerResolution = min(max(tc.wPeriodMin, timerResolution), 
            tc.wPeriodMax);
    
        // Set the timer resolution
        timeBeginPeriod(timerResolution); 
    }
    
    // Set markTime to 0.0
    markTime = 0;

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
    // We're required to call timeEndPeriod on the timer, since we've called
    // timeBeginPeriod to set the resolution
    timeEndPeriod(timerResolution);
}

// ------------------------------------------------------------------------
// Internal function:
// Returns the difference in system time between the two timer values,
// accounting for possible integer wraparound
// ------------------------------------------------------------------------
DWORD vsTimer::getTimeDiff(DWORD latterTime, DWORD formerTime)
{
    // Check the sizes of the parameters, to make sure latterTime is
    // greater than formerTime
    if (latterTime > formerTime)
    {
        // Usual case, return the time difference
        return latterTime - formerTime;
    }
    else
    {
        // The timer has wrapped since formerTime, return the difference
        // accounting for this
        return (latterTime + (UINT_MAX - formerTime));
    }
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
    DWORD newMark;

    // Get and record the current time in milliseconds
    newMark = timeGetTime();

    // Compute the new mark interval (in seconds)
    markInterval = ((double)(getTimeDiff(newMark, markTime))) / 1000.0;

    // Save the new mark time
    markTime = newMark;
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
    UINT systemTime;
    double elapsedTime;

    // Get the current time in seconds from the system
    systemTime = timeGetTime();
    
    // Compute the difference between the current time and the last
    // mark time (in seconds)
    elapsedTime = ((double)(getTimeDiff(systemTime, markTime))) / 1000.0;
    
    // Return the elapsed time
    return elapsedTime;
}
