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
#include "vsVector.h++"

class vsSphere
{
private:

    vsVector    center;
    double      radius;

    void        promote(vsVector *points, int index);

    vsSphere    calcSphereOn(vsVector *points, int pointCount);
    vsSphere    moveToFront(vsVector *points, int pointCount,
                            vsVector *basis, int basisCount,
                            int *supportSize);

    void        promote(vsSphere *spheres, int index);

    vsSphere    calcSphereAround(vsSphere *spheres, int sphereCount);
    vsSphere    moveToFront(vsSphere *spheres, int sphereCount,
                            vsSphere *basis, int basisCount,
                            int *supportSize);

public:

                vsSphere();
                vsSphere(const vsVector &centerPoint,
                         const double &sphereRadius);
                ~vsSphere();

    void        setEmpty();
    void        setSphere(const vsVector &centerPoint,
                          const double &sphereRadius);

    vsVector    getCenterPoint() const;
    double      getRadius() const;

    void        addPoint(const vsVector &point);
    void        addSphere(const vsSphere &sphere);

    void        enclosePoints(vsVector *points, int pointCount);
    void        encloseSpheres(vsSphere *spheres, int sphereCount);

    bool        isPointInside(const vsVector &point) const;
    bool        isSphereInside(const vsSphere &sphere) const;
    bool        isSegIsect(const vsVector &segStart,
                           const vsVector &segEnd) const;

    void        print() const;
    void        print(FILE *fp) const;
};

#endif
