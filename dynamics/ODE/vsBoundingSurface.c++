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
//    VESS Module:  vsBoundingSurface.c++
//
//    Description:  This abstract class represents a bounding surface. A
//                  vsBoundingVolume (used for collision testing) is made
//                  up of one or more instances of a vsBoundingSurface.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsBoundingSurface.h++"

// ------------------------------------------------------------------------
// Constructor
// As this class is abstract, its constructor does nothing.
// ------------------------------------------------------------------------
vsBoundingSurface::vsBoundingSurface()
{
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsBoundingSurface::~vsBoundingSurface()
{
}

// ------------------------------------------------------------------------
// Virtual VESS Internal Function
// Returns the geom ID containing the geometry of this vsBoundingSurface.
// ------------------------------------------------------------------------
dGeomID vsBoundingSurface::getODEGeomID()
{
    return odeGeomID;
}

// ------------------------------------------------------------------------
// Virtual VESS Internal Function
// Translate the bounding surface by the given vector. This functionality
// is required to maintain the specification of collision geometry in
// model-relative rather than mass-relative coordinates.
// ------------------------------------------------------------------------
void vsBoundingSurface::modifyOffset(const atVector &offset)
{
    const dReal *position;

    // Fetch the current position of the surface.
    position = dGeomGetPosition(odeGeomID);

    // Set the new position with the new offset.
    dGeomSetPosition(odeGeomID, position[0] + offset[0],
        position[1] + offset[1], position[2] + offset[2]);
}

