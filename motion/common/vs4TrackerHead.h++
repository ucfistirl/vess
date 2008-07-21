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
//    VESS Module:  vs4TrackerHead.h++
//
//    Description:  Class to handle viewpoint head tracking.
//                  Suited for use with motion tracking systems which
//                  do not provide orientation of trackers. 
//
//    Author(s):    David Smith
//
//------------------------------------------------------------------------

#ifndef VS_4_TRACKER_HEAD_HPP
#define VS_4_TRACKER_HEAD_HPP

#include "vsMotionModel.h++"
#include "vsMotionTracker.h++"
#include "vsKinematics.h++"

class VESS_SYM vs4TrackerHead : public vsMotionModel
{
protected:

    // Motion trackers
    vsMotionTracker     *headTrackerRear, *headTrackerFront;
    vsMotionTracker     *lShoulderTracker, *rShoulderTracker;
    vsMotionTracker     *chestTracker;

    // Kinematics for head
    vsKinematics        *kinematics;

public:

                          vs4TrackerHead(vsMotionTracker *headRear,
                                         vsMotionTracker *headFront,
                                         vsMotionTracker *lShoulder,
                                         vsMotionTracker *rShoulder,
                                         vsKinematics *kin);
    virtual               ~vs4TrackerHead();

    virtual const char    *getClassName(); 

    // Updates the motion model
    virtual void          update();
};
#endif
