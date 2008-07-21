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
//    VESS Module:  vsDifferentialTrackedOrientation.h++
//
//    Description:  Class intended to handle viewpoint head tracking.
//                  Works for any application where a "reference" and
//                  and "offset" orientation measurement are needed.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_DIFFERENTIAL_TRACKED_ORIENTATION_HPP
#define VS_DIFFERENTIAL_TRACKED_ORIENTATION_HPP

// Takes two trackers, one to represent a reference orientation (the back or
// waist) and one to handle the differential orientation (head).  No position 
// tracking is provided, as the differential tracker is assumed  to be a part 
// of a hieararchy.  Because of the nature of this motion model (explicit 
// orientation tracking), no linear or angular velocities are computed in 
// the kinematics.
//
// Not recommended for use with other motion models simultaneously on the same 
// kinematics object.

#include "vsMotionModel.h++"
#include "vsMotionTracker.h++"
#include "vsKinematics.h++"

class VESS_SYM vsDifferentialTrackedOrientation : public vsMotionModel
{
protected:

    // Trackers
    vsMotionTracker    *refTracker;
    vsMotionTracker    *diffTracker;

    // Kinematics
    vsKinematics       *kinematics;

    // Offset from the tracked orientation
    atQuat             orientationOffset;
    
    // Calibration values
    atQuat             resetRef;
    atQuat             resetDiff;

public:

    // Constructor/destructor
                          vsDifferentialTrackedOrientation(vsMotionTracker *ref,
                                                       vsMotionTracker *diff,
                                                       vsKinematics *kinObject);
    virtual               ~vsDifferentialTrackedOrientation();

    // Class name
    virtual const char    *getClassName();

    // Accessors for the orientation offset
    void                  setOrientationOffset(atQuat newOffset);
    atQuat                getOrientationOffset();

    // Updates the motion model
    virtual void          update();

    // Sets the calibration values based on the current orientation
    virtual void          reset();
};

#endif
