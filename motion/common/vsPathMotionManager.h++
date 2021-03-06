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
//    VESS Module:  vsPathMotionManager.h++
//
//    Description:  A manager to control several vsPathMotion objects
//                  simultaneously.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_PATH_MOTION_MANAGER_HPP
#define VS_PATH_MOTION_MANAGER_HPP

#include "vsUpdatable.h++"
#include "vsPathMotion.h++"
#include "vsSequencer.h++"

class VESS_SYM vsPathMotionManager : public vsUpdatable
{
private:

    int                currentPlayMode;

    int                cycleMode;
    int                cycleCount;
    int                currentCycleCount;

    vsSequencer        *pathMotionSequencer;
    int                pathMotionCount;

public:

                          vsPathMotionManager();
                          vsPathMotionManager(vsPathMotionManager *original);
    virtual               ~vsPathMotionManager();

    virtual const char    *getClassName();

    void                  setCycleMode(int mode);
    void                  setCycleCount(int cycles);
    int                   getCycleMode();
    int                   getCycleCount();

    void                  startResume();
    void                  pause();
    void                  stop();
    int                   getPlayMode();
    bool                  isDone();

    virtual void          update();
    virtual void          update(double deltaTime);

    void                  addPathMotion(vsPathMotion *pathMotion);
    void                  removePathMotion(vsPathMotion *pathMotion);

    vsPathMotion          *getPathMotion(int index);
    int                   getPathMotionCount();
};

#endif
