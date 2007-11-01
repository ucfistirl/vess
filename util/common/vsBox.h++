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
#include "atQuat.h++"
#include "vsShape.h++"
#include "atVector.h++"

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

    atVector    scaleVector;

public:

                          vsBox();
                          vsBox(const double &scaleX, const double &scaleY,
                              const double &scaleZ, const atVector &translation,
                              const atQuat &rotation);
                          vsBox(const atVector &corner, const atVector &axisX,
                              const atVector &axisY, const atVector &axisZ);
                          ~vsBox();

    virtual const char    *getClassName();

    void                  setBox(const vsBox &box);
    virtual void          setBox(const double &scaleX, const double &scaleY,
                              const double &scaleZ, const atVector &translation,
                              const atQuat &rotation);
    virtual void          setBox(const atVector &corner, const atVector &axisX,
                              const atVector &axisY, const atVector &axisZ);

    virtual void          setScale(vsScaleType type, double value);

    virtual double        getScale(vsScaleType type) const;

    virtual atVector      getCorner(vsBoxCorner corner) const;

    virtual bool          isPointInside(const atVector &point) const;
};

#endif

