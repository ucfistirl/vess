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
//    VESS Module:  vsDynamicUnit.c++
//
//    Description:  
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsDynamicUnit.h++"

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsDynamicUnit::vsDynamicUnit(vsDynamicWorld *world)
{
    // Create the dBody type using the dWorld of our vsDynamicWorld.
    odeBodyID = dBodyCreate(world->getODEWorldID());
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsDynamicUnit::~vsDynamicUnit()
{
    // Free up the dBody.
    dBodyDestroy(odeBodyID);
}

// ------------------------------------------------------------------------
// Enables calculations on this vsDynamicUnit during updates.
// ------------------------------------------------------------------------
void vsDynamicUnit::enable()
{
    dBodyEnable(odeBodyID);
}

// ------------------------------------------------------------------------
// Disables calculations on this vsDynamicUnit during updates.
// ------------------------------------------------------------------------
void vsDynamicUnit::disable()
{
    dBodyDisable(odeBodyID);
}

// ------------------------------------------------------------------------
// Set the position of this vsDynamicUnit.
// ------------------------------------------------------------------------
void vsDynamicUnit::setPosition(atVector position)
{
    // Set the new position on the dBody.
    dBodySetPosition(odeBodyID,
        position[VS_X], position[VS_Y], position[VS_Z]);
}

// ------------------------------------------------------------------------
// Return the position of this vsDynamicUnit.
// ------------------------------------------------------------------------
atVector vsDynamicUnit::getPosition()
{
    const dReal *position;

    // Get the current position from the dBody.
    position = dBodyGetPosition(odeBodyID);

    // Return a new atVector with the appropriate data.
    return atVector(position[0], position[1], position[2]);
}

// ------------------------------------------------------------------------
// Set the orientation of this vsDynamicUnit.
// ------------------------------------------------------------------------
void vsDynamicUnit::setOrientation(atQuat orientation)
{
    dQuaternion quaternion;

    // Fill the quaternion with the atQuat data. dQuaternions store data in
    // the format w, x, y, z.
    quaternion[0] = orientation[3];
    quaternion[1] = orientation[VS_X];
    quaternion[2] = orientation[VS_Y];
    quaternion[3] = orientation[VS_Z];

    // Set the new position on the dBody.
    dBodySetQuaternion(odeBodyID, quaternion);
}

// ------------------------------------------------------------------------
// Return the orientation of this vsDynamicUnit.
// ------------------------------------------------------------------------
atQuat vsDynamicUnit::getOrientation()
{
    const dReal *quaternion;

    // Fetch the other quaternion.
    quaternion = dBodyGetQuaternion(odeBodyID);

    // Return a new atQuat with the appropriate data. The ODE representation
    // places the W component first, hence the offset.
    return atQuat(quaternion[1], quaternion[2], quaternion[3], quaternion[0]);
}

// ------------------------------------------------------------------------
// Set the linear velocity of this vsDynamicUnit.
// ------------------------------------------------------------------------
void vsDynamicUnit::setVelocity(atVector velocity)
{
    // Set the new linear velocity on the dBody.
    dBodySetLinearVel(odeBodyID,
        velocity[VS_X], velocity[VS_Y], velocity[VS_Z]);
}

// ------------------------------------------------------------------------
// Return the linear velocity of this vsDynamicUnit.
// ------------------------------------------------------------------------
atVector vsDynamicUnit::getVelocity()
{
    const dReal *velocity;

    // Fetch the linear velocity of the dBody.
    velocity = dBodyGetLinearVel(odeBodyID);

    // Return a new atVector with the appropriate data.
    return atVector(velocity[0], velocity[1], velocity[2]);
}

// ------------------------------------------------------------------------
// Set the angular velocity of this vsDynamicUnit.
// ------------------------------------------------------------------------
void vsDynamicUnit::setAngularVelocity(atVector rotAxis, double degreesPerSec)
{
    // Normalize the rotation axis, then scale it by the magnitude of the
    // rotation.
    rotAxis.normalize();
    rotAxis.scale(degreesPerSec);

    // Set the new angular velocity on the dBody.
    dBodySetAngularVel(odeBodyID, rotAxis[VS_X], rotAxis[VS_Y], rotAxis[VS_Z]);
}

// ------------------------------------------------------------------------
// Return the angular velocity of this vsDynamicUnit. The axis of rotation
// is defined by the first three points of the vector, with the magnitude
// of rotation stored in the fourth.
// ------------------------------------------------------------------------
atVector vsDynamicUnit::getAngularVelocity()
{
    const dReal *velocity;
    double magnitude;
    atVector returnVector;

    // Fetch the angular velocity of the dBody.
    velocity = dBodyGetLinearVel(odeBodyID);

    // Set the x, y, and z components of the return vector.
    returnVector.set(velocity[0], velocity[1], velocity[2], 0.0);
    magnitude = returnVector.getMagnitude();

    // Now normalize the return vector and set its magnitude.
    returnVector.normalize();
    returnVector[3] = magnitude;

    // Return the vector.
    return returnVector;
}

// ------------------------------------------------------------------------
// Set the mass properties of this vsDynamicUnit. 
// ------------------------------------------------------------------------
void vsDynamicUnit::setMassProperties(vsMassProperties *mass)
{
    atVector center;
    atMatrix inertia;

    // Fetch the values from the mass properties.
    center = mass->getCenterOfMass();
    inertia = mass->getInertiaMatrix();

    // Use the values stored in the vsMassProperties object.
    dMassSetParameters(&odeMass, center[3], 0.0, 0.0, 0.0,
        inertia[0][0], inertia[1][1], inertia[2][2], inertia[0][1],
        inertia[0][2], inertia[1][2]);
}

// ------------------------------------------------------------------------
// Associate all ODE geometry within the bounding volume with the ODE body
// of this dynamic unit.
// ------------------------------------------------------------------------
void vsDynamicUnit::setBoundingVolume(vsBoundingVolume *volume)
{
    dSpaceID boundingSpace;
    dGeomID boundingGeom;
    int i;

    // Fetch the ODE space ID from this geometry.
    boundingSpace = volume->getODESpaceID();

    // Fetch each of the geometries (or geometry transforms) within this
    // space. Note that this will NOT WORK for "quadtree"-divided spaces
    // according to the most recent documentation on ODE.
    for (i = 0; i < dSpaceGetNumGeoms(boundingSpace); i++)
    {
        // Get the geometry at the current index.
        boundingGeom = dSpaceGetGeom(boundingSpace, i);

        // Associate the dGeom with the dBody of this vsDynamicUnit.
        dGeomSetBody(boundingGeom, odeBodyID);
    }
}

// ------------------------------------------------------------------------
// Apply a force directly to the center of mass of this vsDynamicUnit.
// ------------------------------------------------------------------------
void vsDynamicUnit::applyForce(atVector force)
{
    dBodyAddForce(odeBodyID, force[VS_X], force[VS_Y], force[VS_Z]);
}

// ------------------------------------------------------------------------
// Apply a torque directly about the center of mass of this vsDynamicUnit.
// ------------------------------------------------------------------------
void vsDynamicUnit::applyTorque(atVector torque)
{
    dBodyAddTorque(odeBodyID, torque[VS_X], torque[VS_Y], torque[VS_Z]);
}

// ------------------------------------------------------------------------
// Returns the accumulation of forces applied to this vsDynamicUnit.
// ------------------------------------------------------------------------
atVector vsDynamicUnit::getForces()
{
    const dReal *force;

    // Fetch the force acting upon the dBody of this vsDynamicUnit.
    force = dBodyGetForce(odeBodyID);

    // Return a atVector containing the result.
    return atVector(force[0], force[1], force[2]);
}

// ------------------------------------------------------------------------
// Returns the accumulation of torques applied to this vsDynamicUnit.
// ------------------------------------------------------------------------
atVector vsDynamicUnit::getTorques()
{
    const dReal *torque;

    // Fetch the torque acting upon the dBody of this vsDynamicUnit.
    torque = dBodyGetTorque(odeBodyID);

    // Return a atVector containing the result.
    return atVector(torque[0], torque[1], torque[2]);
}

// ------------------------------------------------------------------------
// Clear all forces acting upon this dynamic unit.
// ------------------------------------------------------------------------
void vsDynamicUnit::clearForces()
{
    dBodySetForce(odeBodyID, 0.0, 0.0, 0.0);
}

// ------------------------------------------------------------------------
// Apply a torque directly about the center of mass of this vsDynamicUnit.
// ------------------------------------------------------------------------
void vsDynamicUnit::clearTorques()
{
    dBodySetTorque(odeBodyID, 0.0, 0.0, 0.0);
}

// ------------------------------------------------------------------------
// VESS Internal Function
// Returns the ODE body ID associated with this dynamic unit.
// ------------------------------------------------------------------------
dBodyID vsDynamicUnit::getODEBodyID()
{
    return odeBodyID;
}
