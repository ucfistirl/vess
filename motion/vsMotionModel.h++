#ifndef VS_MOTION_MODEL_HPP
#define VS_MOTION_MODEL_HPP

// Abstract base class for all motion models

class vsMotionModel
{
public:

    // Constructor/destructor
                        vsMotionModel();
    virtual             ~vsMotionModel();

    // Update function.  Must be overridden by any concrete descendant.
    virtual void        update() = 0;

    // Reset (calibration) function.  May be overridden, not required.
    virtual void        reset();
};

#endif
