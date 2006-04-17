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
//    VESS Module:  vsShape.h++
//
//    Description:  This class represents a generic shape in 3D space. It
//                  internally maintains the translation and rotation of
//                  the shape. Note that this class is abstract and may not
//                  be directly instantiated.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_SHAPE_HPP
#define VS_SHAPE_HPP

#include "vsGlobals.h++"
#include "vsObject.h++"
#include "vsQuat.h++"
#include "vsVector.h++"

enum VS_UTIL_DLL vsScaleType
{
    VS_SCALE_TYPE_X,
    VS_SCALE_TYPE_Y,
    VS_SCALE_TYPE_Z,
    VS_SCALE_TYPE_RADIUS
};

class VS_UTIL_DLL vsShape : public vsObject
{
protected:

    vsVector    translationVector;
    vsQuat      rotationQuat;

public:

                          vsShape();
    virtual               ~vsShape();

    virtual const char    *getClassName() = 0;

    virtual void          setRotation(const vsQuat &rotation);
    virtual void          setScale(vsScaleType type, double value) = 0;
    virtual void          setTranslation(const vsVector &translation);

    virtual vsQuat        getRotation() const;
    virtual double        getScale(vsScaleType type) const = 0;
    virtual vsVector      getTranslation() const;
    
    virtual bool          isPointInside(const vsVector &point) const = 0;
};

#endif

