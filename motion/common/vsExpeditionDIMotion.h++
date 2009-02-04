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
//    VESS Module:  vsExpeditionDIMotion.h++
//
//    Description:  Motion model for the Quantum3D ExpeditionDI system.
//                  The ExpeditionDI is a wearable immersive VR system,
//                  consisting of a tracked HMD and surrogate weapon.
//                  The purpose of this motion model is to coordinate the
//                  measurements of the ExpeditionDI's 3 InertiaCube
//                  trackers.  The tracker hardware isn't as significant
//                  as how they are typically affixed to the user (head,
//                  leg, and weapon).  These orientation measurements
//                  are converted into orientations for the three
//                  kinematics objects (root, head, and weapon).
//                  There is also support for the ExpeditionDI's weapon-
//                  mounted joystick and buttons.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_EXPEDITION_DI_MOTION_HPP
#define VS_EXPEDITION_DI_MOTION_HPP


#include "vsMotionModel.h++"
#include "vsKinematics.h++"
#include "vsMotionTracker.h++"
#include "vsJoystick.h++"


enum vsExpDITrackedLeg
{
   VS_EDI_LEFT_LEG,
   VS_EDI_RIGHT_LEG
};


#define VS_EDI_STAND_THRESHOLD  65.0
#define VS_EDI_KNEEL_THRESHOLD  75.0

#define VS_EDI_FORWARD_WALK_SPEED     2.0
#define VS_EDI_FORWARD_RUN_SPEED      8.0
#define VS_EDI_BACKWARD_WALK_SPEED    2.0
#define VS_EDI_BACKWARD_RUN_SPEED     8.0
#define VS_EDI_SIDESTEP_WALK_SPEED    1.0
#define VS_EDI_SIDESTEP_RUN_SPEED     3.0


class VESS_SYM vsExpeditionDIMotion : public vsMotionModel
{
protected:

    vsMotionTracker     *headTracker;
    vsMotionTracker     *legTracker;
    vsMotionTracker     *weaponTracker;
    vsJoystick          *joystick;

    vsKinematics        *rootKin;
    vsKinematics        *headKin;
    vsKinematics        *weaponKin;

    vsExpDITrackedLeg   trackedLeg;

    double              standThreshold;
    double              kneelThreshold;
    bool                kneeling;

public:

                         vsExpeditionDIMotion(vsMotionTracker *headTrkr,
                                              vsMotionTracker *legTrkr,
                                              vsMotionTracker *weaponTrkr,
                                              vsJoystick *stick,
                                              vsKinematics *root,
                                              vsKinematics *head,
                                              vsKinematics *weapon);
    virtual              ~vsExpeditionDIMotion();

    virtual const char   *getClassName();

    void                 setTrackedLeg(vsExpDITrackedLeg newLeg);
    vsExpDITrackedLeg    getTrackedLeg();

    void                 setKneelThresholds(double newStand, double newKneel);
    void                 getKneelThresholds(double *stand, double *kneel);
    bool                 isKneeling();

    virtual void         update();
};

#endif

