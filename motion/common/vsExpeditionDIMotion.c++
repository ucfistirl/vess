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
//    VESS Module:  vsExpeditionDIMotion.c++
//
//    Description:  Motion model for the Quantum3D ExpeditionDI system.
//                  The ExpeditionDI is a wearable immersive VR system,
//                  consisting of a tracked HMD and surrogate weapon.
//                  The purpose of this motion model is to coordinate the
//                  measurements of the ExpeditionDI's 3 InertiaCube
//                  trackers.  The tracker hardware isn't as significant
//                  as how they are typically affixed to the user (head,
//                  leg, and weapon).  These orientation measurements
//                  are converted into orientations for the three
//                  kinematics objects (root, head, and weapon).
//                  There is also support for the ExpeditionDI's weapon-
//                  mounted joystick and buttons.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsExpeditionDIMotion.h++"
#include "vsTimer.h++"

// ------------------------------------------------------------------------
// Constructs a vsExpeditionDIMotion model.  This requires three trackers
// (head, leg, and weapon), a joystick for lateral movement, and three
// kinematics (root, head, and weapon)
// ------------------------------------------------------------------------
vsExpeditionDIMotion::vsExpeditionDIMotion(vsMotionTracker *headTrkr,
                                           vsMotionTracker *legTrkr,
                                           vsMotionTracker *weaponTrkr,
                                           vsJoystick *stick,
                                           vsKinematics *root,
                                           vsKinematics *head,
                                           vsKinematics *weapon)
                    : vsMotionModel()
{
    // Reference the trackers
    headTracker = headTrkr;
    headTracker->ref();
    legTracker = legTrkr;
    legTracker->ref();
    weaponTracker = weaponTrkr;
    weaponTracker->ref();

    // Reference the joystick
    joystick = stick;
    joystick->ref();

    // And finally, the kinematics
    rootKin = root;
    rootKin->ref();
    headKin = head;
    headKin->ref();
    weaponKin = weapon;
    weaponKin->ref();

    // By default, the leg tracker is affixed to the left leg
    trackedLeg = VS_EDI_LEFT_LEG;

    // Set the default pitch angles for standing and kneeling
    standThreshold = VS_EDI_STAND_THRESHOLD;
    kneelThreshold = VS_EDI_KNEEL_THRESHOLD;

    // We're not kneeling by default
    kneeling = false;
}

