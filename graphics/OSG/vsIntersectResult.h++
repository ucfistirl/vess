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
//    VESS Module:  vsIntersectResult.h++
//
//    Description:  
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_INTERSECT_RESULT_HPP
#define VS_INTERSECT_RESULT_HPP

#include "atMatrix.h++"
#include "atVector.h++"

#include "vsList.h++"
#include "vsGeometry.h++"

class VESS_SYM vsIntersectResult : public vsObject
{
private:

    bool          validFlag;
    atVector      isectPoint;
    atVector      isectNormal;
    atMatrix      isectXform;
    vsGeometry    *isectGeometry;
    int           isectPrimitiveIndex;
    vsList        *isectPath;

public:

                         vsIntersectResult();
                         vsIntersectResult(atVector point, atVector normal,
                                           atMatrix xform,
                                           vsGeometry *geometry,
                                           int primitiveIndex);
    virtual              ~vsIntersectResult();

    virtual const char   *getClassName();

    bool                 isValid();
    atVector             getPoint();
    atVector             getNormal();
    atMatrix             getXform();
    vsGeometry           *getGeometry();
    int                  getPrimitiveIndex();
    vsList               *getPath();
};

#endif

