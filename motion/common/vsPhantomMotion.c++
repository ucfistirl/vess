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
//    VESS Module:  vsPhantomMotion.c++
//
//    Description:  Class intended to take motion data from the Phantom
//		    and apply the movements directly to the component
//
//    Author(s):    Jason Daly, Duvan Cope  
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsPhantomMotion.h++"

// ------------------------------------------------------------------------
// Creates a vsPhantomMotion motion model using the given component and
// phantom
// ------------------------------------------------------------------------
vsPhantomMotion::vsPhantomMotion(vsPhantom *thePhantom,
    vsKinematics *kinObject)
{
    // Get the phantom and kinematics objects
    phantom = thePhantom;   
    kinematics = kinObject;

    // Complain if the phantom object is invalid
    if (phantom == NULL)
    {
        printf("vsPhantomMotion::vsPhantomMotion:  WARNING -- NULL motion "
            "phantom specified!\n");
    }

    // Enable position and orientation tracking by default
    positionEnabled = true;
    orientationEnabled = true;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsPhantomMotion::~vsPhantomMotion()
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsPhantomMotion::getClassName()
{
    return "vsPhantomMotion";
}

// ------------------------------------------------------------------------
// Enables positional motion
// ------------------------------------------------------------------------
void vsPhantomMotion::enablePositionTracking()
{
    positionEnabled = true;
}

// ------------------------------------------------------------------------
// Disables positional motion
// ------------------------------------------------------------------------
void vsPhantomMotion::disablePositionTracking()
{
    positionEnabled = false;
}

// ------------------------------------------------------------------------
// Enables rotational motion
// ------------------------------------------------------------------------
void vsPhantomMotion::enableOrientationTracking()
{
    orientationEnabled = true;
}

// ------------------------------------------------------------------------
// Disables rotational motion
// ------------------------------------------------------------------------
void vsPhantomMotion::disableOrientationTracking()
{
    orientationEnabled = false;
}

// ------------------------------------------------------------------------
// Updates the motion model
// ------------------------------------------------------------------------
void vsPhantomMotion::update()
{
    // Apply the data to the kinematics object
    if (positionEnabled)
    {
        kinematics->setVelocity(phantom->getVelocityVec());
    }
    if (orientationEnabled)
        kinematics->setOrientation(phantom->getOrientationQuat());
}
