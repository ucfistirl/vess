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
//    VESS Module:  vsGeometry.h++
//
//    Description:  vsNode subclass that is a leaf node in a VESS scene
//                  graph. Stores geometry data such as vertex and texture
//                  coordinates, colors, and face normals.
//
//    Author(s):    Bryan Kline, Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_GEOMETRY_HPP
#define VS_GEOMETRY_HPP

#include <Performer/pf/pfGeode.h>
#include <Performer/pr/pfGeoSet.h>
#include "vsVector.h++"
#include "vsAttribute.h++"
#include "vsNode.h++"

enum VS_GRAPHICS_DLL vsGeometryPrimType
{
    VS_GEOMETRY_TYPE_POINTS,
    VS_GEOMETRY_TYPE_LINES,
    VS_GEOMETRY_TYPE_LINE_STRIPS,
    VS_GEOMETRY_TYPE_LINE_LOOPS,
    VS_GEOMETRY_TYPE_TRIS,
    VS_GEOMETRY_TYPE_TRI_STRIPS,
    VS_GEOMETRY_TYPE_TRI_FANS,
    VS_GEOMETRY_TYPE_QUADS,
    VS_GEOMETRY_TYPE_QUAD_STRIPS,
    VS_GEOMETRY_TYPE_POLYS
};

enum VS_GRAPHICS_DLL vsGeometryDataType
{
    VS_GEOMETRY_SKIN_VERTEX_COORDS,
    VS_GEOMETRY_SKIN_NORMALS,
    VS_GEOMETRY_VERTEX_WEIGHTS,
    VS_GEOMETRY_BONE_INDICES,
    VS_GEOMETRY_VERTEX_COORDS,
    VS_GEOMETRY_NORMALS,
    VS_GEOMETRY_COLORS,
    VS_GEOMETRY_TEXTURE0_COORDS,
    VS_GEOMETRY_TEXTURE1_COORDS,
    VS_GEOMETRY_TEXTURE2_COORDS,
    VS_GEOMETRY_TEXTURE3_COORDS,
    VS_GEOMETRY_TEXTURE4_COORDS,
    VS_GEOMETRY_TEXTURE5_COORDS,
    VS_GEOMETRY_TEXTURE6_COORDS,
    VS_GEOMETRY_TEXTURE7_COORDS,
    VS_GEOMETRY_TEXTURE_COORDS = VS_GEOMETRY_TEXTURE0_COORDS
};

enum VS_GRAPHICS_DLL vsGeometryDataBinding
{
    VS_GEOMETRY_BIND_NONE,
    VS_GEOMETRY_BIND_OVERALL,
    VS_GEOMETRY_BIND_PER_PRIMITIVE,
    VS_GEOMETRY_BIND_PER_VERTEX
};

enum VS_GRAPHICS_DLL vsGeometryBinSortMode
{
    VS_GEOMETRY_SORT_STATE,
    VS_GEOMETRY_SORT_DEPTH
};

// The maximum texture units that VESS can support.
#define VS_MAXIMUM_TEXTURE_UNITS PFGS_MAX_TEXTURES

class VS_GRAPHICS_DLL vsGeometry : public vsNode
{
private:

    vsGrowableArray     parentList;
    int                 parentCount;

    pfGeode             *performerGeode;
    pfGeoSet            *performerGeoset;
    pfGeoState          *performerGeostate;

    pfVec4              *colorList;
    int                 colorListSize;
    pfVec3              *normalList;
    int                 normalListSize;
    pfVec2              *texCoordList[VS_MAXIMUM_TEXTURE_UNITS];
    int                 texCoordListSize[VS_MAXIMUM_TEXTURE_UNITS];
    pfVec3              *vertexList;
    int                 vertexListSize;
    int                 *lengthsList;

    int                 lightingEnable;

    pfLight             **lightsList;

    static vsTreeMap    *binModeList;
    int                 renderBin;

VS_INTERNAL:

    static bool         binModesChanged;
    static vsTreeMap    *getBinModeList();

    virtual bool        addParent(vsNode *newParent);
    virtual bool        removeParent(vsNode *targetParent);

    virtual void        applyAttributes();

    static int          geostateCallback(pfGeoState *gstate, void *userData);

public:

                          vsGeometry();
    virtual               ~vsGeometry();

    virtual const char    *getClassName();

    virtual int           getNodeType();

    virtual int           getParentCount();
    virtual vsNode        *getParent(int index);

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
    
    static void           setBinSortMode(int binNum, int sortMode);
    static int            getBinSortMode(int binNum);
    static void           clearBinSortModes();
    
    virtual void          getBoundSphere(vsVector *centerPoint, double *radius);
    virtual vsMatrix      getGlobalXform();

    virtual void            setIntersectValue(unsigned int newValue);
    virtual unsigned int    getIntersectValue();

    virtual void          addAttribute(vsAttribute *newAttribute);

    virtual void          enableCull();
    virtual void          disableCull();

    pfGeode               *getBaseLibraryObject();
};

#endif
