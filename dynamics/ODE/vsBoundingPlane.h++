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
//    VESS Module:  vsBoundingPlane.h++
//
//    Description:  This vsBoundingSurface subclass represents a plane.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_BOUNDING_PLANE_HPP
#define VS_BOUNDING_PLANE_HPP

#include <ode/ode.h>

#include "vsBoundingSurface.h++"

#include "atVector.h++"

#include "vsGlobals.h++"

class VESS_SYM vsBoundingPlane : public vsBoundingSurface
{
VS_INTERNAL:

    virtual void    modifyOffset(const atVector &offset);

public:

               vsBoundingPlane(atVector position, atVector normal);
               vsBoundingPlane(double a, double b, double c, double d);
    virtual    ~vsBoundingPlane();

    virtual const char    *getClassName();

    void       update(atVector position, atVector normal);
    void       update(double a, double b, double c, double d);
};

#endif

