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

vsMatrix vsBallJoint::update()
{
    vsQuat upperOrientQuat, lowerOrientQuat;
    vsVector upperTrackerPos, lowerTrackerPos;
    vsVector upperJointPos, lowerJointPos;
    vsVector armTrueDir, armForwardDir;
    vsVector upTrueDir, upCalcDir;
    vsVector rotAxis;
    double rotAngle;
    vsQuat resultQuat, rollQuat;
    vsQuat inverseQuat;
    double dotProd;
    vsVector componentVec;
    vsQuat lastInverseQuat;
    vsMatrix resultMat;

    // * First, find the points associated with each joint
    upperOrientQuat = upperTracker->getOrientationQuat();
    upperTrackerPos = upperTracker->getPositionVec();
    upperJointPos = upperOrientQuat.rotatePoint(upperJointOffset);
    upperJointPos += upperTrackerPos;
    printf("upperOrientQuat: %lf %lf %lf %lf\n", upperOrientQuat[0],
	upperOrientQuat[1], upperOrientQuat[2], upperOrientQuat[3]);
    printf("upperJointPos: %lf %lf %lf\n", upperJointPos[0],
	upperJointPos[1], upperJointPos[2]);

    lowerOrientQuat = lowerTracker->getOrientationQuat();
    lowerTrackerPos = lowerTracker->getPositionVec();
    lowerJointPos = lowerOrientQuat.rotatePoint(lowerJointOffset);
    lowerJointPos += lowerTrackerPos;
    printf("lowerOrientQuat: %lf %lf %lf %lf\n", lowerOrientQuat[0],
	lowerOrientQuat[1], lowerOrientQuat[2], lowerOrientQuat[3]);
    printf("lowerJointPos: %lf %lf %lf\n", lowerJointPos[0],
	lowerJointPos[1], lowerJointPos[2]);

    // * Second, compute the rotation that rotates the arm to
    // match the direction between the two joint points
    armTrueDir = lowerJointPos - upperJointPos;
    printf("armTrueDir (before): %lf %lf %lf\n", armTrueDir[0], armTrueDir[1],
	armTrueDir[2]);
    inverseQuat = upperOrientQuat;
    inverseQuat.conjugate();
    armTrueDir = inverseQuat.rotatePoint(armTrueDir);
    armTrueDir.setSize(3);
    armTrueDir.normalize();
    printf("armTrueDir (after): %lf %lf %lf\n", armTrueDir[0], armTrueDir[1],
	armTrueDir[2]);

    // Find the origin vector to rotate from: 'forward' multiplied by the
    // origin transform passed in the constructor
    armForwardDir.set(0.0, 1.0, 0.0);
    armForwardDir = directionXform.rotatePoint(armForwardDir);
    printf("armForwardDir: %lf %lf %lf\n", armForwardDir[0], armForwardDir[1],
	armForwardDir[2]);
//    armForwardDir = upperOrientQuat.rotatePoint(armForwardDir);

    // Compute the rotation
    rotAxis = armForwardDir.getCrossProduct(armTrueDir);
    rotAngle = armForwardDir.getAngleBetween(armTrueDir);
    printf("rotation axis & angle: %lf %lf %lf %lf\n", rotAxis[0], rotAxis[1],
	rotAxis[2], rotAngle);
    resultQuat.setAxisAngleRotation(rotAxis[0], rotAxis[1], rotAxis[2],
	rotAngle);
    printf("resultQuat (1): %lf %lf %lf %lf\n", resultQuat[0], resultQuat[1],
	resultQuat[2], resultQuat[3]);

    // Correct for the current orientation of the back tracker
//    inverseQuat = upperOrientQuat;
//    inverseQuat.conjugate();
//    resultQuat = inverseQuat * resultQuat;

    // * Third, correct the 'roll' component of the arm, using the
    // orientation data from the elbow tracker
    upCalcDir.set(0.0, 0.0, 1.0);
    upCalcDir = directionXform.rotatePoint(upCalcDir);
//    upCalcDir = upperOrientQuat.rotatePoint(upCalcDir);
    printf("upCalcDir (0): %lf %lf %lf\n", upCalcDir[0], upCalcDir[1],
	upCalcDir[2]);
    upCalcDir = resultQuat.rotatePoint(upCalcDir);
    printf("upCalcDir (1): %lf %lf %lf\n", upCalcDir[0], upCalcDir[1],
	upCalcDir[2]);
    dotProd = armTrueDir.getDotProduct(upCalcDir);
    componentVec = upCalcDir * dotProd;
    upCalcDir -= componentVec;
    upCalcDir.normalize();
    printf("upCalcDir (2): %lf %lf %lf\n", upCalcDir[0], upCalcDir[1],
	upCalcDir[2]);
    
    upTrueDir.set(0.0, 0.0, 1.0);
    upTrueDir = directionXform.rotatePoint(upTrueDir);
    printf("upTrueDir (0): %lf %lf %lf\n", upTrueDir[0], upTrueDir[1],
	upTrueDir[2]);
    upTrueDir = lowerTrackerRotOffset.rotatePoint(upTrueDir);
    printf("upTrueDir (1): %lf %lf %lf\n", upTrueDir[0], upTrueDir[1],
	upTrueDir[2]);
    upTrueDir = lowerOrientQuat.rotatePoint(upTrueDir);
    printf("upTrueDir (2): %lf %lf %lf\n", upTrueDir[0], upTrueDir[1],
	upTrueDir[2]);
    dotProd = armTrueDir.getDotProduct(upTrueDir);
    componentVec = upTrueDir * dotProd;
    upTrueDir -= componentVec;
    upTrueDir.normalize();
    printf("upTrueDir (3): %lf %lf %lf\n", upTrueDir[0], upTrueDir[1],
	upTrueDir[2]);
    
    rotAxis = upCalcDir.getCrossProduct(upTrueDir);
    rotAngle = upCalcDir.getAngleBetween(upTrueDir);
    printf("rotation axis & angle: %lf %lf %lf %lf\n", rotAxis[0], rotAxis[1],
	rotAxis[2], rotAngle);
    rollQuat.setAxisAngleRotation(rotAxis[0], rotAxis[1], rotAxis[2],
	rotAngle);
    printf("rollQuat: %lf %lf %lf %lf\n", rollQuat[0], rollQuat[1],
	rollQuat[2], rollQuat[3]);

    // * Compute the final result and finish
    resultQuat = rollQuat * resultQuat;
    printf("resultQuat (2): %lf %lf %lf %lf\n\n", resultQuat[0], resultQuat[1],
	resultQuat[2], resultQuat[3]);
    
    lastInverseQuat = lastResult;
    lastInverseQuat.conjugate();
    lastResult = resultQuat;
    
    resultQuat = resultQuat * lastInverseQuat;
    resultMat.setQuatRotation(resultQuat);

    return resultMat;
}
