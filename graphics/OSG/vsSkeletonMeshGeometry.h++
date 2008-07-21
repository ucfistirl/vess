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
//    VESS Module:  vsSkeletonMeshGeometry.h++
//
//    Description:  vsGeometryBase subclass that handles geometry for
//                  skinned characters.  Skinning may be done in software
//                  using the additional applySkin() method of this class,
//                  or in hardware using an appropriate GPU program.
//
//    Author(s):    Duvan Cope, Bryan Kline, Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_SKELETON_MESH_GEOMETRY_HPP
#define VS_SKELETON_MESH_GEOMETRY_HPP

#include <osg/Geode>
#include <osg/Geometry>
#include "atVector.h++"
#include "atArray.h++"
#include "vsAttribute.h++"
#include "vsNode.h++"
#include "vsGeometry.h++"

#define VS_GEOMETRY_SKIN_VERTEX_COORDS 1000
#define VS_GEOMETRY_SKIN_NORMALS       1001

#define VS_GEOMETRY_BONE_INDICES       VS_GEOMETRY_USER_DATA1

class VESS_SYM vsSkeletonMeshGeometry : public vsGeometryBase
{
protected:

    osg::Vec3Array      *originalVertexList;
    osg::Vec3Array      *originalNormalList;

public:

                          vsSkeletonMeshGeometry();
    virtual               ~vsSkeletonMeshGeometry();

    virtual const char    *getClassName();

    virtual int           getNodeType();

    void                  beginNewState();
    void                  finishNewState();

    void                  setBinding(int whichData, int binding);
    int                   getBinding(int whichData);

    void                  setData(int whichData, int dataIndex, atVector data);
    atVector              getData(int whichData, int dataIndex);
    void                  setDataList(int whichData, atVector *dataBuffer);
    void                  getDataList(int whichData, atVector *dataBuffer);
    void                  setDataListSize(int whichData, int newSize);
    int                   getDataListSize(int whichData);

    void                  applySkin(atArray *boneMatrices,
                                    atArray *ITBoneMatrices);
    void                  resetSkin();
};

#endif
