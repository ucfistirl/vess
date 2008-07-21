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
//    VESS Module:  vsBoundingVolume.h++
//
//    Description:  A vsBoundingVolume is the primary class used for
//                  collision detection. Each vsBoundingVolume is a
//                  composite of one or more vsBoundingSurfaces. This
//                  version uses the built-in ODE collision engine.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_BOUNDING_VOLUME_HPP
#define VS_BOUNDING_VOLUME_HPP

#include <ode/ode.h>

#include "vsBoundingSurface.h++"
#include "vsContactPoint.h++"
#include "vsObject.h++"

#include "atList.h++"
#include "atVector.h++"

#include "vsGlobals.h++"

#define VS_BOUNDING_VOLUME_MAX_COLLISIONS 10

struct VESS_SYM vsCollisionResult
{
    vsContactPoint    **contactPoints;
    int               contactCount;
};

struct VESS_SYM vsCollisionProgress
{
    dContactGeom         *contactGeoms;

    int                  maxCollisions;
    int                  curCollisions;
};

class VESS_SYM vsBoundingVolume : public vsObject
{
private:

    atList         *surfaceList;

    dSpaceID       odeSpaceID;

    atVector       volumeOffset;

    bool           volumeLocked;

    static void    nearCallback(void *data, dGeomID geomA, dGeomID geomB);

VS_INTERNAL:

    bool        lock();
    void        unlock();
    bool        isLocked();

    dSpaceID    getODESpaceID();

    void        setSurfaceOffset(const atVector &offset);

public:

                         vsBoundingVolume();
    virtual              ~vsBoundingVolume();

    void                 addSurface(vsBoundingSurface *surface);

    int                  getSurfaceCount();
    vsBoundingSurface    *getSurface(int index);

    void                 clear();

    vsCollisionResult    *collide(vsBoundingVolume *target);
    vsCollisionResult    *collide(vsBoundingVolume *target, int max);
};

#endif

