#ifndef VS_TRACKBALL_MOTION_HPP
#define VS_TRACKBALL_MOTION_HPP

// Motion model that translates and rotates a component with the motion of a
// trackball.  Useful for examining models of objects or controlling them
// in a direct manner.

#include "vsMotionModel.h++"
#include "vsMouse.h++"
#include "vsKinematics.h++"

// Number of degrees rotated per normalized unit of movement
#define VS_TBM_TRANSLATE_CONST 10.0
#define VS_TBM_ROTATE_CONST    180.0

class vsTrackballMotion : public vsMotionModel
{
protected:

    vsInputAxis      *horizontal;
    vsInputAxis      *vertical;

    vsKinematics     *kinematics;

    vsInputButton    *transXZButton;
    vsInputButton    *transYButton;
    vsInputButton    *rotButton;

    double           lastHorizontal, lastVertical;

public:

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

                        ~vsTrackballMotion();
 
    virtual void    update();
};

#endif
