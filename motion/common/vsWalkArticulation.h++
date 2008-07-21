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
//    VESS Module:  vsWalkArticulation.h++
//
//    Description:  Motion model that takes the velocity of an object,
//                  and attemps to make human-like walking movements on
//                  the joints on that object when it is moving.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_WALK_ARTICULATION_HPP
#define VS_WALK_ARTICULATION_HPP

#include "vsMotionModel.h++"
#include "vsKinematics.h++"
#include "vsGrowableArray.h++"

#define VS_WALK_ARTIC_JOINT_COUNT 6

enum  vsWalkArticJoints
{
    VS_WALK_ARTIC_LEFT_HIP,
    VS_WALK_ARTIC_LEFT_KNEE,
    VS_WALK_ARTIC_LEFT_ANKLE,
    VS_WALK_ARTIC_RIGHT_HIP,
    VS_WALK_ARTIC_RIGHT_KNEE,
    VS_WALK_ARTIC_RIGHT_ANKLE
};

struct VESS_SYM vsWalkArticData
{
    atQuat leftHip, leftKnee, leftAnkle;
    atQuat rightHip, rightKnee, rightAnkle;
    double distance;
};

enum  vsWalkArticState
{
    VS_WALK_ARTIC_STOPPED,
    VS_WALK_ARTIC_MOVING,
    VS_WALK_ARTIC_STOPPING
};

class VESS_SYM vsWalkArticulation : public vsMotionModel
{
private:

    vsKinematics       *rootKin;
    
    vsKinematics       *leftHipKin, *leftKneeKin, *leftAnkleKin;
    vsKinematics       *rightHipKin, *rightKneeKin, *rightAnkleKin;
    
    vsGrowableArray    keyframeData;
    int                keyframeCount;
    vsWalkArticData    stopKeyframe;
    
    vsWalkArticData    *fromKeyframe, *toKeyframe;
    int                keyframeIndex;
    
    double             travelDist;
    double             waitTime;
    
    int                moveState;

    void               getLine(FILE *in, char *buffer);
    void               captureStopFrame();

public:

                          vsWalkArticulation(vsKinematics *objectKin,
                                             char *walkDataFilename);
    virtual               ~vsWalkArticulation();

    virtual const char    *getClassName();

    void                  setJointKinematics(int whichJoint,
                                             vsKinematics *kinematics);
    vsKinematics          *getJointKinematics(int whichJoint);
    
    void                  update();
};

#endif
