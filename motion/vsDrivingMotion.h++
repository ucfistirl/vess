#ifndef VS_DRIVING_MOTION_HPP
#define VS_DRIVING_MOTION_HPP

// Motion model for simple driving action.  Takes 2 axes, one for forward
// motion control (throttle), and one for heading changes (steering).  Both
// controls can be set for position, velocity, or acceleration control.  
// Additionally, steering can be set to change heading based on current linear
// velocity (controlled by the throttle, as in a regular car), or to change 
// heading directly (as in a tracked vehicle such as a tank).  Throttle and
// steering have separate scaling factors to scale the input values received.

#include "vsMotionModel.h++"

enum
{
    VS_DM_CONTROL_POSITION,
    VS_DM_CONTROL_VELOCITY,
    VS_DM_CONTROL_ACCELERATION
}

enum
{
    VS_DM_STEER_RELATIVE,
    VS_DM_STEER_ABSOLUTE
}

class vsDrivingMotion : public vsMotionModel
{
protected:

     vsBehaviorList    *behaviors;

     vsInputAxis       *throttle;
     int               throttleControl;
     double            throttleScale;

     vsInputAxis       *steering;
     int               steeringControl;
     int               steeringMode;
     double            steeringScale;

public:

                    vsDrivingMotion(vsComponent *component, 
                                    vsInputAxis *throttleAxis,
                                    vsInputAxis *steeringAxis);

                    vsDrivingMotion(vsComponent *component, 
                                    vsInputDevice *input,
                                    int throttleAxis, int steeringAxis);

    int             setThrottleControl(int control)
    int             setThrottleScale(double scale)

    int             setSteeringControl(int control)
    int             setSteeringMode(int mode)
    int             setSteeringScale(double scale)

    virtual void    update();
};

#endif
