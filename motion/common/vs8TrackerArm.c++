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
//    VESS Module:  vs8TrackerArm.c++
//
//    Description:  Motion model that manipulates the three joints of a
//		    human figure's arm. Works with eight position-only
//                  motion trackers ideally mounted on the subject's 
//                  upper and lower upper arm, upper and lower forearm, 
//                  left and right shoulders, and two on the hand.
//
//    Author(s):    Bryan Kline, David Smith
//
//------------------------------------------------------------------------

#include "vs8TrackerArm.h++"

// ------------------------------------------------------------------------
// Constructor - Stores the given pointers and initializes the joint
// offsets to zero.
// ------------------------------------------------------------------------
vs8TrackerArm::vs8TrackerArm(vsMotionTracker *lShoulderTracker,
                             vsMotionTracker *rShoulderTracker,
                             vsMotionTracker *upperArm1Tracker,
                             vsMotionTracker *upperArm2Tracker,
                             vsKinematics *shoulderJoint,
                             vsMotionTracker *foreArm1Tracker,
                             vsMotionTracker *foreArm2Tracker,
                             vsKinematics *elbowJoint,
                             vsMotionTracker *handTracker1,
                             vsMotionTracker *handTracker2,
                             vsKinematics *wristJoint)
{
    // Store the traker pointers
    this->lShoulderTracker = lShoulderTracker;
    this->rShoulderTracker = rShoulderTracker;
    this->upArm1Tracker = upperArm1Tracker; 
    this->upArm2Tracker = upperArm2Tracker;
    this->forearm1Tracker = foreArm1Tracker; 
    this->forearm2Tracker = foreArm2Tracker;
    this->handTracker1 = handTracker1;
    this->handTracker2 = handTracker2;
    
    // Store the kinematics object pointers
    shoulderKin = shoulderJoint;
    elbowKin = elbowJoint;
    wristKin = wristJoint;
    
    // Initialize the position offset values
    shoulderOffset.set(0.0, 0.0, 0.0);
    elbowOffset.set(0.0, 0.0, 0.0);
    wristOffset.set(0.0, 0.0, 0.0);

    // Initialize the rotation offset values
    shoulderPreRot.set(0.0, 0.0, 0.0, 1.0);
    shoulderPostRot.set(0.0, 0.0, 0.0, 1.0);
    elbowPreRot.set(0.0, 0.0, 0.0, 1.0);
    elbowPostRot.set(0.0, 0.0, 0.0, 1.0);
    wristPreRot.set(0.0, 0.0, 0.0, 1.0);
    wristPostRot.set(0.0, 0.0, 0.0, 1.0);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vs8TrackerArm::~vs8TrackerArm()
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vs8TrackerArm::getClassName()
{
    return "vs8TrackerArm";
}

// ------------------------------------------------------------------------
// Sets the offset, in tracker space, from the back-mounted tracker to the
// shoulder joint
// ------------------------------------------------------------------------
void vs8TrackerArm::setShoulderOffset(atVector newOffset)
{
    // Copy the offset from the specified vector, and force the resulting
    // vector to have a size of 3.
    shoulderOffset.clearCopy(newOffset);
    shoulderOffset.setSize(3);
}

// ------------------------------------------------------------------------
// Retrieves the shoulder joint offset
// ------------------------------------------------------------------------
atVector vs8TrackerArm::getShoulderOffset()
{
    return shoulderOffset;
}

// ------------------------------------------------------------------------
// Sets the offset, in tracker space, from the elbow tracker to the elbow
// joint
// ------------------------------------------------------------------------
void vs8TrackerArm::setElbowOffset(atVector newOffset)
{
    // Copy the offset from the specified vector, and force the resulting
    // vector to have a size of 3.
    elbowOffset.clearCopy(newOffset);
    elbowOffset.setSize(3);
}

// ------------------------------------------------------------------------
// Retrieves the elbow joint offset
// ------------------------------------------------------------------------
atVector vs8TrackerArm::getElbowOffset()
{
    return elbowOffset;
}

// ------------------------------------------------------------------------
// Sets the offset, in tracker space, from the handheld tracker to the
// wrist joint
// ------------------------------------------------------------------------
void vs8TrackerArm::setWristOffset(atVector newOffset)
{
    // Copy the offset from the specified vector, and force the resulting
    // vector to have a size of 3.
    wristOffset.clearCopy(newOffset);
    wristOffset.setSize(3);
}

// ------------------------------------------------------------------------
// Retrieves the wrist joint offset
// ------------------------------------------------------------------------
atVector vs8TrackerArm::getWristOffset()
{
    return wristOffset;
}

// ------------------------------------------------------------------------
// Sets the pre-multiplied shoulder rotation offset
// ------------------------------------------------------------------------
void vs8TrackerArm::setShoulderPreRot(atQuat rotQuat)
{
    shoulderPreRot = rotQuat;
}

// ------------------------------------------------------------------------
// Gets the pre-multiplied shoulder rotation offset
// ------------------------------------------------------------------------
atQuat vs8TrackerArm::getShoulderPreRot()
{
    return shoulderPreRot;
}

// ------------------------------------------------------------------------
// Sets the post-multiplied shoulder rotation offset
// ------------------------------------------------------------------------
void vs8TrackerArm::setShoulderPostRot(atQuat rotQuat)
{
    shoulderPostRot = rotQuat;
}

// ------------------------------------------------------------------------
// Gets the pre-multiplied shoulder rotation offset
// ------------------------------------------------------------------------
atQuat vs8TrackerArm::getShoulderPostRot()
{
    return shoulderPostRot;
}

// ------------------------------------------------------------------------
// Sets the pre-multiplied elbow rotation offset
// ------------------------------------------------------------------------
void vs8TrackerArm::setElbowPreRot(atQuat rotQuat)
{
    elbowPreRot = rotQuat;
}

// ------------------------------------------------------------------------
// Gets the pre-multiplied elbow rotation offset
// ------------------------------------------------------------------------
atQuat vs8TrackerArm::getElbowPreRot()
{
    return elbowPreRot;
}

// ------------------------------------------------------------------------
// Sets the post-multiplied elbow rotation offset
// ------------------------------------------------------------------------
void vs8TrackerArm::setElbowPostRot(atQuat rotQuat)
{
    elbowPostRot = rotQuat;
}

// ------------------------------------------------------------------------
// Gets the post-multiplied elbow rotation offset
// ------------------------------------------------------------------------
atQuat vs8TrackerArm::getElbowPostRot()
{
    return elbowPostRot;
}

// ------------------------------------------------------------------------
// Sets the pre-multiplied wrist rotation offset
// ------------------------------------------------------------------------
void vs8TrackerArm::setWristPreRot(atQuat rotQuat)
{
    wristPreRot = rotQuat;
}

// ------------------------------------------------------------------------
// Gets the pre-multiplied wrist rotation offset
// ------------------------------------------------------------------------
atQuat vs8TrackerArm::getWristPreRot()
{
    return wristPreRot;
}

// ------------------------------------------------------------------------
// Sets the post-multiplied wrist rotation offset
// ------------------------------------------------------------------------
void vs8TrackerArm::setWristPostRot(atQuat rotQuat)
{
    wristPostRot = rotQuat;
}

// ------------------------------------------------------------------------
// Gets the post-multiplied wrist rotation offset
// ------------------------------------------------------------------------
atQuat vs8TrackerArm::getWristPostRot()
{
    return wristPostRot;
}

// ------------------------------------------------------------------------
// Updates the motion model by reading the current tracker data and
// computing rotations for the three joints of the avatar's arm
// ------------------------------------------------------------------------
void vs8TrackerArm::update()
{
    atVector backPos, elbowPos, handPos;
    atQuat backOri, elbowOri, handOri;
    
    atVector shoulderToElbowVec, elbowToWristVec, elbowToShoulderVec;
    atVector forwardVec, upVec, bodyForwardVec;
    atVector lShoulderFloor;
    atVector lShoulderPos, rShoulderPos;
    atVector handForward, handRight, handMidPt, handUp;
    double rotVal;
    
    atQuat shoulderRot, elbowRot, wristRot, bodyOri;
    
    atQuat coordFix;

    // Get the forward vector for the entire body (at shoulders)...
    lShoulderPos = lShoulderTracker->getPositionVec();
    rShoulderPos = rShoulderTracker->getPositionVec();
    lShoulderFloor = lShoulderPos;
    lShoulderFloor[AT_Z] = 0.0;
    bodyForwardVec = (rShoulderPos - lShoulderPos).
        getCrossProduct(lShoulderFloor - lShoulderPos);
    bodyOri.setVecsRotation(atVector(0.0, 1.0, 0.0), atVector(0.0, 0.0, 1.0),
        bodyForwardVec, atVector(0.0, 0.0, 1.0));
    

    // Get the hand orientation (Note: hand tracker #1 is to left of #2)
    handRight = handTracker2->getPositionVec() - 
        handTracker1->getPositionVec();
    handMidPt = handTracker1->getPositionVec() + handRight.getScaled(.5); 
    handForward = handMidPt - forearm2Tracker->getPositionVec();
    handUp = handRight.getScaled(-1.0);
    handOri.setVecsRotation(atVector(0.0, 1.0, 0.0), atVector(0.0, 0.0, 1.0),
        handForward, handUp); 
    

    // * Compute the delta vectors. These vectors, which represent
    // translations from one joint to another, are used in the rotation
    // calculation process.
    shoulderToElbowVec = 
        upArm2Tracker->getPositionVec() - upArm1Tracker->getPositionVec();
    elbowToWristVec = 
        forearm2Tracker->getPositionVec() - forearm1Tracker->getPositionVec();
    elbowToShoulderVec = 
        upArm1Tracker->getPositionVec() - upArm2Tracker->getPositionVec();

    // * Compute the shoulder rotation
    // The rotation is determined by using the atQuat class'
    // setVecsRotation() function, which takes two pairs of vectors and
    // returns the rotation that rotates the first pair to the second.
    // In this case, the first pair are the default directions for the
    // arm, and the second pair are the current way the person's arm is
    // positioned, based on the tracker data.

    // The arm's standard direction is down, with the 'top' of it
    // aimed forward.
    forwardVec.set(0.0, 0.0, -1.0);
    forwardVec = bodyOri.rotatePoint(forwardVec);
    upVec.set(0.0, 1.0, 0.0);
    upVec = bodyOri.rotatePoint(upVec);

    // Find the shoulder rotation; since the orientation data from the
    // elbow tracker is unreliable, we have to guess as to the 'up'
    // direction of the arm. This guess takes the form of the vector from
    // the elbow to the wrist, which should give a usable indication of
    // the shoulder's roll.
    shoulderRot.setVecsRotation(forwardVec, upVec,
        shoulderToElbowVec, elbowToWristVec);

    // Transform the resulting rotation into the body's (shoulders')
    // coordinate space
    shoulderRot = bodyOri.getInverse() * shoulderRot * bodyOri;

    // * Compute the elbow pitch
    // The elbow pitch is simply the angle between the vector from the
    // shoulder to the elbow and the one from the elbow to the wrist.
    rotVal = fabs(shoulderToElbowVec.getAngleBetween(elbowToWristVec));
    elbowRot.setAxisAngleRotation(1.0, 0.0, 0.0, rotVal);
    
    // * Compute the wrist rotation
    // For purposes of this computation, the hand's standard direction
    forwardVec.set(0.0, 1.0, 0.0);
    forwardVec = handOri.rotatePoint(forwardVec);
    upVec.set(0.0, 0.0, 1.0);
    upVec = handOri.rotatePoint(upVec);
    
    // Find the wrist rotation. Again, since the elbow orientation isn't
    // usable, the vector from the elbow to the shoulder should make a
    // good guess as to the 'up' direction. Technically, here we're
    // computing the rotation from the _hand_ to the _arm_, which is the
    // reverse of what we want.
    wristRot.setVecsRotation(forwardVec, upVec,
        elbowToWristVec, elbowToShoulderVec);

    // Transform into the _hand's_ coordinate space.
    wristRot = handOri.getInverse() * wristRot * handOri;

    // Invert the rotation; this will give us the rotation from the
    // arm to the hand, which is what we wanted in the first place.
    wristRot.invert();

    // Correct for the model; the way the avatar is set up now, looking
    // down the arm means looking in the -Z direction, with Y up. But
    // the wrist rotation calculations were done with Y forward and Z up.
    // This coordinate space transform compensates.
    coordFix.setAxisAngleRotation(1.0, 0.0, 0.0, 90.0);
    wristRot = coordFix.getInverse() * wristRot * coordFix;
    
    // Apply the rotation offsets
    shoulderRot = shoulderPostRot * shoulderRot * shoulderPreRot;
    elbowRot = elbowPostRot * elbowRot * elbowPreRot;
    wristRot = wristPostRot * wristRot * wristPreRot;

    // * Apply the results
    shoulderKin->setOrientation(shoulderRot);
    elbowKin->setOrientation(elbowRot);
    wristKin->setOrientation(wristRot);
}
