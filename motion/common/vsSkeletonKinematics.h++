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
//    Description:  
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_SKELETON_KINEMATICS_HPP
#define VS_SKELETON_KINEMATICS_HPP

#include "vsUpdatable.h++"
#include "vsSkeleton.h++"
#include "vsKinematics.h++"

class VS_MOTION_DLL vsSkeletonKinematics : public vsUpdatable
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

    vsKinematics          *getBoneKinematics(int boneID);
    vsKinematics          *getBoneKinematics(vsComponent *component);
    vsKinematics          *getBoneKinematics(char *boneName);

    // Update function from vsUpdatable
    virtual void          update();

    // Update function to take a time value.
    void                  update(double deltaTime);

    // Reset function from vsMotionModel
    virtual void          reset();
};

#endif