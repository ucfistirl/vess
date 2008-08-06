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
//    VESS Module:  vsSkeletonKinematics.h++
//
//    Description:  A convenient object to easily create and manage
//                  vsKinematics for each bone found in a vsSkeleton.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_SKELETON_KINEMATICS_HPP
#define VS_SKELETON_KINEMATICS_HPP

#include "vsUpdatable.h++"
#include "vsSkeleton.h++"
#include "vsKinematics.h++"

class VESS_SYM vsSkeletonKinematics : public vsUpdatable
{
private:

    vsKinematics      **kinematicsList;
    vsSkeleton        *skeleton;
    int               kinematicsCount;

public:

                          vsSkeletonKinematics(vsSkeleton *newSkeleton);
    virtual               ~vsSkeletonKinematics();

    // Inherited from vsObject
    virtual const char    *getClassName();

    int                   getKinematicsCount();
    vsKinematics          *getBoneKinematics(int boneID);
    vsKinematics          *getBoneKinematics(vsComponent *component);
    vsKinematics          *getBoneKinematics(char *boneName);
    int                   getBoneIDForKinematics(vsKinematics *kin);

    // Update function from vsUpdatable
    virtual void          update();

    // Update function to take a time value.
    void                  update(double deltaTime);

    void                  reset();
};

#endif
