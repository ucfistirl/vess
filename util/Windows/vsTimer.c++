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
        if (timerResolution < tc.wPeriodMin)
            timerResolution = tc.wPeriodMin;
        if (timerResolution > tc.wPeriodMax)
            timerResolution = tc.wPeriodMax;
    
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
    // greater than or equal to formerTime.  We test equal because it's
    // possible for mark() to be called followed by getElapsed() in the
    // same frame.  The interval between these two calls may be less than
    // a millisecond, which would cause latterTime and formerTime to be
    // equal.
    if (latterTime >= formerTime)
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

//------------------------------------------------------------------------
// Records the time at which the given interval had elapsed since the last
// mark.  If the given interval is less than the actual elapsed interval
// since the last mark, this function behaves exactly like the mark()
// function.
//------------------------------------------------------------------------
void vsTimer::markAtInterval(double intervalTime)
{
    DWORD newMark, intervalTimeMS;

    // Get and record the current time in milliseconds
    newMark = timeGetTime();

    // Compute the new mark interval (in seconds)
    markInterval = ((double)(getTimeDiff(newMark, markTime))) / 1000.0;

    // See if the given interval is less than the elapsed interval
    if (markInterval > intervalTime)
    {
        // Use the given interval, and set the new mark time to the time
        // that the given interval would have expired.  Make sure to account
        // for timer wraparound here
        intervalTimeMS = (DWORD)((markInterval - intervalTime) * 1000.0);
        if (markTime < intervalTimeMS)
            markTime = UINT_MAX - (intervalTimeMS - newMark);
        else
            markTime = newMark - intervalTimeMS;

        // Save the mark interval
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
