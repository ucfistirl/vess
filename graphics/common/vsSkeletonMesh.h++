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
//    VESS Module:  vsSkeletonMesh.h++
//
//    Description:  Object to manage a set of meshes that are to be updated
//                  using the same skeleton.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_SKELETON_MESH_HPP
#define VS_SKELETON_MESH_HPP

#include "vsComponent.h++"
#include "vsSkeleton.h++"
#include "vsSkeletonMeshGeometry.h++"
#include "vsGrowableArray.h++"

class VS_GRAPHICS_DLL vsSkeletonMesh : public vsUpdatable
{
private:

    vsGrowableArray           *meshList;
    int                       subMeshCount;
    vsComponent               *rootComponent;
    vsSkeleton                *skeleton;
    vsGrowableArray           *boneMatrixList;
    vsGrowableArray           *ITBoneMatrixList;

public:

                              vsSkeletonMesh(vsComponent *newRoot,
                                             vsSkeleton *newSkeleton);
    virtual                   ~vsSkeletonMesh();

    virtual const char        *getClassName();

    vsSkeletonMeshGeometry    *getSubMesh(int index);

    vsComponent               *getRootComponent();

    void                      setSkeleton(vsSkeleton *newSkeleton);
    vsSkeleton                *getSkeleton();

    virtual void              update();
};

#endif
