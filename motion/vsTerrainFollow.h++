// File vsTerrainFollow.h++

#ifndef VS_TERRAIN_FOLLOW_HPP
#define VS_TERRAIN_FOLLOW_HPP

#include "vsMotionModel.h++"
#include "vsKinematics.h++"
#include "vsIntersect.h++"

#define VS_TFOLLOW_DEFAULT_HEIGHT 0.5
#define VS_TFOLLOW_FLOAT_HEIGHT   0.001

class vsTerrainFollow : public vsMotionModel
{
private:

    vsKinematics    *kinematics;
    vsNode          *scene;
    
    vsVector        pointOffset;
    double          stepHeight;
    
    vsIntersect     *intersect;

public:

                    vsTerrainFollow(vsKinematics *objectKin, vsNode *theScene);
                    ~vsTerrainFollow();

    void            setBaseOffset(vsVector newOffset);
    vsVector        getBaseOffset();
    
    void            setStepHeight(double newHeight);
    double          getStepHeight();
    
    void            setIntersectMask(unsigned int newMask);
    unsigned int    getIntersectMask();

    virtual void    update();
};

#endif
