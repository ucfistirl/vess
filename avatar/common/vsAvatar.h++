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
//    VESS Module:  vsAvatar.h++
//
//    Description:  Virtual base class for all avatar objects
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_AVATAR_HPP
#define VS_AVATAR_HPP

#include <stdio.h>
#include "vsUpdatable.h++"
//#include "vsSystem.h++"
#include "vsGrowableArray.h++"
#include "vsNode.h++"
#include "vsOptimizer.h++"

#define VS_AVATAR_LOCAL_ISECT_MASK 0x01000000

enum ConfigLineType
{
    VS_AVT_LINE_END    = -1,
    VS_AVT_LINE_PARAM  = 0,
    VS_AVT_LINE_OBJECT = 1
};

class vsAvatar : public vsUpdatable
{
protected:

    FILE               *cfgFile;
    vsNode             *masterScene;
    vsGrowableArray    *objectArray;
    vsGrowableArray    *objNameArray;
    vsGrowableArray    *objTypeArray;
    int                objectCount;
    void               addObjectToArrays(void *object, char *name, char *type);

    int                isInitted;
    
    vsComponent        *geometryRoot;

    int                readCfgLine(char *buffer);
    
    void               *findObject(char *targetStr);
    
    virtual void       *createObject(char *idString);

    virtual void       setup() = 0;

    // The various functions called by createObject to make objects of
    // the associated type
    void               *makeGeometry();
    void               *makeViewpoint();
    void               *makeInputDevice();

    // input objects
    void               *makeVsISTJoystickBox();
    void               *makeVsUnwinder();
    void               *makeVsFlockOfBirds();
    void               *makeVsSerialMotionStar();
    void               *makeVsFastrak();
    void               *makeVsIS600();
    void               *makeVsEthernetMotionStar();
    
    void               *makeVsWSSpaceball();
    void               *makeVsPinchGloveBox();
    void               *makeVsCyberGloveBox();

#ifdef __linux__
    void               *makeVsLinuxJoystickSystem();
#endif

    // motion model objects
    void               *makeVsKinematics();
    void               *makeVs3TrackerArm();
    void               *makeVsAxisRotation();
    void               *makeVsCollision();
    void               *makeVsDrivingMotion();
    void               *makeVsFlyingMotion();
    void               *makeVsDifferentialTrackedOrientation();
    void               *makeVsTerrainFollow();
    void               *makeVsTrackballMotion();
    void               *makeVsTrackedMotion();
    void               *makeVsWalkArticulation();
    void               *makeVsWalkInPlace();

public:

                          vsAvatar();
                          vsAvatar(vsNode *scene);
    virtual               ~vsAvatar();

    virtual const char    *getClassName();

    virtual void          init(char *configFile);
    
    virtual void          update() = 0;
    
    vsNode                *getGeometry();
};

#endif