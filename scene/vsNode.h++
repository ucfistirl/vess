// File vsNode.h++

#ifndef VS_NODE_HPP
#define VS_NODE_HPP

#include <Performer/pf/pfNode.h>
#include "vsGrowableArray.h++"
#include "vsAttributeList.h++"
#include "vsVector.h++"

#define VS_NODE_NAME_MAX_LENGTH 80

enum vsNodeType
{
    VS_NODE_TYPE_COMPONENT,
    VS_NODE_TYPE_GEOMETRY
};

class vsNode : public vsAttributeList
{
protected:

    vsGrowableArray    parentList;
    int                parentCount;

    char               nodeName[VS_NODE_NAME_MAX_LENGTH];
    
    int                dirtyFlag;

VS_INTERNAL:

    void            addParent(vsNode *newParent);
    void            removeParent(vsNode *targetParent);

    virtual void    saveCurrentAttributes();
    virtual void    applyAttributes();
    virtual void    restoreSavedAttributes();
    
    void            dirty();
    void            clean();
    int             isDirty();

    void	    dirtyUp();
    virtual void    dirtyDown();

public:

                      vsNode();
                      ~vsNode();

    virtual int       getNodeType() = 0;

    int               getParentCount();
    vsNode            *getParent(int index);
    
    void              setName(const char *newName);
    const char        *getName();
    virtual vsNode    *findNodeByName(const char *targetName) = 0;
    
    virtual void      getBoundSphere(vsVector *centerPoint, double *radius) = 0;
};

#endif

