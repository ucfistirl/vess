// File vsKinematics.c++

#include "vsKinematics.h++"

#include <sys/time.h>

vsKinematics::vsKinematics(vsComponent *theComponent)
{
    vsMatrix xform;

    component = theComponent;
    
    transform = (vsTransformAttribute *)
	(component->getTypedAttribute(VS_ATTRIBUTE_TYPE_TRANSFORM, 0));
    if (!transform)
    {
	transform = new vsTransformAttribute();
	component->addAttribute(transform);
    }
    
    reset();
    
    xform = transform->getDynamicTransform();
    position.set(xform[0][3], xform[1][3], xform[2][3]);
    orientation.setMatrixRotation(xform);
}

vsKinematics::~vsKinematics()
{
}

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

vsVector vsKinematics::getPosition()
{
    return position;
}

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

vsQuat vsKinematics::getOrientation()
{
    return orientation;
}

void vsKinematics::preModifyOrientation(vsQuat deltaOrientation)
{
    vsMatrix xform, tempMat;
    
    orientation = orientation * deltaOrientation;
    xform.setQuatRotation(orientation);
    tempMat.setTranslation(position[0], position[1], position[2]);
    xform = tempMat * xform;

    transform->setDynamicTransform(xform);
}

void vsKinematics::postModifyOrientation(vsQuat deltaOrientation)
{
    vsMatrix xform, tempMat;
    
    orientation = deltaOrientation * orientation;
    xform.setQuatRotation(orientation);
    tempMat.setTranslation(position[0], position[1], position[2]);
    xform = tempMat * xform;

    transform->setDynamicTransform(xform);
}

void vsKinematics::setVelocity(vsVector newVelocity)
{
    velocity.clearCopy(newVelocity);
    velocity.setSize(3);
}

vsVector vsKinematics::getVelocity()
{
    return velocity;
}

void vsKinematics::modifyVelocity(vsVector deltaVelocity)
{
    vsVector dVel;
    
    dVel.clearCopy(deltaVelocity);
    dVel.setSize(3);
    
    velocity += dVel;
}

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
    
    angularVelocity.set(axis[0], axis[1], axis[2], degreesPerSec);
}

vsVector vsKinematics::getAngularVelocity()
{
    return angularVelocity;
}

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

vsVector vsKinematics::getCenterOfMass()
{
    vsMatrix xform;
    vsVector result;

    xform = transform->getPostTransform();
    
    result.set(xform[0][3], xform[1][3], xform[2][3]);
    
    return result;
}

void vsKinematics::update()
{
    struct timeval tv;
    double currentTime, deltaTime;
    vsQuat deltaOrient;
    double degrees;

    // Get the time elapsed since the last call
    gettimeofday(&tv, NULL);
    currentTime = (double)(tv.tv_sec) + ((double)(tv.tv_usec) / 1000000.0);
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    
    if (deltaTime > 1.0)
	deltaTime = 1.0;

    // Update the position and orientation from the velocity and angular
    // velocity, respectively.
    modifyPosition(velocity.getScaled(deltaTime));
    
    degrees = (angularVelocity[3] * deltaTime);
    deltaOrient.setAxisAngleRotation(angularVelocity[0], angularVelocity[1],
	angularVelocity[2], degrees);
    postModifyOrientation(deltaOrient);
}

void vsKinematics::reset()
{
    velocity.set(0.0, 0.0, 0.0);
    angularVelocity.set(0.0, 0.0, 0.0, 0.0);
    
    resetTimer();
}

void vsKinematics::resetTimer()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    
    lastTime = (double)(tv.tv_sec) + ((double)(tv.tv_usec) / 1000000.0);
}
