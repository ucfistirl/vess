// File vsNode.h++

#ifndef VS_NODE_HPP
#define VS_NODE_HPP

#include <Performer/pf/pfNode.h>
#include "vsGrowableArray.h++"
#include "vsAttributeList.h++"
#include "vsVector.h++"
#include "vsMatrix.h++"

#define VS_NODE_NAME_MAX_LENGTH 80

class vsComponent;

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

    void            addParent(vsComponent *newParent);
    void            removeParent(vsComponent *targetParent);

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
    virtual             ~vsNode();

    virtual int         getNodeType() = 0;

    int                 getParentCount();
    vsComponent         *getParent(int index);
    
    void                setName(const char *newName);
    const char          *getName();
    virtual vsNode      *findNodeByName(const char *targetName) = 0;
    
    virtual void        getBoundSphere(vsVector *centerPoint,
                                       double *radius) = 0;
    virtual vsMatrix    getGlobalXform() = 0;
    
    virtual void            setIntersectValue(unsigned int newValue) = 0;
    virtual unsigned int    getIntersectValue() = 0;

    virtual void    addAttribute(vsAttribute *newAttribute);
    virtual void    removeAttribute(vsAttribute *targetAttribute);
};

#endif

