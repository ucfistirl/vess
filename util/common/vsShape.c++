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
//    VESS Module:  vsShape.c++
//
//    Description:  This class represents a generic shape in 3D space. It
//                  internally maintains the translation and rotation of
//                  the shape. Note that this class is abstract an may not
//                  be directly instantiated.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsShape.h++"

//------------------------------------------------------------------------
// Constructor - 
//------------------------------------------------------------------------
vsShape::vsShape()
{
    // The object by default should be neither translated nor rotated.
    translationVector.set(0.0, 0.0, 0.0);
    rotationQuat.set(0.0, 0.0, 0.0, 1.0);
}

//------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------
vsShape::~vsShape()
{
}

//------------------------------------------------------------------------
// Virtual function
// Sets the rotation of this shape.
//------------------------------------------------------------------------
void vsShape::setRotation(const vsQuat &rotation)
{
    rotationQuat.copy(rotation);
}

//------------------------------------------------------------------------
// Virtual function
// Sets the scale of this shape. Since a generic shape has no associated
// scales, this function does nothing.
//------------------------------------------------------------------------
void vsShape::setScale(vsScaleType type, double value)
{
}

//------------------------------------------------------------------------
// Virtual function
// Sets the translation offset of this shape.
//------------------------------------------------------------------------
void vsShape::setTranslation(const vsVector &translation)
{
    translationVector.clearCopy(translation);
}

//------------------------------------------------------------------------
// Virtual function
// Returns the rotation of this shape.
//------------------------------------------------------------------------
vsQuat vsShape::getRotation() const
{
    return rotationQuat;
}

//------------------------------------------------------------------------
// Virtual function
// Returns a given scale type for this shape. Since a generic shape has no
// associated scales, this function always returns 0.
//------------------------------------------------------------------------
double vsShape::getScale(vsScaleType type) const
{
    return 0.0;
}

//------------------------------------------------------------------------
// Virtual function
// Returns the translation offset of this shape.
//------------------------------------------------------------------------
vsVector vsShape::getTranslation() const
{
    return translationVector;
}

//------------------------------------------------------------------------
// Virtual function
// Returns if the specified point is bounded by the shape. Since a generic
// shape does not have complete geometric information, this always returns
// false.
//------------------------------------------------------------------------
bool vsShape::isPointInside(const vsVector &point) const
{
    return false;
}
