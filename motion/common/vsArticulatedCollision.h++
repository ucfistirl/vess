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

class VS_MOTION_DLL vsArticulatedCollision : public vsMotionModel
{
private:

    vsInverseKinematics    *invKinematics;

    vsIntersect            *intersect;
    vsNode                 *scene;
    double                 segmentRadius;

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
