#ifndef VS_TRACKBALL_MOTION_HPP
#define VS_TRACKBALL_MOTION_HPP

// Motion model that translates and rotates a component with the motion of a
// trackball.  Useful for examining models of objects or controlling them
// in a direct manner.

#include "vsMotionModel.h++"
#include "vsMouse.h++"

// Number of degrees rotated per normalized unit of movement
#define VS_TBM_TRANSLATE_CONST 10.0
#define VS_TBM_ROTATE_CONST    180.0

class vsTrackballMotion : public vsMotionModel
{
protected:

    vsInputAxis      *horizontal;
    vsInputAxis      *vertical;

    vsInputButton    *transXZButton;
    vsInputButton    *transYButton;
    vsInputButton    *rotButton;

    double           lastHorizontal, lastVertical;

public:

                         vsTrackballMotion(vsMouse *mouse);

                         vsTrackballMotion(vsMouse *mouse, 
                                           int xyTransButtonIndex,
                                           int zTransButtonIndex, 
                                           int rotButtonIndex);

                         vsTrackballMotion(vsInputAxis *horizAxis, 
                                           vsInputAxis *vertAxis,
                                           vsInputButton *xyTransBtn, 
                                           vsInputButton *zTransBtn, 
                                           vsInputButton *rotBtn);

                         ~vsTrackballMotion();
 
    virtual vsVecQuat    update();
};

#endif
