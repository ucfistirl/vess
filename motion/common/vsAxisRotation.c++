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
//    VESS Module:  vsAxisRotation.h++
//
//    Description:  Motion model to translate input from two vsInputAxis
//                  objects into heading and pitch rotations.  Maximum
//                  rotation extents and rotation speeds are configurable.
//                  A "reset button" is also available to re-center the
//                  orientation at any time.
//
//    Author(s):    Carlos Rosas-Anderson
//
//------------------------------------------------------------------------

#include "vsAxisRotation.h++"

// ------------------------------------------------------------------------
// Constructor.  Creates a vsAxisRotation object using the given axes for
// heading and pitch, respectively.
// ------------------------------------------------------------------------
vsAxisRotation::vsAxisRotation(vsInputAxis *hAxis, vsInputAxis *pAxis,
    vsKinematics *kin) 
    : vsMotionModel()
{
    // Save the axes
    headingAxis = hAxis;
    pitchAxis = pAxis;

    // Set the reset button to NULL (disabled)
    resetButton = NULL;

    // Save the kinematics
    kin = kin;

    // Set default values for width and rotation speed
    headingHalfWidth = VS_AR_DEFAULT_HEADING_WIDTH / 2.0;
    pitchHalfWidth = VS_AR_DEFAULT_PITCH_WIDTH / 2.0;
    headingSpeed = VS_AR_DEFAULT_HEADING_SPEED;
    pitchSpeed = VS_AR_DEFAULT_PITCH_SPEED;

    // Grab the current orientation for the starting orientation
    startingOrientation = kin->getOrientation();
}

// ------------------------------------------------------------------------
// Constructor.  Creates a vsAxisRotation object using the given axes for
// heading and pitch, respectively.  Also sets up a "reset" button for
// returning to the original orientation.
// ------------------------------------------------------------------------
vsAxisRotation::vsAxisRotation(vsInputAxis *hAxis, vsInputAxis *pAxis,
    vsInputButton *rButton, vsKinematics *kin) 
    : vsMotionModel()
{
    // Save the axes
    headingAxis = hAxis;
    pitchAxis = pAxis;

    // Save the reset button
    resetButton = rButton;

    // Save the kinematics
    kin = kin;

    // Set default values for width and rotation speed
    headingHalfWidth = VS_AR_DEFAULT_HEADING_WIDTH / 2.0;
    pitchHalfWidth = VS_AR_DEFAULT_PITCH_WIDTH / 2.0;
    headingSpeed = VS_AR_DEFAULT_HEADING_SPEED;
    pitchSpeed = VS_AR_DEFAULT_PITCH_SPEED;

    // Grab the current orientation for the starting orientation
    startingOrientation = kin->getOrientation();
}

// ------------------------------------------------------------------------
// Destructor.  Does nothing.
// ------------------------------------------------------------------------
vsAxisRotation::~vsAxisRotation()
{
}

// ------------------------------------------------------------------------
// Return the class name
// ------------------------------------------------------------------------
const char *vsAxisRotation::getClassName()
{
    return "vsAxisRotation";
}

