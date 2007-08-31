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
//    VESS Module:  vsBoundingBox.c++
//
//    Description:  This vsBoundingSurface subclass represents a box.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsBoundingBox.h++"

#include "atQuat.h++"

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsBoundingBox::vsBoundingBox(const vsBox &box)
{
    double boxScaleX;
    double boxScaleY;
    double boxScaleZ;
    vsVector boxTranslation;
    atQuat boxRotation;
    dQuaternion geomQuaternion;

    // Create the transform without initially placing it in an ODE space.
    odeGeomXformID = dCreateGeomTransform(0);

    // By default, when a collision occurs, the underlying geometry will be
    // provided as a member of the collision, rather than the transform that
    // encapsulates it. Since the underlying geometry is not necessary, change
    // this so that the encapsulating transform itself will be returned.
    dGeomTransformSetInfo(odeGeomXformID, 1);

    // Fetch the scale properties of the box, as these are required in all
    // cases.
    boxScaleX = box.getScale(VS_SCALE_TYPE_X);
    boxScaleY = box.getScale(VS_SCALE_TYPE_Y);
    boxScaleZ = box.getScale(VS_SCALE_TYPE_Z);

    // Create the geometry itself from the vsBox.
    odeGeomID = dCreateBox(0, boxScaleX, boxScaleY, boxScaleZ);

    // Fetch the translation and rotation of the box.
    boxTranslation.clearCopy(box.getTranslation());
    vsQuat tempQuat = box.getRotation();
    boxRotation.set(tempQuat[0], tempQuat[1], tempQuat[2], tempQuat[3]);

    // Position the geometry relative to the origin of the box space.
    dGeomSetPosition(odeGeomID, boxTranslation[AT_X] + (boxScaleX / 2.0),
        boxTranslation[AT_X] + (boxScaleX / 2.0),
        boxTranslation[AT_X] + (boxScaleX / 2.0));

    // Set a dQuaternion from the atQuat. Note that dQuaternions store the W
    // component first, followed by the X, Y, and Z.
    geomQuaternion[0] = boxRotation[AT_W];
    geomQuaternion[1] = boxRotation[AT_X];
    geomQuaternion[2] = boxRotation[AT_Y];
    geomQuaternion[3] = boxRotation[AT_Z];

    // Now apply the rotation to the geometry.
    dGeomSetQuaternion(odeGeomID, geomQuaternion);

    // Finally, associate the geometry with its transform.
    dGeomTransformSetGeom(odeGeomXformID, odeGeomID);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsBoundingBox::~vsBoundingBox()
{
    // Destroy the dynamic geometry and associated transform.
    dGeomDestroy(odeGeomID);
    dGeomDestroy(odeGeomXformID);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name.
// ------------------------------------------------------------------------
const char *vsBoundingBox::getClassName()
{
    return "vsBoundingBox";
}

// ------------------------------------------------------------------------
// This method updates the bounding surface to conform to the given box. It
// keeps the transform of the underlying geometry, disrupting only the
// relationship between the geometry and its transform, which allows the
// fundamental surface type to be modified without causing any problems.
// ------------------------------------------------------------------------
void vsBoundingBox::update(const vsBox &box)
{
    double boxScaleX;
    double boxScaleY;
    double boxScaleZ;
    vsVector boxTranslation;
    atQuat boxRotation;
    dQuaternion geomQuaternion;

    // Fetch the scale properties of the box, as these are required in all
    // cases.
    boxScaleX = box.getScale(VS_SCALE_TYPE_X);
    boxScaleY = box.getScale(VS_SCALE_TYPE_Y);
    boxScaleZ = box.getScale(VS_SCALE_TYPE_Z);

    // Set the lengths on the geom to the scales of the new box.
    dGeomBoxSetLengths(odeGeomID, boxScaleX, boxScaleY, boxScaleZ);

    // Fetch the translation and rotation of the box.
    boxTranslation.clearCopy(box.getTranslation());
    vsQuat tempQuat = box.getRotation();
    boxRotation.set(tempQuat[0], tempQuat[1], tempQuat[2], tempQuat[3]);

    // Position the geometry relative to the origin of the box space. Since the
    // provided box is axis-aligned where the dGeom box is centered, add an
    // offset equal to half of the scale of the box.
    dGeomSetPosition(odeGeomID, boxTranslation[AT_X] + (boxScaleX / 2.0),
        boxTranslation[AT_Y] + (boxScaleY / 2.0),
        boxTranslation[AT_Z] + (boxScaleZ / 2.0));

    // Set a dQuaternion from the atQuat. Note that dQuaternions store the W
    // component first, followed by the X, Y, and Z.
    geomQuaternion[0] = boxRotation[AT_W];
    geomQuaternion[1] = boxRotation[AT_X];
    geomQuaternion[2] = boxRotation[AT_Y];
    geomQuaternion[3] = boxRotation[AT_Z];

    // Now apply the rotation to the geometry.
    dGeomSetQuaternion(odeGeomID, geomQuaternion);
}

// ------------------------------------------------------------------------
// Virtual VESS Internal Function
// Returns the geom ID containing the geometry of this vsBoundingBox. In
// the case of a bounding box, a separate dGeomID representing the
// transformed geometry is returned to allow the box to have an offset.
// ------------------------------------------------------------------------
dGeomID vsBoundingBox::getODEGeomID()
{
    return odeGeomXformID;
}

