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

// ------------------------------------------------------------------------
// Virtual method
// This method returns a default inertia matrix for use by any subclasses
// of vsMassProperties. Note that this should have been a defined value,
// but the C++ language doesn't seem capable of inline generation of an
// object which takes an array in its constructor as atMatrix does.
// ------------------------------------------------------------------------
atMatrix vsMassProperties::getDefaultInertiaMatrix()
{
    atMatrix defaultInertiaMatrix;

    // The default inertia matrix represents a sphere with a radius of 1 meter
    // and a mass of 1 kilogram. The matrix of a solid sphere is calculated
    // based on the equation:
    //
    //      /  D   0.0  0.0 \
    //     |                 |
    // I = |  0.0   D   0.0  |
    //     |                 |
    //      \ 0.0  0.0   D  /
    //
    //     where D = (2/5) * M * R^2 = 0.4
    //
    // The atMatrix should already be initialized to all zeroes, so simply set
    // the significant diagonal values and return it.
    defaultInertiaMatrix[0][0] = 0.4;
    defaultInertiaMatrix[1][1] = 0.4;
    defaultInertiaMatrix[2][2] = 0.4;
    return defaultInertiaMatrix;
}

