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
//    VESS Module:  vsDynamicGeometry.h++
//
//    Description:  vsNode subclass that is a leaf node in a VESS scene
//                  graph. Stores geometry data such as vertex and texture
//                  coordinates, colors, and face normals.  This version
//                  is a simple subclass of vsGeometry for Open Scene
//                  Graph.  Since OSG only operates in a single process,
//                  this version does not need to do any extra work to
//                  support dynamic geometry.
//
//    Author(s):    Bryan Kline, Jason Daly, Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_DYNAMIC_GEOMETRY_HPP
#define VS_DYNAMIC_GEOMETRY_HPP

#include <osg/Geode>
#include <osg/Geometry>
#include "vsVector.h++"
#include "vsAttribute.h++"
#include "vsNode.h++"
#include "vsGeometry.h++"

class VS_GRAPHICS_DLL vsDynamicGeometry : public vsNode
{
private:

    vsGrowableArray     parentList;
    int                 parentCount;

    osg::Geode          *osgGeode;
    osg::Geometry       *osgGeometry;

    osg::Vec4Array      *colorList;
    int                 colorListSize;
    osg::Vec3Array      *normalList;
    int                 normalListSize;
    osg::Vec2Array      *texCoordList[VS_MAXIMUM_TEXTURE_UNITS];
    int                 texCoordListSize[VS_MAXIMUM_TEXTURE_UNITS];
    osg::Vec3Array      *vertexList;
    int                 vertexListSize;

    int                 textureBinding[VS_MAXIMUM_TEXTURE_UNITS];

    int                 *lengthsList;
    int                 primitiveCount;
    int                 primitiveType;
    
    bool                lightingEnable;
    
    int                 renderBin;

    void                rebuildPrimitives();

VS_INTERNAL:

    virtual bool    addParent(vsNode *newParent);
    virtual bool    removeParent(vsNode *targetParent);

    virtual void    applyAttributes();

public:

                          vsDynamicGeometry();
    virtual               ~vsDynamicGeometry();

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

    osg::Geode            *getBaseLibraryObject();
};

#endif
