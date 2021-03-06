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
//    VESS Module:  vsOptimizer.h++
//
//    Description:  Class for reorganizing a VESS scene graph in order to
//                  increase rendering speed
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_OPTIMIZER_HPP
#define VS_OPTIMIZER_HPP

#include "vsAttribute.h++"
#include "vsGeometry.h++"
#include "vsComponent.h++"

#define VS_OPTIMIZER_PROMOTE_ATTRIBUTES 0x01
#define VS_OPTIMIZER_MERGE_GEOMETRY     0x02
#define VS_OPTIMIZER_MERGE_DECALS       0x04
#define VS_OPTIMIZER_CLEAN_TREE         0x08
#define VS_OPTIMIZER_SORT_CHILDREN      0x10
#define VS_OPTIMIZER_MERGE_LODS         0x20
#define VS_OPTIMIZER_CONDENSE_COLORS    0x40
#define VS_OPTIMIZER_CONDENSE_NORMALS   0x80

#define VS_OPTIMIZER_ALL                0xFFFFFFFF

class VESS_SYM vsOptimizer : public vsObject
{
private:

    int         passMask;

    void        optimizeNode(vsNode *node);

    void        cleanChildren(vsComponent *componentNode);
    void        zapComponent(vsComponent *targetComponent);

    void        mergeDecals(vsComponent *componentNode);
    
    void        mergeLODs(vsComponent *componentNode);

    void        mergeGeometry(vsComponent *componentNode);
    bool        isSimilarGeometry(vsGeometry *firstGeo, vsGeometry *secondGeo);
    void        addGeometry(vsGeometry *destGeo, vsGeometry *srcGeo);

    void        condenseGeoData(vsGeometry *geometry, int whichData);

    void        optimizeAttributes(vsComponent *componentNode,
                                   int attributeType);
    void        sortLists(int *countArray, vsAttribute **attrArray,
                          int listSize);

    void        sortByAttribute(vsComponent *componentNode, int attributeType);

public:

                       vsOptimizer();
    virtual            ~vsOptimizer();

    virtual const char *getClassName();

    void               optimize(vsNode *rootNode);

    void               setOptimizations(int mask);
    int                getOptimizations();
};

#endif
