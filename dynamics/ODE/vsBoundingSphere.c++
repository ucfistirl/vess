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
//    VESS Module:  vsBoundingSphere.c++
//
//    Description:  This vsBoundingSurface subclass represents a sphere.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsBoundingSphere.h++"

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsBoundingSphere::vsBoundingSphere(const vsSphere &sphere)
{
    vsVector sphereTranslation;

    // Create the transform without initially placing it in an ODE space.
    odeGeomXformID = dCreateGeomTransform(0);

    // By default, when a collision occurs, the underlying geometry will be
    // provided as a member of the collision, rather than the transform that
    // encapsulates it. Since the underlying geometry is not necessary, change
    // this so that the encapsulating transform itself will be returned.
    dGeomTransformSetInfo(odeGeomXformID, 1);

    // Create the geometry itself from the vsSphere.
    odeGeomID = dCreateSphere(0, sphere.getScale(VS_SCALE_TYPE_RADIUS));

    // Fetch the translation of the sphere.
    sphereTranslation.clearCopy(sphere.getTranslation());

    // Position the geometry relative to the origin of the sphere space.
    dGeomSetPosition(odeGeomID, sphereTranslation[VS_X],
        sphereTranslation[VS_Y], sphereTranslation[VS_Z]);

    // Finally, associate the geometry with its transform.
    dGeomTransformSetGeom(odeGeomXformID, odeGeomID);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsBoundingSphere::~vsBoundingSphere()
{
    // Destroy the dynamic geometry and associated transform.
    dGeomDestroy(odeGeomID);
    dGeomDestroy(odeGeomXformID);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name.
// ------------------------------------------------------------------------
const char *vsBoundingSphere::getClassName()
{
    return "vsBoundingSphere";
}

// ------------------------------------------------------------------------
// This method updates the bounding surface to conform to the given sphere.
// ------------------------------------------------------------------------
void vsBoundingSphere::update(const vsSphere &sphere)
{
    double sphereRadius;
    vsVector sphereTranslation;

    // Fetch the translation and rotation of the box.
    sphereTranslation.clearCopy(sphere.getTranslation());

    // Position the geometry relative to the origin of the box space.
    dGeomSetPosition(odeGeomID, sphereTranslation[VS_X],
        sphereTranslation[VS_Y], sphereTranslation[VS_Z]);

    // Fetch the radius of the sphere.
    sphereRadius = sphere.getScale(VS_SCALE_TYPE_RADIUS);

    // Put the radius into the sphere itself.
    dGeomSphereSetRadius(odeGeomID, sphereRadius);
}

// ------------------------------------------------------------------------
// Virtual VESS Internal Function
// Returns the geom ID containing the geometry of this vsBoundingSphere. In
// the case of a bounding sphere, a separate dGeomID representing the
// transformed geometry is returned to allow the box to have an offset.
// ------------------------------------------------------------------------
dGeomID vsBoundingSphere::getODEGeomID()
{
    return odeGeomXformID;
}

