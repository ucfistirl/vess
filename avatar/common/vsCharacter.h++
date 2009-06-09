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


class VESS_SYM vsCharacter : public vsUpdatable
{
protected:

    atList                     *characterSkeletons;
    atList                     *skeletonKinematics;
    atList                     *characterSkins;
    vsComponent                *characterMesh;

    atArray                    *characterAnimationNames;
    atArray                    *characterAnimations;

    vsPathMotionManager        *currentAnimation;

    vsPathMotionManager        *defaultAnimation;

    bool                       loopStarted;
    vsPathMotionManager        *loopingAnimation;

    bool                       oneTimeStarted;
    double                     oneTimeTransOutTime;
    vsPathMotionManager        *oneTimeAnimation;
    bool                       finalStarted;

    bool                       transitioning;
    vsPathMotionManager        *transitionAnimation;

    atList                     *skinProgramList;

    bool                       hardwareSkinning;
    bool                       validFlag;

    vsComponent                *findLCA(atList *subMeshes);
    vsGLSLProgramAttribute     *createDefaultSkinProgram();

    void                    transitionToAnimation(vsPathMotionManager * target,
                                                  double transitionTime);
    void                    finishTransition();

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
    vsPathMotionManager       *getCurrentAnimation();
    vsPathMotionManager       *getAnimation(atString name);
    vsPathMotionManager       *getAnimation(int index);

    void                      switchAnimation(int index);
    void                      switchAnimation(atString name);
    void                      setCurrentAnimation(vsPathMotionManager *anim);

    void                      setDefaultAnimation(atString name,
                                                  double transitionTime);
    void                      startOneTimeAnimation(atString name,
                                                    double transInTime,
                                                    double transOutTime);
    void                      startLoopingAnimation(atString name,
                                                    double transitionTime);
    void                      finishLoopingAnimation(double transitionTime);
    void                      startFinalAnimation(atString name,
                                                  double transitionTime);
    bool                      isAnimationFinal();
    void                      restartAnimation();

    vsGLSLProgramAttribute    *getSkinProgram(vsSkin *skin);
    bool                      setSkinProgram(vsSkin *skin,
                                             vsGLSLProgramAttribute *prog);

    virtual void              update();
    virtual void              update(double deltaTime);
};


#endif

