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
//    VESS Module:  vsDynamicUnit.h++
//
//    Description:  
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_DYNAMIC_UNIT_HPP
#define VS_DYNAMIC_UNIT_HPP

#include <ode/ode.h>

#include "vsBoundingVolume.h++"
#include "vsDynamicWorld.h++"
#include "vsMassProperties.h++"

#include "atQuat.h++"
#include "atVector.h++"

#include "vsGlobals.h++"

class VS_DYNAMICS_DLL vsDynamicUnit
{
private:

    dWorldID            odeWorldID;

    dBodyID             odeBodyID;
    dMass               odeMass;

VS_INTERNAL:

    dBodyID    getODEBodyID();

public:

                vsDynamicUnit(vsDynamicWorld *world);
    virtual     ~vsDynamicUnit();

    void        enable();
    void        disable();

    void        setPosition(atVector newPosition);
    atVector    getPosition();

    void        setOrientation(atQuat newOrientation);
    atQuat      getOrientation();

    void        setVelocity(atVector newVelocity);
    atVector    getVelocity();

    void        setAngularVelocity(atVector rotAxis,
                    double degreesPerSec);
    atVector    getAngularVelocity();

    void        setMassProperties(vsMassProperties *mass);

    void        setBoundingVolume(vsBoundingVolume *volume);

    void        applyForce(atVector force);
    void        applyTorque(atVector torque);

    atVector    getForces();
    atVector    getTorques();

    void        clearForces();
    void        clearTorques();
};

#endif
