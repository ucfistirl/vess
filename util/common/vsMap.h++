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
//    VESS Module:  vsMap.h++
//
//    Description:  Version of atMap for VESS.  This version provides
//                  vsObject reference counting.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#ifndef VS_MAP_HPP
#define VS_MAP_HPP

#include "vsObject.h++"
#include "vsList.h++"

enum vsMapColor
{
    VS_MAP_BLACK,
    VS_MAP_RED
};

enum vsMapChildType
{
    VS_MAP_LEFTCHILD,
    VS_MAP_RIGHTCHILD,
    VS_MAP_ROOTNODE
};

struct vsMapNode
{
    vsMapNode *   leftChild;
    vsMapNode *   rightChild;
    vsMapNode *   parent;
    int           color;
    
    vsObject *    nodeKey;
    vsObject *    nodeValue;
};

class VESS_SYM vsMap : public vsObject
{
private:

    vsMapNode *   treeRoot;
    u_long        treeSize;
    
    vsMapNode *       findNode(vsMapNode *node, vsObject *key);
    void              rebalanceInsert(vsMapNode *node);
    void              rebalanceDelete(vsMapNode *parent,
                                      int deletedChildType);
    void              deleteNode(vsMapNode *node);
    void              removeNode(vsMapNode *node);
    vsMapNode *       getInorderSuccessor(vsMapNode *node);
    void              rotateLeft(vsMapNode *node);
    void              rotateRight(vsMapNode *node);
    void              deleteTree(vsMapNode *node);
    int               getChildType(vsMapNode *node);
    void              fillLists(vsMapNode *node, vsList *keyList,
                                vsList *valueList);
    void              printTree(vsMapNode *node, int indent);

public:

                           vsMap();
    virtual                ~vsMap();

    virtual const char *   getClassName();

    bool                   addEntry(vsObject *key, vsObject *value);
    bool                   removeEntry(vsObject *key);
    u_long                 getNumEntries();
    
    bool                   containsKey(vsObject *key);
    vsObject *             getValue(vsObject *key);
    bool                   changeValue(vsObject *key, vsObject *value);

    void                   clear();

    void                   getSortedList(vsList *keyList,
                                         vsList *valueList);

    void                   print();
};

#endif
