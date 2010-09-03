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
//    VESS Module:  vsUnmanagedNode.h++
//
//    Description:  A wrapper for base library objects than can be
//                  visualized in a VESS scene graph but not manipulated.
//
//    Author(s):    Bryan Kline, Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_UNMANAGED_NODE_HPP
#define VS_UNMANAGED_NODE_HPP

#include "vsAttribute.h++"
#include "vsArray.h++"
#include "atMatrix.h++"
#include "vsNode.h++"
#include "atVector.h++"
#include <osg/Group>

class VESS_SYM vsUnmanagedNode : public vsNode
{
private:

    vsArray            parentList;

    osg::Node          *osgNode;

VS_INTERNAL:

    virtual bool       addParent(vsNode *newParent);
    virtual bool       removeParent(vsNode *targetParent);

    virtual void       saveCurrentAttributes();
    virtual void       applyAttributes();
    virtual void       restoreSavedAttributes();

    virtual void      getAxisAlignedBoxBounds(atVector * minValues,
                                              atVector * maxValues);

public:

                        vsUnmanagedNode(osg::Node *newNode);
    virtual             ~vsUnmanagedNode();

    virtual const char    *getClassName();

    virtual vsNode      *cloneTree();
    virtual void        deleteTree();

    virtual int         getNodeType();

    virtual bool        addChild(vsNode *newChild);
    virtual bool        insertChild(vsNode *newChild, int index);
    virtual bool        removeChild(vsNode *targetChild);
    virtual bool        replaceChild(vsNode *targetChild, vsNode *newChild);

    virtual int         getParentCount();
    virtual vsNode      *getParent(int index);
    virtual int         getChildCount();
    virtual vsNode      *getChild(int index);

    virtual void        getBoundSphere(atVector *centerPoint, double *radius);
    virtual atMatrix    getGlobalXform();

    virtual void            setIntersectValue(unsigned int newValue);
    virtual unsigned int    getIntersectValue();

    virtual void        disableLighting();
    virtual void        enableLighting();

    virtual void        addAttribute(vsAttribute *newAttribute);
    virtual void        removeAttribute(vsAttribute *targetAttribute);

    virtual void        enableCull();
    virtual void        disableCull();

    virtual osg::Node    *getBaseLibraryObject();
};

#endif
