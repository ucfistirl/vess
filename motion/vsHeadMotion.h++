#ifndef VS_HEAD_MOTION_HPP
#define VS_HEAD_MOTION_HPP

// Class intended to handle viewpoint head tracking.  Takes two trackers, one
// to represent the base orientation (the back or waist) and one to handle the 
// head orientation.  No position tracking is provided, as the head is assumed 
// to be a part of a hieararchy.  Because of the nature of this motion model
// (explicit orientation tracking), no linear or angular velocities are 
// computed.
//
// Not recommended for use with other motion models simultaneously on the same 
// kinematics object.

#include "vsMotionModel.h++"
#include "vsMotionTracker.h++"
#include "vsKinematics.h++"

class vsHeadMotion : public vsMotionModel
{
protected:

    // Trackers
    vsMotionTracker    *backTracker;
    vsMotionTracker    *headTracker;

    // Kinematics
    vsKinematics       *kinematics;

    // Offset from the tracked orientation
    vsQuat             orientationOffset;
    
    // Calibration values
    vsQuat             resetBack;
    vsQuat             resetHead;

public:

    // Constructor/destructor
                    vsHeadMotion(vsMotionTracker *back,
                                 vsMotionTracker *head,
                                 vsKinematics *kinObject);
                    ~vsHeadMotion();

    // Accessors for the orientation offset
    void            setOrientationOffset(vsQuat newOffset);
    vsQuat          getOrientationOffset();

    // Updates the motion model
    virtual void    update();

    // Sets the calibration values based on the current orientation
    virtual void    reset();
};

#endif
