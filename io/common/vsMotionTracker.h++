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
//    VESS Module:  vsMotionTracker.h++
//
//    Description:  Class for storing and returning the state of a motion
//                  tracker
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_MOTION_TRACKER_HPP
#define VS_MOTION_TRACKER_HPP

#include "vs6DInputDevice.h++"

#define VS_MT_MAX_BUTTONS 5

class VESS_SYM vsMotionTracker : public vs6DInputDevice
{
protected:

    // Number of this tracker in tracking system (defaults to 0)
    int              trackerNumber;  

    int              numButtons;
    vsInputButton    *button[VS_MT_MAX_BUTTONS];

VS_INTERNAL:

    void        setPosition(atVector posVec);
    void        setOrientation(atVector ornVec, atMathEulerAxisOrder axisOrder);
    void        setOrientation(atMatrix ornMat);
    void        setOrientation(atQuat ornQuat);

public:

                          vsMotionTracker(int trackerNum);
                          vsMotionTracker(int trackerNum, int nButtons);
                          vsMotionTracker();
    virtual               ~vsMotionTracker();

    virtual const char    *getClassName();

    virtual int           getNumButtons();

    virtual vsInputButton *getButton(int index);

    int                   getTrackerNumber();
    void                  setTrackerNumber(int newNumber);
};

#endif
