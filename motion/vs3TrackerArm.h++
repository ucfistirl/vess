// File vs3TrackerArm.h++

#ifndef VS_3_TRACKER_ARM_HPP
#define VS_3_TRACKER_ARM_HPP

#include "vsMotionModel.h++"
#include "vsKinematics.h++"
#include "vsMotionTracker.h++"

class vs3TrackerArm : public vsMotionModel
{
private:

    vsMotionTracker    *backTrack, *elbowTrack, *handTrack;
    vsKinematics       *shoulderKin, *elbowKin, *wristKin;
    vsVector           shoulderOffset, elbowOffset, wristOffset;

public:

                vs3TrackerArm(vsMotionTracker *backTracker,
                              vsKinematics *shoulderJoint,
                              vsMotionTracker *elbowTracker,
                              vsKinematics *elbowJoint,
                              vsMotionTracker *handTracker,
                              vsKinematics *wristJoint);
                ~vs3TrackerArm();

    void        setShoulderOffset(vsVector newOffset);
    vsVector    getShoulderOffset();
    void        setElbowOffset(vsVector newOffset);
    vsVector    getElbowOffset();
    void        setWristOffset(vsVector newOffset);
    vsVector    getWristOffset();

    void        update();
};

#endif
