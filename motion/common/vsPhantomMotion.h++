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
//    VESS Module:  vsPhantomMotion.h++
//
//    Description:  Class intended to take motion data from the Phantom
//		    and apply the movements directly to the component
//
//    Author(s):    Jason Daly, Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_PHANTOM_MOTION_HPP
#define VS_PHANTOM_MOTION_HPP

#include "vsMotionModel.h++"
#include "vsPhantom.h++"
#include "vsKinematics.h++"

class VS_MOTION_DLL vsPhantomMotion : public vsMotionModel
{
protected:

    // The Phantom device
    vsPhantom          *phantom;

    // Kinematics
    vsKinematics       *kinematics;

    // Flags to indicate if position or orientation tracking is enabled
    bool               positionEnabled;
    bool               orientationEnabled;
    
public:

    // Constructor
                          vsPhantomMotion(vsPhantom *theTracker,
                                          vsKinematics *kinObject);

    // Destructor
    virtual               ~vsPhantomMotion();

    // Inherited from vsObject
    virtual const char    *getClassName();

    // Methods to enable/disable position tracking
    void                  enablePositionTracking();
    void                  disablePositionTracking();
    void                  enableOrientationTracking();
    void                  disableOrientationTracking();
    
    // Update function
    virtual void          update();
};

#endif
