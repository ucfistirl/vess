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

class vsAttribute;

#include "vsObject.h++"
#include "vsGrowableArray.h++"
#include "vsVector.h++"
#include "vsMatrix.h++"
#include "vsAttribute.h++"

#define VS_NODE_NAME_MAX_LENGTH 80

enum vsNodeType
{
    VS_NODE_TYPE_COMPONENT,
    VS_NODE_TYPE_GEOMETRY,
    VS_NODE_TYPE_DYNAMIC_GEOMETRY,
    VS_NODE_TYPE_SCENE
};

class vsNode : public vsObject
{
private:

    static vsObjectMap *nodeMap;

    char               nodeName[VS_NODE_NAME_MAX_LENGTH];

    virtual vsNode     *nodeSearch(const char *name, int *idx);

protected:

    vsGrowableArray    attributeList;
    int                attributeCount;

    int                dirtyFlag;

VS_INTERNAL:

    static vsObjectMap *getMap();
    static void        deleteMap();

    virtual int        addParent(vsNode *newParent);
    virtual int        removeParent(vsNode *targetParent);

    virtual void       saveCurrentAttributes();
    virtual void       applyAttributes();
    virtual void       restoreSavedAttributes();

    void               dirty();
    void               clean();
    int                isDirty();

    void               dirtyUp();
    void               dirtyDown();

public:

                        vsNode();
    virtual             ~vsNode();

    virtual vsNode      *cloneTree();
    virtual void        deleteTree();

    virtual int         getNodeType() = 0;

    virtual int         addChild(vsNode *newChild);
    virtual int         insertChild(vsNode *newChild, int index);
    virtual int         removeChild(vsNode *targetChild);
    virtual int         replaceChild(vsNode *targetChild, vsNode *newChild);

    virtual int         getParentCount();
    virtual vsNode      *getParent(int index);
    virtual int         getChildCount();
    virtual vsNode      *getChild(int index);

    void                setName(const char *newName);
    const char          *getName();
    virtual vsNode      *findNodeByName(const char *targetName);
    virtual vsNode      *findNodeByName(const char *targetName, int index);

    virtual void        getBoundSphere(vsVector *centerPoint,
                                       double *radius) = 0;
    virtual vsMatrix    getGlobalXform() = 0;

    virtual void            setIntersectValue(unsigned int newValue) = 0;
    virtual unsigned int    getIntersectValue() = 0;

    virtual void        addAttribute(vsAttribute *newAttribute);
    virtual void        removeAttribute(vsAttribute *targetAttribute);

    int                 getAttributeCount();
    vsAttribute         *getAttribute(int index);
    vsAttribute         *getTypedAttribute(int attribType, int index);
    vsAttribute         *getCategoryAttribute(int attribCategory, int index);
    vsAttribute         *getNamedAttribute(char *attribName);
};

#endif
