#ifndef VS_WALK_IN_PLACE_HPP
#define VS_WALK_IN_PLACE_HPP

// Motion model for walking action.  The user walks in place to move the
// viewpoint and/or avatar forward.  Takes three motion trackers, one mounted
// on the back to determine heading, and one mounted on each foot or ankle.

#include "vsMotionModel.h++"
#include "vsMotionTracker.h++"
#include "vsKinematics.h++"

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

    // Kinematics object
    vsKinematics       *kinematics;

    // The motion trackers
    vsMotionTracker    *backTracker;
    vsMotionTracker    *lFootTracker;
    vsMotionTracker    *rFootTracker;

    // Previous heading as read from the back tracker
    double             lastTrackerHeading;

    // Motion restriction flags
    int                forwardAllowed;
    int                backwardAllowed;
    int                sideStepAllowed;

    // Motion speed values
    double             forwardSpeed;
    double             backwardSpeed;
    double             sideStepSpeed;

    // Tracker threshold values
    double             forwardThresh;
    double             backwardThresh;
    double             sideStepThresh;

    // Maximum distance allowed per step
    double             maxAllowance;

    // Remaining distance allowed for this step
    double             moveAllowance;

public: 

                    vsWalkInPlace(vsMotionTracker *back, 
                                  vsMotionTracker *left, 
                                  vsMotionTracker *right,
                                  vsKinematics *kin);

                    ~vsWalkInPlace();

    void            enableForward();
    void            enableBackward();
    void            enableSideStep();
    void            disableForward();
    void            disableBackward();
    void            disableSideStep();

    double          getForwardSpeed();
    double          getBackwardSpeed();
    double          getSideStepSpeed();

    void            setForwardSpeed(double unitsPerSec);
    void            setBackwardSpeed(double unitsPerSec);
    void            setSideStepSpeed(double unitsPerSec);

    double          getForwardThreshold();
    double          getBackwardThreshold();
    double          getSideStepThreshold();

    void            setForwardThreshold(double distance);
    void            setBackwardThreshold(double distance);
    void            setSideStepThreshold(double distance);

    virtual void    update();
};

#endif
