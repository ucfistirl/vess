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

class VS_IO_DLL vsMotionTracker : public vs6DInputDevice
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

    virtual const char    *getClassName();

    virtual int           getNumButtons();

    virtual vsInputButton *getButton(int index);

    int                   getTrackerNumber();
    void                  setTrackerNumber(int newNumber);
};

#endif
