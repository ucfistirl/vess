#ifndef VS_MOTION_TRACKER_HPP
#define VS_MOTION_TRACKER_HPP

// Class for storing and returning the state of a motion tracker

#include "vs6DInputDevice.h++"

class vsMotionTracker : public vs6DInputDevice
{
protected:

    // Number of this tracker in tracking system (defaults to 0)
    int         trackerNumber;  

VS_INTERNAL:

    void        setPosition(vsVector posVec);
    void        setOrientation(vsVector ornVec, vsMathEulerAxisOrder axisOrder);
    void        setOrientation(vsMatrix ornMat);
    void        setOrientation(vsQuat ornQuat);

public:

                          vsMotionTracker(int trackerNum);
                          vsMotionTracker();
                          ~vsMotionTracker();

    virtual int           getNumButtons();

    virtual vsInputButton *getButton(int index);

    int                   getTrackerNumber();
    void                  setTrackerNumber(int newNumber);
};

#endif
