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
//    VESS Module:  vs1394Camera.h++
//
//    Description:  Input system for retrieving video data from an IIDC
//                  compliant camera attached to the computer via a 1394
//                  connection
//
//    Author(s):    Bryan Kline, Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_1394_CAMERA_HPP
#define VS_1394_CAMERA_HPP

#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h>
#include "vsIOSystem.h++"
#include "vsTimer.h++"
#include "vsVideoQueue.h++"
#include <pthread.h>

#define VS_1394_CAMERA_TARGET_BRIGHTNESS 252

enum vs1394CameraFrameSize
{
    VS_1394CAM_SIZE_640X480,
    VS_1394CAM_SIZE_800X600,
    VS_1394CAM_SIZE_1024X768,
    VS_1394CAM_SIZE_1280X960,
    VS_1394CAM_SIZE_1600X1200
};

enum vs1394CameraFrameRate
{
    VS_1394CAM_RATE_1_875,
    VS_1394CAM_RATE_3_75,
    VS_1394CAM_RATE_7_5,
    VS_1394CAM_RATE_15,
    VS_1394CAM_RATE_30,
    VS_1394CAM_RATE_60,
};

enum vs1394CameraParameters
{
    VS_1394CAM_PARAM_BRIGHTNESS,
    VS_1394CAM_PARAM_EXPOSURE,
    VS_1394CAM_PARAM_SHARPNESS,
    VS_1394CAM_PARAM_BLUE_BALANCE,
    VS_1394CAM_PARAM_RED_BALANCE,
    VS_1394CAM_PARAM_HUE,
    VS_1394CAM_PARAM_SATURATION,
    VS_1394CAM_PARAM_GAMMA,
    VS_1394CAM_PARAM_SHUTTER,
    VS_1394CAM_PARAM_GAIN,
    VS_1394CAM_PARAM_IRIS,
    VS_1394CAM_PARAM_FOCUS,
    VS_1394CAM_PARAM_TEMPERATURE,
    VS_1394CAM_PARAM_TRIGGER,
    VS_1394CAM_PARAM_ZOOM,
    VS_1394CAM_PARAM_PAN,
    VS_1394CAM_PARAM_TILT,
    VS_1394CAM_PARAM_OPTICAL_FILTER,
    VS_1394CAM_PARAM_CAPTURE_SIZE,
    VS_1394CAM_PARAM_CAPTURE_QUALITY
};

class VS_IO_DLL vs1394Camera : public vsIOSystem
{
private:

    bool                    validCamera;
    raw1394handle_t         busHandle;
    nodeid_t                cameraNodeID;

    char                    videoDeviceName[80];
    int                     frameSize;
    int                     frameRate;

    bool                    activeStream;
    dc1394_cameracapture    cameraInfo;
    bool                    hasFrame;

    bool                    calibrationEnabled;

    vsVideoQueue            *videoQueue;
    int                     videoReferenceID;
    unsigned char           *currentFrameData;

    pthread_t               captureThread;
    pthread_mutex_t         cameraMutex;
    pthread_mutex_t         signalMutex;
    bool                    ceaseCapture;

    void                    connectToCamera(int busIndex, int cameraIndex);
    void                    disconnectFromCamera();

    int                     getFormatConst(int fSize);
    int                     getModeConst(int fSize);
    int                     getFramerateConst(int fRate);

    unsigned int            getFormatMask(int fSize);
    unsigned int            getModeMask(int fSize);
    unsigned int            getFramerateMask(int fRate);

    int                     getParameterConst(int param);

    void                    calibrateColor();
    void                    calibrateBrightness();

    static void             *captureLoop(void *userData);

public:

                     vs1394Camera();
                     vs1394Camera(int busIndex, int cameraIndex);
    virtual          ~vs1394Camera();

    virtual const char    *getClassName();

    virtual void     update();

    void             selectCamera(int busIndex, int cameraIndex);

    bool             isValidFrameSize(int size);
    void             setFrameSize(int size);
    int              getFrameSize();
    int              getFrameWidth();
    int              getFrameHeight();

    bool             isValidFrameRate(int rate);
    void             setFrameRate(int rate);
    int              getFrameRate();

    void             setDeviceName(char *deviceName);
    const char       *getDeviceName();

    void             startStream();
    void             stopStream();
    bool             isStreamGoing();

    vsVideoQueue     *getVideoQueue();

    const char       *getCurrentFramePtr();

    void             enableWhiteBalance();
    void             disableWhiteBalance();

    void             setParameterValue(int param, unsigned int value);
    unsigned int     getParameterValue(int param);
    unsigned int     getParameterMinValue(int param);
    unsigned int     getParameterMaxValue(int param);
};

#endif
