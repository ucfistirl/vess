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
//    VESS Module:  vsSkeletonKinematics.c++
//
//    Description:  A convenient object to easily create and manage
//                  vsKinematics for each bone found in a vsSkeleton.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#include "vsSkeletonKinematics.h++"

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsSkeletonKinematics::vsSkeletonKinematics(vsSkeleton *newSkeleton)
{
    int index;
    vsComponent *bone;
    vsKinematics *boneKin;

    // Keep a reference ot the skeleton.
    skeleton = newSkeleton;
    skeleton->ref();

    // Generate the vsKinematics array.
    kinematicsCount = skeleton->getBoneCount();
    kinematicsList = new vsArray();

    // Create all the proper kinematics objects per bone.
    for (index = 0; index < kinematicsCount; index++)
    {
        bone = skeleton->getBone(index);
        boneKin = new vsKinematics(bone);
        kinematicsList->setEntry(index, boneKin);
    }
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsSkeletonKinematics::~vsSkeletonKinematics()
{
    // Delete the vsKinematics list (this unreferences all kinematics in it)
    delete kinematicsList;

    // Unreference the skeleton.
    vsObject::unrefDelete(skeleton);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsSkeletonKinematics::getClassName()
{
    return "vsSkeletonKinematics";
}

// ------------------------------------------------------------------------
// A simple accessor function to return the number of kinematics objects
// in the array.
// ------------------------------------------------------------------------
int vsSkeletonKinematics::getKinematicsCount()
{
    return kinematicsCount;
}

// ------------------------------------------------------------------------
// Return the relevant bone kinematics based on the bone ID.
// ------------------------------------------------------------------------
vsKinematics *vsSkeletonKinematics::getBoneKinematics(int boneID)
{
    vsKinematics *returnValue;

    returnValue = NULL;

    // If given a valid boneID, return the atMatrix for that boneID.
    if ((boneID < kinematicsCount) && (boneID >= 0))
    {
        returnValue = (vsKinematics *)kinematicsList->getEntry(boneID);
    }

    return returnValue;
}

// ------------------------------------------------------------------------
// Return the relevant bone kinematics based on the vsComponent for the bone.
// ------------------------------------------------------------------------
vsKinematics *vsSkeletonKinematics::getBoneKinematics(vsComponent *component)
{
    vsKinematics *returnValue;
    int boneID;

    returnValue = NULL;

    // Try to get the boneID based on the component we got.
    boneID = skeleton->getBoneID(component);

    // If the boneID is valid, get the proper kinematics object.
    if (boneID > -1)
    {
        returnValue = (vsKinematics *)kinematicsList->getEntry(boneID);
    }

    return returnValue;
}

// ------------------------------------------------------------------------
// Return the relevant bone kinematics based on the name for the bone.
// ------------------------------------------------------------------------
vsKinematics *vsSkeletonKinematics::getBoneKinematics(char *boneName)
{
    vsKinematics *returnValue;
    int boneID;

    returnValue = NULL;

    // Try to get the boneID based on the name we got.
    boneID = skeleton->getBoneID(boneName);

    // If the boneID is valid, get the proper kinematics object.
    if (boneID > -1)
    {
        returnValue = (vsKinematics *)kinematicsList->getEntry(boneID);
    }

    return returnValue;
}

// ------------------------------------------------------------------------
// Return the corresponding bone ID for the given kinematics (or -1 if
// it isn't found)
// ------------------------------------------------------------------------
int vsSkeletonKinematics::getBoneIDForKinematics(vsKinematics *kin)
{
    int i;

    // Iterate over the kinematics in our list
    for (i = 0; i < kinematicsCount; i++)
    {
        // See if this kinematics matches the one specified
        if (kinematicsList->getEntry(i) == kin)
            return i;
    }

    // We didn't find it, so return -1
    return -1;
}

// ------------------------------------------------------------------------
// Update all the bone kinematics.
// ------------------------------------------------------------------------
void vsSkeletonKinematics::update()
{
    int index;

    // Update all the kinematics objects.
    for (index = 0; index < kinematicsCount; index++)
    {
        ((vsKinematics *)kinematicsList->getEntry(index))->update();
    }
}

// ------------------------------------------------------------------------
// Update all the bone kinematics with the given time.
// ------------------------------------------------------------------------
void vsSkeletonKinematics::update(double deltaTime)
{
    int index;

    // Update all the kinematics objects.
    for (index = 0; index < kinematicsCount; index++)
    {
        ((vsKinematics *)kinematicsList->getEntry(index))->update(deltaTime);
    }
}

// ------------------------------------------------------------------------
// Reset all the kinematics to their neutral positions.
// ------------------------------------------------------------------------
void vsSkeletonKinematics::reset()
{
    atVector resetVector;
    atQuat resetQuat;
    int index;
    vsKinematics *kin;

    // Set the rotations and position information to neutral values.
    resetVector.setSize(3);
    resetVector.clear();
    resetQuat.set(0.0, 0.0, 0.0, 1.0);

    // Update all the kinematics objects.
    for (index = 0; index < kinematicsCount; index++)
    {
        kin = (vsKinematics *)kinematicsList->getEntry(index);
        kin->setPosition(resetVector);
        kin->setVelocity(resetVector);
        kin->setAngularVelocity(resetVector, 0.0);
        kin->setOrientation(resetQuat);
    }
}
