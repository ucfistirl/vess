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

    // Keep a reference ot the skeleton.
    skeleton = newSkeleton;
    skeleton->ref();

    // Generate the vsKinematics array.
    kinematicsCount = skeleton->getBoneCount();
    kinematicsList = new vsKinematics*[kinematicsCount];

    // Create all the proper kinematics objects per bone.
    for (index = 0; index < kinematicsCount; index++)
    {
        bone = skeleton->getBone(index);
        kinematicsList[index] = new vsKinematics(bone);
        kinematicsList[index]->ref();
    }
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsSkeletonKinematics::~vsSkeletonKinematics()
{
    int index;

    // Delete all the vsKinematics in the list.
    for (index = 0; index < kinematicsCount; index++)
        vsObject::unrefDelete(kinematicsList[index]);

    // Delete the vsKinematics pointer list.
    delete [] kinematicsList;

    // Unreference the skeleton.
    skeleton->unref();
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
        returnValue = kinematicsList[boneID];
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
        returnValue = kinematicsList[boneID];
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
        returnValue = kinematicsList[boneID];
    }

    return returnValue;
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
        kinematicsList[index]->update();
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
        kinematicsList[index]->update(deltaTime);
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

    // Set the rotations and position information to neutral values.
    resetVector.setSize(3);
    resetVector.clear();
    resetQuat.set(0.0, 0.0, 0.0, 1.0);

    // Update all the kinematics objects.
    for (index = 0; index < kinematicsCount; index++)
    {
        kinematicsList[index]->setPosition(resetVector);
        kinematicsList[index]->setVelocity(resetVector);
        kinematicsList[index]->setAngularVelocity(resetVector, 0.0);
        kinematicsList[index]->setOrientation(resetQuat);
    }
}
