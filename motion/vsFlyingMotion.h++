#ifndef VS_FLYING_MOTION_HPP
#define VS_FLYING_MOTION_HPP

// Motion model for simple flying action (not true aerodynamic flying).  Takes 
// either three axes (heading, pitch, and throttle), or two axes (heading 
// and pitch) and up to three buttons (accelerate, decelerate, and stop)
//
// Note that the motion model expects the vsInputAxes it's given to be
// normalized.  A warning message will be printed if they are not, and 
// performace will not likely be what is expected.

#include "vsMotionModel.h++"
#include "vsMouse.h++"
#include "vsInputAxis.h++"
#include "vsInputButton.h++"

enum vsFlyingAxis
{
    VS_FM_AXIS_HEADING  = 0,
    VS_FM_AXIS_PITCH    = 1,
    VS_FM_AXIS_THROTTLE = 2
};

enum vsFlyingAxisMode
{
    VS_FM_MODE_INCREMENTAL = 0,
    VS_FM_MODE_ABSOLUTE    = 1,
    VS_FM_MODE_NO_CHANGE   = -1
};

#define VS_FM_DEFAULT_ACCEL_RATE   20.0
#define VS_FM_DEFAULT_TURNING_RATE 50.0
#define VS_FM_DEFAULT_MAX_VELOCITY 50.0

#define VS_FM_DEFAULT_HEADING_MODE    VS_FM_MODE_INCREMENTAL
#define VS_FM_DEFAULT_PITCH_MODE      VS_FM_MODE_ABSOLUTE
#define VS_FM_DEFAULT_THROTTLE_MODE   VS_FM_MODE_INCREMENTAL

class vsFlyingMotion : public vsMotionModel
{
protected:

    // Control input primitives
    vsInputAxis         *headingAxis;
    vsInputAxis         *pitchAxis;

    vsInputAxis         *throttleAxis;
    vsInputButton       *accelButton;
    vsInputButton       *decelButton;
    vsInputButton       *stopButton;

    // Last frame's headingAxis and pitchAxis values
    double              lastHeadingAxisVal;
    double              lastPitchAxisVal;

    // Number of units per square second the velocity will increase while
    // the acceleration button is held down, or the throttle axis is in 
    // incremental mode and set to maximum
    double              accelerationRate;

    // Number of degrees per second the orientation will change when the 
    // heading or pitch control is in incremental mode and set to the 
    // maximum or minimum position
    double              turningRate;

    // Current forward velocity
    double              velocity;
    double              maxVelocity;

    // Mode settings for each axis
    vsFlyingAxisMode    headingMode, pitchMode, throttleMode;


public:

                        vsFlyingMotion(vsMouse *mouse);

                        vsFlyingMotion(vsMouse *mouse, int accelButtonIndex,
                                       int decelButtonIndex, 
                                       int stopButtonIndex);

                        vsFlyingMotion(vsInputAxis *headingAx,
                                       vsInputAxis *pitchAx, 
                                       vsInputAxis *throttleAx);

                        vsFlyingMotion(vsInputAxis *headingAx,
                                       vsInputAxis *pitchAx, 
                                       vsInputButton *accelBtn,
                                       vsInputButton *decelBtn, 
                                       vsInputButton *stopBtn);

                        ~vsFlyingMotion();

    void                getAxisModes(vsFlyingAxisMode *heading,
                                     vsFlyingAxisMode *pitch,
                                     vsFlyingAxisMode *throttle);
    void                setAxisModes(vsFlyingAxisMode newHeadingMode,
                                     vsFlyingAxisMode newPitchMode,
                                     vsFlyingAxisMode newThrottleMode);

    double              getAccelerationRate();
    void                setAccelerationRate(double newRate);
    double              getTurningRate();
    void                setTurningRate(double newRate);
    double              getMaxVelocity();
    void                setMaxVelocity(double newMax);

    virtual vsMatrix    update();
};

#endif
