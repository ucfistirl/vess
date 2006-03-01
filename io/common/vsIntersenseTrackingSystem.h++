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
//    VESS Module:  vsIntersenseTrackingSystem.h++
//
//    Description:  Generic driver for all Intersense tracking systems.
//                  Uses the API provided by Intersense to communicate
//                  with hardware.  No threading is carried out in this
//                  class because the InterSense library handles all the
//                  buffering for us.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#ifndef VS_INTERSENSE_TRACKING_SYSTEM_HPP
#define VS_INTERSENSE_TRACKING_SYSTEM_HPP

#include "vsTrackingSystem.h++"
#include "vsJoystick.h++"

// Undefine the symbol Bool (this may be defined as int by X Windows) since 
// the Intersense library will redefine it
#if defined(Bool)
    #undef Bool
#endif
#include "isense.h"

#define VS_ITS_MAX_TRACKERS ISD_MAX_STATIONS

class vsIntersenseTrackingSystem : public vsTrackingSystem
{
private:

    // Note:  A "Tracker" in Intersense-speak is a "System" in VESS, while
    // a "Station" in Intersense is a "Tracker" in VESS.  Since this is a
    // VESS class, the variables defined here use the VESS conventions.
    int                      port;
    ISD_TRACKER_HANDLE       systemHandle;
    ISD_TRACKER_INFO_TYPE    systemConfig;
    ISD_STATION_INFO_TYPE    trackerConfig[VS_ITS_MAX_TRACKERS];

    ISD_HARDWARE_INFO_TYPE            systemInfo;
    ISD_STATION_HARDWARE_INFO_TYPE    trackerInfo[VS_ITS_MAX_TRACKERS];

    vsMotionTracker          *tracker[VS_ITS_MAX_TRACKERS];
    vsJoystick               *joystick[VS_ITS_MAX_TRACKERS];
    int                      numTrackers;

    // Mapping from tracker number to station number and vice versa
    int                      trackerToStation[VS_ITS_MAX_TRACKERS];
    int                      stationToTracker[VS_ITS_MAX_TRACKERS + 1];

    bool                     valid;

    vsQuat                   coordXform;

    bool                     configureSystem();
    void                     configureJoystick(int trackerNum);
    void                     enumerateTrackers();

public:

                               vsIntersenseTrackingSystem(int portNumber);
                               ~vsIntersenseTrackingSystem();

    virtual const char         *getClassName();

    virtual int                getNumTrackers();
    virtual vsMotionTracker    *getTracker(int index);

    bool                       hasJoystick(int index);
    vsJoystick                 *getJoystick(int index);

    void                       setAngleAlignment(int tracker, float h, float p,
                                                 float r);
    void                       setAngleAlignment(int tracker);
    void                       clearAngleAlignment(int tracker);

    // IS-900 only
    void                       enableLEDs();
    void                       disableLEDs();

    virtual void               update();
};

#endif
