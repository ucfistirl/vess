#ifndef VS_WALK_IN_PLACE_HPP
#define VS_WALK_IN_PLACE_HPP

// Motion model for walking action.  The user walks in place to move the
// viewpoint and/or avatar forward.  Takes three motion trackers, one mounted
// on the back to determine heading, and one mounted on each foot or ankle.

#include "vsMotionModel.h++"
#include "vsMotionTracker.h++"

#define VS_WIP_DEFAULT_FWD_THRESH 6.0
#define VS_WIP_DEFAULT_BCK_THRESH 12.0
#define VS_WIP_DEFAULT_SS_THRESH  2.0

#define VS_WIP_DEFAULT_FWD_SPD    1.5
#define VS_WIP_DEFAULT_BCK_SPD    1.5
#define VS_WIP_DEFAULT_SS_SPD     1.5

#define VS_WIP_DEFAULT_ALLOWANCE  2.0

class vsWalkInPlace : public vsMotionModel
{
protected:

    // The motion trackers
    vsMotionTracker    *backTracker;
    vsMotionTracker    *lFootTracker;
    vsMotionTracker    *rFootTracker;

    // Previous heading as read from the back tracker
    double             lastTrackerHeading;

    // Motion restriction flags
    int                forwardAllowed;
    int                backUpAllowed;
    int                sideStepAllowed;

    // Motion speed values
    double             forwardSpeed;
    double             backUpSpeed;
    double             sideStepSpeed;

    // Tracker threshold values
    double             forwardThresh;
    double             backUpThresh;
    double             sideStepThresh;

    // Maximum distance allowed per step
    double             maxAllowance;

    // Remaining distance allowed for this step
    double             moveAllowance;

public: 

                        vsWalkInPlace(vsMotionTracker *back, 
                                      vsMotionTracker *left, 
                                      vsMotionTracker *right);

                        ~vsWalkInPlace();

    void                enableForward(int enabled);
    void                enableBackUp(int enabled);
    void                enableSideStep(int enabled);

    double              getForwardSpeed();
    double              getBackUpSpeed();
    double              getSideStepSpeed();

    void                setForwardSpeed(double unitsPerSec);
    void                setBackUpSpeed(double unitsPerSec);
    void                setSideStepSpeed(double unitsPerSec);

    double              getForwardThreshold();
    double              getBackUpThreshold();
    double              getSideStepThreshold();

    void                setForwardThreshold(double distance);
    void                setBackUpThreshold(double distance);
    void                setSideStepThreshold(double distance);

    virtual vsMatrix    update();
};

#endif
