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
//    VESS Module:  vsWalkArticulation.c++
//
//    Description:  Motion model that takes the velocity of an object,
//                  and attemps to make human-like walking movements on
//                  the joints on that object when it is moving.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsTimer.h++"
#include "vsWalkArticulation.h++"

// ------------------------------------------------------------------------
// Constructor - Reads in the specified walk articulation data file and
// sets up the frames of the walk animation
// ------------------------------------------------------------------------
vsWalkArticulation::vsWalkArticulation(vsKinematics *objectKin,
                                       char *walkDataFilename)
{
    FILE *datafile;
    char lineBuffer[256];
    double h, p, r;
    atQuat jointRot;
    double dist;
    int loop;
    vsWalkArticData *keyData;
    vsWalkArticData *tempFrame;

    // Store the given root kinematics object
    rootKin = objectKin;

    // Initialize the six joint kinematics objects to 'not present'
    leftHipKin = NULL;
    leftKneeKin = NULL;
    leftAnkleKin = NULL;
    rightHipKin = NULL;
    rightKneeKin = NULL;
    rightAnkleKin = NULL;
    
    // Read in the animation data from the given file, one frame at a time.
    // Frame zero (the first frame) is special; it is the neutral (not moving)
    // position of the joints, and is not part of the cycle while the object
    // is moving.
    keyframeIndex = 0;
    datafile = fopen(walkDataFilename, "r");
    if (datafile)
    {
        while (!feof(datafile))
        {
            // Create a walk articulation data structure to hold the
	    // frame information
            keyData = new vsWalkArticData();

            // Read the articulation data, one line for each joint
            for (loop = 0; loop < 6; loop++)
            {
                // Animation data for each frame is in the form 'heading
                // pitch roll', for each of the six supported joints
                getLine(datafile, lineBuffer);
                sscanf(lineBuffer, "%lf %lf %lf", &h, &p, &r);
                jointRot.setEulerRotation(AT_EULER_ANGLES_ZXY_R, h, p, r);
                switch (loop)
                {
                    case 0:
                        keyData->setLeftHip(jointRot);
                        break;
                    case 1:
                        keyData->setLeftKnee(jointRot);
                        break;
                    case 2:
                        keyData->setLeftAnkle(jointRot);
                        break;
                    case 3:
                        keyData->setRightHip(jointRot);
                        break;
                    case 4:
                        keyData->setRightKnee(jointRot);
                        break;
                    case 5:
                        keyData->setRightAnkle(jointRot);
                        break;
                }
            }

            // The seventh line in each frame is the distance over which the
            // data in that frame is active.
            getLine(datafile, lineBuffer);
            sscanf(lineBuffer, "%lf", &dist);
            keyData->setDistance(dist);

            // Store the articulation information in our keyframe array, and
	    // increment the total-number-of-frames counter
            keyframeData.addEntry(keyData);
        }
        fclose(datafile);
    }
    else
        printf("vsWalkArticulation::vsWalkArticulation: Unable to open "
            "keyframe data file %s\n", walkDataFilename);

    // Initialize the keyframe pointers
    if (keyframeData.getNumEntries() > 0)
    {
        fromKeyframe = (vsWalkArticData *) keyframeData.getEntry(0);
        toKeyframe = (vsWalkArticData *) keyframeData.getEntry(0);
    }

    // Check to see if only one frame was specified; if so, then duplicate
    // that frame.
    if (keyframeData.getNumEntries() == 1)
    {
        tempFrame = (vsWalkArticData *) keyframeData.getEntry(0);
	keyframeData.addEntry(tempFrame->clone());
    }
    
    // Initialize the object to not-currently-moving values
    travelDist = 0.0;
    waitTime = 0.0;
    moveState = VS_WALK_ARTIC_STOPPED;
}

