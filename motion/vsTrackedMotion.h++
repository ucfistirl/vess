#ifndef VS_TRACKED_MOTION_HPP
#define VS_TRACKED_MOTION_HPP

// Class inteded to take motion data directly from motion trackers and
// apply the movements directly to the component.  Useful for viewpoint
// head tracking.  This motion model is not intended for use with other
// motion models simultaneously.

#include "vsMotionModel.h++"
#include "vsMotionTracker.h++"

class vsTrackedMotion : public vsMotionModel
{
protected:

    vsMotionTracker *tracker;
    int             positionEnabled;
    int             orientationEnabled;

    vsVector        lastTrackerPos;
    vsQuat          lastTrackerOrn;

public:

                         vsTrackedMotion(vsMotionTracker *theTracker);

                         ~vsTrackedMotion();

    void                 enablePosition(int enabled);
    void                 enableOrientation(int enabled);

    virtual vsVecQuat    update();
};

#endif
