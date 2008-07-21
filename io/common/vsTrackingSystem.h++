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
//    VESS Module:  vsTrackingSystem.h++
//
//    Description:  Abstract base class for all motion tracking systems
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_TRACKING_SYSTEM_HPP
#define VS_TRACKING_SYSTEM_HPP

#include "vsMotionTracker.h++"
#include "vsIOSystem.h++"

class VESS_SYM vsTrackingSystem : public vsIOSystem
{
public:

                            vsTrackingSystem();
    virtual                 ~vsTrackingSystem();

    virtual int             getNumTrackers(void) = 0;
    virtual vsMotionTracker *getTracker(int index) = 0;
};

#endif
