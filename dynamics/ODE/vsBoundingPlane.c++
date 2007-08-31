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
//    VESS Module:  vsBoundingPlane.c++
//
//    Description:  This vsBoundingSurface subclass represents a plane.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsBoundingPlane.h++"

// ------------------------------------------------------------------------
// Constructor - This version of the constructor takes a position through
// which the desired plane passes and a normal vector in world coordinates.
// ------------------------------------------------------------------------
vsBoundingPlane::vsBoundingPlane(atVector position, atVector normal)
{
    double d;

    // Confirm that the normal vector is normalized.
    normal.normalize();

    // Calculate the 'd' value from the normal vector and the position that
    // the plane passes through, using the plane equation:
    // ax + by + cz = d
    d = (position[AT_X] * normal[AT_X]) + (position[AT_Y] * normal[AT_Y]) +
        (position[AT_Z] * normal[AT_Z]);

    // Create the geometry itself from the plane parameters.
    odeGeomID = dCreatePlane(0, normal[AT_X], normal[AT_Y], normal[AT_Z], d);
}

// ------------------------------------------------------------------------
// Constructor - This version takes the four parameters directly, based on
// the equation ax + by + cz = d. Note that ODE requires the normal vector
// <a, b, c> to be of unit length.
// ------------------------------------------------------------------------
vsBoundingPlane::vsBoundingPlane(double a, double b, double c, double d)
{
    // Create the geometry itself from the plane parameters.
    odeGeomID = dCreatePlane(0, a, b, c, d);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsBoundingPlane::~vsBoundingPlane()
{
    // Destroy the dynamic geometry itself.
    dGeomDestroy(odeGeomID);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name.
// ------------------------------------------------------------------------
const char *vsBoundingPlane::getClassName()
{
    return "vsBoundingPlane";
}

// ------------------------------------------------------------------------
// This version of the update method takes a position through which the
// desired plane passes and a normal vector in world coordinates.
// ------------------------------------------------------------------------
void vsBoundingPlane::update(atVector position, atVector normal)
{
    double d;

    // Confirm that the normal vector is normalized.
    normal.normalize();

    // Calculate the 'd' value from the normal vector and the position that
    // the plane passes through, using the plane equation:
    // ax + by + cz = d
    d = (position[AT_X] * normal[AT_X]) + (position[AT_Y] * normal[AT_Y]) +
        (position[AT_Z] * normal[AT_Z]);

    // Set the plane parameters.
    dGeomPlaneSetParams(odeGeomID,
        normal[AT_X], normal[AT_Y], normal[AT_Z], d);
}

// ------------------------------------------------------------------------
// This method updates the bounding surface to conform to the given plane.
// This version takes the four parameters directly, based on the equation
// ax + by + cz = d. Note that ODE requires the normal vector <a, b, c> to
// be of unit length.
// ------------------------------------------------------------------------
void vsBoundingPlane::update(double a, double b, double c, double d)
{
    // Set the plane parameters.
    dGeomPlaneSetParams(odeGeomID, a, b, c, d);
}

// ------------------------------------------------------------------------
// Virtual VESS Internal Function
// This method normally transforms the geometry itself. While it may be
// useful for it to modify the position through which the plane passes, at
// this point the method should do nothing (as ODE considers planes to be
// fixed in world space).
// ------------------------------------------------------------------------
void vsBoundingPlane::modifyOffset(const atVector &offset)
{
}

