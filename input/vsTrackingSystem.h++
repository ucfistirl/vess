#ifndef VS_TRACKING_SYSTEM_HPP
#define VS_TRACKING_SYSTEM_HPP

// Abstract base class for all motion tracking systems

#include "vsMotionTracker.h++"
#include "vsInputSystem.h++"

class vsTrackingSystem : public vsInputSystem
{
public:

                            vsTrackingSystem();
    virtual                 ~vsTrackingSystem();

    virtual int             getNumTrackers(void) = 0;
    virtual vsMotionTracker *getTracker(int index) = 0;
};

#endif
