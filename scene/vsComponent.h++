// File vsComponent.h++

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

    virtual void            setIntersectMask(unsigned int newMask);
    virtual unsigned int    getIntersectMask();

    virtual void        addAttribute(vsAttribute *newAttribute);

    pfGroup             *getBaseLibraryObject();
};

#endif
