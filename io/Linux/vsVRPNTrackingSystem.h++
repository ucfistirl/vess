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
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_VRPN_TRACKING_SYSTEM_HPP
#define VS_VRPN_TRACKING_SYSTEM_HPP


#include "vsMotionTracker.h++"
#include "vsTrackingSystem.h++"

#include "atList.h++"
#include "atQuat.h++"
#include "atString.h++"
#include "atVector.h++"

#include <vrpn_Button.h>
#include <vrpn_Tracker.h>


#define VS_VRPN_MAX_REMOTE_TRACKERS    32
#define VS_VRPN_MAX_REMOTE_BUTTONS     8


struct vsVRPNRemoteTracker
{
    atString               trackerName;
    vrpn_Tracker_Remote    *vrpnTracker;

    atVector               trackerPosition;
    atQuat                 trackerOrientation;
};


struct vsVRPNRemoteButton
{
    atString              buttonName;
    vrpn_Button_Remote    *vrpnButton;

    bool                  buttonState;
};


class vsVRPNTrackingSystem : public vsTrackingSystem
{
protected:

    atString               remoteHostname;

    int                    numRemoteTrackers;
    vsVRPNRemoteTracker    *remoteTrackers[VS_VRPN_MAX_REMOTE_TRACKERS];

    int                    numRemoteButtons;
    vsVRPNRemoteButton     *remoteButtons[VS_VRPN_MAX_REMOTE_BUTTONS];

    static void            remoteTrackerChangeHandler(void *userData,
                               const vrpn_TRACKERCB tracker);
    static void            remoteButtonChangeHandler(void *userData,
                               const vrpn_BUTTONCB button);

public:

                          vsVRPNTrackingSystem(atString hostName,
                              atList * trackerNames, atList * buttonNames);
    virtual               ~vsVRPNTrackingSystem();

    virtual const char    *getClassName() = 0;

    virtual void          update();
};


#endif

