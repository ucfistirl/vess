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
//    VESS Module:  vs8TrackerArm.h++
//
//    Description:  Motion model that manipulates the three joints of a
//		    human figure's arm. Works with three motion trackers
//		    ideally mounted on the subject's back, upper arm,
//		    and hand.
//
//    Author(s):    Bryan Kline, David Smith
//
//------------------------------------------------------------------------

#ifndef VS_8_TRACKER_ARM_HPP
#define VS_8_TRACKER_ARM_HPP

#include "vsMotionModel.h++"
#include "vsKinematics.h++"
#include "vsMotionTracker.h++"

class VESS_SYM vs8TrackerArm : public vsMotionModel
{
private:

    vsMotionTracker    *lShoulderTracker, *rShoulderTracker;
    vsMotionTracker    *upArm1Tracker, *upArm2Tracker;
    vsMotionTracker    *forearm1Tracker, *forearm2Tracker;
    vsMotionTracker    *handTracker1, *handTracker2;
    vsKinematics       *shoulderKin, *elbowKin, *wristKin;

    atVector           shoulderOffset, elbowOffset, wristOffset;
    atQuat	       shoulderPreRot, shoulderPostRot;
    atQuat	       elbowPreRot, elbowPostRot;
    atQuat	       wristPreRot, wristPostRot;

public:

                          vs8TrackerArm(vsMotionTracker *lShoulderTracker,
                                        vsMotionTracker *rShoulderTracker,
                                        vsMotionTracker *upperArm1Tracker,
                                        vsMotionTracker *upperArm2Tracker,
                                        vsKinematics *shoulderJoint,
                                        vsMotionTracker *foreArm1Tracker,
                                        vsMotionTracker *foreArm2Tracker,
                                        vsKinematics *elbowJoint,
                                        vsMotionTracker *handTracker1,
                                        vsMotionTracker *handTracker2,
                                        vsKinematics *wristJoint);
    virtual               ~vs8TrackerArm();

    virtual const char    *getClassName();

    void                  setShoulderOffset(atVector newOffset);
    atVector              getShoulderOffset();
    void                  setElbowOffset(atVector newOffset);
    atVector              getElbowOffset();
    void                  setWristOffset(atVector newOffset);
    atVector              getWristOffset();
    
    void	              setShoulderPreRot(atQuat rotQuat);
    atQuat                getShoulderPreRot();
    void                  setShoulderPostRot(atQuat rotQuat);
    atQuat                getShoulderPostRot();
    void                  setElbowPreRot(atQuat rotQuat);
    atQuat                getElbowPreRot();
    void                  setElbowPostRot(atQuat rotQuat);
    atQuat                getElbowPostRot();
    void                  setWristPreRot(atQuat rotQuat);
    atQuat                getWristPreRot();
    void                  setWristPostRot(atQuat rotQuat);
    atQuat                getWristPostRot();

    void                  update();
};

#endif
