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
//                  uses Performer's pfFlux to handle dynamic geometry.
//
//    Author(s):    Bryan Kline, Jason Daly, Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_SKELETON_MESH_GEOMETRY_HPP
#define VS_SKELETON_MESH_GEOMETRY_HPP

#include <Performer/pr/pfFlux.h>
#include <Performer/pf/pfGeode.h>
#include <Performer/pr/pfGeoArray.h>
#include "vsVector.h++"
#include "vsAttribute.h++"
#include "vsGeometry.h++"

#define VS_WEIGHT_ATTRIBUTE_INDEX    1
#define VS_BONE_ATTRIBUTE_INDEX      7

class VS_GRAPHICS_DLL vsSkeletonMeshGeometry : public vsNode
{
private:

    vsGrowableArray    parentList;
    int                parentCount;

    pfGeode            *performerGeode;
    pfFlux             *performerFlux;
    pfGeoArray         *performerGeoarray;
    pfGeoState         *performerGeostate;

    pfVec4             *colorList;
    int                colorListSize;
    pfVec3             *originalNormalList;
    pfVec3             *normalList;
    int                normalListSize;
    pfVec2             *texCoordList[VS_MAXIMUM_TEXTURE_UNITS];
    int                texCoordListSize[VS_MAXIMUM_TEXTURE_UNITS];
    pfVec3             *originalVertexList;
    pfVec3             *vertexList;
    int                vertexListSize;
    int                *lengthsList;

    pfVec4             *boneList;
    int                boneListSize;
    pfVec4             *weightList;
    int                weightListSize;

    // Attributes for the pfGeoArray to hold the bone and weight values. 
    pfVertexAttr       *weightAttr;
    pfVertexAttr       *boneAttr;

    // Storage for some of the geometry attributes that
    // may stay static
    int                primitiveType;
    int                primitiveCount;
    int                colorBinding;
    int                normalBinding;
    int                texCoordBinding[VS_MAXIMUM_TEXTURE_UNITS];
    int                vertexBinding;

    pfLight            **lightsList;

    int                renderBin;

    static int         initFluxedGeoArray(pfFluxMemory *fluxMem);

    void               inflateFlatGeometry();

    static int         preCullNode(pfTraverser *trav, void *data);

VS_INTERNAL:

    virtual bool    addParent(vsNode *newParent);
    virtual bool    removeParent(vsNode *targetParent);

    virtual void    applyAttributes();

    static int      geostateCallback(pfGeoState *gstate, void *userData);

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

    void                  setData(int whichData, int dataIndex, vsVector data);
    vsVector              getData(int whichData, int dataIndex);
    void                  setDataList(int whichData, vsVector *dataList);
    void                  getDataList(int whichData, vsVector *dataBuffer);
    void                  setDataListSize(int whichData, int newSize);
    int                   getDataListSize(int whichData);

    void                  enableLighting();
    void                  disableLighting();
    bool                  isLightingEnabled();

    void                  setRenderBin(int binNum);
    int                   getRenderBin();
    
    virtual void          getBoundSphere(vsVector *centerPoint, 
                                         double *radius);
    virtual vsMatrix      getGlobalXform();

    virtual void            setIntersectValue(unsigned int newValue);
    virtual unsigned int    getIntersectValue();

    virtual void          addAttribute(vsAttribute *newAttribute);

    virtual void          enableCull();
    virtual void          disableCull();

    pfGeode               *getBaseLibraryObject();

    void                  applySkin(vsGrowableArray *boneMatrices,
                                    vsGrowableArray *ITBoneMatrices);
};

#endif
