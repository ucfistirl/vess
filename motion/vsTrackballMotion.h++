#ifndef VS_TRACKBALL_MOTION_HPP
#define VS_TRACKBALL_MOTION_HPP

// Motion model that translates and rotates a component with the motion of a
// trackball (or mouse, joystick, etc. acting as a trackball).  Useful for 
// examining models of objects or controlling them in a direct manner.  Can be 
// inertialess or inertia-based, depending on whether the user enables inertia 
// on the associated vsKinematics or not.
// 
// Because of its nature (trackball-style control) this motion model may not
// work well with other motion models on the same kinematics object.

#include "vsMotionModel.h++"
#include "vsMouse.h++"
#include "vsKinematics.h++"

// Number of degrees rotated per normalized unit of movement
#define VS_TBM_TRANSLATE_CONST 10.0
#define VS_TBM_ROTATE_CONST    180.0

class vsTrackballMotion : public vsMotionModel
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

public:

    // Constructors (see the source file or documentation for a description
    // of each form).
                        vsTrackballMotion(vsMouse *mouse, vsKinematics *kin);

                        vsTrackballMotion(vsMouse *mouse, 
                                          int xyTransButtonIndex,
                                          int zTransButtonIndex, 
                                          int rotButtonIndex,
                                          vsKinematics *kin);

                        vsTrackballMotion(vsInputAxis *horizAxis, 
                                          vsInputAxis *vertAxis,
                                          vsInputButton *xyTransBtn, 
                                          vsInputButton *zTransBtn, 
                                          vsInputButton *rotBtn,
                                          vsKinematics *kin);

    // Destructor
                        ~vsTrackballMotion();
 
    // Update function
    virtual void    update();
};

#endif
