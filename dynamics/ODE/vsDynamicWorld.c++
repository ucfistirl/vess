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
//    VESS Module:  vsDynamicWorld.c++
//
//    Description:  This class represents a dynamic world. Dynamic units,
//                  joints, and contact points all live under a dynamic
//                  world, and may be updated in a variable time step.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsDynamicWorld.h++"

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsDynamicWorld::vsDynamicWorld()
{
    // Create a dWorld.
    odeWorldID = dWorldCreate();

    // Set up the world with default properties.
    setERP(VS_DW_DEFAULT_ERP);
    setCFM(VS_DW_DEFAULT_CFM);
    setGravity(VS_DW_DEFAULT_GRAVITY);
    setContactProperties(VS_DW_DEFAULT_CONTACT_VELOCITY,
        VS_DW_DEFAULT_CONTACT_DEPTH);

    // Create a joint group dedicated to this dynamic world. This functionality
    // is managed within the dynamic world class because its organization is
    // specific to ODE, and because each world needs exactly one contact group.
    odeContactGroupID = dJointGroupCreate(0);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsDynamicWorld::~vsDynamicWorld()
{
    // Destroy the dWorld.
    dWorldDestroy(odeWorldID);

    // Destroy the joint group.
    dJointGroupDestroy(odeContactGroupID);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
const char *vsDynamicWorld::getClassName()
{
    return "vsDynamicWorld";
}

// ------------------------------------------------------------------------
// Update the constraint force mixing (CFM) parameter for this world.
// ------------------------------------------------------------------------
void vsDynamicWorld::setCFM(double cfm)
{
    dWorldSetCFM(odeWorldID, cfm);
}

// ------------------------------------------------------------------------
// Update the constraint force mixing (CFM) parameter for this world.
// ------------------------------------------------------------------------
void vsDynamicWorld::setContactProperties(double velocity, double depth)
{
    dWorldSetContactMaxCorrectingVel(odeWorldID, velocity);
    dWorldSetContactSurfaceLayer(odeWorldID, depth);
}

// ------------------------------------------------------------------------
// Update the error reduction paramater (ERP) for this dynamic world.
// ------------------------------------------------------------------------
void vsDynamicWorld::setERP(double erp)
{
    dWorldSetERP(odeWorldID, erp);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void vsDynamicWorld::setGravity(atVector gravity)
{
    dWorldSetGravity(odeWorldID, gravity[VS_X], gravity[VS_Y], gravity[VS_Z]);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void vsDynamicWorld::addContact(vsContactPoint *contact)
{
    dJointID contactJoint;
    dContact odeContact;

    // Fetch the contact structure from the vsContactPoint.
    odeContact = contact->getODEContact();

    // Create a contact joint to represent the collision point. Place that
    // joint within the dynamic world, using the contact joint group.
    contactJoint =
        dJointCreateContact(odeWorldID, odeContactGroupID, &odeContact);

    // Attach the bodies of the colliding geoms.
    dJointAttach(contactJoint, dGeomGetBody(odeContact.geom.g1),
        dGeomGetBody(odeContact.geom.g2));
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void vsDynamicWorld::clearContacts()
{
    dJointGroupEmpty(odeContactGroupID);
}

// ------------------------------------------------------------------------
// Update the underlying dWorld with the provided time step.
// ------------------------------------------------------------------------
void vsDynamicWorld::update(double timestep)
{
    dWorldQuickStep(odeWorldID, timestep);
}

// ------------------------------------------------------------------------
// VESS Internal Function
// This method returns the ID value of the ODE world that this dynamic
// world object represents.
// ------------------------------------------------------------------------
dWorldID vsDynamicWorld::getODEWorldID()
{
    return odeWorldID;
}

