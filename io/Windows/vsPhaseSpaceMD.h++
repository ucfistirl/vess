//------------------------------------------------------------------------
//
//    VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//    Copyright (c) 2005, University of Central Florida
//
//       See the file LICENSE for license information
//
//    E-mail:  vess@ist.ucf.edu
//    WWW:     http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//    VESS Module:  vsPhaseSpaceMD.h++
//
//    Description:  VESS I/O driver for the PhaseSpace Motion Digitizer,
//                  an active LED-based optical tracking system.  Since 
//                  the client-server communications are kept confidential
//                  by PhaseSpace, this implementation relies on the 
//                  PhaseSpace OWL API (libowl.so)
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#ifndef VS_PHASE_SPACE_MD_HPP
#define VS_PHASE_SPACE_MD_HPP

#include "vsTrackingSystem.h++"
#include "owl.h"

#define VS_PSMD_MAX_TRACKERS        64
#define VS_PSMD_MAX_CAMERAS         64
#define VS_PSMD_DEFAULT_REPORT_RATE 60.0f
#define VS_PSMD_MODE_NONE           0
#define VS_PSMD_MODE_POINT          1
#define VS_PSMD_MODE_RIGID          2

enum vsPSMDTrackerType
{
    VS_PSMD_INVALID_TRACKER,
    VS_PSMD_POINT_TRACKER,
    VS_PSMD_RIGID_BODY_TRACKER
};

struct vsPSMDCamera
{
    int id;
    atVector position;
    atQuat orientation;
};


class vsPhaseSpaceMD : public vsTrackingSystem
{
protected:
    vsMotionTracker        *trackers[VS_PSMD_MAX_TRACKERS];
    vsPSMDTrackerType      trackerType[VS_PSMD_MAX_TRACKERS];
    float                  confidence[VS_PSMD_MAX_TRACKERS];         

    int                    numTrackers;
    int                    numMarkers;
    int                    mode; 

    bool                   master;

    float                  reportRate;
    bool                   streaming;

    vsPSMDCamera           cameras[VS_PSMD_MAX_CAMERAS];
    int                    numCameras;

    vsMotionTracker        *privateTrackers[VS_PSMD_MAX_TRACKERS];
    float                  privateConfidence[VS_PSMD_MAX_TRACKERS];         
    bool                   threaded;
    HANDLE                 threadHandle;
    DWORD                  threadID;
    CRITICAL_SECTION       criticalSection;
    bool                   quitFlag;

    void                   updateSystem();

    static DWORD WINAPI    *threadLoop(void *objectPtr);

public:

                               vsPhaseSpaceMD(const char *serverName, 
                                              bool master, bool postprocess,
                                              int mode);
    virtual                    ~vsPhaseSpaceMD();

    virtual const char         *getClassName(); 
    virtual int                getNumTrackers();
    virtual vsMotionTracker    *getTracker(int index);
    vsPSMDTrackerType          getTrackerType(int index);

    void                       setTrackerMode(int mode);
    int                        getTrackerMode();

    void                       setScale(float newScale);
    void                       setReferenceFrame(atVector position, 
                                                 atQuat orientation);

    void                       enableButtonData();
    void                       disableButtonData();

    void                       enableMarkerData();
    void                       disableMarkerData();

    void                       setInterpolationInterval(int numFrames);

    void                       setReportRate(float newRate);

    int                        getNumCameras();
    vsPSMDCamera               *getCamera(int index);

    void                       createPointTracker(int ledIndex);
    void                       createRigidTracker(int ledCount, int *ledIndices,
                                                  atVector *ledOffsets);

    void                       enableTracker(int trackerIndex);
    void                       disableTracker(int trackerIndex);

    float                      getTrackerConfidence(int index);

    void                       startStream();
    void                       stopStream();

    void                       forkTracking();

    void                       update();
    
    const char                 *getErrorString();
};

#endif
