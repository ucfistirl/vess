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
//                  uses Performer's pfFlux to handle dynamic geometry.
//
//    Author(s):    Bryan Kline, Jason Daly, Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_DYNAMIC_GEOMETRY_HPP
#define VS_DYNAMIC_GEOMETRY_HPP

#include <Performer/pr/pfFlux.h>
#include <Performer/pf/pfGeode.h>
#include <Performer/pr/pfGeoSet.h>
#include "vsVector.h++"
#include "vsAttribute.h++"
#include "vsGeometry.h++"

class VS_GRAPHICS_DLL vsDynamicGeometry : public vsNode
{
private:

    vsGrowableArray    parentList;
    int                parentCount;

    pfGeode            *performerGeode;
    pfFlux             *performerFlux;
    pfGeoSet           *performerGeoset;
    pfGeoState         *performerGeostate;

    pfVec4             *colorList;
    int                colorListSize;
    pfVec3             *normalList;
    int                normalListSize;
    pfVec2             *texCoordList[VS_MAXIMUM_TEXTURE_UNITS];
    int                texCoordListSize[VS_MAXIMUM_TEXTURE_UNITS];
    pfVec3             *vertexList;
    int                vertexListSize;
    int                *lengthsList;

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

    static int         initFluxedGeoSet(pfFluxMemory *fluxMem);

    void               inflateFlatGeometry();

    static int         preCullNode(pfTraverser *trav, void *data);

VS_INTERNAL:

    virtual bool    addParent(vsNode *newParent);
    virtual bool    removeParent(vsNode *targetParent);

    virtual void    applyAttributes();

    static int      geostateCallback(pfGeoState *gstate, void *userData);

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

    pfGeode               *getBaseLibraryObject();
};

#endif
