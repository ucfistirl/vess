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
//    VESS Module:  vsKinematics.c++
//
//    Description:  Main object for associating a motion model with a
//		    component in the scene graph
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsSystem.h++"
#include "vsKinematics.h++"

// ------------------------------------------------------------------------
// Constructor - Verfies that there is a transform attribute on the
// component and sets the internal position and orientation data
// ------------------------------------------------------------------------
vsKinematics::vsKinematics(vsComponent *theComponent)
{
    vsMatrix xform;

    // Save the component passed in
    component = theComponent;

    // Complain if the given component is NULL
    if (!component)
    {
	printf("vsKinematics::vsKinematics: NULL component\n");
	return;
    }
    
    // vsKinematics objects require that the associated component
    // have a vsTransformAttribute present and will create one
    // on the component if there is not one already.
    transform = (vsTransformAttribute *)
        (component->getTypedAttribute(VS_ATTRIBUTE_TYPE_TRANSFORM, 0));
    if (!transform)
    {
        transform = new vsTransformAttribute();
        component->addAttribute(transform);
    }
    
    // Initialize position/orientation from the vsTransformAttribute
    xform = transform->getDynamicTransform();
    position.set(xform[0][3], xform[1][3], xform[2][3]);
    orientation.setMatrixRotation(xform);

    // Initialize velocities to zero
    velocity.setSize(3);
    velocity.clear();
    angularVelocity.setSize(4);
    angularVelocity.clear();

    // Default inertia to false
    inertia = VS_FALSE;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsKinematics::~vsKinematics()
{
}

// ------------------------------------------------------------------------
// Turns inertia on.  Velocities will be preserved between frames.
// ------------------------------------------------------------------------
void vsKinematics::enableInertia()
{
    inertia = VS_TRUE;
}

// ------------------------------------------------------------------------
// Turns inertia off.  Velocities will zeroed before each frame.
// ------------------------------------------------------------------------
void vsKinematics::disableInertia()
{
    inertia = VS_FALSE;
}

// ------------------------------------------------------------------------
// Sets the translation
// ------------------------------------------------------------------------
void vsKinematics::setPosition(vsVector newPosition)
{
    vsMatrix xform;
    int loop;
    
    // Copy the new position
    position.clearCopy(newPosition);
    position.setSize(3);
    
    // Get the dynamic transform matrix
    xform = transform->getDynamicTransform();

    // Modify the matrix with the new position
    for (loop = 0; loop < 3; loop++)
        xform[loop][3] = position[loop];

    // Update the dynamic transform
    transform->setDynamicTransform(xform);
}

// ------------------------------------------------------------------------
// Retrieves the translation
// ------------------------------------------------------------------------
vsVector vsKinematics::getPosition()
{
    return position;
}

// ------------------------------------------------------------------------
// Adds the given vector to the current translation
// ------------------------------------------------------------------------
void vsKinematics::modifyPosition(vsVector deltaPosition)
{
    vsVector dPos;
    vsMatrix xform;
    int loop;
    
    // Copy the position change and add it to the current position
    dPos.clearCopy(deltaPosition);
    dPos.setSize(3);
    position += dPos;
    
    // Get the dynamic transform matrix
    xform = transform->getDynamicTransform();

    // Modify the matrix with the new position
    for (loop = 0; loop < 3; loop++)
        xform[loop][3] = position[loop];

    // Update the dynamic transform
    transform->setDynamicTransform(xform);
}

// ------------------------------------------------------------------------
// Sets the rotation
// ------------------------------------------------------------------------
void vsKinematics::setOrientation(vsQuat newOrientation)
{
    vsMatrix xform, tempMat;
    vsQuat tempQuat;
    
    // Copy the new orientation 
    orientation = newOrientation;

    // Construct a transform matrix with the new orientation and
    // the current translation
    xform.setQuatRotation(orientation);
    tempMat.setTranslation(position[0], position[1], position[2]);
    xform = tempMat * xform;

    // Update the dynamic transform
    transform->setDynamicTransform(xform);
}

// ------------------------------------------------------------------------
// Retrieves the rotation
// ------------------------------------------------------------------------
vsQuat vsKinematics::getOrientation()
{
    return orientation;
}

// ------------------------------------------------------------------------
// Multiplies the current rotation by the given rotation on the left
// ------------------------------------------------------------------------
void vsKinematics::preModifyOrientation(vsQuat deltaOrientation)
{
    vsMatrix xform, tempMat;
    
    // Modify the current orientation (preMultiply)
    orientation = deltaOrientation * orientation;

    // Construct a transform matrix with the new orientation and
    // the current translation
    xform.setQuatRotation(orientation);
    tempMat.setTranslation(position[0], position[1], position[2]);
    xform = tempMat * xform;

    // Update the dynamic transform
    transform->setDynamicTransform(xform);
}

// ------------------------------------------------------------------------
// Multiplies the current rotation by the given rotation on the right
// ------------------------------------------------------------------------
void vsKinematics::postModifyOrientation(vsQuat deltaOrientation)
{
    vsMatrix xform, tempMat;
    
    // Modify the current orientation (postMultiply)
    orientation = orientation * deltaOrientation;

    // Construct a transform matrix with the new orientation and
    // the current translation
    xform.setQuatRotation(orientation);
    tempMat.setTranslation(position[0], position[1], position[2]);
    xform = tempMat * xform;

    // Update the dynamic transform
    transform->setDynamicTransform(xform);
}

// ------------------------------------------------------------------------
// Sets the positional velocity
// ------------------------------------------------------------------------
void vsKinematics::setVelocity(vsVector newVelocity)
{
    // Copy the new velocity to the kinematics velocity vector
    velocity.clearCopy(newVelocity);

    // Make sure the vector is size 3
    velocity.setSize(3);
}

// ------------------------------------------------------------------------
// Retrieves the positional velocity
// ------------------------------------------------------------------------
vsVector vsKinematics::getVelocity()
{
    return velocity;
}

// ------------------------------------------------------------------------
// Adds the given velocity to the current positional velocity
// ------------------------------------------------------------------------
void vsKinematics::modifyVelocity(vsVector deltaVelocity)
{
    vsVector dVel;
    
    // Copy the delta velocity
    dVel.clearCopy(deltaVelocity);
    dVel.setSize(3);
    
    // Add the delta velocity to the current velocity
    velocity += dVel;
}

// ------------------------------------------------------------------------
// Sets the angular velocity
// ------------------------------------------------------------------------
void vsKinematics::setAngularVelocity(vsVector rotAxis, double degreesPerSec)
{
    vsVector axis;
    double mag;
    
    // Copy the rotation axis
    axis.clearCopy(rotAxis);
    axis.setSize(3);

    // If the axis magnitude is zero, return immediately
    mag = axis.getMagnitude();
    if (mag < 1E-6)
    {
        angularVelocity.set(0.0, 0.0, 0.0, 0.0);
        return;
    }

    // Normalize the axis
    axis.normalize();
    
    // The internal representation of angular velocity is an axis of
    // rotation and a degrees-per-second value. This is similar to, but
    // not quite the same as, the internal representation of a
    // quaternion.
    angularVelocity.set(axis[0], axis[1], axis[2], degreesPerSec);
}

// ------------------------------------------------------------------------
// Retrieves the angular velocity as a vector containing the axis of
// rotation in the first three positions and a rotation speed, represented
// as degrees per second, in the fourth position.
// ------------------------------------------------------------------------
vsVector vsKinematics::getAngularVelocity()
{
    return angularVelocity;
}

// ------------------------------------------------------------------------
// Modifies the current angular velocity to be a composite of the current
// angular velocity and the given angular velocity
// ------------------------------------------------------------------------
void vsKinematics::modifyAngularVelocity(vsVector rotAxis,
    double degreesPerSec)
{
    vsVector avel1, avel2, result;
    double mag;
    
    // If the degrees per second of the current angular velocity is zero
    // just set the modification as the new velocity
    if (angularVelocity[3] == 0.0)
    {
        setAngularVelocity(rotAxis, degreesPerSec);
        return;
    }

    // Copy the rotation axis of the modification
    avel1.clearCopy(rotAxis);
    avel1.setSize(3);

    // If the axis is near zero in length, there is no modification
    // to make, so bail
    mag = avel1.getMagnitude();
    if (mag < 1E-6)
        return;

    // Normalize the axis, and scale it by the rotation rate
    avel1.normalize();
    avel1.scale(degreesPerSec);
    
    // Copy the current angular velocity's rotation vector and
    // scale it by the current rotation rate
    avel2.set(angularVelocity[0], angularVelocity[1], angularVelocity[2]);
    avel2.scale(angularVelocity[3]);
    
    // Combine the two rotations, add the vectors together
    result = avel1 + avel2;

    // Get the magnitude of the resulting vector
    mag = result.getMagnitude();

    // If the magnitude is near zero, clamp the angular velocity to
    // zero, otherwise set the rotation rate to the magnitude of the 
    // vector and normalize vector to get the axis of rotation.
    if (mag > 1E-6)
    {
        result.normalize();
        angularVelocity.set(result[0], result[1], result[2], mag);
    }
    else
        angularVelocity.set(0.0, 0.0, 0.0, 0.0);
}

// ------------------------------------------------------------------------
// Sets the center point for rotations of this object
// ------------------------------------------------------------------------
void vsKinematics::setCenterOfMass(vsVector newCenter)
{
    vsMatrix xform;
    vsVector nCenter;
    int loop;
    
    // Copy the center of mass and make sure it is a 3-component vector
    nCenter.clearCopy(newCenter);
    nCenter.setSize(3);

    // Set the pre-transform of the kinematics transform attribute to
    // the new center of mass
    xform = transform->getPreTransform();
    for (loop = 0; loop < 3; loop++)
        xform[loop][3] = nCenter[loop];
    transform->setPreTransform(xform);

    // Set the post-transform of the kinematics transform attribute to
    // the inverse of the new center of mass
    xform = transform->getPostTransform();
    for (loop = 0; loop < 3; loop++)
        xform[loop][3] = -(nCenter[loop]);
    transform->setPostTransform(xform);
}

// ------------------------------------------------------------------------
// Retrieves the center point for rotations of this object
// ------------------------------------------------------------------------
vsVector vsKinematics::getCenterOfMass()
{
    vsMatrix xform;
    vsVector result;

    // Get the post-transform of the kinematics transform attribute
    xform = transform->getPostTransform();
    
    // Copy the translation component of the transform matrix and invert
    // it
    result.set(xform[0][3], xform[1][3], xform[2][3]);
    result.scale(-1.0);
    
    return result;
}

// ------------------------------------------------------------------------
// Retrieves the component for this object
// ------------------------------------------------------------------------
vsComponent *vsKinematics::getComponent()
{
    return component;
}

// ------------------------------------------------------------------------
// Updates the kinematics using the system frame time for the time interval
// ------------------------------------------------------------------------
void vsKinematics::update()
{
    double deltaTime;

    // Get the time elapsed since the last call
    deltaTime = vsSystem::systemObject->getFrameTime();

    // Constrain the time to a maximum of one second to try to maintain
    // interactivity when the frame rate drops very low
    if (deltaTime > 1.0)
        deltaTime = 1.0;

    // Update the kinematics using this interval
    update(deltaTime);
}

// ------------------------------------------------------------------------
// Updates the kinematics by using the specified time interval and the 
// current positional and angular velocities to modify the current position 
// and orientation.  This form of update() is useful for non-realtime 
// applications.
// ------------------------------------------------------------------------
void vsKinematics::update(double deltaTime)
{
    vsQuat deltaOrient;
    double degrees;

    // Make sure we have a valid deltaTime
    if (deltaTime <= 0.0)
        return;
    
    // Update the position and orientation from the velocity and angular
    // velocity, respectively.
    modifyPosition(velocity.getScaled(deltaTime));
    
    // Scale the degrees per second of the angular velocity
    degrees = (angularVelocity[3] * deltaTime);

    // Create a quaternion representing the orienation adjustment
    deltaOrient.setAxisAngleRotation(angularVelocity[0], angularVelocity[1],
        angularVelocity[2], degrees);

    // Apply the orientation adjustment
    postModifyOrientation(deltaOrient);

    // Clear velocities if in inertialess mode
    if (!inertia)
    {
        velocity.clear();
        angularVelocity.clear();
    }
}
