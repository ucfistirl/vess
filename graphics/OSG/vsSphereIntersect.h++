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
//    VESS Module:  vsSphereIntersect.h++
//
//    Description:  Class for performing intersection tests between a set
//                  of spheres and a whole or part of a VESS scene graph
//
//    Author(s):    Jason Daly, Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_SPHERE_INTERSECT_HPP
#define VS_SPHERE_INTERSECT_HPP

#include "atList.h++"
#include "atMatrix.h++"
#include "atVector.h++"
#include "vsArray.h++"
#include "vsGeometry.h++"
#include "vsIntersectResult.h++"
#include "vsSphere.h++"

#define VS_SPH_ISECT_MAX_SPHERES 32

enum vsSphereIntersectSwitchTraversalMode
{
    VS_SPH_ISECT_SWITCH_NONE,
    VS_SPH_ISECT_SWITCH_CURRENT,
    VS_SPH_ISECT_SWITCH_ALL
};

enum vsSphereIntersectSequenceTraversalMode
{
    VS_SPH_ISECT_SEQUENCE_NONE,
    VS_SPH_ISECT_SEQUENCE_CURRENT,
    VS_SPH_ISECT_SEQUENCE_ALL
};

enum vsSphereIntersectLODTraversalMode
{
    VS_SPH_ISECT_LOD_NONE,
    VS_SPH_ISECT_LOD_FIRST,
    VS_SPH_ISECT_LOD_ALL
};

class VESS_SYM vsSphereIntersect
{
private:

    int                sphereListSize;
    int                sphereListChanged;
    vsArray            sphereList;
    vsArray            resultList;

    vsSphere           boundSphere;

    bool               pathsEnabled;
    int                switchTravMode;
    int                sequenceTravMode;
    int                lodTravMode;

    unsigned int       intersectMask;

    atMatrix           currentXform;
    vsList             *currentPath;

    // Parametric coordinates used during computation of the closest point
    // on a triangle, and subsequent intersection evaluation 
    double             s, t;

    // Intermediate values used in the closest point calculation, kept
    // in memory for efficiency (these are computed and shared between the 
    // computePointInRegion() and getClosestPoint() methods)
    double             a, b, c, d, e, f, det;

    // Square distance between the sphere and the closest point encountered 
    // so far on the current traversal
    double             closestSqrDist[VS_SPH_ISECT_MAX_SPHERES];

    // Intersection subroutines
    void               computePointInRegion(int regionNum);
    bool               getClosestPoint(atVector testPoint, atVector A,
                                       atVector B, atVector C, 
                                       atVector *closestPoint);
    bool               intersectWithBox(vsSphere sphere, osg::BoundingBox box);
    atVector           getNormal(vsGeometry *geometry, int aIndex, int bIndex,
                                 int cIndex, int primIndex);
    void               intersectWithGeometry(int sphIndex, 
                                             vsGeometry *geometry);
    void               intersectSpheres(vsNode *targetNode);

public:

                        vsSphereIntersect();
    virtual             ~vsSphereIntersect();

    void                setSphereListSize(int newSize);
    int                 getSphereListSize();

    void                setSphere(int sphNum, atVector center, double radius);
    atVector            getSphereCenter(int sphNum);
    double              getSphereRadius(int sphNum);

    void                setMask(unsigned int newMask);
    unsigned int        getMask();

    void                enablePaths();
    void                disablePaths();

    void                setSwitchTravMode(int newMode);
    int                 getSwitchTravMode();

    void                setSequenceTravMode(int newMode);
    int                 getSequenceTravMode();

    void                setLODTravMode(int newMode);
    int                 getLODTravMode();

    void                intersect(vsNode *targetNode);

    vsIntersectResult   *getIntersection(int sphNum);
};

#endif
