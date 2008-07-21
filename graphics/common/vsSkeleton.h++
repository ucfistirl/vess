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
#include "atArray.h++"
#include "vsUpdatable.h++"
#include "vsTransformAttribute.h++"

class VESS_SYM vsSkeleton : public vsUpdatable
{
private:

    atArray               *skeletonComponentMap;
    atArray               *skeletonMatrices;
    vsComponent           *skeletonRoot;
    int                   boneCount;
    int                   lastFoundIndex;
    atMatrix              offsetMatrix;
    
    void                  copySkeletonTree(vsNode *newNode, vsNode *origNode,
                                           atArray *origMap);
VS_INTERNAL:

    void                  updateMatrices(vsNode *node,
                                         atMatrix currentMatrix);

    void                  makeBoneGeometry(vsComponent *currentBone,
                                           vsGeometry *currentBoneLine);

public:

                          vsSkeleton(atArray *componentList, int listLength,
                                     vsComponent *root);
                          vsSkeleton(vsSkeleton *original);
    virtual               ~vsSkeleton();

    virtual const char    *getClassName();

    vsComponent           *getBone(int boneID);

    atMatrix              *getBoneMatrix(int boneID);
    atArray               *getBoneMatrixList();

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
