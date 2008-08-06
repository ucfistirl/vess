
#include "vsCOLLADAChannelGroup.h++"

#include "vsKinematics.h++"


vsCOLLADAChannelGroup::vsCOLLADAChannelGroup(vsCOLLADANode *target)
{
    // Store the target node and reference it
    targetNode = target;
    targetNode->ref();

    // Create the channel list
    channels = new atList();
}

vsCOLLADAChannelGroup::~vsCOLLADAChannelGroup()
{
    vsCOLLADAChannel *channel;

    // Unreference (possibly delete) the channels in the list
    channel = (vsCOLLADAChannel *)channels->getFirstEntry();
    while (channel != NULL)
    {
        // Remove the channel from the list
        channels->removeCurrentEntry();

        // Unref/delete the channel
        vsObject::unrefDelete(channel);

        // Next channel
        channel = (vsCOLLADAChannel *)channels->getNextEntry();
    }

    // Delete the list
    delete channels;

    // Unref/delete the channel target node
    vsObject::unrefDelete(targetNode);
}


void vsCOLLADAChannelGroup::setPathPoint(vsPathMotion *path, int pointIndex,
                                         vsCOLLADATransform *xform,
                                         atString xformAddr,
                                         vsCOLLADAKeyframe *keyframe,
                                         vsCOLLADAKeyframe *lastKeyframe)
{
    double previousTime;
    atVector basePos, keyPos;
    atQuat baseOrn, keyOrn;
    double tmpX, tmpY, tmpZ, tmpW;
    const char *addrStr;
    char addrTokens[32];
    atMatrix keyMat;
    int i, j;

    // Set the path point's time
    if (lastKeyframe == NULL)
    {
        // This is the first point on the path, so set the time directly
        path->setTime(pointIndex, keyframe->getTime());
    }
    else
    {
        // The path point's time is actually the difference in time between
        // this keyframe and the last keyframe
        path->setTime(pointIndex, 
            keyframe->getTime() - lastKeyframe->getTime());
    }

    // Get the transform address string
    addrStr = xformAddr.getString();

    // The data is handled differently based on the transform type
    switch (xform->getType())
    {
        case VS_COLLADA_XFORM_ROTATE:

            // See if there is an address specified for the transform
            if (strlen(addrStr) > 0)
            {
                // Get the base orientation of the transform
                baseOrn = xform->getOrientation();

                // Get the base transform values
                baseOrn.getAxisAngleRotation(&tmpX, &tmpY, &tmpZ, &tmpW);

                // See which part of the rotation we're changing
                if ((strcmp(addrStr, ".ANGLE") == 0) ||
                    (strcmp(addrStr, "(3)") == 0) ||
                    (strcmp(addrStr, "[3]") == 0))
                {
                    // Get the keyframe's data and update the rotation
                    tmpW = keyframe->getData(0);
                }
                else if ((strcmp(addrStr, ".X") == 0) ||
                         (strcmp(addrStr, "(0)") == 0) ||
                         (strcmp(addrStr, "[0]") == 0))
                {
                    // Get the keyframe's data and update the rotation
                    tmpX = keyframe->getData(0);
                }
                else if ((strcmp(addrStr, ".Y") == 0) ||
                         (strcmp(addrStr, "(1)") == 0) ||
                         (strcmp(addrStr, "[1]") == 0))
                {
                    // Get the keyframe's data and update the rotation
                    tmpY = keyframe->getData(0);
                }
                else if ((strcmp(addrStr, ".Z") == 0) ||
                         (strcmp(addrStr, "(2)") == 0) ||
                         (strcmp(addrStr, "[2]") == 0))
                {
                    // Get the keyframe's data and update the rotation
                    tmpZ = keyframe->getData(0);
                }

                // Set the new point on the path motion
                keyOrn.setAxisAngleRotation(tmpX, tmpY, tmpZ, tmpW);
                path->setOrientation(pointIndex, keyOrn);
            }
            else
            {
                // Get the keyframe's data
                tmpX = keyframe->getData(0);
                tmpY = keyframe->getData(1);
                tmpZ = keyframe->getData(2);
                tmpW = keyframe->getData(3);

                // Set the new point on the path motion
                keyOrn.setAxisAngleRotation(tmpX, tmpY, tmpZ, tmpW);
                path->setOrientation(pointIndex, keyOrn);
            }

        break;

        case VS_COLLADA_XFORM_TRANSLATE:

            // See if there is an address specified for the transform
            if (strlen(addrStr) > 0)
            {
                // Get the base orientation of the transform
                basePos = xform->getPosition();

                // See which part of the translation we're changing
                if ((strcmp(addrStr, ".X") == 0) ||
                    (strcmp(addrStr, "(0)") == 0) ||
                    (strcmp(addrStr, "[0]") == 0))
                {
                    // Get the keyframe's data and update the rotation
                    tmpX = keyframe->getData(0);
                }
                else if ((strcmp(addrStr, ".Y") == 0) ||
                         (strcmp(addrStr, "(1)") == 0) ||
                         (strcmp(addrStr, "[1]") == 0))
                {
                    // Get the keyframe's data and update the rotation
                    tmpY = keyframe->getData(0);
                }
                else if ((strcmp(addrStr, ".Z") == 0) ||
                         (strcmp(addrStr, "(2)") == 0) ||
                         (strcmp(addrStr, "[2]") == 0))
                {
                    // Get the keyframe's data and update the rotation
                    tmpZ = keyframe->getData(0);
                }

                // Set the new point on the path motion
                keyPos.set(tmpX, tmpY, tmpZ);
                path->setPosition(pointIndex, keyPos);
            }
            else
            {
                // Get the keyframe's data
                tmpX = keyframe->getData(0);
                tmpY = keyframe->getData(1);
                tmpZ = keyframe->getData(2);

                // Set the new point on the path motion
                keyPos.set(tmpX, tmpY, tmpZ, tmpW);
                path->setPosition(pointIndex, keyPos);
            }

        break;

        case VS_COLLADA_XFORM_MATRIX:

            // See if there is an address specified for the transform
            if (strlen(addrStr) > 0)
            {
                // Get the transform's matrix
                keyMat = xform->getMatrix();

                // JPD:  I'm making the assumption that both indices are
                // specified, so that only one element of the matrix
                // is changed.  The spec isn't clear about this so we
                // might have to revisit this at some point
                strcpy(addrTokens, addrStr);
                i = atoi(strtok(addrTokens, "()"));
                j = atoi(strtok(NULL, "()"));

                // Update the matrix with the channel data
                keyMat[j][i] = keyframe->getData(0);

                // Get the position and orientation from the new matrix
                keyPos = keyMat.getTranslation();
                keyOrn.setMatrixRotation(keyMat);

                // Set the new point on the path motion
                path->setPosition(pointIndex, keyPos);
                path->setOrientation(pointIndex, keyOrn);
            }
            else
            {
                // Get the keyframe data
                for (i = 0; i < 4; i++)
                    for (j = 0; j < 4; j++)
                        keyMat[i][j] = keyframe->getData(i*4 + j);

                // Get the position and orientation from the new matrix
                keyPos = keyMat.getTranslation();
                keyOrn.setMatrixRotation(keyMat);

                // Set the new point on the path motion
                path->setPosition(pointIndex, keyPos);
                path->setOrientation(pointIndex, keyOrn);
            }

        break;
    }
}

