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
//    VESS Module:  vsNode.h++
//
//    Description:  Abstract parent class for all objects that can be a
//                  part of a VESS scene graph
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_NODE_HPP
#define VS_NODE_HPP

#include <Performer/pf/pfNode.h>
#include "vsObject.h++"
#include "vsGrowableArray.h++"
#include "vsAttribute.h++"
#include "vsVector.h++"
#include "vsMatrix.h++"

#define VS_NODE_NAME_MAX_LENGTH 80

class vsComponent;

enum vsNodeType
{
    VS_NODE_TYPE_COMPONENT,
    VS_NODE_TYPE_GEOMETRY,
    VS_NODE_TYPE_DYNAMIC_GEOMETRY
};

class vsNode : public vsObject
{
protected:

    vsGrowableArray    parentList;
    int                parentCount;

    char               nodeName[VS_NODE_NAME_MAX_LENGTH];

    vsGrowableArray    attributeList;
    int                attributeCount;

    int                dirtyFlag;

VS_INTERNAL:

    virtual vsNode    *nodeSearch(const char *name, int *idx) = 0;

    void              addParent(vsComponent *newParent);
    void              removeParent(vsComponent *targetParent);

    virtual void      saveCurrentAttributes();
    virtual void      applyAttributes();
    virtual void      restoreSavedAttributes();

    void              dirty();
    void              clean();
    int               isDirty();

    void              dirtyUp();
    virtual void      dirtyDown();

public:

                        vsNode();
    virtual             ~vsNode();

    virtual vsNode      *cloneTree();

    virtual int         getNodeType() = 0;

    int                 getParentCount();
    vsComponent         *getParent(int index);

    void                setName(const char *newName);
    const char          *getName();
    virtual vsNode      *findNodeByName(const char *targetName);
    virtual vsNode      *findNodeByName(const char *targetName, int index);

    virtual void        getBoundSphere(vsVector *centerPoint,
                                       double *radius) = 0;
    virtual vsMatrix    getGlobalXform() = 0;

    virtual void            setIntersectValue(unsigned int newValue) = 0;
    virtual unsigned int    getIntersectValue() = 0;

    virtual void            setVisibilityValue(unsigned int newValue) = 0;
    virtual unsigned int    getVisibilityValue() = 0;

    virtual void        addAttribute(vsAttribute *newAttribute);
    virtual void        removeAttribute(vsAttribute *targetAttribute);

    int                 getAttributeCount();
    vsAttribute         *getAttribute(int index);
    vsAttribute         *getTypedAttribute(int attribType, int index);
    vsAttribute         *getCategoryAttribute(int attribCategory, int index);
    vsAttribute         *getNamedAttribute(char *attribName);
};

#endif
