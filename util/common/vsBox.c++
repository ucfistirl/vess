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
//    VESS Module:  vsBox.c++
//
//    Description:  Util library class that contains a representation of
//                  a box as a corner point and three orthogonal vectors.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include <stdio.h>
#include <math.h>
#include "atMatrix.h++"
#include "vsBox.h++"

//------------------------------------------------------------------------
// Constructor - Creates an axis-aligned unit box.
//------------------------------------------------------------------------
vsBox::vsBox()
{
    // The axis scales should all be 1.0 for the unit box.
    scaleVector.set(1.0, 1.0, 1.0);

    // The box should be neither translated nor rotated.
    translationVector.set(0.0, 0.0, 0.0);
    rotationQuat.set(0.0, 0.0, 0.0, 1.0);
}

//------------------------------------------------------------------------
// Constructor - Creates a box with the specified scales, corner point
// translation, and orientation.
//------------------------------------------------------------------------
vsBox::vsBox(const double &scaleX, const double &scaleY,
    const double &scaleZ, const atVector &translation, const atQuat &rotation)
{
    setBox(scaleX, scaleY, scaleZ, translation, rotation);
}

//------------------------------------------------------------------------
// Constructor - Creates a box with the specified corner point and axis
// vectors.
//------------------------------------------------------------------------
vsBox::vsBox(const atVector &corner, const atVector &axisX,
    const atVector &axisY, const atVector &axisZ)
{
    setBox(corner, axisX, axisY, axisZ);
}

//------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------
vsBox::~vsBox()
{
}

//------------------------------------------------------------------------
// Virtual function
//------------------------------------------------------------------------
const char *vsBox::getClassName()
{
    return "vsBox";
}

//------------------------------------------------------------------------
// Sets the box to have the same scales, corner point translation, and
// rotation as the provided box.
//------------------------------------------------------------------------
void vsBox::setBox(const vsBox &box)
{
    // Copy the scale values.
    scaleVector.set(box.getScale(VS_SCALE_TYPE_X),
        box.getScale(VS_SCALE_TYPE_Y), box.getScale(VS_SCALE_TYPE_Z));

    // Copy the translation.
    translationVector.clearCopy(box.getTranslation());

    // Copy the rotation.
    rotationQuat.copy(box.getRotation());
}

//------------------------------------------------------------------------
// Virtual function
// Sets the box to have the specified scales, corner point translation,
// and rotation.
//------------------------------------------------------------------------
void vsBox::setBox(const double &scaleX, const double &scaleY,
    const double &scaleZ, const atVector &translation, const atQuat &rotation)
{
    // Copy the scale values.
    scaleVector.set(scaleX, scaleY, scaleZ);

    // Copy the translation.
    translationVector.clearCopy(translation);

    // Copy the rotation.
    rotationQuat.copy(rotation);
}

//------------------------------------------------------------------------
// Virtual function
// Set the box to have the specified corner point and axis vectors.
//------------------------------------------------------------------------
void vsBox::setBox(const atVector &corner, const atVector &axisX,
    const atVector &axisY, const atVector &axisZ)
{
    // The translation is simply the location of the corner point.
    translationVector.clearCopy(corner);

    // The scales are simply the lengths of the axes.
    scaleVector.set(
        axisX.getMagnitude(), axisY.getMagnitude(), axisZ.getMagnitude());

    // Set the rotation quaternion based on the y- and z-axes.
    rotationQuat.setVecsRotation(atVector(0.0, 1.0, 0.0),
        atVector(0.0, 0.0, 1.0), axisY, axisZ);
}

//------------------------------------------------------------------------
// Virtual function
// Sets the scale of a given edge of this box. The supported types are
// VS_SCALE_TYPE_X, VS_SCALE_TYPE_Y, and VS_SCALE_TYPE_Z.
//------------------------------------------------------------------------
void vsBox::setScale(vsScaleType type, double value)
{
    // Return the correct scale value.
    if (type == VS_SCALE_TYPE_X)
    {
        scaleVector[AT_X] = value;
    }
    else if (type == VS_SCALE_TYPE_Y)
    {
        scaleVector[AT_Y] = value;
    }
    else if (type == VS_SCALE_TYPE_Z)
    {
        scaleVector[AT_Z] = value;
    }
}

