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
//    VESS Module:  vsInverseKinematics.h++
//
//    Description:  Class for performing inverse kinematics computations
//                  on a chain of vsKinematics objects
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsInverseKinematics.h++"

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsInverseKinematics::vsInverseKinematics() : dataArray(1, 1)
{
    // The default kinematics chain size is one
    dataArraySize = 0;
    setKinematicsChainSize(1);

    // The default endpoint is one unit in the y-direction from the last joint
    endpointOffset.set(0.0, 1.0, 0.0);

    // Set other default values
    maxProcessLoops = 20;
    successThreshold = 0.001;
    dampeningConstant = 0.005;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsInverseKinematics::~vsInverseKinematics()
{
    int loop;
    vsInvKinData *data;

    // Delete all of the inverse kinematics data structures
    for (loop = 0; loop < dataArraySize; loop++)
        deleteData((vsInvKinData *)(dataArray[loop]));
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsInverseKinematics::getClassName()
{
    return "vsInverseKinematics";
}

// ------------------------------------------------------------------------
// Sets the number of kinematics objects in the chain
// ------------------------------------------------------------------------
void vsInverseKinematics::setKinematicsChainSize(int size)
{
    int loop;

    // Bounds checking
    if (size < 1)
    {
        printf("vsInverseKinematics::setKinematicsChainSize: "
            "Invalid size (%d)\n", size);
        return;
    }

    // Check if we are shrinking or growing the chain
    if (size > dataArraySize)
    {
        // Resize the array to hold the new entries
        dataArray.setSize(size);

        // Create a new data structure for each entry
        for (loop = dataArraySize; loop < size; loop++)
            dataArray[loop] = createData();
    }
    else if (size < dataArraySize)
    {
        // Delete all of the data structures in the unneeded array entries
        for (loop = size; loop < dataArraySize; loop++)
            deleteData((vsInvKinData *)(dataArray[loop]));

        // Shrink the array to the desired size
        dataArray.setSize(size);
    }

    // Record the new array size
    dataArraySize = size;
}

// ------------------------------------------------------------------------
// Gets the number of kinematics objects in the chain
// ------------------------------------------------------------------------
int vsInverseKinematics::getKinematicsChainSize()
{
    return dataArraySize;
}

// ------------------------------------------------------------------------
// Sets the kinematics object associated with a particular joint. The joint
// numbering is zero-based, with the 0th joint being the one closest to the
// origin of the system.
// ------------------------------------------------------------------------
void vsInverseKinematics::setKinematicsObject(int jointIdx,
    vsKinematics *kinematics)
{
    vsInvKinData *data;

    // Bounds checking
    if ((jointIdx < 0) || (jointIdx >= dataArraySize))
    {
        printf("vsInverseKinematics::setKinematicsObject: "
            "Invalid index (%d)\n", jointIdx);
        return;
    }
    
    // Object validity
    if (!(kinematics->isValidObject()))
    {
        printf("vsInverseKinematics::setKinematicsObject: "
            "Invalid kinematics object\n");
        return;
    }

    // Set the kinematics object on the desired joint
    data = (vsInvKinData *)(dataArray[jointIdx]);
    setDataKinematics(data, kinematics);
}

// ------------------------------------------------------------------------
// Gets the kinematics object associated with a particular joint. The joint
// numbering is zero-based, with the 0th joint being the one closest to the
// origin of the system.
// ------------------------------------------------------------------------
vsKinematics* vsInverseKinematics::getKinematicsObject(int jointIdx)
{
    vsInvKinData *data;

    // Bounds checking
    if ((jointIdx < 0) || (jointIdx >= dataArraySize))
    {
        printf("vsInverseKinematics::getKinematicsObject: "
            "Invalid index (%d)\n", jointIdx);
        return NULL;
    }

    // Get the kinematics object on the desired joint
    data = (vsInvKinData *)(dataArray[jointIdx]);
    return data->kin;
}

// ------------------------------------------------------------------------
// Sets the constraint angles for a particular joint. The joint numbering
// is zero-based, with the 0th joint being the one closest to the origin of
// the system. All angles are in degrees.
// ------------------------------------------------------------------------
void vsInverseKinematics::setConstraint(int jointIdx, int axisIdx,
    vsVector constraintAxis, double axisMin, double axisMax)
{
    vsInvKinData *data;

    // Bounds checking
    if ((jointIdx < 0) || (jointIdx >= dataArraySize))
    {
        printf("vsInverseKinematics::setKinematicsConstraints: "
            "Invalid joint index (%d)\n", jointIdx);
        return;
    }
    if ((axisIdx < 0) || (axisIdx > 2))
    {
        printf("vsInverseKinematics::setKinematicsConstraints: "
            "Invalid axis index (%d)\n", axisIdx);
        return;
    }

    // Set the constraint values for the joint
    data = (vsInvKinData *)(dataArray[jointIdx]);
    data->constraintAxis[axisIdx] = constraintAxis;
    data->axisMinValue[axisIdx] = axisMin;
    data->axisMaxValue[axisIdx] = axisMax;
}

// ------------------------------------------------------------------------
// Gets the constraint angles for a particular joint. The joint numbering
// is zero-based, with the 0th joint being the one closest to the origin of
// the system. All angles are in degrees. NULL may be passed in for any
// undesired return values.
// ------------------------------------------------------------------------
void vsInverseKinematics::getConstraint(int jointIdx, int axisIdx,
    vsVector *constraintAxis, double *axisMin, double *axisMax)
{
    vsInvKinData *data;

    // Bounds checking
    if ((jointIdx < 0) || (jointIdx >= dataArraySize))
    {
        printf("vsInverseKinematics::getKinematicsConstraints: "
            "Invalid index (%d)\n", jointIdx);
        return;
    }
    if ((axisIdx < 0) || (axisIdx > 2))
    {
        printf("vsInverseKinematics::getKinematicsConstraints: "
            "Invalid axis index (%d)\n", axisIdx);
        return;
    }

    // Get the constraint values for the joint
    data = (vsInvKinData *)(dataArray[jointIdx]);
    if (constraintAxis)
        *constraintAxis = data->constraintAxis[axisIdx];
    if (axisMin)
        *axisMin = data->axisMinValue[axisIdx];
    if (axisMax)
        *axisMax = data->axisMaxValue[axisIdx];
}

// ------------------------------------------------------------------------
// Sets the translation offset from the last kinematics joint to the end
// effector of the kinematics chain
// ------------------------------------------------------------------------
void vsInverseKinematics::setEndpointOffset(vsVector offset)
{
    endpointOffset.clearCopy(offset);
}

// ------------------------------------------------------------------------
// Gets the translation offset from the last kinematics joint to the end
// effector of the kinematics chain
// ------------------------------------------------------------------------
vsVector vsInverseKinematics::getEndpointOffset()
{
    return endpointOffset;
}

// ------------------------------------------------------------------------
// Sets the maximum number of iterations the inverse kinematics algorithm
// can perform before settling on a final answer
// ------------------------------------------------------------------------
void vsInverseKinematics::setMaxLoops(int loops)
{
    // Bounds checking
    if (loops < 1)
    {
        printf("vsInverseKinematics::setMaxLoops: "
            "Invalid iteration count (%d)\n", loops);
        return;
    }

    // Set the loop count
    maxProcessLoops = loops;
}

// ------------------------------------------------------------------------
// Gets the maximum number of iterations the inverse kinematics algorithm
// can perform before settling on a final answer
// ------------------------------------------------------------------------
int vsInverseKinematics::getMaxLoops()
{
    return maxProcessLoops;
}

// ------------------------------------------------------------------------
// Sets the 'good enough' threshold distance within which the inverse
// kinematics algorithm will terminate
// ------------------------------------------------------------------------
void vsInverseKinematics::setThreshold(double threshold)
{
    // Bounds check
    if (threshold < 0.0)
    {
        printf("vsInverseKinematics::setThreshold: "
            "Invalid threshold (%lf)\n", threshold);
        return;
    }

    // Set the success threshold
    successThreshold = threshold;
}

// ------------------------------------------------------------------------
// Gets the 'good enough' threshold distance within which the inverse
// kinematics algorithm will terminate
// ------------------------------------------------------------------------
double vsInverseKinematics::getThreshold()
{
    return successThreshold;
}

// ------------------------------------------------------------------------
// Sets the dampening constant for the inverse kinematics algorithm
// ------------------------------------------------------------------------
void vsInverseKinematics::setDampeningConstant(double constant)
{
    // Bounds check
    if ((constant < 0.0) || (constant > 1.0))
    {
        printf("vsInverseKinematics::setDampeningConstant: "
            "Invalid constant (%lf)\n", constant);
        return;
    }

    // Set the dampening constant
    dampeningConstant = constant;
}

// ------------------------------------------------------------------------
// Gets the dampening constant for the inverse kinematics algorithm
// ------------------------------------------------------------------------
double vsInverseKinematics::getDampeningConstant()
{
    return dampeningConstant;
}

// ------------------------------------------------------------------------
// Starts the inverse kinematics algorithm. Attempts to manipulate the
// rotations of the chain of kinematics objects such that the end effector
// of the object chain coincides as closely as possible with the specified
// point. Runs until the end effector is within the specified threshold
// distance of the target point, or until the maximum number of algorithm
// iterations has been reached.
// ------------------------------------------------------------------------
#define VS_KINEMATICS_PRIME_LOOPS 1

void vsInverseKinematics::reachForPoint(vsVector targetPoint)
{
    int iterationCount = 0;
    vsKinematics *endKinematics;
    vsVector currentEndpoint;
    double currentDistance;
    vsVector targetPt;

    vsInvKinData *data;
    vsMatrix tempMat;
    int loop;
    vsVector jointPoint;
    vsKinematics *jointKin;
    vsQuat tempQuat;

    vsVector jointToEndVec, jointToTargetVec;
    vsVector rotAxis;
    double rotAngle;
    vsQuat rotQuat;

    // Force the target point vector to be the correct size
    targetPt.setSize(3);
    targetPt.clearCopy(targetPoint);

    // Clear out the current rotations, for good measure
    for (loop = 0; loop < dataArraySize; loop++)
    {
        data = (vsInvKinData *)(dataArray[loop]);
        jointKin = data->kin;
        jointKin->setOrientation(vsQuat(0.0, 0.0, 0.0, 1.0));
    }

    // Get the kinematics object corresponding to the last joint in the chain
    data = (vsInvKinData *)(dataArray[dataArraySize - 1]);
    endKinematics = data->kin;

    // Prime the loop by computing the current end effector location and
    // distance to target
    tempMat = endKinematics->getComponent()->getGlobalXform();
    currentEndpoint = tempMat.getPointXform(endpointOffset + endKinematics->getCenterOfMass());
    currentDistance = (currentEndpoint - targetPt).getMagnitude();

    // Run the cyclic-coordinate descent algorithm. This algorithm adjusts
    // each joint angle individually, modifying the orientation so that the
    // vector from the joint's origin position to the current end effector
    // points in the direction of the targetPoint. This is an iterative
    // process, with each iteration of the algorithm improving the estimate.
    // The algorithm continues to iterate until it reaches a 'close enough'
    // end effector location, or runs out of iterations.
    // * Additionally, run the loop a small number of times to 'prime' it, by
    // computing the joint positions with the constraints disabled. Since this
    // algorithm has a tendency to compute a local maximum, we want the
    // starting point to be somewhat close to the desired result.
    while ((currentDistance > successThreshold) &&
        (iterationCount < (maxProcessLoops + VS_KINEMATICS_PRIME_LOOPS)))
    {
        // Run the algorithm once for each joint, starting with the joint
        // closest to the end effector
        for (loop = (dataArraySize - 1); loop >= 0; loop--)
        {
            // Get the kinematics object from the joint in question, and
            // use that to determine the location of the joint's origin
            data = (vsInvKinData *)(dataArray[loop]);
            jointKin = data->kin;
            tempMat = jointKin->getComponent()->getParent(0)->getGlobalXform();
            jointPoint = tempMat.getPointXform(jointKin->getCenterOfMass());

            // Calculate the direction vectors from the joint's origin to the
            // end effector and target point
            jointToEndVec = (currentEndpoint - jointPoint).getNormalized();
            jointToTargetVec = (targetPt - jointPoint).getNormalized();

            // Create the rotation quaternion that rotates the end effector
            // vector to the target point vector
            rotAxis = jointToEndVec.getCrossProduct(jointToTargetVec).getNormalized();
            rotAngle = VS_RAD2DEG(acos(jointToEndVec.getDotProduct(jointToTargetVec)));

            rotQuat.setAxisAngleRotation(rotAxis[0], rotAxis[1], rotAxis[2],
                rotAngle);

            // Transform this new rotation into the joint's coordinate system
            tempQuat.setMatrixRotation(tempMat);
            rotQuat = tempQuat.getConjugate() * rotQuat * tempQuat;

            // Modify the joint's orientation by this new rotation
            if (!VS_EQUAL(0.0, rotAngle))
                jointKin->preModifyOrientation(rotQuat);


            // Apply the joint's constraints to the new orientation
            // * Only do this if we're not 'priming' the chain
            if (iterationCount >= VS_KINEMATICS_PRIME_LOOPS)
            {
                // Get the total orientation from the kinematics object
                rotQuat = jointKin->getOrientation();

                // Add the dampening here while we're at it
                rotQuat = applyDampening(rotQuat, dampeningConstant);

                // Apply the joint constraints
                rotQuat = applyConstraints(loop, rotQuat);

                // Put the adjusted orientation back into the kinematics object
                jointKin->setOrientation(rotQuat);
            }

            // Recompute the location of the end effector
            tempMat = endKinematics->getComponent()->getGlobalXform();
            currentEndpoint = tempMat.getPointXform(endpointOffset + endKinematics->getCenterOfMass());
        }

        // Calculate the distance between the new location of the end effector
        // and the target point
        currentDistance = (currentEndpoint - targetPt).getMagnitude();

        // If we're 'priming' the chain, then artifically set the 'error'
        // distance to a value greater than the threshold
        if (iterationCount < VS_KINEMATICS_PRIME_LOOPS)
            currentDistance = successThreshold * 2.0;

        // One more iteration completed
        iterationCount++;
    }
}

// ------------------------------------------------------------------------
// Private function
// Creates one of the inverse kinematics data structures associated with a
// particular joint
// ------------------------------------------------------------------------
vsInvKinData *vsInverseKinematics::createData()
{
    vsInvKinData *data;
    int loop;

    // Allocate a new data structure
    data = new vsInvKinData;

    // Set the default values for the data structure
    data->kin = NULL;
    for (loop = 0; loop < 3; loop++)
    {
        // Default constraints axes are one for each axis: z, y, x
        data->constraintAxis[loop].set(0.0, 0.0, 0.0);
        data->constraintAxis[loop][2-loop] = 1.0;

        // Default constraint measures are non-constraining (full 360 degrees)
        data->axisMinValue[loop] = -180.0;
        data->axisMaxValue[loop] = 180.0;
    }

    return data;
}

// ------------------------------------------------------------------------
// Private function
// Sets the vsKinematics object on one of the inverse kinematics data
// structures
// ------------------------------------------------------------------------
void vsInverseKinematics::setDataKinematics(vsInvKinData *data,
    vsKinematics *kinematics)
{
    // Release the kinematics object if there is one
    if (data->kin)
        vsObject::unrefDelete(data->kin);

    // Set the new kinematics object and reference it
    data->kin = kinematics;
    if (data->kin)
        data->kin->ref();
}

// ------------------------------------------------------------------------
// Private function
// Destroys one of the inverse kinematics data structures
// ------------------------------------------------------------------------
void vsInverseKinematics::deleteData(vsInvKinData *data)
{
    // Release the kinematics object if there is one
    if (data->kin)
        vsObject::unrefDelete(data->kin);

    // Delete the data structure
    delete data;
}

// ------------------------------------------------------------------------
// Private function
// Clamps the degree measure 'value' to the range indicated by the
// minDegrees and maxDegrees parameters. The clamping takes into account
// the 'circular' nature of angle measures; a value far enough below the
// low end of the range might 'circle around' and be clamped to the high
// end of the range, and vice versa.
// ------------------------------------------------------------------------
double vsInverseKinematics::constrainAngle(double minDegrees,
    double maxDegrees, double value)
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
double vsInverseKinematics::calculateAxisRotation(vsQuat rotation, vsVector axis)
{
    vsVector planeVec, rotVec, axisVec;
    double dotProd, rotDegrees;

    // * Create a vector that is in the plane perpendicular to the axis vector

    // Start with an arbitrary vector
    planeVec.set(0.0, 1.0, 0.0);
    // The vector must not point in the same (or opposite) direction as the
    // axis vector; pick a new vector if that is the case
    dotProd = planeVec.getDotProduct(axis);
    if (VS_EQUAL(fabs(dotProd), 1.0))
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
    if (VS_EQUAL(fabs(dotProd), 1.0))
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

// ------------------------------------------------------------------------
// Private function
// Apply the constraints for the specified joint to the given rotation
// quaternion
// ------------------------------------------------------------------------
vsQuat vsInverseKinematics::applyConstraints(int jointIdx, vsQuat rotation)
{
    vsInvKinData *jointData;
    vsQuat currentRot, newRot;
    vsQuat resultRot;
    vsVector constraintAxis;
    double rotAngle;
    int loop;

    // Get a pointer to the joint's inverse kinematics data
    jointData = (vsInvKinData *)(dataArray[jointIdx]);

    // Initialize the rotation to pull from to the specified rotation
    currentRot = rotation;

    // Initilize the result to no rotation
    resultRot.set(0.0, 0.0, 0.0, 1.0);
    
    // Loop over all three constraints
    for (loop = 0; loop < 3; loop++)
        // Only apply a constraint if the axis is nonzero
        if (jointData->constraintAxis[loop].getMagnitude() > 0.0)
        {
            // Copy the constraint axis to a more convenient variable
            constraintAxis = jointData->constraintAxis[loop];

            // Determine how much the rotation rotates around the constraint
            // axis
            rotAngle = calculateAxisRotation(currentRot, constraintAxis);

            // Clamp the rotation around the constraint axis to the values
            // specified for this constraint
            rotAngle = constrainAngle(jointData->axisMinValue[loop],
                jointData->axisMaxValue[loop], rotAngle);

            // Compute the quaternion that represents the constrained rotation
            newRot.setAxisAngleRotation(constraintAxis[0], constraintAxis[1],
                constraintAxis[2], rotAngle);

            // Remove the constrained rotation from the input rotation...
            currentRot = newRot.getInverse() * currentRot;
            // ...and multiply it into the output rotation.
            resultRot = resultRot * newRot;
        }

    // Return the product of the constrained rotations
    return resultRot;
}

// ------------------------------------------------------------------------
// Private function
// Scale the amount of rotation of the specified rotation quaternion by
// one minus the specified fraction.
// ------------------------------------------------------------------------
vsQuat vsInverseKinematics::applyDampening(vsQuat rotation,
    double dampeningFraction)
{
    double x, y, z, theta;
    vsQuat result;

    // Break the rotation down into axis and angle
    rotation.getAxisAngleRotation(&x, &y, &z, &theta);

    // Reconstruct the rotation with the axis and scaled angle
    result.setAxisAngleRotation(x, y, z, theta * (1.0 - dampeningFraction));

    return result;
}
