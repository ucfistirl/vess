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
//    VESS Module:  vsComponent.h++
//
//    Description:  vsNode subclass that acts as a non-leaf part of a
//                  VESS scene graph
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_COMPONENT_HPP
#define VS_COMPONENT_HPP

#include "vsAttribute.h++"
#include "vsBox.h++"
#include "vsGrowableArray.h++"
#include "vsNode.h++"
#include "atVector.h++"
#include <osg/Group>

class VESS_SYM vsComponent : public vsNode
{
private:

    int                childCount;
    vsGrowableArray    childList;

    vsNode             *parentNode;

    osg::Group         *topGroup;
    osg::Group         *lightHook;
    osg::Group         *bottomGroup;

    virtual vsNode    *cloneTreeRecursive();
    virtual bool      addChild(vsNode *newChild, bool dirtyFlag);

VS_INTERNAL:

    osg::Group        *getTopGroup();
    osg::Group        *getLightHook();
    osg::Group        *getBottomGroup();
    void              replaceBottomGroup(osg::Group *newGroup);

    virtual bool      addParent(vsNode *newParent);
    virtual bool      removeParent(vsNode *targetParent);

    virtual void      getAxisAlignedBoxBounds(atVector * minValues,
                                              atVector * maxValues);

public:

                          vsComponent();
    virtual               ~vsComponent();

    virtual const char    *getClassName();

    virtual vsNode        *cloneTree();

    virtual int           getNodeType();

    virtual bool          addChild(vsNode *newChild);
    virtual bool          insertChild(vsNode *newChild, int index);
    virtual bool          removeChild(vsNode *targetChild);
    virtual bool          replaceChild(vsNode *targetChild, vsNode *newChild);

    virtual int           getParentCount();
    virtual vsNode        *getParent(int index);
    virtual int           getChildCount();
    virtual vsNode        *getChild(int index);

    virtual void          getBoundSphere(atVector *centerPoint, double *radius);
    virtual atMatrix      getGlobalXform();

    virtual void            setIntersectValue(unsigned int newValue);
    virtual unsigned int    getIntersectValue();

    virtual void          addAttribute(vsAttribute *newAttribute);
    
    virtual void          enableLighting();
    virtual void          disableLighting();

    virtual void          enableCull();
    virtual void          disableCull();

    osg::Group            *getBaseLibraryObject();
};

#endif
