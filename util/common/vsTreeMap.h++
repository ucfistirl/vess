
#ifndef VS_TREE_MAP_HPP
#define VS_TREE_MAP_HPP

#include "atGlobals.h++"
#include "atList.h++"
#include "atItem.h++"
#include "atNotifier.h++"
#include "atOSDefs.h++"
#include "atPair.h++"

enum vsTreeMapColor
{
    VS_TREE_MAP_BLACK,
    VS_TREE_MAP_RED
};

enum vsTreeMapChildType
{
    VS_TREE_MAP_LEFTCHILD,
    VS_TREE_MAP_RIGHTCHILD,
    VS_TREE_MAP_ROOTNODE
};

struct vsTreeMapNode
{
    vsTreeMapNode *   leftChild;
    vsTreeMapNode *   rightChild;
    vsTreeMapNode *   parent;
    int               color;
    
    atItem *          nodeKey;
    atItem *          nodeValue;
};

class VESS_SYM vsTreeMap : public atNotifier
{
private:

    vsTreeMapNode *   treeRoot;
    u_long            treeSize;
    
    void              clear();

    vsTreeMapNode *   findNode(vsTreeMapNode * node, atItem * key);
    void              rebalanceInsert(vsTreeMapNode * node);
    void              rebalanceDelete(vsTreeMapNode * parent,
                                      int deletedChildType);
    void              deleteNode(vsTreeMapNode * node);
    void              removeNode(vsTreeMapNode * node);
    vsTreeMapNode *   getInorderSuccessor(vsTreeMapNode * node);
    void              rotateLeft(vsTreeMapNode * node);
    void              rotateRight(vsTreeMapNode * node);
    void              deleteTree(vsTreeMapNode * node, bool deleteContents);
    int               getChildType(vsTreeMapNode * node);
    void              fillLists(vsTreeMapNode * node, atList * keyList,
                                atList * valueList);
    void              printTree(vsTreeMapNode * node, int indent);

public:

                vsTreeMap();
    virtual     ~vsTreeMap();

    bool        addEntry(atItem * key, atItem * value);
    atPair *    removeEntry(atItem * key);
    void        removeAllEntries();
    u_long      getNumEntries();
    
    bool        containsKey(atItem * key);
    atItem *    getValue(atItem * key);
    atItem *    changeValue(atItem * key, atItem * newValue);

    void        getSortedList(atList * keyList,
                              atList * valueList);

    void        print();
};

#endif
