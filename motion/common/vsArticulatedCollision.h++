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
//    VESS Module:  vsArticulatedCollision.c++
//
//    Description:  Class for performing collision detection and handling
//                  on an articulated object
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_ARTICULATED_COLLISION_HPP
#define VS_ARTICULATED_COLLISION_HPP

#include "vsInverseKinematics.h++"
#include "vsIntersect.h++"
#include "vsMotionModel.h++"

#define VS_ARTCOL_SEGMENT_COUNT 16

class VS_MOTION_DLL vsArticulatedCollision : public vsMotionModel
{
protected:

    vsInverseKinematics    *invKinematics;

    vsIntersect            *intersect;
    vsNode                 *scene;
    double                 segmentRadius;

    // * Subclass and override THIS function if you want to modify how * 
    // * the object handles collisions                                 *
    virtual bool           processCollision(atVector collisionPoint,
                                            int jointSegmentIdx,
                                            int isectSegmentIdx);

public:

    // Constructor/destructor
                           vsArticulatedCollision(vsInverseKinematics *invkin,
                                                  vsNode *theScene);
    virtual                ~vsArticulatedCollision();

    // Inherited from vsObject
    virtual const char     *getClassName();

    // Inherited from vsUpdatable
    virtual void           update();

    // Set collision parameters
    void                   setSegmentRadius(double radius);
    double                 getSegmentRadius();

    // Retrieve helper objects
    vsInverseKinematics    *getInverseKinematics();
    vsIntersect            *getIntersectionObject();
};

#endif
