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
//    VESS Module:  vs3TrackerArm.c++
//
//    Description:  Motion model that manipulates the three joints of a
//		    human figure's arm. Works with three motion trackers
//		    ideally mounted on the subject's back, upper arm,
//		    and hand.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vs3TrackerArm.h++"

// ------------------------------------------------------------------------
// Constructor - Stores the given pointers and initializes the joint
// offsets to zero.
// ------------------------------------------------------------------------
vs3TrackerArm::vs3TrackerArm(vsMotionTracker *backTracker,
                             vsKinematics *shoulderJoint,
                             vsMotionTracker *elbowTracker,
                             vsKinematics *elbowJoint,
                             vsMotionTracker *handTracker,
                             vsKinematics *wristJoint)
{
    // Store the traker pointers
    backTrack = backTracker;
    elbowTrack = elbowTracker;
    handTrack = handTracker;
    
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
vs3TrackerArm::~vs3TrackerArm()
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vs3TrackerArm::getClassName()
{
    return "vs3TrackerArm";
}

// ------------------------------------------------------------------------
// Sets the offset, in tracker space, from the back-mounted tracker to the
// shoulder joint
// ------------------------------------------------------------------------
void vs3TrackerArm::setShoulderOffset(vsVector newOffset)
{
    // Copy the offset from the specified vector, and force the resulting
    // vector to have a size of 3.
    shoulderOffset.clearCopy(newOffset);
    shoulderOffset.setSize(3);
}

// ------------------------------------------------------------------------
// Retrieves the shoulder joint offset
// ------------------------------------------------------------------------
vsVector vs3TrackerArm::getShoulderOffset()
{
    return shoulderOffset;
}

// ------------------------------------------------------------------------
// Sets the offset, in tracker space, from the elbow tracker to the elbow
// joint
// ------------------------------------------------------------------------
void vs3TrackerArm::setElbowOffset(vsVector newOffset)
{
    // Copy the offset from the specified vector, and force the resulting
    // vector to have a size of 3.
    elbowOffset.clearCopy(newOffset);
    elbowOffset.setSize(3);
}

// ------------------------------------------------------------------------
// Retrieves the elbow joint offset
// ------------------------------------------------------------------------
vsVector vs3TrackerArm::getElbowOffset()
{
    return elbowOffset;
}

// ------------------------------------------------------------------------
// Sets the offset, in tracker space, from the handheld tracker to the
// wrist joint
// ------------------------------------------------------------------------
void vs3TrackerArm::setWristOffset(vsVector newOffset)
{
    // Copy the offset from the specified vector, and force the resulting
    // vector to have a size of 3.
    wristOffset.clearCopy(newOffset);
    wristOffset.setSize(3);
}

// ------------------------------------------------------------------------
// Retrieves the wrist joint offset
// ------------------------------------------------------------------------
vsVector vs3TrackerArm::getWristOffset()
{
    return wristOffset;
}

// ------------------------------------------------------------------------
// Sets the pre-multiplied shoulder rotation offset
// ------------------------------------------------------------------------
void vs3TrackerArm::setShoulderPreRot(vsQuat rotQuat)
{
    shoulderPreRot = rotQuat;
}

// ------------------------------------------------------------------------
// Gets the pre-multiplied shoulder rotation offset
// ------------------------------------------------------------------------
vsQuat vs3TrackerArm::getShoulderPreRot()
{
    return shoulderPreRot;
}

// ------------------------------------------------------------------------
// Sets the post-multiplied shoulder rotation offset
// ------------------------------------------------------------------------
void vs3TrackerArm::setShoulderPostRot(vsQuat rotQuat)
{
    shoulderPostRot = rotQuat;
}

// ------------------------------------------------------------------------
// Gets the pre-multiplied shoulder rotation offset
// ------------------------------------------------------------------------
vsQuat vs3TrackerArm::getShoulderPostRot()
{
    return shoulderPostRot;
}

// ------------------------------------------------------------------------
// Sets the pre-multiplied elbow rotation offset
// ------------------------------------------------------------------------
void vs3TrackerArm::setElbowPreRot(vsQuat rotQuat)
{
    elbowPreRot = rotQuat;
}

// ------------------------------------------------------------------------
// Gets the pre-multiplied elbow rotation offset
// ------------------------------------------------------------------------
vsQuat vs3TrackerArm::getElbowPreRot()
{
    return elbowPreRot;
}

// ------------------------------------------------------------------------
// Sets the post-multiplied elbow rotation offset
// ------------------------------------------------------------------------
void vs3TrackerArm::setElbowPostRot(vsQuat rotQuat)
{
    elbowPostRot = rotQuat;
}

// ------------------------------------------------------------------------
// Gets the post-multiplied elbow rotation offset
// ------------------------------------------------------------------------
vsQuat vs3TrackerArm::getElbowPostRot()
{
    return elbowPostRot;
}

// ------------------------------------------------------------------------
// Sets the pre-multiplied wrist rotation offset
// ------------------------------------------------------------------------
void vs3TrackerArm::setWristPreRot(vsQuat rotQuat)
{
    wristPreRot = rotQuat;
}

// ------------------------------------------------------------------------
// Gets the pre-multiplied wrist rotation offset
// ------------------------------------------------------------------------
vsQuat vs3TrackerArm::getWristPreRot()
{
    return wristPreRot;
}

// ------------------------------------------------------------------------
// Sets the post-multiplied wrist rotation offset
// ------------------------------------------------------------------------
void vs3TrackerArm::setWristPostRot(vsQuat rotQuat)
{
    wristPostRot = rotQuat;
}

// ------------------------------------------------------------------------
// Gets the post-multiplied wrist rotation offset
// ------------------------------------------------------------------------
vsQuat vs3TrackerArm::getWristPostRot()
{
    return wristPostRot;
}

// ------------------------------------------------------------------------
// Updates the motion model by reading the current tracker data and
// computing rotations for the three joints of the avatar's arm
// ------------------------------------------------------------------------
void vs3TrackerArm::update()
{
    vsVector backPos, elbowPos, handPos;
    vsQuat backOri, elbowOri, handOri;
    
    vsVector shoulderPoint, elbowPoint, wristPoint;
    vsVector shoulderToElbowVec, elbowToWristVec;
    vsVector elbowToShoulderVec;
    vsVector forwardVec, upVec;
    double rotVal;
    
    vsQuat shoulderRot, elbowRot, wristRot;
    
    vsQuat coordFix;

    // * Determine where each of the person's joints is in real-space by
    // transforming each joint's tracker-to-joint offset by the current
    // orientation of the tracker, and then adding in the current tracker
    // translation.

    // Shoulder
    backPos = backTrack->getPositionVec();
    backOri = backTrack->getOrientationQuat();
    shoulderPoint = backOri.rotatePoint(shoulderOffset);
    shoulderPoint += backPos;

    // Elbow
    elbowPos = elbowTrack->getPositionVec();
    elbowOri = elbowTrack->getOrientationQuat();
    elbowPoint = elbowOri.rotatePoint(elbowOffset);
    elbowPoint += elbowPos;

    // Wrist
    handPos = handTrack->getPositionVec();
    handOri = handTrack->getOrientationQuat();
    wristPoint = handOri.rotatePoint(wristOffset);
    wristPoint += handPos;
    
    // * Compute the delta vectors. These vectors, which represent
    // translations from one joint to another, are used in the rotation
    // calculation process.
    shoulderToElbowVec = elbowPoint - shoulderPoint;
    elbowToWristVec = wristPoint - elbowPoint;
    elbowToShoulderVec = shoulderPoint - elbowPoint;

    // * Compute the shoulder rotation
    // The rotation is determined by using the vsQuat class'
    // setVecsRotation() function, which takes two pairs of vectors and
    // returns the rotation that rotates the first pair to the second.
    // In this case, the first pair are the default directions for the
    // arm, and the second pair are the current way the person's arm is
    // positioned, based on the tracker data.

    // The arm's standard direction is down, with the 'top' of it
    // aimed forward.
    forwardVec.set(0.0, 0.0, -1.0);
    forwardVec = backOri.rotatePoint(forwardVec);
    upVec.set(0.0, 1.0, 0.0);
    upVec = backOri.rotatePoint(upVec);

    // Find the shoulder rotation; since the orientation data from the
    // elbow tracker is unreliable, we have to guess as to the 'up'
    // direction of the arm. This guess takes the form of the vector from
    // the elbow to the wrist, which should give a usable indication of
    // the shoulder's roll.
    shoulderRot.setVecsRotation(forwardVec, upVec,
        shoulderToElbowVec, elbowToWristVec);

    // Transform the resulting rotation into the back tracker's
    // coordinate space
    shoulderRot = backOri.getInverse() * shoulderRot * backOri;

    // * Compute the elbow pitch
    // The elbow pitch is simply the angle between the vector from the
    // shoulder to the elbow and the one from the elbow to the wrist.
    rotVal = fabs(shoulderToElbowVec.getAngleBetween(elbowToWristVec));
    elbowRot.setAxisAngleRotation(1.0, 0.0, 0.0, rotVal);
    
    // * Compute the wrist rotation
    // For purposes of this computation, the hand's standard direction
    // is forward, with the top pointed up.
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
