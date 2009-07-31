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
//    VESS Module:  vsSkin.h++
//
//    Description:  Object to manage a set of meshes that are to be updated
//                  using the same skeleton.
//
//    Author(s):    Duvan Cope, Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_SKIN_HPP
#define VS_SKIN_HPP

#include "vsComponent.h++"
#include "vsSkeleton.h++"
#include "vsSkeletonMeshGeometry.h++"
#include "vsArray.h++"
#include "atArray.h++"

class VESS_SYM vsSkin : public vsUpdatable
{
private:

    vsArray                   *meshList;
    int                       subMeshCount;
    vsComponent               *rootComponent;
    vsSkeleton                *skeleton;
    atArray                   *boneMatrixList;
    atArray                   *boneSpaceMatrixList;

    atArray                   *skinMatrixList;
    atArray                   *skinITMatrixList;

    bool                      *boneUsed;

    void                      findSubmeshes(vsNode *node);
    void                      findUsedBones();

public:

                              vsSkin(vsComponent *newRoot,
                                     vsSkeleton *newSkeleton,
                                     atArray *boneSpaceMatrices);
                              vsSkin(vsSkin *original);
    virtual                   ~vsSkin();

    virtual const char        *getClassName();

    int                       getNumSubMeshes();
    vsSkeletonMeshGeometry    *getSubMesh(int index);

    vsComponent               *getRootComponent();

    void                      setSkeleton(vsSkeleton *newSkeleton);
    vsSkeleton                *getSkeleton();

    bool                      usesBone(int boneIndex);
    atMatrix                  getSkinMatrix(int boneIndex);

    virtual void              update();

    void                      applySkin();
    void                      reset();
};

#endif
