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
    // Initialize the pfLOD pointer
    performerLOD = NULL;
    
    // Mark this attribute as not attached
    attachedFlag = 0;
}

// ------------------------------------------------------------------------
// VESS internal function
// Constructor - Stores the Performer LOD object and corrects its closest
// range
// ------------------------------------------------------------------------
vsLODAttribute::vsLODAttribute(pfLOD *lodGroup)
{
    // Store the pointer to the pfLOD
    performerLOD = lodGroup;
    performerLOD->ref();
    
    // Set the first entry in the renge list to zero, which causes Performer
    // to not have a 'too close' range within which the object can't be seen
    performerLOD->setRange(0, 0.0);

    // Mark this attribute as attached
    attachedFlag = 1;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsLODAttribute::~vsLODAttribute()
{
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
    // Unattached LODs can't be manipulated
    if (!attachedFlag)
    {
        printf("vsLODAttribute::setRangeEnd: Attribute must be attached "
            "before LOD can be manipulated\n");
        return;
    }

    // Bounds check
    if ((childNum < 0) || (childNum >= performerLOD->getNumChildren()))
    {
        printf("vsLODAttribute::setRangeEnd: Index out of bounds\n");
        return;
    }

    // Set the desired range value in the Performer object
    performerLOD->setRange(childNum+1, rangeLimit);
}

// ------------------------------------------------------------------------
// Retrieves the far distance limit for which this child is displayed.
// The index of the first child is 0.
// ------------------------------------------------------------------------
double vsLODAttribute::getRangeEnd(int childNum)
{
    // Unattached LODs can't be manipulated
    if (!attachedFlag)
    {
        printf("vsLODAttribute::getRangeEnd: Attribute must be attached "
            "before LOD can be manipulated\n");
        return 0.0;
    }

    // Bounds check
    if ((childNum < 0) || (childNum >= performerLOD->getNumChildren()))
    {
        printf("vsLODAttribute::getRangeEnd: Index out of bounds\n");
        return 0.0;
    }

    // Get the desired range value from the Performer object
    return performerLOD->getRange(childNum+1);
}

// ------------------------------------------------------------------------
// VESS internal function
// Returns if this attribute is available to be attached to a node
// ------------------------------------------------------------------------
int vsLODAttribute::canAttach()
{
    // This attribute is not available to be attached if it is already
    // attached to another node
    if (attachedFlag)
        return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// VESS internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsLODAttribute::attach(vsNode *theNode)
{
    int loop, childCount;

    // Verify that we're not already attached to something
    if (attachedFlag)
    {
        printf("vsLODAttribute::attach: Attribute is already attached\n");
        return;
    }

    // LOD attributes may not be attached to geometry nodes
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
    
    // Set the LOD ranges to default values
    childCount = performerLOD->getNumChildren();
    for (loop = 1; loop <= childCount; loop++)
        performerLOD->setRange(loop,
            ((1000.0 * (double)loop) / (double)childCount));

    // Mark this attribute as attached
    attachedFlag = 1;
}

// ------------------------------------------------------------------------
// VESS internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsLODAttribute::detach(vsNode *theNode)
{
    pfGroup *newGroup;

    // Can't detach an attribute that is not attached
    if (!attachedFlag)
    {
        printf("vsLODAttribute::detach: Attribute is not attached\n");
        return;
    }
    
    // Replace the LOD group with an ordinary group
    newGroup = new pfGroup();
    ((vsComponent *)theNode)->replaceBottomGroup(newGroup);
    performerLOD = NULL;
    
    // Mark this attribute as unattached
    attachedFlag = 0;
}

// ------------------------------------------------------------------------
// VESS internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsLODAttribute::attachDuplicate(vsNode *theNode)
{
    vsLODAttribute *newAttrib;
    int loop;
    vsComponent *theComponent;
    
    // Make sure that it's a component that we're getting copied to
    if (theNode->getNodeType() == VS_NODE_TYPE_COMPONENT)
        theComponent = (vsComponent *)theNode;
    else
        return;

    // Create a duplicate LOD attribute
    newAttrib = new vsLODAttribute(performerLOD);

    // Attach the duplicate attribute to the specified node first, so that
    // we can manipulate its values
    theNode->addAttribute(newAttrib);

    // Copy the range values from this attribute to the duplicate
    for (loop = 0; loop < theComponent->getChildCount(); loop++)
        newAttrib->setRangeEnd(loop, getRangeEnd(loop));
}
