
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
                                         vsCOLLADAKeyframe *keyframe)
{
    atVector basePos, keyPos;
    atQuat baseOrn, keyOrn;
    double tmpX, tmpY, tmpZ, tmpW;

    // Set the path point's time
    path->setTime(pointIndex, keyframe->getTime());

    // The data is handled differently based on the transform type
    switch (xform->getType())
    {
        case VS_COLLADA_XFORM_ROTATE:

            // See if there is an address specified for the transform
            if (strlen(xformAddr) > 0)
            {
                // Get the base orientation of the transform
                baseOrient = xform->getOrientation();

                // Get the base transform values
                baseOrn.getAxisAngleRotation(&tmpX, &tmpY, &tmpZ, &tmpW);

                // See which part of the rotation we're changing
                if ((strcmp(xformAddr, ".ANGLE") == 0) ||
                    (strcmp(xformAddr, "(3)") == 0) ||
                    (strcmp(xformAddr, "[3]") == 0))
                {
                    // Get the keyframe's data and update the rotation
                    tmpW = keyframe->getData(0);
                }
                else if ((strcmp(xformAddr, ".X") == 0) ||
                         (strcmp(xformAddr, "(0)") == 0) ||
                         (strcmp(xformAddr, "[0]") == 0))
                {
                    // Get the keyframe's data and update the rotation
                    tmpX = keyframe->getData(0);
                }
                else if ((strcmp(xformAddr, ".Y") == 0) ||
                         (strcmp(xformAddr, "(1)") == 0) ||
                         (strcmp(xformAddr, "[1]") == 0))
                {
                    // Get the keyframe's data and update the rotation
                    tmpY = keyframe->getData(0);
                }
                else if ((strcmp(xformAddr, ".Z") == 0) ||
                         (strcmp(xformAddr, "(2)") == 0) ||
                         (strcmp(xformAddr, "[2]") == 0))
                {
                    // Get the keyframe's data and update the rotation
                    tmpZ = keyframe->getData(0);
                }

                // Set the new point on the path motion
                keyOrn.setAxisAngleRotation(tmpX, tmpY, tmpZ, tmpW);
                path->setPoint(pointIndex, keyOrn);
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
                path->setPoint(pointIndex, keyOrn);
            }

        break;

        case VS_COLLADA_XFORM_TRANSLATE:

            // See if there is an address specified for the transform
            if (strlen(xformAddr) > 0)
            {
                // Get the base orientation of the transform
                basePos = xform->getPosition();

                // See which part of the translation we're changing
                if ((strcmp(xformAddr, ".X") == 0) ||
                    (strcmp(xformAddr, "(0)") == 0) ||
                    (strcmp(xformAddr, "[0]") == 0))
                {
                    // Get the keyframe's data and update the rotation
                    tmpX = keyframe->getData(0);
                }
                else if ((strcmp(xformAddr, ".Y") == 0) ||
                         (strcmp(xformAddr, "(1)") == 0) ||
                         (strcmp(xformAddr, "[1]") == 0))
                {
                    // Get the keyframe's data and update the rotation
                    tmpY = keyframe->getData(0);
                }
                else if ((strcmp(xformAddr, ".Z") == 0) ||
                         (strcmp(xformAddr, "(2)") == 0) ||
                         (strcmp(xformAddr, "[2]") == 0))
                {
                    // Get the keyframe's data and update the rotation
                    tmpZ = keyframe->getData(0);
                }

                // Set the new point on the path motion
                keyPos.set(tmpX, tmpY, tmpZ);
                path->setPoint(pointIndex, keyPos);
            }
            else
            {
                // Get the keyframe's data
                tmpX = keyframe->getData(0);
                tmpY = keyframe->getData(1);
                tmpZ = keyframe->getData(2);

                // Set the new point on the path motion
                keyPos.set(tmpX, tmpY, tmpZ, tmpW);
                path->setPoint(pointIndex, keyPos);
            }

        break;

        case VS_COLLADA_XFORM_MATRIX:

            // See if there is an address specified for the transform
            if (strlen(xformAddr) > 0)
            {
            }
            else
            {
                // Get the keyframe data
                for (i = 0; i < 4; i++)
                    for (j = 0; j < 4; j++)
                        keyMat[i][j] = keyframe->getData(i*4 + j)

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

vsPathMotion *vsCOLLADAChannelGroup::instance()
{
    vsKinematics  *kin;
    vsPathMotion *pathMotion;
    int numChannels;
    vsCOLLADAChannel *channel;
    double currentTime;
    double *nextTimes;
    double minTime;
    char *xformSID;
    char *ch;
    char baseTarget[256];
    char targetAddr[64];
    vsCOLLADATransform *targetXform;

    // Create a kinematics object for the target node
    kin = new vsKinematics(targetNode);

    // Create a path motion model using the kinematics
    pathMotion = new vsPathMotion(kin);

    // Get the number of channels in the list
    numChannels = channels->getNumEntries();

    // If there are no channels, just return the empty path motion
    if (numChannels == 0)
        return pathMotion;

    // If there is only one channel, we can take a shortcut
    if (numChannels == 1)
    {
        // Get the full scoped ID of the transform we're targeting
        xformSID = channel->getTargetXformSID().getString();

        // Strip off any addressing stuff at the end (".ANGLE" or "(3)" for
        // example)
        ch = strpbrk(xformSID, ".[(");
        if (ch != NULL)
        {
            // Copy the base target
            strncpy(baseTarget, xformSID, ch - xformSID - 1);

            // Copy the address
            strcpy(targetAddr, ch);
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
        
        // Get the channel
        channel = (vsCOLLADAChannel *)channels->getFirstEntry();

        // Set up the path motion with the channel's keyframes
        pathMotion->setPointListSize(channel->getNumKeyframes());

        // Iterate over the channel's keyframes
        keyframe = channel->getFirstKeyframe();
        pointIndex = 0;
        while (keyframe != NULL)
        {
            // Set the appropriate path point from the channel's keyframe
            // data
            setPathPoint(pointIndex, transform, keyframe);

            // Get the next keyframe
            keyframe = channel->getNextKeyframe();
        }
    }

    // Create an array of time values to keep track of where we are
    // in each channel
    

    return new vsPathMotion(kin);
}