// ------------------------------------------------------------------------
// Destructor - Deletes the animation data
// ------------------------------------------------------------------------
vsWalkArticulation::~vsWalkArticulation()
{
    // The keyframes in the keyframeData array will be deleted when the
    // array itself goes out of scope
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsWalkArticulation::getClassName()
{
    return "vsWalkArticulation";
}

// ------------------------------------------------------------------------
// Sets the kinematics object corresponding to the specified leg joint to
// the given object
// ------------------------------------------------------------------------
void vsWalkArticulation::setJointKinematics(int whichJoint,
    vsKinematics *kinematics)
{
    // Interpret the joint constant
    switch (whichJoint)
    {
        case VS_WALK_ARTIC_LEFT_HIP:
            leftHipKin = kinematics;
            break;
        case VS_WALK_ARTIC_LEFT_KNEE:
            leftKneeKin = kinematics;
            break;
        case VS_WALK_ARTIC_LEFT_ANKLE:
            leftAnkleKin = kinematics;
            break;
        case VS_WALK_ARTIC_RIGHT_HIP:
            rightHipKin = kinematics;
            break;
        case VS_WALK_ARTIC_RIGHT_KNEE:
            rightKneeKin = kinematics;
            break;
        case VS_WALK_ARTIC_RIGHT_ANKLE:
            rightAnkleKin = kinematics;
            break;
        default:
            printf("vsWalkArticulation::setJointKinematics: Unrecognized "
                "joint constant\n");
            break;
    }
}

// ------------------------------------------------------------------------
// Gets the kinematics object associated with the specified leg joint
// ------------------------------------------------------------------------
vsKinematics *vsWalkArticulation::getJointKinematics(int whichJoint)
{
    // Interpret the joint constant
    switch (whichJoint)
    {
        case VS_WALK_ARTIC_LEFT_HIP:
            return leftHipKin;
        case VS_WALK_ARTIC_LEFT_KNEE:
            return leftKneeKin;
        case VS_WALK_ARTIC_LEFT_ANKLE:
            return leftAnkleKin;
        case VS_WALK_ARTIC_RIGHT_HIP:
            return rightHipKin;
        case VS_WALK_ARTIC_RIGHT_KNEE:
            return rightKneeKin;
        case VS_WALK_ARTIC_RIGHT_ANKLE:
            return rightAnkleKin;
        default:
            printf("vsWalkArticulation::getJointKinematics: Unrecognized "
                "joint constant\n");
            break;
    }

    // If the joint constant is unrecognized, just return NULL.
    return NULL;
}
    
// ------------------------------------------------------------------------
// Sets the orientations of the leg joint kinematics based on the velocity
// of the object as given by the root kinematics object
// ------------------------------------------------------------------------
void vsWalkArticulation::update()
{
    double speed;
    int keyframeCount;

    // Get the number of keyframes
    keyframeCount = keyframeData.getNumEntries();

    // If no frames were specified (for whatever reason), abort.
    if (keyframeCount <= 0)
        return;

    // Get the current travel speed, ignoring what direction it's in
    speed = (rootKin->getVelocity()).getMagnitude();
    
    // Check to see if we're moving or not
    if (speed > 0.0)
    {
        // Start moving or keep moving

        if (moveState == VS_WALK_ARTIC_STOPPED)
        {
            // * Start moving
            // Start the animation sequence by setting the current frame
	    // to the first (non-static) one, setting the frames to
	    // interpolate between to the static one and first animated
	    // one, and setting the movement state to 'in-motion'.
            travelDist = 0.0;
            keyframeIndex = 1;
            waitTime = -1.0;
            fromKeyframe = (vsWalkArticData *) keyframeData.getEntry(0);
            toKeyframe = (vsWalkArticData *) keyframeData.getEntry(1);
            moveState = VS_WALK_ARTIC_MOVING;
        }
        else if (moveState == VS_WALK_ARTIC_STOPPING)
        {
            // * Go from slowing down back to full speed
	    // Resume the animation sequence by capturing the current
	    // articulation positions as a new key frame, setting the
	    // frames to interpolate between to this new key frame and
	    // what would have been the next one before we first decided
	    // to stop, and setting the movement state back to 'in-motion'.
            travelDist = 0.0;
            waitTime = -1.0;
            captureStopFrame();
            fromKeyframe = &stopKeyframe;
            toKeyframe = (vsWalkArticData *)
                keyframeData.getEntry(keyframeIndex);
            moveState = VS_WALK_ARTIC_MOVING;
        }

	// Calculate the distance travelled, and use that to determine if
	// we should switch to the next key frame in the sequence
        travelDist += (vsTimer::getSystemTimer()->getInterval()) * speed;
        while (travelDist > toKeyframe->getDistance())
        {
            // Subtract the distance that the destination keyframe
	    // covers from the total travelled distance
            travelDist -= toKeyframe->getDistance();

            // Increment the current animation frame index
            keyframeIndex = ((keyframeIndex + 1) % keyframeCount);
            if (keyframeIndex == 0)
                keyframeIndex = 1;

            // Set the old animation data to the new data, and the new
	    // data to the next frame in the sequence
            fromKeyframe = toKeyframe;
            toKeyframe = (vsWalkArticData *)
                keyframeData.getEntry(keyframeIndex);
        }
    }
    else if (moveState != VS_WALK_ARTIC_STOPPED)
    {
        // Stop moving

        // Check if we were previously moving full speed
        if (moveState == VS_WALK_ARTIC_MOVING)
        {
            // * Start stopping
	    // Capture the current articulation positions as a new key frame,
	    // set the frames to interpolate between to the new key frame
	    // and the static frame, and set the movement state to
	    // 'slowing-down'.
            waitTime = 0.0;
            captureStopFrame();
            fromKeyframe = &stopKeyframe;
            toKeyframe = (vsWalkArticData *) keyframeData.getEntry(0);
            moveState = VS_WALK_ARTIC_STOPPING;
        }
        
	// Add the amount of time that has just passed to the stop timer;
	// if this timer expires, then we've stopped completely
        waitTime += vsTimer::getSystemTimer()->getInterval();
        travelDist = waitTime;
        
        // Check if we are stopping, and if we have been stopping long
	// enough to consider ourselves completely stopped
        if ((moveState == VS_WALK_ARTIC_STOPPING) &&
            (waitTime > toKeyframe->getDistance()))
        {
            // * Finish stopping
	    // Set both frames to interpolate between to the static frame,
	    // and set the movement state to 'not-moving'.
            travelDist = 0.0;
            waitTime = -1.0;
            fromKeyframe = (vsWalkArticData *) keyframeData.getEntry(0);
            toKeyframe = (vsWalkArticData *) keyframeData.getEntry(0);
            keyframeIndex = 0;
            moveState = VS_WALK_ARTIC_STOPPED;
        }
    }

    // * Interpolate the new joint positions
    interpolateKeys(fromKeyframe, toKeyframe, travelDist);
}

// ------------------------------------------------------------------------
// Private function
// Reads a line from a configuration file and stores it in the area
// indicated by the buffer pointer. Strips out blank lines and comment
// lines (lines starting with the # character).
// ------------------------------------------------------------------------
void vsWalkArticulation::getLine(FILE *in, char *buffer)
{
    // Prime the while loop
    buffer[0] = '#';
    buffer[1] = 0;

    // Strip whitespace
    if (!feof(in))
        fscanf(in, " \n");
    
    // Search for a line that doesn't start with a #
    while (!feof(in) && (buffer[0] == '#'))
    {
        fgets(buffer, 255, in);
        if (!feof(in))
            fscanf(in, " \n");
    }
    
    // If the line begins with a comment character, then we must have
    // hit EOF. Just set the return value to an empty string.
    if (buffer[0] == '#')
        buffer[0] = 0;
}

// ------------------------------------------------------------------------
// Private function
// 'Captures' the current leg articulation and stores it in the
// stopKeyframe member
// ------------------------------------------------------------------------
void vsWalkArticulation::captureStopFrame()
{
    // For each active joint, read its current orientation and copy that
    // into the temporary keyframe structure. If the joint is inactive,
    // copy a no-rotation value instead.

    if (leftHipKin)
        stopKeyframe.setLeftHip(leftHipKin->getOrientation());
    else
        stopKeyframe.setLeftHip(atQuat(0.0, 0.0, 0.0, 1.0));

    if (leftKneeKin)
        stopKeyframe.setLeftKnee(leftKneeKin->getOrientation());
    else
        stopKeyframe.setLeftKnee(atQuat(0.0, 0.0, 0.0, 1.0));

    if (leftAnkleKin)
        stopKeyframe.setLeftAnkle(leftAnkleKin->getOrientation());
    else
        stopKeyframe.setLeftAnkle(atQuat(0.0, 0.0, 0.0, 1.0));

    if (rightHipKin)
        stopKeyframe.setRightHip(rightHipKin->getOrientation());
    else
        stopKeyframe.setRightHip(atQuat(0.0, 0.0, 0.0, 1.0));

    if (rightKneeKin)
        stopKeyframe.setRightKnee(rightKneeKin->getOrientation());
    else
        stopKeyframe.setRightKnee(atQuat(0.0, 0.0, 0.0, 1.0));

    if (rightAnkleKin)
        stopKeyframe.setRightAnkle(rightAnkleKin->getOrientation());
    else
        stopKeyframe.setRightAnkle(atQuat(0.0, 0.0, 0.0, 1.0));
}

// ------------------------------------------------------------------------
// Private function.
// Interpolates between two keyframes using the given travel distance as
// a parameter
// ------------------------------------------------------------------------
void vsWalkArticulation::interpolateKeys(vsWalkArticData *key1,
                                         vsWalkArticData *key2,
                                         double dist)
{
    atQuat a, b;
    double t;
    atQuat result;

    // For each active joint, compute that joint's orientation by using
    // the atQuat slerp() call to interpolate the joint's orientation
    // between the two currently active keyframes, using the distance that
    // the object has travelled as the interpolation value.
    if (leftHipKin)
    {
        // Get the two key joint angles
        a = key1->getLeftHip();
        b = key2->getLeftHip();

        // Compute the interpolation parameter
        t = dist / key2->getDistance();

        // Slerp the two angles and set the result on the joint
        result = a.slerp(b, t);
        leftHipKin->setOrientation(result);
    }

    // Same procedure for the left knee
    if (leftKneeKin)
    {
        // Get the two key joint angles
        a = key1->getLeftKnee();
        b = key2->getLeftKnee();

        // Compute the interpolation parameter
        t = dist / key2->getDistance();

        // Slerp the two angles and set the result on the joint
        result = a.slerp(b, t);
        leftKneeKin->setOrientation(result);
    }

    // Same procedure for the left ankle
    if (leftAnkleKin)
    {
        // Get the two key joint angles
        a = key1->getLeftAnkle();
        b = key2->getLeftAnkle();

        // Compute the interpolation parameter
        t = dist / key2->getDistance();

        // Slerp the two angles and set the result on the joint
        result = a.slerp(b, t);
        leftAnkleKin->setOrientation(result);
    }

    // Same procedure for the right hip
    if (rightHipKin)
    {
        // Get the two key joint angles
        a = key1->getRightHip();
        b = key2->getRightHip();

        // Compute the interpolation parameter
        t = dist / key2->getDistance();

        // Slerp the two angles and set the result on the joint
        result = a.slerp(b, t);
        rightHipKin->setOrientation(result);
    }

    // Same procedure for the right knee
    if (rightKneeKin)
    {
        // Get the two key joint angles
        a = key1->getRightKnee();
        b = key2->getRightKnee();

        // Compute the interpolation parameter
        t = dist / key2->getDistance();

        // Slerp the two angles and set the result on the joint
        result = a.slerp(b, t);
        rightKneeKin->setOrientation(result);
    }

    // Same procedure for the right ankle
    if (rightAnkleKin)
    {
        // Get the two key joint angles
        a = key1->getRightAnkle();
        b = key2->getRightAnkle();

        // Compute the interpolation parameter
        t = dist / key2->getDistance();

        // Slerp the two angles and set the result on the joint
        result = a.slerp(b, t);
        rightAnkleKin->setOrientation(result);
    }
}
