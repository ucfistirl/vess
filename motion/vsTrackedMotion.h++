#ifndef VS_TRACKED_MOTION_HPP
#define VS_TRACKED_MOTION_HPP

// Class intended to take motion data from a motion tracker and apply
// the movements directly to the component.  Useful for viewpoint head
// tracking when the head is not part of a tracker hierarchy (i.e.: when
// the back or waist is not being tracked along with the head).  Also useful
// for positioning and orienting tracked objects in the scene.  This motion 
// model is not intended for use with other motion models simultaneously on 
// the same kinematics object.

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
