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
//    VESS Module:  vsMassProperties.h++
//
//    Description:  This abstract class represents the mass properties of
//                  an object. Subclasses are required to implement methods
//                  that describe the mass distribution of an object via
//                  its center of mass and moment of inertia matrix.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_MASS_PROPERTIES_HPP
#define VS_MASS_PROPERTIES_HPP

#include "vsMatrix.h++"
#include "vsVector.h++"

class VS_DYNAMICS_DLL vsMassProperties
{
public:

                        vsMassProperties();
    virtual             ~vsMassProperties();

    virtual vsVector    getCenterOfMass() = 0;
    virtual vsMatrix    getInertiaMatrix() = 0;
};

#endif
