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
#include "vsDatabaseLoader.h++"

class vsComponent : public vsNode
{
private:

    int                childCount;
    vsGrowableArray    childList;

    pfGroup            *topGroup;
    pfGroup            *lightHook;
    pfGroup            *bottomGroup;

VS_INTERNAL:

                    vsComponent(pfGroup *targetGraph,
                                vsDatabaseLoader *nameDirectory);

    int             handleName(pfNode *targetNode,
                               vsDatabaseLoader *nameDirectory);

    pfGroup         *getTopGroup();
    pfGroup         *getLightHook();
    pfGroup         *getBottomGroup();
    void            setBottomGroup(pfGroup *newBottom);
    void            replaceBottomGroup(pfGroup *newGroup);

    virtual void    dirtyDown();

public:

                        vsComponent();
    virtual             ~vsComponent();

    void		deleteTree();

    virtual int         getNodeType();
    virtual vsNode      *findNodeByName(const char *targetName);

    void                addChild(vsNode *newChild);
    void                insertChild(vsNode *newChild, int index);
    void                removeChild(vsNode *targetChild);
    void                replaceChild(vsNode *targetChild, vsNode *newChild);

    int                 getChildCount();
    vsNode              *getChild(int index);

    virtual void        getBoundSphere(vsVector *centerPoint, double *radius);
    virtual vsMatrix    getGlobalXform();

    virtual void            setIntersectValue(unsigned int newValue);
    virtual unsigned int    getIntersectValue();

    virtual void        addAttribute(vsAttribute *newAttribute);

    pfGroup             *getBaseLibraryObject();
};

#endif
