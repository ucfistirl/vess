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
//    VESS Module:  vsTerrainFollow.h++
//
//    Description:  Motion model for forcing an object to stay in contact
//		    with the ground
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_TERRAIN_FOLLOW_HPP
#define VS_TERRAIN_FOLLOW_HPP

#include "vsMotionModel.h++"
#include "vsKinematics.h++"
#include "vsIntersect.h++"

#define VS_TFOLLOW_DEFAULT_HEIGHT 0.5
#define VS_TFOLLOW_FLOAT_HEIGHT   0.001

class VESS_SYM vsTerrainFollow : public vsMotionModel
{
private:

    vsKinematics    *kinematics;
    vsNode          *scene;
    
    atVector        pointOffset;
    double          stepHeight;
    
    vsIntersect     *intersect;

public:

                          vsTerrainFollow(vsKinematics *objectKin, vsNode *theScene);
    virtual               ~vsTerrainFollow();

    virtual const char    *getClassName();

    void                  setBaseOffset(atVector newOffset);
    atVector              getBaseOffset();
    
    void                  setStepHeight(double newHeight);
    double                getStepHeight();
    
    void                  setIntersectMask(unsigned int newMask);
    unsigned int          getIntersectMask();

    virtual void          update();
};

#endif
