// File vsKinematics.c++

#include "vsSystem.h++"
#include "vsKinematics.h++"


// ------------------------------------------------------------------------
// Constructor - Verfies that there is a transform attribute on the
// component and sets the internal position and orientation data
// ------------------------------------------------------------------------
vsKinematics::vsKinematics(vsComponent *theComponent)
{
    vsMatrix xform;

    component = theComponent;
    if (!component)
    {
	printf("vsKinematics::vsKinematics: NULL component\n");
	return;
    }
    
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
    
    position.clearCopy(newPosition);
    position.setSize(3);
    
    xform = transform->getDynamicTransform();

    for (loop = 0; loop < 3; loop++)
        xform[loop][3] = position[loop];

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
    
    dPos.clearCopy(deltaPosition);
    dPos.setSize(3);
    position += dPos;
    
    xform = transform->getDynamicTransform();

    for (loop = 0; loop < 3; loop++)
        xform[loop][3] = position[loop];

    transform->setDynamicTransform(xform);
}

// ------------------------------------------------------------------------
// Sets the rotation
// ------------------------------------------------------------------------
void vsKinematics::setOrientation(vsQuat newOrientation)
{
    vsMatrix xform, tempMat;
    vsQuat tempQuat;
    
    orientation = newOrientation;
    xform.setQuatRotation(orientation);
    tempMat.setTranslation(position[0], position[1], position[2]);
    xform = tempMat * xform;

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
    
    orientation = deltaOrientation * orientation;
    xform.setQuatRotation(orientation);
    tempMat.setTranslation(position[0], position[1], position[2]);
    xform = tempMat * xform;

    transform->setDynamicTransform(xform);
}

// ------------------------------------------------------------------------
// Multiplies the current rotation by the given rotation on the right
// ------------------------------------------------------------------------
void vsKinematics::postModifyOrientation(vsQuat deltaOrientation)
{
    vsMatrix xform, tempMat;
    
    orientation = orientation * deltaOrientation;
    xform.setQuatRotation(orientation);
    tempMat.setTranslation(position[0], position[1], position[2]);
    xform = tempMat * xform;

    transform->setDynamicTransform(xform);
}

// ------------------------------------------------------------------------
// Sets the positional velocity
// ------------------------------------------------------------------------
void vsKinematics::setVelocity(vsVector newVelocity)
{
    velocity.clearCopy(newVelocity);
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
    
    dVel.clearCopy(deltaVelocity);
    dVel.setSize(3);
    
    velocity += dVel;
}

// ------------------------------------------------------------------------
// Sets the angular velocity
// ------------------------------------------------------------------------
void vsKinematics::setAngularVelocity(vsVector rotAxis, double degreesPerSec)
{
    vsVector axis;
    double mag;
    
    axis.clearCopy(rotAxis);
    axis.setSize(3);
    mag = axis.getMagnitude();
    if (mag < 1E-6)
    {
        angularVelocity.set(0.0, 0.0, 0.0, 0.0);
        return;
    }
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
    
    if (angularVelocity[3] == 0.0)
    {
        setAngularVelocity(rotAxis, degreesPerSec);
        return;
    }

    // Prepare the delta rotation
    avel1.clearCopy(rotAxis);
    avel1.setSize(3);
    mag = avel1.getMagnitude();
    if (mag < 1E-6)
        return;
    avel1.normalize();
    avel1.scale(degreesPerSec);
    
    // Prepare the current rotation
    avel2.set(angularVelocity[0], angularVelocity[1], angularVelocity[2]);
    avel2.scale(angularVelocity[3]);
    
    // Combine the two rotations and store
    result = avel1 + avel2;
    mag = result.getMagnitude();
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
    
    nCenter.clearCopy(newCenter);
    nCenter.setSize(3);

    xform = transform->getPreTransform();
    for (loop = 0; loop < 3; loop++)
        xform[loop][3] = -(nCenter[loop]);
    transform->setPreTransform(xform);

    xform = transform->getPostTransform();
    for (loop = 0; loop < 3; loop++)
        xform[loop][3] = nCenter[loop];
    transform->setPostTransform(xform);
}

// ------------------------------------------------------------------------
// Retrieves the center point for rotations of this object
// ------------------------------------------------------------------------
vsVector vsKinematics::getCenterOfMass()
{
    vsMatrix xform;
    vsVector result;

    xform = transform->getPostTransform();
    
    result.set(xform[0][3], xform[1][3], xform[2][3]);
    
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
// Updates the kinematics by computing the time since the last update, and
// using that time value and the current positional and angular velocities
// to modify the current position and orientation
// ------------------------------------------------------------------------
void vsKinematics::update()
{
    double deltaTime;
    vsQuat deltaOrient;
    double degrees;

    vsVector ornVec;

    // Get the time elapsed since the last call
    deltaTime = vsSystem::systemObject->getFrameTime();

    // Make sure we have a valid deltaTime
    if (deltaTime <= 0.0)
        return;
    
    // Maximum frame time of one second for motion purposes
    if (deltaTime > 1.0)
        deltaTime = 1.0;

    // Update the position and orientation from the velocity and angular
    // velocity, respectively.
    modifyPosition(velocity.getScaled(deltaTime));
    
    degrees = (angularVelocity[3] * deltaTime);
    deltaOrient.setAxisAngleRotation(angularVelocity[0], angularVelocity[1],
        angularVelocity[2], degrees);
    postModifyOrientation(deltaOrient);

    orientation.getEulerRotation(VS_EULER_ANGLES_ZXY_R, &ornVec[VS_H],
        &ornVec[VS_P], &ornVec[VS_R]);

    // Clear velocities if in inertialess mode
    if (!inertia)
    {
        velocity.clear();
        angularVelocity.clear();
    }

}
