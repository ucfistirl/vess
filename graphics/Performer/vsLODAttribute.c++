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
//    VESS Module:  vsLODAttribute.c++
//
//    Description:  Specifies that the children of the component are all
//                  levels-of-detail of the same object and are not to be
//                  drawn all at the same time; only one of the children
//                  should be drawn, with the determination of which to
//                  draw based on the distance from the viewer to the
//                  object.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsLODAttribute.h++"

#include "vsComponent.h++"

// ------------------------------------------------------------------------
// Default Constructor
// ------------------------------------------------------------------------
vsLODAttribute::vsLODAttribute()
{
    performerLOD = NULL;
    
    attachedFlag = 0;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsLODAttribute::~vsLODAttribute()
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsLODAttribute::getClassName()
{
    return "vsLODAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsLODAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_LOD;
}

// ------------------------------------------------------------------------
// Retrieves the category of this attribute
// ------------------------------------------------------------------------
int vsLODAttribute::getAttributeCategory()
{
    return VS_ATTRIBUTE_CATEGORY_GROUPING;
}

// ------------------------------------------------------------------------
// Sets the far limit for which the child with index childNum on the
// parent component is displayed. The near limit is the far limit of the
// child with the next lower index, or 0 for the child at index 0. The
// first child has an index of 0.
// ------------------------------------------------------------------------
void vsLODAttribute::setRangeEnd(int childNum, double rangeLimit)
{
    if (!attachedFlag)
    {
        printf("vsLODAttribute::setRangeEnd: Attribute must be attached "
            "before LOD can be manipulated\n");
        return;
    }

    if ((childNum < 0) || (childNum >= performerLOD->getNumChildren()))
    {
        printf("vsLODAttribute::setRangeEnd: Index %d out of bounds (%d-%d)\n",
            childNum, 0, performerLOD->getNumChildren());
        return;
    }

    performerLOD->setRange(childNum+1, rangeLimit);
}

// ------------------------------------------------------------------------
// Retrieves the far distance limit for which this child is displayed.
// The index of the first child is 0.
// ------------------------------------------------------------------------
double vsLODAttribute::getRangeEnd(int childNum)
{
    if (!attachedFlag)
    {
        printf("vsLODAttribute::getRangeEnd: Attribute must be attached "
            "before LOD can be manipulated\n");
        return 0.0;
    }

    if ((childNum < 0) || (childNum >= performerLOD->getNumChildren()))
    {
        printf("vsLODAttribute::getRangeEnd: Index out of bounds\n");
        return 0.0;
    }

    return performerLOD->getRange(childNum+1);
}

// ------------------------------------------------------------------------
// Internal function
// Returns if this attribute is available to be attached to a node
// ------------------------------------------------------------------------
int vsLODAttribute::canAttach()
{
    if (attachedFlag)
        return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsLODAttribute::attach(vsNode *theNode)
{
    int loop, childCount;

    if (attachedFlag)
    {
        printf("vsLODAttribute::attach: Attribute is already attached\n");
        return;
    }

    if ((theNode->getNodeType() == VS_NODE_TYPE_GEOMETRY) ||
        (theNode->getNodeType() == VS_NODE_TYPE_DYNAMIC_GEOMETRY))
    {
        printf("vsLODAttribute::attach: Can't attach LOD attributes to "
            "geometry nodes\n");
        return;
    }
    
    // Replace the bottom group with an LOD group
    performerLOD = new pfLOD();
    ((vsComponent *)theNode)->replaceBottomGroup(performerLOD);

    // Set the first range on the Performer LOD to 0.0, so the highest
    // level of detail is used when close
    performerLOD->setRange(0, 0.0);
    
    // Set the remaining ranges to a good default distribution for the LOD
    // children
    childCount = performerLOD->getNumChildren();
    for (loop = 1; loop <= childCount; loop++)
        performerLOD->setRange(loop,
            ((1000.0 * (double)loop) / (double)childCount));

    attachedFlag = 1;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsLODAttribute::detach(vsNode *theNode)
{
    pfGroup *newGroup;

    if (!attachedFlag)
    {
        printf("vsLODAttribute::detach: Attribute is not attached\n");
        return;
    }
    
    // Replace the LOD group with an ordinary group
    newGroup = new pfGroup();
    ((vsComponent *)theNode)->replaceBottomGroup(newGroup);
    performerLOD = NULL;
    
    attachedFlag = 0;
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsLODAttribute::attachDuplicate(vsNode *theNode)
{
    vsLODAttribute *newAttrib;
    int loop;
    vsComponent *theComponent;
    
    if (theNode->getNodeType() == VS_NODE_TYPE_COMPONENT)
        theComponent = (vsComponent *)theNode;
    else
        return;

    newAttrib = new vsLODAttribute();

    theNode->addAttribute(newAttrib);

    for (loop = 0; loop < theComponent->getChildCount(); loop++)
        newAttrib->setRangeEnd(loop, getRangeEnd(loop));
}
