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
//    VESS Module:  vsHiball.h++
//
//    Description:  Class to handle input from a Hiball tracking system.
//
//    Author(s):    Jason Daly, Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_HIBALL_HPP
#define VS_HIBALL_HPP


#include "vsMotionTracker.h++"
#include "vsTrackingSystem.h++"
#include "vsVRPNTrackingSystem.h++"

#include "atList.h++"
#include "atQuat.h++"
#include "atVector.h++"


class vsHiball : public vsVRPNTrackingSystem
{
protected:

    int                     numTrackers;
    vsMotionTracker         *motionTrackers[VS_VRPN_MAX_REMOTE_TRACKERS];

public:

                               vsHiball(atString hostName,
                                   atList * trackerNames,
                                   atList * buttonNames);
    virtual                    ~vsHiball();

    virtual const char         *getClassName();

    virtual int                getNumTrackers();
    virtual vsMotionTracker    *getTracker(int index);

    virtual void               update();
};


#endif

