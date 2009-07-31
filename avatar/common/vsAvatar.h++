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

#include "vsUpdatable.h++"
#include "vsArray.h++"
#include "vsNode.h++"
#include "vsOptimizer.h++"
#include "atString.h++"

#define VS_AVATAR_LOCAL_ISECT_MASK 0x01000000

enum  ConfigLineType
{
    VS_AVT_LINE_END    = -1,
    VS_AVT_LINE_PARAM  = 0,
    VS_AVT_LINE_OBJECT = 1
};

class VESS_SYM vsAvatar : public vsUpdatable
{
protected:

    FILE               *cfgFile;
    vsNode             *masterScene;
    vsArray            *objectArray;
    atArray            *objNameArray;
    atArray            *objTypeArray;
    int                objectCount;
    void               addObjectToArrays(vsObject *object, atString *name,
                                         atString *type);

    bool               isInitted;
    
    vsComponent        *geometryRoot;

    int                readCfgLine(char *buffer);
    
    vsObject           *findObject(char *targetStr);
    
    virtual vsObject   *createObject(char *idString);

    virtual void       setup() = 0;

    // The various functions called by createObject to make objects of
    // the associated type

    // special objects
    vsObject           *makeGeometry();
    vsObject           *makeViewpoint();
    vsObject           *makeIODevice();
    vsObject           *makeVsSequencer();

    // input objects
    vsObject           *makeVsISTJoystickBox();
    vsObject           *makeVsUnwinder();
    vsObject           *makeVsFlockOfBirds();
    vsObject           *makeVsSerialMotionStar();
    vsObject           *makeVsFastrak();
    vsObject           *makeVsIS600();
    vsObject           *makeVsEthernetMotionStar();
    vsObject           *makeVsPolaris();
    vsObject           *makeVsWSSpaceball();
    vsObject           *makeVsPinchGloveBox();
    vsObject           *makeVsCyberGloveBox();

#ifdef __linux__
    vsObject           *makeVsLinuxJoystickSystem();
#endif

    // input adapters
    vsObject           *makeVsButtonAxis();

    // motion model objects
    vsObject           *makeVsKinematics();
    vsObject           *makeVs3TrackerArm();
    vsObject           *makeVsAxisRotation();
    vsObject           *makeVsCollision();
    vsObject           *makeVsDrivingMotion();
    vsObject           *makeVsFlyingMotion();
    vsObject           *makeVsDifferentialTrackedOrientation();
    vsObject           *makeVsPathMotion();
    vsObject           *makeVsTerrainFollow();
    vsObject           *makeVsTrackballMotion();
    vsObject           *makeVsTrackedMotion();
    vsObject           *makeVsWalkArticulation();
    vsObject           *makeVsWalkInPlace();
    vsObject           *makeVsFPSMotion();

    // haptics objects
    vsObject           *makeVsVestSystem();

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
