// File vsComponent.h++

#ifndef VS_COMPONENT_HPP
#define VS_COMPONENT_HPP

class vsComponent;

#include <Performer/pf/pfGroup.h>
#include <Performer/pf/pfSwitch.h>
#include <Performer/pf/pfSequence.h>
#include <Performer/pf/pfLOD.h>
#include <Performer/pf/pfGeode.h>

#include "vsSystem.h++"
#include "vsTransformAttribute.h++"
#include "vsSwitchAttribute.h++"
#include "vsSequenceAttribute.h++"
#include "vsLODAttribute.h++"
#include "vsBillboardAttribute.h++"
#include "vsGeometry.h++"
#include "vsDatabaseLoader.h++"
#include "vsNode.h++"

#define VS_COMPONENT_MAX_CHILDREN 100

class vsComponent : public vsNode
{
private:

    int         childCount;
    vsNode      *childList[VS_COMPONENT_MAX_CHILDREN];

    pfGroup     *topGroup;
    pfGroup     *lightHook;
    pfGroup     *bottomGroup;


VS_INTERNAL:

                vsComponent(pfGroup *targetGraph,
                            vsDatabaseLoader *nameDirectory);

    int         handleName(pfNode *targetNode,
                           vsDatabaseLoader *nameDirectory);

    pfGroup     *getTopGroup();
    pfGroup     *getLightHook();
    pfGroup     *getBottomGroup();
    void        setBottomGroup(pfGroup *newBottom);
    void        replaceBottomGroup(pfGroup *newGroup);

public:

                      vsComponent();
    virtual           ~vsComponent();

    virtual int       getNodeType();
    virtual vsNode    *findNodeByName(const char *targetName);

    void              addChild(vsNode *newChild);
    void              removeChild(vsNode *targetChild);

    int               getChildCount();
    vsNode            *getChild(int index);

    void              getBoundSphere(vsVector *centerPoint, double *radius);

    virtual void      addAttribute(vsAttribute *newAttribute);
    virtual void      removeAttribute(vsAttribute *targetAttribute);

    pfGroup           *getBaseLibraryObject();
};

#endif

