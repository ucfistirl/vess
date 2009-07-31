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
//                  component in the scene graph
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsTimer.h++"
#include "vsKinematics.h++"

// ------------------------------------------------------------------------
// Constructor - Verfies that there is a transform attribute on the
// component and sets the internal position and orientation data
// ------------------------------------------------------------------------
vsKinematics::vsKinematics(vsComponent *theComponent)
{
    atMatrix xform;
    int loop;

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
    inertia = false;

    // * Initialize orientation constraint values
    constrainOnUpdate = false;

    // Axes: heading (z), pitch (x), roll (y)
    constraintAxis[0].set(0.0, 0.0, 1.0);
    constraintAxis[1].set(1.0, 0.0, 0.0);
    constraintAxis[2].set(0.0, 1.0, 0.0);

    // Angles: full range (-180.0 to 180.0)
    for (loop = 0; loop < 3; loop++)
    {
        constraintMinAngle[loop] = -180.0;
        constraintMaxAngle[loop] = 180.0;
    }

    // The update method must be called only if we need to integrate
    // velocities into position/orientation changes, or if constraints need
    // to be applied.  Assume at first that we don't need any updates
    updateRequired = false;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsKinematics::~vsKinematics()
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsKinematics::getClassName()
{
    return "vsKinematics";
}

// ------------------------------------------------------------------------
// Turns inertia on.  Velocities will be preserved between frames.
// ------------------------------------------------------------------------
void vsKinematics::enableInertia()
{
    inertia = true;
}

// ------------------------------------------------------------------------
// Turns inertia off.  Velocities will zeroed before each frame.
// ------------------------------------------------------------------------
void vsKinematics::disableInertia()
{
    inertia = false;
}

// ------------------------------------------------------------------------
// Returns if inertia is enabled and velocities are preserved between
// frames.
// ------------------------------------------------------------------------
bool vsKinematics::isInertiaEnabled()
{
   return inertia;
}

// ------------------------------------------------------------------------
// Sets the translation
// ------------------------------------------------------------------------
void vsKinematics::setPosition(atVector newPosition)
{
    atMatrix xform;
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
atVector vsKinematics::getPosition()
{
    return position;
}

// ------------------------------------------------------------------------
// Adds the given vector to the current translation
// ------------------------------------------------------------------------
void vsKinematics::modifyPosition(atVector deltaPosition)
{
    atVector dPos;
    atMatrix xform;
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
void vsKinematics::setOrientation(atQuat newOrientation)
{
    atMatrix xform, tempMat;
    atQuat tempQuat;
    
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
atQuat vsKinematics::getOrientation()
{
    return orientation;
}

// ------------------------------------------------------------------------
// Multiplies the current rotation by the given rotation on the left
// ------------------------------------------------------------------------
void vsKinematics::preModifyOrientation(atQuat deltaOrientation)
{
    atMatrix xform, tempMat;
    
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
void vsKinematics::postModifyOrientation(atQuat deltaOrientation)
{
    atMatrix xform, tempMat;
    
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
void vsKinematics::setVelocity(atVector newVelocity)
{
    // Copy the new velocity to the kinematics velocity vector
    velocity.clearCopy(newVelocity);

    // Make sure the vector is size 3
    velocity.setSize(3);

    // If the velocity is non-zero, set the flag indicating we require
    // an update
    if (fabs(velocity.getMagnitudeSquared()) > 0.0)
        updateRequired = true;
}

// ------------------------------------------------------------------------
// Retrieves the positional velocity
// ------------------------------------------------------------------------
atVector vsKinematics::getVelocity()
{
    return velocity;
}

// ------------------------------------------------------------------------
// Adds the given velocity to the current positional velocity
// ------------------------------------------------------------------------
void vsKinematics::modifyVelocity(atVector deltaVelocity)
{
    atVector dVel;
    
    // Copy the delta velocity
    dVel.clearCopy(deltaVelocity);
    dVel.setSize(3);
    
    // Add the delta velocity to the current velocity
    velocity += dVel;

    // If the velocity is non-zero, set the flag indicating we require
    // an update
    if (fabs(velocity.getMagnitudeSquared()) > 0.0)
        updateRequired = true;
}

// ------------------------------------------------------------------------
// Sets the angular velocity
// ------------------------------------------------------------------------
void vsKinematics::setAngularVelocity(atVector rotAxis, double degreesPerSec)
{
    atVector axis;
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

    // If the angular velocity is non-zero, set the flag that indicates
    // we require an update
    if (fabs(degreesPerSec) > 0.0)
        updateRequired = true;
}

// ------------------------------------------------------------------------
// Retrieves the angular velocity as a vector containing the axis of
// rotation in the first three positions and a rotation speed, represented
// as degrees per second, in the fourth position.
// ------------------------------------------------------------------------
atVector vsKinematics::getAngularVelocity()
{
    return angularVelocity;
}

// ------------------------------------------------------------------------
// Modifies the current angular velocity to be a composite of the current
// angular velocity and the given angular velocity
// ------------------------------------------------------------------------
void vsKinematics::modifyAngularVelocity(atVector rotAxis,
    double degreesPerSec)
{
    atVector avel1, avel2, result;
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

    // If the angular velocity is non-zero, set the flag that indicates
    // we require an update
    if (fabs(angularVelocity[3]) > 0.0)
        updateRequired = true;
}

// ------------------------------------------------------------------------
// Sets the center point for rotations of this object
// ------------------------------------------------------------------------
void vsKinematics::setCenterOfMass(atVector newCenter)
{
    atMatrix xform;
    atVector nCenter;
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
atVector vsKinematics::getCenterOfMass()
{
    atMatrix xform;
    atVector result;

    // Get the post-transform of the kinematics transform attribute
    xform = transform->getPostTransform();
    
    // Copy the translation component of the transform matrix and invert
    // it
    result.set(xform[0][3], xform[1][3], xform[2][3]);
    result.scale(-1.0);
    
    return result;
}

// ------------------------------------------------------------------------
// Sets one of the orientation constraints for this kinematics object. The
// amount of rotation that the kinematics' orientation makes around the
// specified axis is limited to the specified degree measures. A zero axis
// value specifies that this constraint is not used. The idx value
// specifies which of the three constraints is being set, with the first
// constraint having a value of zero.
// ------------------------------------------------------------------------
void vsKinematics::setConstraint(int idx, atVector axis, double minAngle,
    double maxAngle)
{
    // Bounds/sanity checking
    if ((idx < 0) || (idx > 2))
    {
        printf("vsKinematics::setConstraint: Invalid idx value (%d)\b", idx);
        return;
    }
    if ((minAngle < -180.0) || (minAngle > 180.0))
    {
        printf("vsKinematics::setConstraint: minAngle (%lf) must be in the "
            "range [-180.0, 180.0]\n", minAngle);
        return;
    }
    if ((maxAngle < -180.0) || (maxAngle > 180.0))
    {
        printf("vsKinematics::setConstraint: maxAngle (%lf) must be in the "
            "range [-180.0, 180.0]\n", maxAngle);
        return;
    }
    if (minAngle > maxAngle)
    {
        printf("vsKinematics::setConstraint: minAngle (%lf) must be less "
            "than or equal to maxAngle (%lf)\n", minAngle, maxAngle);
        return;
    }

    // Set the constraint data
    constraintAxis[idx].clearCopy(axis);
    constraintMinAngle[idx] = minAngle;
    constraintMaxAngle[idx] = maxAngle;
}

// ------------------------------------------------------------------------
// Gets one of the orientation constraints for this kinematics object. The
// amount of rotation that the kinematics' orientation makes around the
// specified axis is limited to the specified degree measures. A zero axis
// value specifies that this constraint is not used. The idx value
// specifies which of the three constraints is being retrieved, with the
// first constraint having a value of zero.
// ------------------------------------------------------------------------
void vsKinematics::getConstraint(int idx, atVector *axis, double *minAngle,
    double *maxAngle)
{
    // Bounds checking
    if ((idx < 0) || (idx > 2))
    {
        printf("vsKinematics::getConstraint: Invalid idx value (%d)\b", idx);
        return;
    }

    // Copy the constraint data to the specified locations
    if (axis)
        *axis = constraintAxis[idx];
    if (minAngle)
        *minAngle = constraintMinAngle[idx];
    if (maxAngle)
        *maxAngle = constraintMaxAngle[idx];
}

// ------------------------------------------------------------------------
// Enables the application of orientation constraints at the end of every
// update call
// ------------------------------------------------------------------------
void vsKinematics::enableConstrainOnUpdate()
{
    constrainOnUpdate = true;

    // We're applying constraints now, so we need updates
    updateRequired = true;
}

// ------------------------------------------------------------------------
// Disables the application of orientation constraints at the end of every
// update call
// ------------------------------------------------------------------------
void vsKinematics::disableConstrainOnUpdate()
{
    constrainOnUpdate = false;
}

// ------------------------------------------------------------------------
// Returns if orientation constraints are set to be applied during update
// calls
// ------------------------------------------------------------------------
bool vsKinematics::isConstrainOnUpdateEnabled()
{
    return constrainOnUpdate;
}

// ------------------------------------------------------------------------
// Apply the orientation constraints to the kinematics' current orientation
// ------------------------------------------------------------------------
void vsKinematics::applyConstraints()
{
    atQuat currentRot, newRot;
    atQuat resultRot;
    atVector axis;
    double rotAngle;
    int loop;

    // Initialize the rotation to pull from to the specified rotation
    currentRot = getOrientation();

    // Initilize the result to no rotation
    resultRot.set(0.0, 0.0, 0.0, 1.0);

    // Loop over all three constraints
    for (loop = 0; loop < 3; loop++)
        // Only apply a constraint if the axis is nonzero
        if (constraintAxis[loop].getMagnitude() > 0.0)
        {
            // Copy the constraint axis to a more convenient variable
            axis = constraintAxis[loop];

            // Determine how much the rotation rotates around the constraint
            // axis
            rotAngle = calculateAxisRotation(currentRot, axis);

            // Clamp the rotation around the constraint axis to the values
            // specified for this constraint
            rotAngle = constrainAngle(rotAngle, constraintMinAngle[loop],
                constraintMaxAngle[loop]);

            // Compute the quaternion that represents the constrained rotation
            newRot.setAxisAngleRotation(axis[0], axis[1], axis[2], rotAngle);

            // Remove the constrained rotation from the input rotation...
            currentRot = newRot.getInverse() * currentRot;
            // ...and multiply it into the output rotation.
            resultRot = resultRot * newRot;
        }

    // Set the final orientation to the product of the constrained orientations
    setOrientation(resultRot);
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

    // Get the time elapsed between system frames
    deltaTime = vsTimer::getSystemTimer()->getInterval();

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
    atQuat deltaOrient;
    double degrees;

    // If we don't need an update, return immediately
    if (!updateRequired)
        return;

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

        // Velocities are zero again, so we don't need an update unless
        // constraints are enabled
        updateRequired = constrainOnUpdate;
    }

    // Apply the orientation constraints, if applicable
    if (constrainOnUpdate)
        applyConstraints();
}

// ------------------------------------------------------------------------
// Private function
// Clamps the degree measure 'value' to the range indicated by the
// minDegrees and maxDegrees parameters. The clamping takes into account
// the 'circular' nature of angle measures; a value far enough below the
// low end of the range might 'circle around' and be clamped to the high
// end of the range, and vice versa.
// ------------------------------------------------------------------------
double vsKinematics::constrainAngle(double value, double minDegrees,
    double maxDegrees)
{
    double dist2Min, dist2Max;

    // Force the value to be in the range <-180.0, 180.0]
    if (value > 180.0)
        value -= 360.0;
    if (value <= -180.0)
        value += 360.0;

    // If the value falls within the acceptable range, return it
    if ((value >= minDegrees) && (value <= maxDegrees))
        return value;

    // If the value is outside of the range, determine which extreme of the
    // range that the value is closest to, taking into account the cyclic
    // nature of angle measurements
    if (value < minDegrees)
    {
        dist2Min = minDegrees - value;
        dist2Max = value - (maxDegrees - 360.0);
    }
    else // (value > maxDegrees)
    {
        dist2Min = (minDegrees + 360.0) - value;
        dist2Max = value - maxDegrees;
    }

    // Return the degree measure of the closest range extreme
    if (dist2Min < dist2Max)
        return minDegrees;
    else
        return maxDegrees;
}

// ------------------------------------------------------------------------
// Private function
// Determine the amount of rotation that the specified rotation rotates
// around the specified rotation axis
// ------------------------------------------------------------------------
double vsKinematics::calculateAxisRotation(atQuat rotation, atVector axis)
{
    atVector planeVec, rotVec, axisVec;
    double dotProd, rotDegrees;

    // * Create a vector that is in the plane perpendicular to the axis vector

    // Start with an arbitrary vector
    planeVec.set(0.0, 1.0, 0.0);
    // The vector must not point in the same (or opposite) direction as the
    // axis vector; pick a new vector if that is the case
    dotProd = planeVec.getDotProduct(axis);
    if (AT_EQUAL(fabs(dotProd), 1.0))
    {
        planeVec.set(0.0, 0.0, 1.0);
        dotProd = planeVec.getDotProduct(axis);
    }
    // Remove the portion of the vector that points in the direction of the
    // axis vector
    planeVec -= axis.getScaled(dotProd);
    // Force the result to be unit-length
    planeVec.normalize();

    // * Calculate the vector that is the in-plane vector, rotated by the
    // given quaternion, and constrained to the perpendicular plane

    // Rotate the in-plane vector by the quaternion
    rotVec = rotation.rotatePoint(planeVec);
    // If the result is parallel to the axis, then this information doesn't
    // help us any; try again with another vector in the plane
    dotProd = rotVec.getDotProduct(axis);
    if (AT_EQUAL(fabs(dotProd), 1.0))
    {
        // Make a new in-plane vector
        planeVec = planeVec.getCrossProduct(axis);
        // And run the calculation again
        rotVec = rotation.rotatePoint(planeVec);
        dotProd = rotVec.getDotProduct(axis);
    }
    // Project the resulting vector back into the axis-perpendicular plane
    rotVec -= axis.getScaled(dotProd);
    // And force it to be unit-length
    rotVec.normalize();

    // * Determine the net amount of rotation applied to the in-plane vector

    // Get the degree measure of rotation between the vectors
    rotDegrees = fabs(planeVec.getAngleBetween(rotVec));
    // Get the axis of rotation between the two vectors
    axisVec = planeVec.getCrossProduct(rotVec);
    // If the calculated axis of rotation is opposite from the original target
    // axis, then negate the rotation amount
    if (axis.getDotProduct(axisVec) < 0.0)
        rotDegrees *= -1.0;

    // Done.
    return rotDegrees;
}
