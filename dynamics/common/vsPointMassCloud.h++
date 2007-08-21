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
//    VESS Module:  vsPointMassCloud.h++
//
//    Description:  This subclass of vsMassProperties represents mass
//                  properties of an object as a cloud of point masses.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_POINT_MASS_CLOUD_HPP
#define VS_POINT_MASS_CLOUD_HPP

#include "vsGlobals.h++"

#include "vsMassProperties.h++"

#include "atList.h++"
#include "atMatrix.h++"
#include "atVector.h++"

class VS_DYNAMICS_DLL vsPointMassCloud : public vsMassProperties
{
protected:

    atList      *pointList;

    atVector    centerOfMass;
    atMatrix    inertiaMatrix;
    bool        inertiaValid;

public:

                vsPointMassCloud();
    virtual     ~vsPointMassCloud();

    void        addPointMass(atVector position, double mass);
    void        clear();

    virtual atVector    getCenterOfMass();
    virtual atMatrix    getInertiaMatrix();
};

#endif

