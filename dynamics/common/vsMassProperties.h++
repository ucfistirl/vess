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

#include "atMatrix.h++"
#include "atVector.h++"

#include "vsGlobals.h++"

class VS_DYNAMICS_DLL vsMassProperties
{
protected:

    virtual atMatrix   getDefaultInertiaMatrix();

public:

                        vsMassProperties();
    virtual             ~vsMassProperties();

    virtual atVector    getCenterOfMass() = 0;
    virtual atMatrix    getInertiaMatrix() = 0;
};

#endif
