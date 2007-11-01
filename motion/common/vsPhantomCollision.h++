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
//    VESS Module:  vsPhantomCollision.h++
//
//    Description:  Motion model that implements collision detection for
//		            any object and apply forces to a Phantom.  Works by
//                  taking a set of designated 'hot' points on an object
//                  and making sure that none of those points pass through
//                  a solid object.
//
//    Author(s):    Bryan Kline, Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_PHANTOM_COLLISION_HPP
#define VS_PHANTOM_COLLISION_HPP

#include "vsMotionModel.h++"
#include "vsKinematics.h++"
#include "vsSphereIntersect.h++"
#include "vsPhantomSystem.h++"

#define VS_PHANTOM_COLLISION_POINTS_MAX        VS_SPH_ISECT_MAX_SPHERES
#define VS_PHANTOM_COLLISION_DEFAULT_RADIUS    0.02
#define VS_PHANTOM_COLLISION_DEFAULT_FORCE     4.0
#define VS_PHANTOM_COLLISION_MAX_FORCE         8.5
#define VS_PHANTOM_COLLISION_MAX_PASSES        10

// Define this to have the normal and force vectors drawn as lines.
//#define VS_PHANTOM_COLLISION_DEBUG

class VS_MOTION_DLL vsPhantomCollision : public vsMotionModel
{
private:

    vsPhantomSystem    *phantomSys;
    vsKinematics       *kinematics;
    vsNode             *scene;
 
    vsSphereIntersect  *intersect;

    atVector           offsetPoints[VS_PHANTOM_COLLISION_POINTS_MAX];
    int                offsetCount;

#ifdef VS_PHANTOM_COLLISION_DEBUG
    vsGeometry         *forceLine;
    vsGeometry         *vertOneLine;
    vsGeometry         *vertTwoLine;
    vsGeometry         *vertThreeLine;
#endif
 
    double             sphereRadius;
    double             maximumForce;
 
    double             getCollisionData(atMatrix globalXform,
                                        atVector *hitNorm);
 
public:

                          vsPhantomCollision(vsPhantomSystem *phantomSys,
                                             vsKinematics *objectKin,
                                             vsNode *theScene);
    virtual               ~vsPhantomCollision();

    virtual const char    *getClassName();

    void                  setPointCount(int numPoints);
    int                   getPointCount();

    void                  setPoint(int index, atVector newOffset);
    atVector              getPoint(int index);

    void                  setIntersectMask(unsigned int newMask);
    unsigned int          getIntersectMask();
 
    void                  setRadius(double newRadius);
    double                getRadius();

    void                  setMaxForce(double newMaxForce);
    double                getMaxForce();

    virtual void          update();
};

#endif
