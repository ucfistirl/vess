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
//    VESS Module:  vsTreeMap.h++
//
//    Description:  Utility class that implements a mapping from key
//                  values to data values, stored using a red-black
//                  tree algorithm
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_TREE_MAP_HPP
#define VS_TREE_MAP_HPP

#include "vsGrowableArray.h++"

enum VS_UTIL_DLL vsTreeMapColor
{
    VS_TREE_MAP_BLACK,
    VS_TREE_MAP_RED
};

enum VS_UTIL_DLL vsTreeMapChildType
{
    VS_TREE_MAP_LEFTCHILD,
    VS_TREE_MAP_RIGHTCHILD,
    VS_TREE_MAP_ROOTNODE
};

struct VS_UTIL_DLL vsTreeMapNode
{
    vsTreeMapNode   *leftChild;
    vsTreeMapNode   *rightChild;
    vsTreeMapNode   *parent;
    int             color;
    
    void            *nodeKey;
    void            *nodeValue;
};

class VS_UTIL_DLL vsTreeMap
{
private:

    vsTreeMapNode    *treeRoot;
    int              treeSize;
    
    vsTreeMapNode    *findNode(vsTreeMapNode *node, void *key);
    void             rebalanceInsert(vsTreeMapNode *node);
    void             rebalanceDelete(vsTreeMapNode *parent,
                                     int deletedChildType);
    void             deleteNode(vsTreeMapNode *node);
    vsTreeMapNode    *getInorderSuccessor(vsTreeMapNode *node);
    void             rotateLeft(vsTreeMapNode *node);
    void             rotateRight(vsTreeMapNode *node);
    void             deleteTree(vsTreeMapNode *node);
    int              getChildType(vsTreeMapNode *node);
    void             fillArrays(vsTreeMapNode *node, int *arrayPos,
                                vsGrowableArray *keyList,
                                vsGrowableArray *valueList);

public:

                vsTreeMap();
                ~vsTreeMap();

    int         addEntry(void *key, void *value);
    int         deleteEntry(void *key);
    int         getEntryCount();
    
    int         containsKey(void *key);
    void        *getValue(void *key);
    int         changeValue(void *key, void *newValue);

    void        clear();

    void        getSortedList(vsGrowableArray *keyList,
                              vsGrowableArray *valueList);
};




#endif
