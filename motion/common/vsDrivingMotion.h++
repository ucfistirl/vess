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
//    VESS Module:  vsDrivingMotion.h++
//
//    Description:  Motion model for simple driving action
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_DRIVING_MOTION_HPP
#define VS_DRIVING_MOTION_HPP

// Takes 2 axes, one for forward motion control (throttle), and one for
// heading changes (steering).  Both controls can be set for position,
// velocity, or acceleration control.  Additionally, steering can be set to
// change heading based on current linear velocity (controlled by the throttle,
// as in a regular car), or to change heading directly (as in a tracked 
// vehicle such as a tank).  Throttle and steering have separate scaling 
// factors to scale the input values received.
//
// The vsKinematics object provided should have inertia disabled.  Should work 
// well with other motion models on the same vsKinematics, provided the other
// motion models don't require inertia.

#include "vsMotionModel.h++"
#include "vsKinematics.h++"
#include "vsMouse.h++"

enum  vsDMThrottleMode
{
    VS_DM_THROTTLE_VELOCITY,
    VS_DM_THROTTLE_ACCELERATION
};

enum  vsDMSteeringMode
{
    VS_DM_STEER_RELATIVE,
    VS_DM_STEER_ABSOLUTE
};

#define VS_DM_DEFAULT_ACCEL_RATE    20.0
#define VS_DM_DEFAULT_STEER_RATE    50.0
#define VS_DM_DEFAULT_MAX_SPEED     150.0
#define VS_DM_DEFAULT_THROTTLE_MODE VS_DM_THROTTLE_ACCELERATION
#define VS_DM_DEFAULT_STEERING_MODE VS_DM_STEER_ABSOLUTE

class VS_MOTION_DLL vsDrivingMotion : public vsMotionModel
{
protected:

     // Kinematics object (current motion state)
     vsKinematics        *kinematics;

     // Throttle control (either a single axis or a collection of buttons)
     vsInputAxis         *throttle;

     vsInputButton       *accelButton;
     vsInputButton       *decelButton;
     vsInputButton       *stopButton;

     // Throttle parameters
     vsDMThrottleMode    throttleMode;
     double              currentSpeed;
     double              maxForwardSpeed;
     double              maxReverseSpeed;
     double              accelerationRate;
     double              lastThrottleVal;
     
     // Steering control
     vsInputAxis         *steering;

     // Steering parameters
     vsDMSteeringMode    steeringMode;
     double              steeringRate;
     double              lastSteeringVal;

public:

    // Constructors (see the source file or documentation for an explanation
    // of each form)
                          vsDrivingMotion(vsInputAxis *steeringAxis,
                                          vsInputAxis *throttleAxis,
                                          vsKinematics *kin);

                          vsDrivingMotion(vsInputAxis *steeringAxis,
                                          vsInputButton *accelButton,
                                          vsInputButton *decelButton,
                                          vsInputButton *stopButton,
                                          vsKinematics *kin);

                          vsDrivingMotion(vsMouse *mouse, vsKinematics *kin);

                          vsDrivingMotion(vsMouse *mouse, 
                                          int accelButtonIndex,
                                          int decelButtonIndex, 
                                          int stopButtonIndex,
                                          vsKinematics *kin);

    // Destructor
    virtual               ~vsDrivingMotion();

    // Inherited from vsObject
    virtual const char    *getClassName();

    // Throttle parameter accessors
    vsDMThrottleMode      getThrottleMode();
    void                  setThrottleMode(vsDMThrottleMode mode);
    double                getAccelerationRate();
    void                  setAccelerationRate(double rate);
    double                getMaxForwardSpeed();
    void                  setMaxForwardSpeed(double max);
    double                getMaxReverseSpeed();
    void                  setMaxReverseSpeed(double max);

    // Steering parameter accessors
    vsDMSteeringMode      getSteeringMode();
    void                  setSteeringMode(vsDMSteeringMode mode);
    double                getSteeringRate();
    void                  setSteeringRate(double rate);

    // Updates the motion model
    virtual void          update();
};

#endif
