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
//    VESS Module:  vsSphere.h++
//
//    Description:  Math library class that contains a representation of
//                  a sphere as a center point and radius.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_SPHERE_HPP
#define VS_SPHERE_HPP

#include "vsGlobals.h++"
#include "vsShape.h++"
#include "vsVector.h++"

class VS_UTIL_DLL vsSphere : public vsShape
{
private:

    double      radius;

    void        promote(vsVector *points, int index);

    vsSphere    calcSphereOn(vsVector *points, int pointCount,
                             bool *errorFlag);
    vsSphere    moveToFront(vsVector *points, int pointCount,
                            vsVector *basis, int basisCount,
                            int *supportSize, bool *errorFlag);

    void        promote(vsSphere *spheres, int index);

    vsSphere    calcSphereAround(vsSphere *spheres, int sphereCount,
                                 bool *errorFlag);
    vsSphere    moveToFront(vsSphere *spheres, int sphereCount,
                            vsSphere *basis, int basisCount,
                            int *supportSize, bool *errorFlag);

public:

                          vsSphere();
                          vsSphere(const vsVector &centerPoint,
                                   const double &sphereRadius);
                          ~vsSphere();

    virtual const char    *getClassName();

    void                  setEmpty();
    virtual void          setSphere(const vsVector &center,
                              const double &radius);

    virtual void          setScale(vsScaleType type, double value);

    virtual double        getScale(vsScaleType type) const;

    vsVector              getCenterPoint() const;
    double                getRadius() const;

    void                  addPoint(const vsVector &point);
    void                  addSphere(const vsSphere &sphere);

    void                  enclosePoints(vsVector *points, int pointCount);
    void                  encloseSpheres(vsSphere *spheres, int sphereCount);

    bool                  isPointInside(const vsVector &point) const;
    bool                  isSphereInside(const vsSphere &sphere) const;
    bool                  isSegIsect(const vsVector &segStart,
                                     const vsVector &segEnd) const;
    bool                  isSphereIsect(const vsSphere &sphere) const;

    void                  print() const;
    void                  print(FILE *fp) const;
};

#endif
