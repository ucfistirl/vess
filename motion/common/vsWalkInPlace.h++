//------------------------------------------------------------------------
//
//    VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//    Copyright (c) 2001, University of Central Florida
//
//       See the file LICENSE for license information
//
//    E-mail:  vess@ist.ucf.edu
//    WWW:     http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//    VESS Module:  vsWalkInPlace.h++
//
//    Description:  Motion model for walking action
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_WALK_IN_PLACE_HPP
#define VS_WALK_IN_PLACE_HPP

// The user walks in place to move the viewpoint and/or avatar forward.
// Takes three motion trackers, one mounted on the back to determine heading,
// and one mounted on each foot or ankle.  Intended to be used with a
// vsKinematics object with inertia disabled.

#include "vsMotionModel.h++"
#include "vsMotionTracker.h++"
#include "vsKinematics.h++"

// Threshold values (in tracker units)
#define VS_WIP_DEFAULT_FWD_THRESH  6.0
#define VS_WIP_DEFAULT_BCK_THRESH  12.0
#define VS_WIP_DEFAULT_SS_THRESH   -1.0

// Movement speeds (in database units)
#define VS_WIP_DEFAULT_FWD_SPD     1.95
#define VS_WIP_DEFAULT_BCK_SPD     1.95
#define VS_WIP_DEFAULT_SS_SPD      1.95

// Maximum movement distance (in database units)
#define VS_WIP_DEFAULT_ALLOWANCE   2.0

// Default state of the movementLimited flag (VS_TRUE means movement is 
// limited by the distance in maxAllowance)
#define VS_WIP_DEFAULT_LIMIT_STATE VS_TRUE

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

    // Flag to indicate whether movement allowance is enabled or not
    int                movementLimited;

public: 

    // Constructor
                          vsWalkInPlace(vsMotionTracker *back, 
                                        vsMotionTracker *left, 
                                        vsMotionTracker *right,
                                        vsKinematics *kin);

    // Destructor
                          ~vsWalkInPlace();

    // Inherited from vsObject
    virtual const char    *getClassName();

    // Methods to enable/disable the various forms of motion
    void                  enableForward();
    void                  enableBackward();
    void                  enableSideStep();
    void                  disableForward();
    void                  disableBackward();
    void                  disableSideStep();

    // Speed accessors
    double                getForwardSpeed();
    double                getBackwardSpeed();
    double                getSideStepSpeed();
    void                  setForwardSpeed(double unitsPerSec);
    void                  setBackwardSpeed(double unitsPerSec);
    void                  setSideStepSpeed(double unitsPerSec);

    // Threshold accessors
    double                getForwardThreshold();
    double                getBackwardThreshold();
    double                getSideStepThreshold();
    void                  setForwardThreshold(double distance);
    void                  setBackwardThreshold(double distance);
    void                  setSideStepThreshold(double distance);

    // Movement limit accessors
    double                getMovementAllowance();
    void                  setMovementAllowance(double distance);
    void                  enableMovementLimit();
    void                  disableMovementLimit();

    // Update method
    virtual void          update();
};

#endif
