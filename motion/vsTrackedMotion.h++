#ifndef VS_TRACKED_MOTION_HPP
#define VS_TRACKED_MOTION_HPP

// Class intended to take motion data from a motion tracker and apply
// the movements directly to the component.  Useful for viewpoint head
// tracking.  This motion model is not intended for use with other motion
// models simultaneously on the same kinematics object.

#include "vsMotionModel.h++"
#include "vsMotionTracker.h++"
#include "vsKinematics.h++"

class vsTrackedMotion : public vsMotionModel
{
protected:

    vsMotionTracker    *tracker;
    vsKinematics       *kinematics;

    int                positionEnabled;
    int                orientationEnabled;
    
    vsVector           positionOffset;
    vsQuat             orientationOffset;
    
    vsVector           resetPosition;
    vsQuat             resetOrientation;

public:

                    vsTrackedMotion(vsMotionTracker *theTracker,
                                    vsKinematics *kinObject);
                    ~vsTrackedMotion();

    void            enablePositionTracking();
    void            disablePositionTracking();
    void            enableOrientationTracking();
    void            disableOrientationTracking();
    
    void            setPositionOffset(vsVector newOffset);
    vsVector        getPositionOffset();
    void            setOrientationOffset(vsQuat newOffset);
    vsQuat          getOrientationOffset();

    virtual void    update();
    virtual void    reset();
};

#endif
