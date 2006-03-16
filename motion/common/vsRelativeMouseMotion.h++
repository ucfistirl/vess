//------------------------------------------------------------------------
//
//    VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//    Copyright (c) 2002, University of Central Florida
//
//       See the file LICENSE for license information
//
//    E-mail:  vess@ist.ucf.edu
//    WWW:     http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//    VESS Module:  vsRelativeMouseMotion.h++
//
//    Description:  Motion model to move based on relative moments of the
//                  given input axes. For example, this gives "Quake-like"
//                  motion to a kinematics object when combined with the
//                  vsMouse from vsWindowSystem.
//
//    Author(s):    Ryan Wilson, Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_RELATIVE_MOTION_HPP
#define VS_RELATIVE_MOTION_HPP

#include "vsMotionModel.h++"
#include "vsFlyingMotion.h++"
#include "vsInputAxis.h++"

#define NUMBER_OF_AXES 2

class VS_MOTION_DLL vsRelativeMouseMotion : public vsMotionModel
{
private:
    // The input axes that will affect the orientation
    vsInputAxis         *inputAxis[NUMBER_OF_AXES];

    // The kinematics object affected by the orientation and velocity
    vsKinematics        *kinematics;

    // Scaling factor - for every full window scroll of the mouse, rotate this
    // many degrees around the given axis
    double  axisChange[NUMBER_OF_AXES];

    // prePost affects how the orientation transformation is applied.
    // Whether or not to do pre- or post-multiplication
    //  PRE=false =  newOrientation = transform * currentOrientation
    //  POST=true =  newOrientation = currentOrientation * transform
    //    * Rotate around FIXED axis in space
    bool    prePost[NUMBER_OF_AXES];

    // Enforce axis limits (i.e. can't turn head past certain angle)
    double  axisLimits[NUMBER_OF_AXES];
    double  kinMin[NUMBER_OF_AXES];
    double  kinMax[NUMBER_OF_AXES];

    // which axis to rotate around? AXIS_X, AXIS_Y, AXIS_Z
    int     rotationAxis[NUMBER_OF_AXES];

    // Control input primitives (throttle axis is currently not used)
    vsInputAxis         *throttleAxis;
    vsInputButton       *accelButton;
    vsInputButton       *decelButton;
    vsInputButton       *stopButton;

    // Number of units per square second the velocity will increase while
    // the acceleration button is held down, or the throttle axis is in 
    // incremental mode and set to maximum
    double              accelerationRate;

    // Current forward velocity
    double              currentSpeed;

    // Maximum forward velocity 
    // (NOTE: overrides all other motion model input applied previous to
    //        this one)
    double              maxSpeed;

    // Mode settings for each axis
    vsFlyingAxisMode    throttleMode;

public:
                          vsRelativeMouseMotion(vsMouse *, vsKinematics *);
    virtual               ~vsRelativeMouseMotion();

    virtual const char    *getClassName();

    void                  reset();

    // Update velocity and orientation
    void                  update();

    // Update just velocity or orientation
    void                  updateVelocity();
    void                  updateOrientation();

    void                  setThrottleAxisMode( vsFlyingAxisMode newMode );
    vsFlyingAxisMode      getThrottleAxisMode();

    void                  setAxisLimits(int axis, double minLimit,
                                        double maxLimit);
    void                  setAxisLimits(int axis, double minLimit);
    void                  setAxisLimits(int axis);

    // Apply the transformation with (pre|post)modify?
    void                  setAxisPrePost(int axis, bool isPost);
    bool                  getAxisPrePost(int axis);

    // For every 1.0 movement of the vsInputAxis, rotate scaleFactor degrees
    void                  setAxisChange(int axis, double scaleFactor);
    double                getAxisChange(int axis);

    // Which axis (VS_X, VS_Y, VS_Z) to rotate around
    void                  setRotationAxis(int axis, int newRotationAxis);
    int                   getRotationAxis(int axis);

    // Motion parameter accessors
    double                getAccelerationRate();
    void                  setAccelerationRate(double newRate);
    double                getMaxSpeed();
    void                  setMaxSpeed(double newMax);
};

#endif
