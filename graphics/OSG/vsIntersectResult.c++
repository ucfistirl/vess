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
    isectPath = new atList();
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
    isectPath = new atList();
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsIntersectResult::~vsIntersectResult()
{
    if (isectGeometry)
        vsObject::unrefDelete(isectGeometry);

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
// ------------------------------------------------------------------------
bool vsIntersectResult::isValid()
{
    return validFlag;
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
atVector vsIntersectResult::getPoint()
{
    return isectPoint;
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
atVector vsIntersectResult::getNormal()
{
    return isectNormal;
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
atMatrix vsIntersectResult::getXform()
{
    return isectXform;
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
vsGeometry *vsIntersectResult::getGeometry()
{
    return isectGeometry;
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
int vsIntersectResult::getPrimitiveIndex()
{
    return isectPrimitiveIndex;
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
atList *vsIntersectResult::getPath()
{
    return isectPath;
}

