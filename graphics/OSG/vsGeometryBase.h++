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
//    VESS Module:  vsGeometryBase.h++
//
//    Description:  vsNode subclass that is a base class for all geometry
//                  nodes.  Geometries are leaf nodes in a VESS scene
//                  graph. They store geometry data such as vertex and
//                  texture coordinates, colors, and face normals.
//
//    Author(s):    Bryan Kline, Duvan Cope, Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_GEOMETRY_BASE_HPP
#define VS_GEOMETRY_BASE_HPP

#include "atVector.h++"
#include "vsAttribute.h++"
#include "vsNode.h++"
#include "vsRenderBin.h++"
#include <osg/Geode>
#include <osg/Geometry>

enum vsGeometryPrimType
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

// Within this enum, each pair of values actually represents the same data
// area; vertex coordinates use up the same space as generic attribute #0
// does, and so on. However, to prevent accidentally clobbering existing data,
// only one constant out of each pair may be used at a time. For example, if a
// geometry object already has vertex coordinates, then any attempt to read
// from or write to generic attribute #0 will fail. In order to use the other
// constant for the pair, the currently existing list must be emptied (by
// using the setDataListSize() method to set its size to zero). After that the
// new list, using the other constant, can be initialized by calling
// setDataListSize() with that constant. Following the example again, to
// switch from using conventional vertex coordinates to generic attribute #0,
// you would call:
//
//      setDataListSize(VS_GEOMETRY_VERTEX_COORDS, 0);
//
// And then follow it with:
//
//      setDataListSize(VS_GEOMETRY_GENERIC_0, newSize);
//
// The reason for this weirdness is mostly because of how OpenGL handles
// generic attributes. (If you don't know what those are, then you probably
// won't need to use them.) For more information, try looking in the OpenGL
// Extensions Registry documentation for ARB_vertex_program here:
//
//  http://oss.sgi.com/projects/ogl-sample/registry/ARB/vertex_program.txt

enum vsGeometryDataType
{
    VS_GEOMETRY_VERTEX_COORDS   = 0,
    VS_GEOMETRY_GENERIC_0       = 16,

    VS_GEOMETRY_VERTEX_WEIGHTS  = 1,
    VS_GEOMETRY_GENERIC_1       = 17,

    VS_GEOMETRY_NORMALS         = 2,
    VS_GEOMETRY_GENERIC_2       = 18,

    VS_GEOMETRY_COLORS          = 3,
    VS_GEOMETRY_GENERIC_3       = 19,

    VS_GEOMETRY_ALT_COLORS      = 4,
    VS_GEOMETRY_GENERIC_4       = 20,

    VS_GEOMETRY_FOG_COORDS      = 5,
    VS_GEOMETRY_GENERIC_5       = 21,

    VS_GEOMETRY_USER_DATA0      = 6,
    VS_GEOMETRY_GENERIC_6       = 22,

    VS_GEOMETRY_USER_DATA1      = 7,
    VS_GEOMETRY_GENERIC_7       = 23,

    VS_GEOMETRY_TEXTURE0_COORDS = 8,
    VS_GEOMETRY_GENERIC_8       = 24,

    VS_GEOMETRY_TEXTURE1_COORDS = 9,
    VS_GEOMETRY_GENERIC_9       = 25,

    VS_GEOMETRY_TEXTURE2_COORDS = 10,
    VS_GEOMETRY_GENERIC_10      = 26,

    VS_GEOMETRY_TEXTURE3_COORDS = 11,
    VS_GEOMETRY_GENERIC_11      = 27,

    VS_GEOMETRY_TEXTURE4_COORDS = 12,
    VS_GEOMETRY_GENERIC_12      = 28,

    VS_GEOMETRY_TEXTURE5_COORDS = 13,
    VS_GEOMETRY_GENERIC_13      = 29,

    VS_GEOMETRY_TEXTURE6_COORDS = 14,
    VS_GEOMETRY_GENERIC_14      = 30,

    VS_GEOMETRY_TEXTURE7_COORDS = 15,
    VS_GEOMETRY_GENERIC_15      = 31
};

// Set the default texture unit to the zeroth unit.  For convenience and
// backwards compatability.
#define VS_GEOMETRY_TEXTURE_COORDS VS_GEOMETRY_TEXTURE0_COORDS

enum vsGeometryDataBinding
{
    VS_GEOMETRY_BIND_NONE,
    VS_GEOMETRY_BIND_OVERALL,
    VS_GEOMETRY_BIND_PER_PRIMITIVE,
    VS_GEOMETRY_BIND_PER_VERTEX
};

#define VS_GEOMETRY_MAX_LIST_INDEX 1000000

// The maximum texture units that VESS can support.
#define VS_MAXIMUM_TEXTURE_UNITS 8

#define VS_GEOMETRY_LIST_COUNT 16

class VESS_SYM vsGeometryBase : public vsNode
{
protected:

    vsGrowableArray     parentList;
    int                 parentCount;

    osg::Geode          *osgGeode;
    osg::Geometry       *osgGeometry;

    osg::Array          *dataList[VS_GEOMETRY_LIST_COUNT];
    int                 dataListSize[VS_GEOMETRY_LIST_COUNT];
    bool                dataIsGeneric[VS_GEOMETRY_LIST_COUNT];

    int                 textureBinding[VS_MAXIMUM_TEXTURE_UNITS];

    u_int               *indexList;
    int                 indexListSize;

    int                 *lengthsList;
    int                 primitiveCount;
    int                 primitiveType;
    
    bool                lightingEnable;
    
    vsRenderBin         *renderBin;

    void                rebuildPrimitives();

    int                 getDataElementCount(int whichData);
    void                allocateDataArray(int whichData);
    void                notifyOSGDataChanged(int whichData);

    bool                areVerticesEquivalent(int v1, int v2);

VS_INTERNAL:

    virtual bool    addParent(vsNode *newParent);
    virtual bool    removeParent(vsNode *targetParent);

    virtual void    applyAttributes();

    virtual void    getAxisAlignedBoxBounds(atVector *minValues, 
                                            atVector *maxValues);

public:

                          vsGeometryBase();
    virtual               ~vsGeometryBase();

    virtual const char    *getClassName() = 0;

    virtual int           getNodeType() = 0;

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
    
    void                  setRenderBin(vsRenderBin *newBin);
    vsRenderBin           *getRenderBin();
    
    virtual void          getBoundSphere(atVector *centerPoint, double *radius);
    virtual atMatrix      getGlobalXform();

    virtual void            setIntersectValue(unsigned int newValue);
    virtual unsigned int    getIntersectValue();

    virtual void          addAttribute(vsAttribute *newAttribute);

    virtual void          enableCull();
    virtual void          disableCull();

    virtual void          deindexGeometry();
    virtual void          expandToPerVertex(int whichData);
    virtual void          optimizeVertices();

    osg::Geode            *getBaseLibraryObject();
};

#endif
