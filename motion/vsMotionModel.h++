#ifndef VS_MOTION_MODEL_HPP
#define VS_MOTION_MODEL_HPP

// Abstract base class for all motion models

class vsMotionModel
{
private:

    // Time of day when this motion model was last updated
    double      lastTime;

protected:

    // Returns time in seconds between calls to this function
    double      getTimeInterval();

public:
                        vsMotionModel();
    virtual             ~vsMotionModel();

    virtual void        update() = 0;
    virtual void        reset();
};

#endif
