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
//    VESS Module:  vsInputAxis.c++
//
//    Description:  Class for handling the position of an input device's
//                  axis
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsInputAxis.h++"
#include <stdio.h>
#include <string.h>
#include <math.h>

// ------------------------------------------------------------------------
// Default constructor.  Sets up a vsInputAxis in non-normalized mode
// ------------------------------------------------------------------------
vsInputAxis::vsInputAxis(void)
{
    // Initialize variables
    axisMin = 0.0;
    axisMax = 0.0;
    offset = 0.0;
    position = 0.0;
    normalized = VS_FALSE;
    inverted = VS_FALSE;
    passiveCalibration = VS_FALSE;
}

// ------------------------------------------------------------------------
// Sets up a vsInputAxis with the specified range in normalized mode.  If
// the range is invalid, it defaults instead to non-normalized mode.  
//
// The current and idle positions default to the center of the axis range.
// ------------------------------------------------------------------------
vsInputAxis::vsInputAxis(double minPos, double maxPos)
{
    char errStr[100];

    if (minPos < maxPos) 
    {
        // Initialize variables
        position = (maxPos + minPos) / 2;
        offset = position;
        axisMin = minPos;
        axisMax = maxPos;
        threshold = 0.0;
        normalized = VS_TRUE;
        inverted = VS_FALSE;
        passiveCalibration = VS_FALSE;
    }
    else
    {
        sprintf(errStr, "vsInputAxis::vsInputAxis:  Invalid axis range");
        strcat(errStr, "specfied.");
        puts(errStr);
        sprintf(errStr, "vsInputAxis::vsInputAxis:  Using non-normalized");
        strcat(errStr, "mode.");
        puts(errStr);

        // Initialize variables
        axisMin = 0.0;
        axisMax = 0.0;
        position = 0.0;
        offset = 0.0;
        threshold = 0.0;
        normalized = VS_FALSE;
        inverted = VS_FALSE;
    }
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsInputAxis::~vsInputAxis(void)
{

}

// ------------------------------------------------------------------------
// Set the current position of the input device on this axis
// ------------------------------------------------------------------------
void vsInputAxis::setPosition(double rawPos)
{
    // Invert axis value if necessary
    if (inverted)
        rawPos = -rawPos;

    // Do calibration if it's enabled
    if (passiveCalibration)
    {
        if (rawPos < axisMin)
            axisMin = rawPos;

        if (rawPos > axisMax)
            axisMax = rawPos;

#ifdef VS_IA_DEBUG
        printf("vsInputAxis::setPosition: "
            "axisMin = %0.2lf  axisMax = %0.2lf\n", axisMin, axisMax);
#endif
    }

    // Set the new position
    position = rawPos;
}

// ------------------------------------------------------------------------
// If we are set to normalize, return the scaled axis position based on the
// axis range and indle position.  If not, return the raw device position
//
// The normalized value will be between -1.0 and 1.0
// ------------------------------------------------------------------------
double vsInputAxis::getPosition(void)
{
    double temp1, temp2;
    double normalizedPos;

    if (normalized)
    { 
        // Determine if the axis position is on the negative or positive
        // side of the idle position
        temp1 = position - offset;
   
        if (temp1 < 0) 
        {
            // Normalize the axis to a value between the minimum extent 
            // and the idle position of the axis
            temp2 = offset - axisMin;

            // Avoid dividing by zero
            if (fabs(temp2) > 1E-6)
            {
                // Calculate the normalized position
                normalizedPos = temp1 / temp2;

                // Check the value vs. the threshold and return zero
                // or the normalized position accordingly
                if (normalizedPos < -threshold)
                    return normalizedPos;
                else
                    return 0.0;
            }
            else
                return 0.0;
        }
        else
        {
            // Normalize the axis to a value between the maximum extent 
            // and the idle position of the axis
            temp2 = axisMax - offset;

            // Avoid dividing by zero
            if (fabs(temp2) > 1E-6)
            {
                // Calculate the normalized position
                normalizedPos = temp1 / temp2;
 
                // Check the value vs. the threshold and return zero
                // or the normalized position accordingly
                if (normalizedPos > threshold)
                    return normalizedPos;
                else
                    return 0.0;
            }
            else
                return 0.0;
        }
    }
    else
    {
        // Check the raw position against the threshold and return
        // the position or zero accordingly
        if (fabs(position - offset) > threshold)
            return position;
        else
            return 0.0;
    }
}

// ------------------------------------------------------------------------
// Turn normalizing on or off.  Reject request to set it to on if there is 
// not a valid axis range and/or idle position set
// ------------------------------------------------------------------------
void vsInputAxis::setNormalized(int normOn)
{
    char errMsg[100];

    if (normOn)
    {
        // Check for valid extents
        if ((axisMin >= axisMax) || (offset < axisMin) || (offset > axisMax))
        {
            sprintf(errMsg, "vsInputAxis::setNormalized:  ");
            strcat(errMsg, "Invalid range and/or offset values\n");
            puts(errMsg);
            sprintf(errMsg, "vsInputAxis::setNormalized:  ");
            strcat(errMsg, "Using non-normalized mode.\n");
            puts(errMsg);

            normalized = VS_FALSE;
        }
        else
        {
            normalized = VS_TRUE;
        }
    }
    else
    {
        normalized = VS_FALSE;
    }
}

// ------------------------------------------------------------------------
// Returns whether or not the axis values are normalized
// ------------------------------------------------------------------------
int vsInputAxis::isNormalized(void)
{
    return normalized;
}

// ------------------------------------------------------------------------
// Specifies whether or not the axis values should be inverted (negated)
// ------------------------------------------------------------------------
void vsInputAxis::setInverted(int invert)
{
    inverted = invert;
}

// ------------------------------------------------------------------------
// Returns whether or not the axis values are inverted
// ------------------------------------------------------------------------
int vsInputAxis::isInverted()
{
    return inverted;
}

// ------------------------------------------------------------------------
// Set the range of values that the input device returns for this axis
// ------------------------------------------------------------------------
void vsInputAxis::setRange(double minPos, double maxPos)
{
    if (minPos >= maxPos)
    {
        printf("vsInputAxis::setRange: "
            "Invalid range specified, range not set");
    }
    else
    {
        axisMin = minPos;
        axisMax = maxPos;
    }
}

// ------------------------------------------------------------------------
// Get the range of values that the input device returns for this axis
// ------------------------------------------------------------------------
void vsInputAxis::getRange(double *minPos, double *maxPos)
{
    if (minPos >= maxPos)
    {
        *minPos = 0;
        *maxPos = 0;
    }
    else
    {
        *minPos = axisMin;
        *maxPos = axisMax;
    }
}

// ------------------------------------------------------------------------
// Set the idle position of the input device on this axis using the current
// axis value
// ------------------------------------------------------------------------
void vsInputAxis::setIdlePosition()
{
    offset = position;

    // Reset calibration data
    if (passiveCalibration)
    {
        axisMin = offset - 0.01;
        axisMax = offset + 0.01;
    }
}

// ------------------------------------------------------------------------
// Set the idle position of the input device on this axis
// ------------------------------------------------------------------------
void vsInputAxis::setIdlePosition(double newOffset)
{
    offset = newOffset;

    // Reset calibration data
    if (passiveCalibration)
    {
        axisMin = offset - 0.01;
        axisMax = offset + 0.01;
    }
}

// ------------------------------------------------------------------------
// Get the idle position of the input device on this axis
// ------------------------------------------------------------------------
double vsInputAxis::getIdlePosition()
{
    return offset;
}

// ------------------------------------------------------------------------
// Sets the threshold for this axis.  Any subsequent getPosition() calls
// that would normally return a value whose absolute value is less than the 
// threshold will instead be reported as 0.0.  
// ------------------------------------------------------------------------
void vsInputAxis::setThreshold(double newThreshold)
{
    threshold = newThreshold;
}

// ------------------------------------------------------------------------
// Returns the current
// ------------------------------------------------------------------------
double vsInputAxis::getThreshold()
{
    return threshold;
}

// ------------------------------------------------------------------------
// Turn on/off passive calibration.  Passive calibration constantly updates
// the axis's range extents to provide ever more accurate axis data.
// ------------------------------------------------------------------------
void vsInputAxis::passiveCalibrate(int enable)
{
    if (enable)
    {
        axisMin = offset - 0.01;
        axisMax = offset + 0.01;

        passiveCalibration = VS_TRUE;
    }
    else
    {
        passiveCalibration = VS_FALSE;
    }
}
