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
//    VESS Module:  vsFlyingMotion.h++
//
//    Description:  Motion model for simple flying action (not true
//		    aerodynamic flying)
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_FLYING_MOTION_HPP
#define VS_FLYING_MOTION_HPP

// Takes either three axes (heading, pitch, and throttle), or two axes
// (heading and pitch) and up to three buttons (accelerate, decelerate, and
// stop).  Intended to provide a simple way to explore a scene with either
// mouse controls or a joystick.
//
// Note that the motion model expects the vsInputAxes it's given to be
// normalized.  A warning message will be printed if they are not, and 
// performance will not likely be what is expected.
//
// Also note that this motion model is exclusive.  It most likely will not
// work well with other motion models on the same vsKinematics object, since 
// it eliminates any roll component incurred, and sets the heading and pitch 
// absolutely.  The vsKinematics object provided should have inertia 
// disabled.

#include "vsMotionModel.h++"
#include "vsMouse.h++"
#include "vsInputAxis.h++"
#include "vsInputButton.h++"
#include "vsKinematics.h++"

// Axis indices
enum VS_MOTION_DLL vsFlyingAxis
{
    VS_FM_AXIS_HEADING  = 0,
    VS_FM_AXIS_PITCH    = 1,
    VS_FM_AXIS_THROTTLE = 2
};

// Axis modes
enum VS_MOTION_DLL vsFlyingAxisMode
{
    VS_FM_MODE_INCREMENTAL = 0,
    VS_FM_MODE_ABSOLUTE    = 1,
    VS_FM_MODE_NO_CHANGE   = -1
};

// Parameter defaults
#define VS_FM_DEFAULT_ACCEL_RATE   20.0
#define VS_FM_DEFAULT_TURNING_RATE 50.0
#define VS_FM_DEFAULT_MAX_SPEED    50.0

#define VS_FM_DEFAULT_HEADING_MODE    VS_FM_MODE_INCREMENTAL
#define VS_FM_DEFAULT_PITCH_MODE      VS_FM_MODE_ABSOLUTE
#define VS_FM_DEFAULT_THROTTLE_MODE   VS_FM_MODE_INCREMENTAL

class VS_MOTION_DLL vsFlyingMotion : public vsMotionModel
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

    // Current forward velocity
    double              currentSpeed;

    // Maximum forward velocity 
    // (NOTE: overrides all other motion model input applied previous to
    //        this one)
    double              maxSpeed;

    // Mode settings for each axis
    vsFlyingAxisMode    headingMode, pitchMode, throttleMode;


public:

    // Constructors (see the source file or documentation for a description
    // of each form)
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

    // Destructor
    virtual               ~vsFlyingMotion();

    // Inherited from vsObject
    virtual const char    *getClassName();

    // Axis mode parameter accessors
    void                  getAxisModes(vsFlyingAxisMode *heading,
                                       vsFlyingAxisMode *pitch,
                                       vsFlyingAxisMode *throttle);
    void                  setAxisModes(vsFlyingAxisMode newHeadingMode,
                                       vsFlyingAxisMode newPitchMode,
                                       vsFlyingAxisMode newThrottleMode);

    // Motion parameter accessors
    double                getAccelerationRate();
    void                  setAccelerationRate(double newRate);
    double                getTurningRate();
    void                  setTurningRate(double newRate);
    double                getMaxSpeed();
    void                  setMaxSpeed(double newMax);

    // Update function
    virtual void          update();
};

#endif
