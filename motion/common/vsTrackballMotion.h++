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
//    VESS Module:  vsTrackballMotion.h++
//
//    Description:  Motion model that translates and rotates a component
//		    with the motion of a trackball (or mouse, joystick,
//		    etc. acting as a trackball)
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_TRACKBALL_MOTION_HPP
#define VS_TRACKBALL_MOTION_HPP

// Useful for examining models of objects or controlling them in a direct
// manner.  Can be inertialess or inertia-based, depending on whether the
// user enables inertia on the associated vsKinematics or not.
// 
// Because of its nature (trackball-style control) this motion model may not
// work well with other motion models on the same kinematics object.

#include "vsMotionModel.h++"
#include "vsMouse.h++"
#include "vsKinematics.h++"

// Default number of database units moved (or degrees rotated) per 
// normalized unit of input movement
#define VS_TBM_DEFAULT_TRANSLATE_CONST 10.0
#define VS_TBM_DEFAULT_ROTATE_CONST    180.0

class VS_MOTION_DLL vsTrackballMotion : public vsMotionModel
{
protected:

    // Input axes
    vsInputAxis      *horizontal;
    vsInputAxis      *vertical;

    // Input buttons
    vsInputButton    *transXZButton;
    vsInputButton    *transYButton;
    vsInputButton    *rotButton;

    // Kinematics
    vsKinematics     *kinematics;

    // Previous input values used to calculate velocities
    double           lastHorizontal, lastVertical;

    // Translation/rotation constants
    double           transConst;
    double           rotConst;

public:

    // Constructors (see the source file or documentation for a description
    // of each form).
                          vsTrackballMotion(vsMouse *mouse, vsKinematics *kin);

                          vsTrackballMotion(vsMouse *mouse, 
                                            int xzTransButtonIndex,
                                            int yTransButtonIndex, 
                                            int rotButtonIndex,
                                            vsKinematics *kin);

                          vsTrackballMotion(vsInputAxis *horizAxis, 
                                            vsInputAxis *vertAxis,
                                            vsInputButton *xzTransBtn, 
                                            vsInputButton *yTransBtn, 
                                            vsInputButton *rotBtn,
                                            vsKinematics *kin);

    // Destructor
    virtual               ~vsTrackballMotion();

    // Inherited from vsObject
    virtual const char    *getClassName();

    // Movement constant accessors
    void                  setTranslationConstant(double newConst);
    double                getTranslationConstant();
    void                  setRotationConstant(double newConst);
    double                getRotationConstant();
 
    // Update function
    virtual void          update();
};

#endif
