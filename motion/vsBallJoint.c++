// File vsBallJoint.c++

#include "vsBallJoint.h++"

vsBallJoint::vsBallJoint(vsMotionTracker *shoulderTracker,
                         vsVector shoulderJointOffset,
                         vsMotionTracker *elbowTracker,
                         vsVector elbowJointOffset,
                         vsQuat elbowRotOffset,
			 vsQuat originXform)
{
    upperTracker = shoulderTracker;
    lowerTracker = elbowTracker;
    upperJointOffset = shoulderJointOffset;
    lowerJointOffset = elbowJointOffset;
    lowerTrackerRotOffset = elbowRotOffset;
    directionXform = originXform;
    
    lastResult.set(0, 0, 0, 1);
}

vsVecQuat vsBallJoint::update()
{
    vsQuat upperOrientQuat, lowerOrientQuat;
    vsVector upperTrackerPos, lowerTrackerPos;
    vsVector upperJointPos, lowerJointPos;
    vsVector armTrueDir, armForwardDir;
    vsVector upTrueDir, upCalcDir;
    vsVector rotAxis;
    double rotAngle;
    vsQuat resultQuat, rollQuat;
    double dotProd;
    vsVector componentVec;
    vsQuat lastInverseQuat;
    vsVecQuat result;

    // * First, find the points associated with each joint
    upperOrientQuat = upperTracker->getOrientationQuat();
    upperTrackerPos = upperTracker->getPositionVec();
    upperJointPos = upperOrientQuat.rotatePoint(upperJointOffset);
    upperJointPos += upperTrackerPos;

    lowerOrientQuat = lowerTracker->getOrientationQuat();
    lowerTrackerPos = lowerTracker->getPositionVec();
    lowerJointPos = lowerOrientQuat.rotatePoint(lowerJointOffset);
    lowerJointPos += lowerTrackerPos;

    // * Second, compute the rotation that rotates the joint to
    // match the direction between the two joint points
    armTrueDir = lowerJointPos - upperJointPos;
    armTrueDir.setSize(3);
    armTrueDir.normalize();

    // Find the origin vector to rotate from: 'forward' multiplied by the
    // origin transform passed in the constructor
    armForwardDir.set(0.0, 1.0, 0.0);
    armForwardDir = directionXform.rotatePoint(armForwardDir);

    rotAxis = armForwardDir.getCrossProduct(armTrueDir);
    rotAngle = armForwardDir.getAngleBetween(armTrueDir);
    resultQuat.setAxisAngleRotation(rotAxis[0], rotAxis[1], rotAxis[2],
	rotAngle);

    // * Third, correct the 'roll' component of the arm, using the
    // orientation data from the elbow tracker
    upCalcDir.set(0.0, 0.0, 1.0);
    upCalcDir = directionXform.rotatePoint(upCalcDir);
    upCalcDir = resultQuat.rotatePoint(upCalcDir);
    dotProd = armTrueDir.getDotProduct(upCalcDir);
    componentVec = upCalcDir * dotProd;
    upCalcDir -= componentVec;
    upCalcDir.normalize();
    
    upTrueDir.set(0.0, 0.0, 1.0);
    upTrueDir = directionXform.rotatePoint(upTrueDir);
    upTrueDir = lowerOrientQuat.rotatePoint(upTrueDir);
    upTrueDir = lowerTrackerRotOffset.rotatePoint(upTrueDir);
    dotProd = armTrueDir.getDotProduct(upTrueDir);
    componentVec = upTrueDir * dotProd;
    upTrueDir -= componentVec;
    upTrueDir.normalize();
    
    rotAxis = upCalcDir.getCrossProduct(upTrueDir);
    rotAngle = upCalcDir.getAngleBetween(upTrueDir);
    rollQuat.setAxisAngleRotation(rotAxis[0], rotAxis[1], rotAxis[2],
	rotAngle);

    // * Compute the final result and finish
    resultQuat = rollQuat * resultQuat;
    
    lastInverseQuat = lastResult;
    lastInverseQuat.conjugate();
    lastResult = resultQuat;
    
    result.vector.clear();
    result.quat = resultQuat * lastInverseQuat;

    return result;
}
