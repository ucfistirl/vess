// File vs3TrackerArm.c++

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
    backTrack = backTracker;
    elbowTrack = elbowTracker;
    handTrack = handTracker;
    
    shoulderKin = shoulderJoint;
    elbowKin = elbowJoint;
    wristKin = wristJoint;
    
    shoulderOffset.set(0.0, 0.0, 0.0);
    elbowOffset.set(0.0, 0.0, 0.0);
    wristOffset.set(0.0, 0.0, 0.0);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vs3TrackerArm::~vs3TrackerArm()
{
}

// ------------------------------------------------------------------------
// Sets the offset, in tracker space, from the back-mounted tracker to the
// shoulder joint
// ------------------------------------------------------------------------
void vs3TrackerArm::setShoulderOffset(vsVector newOffset)
{
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

    // * Compute the locations of the joints
    backPos = backTrack->getPositionVec();
    backOri = backTrack->getOrientationQuat();
    shoulderPoint = backOri.rotatePoint(shoulderOffset);
    shoulderPoint += backPos;

    elbowPos = elbowTrack->getPositionVec();
    elbowOri = elbowTrack->getOrientationQuat();
    elbowPoint = elbowOri.rotatePoint(elbowOffset);
    elbowPoint += elbowPos;

    handPos = handTrack->getPositionVec();
    handOri = handTrack->getOrientationQuat();
    wristPoint = handOri.rotatePoint(wristOffset);
    wristPoint += handPos;
    
    // * Compute the delta vectors
    shoulderToElbowVec = elbowPoint - shoulderPoint;
    elbowToWristVec = wristPoint - elbowPoint;
    elbowToShoulderVec = shoulderPoint - elbowPoint;

    // * Compute the shoulder rotation
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

    // Correct for the model; in the 'zero' position, the avatar's arm
    // isn't quite pointed straight down but is rather about ten degrees
    // out to the side. This extra rotation should compensate.
    coordFix.setAxisAngleRotation(0.0, 1.0, 0.0, -10.0);
    shoulderRot = coordFix * shoulderRot;

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
    
    // Correct for the tracker mounting; the tracker mounted on the
    // joystick handle doesn't point exactly the same way the arm does.
    // Instead, it's pitched down a bit. This should compensate.
    coordFix.setAxisAngleRotation(1.0, 0.0, 0.0, 10.0);
    wristRot = wristRot * coordFix;

    // * Apply the results
    shoulderKin->setOrientation(shoulderRot);
    elbowKin->setOrientation(elbowRot);
    wristKin->setOrientation(wristRot);
}
