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
//    VESS Module:  vsCollision.h++
//
//    Description:  Motion model that implements collision detection for
//		    any object. Works by taking a set of designated 'hot'
//		    points on an object and making sure that none of those
//		    points pass through a solid object.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_COLLISION_HPP
#define VS_COLLISION_HPP

#include "vsMotionModel.h++"
#include "vsKinematics.h++"
#include "vsIntersect.h++"

#define VS_COLLISION_POINTS_MAX     VS_INTERSECT_SEGS_MAX
#define VS_COLLISION_DEFAULT_MARGIN 0.01
#define VS_COLLISION_MAX_PASSES     10

enum VS_MOTION_DLL vsCollisionMode
{
    VS_COLLISION_MODE_STOP,
    VS_COLLISION_MODE_SLIDE,
    VS_COLLISION_MODE_BOUNCE
};

class VS_MOTION_DLL vsCollision : public vsMotionModel
{
private:

    vsKinematics    *kinematics;
    vsNode          *scene;
    
    vsIntersect     *intersect;
    
    vsVector        offsetPoints[VS_COLLISION_POINTS_MAX];
    int             offsetCount;
    
    int             collisionMode;
    
    double          wallMargin;
    
    double          distance(vsVector start, vsVector end);
    vsVector        fixNormal(vsVector sourcePt, vsVector isectPt,
                              vsVector isectNorm);
    
    double          calcMoveAllowed(vsMatrix globalXform, vsVector posOffset,
                                    vsVector moveDir, double maxMove,
                                    vsVector *hitNorm);
    
public:

                          vsCollision(vsKinematics *objectKin, vsNode *theScene);
    virtual               ~vsCollision();

    virtual const char    *getClassName();

    void                  setPointCount(int numPoints);
    int                   getPointCount();

    void                  setPoint(int index, vsVector newOffset);
    vsVector              getPoint(int index);

    void                  setCollisionMode(int newMode);
    int                   getCollisionMode();

    void                  setIntersectMask(unsigned int newMask);
    unsigned int          getIntersectMask();
    
    void                  setMargin(double newMargin);
    double                getMargin();

    virtual void          update();
};

#endif
