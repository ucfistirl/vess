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

class VS_MOTION_DLL vs8TrackerArm : public vsMotionModel
{
private:

    vsMotionTracker    *lShoulderTracker, *rShoulderTracker;
    vsMotionTracker    *upArm1Tracker, *upArm2Tracker;
    vsMotionTracker    *forearm1Tracker, *forearm2Tracker;
    vsMotionTracker    *handTracker1, *handTracker2;
    vsKinematics       *shoulderKin, *elbowKin, *wristKin;

    vsVector           shoulderOffset, elbowOffset, wristOffset;
    vsQuat	       shoulderPreRot, shoulderPostRot;
    vsQuat	       elbowPreRot, elbowPostRot;
    vsQuat	       wristPreRot, wristPostRot;

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

    void                  setShoulderOffset(vsVector newOffset);
    vsVector              getShoulderOffset();
    void                  setElbowOffset(vsVector newOffset);
    vsVector              getElbowOffset();
    void                  setWristOffset(vsVector newOffset);
    vsVector              getWristOffset();
    
    void	              setShoulderPreRot(vsQuat rotQuat);
    vsQuat                getShoulderPreRot();
    void                  setShoulderPostRot(vsQuat rotQuat);
    vsQuat                getShoulderPostRot();
    void                  setElbowPreRot(vsQuat rotQuat);
    vsQuat                getElbowPreRot();
    void                  setElbowPostRot(vsQuat rotQuat);
    vsQuat                getElbowPostRot();
    void                  setWristPreRot(vsQuat rotQuat);
    vsQuat                getWristPreRot();
    void                  setWristPostRot(vsQuat rotQuat);
    vsQuat                getWristPostRot();

    void                  update();
};

#endif