// ------------------------------------------------------------------------
// Destroys a vsExpeditionDIMotion model
// ------------------------------------------------------------------------
vsExpeditionDIMotion::~vsExpeditionDIMotion()
{
    // Unreference all input devices and kinematics
    vsObject::unrefDelete(headTracker);
    vsObject::unrefDelete(legTracker);
    vsObject::unrefDelete(weaponTracker);
    vsObject::unrefDelete(joystick);
    vsObject::unrefDelete(rootKin);
    vsObject::unrefDelete(headKin);
    vsObject::unrefDelete(weaponKin);
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsExpeditionDIMotion::getClassName()
{
    return "vsExpeditionDIMotion";
}

// ------------------------------------------------------------------------
// Changes to which leg the leg tracker is attached.  This affects the
// root orientation and in which direction the pitch angle is measured
// ------------------------------------------------------------------------
void vsExpeditionDIMotion::setTrackedLeg(vsExpDITrackedLeg newLeg)
{
    trackedLeg = newLeg;
}

// ------------------------------------------------------------------------
// Returns the current setting for the tracked leg
// ------------------------------------------------------------------------
vsExpDITrackedLeg vsExpeditionDIMotion::getTrackedLeg()
{
    return trackedLeg;
}

// ------------------------------------------------------------------------
// Sets the threshold pitch angles for kneeling and standing.  By default
// the kneeling angle is set higher than the standing angle to prevent
// "posture-fighting" when the tracker is near the cutoff for one pose or
// the other.  The standing angle must be less than or equal to the
// kneeling angle
// ------------------------------------------------------------------------
void vsExpeditionDIMotion::setKneelThresholds(double newStand, double newKneel)
{
    // Validate the angles
    if ((newStand < 0.0) || (newStand > 90.0) ||
        (newKneel < 0.0) || (newKneel > 90.0))
    {
        printf("vsExpeditionDIMotion::setKneelThresholds:  Invalid angle "
            "specified\n");
        return;
    }

    // Make sure the stand angle is less than or equal to the kneel angle
    if (newStand > newKneel)
    {
        printf("vsExpeditionDIMotion::setKneelThresholds:  Stand angle "
            "is greater than kneel angle, ignoring new thresholds\n");
        return;
    }

    // Set the new thresholds
    standThreshold = newStand;
    kneelThreshold = newKneel;
}

// ------------------------------------------------------------------------
// Return the current standing and kneeling thresholds
// ------------------------------------------------------------------------
void vsExpeditionDIMotion::getKneelThresholds(double *stand, double *kneel)
{
    // Return the current angles if valid pointers were supplied
    if (stand != NULL)
        *stand = standThreshold;
    if (kneel != NULL)
        *kneel = kneelThreshold;
}

// ------------------------------------------------------------------------
// Return whether or not the tracked leg is currently in a kneeling pose
// ------------------------------------------------------------------------
bool vsExpeditionDIMotion::isKneeling()
{
    return kneeling;
}

// ------------------------------------------------------------------------
// Calculate the new kinematic state of the overall system based on tracker
// and joystick input
// ------------------------------------------------------------------------
void vsExpeditionDIMotion::update()
{
    double xPos, yPos;
    bool xRun, yRun;
    atVector delta;
    atQuat legQuat;
    atVector legForward, legUp;
    double fwdMag;
    atVector projectedForward, projectedUp;
    double rootHeading;
    double legPitch;
    atQuat rollQuat;
    atQuat rootOrient;
    atQuat headOrient;
    atQuat weaponOrient;

    // Read the joystick values and see if we should be "running" in the
    // x or y direction
    xPos = joystick->getAxis(1)->getPosition();
    yPos = joystick->getAxis(0)->getPosition();
    if (fabs(xPos) > 0.9)
        xRun = true;
    if (fabs(yPos) > 0.9)
        yRun = true;

    // Initialize the velocity vector
    delta.setSize(3);
    delta.clear();

    // Calculate the desired side-to-side position offset
    if (xRun)
        delta[AT_X] = xPos * VS_EDI_SIDESTEP_RUN_SPEED;
    else
        delta[AT_X] = xPos * VS_EDI_SIDESTEP_WALK_SPEED;

    // Do the same for the forward-backward offset
    if (yPos > 0.0)
    {
        // We're moving forward
        if (yRun)
            delta[AT_Y] = yPos * VS_EDI_FORWARD_RUN_SPEED;
        else
            delta[AT_Y] = yPos * VS_EDI_FORWARD_WALK_SPEED;
    }
    else
    {
        // We're moving backward
        if (yRun)
            delta[AT_Y] = yPos * VS_EDI_BACKWARD_RUN_SPEED;
        else
            delta[AT_Y] = yPos * VS_EDI_BACKWARD_WALK_SPEED;
    }

    // Get the root heading of the tracker, being very careful to avoid
    // gimbal lock if the leg is pitched up very far.  We assume here that
    // there isn't a significant amount of roll (even if there is, it should
    // be OK as long as it's fairly static)
    legQuat = legTracker->getOrientationQuat();
    legForward = legQuat.rotatePoint(atVector(0.0, 1.0, 0.0));
    legUp = legQuat.rotatePoint(atVector(0.0, 0.0, 1.0));

    // Project the forward vector onto the XY plane and check its magnitude
    projectedForward = legForward;
    projectedForward[AT_Z] = 0.0;
    fwdMag = projectedForward.getMagnitudeSquared();
    if (fabs(fwdMag) < 0.1)
    {
        // The forward vector is unreliable (it's too close to the Z axis), so
        // we'll project and up vector and then negate it to get a projected
        // forward vector. (If forward is pointing up, then up is pointing
        // backward, and negative backward is forward, get it? :-) )
        projectedUp = legUp;
        projectedUp[AT_Z] = 0.0;
        projectedUp.normalize();
        projectedForward = projectedUp * -1.0;

        // Since we're using the positive Y direction as zero heading, the
        // atan2() call is a bit different from what it normally would be
        rootHeading =
            AT_RAD2DEG(atan2(-projectedForward[AT_X], projectedForward[AT_Y]));

        // Compute the pitch using the angle between the projected vector and
        // the actual vector
        legPitch = 90.0 - projectedUp.getAngleBetween(legUp);
        if (legForward[AT_Z] < 0.0)
           legPitch = -legPitch;
    }
    else
    {
        // The leg isn't pitched up too much, so we can get a good heading
        // from the forward vector
        projectedForward.normalize();

        // Since we're using the positive Y direction as zero heading, the
        // atan2() call is a bit different from what it normally would be
        rootHeading = 
            AT_RAD2DEG(atan2(-projectedForward[AT_X], projectedForward[AT_Y]));

        // Compute the pitch using the angle between the projected vector and
        // the actual vector
        legPitch = projectedForward.getAngleBetween(legForward);
        if (legForward[AT_Z] < 0.0)
            legPitch = -legPitch;
    }

    // Adjust the values if the tracker is on the right leg instead of
    // the left
    if (trackedLeg == VS_EDI_RIGHT_LEG)
    {
        // Turn the heading around 180 degrees
        if (rootHeading > 0.0)
            rootHeading -= 180.0;
        else
            rootHeading += 180.0;

        // Negate the leg's pitch
        legPitch = -legPitch;
    }

    // Set the root orientation to the leg tracker's heading
    rootOrient.setAxisAngleRotation(0.0, 0.0, 1.0, rootHeading);
    rootKin->setOrientation(rootOrient);

    // Orient the delta vector
    delta = rootOrient.rotatePoint(delta);

    // Update the root kinematics's velocity
    rootKin->setVelocity(delta);

    // Update the kneeling pose
    if (kneeling)
    {
        // Signal that we're no longer kneeling if the leg tracker's
        // pitch has fallen below the stand threshold
        if (legPitch < standThreshold)
            kneeling = false;
    }
    else
    {
        // Signal that we're now kneeling if the leg tracker's
        // pitch has risen above the kneel threshold
        if (legPitch > kneelThreshold)
            kneeling = true;
    }

    // Now, set the head's orientation, factoring out the root heading
    headOrient = headTracker->getOrientationQuat();
    headOrient = rootOrient.getInverse() * headOrient;
    headKin->setOrientation(headOrient);

    // Get the weapon's orientation
    weaponOrient = weaponTracker->getOrientationQuat();

    // The weapon tracker is mounted upside-down, so start with a 180 degree
    // roll before calculating the orientation
    rollQuat.setAxisAngleRotation(0.0, 1.0, 0.0, 180.0);
    weaponOrient = weaponOrient * rollQuat;

    // Set the weapon's orientation, factoring out the root heading and
    // applying the roll correction
    weaponOrient = rootOrient.getInverse() * weaponOrient;
    weaponKin->setOrientation(weaponOrient);
}

