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
#include "vsKinematics.h++"

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

    // Kinematics
    vsKinematics        *kinematics;

    // Control input primitives
    vsInputAxis         *headingAxis;
    vsInputAxis         *pitchAxis;

    vsInputAxis         *throttleAxis;
    vsInputButton       *accelButton;
    vsInputButton       *decelButton;
    vsInputButton       *stopButton;

    // Number of units per square second the velocity will increase while
    // the acceleration button is held down, or the throttle axis is in 
    // incremental mode and set to maximum
    double              accelerationRate;

    // Number of degrees per second the orientation will change when the 
    // heading or pitch control is in incremental mode and set to the 
    // maximum or minimum position
    double              turningRate;

    // Maximum forward velocity 
    // (NOTE: overrides all other motion model input)
    double              maxVelocity;

    // Mode settings for each axis
    vsFlyingAxisMode    headingMode, pitchMode, throttleMode;


public:

                        vsFlyingMotion(vsMouse *mouse, vsKinematics *kin);

                        vsFlyingMotion(vsMouse *mouse, int accelButtonIndex,
                                       int decelButtonIndex, 
                                       int stopButtonIndex,
                                       vsKinematics *kin);

                        vsFlyingMotion(vsInputAxis *headingAx,
                                       vsInputAxis *pitchAx, 
                                       vsInputAxis *throttleAx,
                                       vsKinematics *kin);

                        vsFlyingMotion(vsInputAxis *headingAx,
                                       vsInputAxis *pitchAx, 
                                       vsInputButton *accelBtn,
                                       vsInputButton *decelBtn, 
                                       vsInputButton *stopBtn,
                                       vsKinematics *kin);

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

    virtual void        update();
};

#endif
