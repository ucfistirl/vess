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
//    VESS Module:  vsSkeleton.h++
//
//    Description:  This file manages the bone subgraph.  It maintains
//                  the boneIDs from Cal3D and generates the matrices
//                  that represent each bone.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_SKELETON_HPP
#define VS_SKELETON_HPP

#include "vsComponent.h++"
#include "vsNode.h++"
#include "vsGeometry.h++"
#include "vsGrowableArray.h++"
#include "vsUpdatable.h++"
#include "vsTransformAttribute.h++"

class VS_GRAPHICS_DLL vsSkeleton : public vsUpdatable
{
private:

    vsGrowableArray       *skeletonComponentMap;
    vsGrowableArray       *skeletonMatrices;
    vsGrowableArray       *skeletonITMatrices;
    vsGrowableArray       *skeletonBoneSpaceMatrices;
    vsComponent           *skeletonRoot;
    vsComponent           *skeletonRootBone;
    vsTransformAttribute  *skeletonTransform;
    int                   boneCount;
    int                   lastFoundIndex;
    atMatrix              offsetMatrix;
    
    void                  copySkeletonTree(vsNode *newNode, vsNode *origNode,
                                           vsGrowableArray *origMap);
VS_INTERNAL:

    void                  updateMatrices(vsNode *node,
                                         atMatrix currentMatrix);

    void                  makeBoneGeometry(vsComponent *currentBone,
                                           vsGeometry *currentBoneLine);

public:

                          vsSkeleton(vsGrowableArray *componentList,
                                     vsGrowableArray *boneSpaceMatrixList,
                                     int listLength, vsComponent *root);
                          vsSkeleton(vsSkeleton *original);
    virtual               ~vsSkeleton();

     // Inherited from vsObject
    virtual const char    *getClassName();

    vsComponent           *getBone(int boneID);

    atMatrix              *getBoneMatrix(int boneID);
    atMatrix              *getITBoneMatrix(int boneID);
    atMatrix              *getBoneSpaceMatrix(int boneID);

    vsGrowableArray       *getBoneMatrixList();
    vsGrowableArray       *getITBoneMatrixList();
    vsGrowableArray       *getBoneSpaceMatrixList();

    int                   getBoneID(vsComponent *component);
    int                   getBoneID(char *boneName);

    vsComponent           *getRoot();

    int                   getBoneCount();

    void                  makeBoneGeometry();

    void                  setOffsetMatrix(atMatrix newOffsetMatrix);
    atMatrix              getOffsetMatrix();

    void                  update();
};

#endif
