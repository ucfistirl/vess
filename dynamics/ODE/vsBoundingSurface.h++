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
//    VESS Module:  vsBoundingSurface.h++
//
//    Description:  This abstract class represents a bounding surface. A
//                  vsBoundingVolume (used for collision testing) is made
//                  up of one or more instances of a vsBoundingSurface.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_BOUNDING_SURFACE_HPP
#define VS_BOUNDING_SURFACE_HPP

#include <ode/ode.h>

#include "vsObject.h++"

#include "atVector.h++"

#include "vsGlobals.h++"

class VESS_SYM vsBoundingSurface : public vsObject
{
protected:

    dGeomID     odeGeomID;

VS_INTERNAL:

    virtual dGeomID    getODEGeomID();

    virtual void       modifyOffset(const atVector &offset);

public:

                vsBoundingSurface();
    virtual     ~vsBoundingSurface();

    virtual const char    *getClassName() = 0;
};

#endif

