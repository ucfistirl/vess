#ifndef VS_TRACKED_MOTION_HPP
#define VS_TRACKED_MOTION_HPP

// Class intended to take motion data from a motion tracker and apply
// the movements directly to the component.  Useful for viewpoint head
// tracking when the head is not part of a tracker hierarchy (i.e.: when
// the back or waist is not being tracked along with the head).  Also useful
// for positioning and orienting tracked objects in the scene.  
//
// Because of the nature of this motion model (explicit position/orientation
// tracking), no linear or angular velocities are computed.  This motion model 
// is not intended for use with other motion models simultaneously on the same 
// kinematics object.  

#include "vsMotionModel.h++"
#include "vsMotionTracker.h++"
#include "vsKinematics.h++"

class vsTrackedMotion : public vsMotionModel
{
protected:

    // The tracker
    vsMotionTracker    *tracker;

    // Kinematics
    vsKinematics       *kinematics;

    // Flags to indicate if position or orientation tracking is enabled
    int                positionEnabled;
    int                orientationEnabled;
    
    // User-specified offsets from the actual tracker position
    vsVector           positionOffset;
    vsQuat             orientationOffset;
    
    // Calibration offsets from the actual tracker position (set with the
    // reset method)
    vsVector           resetPosition;
    vsQuat             resetOrientation;

public:

    // Constructor
                    vsTrackedMotion(vsMotionTracker *theTracker,
                                    vsKinematics *kinObject);

    // Destructor
                    ~vsTrackedMotion();

    // Methods to enable/disable position tracking
    void            enablePositionTracking();
    void            disablePositionTracking();
    void            enableOrientationTracking();
    void            disableOrientationTracking();
    
    // Accessors for the user-specified offsets
    void            setPositionOffset(vsVector newOffset);
    vsVector        getPositionOffset();
    void            setOrientationOffset(vsQuat newOffset);
    vsQuat          getOrientationOffset();

    // Update function
    virtual void    update();

    // Reset (calibration) funtion
    virtual void    reset();
};

#endif
