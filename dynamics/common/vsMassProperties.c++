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
//    VESS Module:  vsMassProperties.c++
//
//    Description:  This abstract class represents the mass properties of
//                  an object. Subclasses are required to implement methods
//                  that describe the mass distribution of an object via
//                  its center of mass and moment of inertia matrix.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsMassProperties.h++"

#include "stdlib.h"

// ------------------------------------------------------------------------
// Constructor
// This class is abstract, and has no member variables to initialize.
// ------------------------------------------------------------------------
vsMassProperties::vsMassProperties()
{
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsMassProperties::~vsMassProperties()
{
}

