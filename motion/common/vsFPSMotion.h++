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
//    VESS Module:  vsFPSMotion.h++
//
//    Description:  Motion model for typical first-person shooter motion
//                  control.  Works with either dual analog stick 
//                  controllers, or a single analog stick and a mouse.
//                  Using vsButtonAxis, four keys on the keyboard (up, 
//                  down, left, right or w, s, a, d) can be converted to 
//                  two movement axes, allowing the typical keyboard/mouse 
//                  shooter controls.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_FPS_MOTION_HPP
#define VS_FPS_MOTION_HPP

// Takes 4 axes, one for forward motion control, one for side-side (strafing),
// one for heading changes (steering), and one for pitch changes.  Two 
// kinematics are used.  The first three axes (forward, strafe, heading) apply 
// to the first kinematics, and the last one (pitch) applies to the second.
// Typically the kinematics provided will be an avatar's root kinematics and
// the avatar's neck, respectively.  However, the same kinematics object can 
// be given for both parameters, if desired (if no avatar is used, for 
// example).

#include "vsMotionModel.h++"
#include "vsKinematics.h++"
#include "vsMouse.h++"

#define VS_FPSM_DEFAULT_MAX_SPEED     4.0
#define VS_FPSM_DEFAULT_HEADING_RATE  100.0
#define VS_FPSM_DEFAULT_PITCH_RATE    90.0
#define VS_FPSM_DEFAULT_PITCH_LIMIT   80.0

enum vsFPSMAxisMode
{
    VS_FPSM_MODE_INCREMENTAL,
    VS_FPSM_MODE_ABSOLUTE
};

class VESS_SYM vsFPSMotion : public vsMotionModel
{
protected:

     // Kinematics object (current motion state)
     vsKinematics        *rootKinematics;
     vsKinematics        *viewKinematics;

     vsInputAxis         *forward;
     vsInputAxis         *strafe;
     vsInputAxis         *heading;
     vsInputAxis         *pitch;

     double              maxForwardSpeed;
     double              maxReverseSpeed;
     double              maxStrafeSpeed;
     double              headingRate;
     vsFPSMAxisMode      headingMode;
     double              pitchRate;
     double              minPitch, maxPitch;
     vsFPSMAxisMode      pitchMode;

public:

                          vsFPSMotion(vsInputAxis *forwardAxis,
                                      vsInputAxis *strafeAxis,
                                      vsInputAxis *headingAxis,
                                      vsInputAxis *pitchAxis,
                                      vsKinematics *rootKin,
                                      vsKinematics *viewKin);

                          vsFPSMotion(vsInputAxis *forwardAxis, 
                                      vsInputAxis *strafeAxis, 
                                      vsMouse *mouse,
                                      vsKinematics *rootKin,
                                      vsKinematics *viewKin);

    // Destructor
    virtual               ~vsFPSMotion();

    // Inherited from vsObject
    virtual const char    *getClassName();

    double                getMaxForwardSpeed();
    void                  setMaxForwardSpeed(double max);
    double                getMaxReverseSpeed();
    void                  setMaxReverseSpeed(double max);
    double                getMaxStrafeSpeed();
    void                  setMaxStrafeSpeed(double max);
    double                getHeadingRate();
    void                  setHeadingRate(double rate);
    vsFPSMAxisMode        getHeadingAxisMode();           
    void                  setHeadingAxisMode(vsFPSMAxisMode newMode);           
    double                getPitchRate();
    void                  setPitchRate(double rate);
    void                  getPitchLimits(double *min, double *max);
    void                  setPitchLimits(double min, double max);
    vsFPSMAxisMode        getPitchAxisMode();           
    void                  setPitchAxisMode(vsFPSMAxisMode newMode);           

    // Updates the motion model
    virtual void          update();
};

#endif
