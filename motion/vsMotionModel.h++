#ifndef VS_MOTION_MODEL_HPP
#define VS_MOTION_MODEL_HPP

// Abstract base class for all motion models

#include "vsGlobals.h++"
#include "vsVecQuat.h++"
#include "vsComponent.h++"
#include "vsTransformAttribute.h++"
#include "vsInputAxis.h++"
#include "vsInputButton.h++"

class vsMotionModel
{
protected:

    // Time of day when this motion model was last updated
    double      lastTime;

public:
                         vsMotionModel();
    virtual              ~vsMotionModel();

    // Returns time in seconds between calls to this function
    double               getTimeInterval();

    virtual vsVecQuat    update() = 0;
};

#endif