//------------------------------------------------------------------------
// Virtual function
// This method takes a scale type and returns the double value associated
// with that type, or 0.0 if the scale type is undefined for this shape.
// The supported types are VS_SCALE_TYPE_X, VS_SCALE_TYPE_Y, and
// VS_SCALE_TYPE_Z.
//------------------------------------------------------------------------
double vsBox::getScale(vsScaleType type) const
{
    // Return the correct scale value.
    if (type == VS_SCALE_TYPE_X)
    {
        return scaleVector[AT_X];
    }
    else if (type == VS_SCALE_TYPE_Y)
    {
        return scaleVector[AT_Y];
    }
    else if (type == VS_SCALE_TYPE_Z)
    {
        return scaleVector[AT_Z];
    }

    // Return a default value.
    return 0.0;
}

//------------------------------------------------------------------------
// Virtual function
// This method takes a value indicating one of the eight corners of the
// box and returns the atVector describing the point at that corner.
//------------------------------------------------------------------------
atVector vsBox::getCorner(vsBoxCorner corner) const
{
    atVector returnPoint;

    // Determine which corner is to be returned. In the case of the front-
    // bottom-left corner, which is the origin of the box, the return is
    // trivial. In all other cases, the return point is set as an appropriate
    // point in box space, then transformed to the correct location.
    if (corner == VS_BOX_CORNER_FBL)
    {
        // In the case of the front-bottom-left corner, which is the origin of
        // the box, the return is trivially the translation vector. 
        return translationVector;
    }
    else if (corner == VS_BOX_CORNER_FBR)
    {
        returnPoint.set(scaleVector[AT_X], 0.0, 0.0);
    }
    else if (corner == VS_BOX_CORNER_FTL)
    {
        returnPoint.set(0.0, 0.0, scaleVector[AT_Z]);
    }
    else if (corner == VS_BOX_CORNER_FTR)
    {
        returnPoint.set(scaleVector[AT_X], 0.0, scaleVector[AT_Z]);
    }
    else if (corner == VS_BOX_CORNER_BBL)
    {
        returnPoint.set(0.0, scaleVector[AT_Y], 0.0);
    }
    else if (corner == VS_BOX_CORNER_BBR)
    {
        returnPoint.set(scaleVector[AT_X], scaleVector[AT_Y], 0.0);
    }
    else if (corner == VS_BOX_CORNER_BTL)
    {
        returnPoint.set(0.0, scaleVector[AT_Y], scaleVector[AT_Z]);
    }
    else if (corner == VS_BOX_CORNER_BTR)
    {
        returnPoint.set(scaleVector[AT_X], scaleVector[AT_Y],
            scaleVector[AT_Z]);
    }

    // Now rotate the return point by the orientation.
    returnPoint = rotationQuat.rotatePoint(returnPoint);

    // Now translate the point by the box translation and return the result.
    return returnPoint.getSum(translationVector);
}

//------------------------------------------------------------------------
// Virtual function
// This method takes in a point as a atVector and determines if it the 
// point is inside the box
//------------------------------------------------------------------------
bool vsBox::isPointInside(const atVector &point) const
{
    atVector transformedPoint, min, max;
    
    // Must first transform point into object space
    // Rotate
    transformedPoint = rotationQuat.rotatePoint(transformedPoint);
    // Translate
    transformedPoint = point.getSum(translationVector);

    // Can now perform min-max extent testing
	min = getCorner(VS_BOX_CORNER_FBL);
    max = getCorner(VS_BOX_CORNER_BTR);
    // Point is inside if it is bounded by the min/max extents
    return (transformedPoint[AT_X] >= min[AT_X] && 
            transformedPoint[AT_X] <= max[AT_X]) &&
           (transformedPoint[AT_Y] >= min[AT_Y] &&
            transformedPoint[AT_Y] <= max[AT_Y]) &&
           (transformedPoint[AT_Z] >= min[AT_Z] &&
            transformedPoint[AT_Z] <= max[AT_Z]);
}
