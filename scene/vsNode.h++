// File vsNode.h++

#ifndef VS_NODE_HPP
#define VS_NODE_HPP

class vsNode;

#include <Performer/pf/pfNode.h>
#include "vsAttributeList.h++"

#define VS_NODE_MAX_PARENTS     32
#define VS_NODE_NAME_MAX_LENGTH 80

enum vsNodeType
{
    VS_NODE_TYPE_COMPONENT,
    VS_NODE_TYPE_GEOMETRY
};

class vsNode : public vsAttributeList
{
protected:

    int         parentCount;
    vsNode      *parentList[VS_NODE_MAX_PARENTS];

    char        nodeName[VS_NODE_NAME_MAX_LENGTH];

VS_INTERNAL:

    void          addParent(vsNode *newParent);
    void          removeParent(vsNode *targetParent);

    void          saveCurrentAttributes();
    void          applyAttributes();
    void          restoreSavedAttributes();

    static int    preDrawCallback(pfTraverser *_trav, void *_userData);
    static int    postDrawCallback(pfTraverser *_trav, void *_userData);

public:

                      vsNode();
                      ~vsNode();

    virtual int       getNodeType() = 0;

    vsNode            *getParent(int index);
    
    void              setName(const char *newName);
    const char        *getName();
    virtual vsNode    *findNodeByName(const char *targetName) = 0;
};

#endif

