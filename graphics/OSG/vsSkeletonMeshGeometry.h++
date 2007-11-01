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
//    Description:  vsNode subclass that is a leaf node in a VESS scene
//                  graph. Stores geometry data such as vertex and texture
//                  coordinates, colors, and face normals.  This version
//                  is a simple subclass of vsGeometry for Open Scene
//                  Graph.  Since OSG only operates in a single process,
//                  this version does not need to do any extra work to
//                  support dynamic geometry.  This specialized form of
//                  vsDynamicGeometry handles all the data for skins, and
//                  applying them using the main processor (instead of
//                  the graphics hardware).
//
//    Author(s):    Duvan Cope, Bryan Kline, Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_SKELETON_MESH_GEOMETRY_HPP
#define VS_SKELETON_MESH_GEOMETRY_HPP

#include <osg/Geode>
#include <osg/Geometry>
#include "atVector.h++"
#include "vsAttribute.h++"
#include "vsNode.h++"
#include "vsGeometry.h++"

#define VS_GEOMETRY_SKIN_VERTEX_COORDS 1000
#define VS_GEOMETRY_SKIN_NORMALS       1001

#define VS_GEOMETRY_BONE_INDICES       VS_GEOMETRY_USER_DATA1

class VS_GRAPHICS_DLL vsSkeletonMeshGeometry : public vsNode
{
private:

    vsGrowableArray     parentList;
    int                 parentCount;

    osg::Geode          *osgGeode;
    osg::Geometry       *osgGeometry;

    osg::Array          *dataList[VS_GEOMETRY_LIST_COUNT];
    int                 dataListSize[VS_GEOMETRY_LIST_COUNT];
    bool                dataIsGeneric[VS_GEOMETRY_LIST_COUNT];

    int                 textureBinding[VS_MAXIMUM_TEXTURE_UNITS];

    osg::Vec3Array      *originalVertexList;
    osg::Vec3Array      *originalNormalList;

    u_int               *indexList;
    int                 indexListSize;

    int                 *lengthsList;
    int                 primitiveCount;
    int                 primitiveType;
    
    bool                lightingEnable;
    
    int                 renderBin;

    void                rebuildPrimitives();

    int                 getDataElementCount(int whichData);
    void                allocateDataArray(int whichData);
    void                notifyOSGDataChanged(int whichData);

VS_INTERNAL:

    virtual bool    addParent(vsNode *newParent);
    virtual bool    removeParent(vsNode *targetParent);

    virtual void    applyAttributes();

    virtual void    getAxisAlignedBoxBounds(atVector *minValues, 
                                            atVector *maxValues);

public:

                          vsSkeletonMeshGeometry();
    virtual               ~vsSkeletonMeshGeometry();

    virtual const char    *getClassName();

    virtual int           getNodeType();

    virtual int           getParentCount();
    virtual vsNode        *getParent(int index);

    void                  beginNewState();
    void                  finishNewState();

    void                  setPrimitiveType(int newType);
    int                   getPrimitiveType();

    void                  setPrimitiveCount(int newCount);
    int                   getPrimitiveCount();

    void                  setPrimitiveLength(int index, int length);
    int                   getPrimitiveLength(int index);
    void                  setPrimitiveLengths(int *lengths);
    void                  getPrimitiveLengths(int *lengthsBuffer);

    void                  setBinding(int whichData, int binding);
    int                   getBinding(int whichData);

    void                  setData(int whichData, int dataIndex, atVector data);
    atVector              getData(int whichData, int dataIndex);
    void                  setDataList(int whichData, atVector *dataBuffer);
    void                  getDataList(int whichData, atVector *dataBuffer);
    void                  setDataListSize(int whichData, int newSize);
    int                   getDataListSize(int whichData);

    void                  setIndex(int indexIndex, u_int index);
    u_int                 getIndex(int indexIndex);
    void                  setIndexList(u_int *indexBuffer);
    void                  getIndexList(u_int *indexBuffer);
    void                  setIndexListSize(int newSize);
    int                   getIndexListSize();
    
    virtual void          enableLighting();
    virtual void          disableLighting();
    bool                  isLightingEnabled();
    
    void                  setRenderBin(int binNum);
    int                   getRenderBin();
    
    virtual void          getBoundSphere(atVector *centerPoint, double *radius);
    virtual atMatrix      getGlobalXform();

    virtual void            setIntersectValue(unsigned int newValue);
    virtual unsigned int    getIntersectValue();

    virtual void          addAttribute(vsAttribute *newAttribute);

    virtual void          enableCull();
    virtual void          disableCull();

    osg::Geode            *getBaseLibraryObject();

    void                  applySkin(vsGrowableArray *boneMatrices,
                                    vsGrowableArray *ITBoneMatrices);
    void                  resetSkin();
};

#endif
