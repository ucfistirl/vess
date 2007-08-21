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
#include "vsQuat.h++"
#include "vsVector.h++"

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

    void        setPosition(vsVector newPosition);
    vsVector    getPosition();

    void        setOrientation(vsQuat newOrientation);
    vsQuat      getOrientation();

    void        setVelocity(vsVector newVelocity);
    vsVector    getVelocity();

    void        setAngularVelocity(vsVector rotAxis,
                    double degreesPerSec);
    vsVector    getAngularVelocity();

    void        setMassProperties(vsMassProperties *mass);

    void        setBoundingVolume(vsBoundingVolume *volume);

    void        applyForce(vsVector force);
    void        applyTorque(vsVector torque);

    vsVector    getForces();
    vsVector    getTorques();

    void        clearForces();
    void        clearTorques();
};

#endif
