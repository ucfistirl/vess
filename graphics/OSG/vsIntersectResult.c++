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
//    VESS Module:  vsIntersectResult.c++
//
//    Description:  
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include <stdio.h>

#include "vsIntersectResult.h++"

// ------------------------------------------------------------------------
// Constructor - Default
// The default constructor creates an invalid result and populates its
// fields with rational defaults.
// ------------------------------------------------------------------------
vsIntersectResult::vsIntersectResult()
{
    // Set default fields.
    validFlag = false;
    isectPoint.set(0.0, 0.0, 0.0);
    isectNormal.set(0.0, 0.0, 0.0);
    isectXform.setIdentity();
    isectGeometry = NULL;
    isectPrimitiveIndex = 0;
    isectPath = new vsList();
}

// ------------------------------------------------------------------------
// Constructor
// This constructor accepts the necessary fields and assumes validity.
// ------------------------------------------------------------------------
vsIntersectResult::vsIntersectResult(atVector point, atVector normal,
    atMatrix xform, vsGeometry *geometry, int primitiveIndex)
{
    // Indicate that this is a valid intersection.
    validFlag = true;

    // Store the basic fields.
    isectPoint.copy(point);
    isectNormal.copy(normal);
    isectXform.copy(xform);
    isectPrimitiveIndex = primitiveIndex;

    // Store the geometry (which is allowed to be NULL) and add a reference.
    isectGeometry = geometry;
    if (isectGeometry)
        isectGeometry->ref();

    // Begin with an empty list.
    isectPath = new vsList();
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsIntersectResult::~vsIntersectResult()
{
    // Unreference the geometry
    if (isectGeometry)
        vsObject::unrefDelete(isectGeometry);

    // Clean up the intersection path (this will unreference the VESS
    // nodes it contains)
    delete isectPath;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name.
// ------------------------------------------------------------------------
const char *vsIntersectResult::getClassName()
{
    return "vsIntersectResult";
}

// ------------------------------------------------------------------------
// Return whether or not we intersected anything, and hence, whether any
// of our other results are valid
// ------------------------------------------------------------------------
bool vsIntersectResult::isValid()
{
    return validFlag;
}

// ------------------------------------------------------------------------
// Return the point of intersection
// ------------------------------------------------------------------------
atVector vsIntersectResult::getPoint()
{
    return isectPoint;
}

// ------------------------------------------------------------------------
// Return the surface normal at the intersection point
// ------------------------------------------------------------------------
atVector vsIntersectResult::getNormal()
{
    return isectNormal;
}

// ------------------------------------------------------------------------
// Return the accumulated scene transform of the geometry that was
// intersected
// ------------------------------------------------------------------------
atMatrix vsIntersectResult::getXform()
{
    return isectXform;
}

// ------------------------------------------------------------------------
// Get the geometry object that was intersected
// ------------------------------------------------------------------------
vsGeometry *vsIntersectResult::getGeometry()
{
    return isectGeometry;
}

// ------------------------------------------------------------------------
// Get the index of the primitive within the geometry that was intersected
// ------------------------------------------------------------------------
int vsIntersectResult::getPrimitiveIndex()
{
    return isectPrimitiveIndex;
}

// ------------------------------------------------------------------------
// Get the traversal path from the root of the scene to the geometry that
// was intersected
// ------------------------------------------------------------------------
vsList *vsIntersectResult::getPath()
{
    return isectPath;
}

