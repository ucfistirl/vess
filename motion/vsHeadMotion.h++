#ifndef VS_HEAD_MOTION_HPP
#define VS_HEAD_MOTION_HPP

// Class intended to handle viewpoint head tracking.  Takes two trackers, one
// to represent the base orientation (the back or waist) and one to handle the 
// head orientation.  No position tracking is provided, as the head is assumed 
// to be a part of a hieararchy.  Not recommended for use with other motion 
// models simultaneously on the same kinematics object.

#include "vsMotionModel.h++"
#include "vsMotionTracker.h++"
#include "vsKinematics.h++"

class vsHeadMotion : public vsMotionModel
{
protected:

    vsMotionTracker    *backTracker;
    vsMotionTracker    *headTracker;
    vsKinematics       *kinematics;

    int                positionEnabled;
    int                orientationEnabled;
    
    vsQuat             orientationOffset;
    
    vsQuat             resetBack;
    vsQuat             resetHead;

public:

                    vsHeadMotion(vsMotionTracker *back,
                                 vsMotionTracker *head,
                                 vsKinematics *kinObject);
                    ~vsHeadMotion();

    void            setOrientationOffset(vsQuat newOffset);
    vsQuat          getOrientationOffset();

    virtual void    update();
    virtual void    reset();
};

#endif
