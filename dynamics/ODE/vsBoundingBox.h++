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
//    VESS Module:  vsBoundingBox.h++
//
//    Description:  This vsBoundingSurface subclass represents a box.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_BOUNDING_BOX_HPP
#define VS_BOUNDING_BOX_HPP

#include <ode/ode.h>

#include "vsBox.h++"
#include "vsBoundingSurface.h++"

#include "atVector.h++"

#include "vsGlobals.h++"

class VS_DYNAMICS_DLL vsBoundingBox : public vsBoundingSurface
{
protected:

    dGeomID     odeGeomXformID;

VS_INTERNAL:

    virtual dGeomID    getODEGeomID();

public:

               vsBoundingBox(const vsBox &box);
    virtual    ~vsBoundingBox();

    virtual const char    *getClassName();

    void       update(const vsBox &box);
};

#endif

