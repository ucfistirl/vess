// File ARIAvatar.h++

#ifndef ARI_AVATAR_HPP
#define ARI_AVATAR_HPP

#include "vsAvatar.h++"

#include "vsEthernetMotionStar.h++"
#include "vsUnwinder.h++"
#include "vsHeadMotion.h++"
#include "vsWalkInPlace.h++"
#include "vs3TrackerArm.h++"
#include "vsTerrainFollow.h++"
#include "vsCollision.h++"

struct armData
{
    vsVector shoulderOffset;
    vsVector elbowOffset;
    vsVector wristOffset;
};

class ARIAvatar : public vsAvatar
{
private:

    vsEthernetMotionStar    *mstar;
    vsUnwinder              *unwinder;

    vsKinematics            *root, *head, *r_shoulder, *r_elbow, *r_wrist;
    vsComponent		    *scene;

    vsHeadMotion            *headMotion;
    vsWalkInPlace           *walkMotion;
    vs3TrackerArm           *armMotion;
    
    vsTerrainFollow	    *tFollow;
    vsCollision		    *collide;
    
    vsView		    *leftEyeView, *rightEyeView;

    void                    *makeArmData();

    virtual void            *createObject(char *idString);

    virtual void            setup(vsGrowableArray *objArray,
                                  vsGrowableArray *strArray, int objCount);

public:

                    ARIAvatar(vsComponent *theScene);
    virtual         ~ARIAvatar();
    
    virtual void    update();
    
    vsView          *getLeftEyeView();
    vsView          *getRightEyeView();
    
    vsKinematics    *getRootKin();
    int		    getButtonPress();
};

#endif
