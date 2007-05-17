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
//    VESS Module:  vsBox.h++
//
//    Description:  Util library class that contains a representation of
//                  a box as a scale, a translation, and a rotation.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_BOX_HPP
#define VS_BOX_HPP

#include "vsGlobals.h++"
#include "vsQuat.h++"
#include "vsShape.h++"
#include "vsVector.h++"

enum vsBoxCorner
{
    VS_BOX_CORNER_FBL,
    VS_BOX_CORNER_FBR,
    VS_BOX_CORNER_FTL,
    VS_BOX_CORNER_FTR,
    VS_BOX_CORNER_BBL,
    VS_BOX_CORNER_BBR,
    VS_BOX_CORNER_BTL,
    VS_BOX_CORNER_BTR
};

class VS_UTIL_DLL vsBox : public vsShape
{
protected:

    vsVector    scaleVector;

public:

                          vsBox();
                          vsBox(const double &scaleX, const double &scaleY,
                              const double &scaleZ, const vsVector &translation,
                              const vsQuat &rotation);
                          vsBox(const vsVector &corner, const vsVector &axisX,
                              const vsVector &axisY, const vsVector &axisZ);
                          ~vsBox();

    virtual const char    *getClassName();

    void                  setBox(const vsBox &box);
    virtual void          setBox(const double &scaleX, const double &scaleY,
                              const double &scaleZ, const vsVector &translation,
                              const vsQuat &rotation);
    virtual void          setBox(const vsVector &corner, const vsVector &axisX,
                              const vsVector &axisY, const vsVector &axisZ);

    virtual void          setScale(vsScaleType type, double value);

    virtual double        getScale(vsScaleType type) const;

    virtual vsVector      getCorner(vsBoxCorner corner) const;

    virtual bool          isPointInside(const vsVector &point) const;
};

#endif

