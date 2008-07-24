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
//    VESS Module:  vsCharacter.h++
//
//    Description:  Class to encapsulate a virtual character, including
//                  one or more skeletons, skeleton kinematics, skins,
//                  and associated geometry.  Animations for the character
//                  are also managed.  Hardware and software skinning are 
//                  both supported, and can be enabled or disabled on the
//                  fly.  A default GLSL program is generated for each 
//                  skin, but the user can elect to specify a custom 
//                  program.
//                     
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#ifndef VS_CHARACTER_HPP
#define VS_CHARACTER_HPP

#include "atItem.h++"
#include "atList.h++"
#include "atMap.h++"
#include "atString.h++"
#include "vsSkeleton.h++"
#include "vsSkeletonKinematics.h++"
#include "vsSkin.h++"
#include "vsComponent.h++"
#include "vsPathMotionManager.h++"
#include "vsGLSLProgramAttribute.h++"
#include "vsGLSLUniform.h++"
#include "vsTransparencyAttribute.h++"
#include "vsMaterialAttribute.h++"

#define VS_CHAR_MAX_BONES 36


class vsCharacter : public vsUpdatable
{
protected:

    atList                     *characterSkeletons;
    atList                     *skeletonKinematics;
    atList                     *characterSkins;
    vsComponent                *characterMesh;

    atArray                    *characterAnimationNames;
    atArray                    *characterAnimations;
    vsPathMotionManager        *currentAnimation;

    atList                     *skinProgramList;

    bool                       hardwareSkinning;
    bool                       validFlag;

    vsComponent                *findLCA(atList *subMeshes);
    vsGLSLProgramAttribute     *createDefaultSkinProgram();

public:

                              vsCharacter(vsSkeleton *skeleton,
                                          vsSkeletonKinematics *skelKin,
                                          vsSkin *skin,
                                          atArray *animationNames,
                                          atArray *animations);
                              vsCharacter(atList *skeletons, atList *skelKins,
                                          atList *skins,
                                          atArray *animationNames,
                                          atArray *animations);
                              ~vsCharacter();

    virtual const char        *getClassName();

    vsCharacter               *clone();

    bool                      isValid();

    vsComponent               *getMesh();

    int                       getNumSkeletons();
    vsSkeleton                *getSkeleton(int index);

    int                       getNumSkeletonKinematics();
    vsSkeletonKinematics      *getSkeletonKinematics(int index);

    int                       getNumSkins();
    vsSkin                    *getSkin(int index);

    void                      enableHardwareSkinning();
    void                      disableHardwareSkinning();
    bool                      isHardwareSkinning();

    int                       getNumAnimations();
    atString                  getAnimationName(int index);
    vsPathMotionManager       *getAnimation(atString name);
    vsPathMotionManager       *getAnimation(int index);

    void                      switchAnimation(int index);
    void                      switchAnimation(atString name);
    void                      setCurrentAnimation(vsPathMotionManager *anim);

    vsGLSLProgramAttribute    *getSkinProgram(vsSkin *skin);
    bool                      setSkinProgram(vsSkin *skin,
                                             vsGLSLProgramAttribute *prog);

    virtual void              update();
    virtual void              update(double deltaTime);
};


#endif

