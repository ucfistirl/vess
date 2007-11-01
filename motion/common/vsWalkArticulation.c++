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
    char *walkDataFilename) : keyframeData(10, 10)
{
    FILE *datafile;
    char lineBuffer[256];
    double h, p, r;
    atQuat jointRot;
    int loop;
    vsWalkArticData *keyData;

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
    keyframeCount = 0;
    keyframeIndex = 0;
    datafile = fopen(walkDataFilename, "r");
    if (datafile)
    {
        while (!feof(datafile))
        {
            // Create a walk articulation data structure to hold the
	    // frame information
            keyData = (vsWalkArticData *)(malloc(sizeof(vsWalkArticData)));

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
                        keyData->leftHip = jointRot;
                        break;
                    case 1:
                        keyData->leftKnee = jointRot;
                        break;
                    case 2:
                        keyData->leftAnkle = jointRot;
                        break;
                    case 3:
                        keyData->rightHip = jointRot;
                        break;
                    case 4:
                        keyData->rightKnee = jointRot;
                        break;
                    case 5:
                        keyData->rightAnkle = jointRot;
                        break;
                }
            }

            // The seventh line in each frame is the distance over which the
            // data in that frame is active.
            getLine(datafile, lineBuffer);
            sscanf(lineBuffer, "%lf", &(keyData->distance));

            // Store the articulation information in our keyframe array, and
	    // increment the total-number-of-frames counter
            keyframeData[keyframeCount] = keyData;
            keyframeCount++;
        }
        fclose(datafile);
    }
    else
        printf("vsWalkArticulation::vsWalkArticulation: Unable to open "
            "keyframe data file %s\n", walkDataFilename);

    // Initialize the keyframe pointers
    if (keyframeCount > 0)
    {
        fromKeyframe = (vsWalkArticData *)(keyframeData[0]);
        toKeyframe = (vsWalkArticData *)(keyframeData[0]);
    }

    // Check to see if only one frame was specified; if so, then duplicate
    // that frame.
    if (keyframeCount == 1)
    {
	keyframeData[1] = keyframeData[0];
	keyframeCount = 2;
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
    int loop;

    // Delete the articulation data
    for (loop = 0; loop < keyframeCount; loop++)
        free(keyframeData[loop]);
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

    // If no frames were specified (for whatever reason), abort.
    if (keyframeCount == 0)
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
            fromKeyframe = (vsWalkArticData *)(keyframeData[0]);
            toKeyframe = (vsWalkArticData *)(keyframeData[1]);
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
            toKeyframe = (vsWalkArticData *)(keyframeData[keyframeIndex]);
            moveState = VS_WALK_ARTIC_MOVING;
        }

	// Calculate the distance travelled, and use that to determine if
	// we should switch to the next key frame in the sequence
        travelDist += (vsTimer::getSystemTimer()->getInterval()) * speed;
        while (travelDist > toKeyframe->distance)
        {
            // Subtract the distance that the destination keyframe
	    // covers from the total travelled distance
            travelDist -= toKeyframe->distance;

            // Increment the current animation frame index
            keyframeIndex = ((keyframeIndex + 1) % keyframeCount);
            if (keyframeIndex == 0)
                keyframeIndex = 1;

            // Set the old animation data to the new data, and the new
	    // data to the next frame in the sequence
            fromKeyframe = toKeyframe;
            toKeyframe = (vsWalkArticData *)(keyframeData[keyframeIndex]);
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
            toKeyframe = (vsWalkArticData *)(keyframeData[0]);
            moveState = VS_WALK_ARTIC_STOPPING;
        }
        
	// Add the amount of time that has just passed to the stop timer;
	// if this timer expires, then we've stopped completely
        waitTime += vsTimer::getSystemTimer()->getInterval();
        travelDist = waitTime;
        
        // Check if we are stopping, and if we have been stopping long
	// enough to consider ourselves completely stopped
        if ((moveState == VS_WALK_ARTIC_STOPPING) &&
            (waitTime > toKeyframe->distance))
        {
            // * Finish stopping
	    // Set both frames to interpolate between to the static frame,
	    // and set the movement state to 'not-moving'.
            travelDist = 0.0;
            waitTime = -1.0;
            fromKeyframe = (vsWalkArticData *)(keyframeData[0]);
            toKeyframe = (vsWalkArticData *)(keyframeData[0]);
            keyframeIndex = 0;
            moveState = VS_WALK_ARTIC_STOPPED;
        }
    }

    // * Interpolate the joint positions
    // For each active joint, compute that joint's orientation by using
    // the atQuat slerp() call to interpolate the joint's orientation
    // between the two currently active keyframes, using the distance that
    // the object has travelled as the interpolation value.
    if (leftHipKin)
        leftHipKin->setOrientation(fromKeyframe->leftHip.
            slerp(toKeyframe->leftHip, (travelDist / toKeyframe->distance)));
    if (leftKneeKin)
        leftKneeKin->setOrientation(fromKeyframe->leftKnee.
            slerp(toKeyframe->leftKnee, (travelDist / toKeyframe->distance)));
    if (leftAnkleKin)
        leftAnkleKin->setOrientation(fromKeyframe->leftAnkle.
            slerp(toKeyframe->leftAnkle, (travelDist / toKeyframe->distance)));
    if (rightHipKin)
        rightHipKin->setOrientation(fromKeyframe->rightHip.
            slerp(toKeyframe->rightHip, (travelDist / toKeyframe->distance)));
    if (rightKneeKin)
        rightKneeKin->setOrientation(fromKeyframe->rightKnee.
            slerp(toKeyframe->rightKnee, (travelDist / toKeyframe->distance)));
    if (rightAnkleKin)
        rightAnkleKin->setOrientation(fromKeyframe->rightAnkle.
            slerp(toKeyframe->rightAnkle, (travelDist / toKeyframe->distance)));
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
        stopKeyframe.leftHip = leftHipKin->getOrientation();
    else
        stopKeyframe.leftHip.set(0.0, 0.0, 0.0, 1.0);

    if (leftKneeKin)
        stopKeyframe.leftKnee = leftKneeKin->getOrientation();
    else
        stopKeyframe.leftKnee.set(0.0, 0.0, 0.0, 1.0);

    if (leftAnkleKin)
        stopKeyframe.leftAnkle = leftAnkleKin->getOrientation();
    else
        stopKeyframe.leftAnkle.set(0.0, 0.0, 0.0, 1.0);

    if (rightHipKin)
        stopKeyframe.rightHip = rightHipKin->getOrientation();
    else
        stopKeyframe.rightHip.set(0.0, 0.0, 0.0, 1.0);

    if (rightKneeKin)
        stopKeyframe.rightKnee = rightKneeKin->getOrientation();
    else
        stopKeyframe.rightKnee.set(0.0, 0.0, 0.0, 1.0);

    if (rightAnkleKin)
        stopKeyframe.rightAnkle = rightAnkleKin->getOrientation();
    else
        stopKeyframe.rightAnkle.set(0.0, 0.0, 0.0, 1.0);
}
