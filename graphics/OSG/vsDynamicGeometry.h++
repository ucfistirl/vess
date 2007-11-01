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
//                  coordinates, colors, and face normals.  Under Open
//                  Scene Graph, the only real difference between this 
//                  class and vsGeometry is that this class disables the
//                  use of display lists.
//
//    Author(s):    Bryan Kline, Jason Daly, Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_DYNAMIC_GEOMETRY_HPP
#define VS_DYNAMIC_GEOMETRY_HPP

#include <osg/Geode>
#include <osg/Geometry>
#include "atVector.h++"
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
};

#endif
