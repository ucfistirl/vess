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
//    VESS Module:  vs3TrackerArm.h++
//
//    Description:  Motion model that manipulates the three joints of a
//		    human figure's arm. Works with three motion trackers
//		    ideally mounted on the subject's back, upper arm,
//		    and hand.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_3_TRACKER_ARM_HPP
#define VS_3_TRACKER_ARM_HPP

#include "vsMotionModel.h++"
#include "vsKinematics.h++"
#include "vsMotionTracker.h++"

class VS_MOTION_DLL vs3TrackerArm : public vsMotionModel
{
private:

    vsMotionTracker    *backTrack, *elbowTrack, *handTrack;
    vsKinematics       *shoulderKin, *elbowKin, *wristKin;

    vsVector           shoulderOffset, elbowOffset, wristOffset;
    vsQuat	       shoulderPreRot, shoulderPostRot;
    vsQuat	       elbowPreRot, elbowPostRot;
    vsQuat	       wristPreRot, wristPostRot;

public:

                          vs3TrackerArm(vsMotionTracker *backTracker,
                                        vsKinematics *shoulderJoint,
                                        vsMotionTracker *elbowTracker,
                                        vsKinematics *elbowJoint,
                                        vsMotionTracker *handTracker,
                                        vsKinematics *wristJoint);
                          ~vs3TrackerArm();

    virtual const char    *getClassName();

    void                  setShoulderOffset(vsVector newOffset);
    vsVector              getShoulderOffset();
    void                  setElbowOffset(vsVector newOffset);
    vsVector              getElbowOffset();
    void                  setWristOffset(vsVector newOffset);
    vsVector              getWristOffset();
    
    void	          setShoulderPreRot(vsQuat rotQuat);
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
