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

#include <Performer/pf/pfGroup.h>
#include "vsGrowableArray.h++"
#include "vsVector.h++"
#include "vsAttribute.h++"
#include "vsNode.h++"

class vsComponent : public vsNode
{
private:

    int                childCount;
    vsGrowableArray    childList;
    
    vsNode             *parentNode;

    pfGroup            *topGroup;
    pfGroup            *lightHook;
    pfGroup            *bottomGroup;


VS_INTERNAL:

    pfGroup           *getTopGroup();
    pfGroup           *getLightHook();
    pfGroup           *getBottomGroup();

    void              replaceBottomGroup(pfGroup *newGroup);

    virtual int       addParent(vsNode *newParent);
    virtual int       removeParent(vsNode *targetParent);

public:

                          vsComponent();
    virtual               ~vsComponent();

    virtual const char    *getClassName();

    virtual vsNode        *cloneTree();
    virtual void          deleteTree();

    virtual int           getNodeType();

    virtual int           addChild(vsNode *newChild);
    virtual int           insertChild(vsNode *newChild, int index);
    virtual int           removeChild(vsNode *targetChild);
    virtual int           replaceChild(vsNode *targetChild, vsNode *newChild);

    virtual int           getParentCount();
    virtual vsNode        *getParent(int index);
    virtual int           getChildCount();
    virtual vsNode        *getChild(int index);

    virtual void          getBoundSphere(vsVector *centerPoint, double *radius);
    virtual vsMatrix      getGlobalXform();

    virtual void            setIntersectValue(unsigned int newValue);
    virtual unsigned int    getIntersectValue();

    virtual void          addAttribute(vsAttribute *newAttribute);

    virtual void          enableCull();
    virtual void          disableCull();

    pfGroup               *getBaseLibraryObject();
};

#endif
