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
//    VESS Module:  vsDynamicWorld.h++
//
//    Description:  This class represents a dynamic world. Dynamic units,
//                  joints, and contact points all live under a dynamic
//                  world, and may be updated in a variable time step.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_DYNAMIC_WORLD_HPP
#define VS_DYNAMIC_WORLD_HPP

class vsDynamicWorld;

#include <ode/ode.h>

#include "vsContactPoint.h++"
#include "vsObject.h++"

#include "atVector.h++"

#include "vsGlobals.h++"

#define VS_DW_DEFAULT_ERP 0.8
#define VS_DW_DEFAULT_CFM 1e-5
#define VS_DW_DEFAULT_GRAVITY atVector(0.0, 0.0, -9.8)
#define VS_DW_DEFAULT_CONTACT_VELOCITY 1.0
#define VS_DW_DEFAULT_CONTACT_DEPTH 1e-3

class VESS_SYM vsDynamicWorld : public vsObject
{
private:

    dWorldID         odeWorldID;
    dJointGroupID    odeContactGroupID;

VS_INTERNAL:

    dWorldID         getODEWorldID();

public:

                vsDynamicWorld();
    virtual     ~vsDynamicWorld();

    virtual const char    *getClassName();

    void        setCFM(double cfm);
    void        setERP(double erp);
    void        setGravity(atVector gravity);

    void        setContactProperties(double velocity, double depth);
    void        addContact(vsContactPoint *contact);
    void        clearContacts();

    void        update(double timestep);
};

#endif

