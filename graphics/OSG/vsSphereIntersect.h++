#ifndef VS_SPHERE_INTERSECT_HPP
#define VS_SPHERE_INTERSECT_HPP

#include "vsGrowableArray.h++"
#include "vsSphere.h++"
#include "vsMatrix.h++"
#include "vsVector.h++"
#include "vsGeometry.h++"

#define VS_SPH_ISECT_MAX_SPHERES 32

enum VS_GRAPHICS_DLL vsSphereIntersectSwitchTraversalMode
{
    VS_SPH_ISECT_SWITCH_NONE,
    VS_SPH_ISECT_SWITCH_CURRENT,
    VS_SPH_ISECT_SWITCH_ALL
};

enum VS_GRAPHICS_DLL vsSphereIntersectSequenceTraversalMode
{
    VS_SPH_ISECT_SEQUENCE_NONE,
    VS_SPH_ISECT_SEQUENCE_CURRENT,
    VS_SPH_ISECT_SEQUENCE_ALL
};

enum VS_GRAPHICS_DLL vsSphereIntersectLODTraversalMode
{
    VS_SPH_ISECT_LOD_NONE,
    VS_SPH_ISECT_LOD_FIRST,
    VS_SPH_ISECT_LOD_ALL
};

class VS_GRAPHICS_DLL vsSphereIntersect
{
private:

    vsGrowableArray    sphereList;
    int                sphereListSize;
    int                sphereListChanged;

    vsSphere           boundSphere;

    bool               pathsEnabled;
    int                switchTravMode;
    int                sequenceTravMode;
    int                lodTravMode;

    unsigned int       intersectMask;

    vsMatrix           currentXform;
    vsGrowableArray    currentPath;
    int                currentPathLength;

    // Intersection results
    bool               validFlag[VS_SPH_ISECT_MAX_SPHERES];
    vsVector           sectPoint[VS_SPH_ISECT_MAX_SPHERES];
    vsVector           sectNorm[VS_SPH_ISECT_MAX_SPHERES];
    vsMatrix           sectXform[VS_SPH_ISECT_MAX_SPHERES];
    vsGeometry         *sectGeom[VS_SPH_ISECT_MAX_SPHERES];
    int                sectPrim[VS_SPH_ISECT_MAX_SPHERES];
    vsGrowableArray    *sectPath[VS_SPH_ISECT_MAX_SPHERES];

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
    bool               getClosestPoint(vsVector testPoint, vsVector A,
                                       vsVector B, vsVector C, 
                                       vsVector *closestPoint);
    bool               intersectWithBox(vsSphere sphere, osg::BoundingBox box);
    vsVector           getNormal(vsGeometry *geometry, int aIndex, int bIndex,
                                 int cIndex, int primIndex);
    void               intersectWithGeometry(int sphIndex, 
                                             vsGeometry *geometry);
    void               intersectSpheres(vsNode *targetNode);

public:

                       vsSphereIntersect();
    virtual            ~vsSphereIntersect();

    void               setSphereListSize(int newSize);
    int                getSphereListSize();

    void               setSphere(int sphNum, vsVector center, double radius);
    vsVector           getSphereCenter(int sphNum);
    double             getSphereRadius(int sphNum);

    void               setMask(unsigned int newMask);
    unsigned int       getMask();

    void               enablePaths();
    void               disablePaths();

    void               setSwitchTravMode(int newMode);
    int                getSwitchTravMode();

    void               setSequenceTravMode(int newMode);
    int                getSequenceTravMode();

    void               setLODTravMode(int newMode);
    int                getLODTravMode();

    void               intersect(vsNode *targetNode);

    bool               getIsectValid(int sphNum);
    vsVector           getIsectPoint(int sphNum);
    vsVector           getIsectNorm(int sphNum);
    vsMatrix           getIsectXform(int sphNum);
    vsGeometry         *getIsectGeometry(int sphNum);
    int                getIsectPrimNum(int sphNum);
    vsGrowableArray    *getIsectPath(int sphNum);
};

#endif
