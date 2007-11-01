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
vsInverseKinematics::vsInverseKinematics() : kinematicsArray(1, 1)
{
    // The default kinematics chain size is one
    kinematicsArraySize = 0;
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
    vsKinematics *kin;

    // Release all of the kinematics objects
    for (loop = 0; loop < kinematicsArraySize; loop++)
    {
        kin = (vsKinematics *)(kinematicsArray[loop]);
        if (kin)
            vsObject::unrefDelete(kin);
    }
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

    // Resize the kinematics array to match the desired size
    kinematicsArray.setSize(size);

    // If we are growing the array, fill in the new spaces with NULL values
    if (size > kinematicsArraySize)
        for (loop = kinematicsArraySize; loop < size; loop++)
            kinematicsArray[loop] = NULL;

    // Record the new array size
    kinematicsArraySize = size;
}

// ------------------------------------------------------------------------
// Gets the number of kinematics objects in the chain
// ------------------------------------------------------------------------
int vsInverseKinematics::getKinematicsChainSize()
{
    return kinematicsArraySize;
}

// ------------------------------------------------------------------------
// Sets the kinematics object associated with a particular joint. The joint
// numbering is zero-based, with the 0th joint being the one closest to the
// origin of the system.
// ------------------------------------------------------------------------
void vsInverseKinematics::setKinematicsObject(int jointIdx,
    vsKinematics *kinematics)
{
    // Bounds checking
    if ((jointIdx < 0) || (jointIdx >= kinematicsArraySize))
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

    // Release the previous kinematics object, if any
    if (kinematicsArray[jointIdx])
        vsObject::unrefDelete((vsKinematics *)(kinematicsArray[jointIdx]));

    // Set the kinematics object on the desired joint, and reference it
    kinematicsArray[jointIdx] = kinematics;
    if (kinematics)
        kinematics->ref();
}

// ------------------------------------------------------------------------
// Gets the kinematics object associated with a particular joint. The joint
// numbering is zero-based, with the 0th joint being the one closest to the
// origin of the system.
// ------------------------------------------------------------------------
vsKinematics* vsInverseKinematics::getKinematicsObject(int jointIdx)
{
    // Bounds checking
    if ((jointIdx < 0) || (jointIdx >= kinematicsArraySize))
    {
        printf("vsInverseKinematics::getKinematicsObject: "
            "Invalid index (%d)\n", jointIdx);
        return NULL;
    }

    // Get the kinematics object on the desired joint
    return (vsKinematics *)(kinematicsArray[jointIdx]);
}

// ------------------------------------------------------------------------
// Sets the translation offset from the last kinematics joint to the end
// effector of the kinematics chain
// ------------------------------------------------------------------------
void vsInverseKinematics::setEndpointOffset(atVector offset)
{
    endpointOffset.clearCopy(offset);
}

// ------------------------------------------------------------------------
// Gets the translation offset from the last kinematics joint to the end
// effector of the kinematics chain
// ------------------------------------------------------------------------
atVector vsInverseKinematics::getEndpointOffset()
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

void vsInverseKinematics::reachForPoint(atVector targetPoint)
{
    int iterationCount = 0;
    vsKinematics *endKinematics;
    atVector currentEndpoint;
    double currentDistance;
    atVector targetPt;

    atMatrix tempMat;
    int loop;
    atVector jointPoint;
    vsKinematics *jointKin;
    atQuat tempQuat;

    atVector jointToEndVec, jointToTargetVec;
    atVector rotAxis;
    double rotAngle;
    atQuat rotQuat;

    // Force the target point vector to be the correct size
    targetPt.setSize(3);
    targetPt.clearCopy(targetPoint);

    // Clear out the current rotations, for good measure
    for (loop = 0; loop < kinematicsArraySize; loop++)
    {
        jointKin = (vsKinematics *)(kinematicsArray[loop]);
        jointKin->setOrientation(atQuat(0.0, 0.0, 0.0, 1.0));
    }

    // Get the kinematics object corresponding to the last joint in the chain
    endKinematics = (vsKinematics *)(kinematicsArray[kinematicsArraySize - 1]);

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
        for (loop = (kinematicsArraySize - 1); loop >= 0; loop--)
        {
            // Get the kinematics object from the joint in question, and
            // use that to determine the location of the joint's origin
            jointKin = (vsKinematics *)(kinematicsArray[loop]);
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

            // Apply the dampening to the joint's orientation
            rotQuat = jointKin->getOrientation();
            rotQuat = applyDampening(rotQuat, dampeningConstant);
            jointKin->setOrientation(rotQuat);

            // Apply the joint's constraints to the new orientation
            // * Only do this if we're not 'priming' the chain
            if (iterationCount >= VS_KINEMATICS_PRIME_LOOPS)
                jointKin->applyConstraints();

            // Recompute the location of the end effector
            tempMat = endKinematics->getComponent()->getGlobalXform();
            currentEndpoint = tempMat.getPointXform(endpointOffset + endKinematics->getCenterOfMass());
        }

        // Calculate the distance between the new location of the end effector
        // and the target point
        currentDistance = (currentEndpoint - targetPt).getMagnitude();

        // If we're 'priming' the chain, then artificially set the 'error'
        // distance to a value greater than the threshold
        if (iterationCount < VS_KINEMATICS_PRIME_LOOPS)
            currentDistance = successThreshold * 2.0;

        // One more iteration completed
        iterationCount++;
    }
}

// ------------------------------------------------------------------------
// Private function
// Scale the amount of rotation of the specified rotation quaternion by
// one minus the specified fraction.
// ------------------------------------------------------------------------
atQuat vsInverseKinematics::applyDampening(atQuat rotation,
    double dampeningFraction)
{
    double x, y, z, theta;
    atQuat result;

    // Break the rotation down into axis and angle
    rotation.getAxisAngleRotation(&x, &y, &z, &theta);

    // Reconstruct the rotation with the axis and scaled angle
    result.setAxisAngleRotation(x, y, z, theta * (1.0 - dampeningFraction));

    return result;
}
