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

#include "vsSystem.h++"

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
    vsQuat jointRot;
    int loop;
    vsWalkArticData *keyData;

    rootKin = objectKin;

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
            keyData = (vsWalkArticData *)
                (malloc(sizeof(vsWalkArticData)));
            for (loop = 0; loop < 6; loop++)
            {
                // Animation data for each frame is in the form 'heading
                // pitch roll', for each of the six supported joints
                getLine(datafile, lineBuffer);
                sscanf(lineBuffer, "%lf %lf %lf", &h, &p, &r);
                jointRot.setEulerRotation(VS_EULER_ANGLES_ZXY_R, h, p, r);
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

            keyframeData[keyframeCount] = keyData;
            keyframeCount++;
        }
        fclose(datafile);
    }
    else
        printf("vsWalkArticulation::vsWalkArticulation: Unable to open "
            "keyframe data file %s\n", walkDataFilename);

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

    for (loop = 0; loop < keyframeCount; loop++)
        free(keyframeData[loop]);
}

// ------------------------------------------------------------------------
// Sets the kinematics object corresponding to the specified leg joint to
// the given object
// ------------------------------------------------------------------------
void vsWalkArticulation::setJointKinematics(int whichJoint,
    vsKinematics *kinematics)
{
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

    speed = (rootKin->getVelocity()).getMagnitude();
    
    if (speed > 0.0)
    {
        // Start moving or keep moving

        if (moveState == VS_WALK_ARTIC_STOPPED)
        {
            // Start moving
            travelDist = 0.0;
            keyframeIndex = 1;
            waitTime = -1.0;
            fromKeyframe = (vsWalkArticData *)(keyframeData[0]);
            toKeyframe = (vsWalkArticData *)(keyframeData[1]);
            moveState = VS_WALK_ARTIC_MOVING;
        }
        else if (moveState == VS_WALK_ARTIC_STOPPING)
        {
            // Go from slowing down back to full speed
            travelDist = 0.0;
            waitTime = -1.0;
            captureStopFrame();
            fromKeyframe = &stopKeyframe;
            toKeyframe = (vsWalkArticData *)(keyframeData[keyframeIndex]);
            moveState = VS_WALK_ARTIC_MOVING;
        }

        travelDist += (vsSystem::systemObject)->getFrameTime() * speed;
        while (travelDist > toKeyframe->distance)
        {
            travelDist -= toKeyframe->distance;
            keyframeIndex = ((keyframeIndex + 1) % keyframeCount);
            if (keyframeIndex == 0)
                keyframeIndex = 1;
            fromKeyframe = toKeyframe;
            toKeyframe = (vsWalkArticData *)(keyframeData[keyframeIndex]);
        }
    }
    else if (moveState != VS_WALK_ARTIC_STOPPED)
    {
        // Stop moving

        if (moveState == VS_WALK_ARTIC_MOVING)
        {
            // Start stopping
            waitTime = 0.0;
            captureStopFrame();
            fromKeyframe = &stopKeyframe;
            toKeyframe = (vsWalkArticData *)(keyframeData[0]);
            moveState = VS_WALK_ARTIC_STOPPING;
        }
        
        waitTime += (vsSystem::systemObject)->getFrameTime();
        travelDist = waitTime;
        
        if ((moveState == VS_WALK_ARTIC_STOPPING) &&
            (waitTime > toKeyframe->distance))
        {
            // Finish stopping
            travelDist = 0.0;
            waitTime = -1.0;
            fromKeyframe = (vsWalkArticData *)(keyframeData[0]);
            toKeyframe = (vsWalkArticData *)(keyframeData[0]);
            keyframeIndex = 0;
            moveState = VS_WALK_ARTIC_STOPPED;
        }
    }

    // Interpolate the joint positions
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
    buffer[0] = '#';
    buffer[1] = 0;
    if (!feof(in))
        fscanf(in, " \n");
    
    while (!feof(in) && (buffer[0] == '#'))
    {
        fgets(buffer, 255, in);
        if (!feof(in))
            fscanf(in, " \n");
    }
    
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