const char *vsCOLLADAChannelGroup::getClassName()
{
    return "vsCOLLADAChannelGroup";
}

atString vsCOLLADAChannelGroup::getTargetNodeID()
{
    return targetNode->getID();
}

void vsCOLLADAChannelGroup::addChannel(vsCOLLADAChannel *channel)
{
    // Make sure the channel is valid
    if (channel != NULL)
    {
        // Add the channel to the list and reference it
        channels->addEntry(channel);
        channel->ref();
    }
}

int vsCOLLADAChannelGroup::getNumChannels()
{
    return channels->getNumEntries();
}

vsCOLLADAChannel *vsCOLLADAChannelGroup::getChannel(int index)
{
    return (vsCOLLADAChannel *)channels->getNthEntry(index);
}

// ------------------------------------------------------------------------
// Create a vsPathMotion from the animation channel(s) attached to the
// target node.  The provided kinematics should be attached to the same
// node as this channel group's target node.  We allow the kinematics
// to be created externally, as it makes the creation of skeletal
// animation objects easier
// ------------------------------------------------------------------------
vsPathMotion *vsCOLLADAChannelGroup::instance(vsKinematics *kin)
{
    vsPathMotion *pathMotion;
    int numChannels;
    vsCOLLADAChannel *channel;
    double currentTime;
    double *nextTimes;
    double minTime;
    char xformSID[512];
    char *ch;
    char baseTarget[256];
    char sidAddr[64];
    vsCOLLADATransform *targetXform;
    vsCOLLADAKeyframe *keyframe;
    vsCOLLADAKeyframe *lastKeyframe;
    int pointIndex;

    // Create a path motion model using the provided kinematics
    pathMotion = new vsPathMotion(kin);

    // Get the number of channels in the list
    numChannels = channels->getNumEntries();

    // If there are no channels, just return the empty path motion
    if (numChannels == 0)
        return pathMotion;

    // If there is only one channel, we can take a shortcut
    if (numChannels == 1)
    {
        // Get the channel
        channel = (vsCOLLADAChannel *)channels->getFirstEntry();

        // Set the interpolation parameters
        pathMotion->setPositionMode(channel->getPositionInterpMode());
        pathMotion->setOrientationMode(channel->getOrientationInterpMode());

        // Get the full scoped ID of the transform we're targeting
        strcpy(xformSID, channel->getTargetXformSID().getString());

        // Strip off any addressing stuff at the end (".ANGLE" or "(3)" for
        // example)
        ch = strpbrk(xformSID, ".[(");
        if (ch != NULL)
        {
            // Copy the base target
            strncpy(baseTarget, xformSID, ch - xformSID - 1);

            // Copy the address
            strcpy(sidAddr, ch);
        }
        else
        {
            // No address on this target, so just copy the base target and
            // initialize the address to nothing
            strcpy(baseTarget, xformSID);
            memset(sidAddr, 0, sizeof(sidAddr));
        }

        // Grab the target transform from the node
        targetXform = targetNode->getTransform(atString(baseTarget));

        // If we can't find the transform, we can't animate it
        if (targetXform == NULL)
        {
            // Return the empty path motion
            return pathMotion;
        }

        // In order for us to animate it, the transform must be a translate,
        // rotate, or matrix (we can't animate scales or skews with a
        // vsPathMotion, and vsPathMotion doesn't provide for animated
        // "look at" points)
        if ((targetXform->getType() == VS_COLLADA_XFORM_LOOKAT) ||
            (targetXform->getType() == VS_COLLADA_XFORM_SKEW) ||
            (targetXform->getType() == VS_COLLADA_XFORM_SCALE))
        {
            // Return the empty path motion
            return pathMotion;
        }
        
        // Set up the path motion with the channel's keyframes
        pathMotion->setPointListSize(channel->getNumKeyframes());

        // Iterate over the channel's keyframes, but keep track of the
        // previous keyframe as we traverse the list
        lastKeyframe = NULL;
        keyframe = channel->getFirstKeyframe();
        pointIndex = 0;
        while (keyframe != NULL)
        {
            // Set the appropriate path point from the channel's keyframe
            // data
            setPathPoint(pathMotion, pointIndex, targetXform,
                sidAddr, keyframe, lastKeyframe);

            // Increment the point index
            pointIndex++;
 
            // Get the next keyframe
            lastKeyframe = keyframe;
            keyframe = channel->getNextKeyframe();
        }

        // Return the path motion
        return pathMotion;
    }

    // TODO:  Multiple channels on the same target node aren't supported yet
    //        Just return the empty path motion for now
    return pathMotion;
}

