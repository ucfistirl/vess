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
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_GEOMETRY_HPP
#define VS_GEOMETRY_HPP

#include <Performer/pf/pfGeode.h>
#include "vsVector.h++"
#include "vsAttribute.h++"
#include "vsNode.h++"

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

enum vsGeometryDataType
{
    VS_GEOMETRY_VERTEX_COORDS,
    VS_GEOMETRY_NORMALS,
    VS_GEOMETRY_COLORS,
    VS_GEOMETRY_TEXTURE_COORDS
};

enum vsGeometryDataBinding
{
    VS_GEOMETRY_BIND_NONE,
    VS_GEOMETRY_BIND_OVERALL,
    VS_GEOMETRY_BIND_PER_PRIMITIVE,
    VS_GEOMETRY_BIND_PER_VERTEX
};

class vsGeometry : public vsNode
{
private:

    pfGeode         *performerGeode;
    pfGeoSet        *performerGeoset;
    pfGeoState      *performerGeostate;
    
    pfVec4          *colorList;
    int             colorListSize;
    pfVec3          *normalList;
    int             normalListSize;
    pfVec2          *texCoordList;
    int             texCoordListSize;
    pfVec3          *vertexList;
    int             vertexListSize;
    int             *lengthsList;
    
    pfLight         **lightsList;

    void            inflateFlatGeometry();

VS_INTERNAL:

                    vsGeometry(pfGeode *targetGeode);

    virtual void    applyAttributes();
    
    static int      geostateCallback(pfGeoState *gstate, void *userData);

public:

                        vsGeometry();
    virtual             ~vsGeometry();

    virtual int         getNodeType();
    virtual vsNode      *findNodeByName(const char *targetName);

    void                setPrimitiveType(int newType);
    int                 getPrimitiveType();

    void                setPrimitiveCount(int newCount);
    int                 getPrimitiveCount();

    void                setPrimitiveLength(int index, int length);
    int                 getPrimitiveLength(int index);
    void                setPrimitiveLengths(int *lengths);
    void                getPrimitiveLengths(int *lengthsBuffer);

    void                setBinding(int whichData, int binding);
    int                 getBinding(int whichData);

    void                setData(int whichData, int dataIndex, vsVector data);
    vsVector            getData(int whichData, int dataIndex);
    void                setDataList(int whichData, vsVector *dataList);
    void                getDataList(int whichData, vsVector *dataBuffer);
    void                setDataListSize(int whichData, int newSize);
    int                 getDataListSize(int whichData);
    
    virtual void        getBoundSphere(vsVector *centerPoint, double *radius);
    virtual vsMatrix    getGlobalXform();

    virtual void            setIntersectValue(unsigned int newValue);
    virtual unsigned int    getIntersectValue();

    virtual void        addAttribute(vsAttribute *newAttribute);

    pfGeode             *getBaseLibraryObject();
};

#endif
