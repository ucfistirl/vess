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
//    VESS Module:  vsBoundingSphere.h++
//
//    Description:  This vsBoundingSurface subclass represents a sphere.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_BOUNDING_SPHERE_HPP
#define VS_BOUNDING_SPHERE_HPP

#include <ode/ode.h>

#include "vsBoundingSurface.h++"
#include "vsSphere.h++"

#include "vsGlobals.h++"

class VS_DYNAMICS_DLL vsBoundingSphere : public vsBoundingSurface
{
protected:

    dGeomID     odeGeomXformID;

VS_INTERNAL:

    virtual dGeomID    getODEGeomID();

public:

                vsBoundingSphere(const vsSphere &sphere);
    virtual     ~vsBoundingSphere();

    virtual const char    *getClassName();

    void        update(const vsSphere &sphere);
};

#endif