// ------------------------------------------------------------------------
// Update method.  Reads the axis values and performs the necessary 
// rotations on the kinematics.
// ------------------------------------------------------------------------
void vsAxisRotation::update()
{
    double headingPos;
    double pitchPos;
    double h, p, r;
    vsQuat tempQuat;

    // Initialize axis position variables
    headingPos = 0.0;
    pitchPos = 0.0;

    // If the kinematics object is invalid, bail
    if (kin == NULL)
        return;

    // Check the reset button
    if (resetButton != NULL)
    {
        // Reset the orientation to the default if the button is pressed
        if (resetButton->isPressed())
            reset();
    }

    // Get the position of the heading axis
    if (headingAxis != NULL)
    {
        // Make sure the heading axis is normalized
        if (headingAxis->isNormalized())
        {
            // Get the normalized axis position
            headingPos = headingAxis->getPosition();
        }
        else
        {
            // If not normalized, set to normalized temporarily and
            // get the position
            headingAxis->setNormalized(VS_TRUE);
            headingPos = headingAxis->getPosition();
            headingAxis->setNormalized(VS_FALSE);
        }
    }

    // Get the position of the pitch axis
    if (pitchAxis != NULL)
    {
        // Make sure the pitch axis is normalized
        if(pitchAxis->isNormalized())
        {
            // Get the normalized axis position
            pitchPos = pitchAxis->getPosition();
        }
        else
        {
            // If not normalized, set to normalized temporarily and
            // get the position
            pitchAxis->setNormalized(VS_TRUE);
            pitchPos = pitchAxis->getPosition();
            pitchAxis->setNormalized(VS_FALSE);
        }
    }

    // Get the current orientation of the kinematics in Euler angles
    tempQuat = kin->getOrientation();
    tempQuat.getEulerRotation(VS_EULER_ANGLES_ZXY_R, &h, &p, &r);

    // Ensure that the heading rotation doesn't exceed the limits
    if (((headingPos < 0) && (h > -headingHalfWidth)) || 
        ((headingPos > 0) && (h < headingHalfWidth)))
    {
        // Don't do anything if the axis is centered
        if(fabs(headingPos) < VS_AR_DOUBLE_TOLERANCE)
        {
            // Update the angular velocity of the kinematics based
            // on the position of the heading axis and the heading 
            // rotation speed
            kin->modifyAngularVelocity(vsVector(0.0, 0.0, 1.0),
                headingPos * headingSpeed);
        }
    }

    // Ensure that the pitch rotation doesn't exceed the limits
    if (((pitchPos < 0) && (p > -pitchHalfWidth)) || 
        ((pitchPos > 0) && (p < pitchHalfWidth)))
    {
        // Don't do anything if the axis is centered
        if (fabs(pitchPos) < VS_AR_DOUBLE_TOLERANCE)
        {
            // Update the angular velocity of the kinematics based
            // on the position of the pitch axis and the pitch 
            // rotation speed
            kin->modifyAngularVelocity(vsVector(1.0, 0.0, 0.0),
                pitchPos * pitchSpeed);
        }
    }
}

// ------------------------------------------------------------------------
// Returns the kinematics' orientation to the default orientation (as set
// by setStartingOrientation())
// ------------------------------------------------------------------------
void vsAxisRotation::center()
{
    kin->setOrientation(startingOrientation);
}

// ------------------------------------------------------------------------
// Sets the maximum rotation for the heading axis
// ------------------------------------------------------------------------
void vsAxisRotation::setHeadingWidth(double hWidth)
{
    // Store the width as a half-width to make update calculations
    // easier
    headingHalfWidth = hWidth / 2.0;
}

// ------------------------------------------------------------------------
// Returns the maximum rotation for the heading axis
// ------------------------------------------------------------------------
double vsAxisRotation::getHeadingWidth()
{
    // Double the stored half-width (to get the total width) and return it
    return headingHalfWidth * 2.0;
}

// ------------------------------------------------------------------------
// Sets the maximum rotation for the pitch axis
// ------------------------------------------------------------------------
void vsAxisRotation::setPitchWidth(double pWidth)
{
    // Store the width as a half-width to make update calculations
    // easier
    pitchHalfWidth = pWidth / 2.0;
}

// ------------------------------------------------------------------------
// Returns the maximum rotation for the pitch axis
// ------------------------------------------------------------------------
double vsAxisRotation::getPitchWidth()
{
    // Double the stored half-width (to get the total width) and return it
    return pitchHalfWidth * 2.0;
}

// ------------------------------------------------------------------------
// Sets the maximum rotation rate (in degrees/sec) for the heading axis
// ------------------------------------------------------------------------
void vsAxisRotation::setHeadingSpeed(double hSpeed)
{
    headingSpeed = hSpeed;
}

// ------------------------------------------------------------------------
// Returns the maximum rotation rate (in degrees/sec) for the heading axis
// ------------------------------------------------------------------------
double vsAxisRotation::getHeadingSpeed()
{
    return headingSpeed;
}

// ------------------------------------------------------------------------
// Sets the maximum rotation rate (in degrees/sec) for the pitch axis
// ------------------------------------------------------------------------
void vsAxisRotation::setPitchSpeed(double pSpeed)
{
    pitchSpeed = pSpeed;
}

// ------------------------------------------------------------------------
// Returns the maximum rotation rate (in degrees/sec) for the pitch axis
// ------------------------------------------------------------------------
double vsAxisRotation::getPitchSpeed()
{
    return pitchSpeed;
}

// ------------------------------------------------------------------------
// Sets the default orientation for the kinematics.  Calling center() will
// return the kinematics to the orientation specified.
// ------------------------------------------------------------------------
void vsAxisRotation::setStartingOrientation(vsQuat orientation)
{
    startingOrientation = orientation;
}

// ------------------------------------------------------------------------
// Returns the default orientation for the kinematics
// ------------------------------------------------------------------------
vsQuat vsAxisRotation::getStartingOrientation()
{
    return startingOrientation;
}
