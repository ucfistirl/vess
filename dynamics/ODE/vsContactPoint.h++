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
//    VESS Module:  vsContactPoint.h++
//
//    Description:  A contact point
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_CONTACT_POINT_HPP
#define VS_CONTACT_POINT_HPP

#include <ode/ode.h>

#define VS_CONTACT_DEFAULT_MU            100.0
#define VS_CONTACT_DEFAULT_SOFT_ERP      0.99
#define VS_CONTACT_DEFAULT_SOFT_CFM      0.01
#define VS_CONTACT_DEFAULT_BOUNCE        0.2
#define VS_CONTACT_DEFAULT_BOUNCE_VEL    0.01

#include "vsGlobals.h++"

class VS_DYNAMICS_DLL vsContactPoint
{
private:

    dContact       odeContact;

VS_INTERNAL:

                vsContactPoint(const dContactGeom &geom);

    void        setContactGeom(const dContactGeom &geom);

    dContact    getODEContact() const;

public:

                   vsContactPoint();
    virtual        ~vsContactPoint();

    void           setBounce(bool bounce, double factor, double threshold);
    void           setConstraintForceMixing(double cfm);
    void           setErrorReductionParameter(double erp);
    void           setFriction(double mu);
};

#endif

