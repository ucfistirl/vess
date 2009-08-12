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
//    VESS Module:  vsVRPNTrackingSystem.h++
//
//    Description:  Class to manage communication with tracking systems
//                  hosted over a VRPN network.
//
//    Author(s):    Casey Thurston, Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_VRPN_TRACKING_SYSTEM_HPP
#define VS_VRPN_TRACKING_SYSTEM_HPP


#include "vsMotionTracker.h++"
#include "vsTrackingSystem.h++"

#include "atArray.h++"
#include "atQuat.h++"
#include "atString.h++"
#include "atVector.h++"

#include <vrpn_Button.h>
#include <vrpn_Tracker.h>


class VESS_SYM vsVRPNTrackingSystem : public vsTrackingSystem
{
protected:

    atString               remoteHostname;

    atString               localHostname;
    vrpn_Connection        *remoteConnection;

    vrpn_Tracker_Remote    *remoteTrackerConnection;
    atArray                *motionTrackers;

    vrpn_Button_Remote     *remoteButtonConnection;
    atArray                *trackerButtons;

    void                   init(atString hostName, atString trackerServerName,
                                atString buttonServerName);

    static void            remoteTrackerChangeHandler(
                                                 void *userData,
                                                 const vrpn_TRACKERCB tracker);

    static void            remoteButtonChangeHandler(
                                                 void *userData,
                                                 const vrpn_BUTTONCB button);

public:

                              vsVRPNTrackingSystem(atString serverHostName,
                                                   atString trackerServerName,
                                                   atString buttonServerName);
                              vsVRPNTrackingSystem(atString serverHostName,
                                                   atString localHostName, 
                                                   atString trackerServerName,
                                                   atString buttonServerName);
    virtual                   ~vsVRPNTrackingSystem();

    virtual const char        *getClassName();

    virtual int               getNumTrackers();
    virtual vsMotionTracker   *getTracker(int index);

    virtual int               getNumButtons();
    virtual vsInputButton     *getButton(int index);

    virtual void              update();
};


#endif

