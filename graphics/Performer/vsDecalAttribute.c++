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
//    VESS Module:  vsDecalAttribute.c++
//
//    Description:  Attribute that specifies that the children of the
//                  component be drawn with different depth offsets in
//                  order to reduce z-fighting
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsDecalAttribute.h++"

#include "vsComponent.h++"

// ------------------------------------------------------------------------
// Default Constructor
// ------------------------------------------------------------------------
vsDecalAttribute::vsDecalAttribute()
{
    performerLayer = NULL;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsDecalAttribute::~vsDecalAttribute()
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsDecalAttribute::getClassName()
{
    return "vsDecalAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsDecalAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_DECAL;
}

// ------------------------------------------------------------------------
// Retrieves the category of this attribute
// ------------------------------------------------------------------------
int vsDecalAttribute::getAttributeCategory()
{
    return VS_ATTRIBUTE_CATEGORY_GROUPING;
}

// ------------------------------------------------------------------------
// Internal function
// Returns if this attribute is available to be attached to a node
// ------------------------------------------------------------------------
bool vsDecalAttribute::canAttach()
{
    // This attribute is not available to be attached if it is already
    // attached to another node
    if (attachedCount)
        return false;

    return true;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsDecalAttribute::attach(vsNode *theNode)
{
    // Verify that we're not already attached to something
    if (attachedCount)
    {
        printf("vsDecalAttribute::attach: Attribute is already attached\n");
        return;
    }

    // Switch attributes may not be attached to geometry nodes
    if ((theNode->getNodeType() == VS_NODE_TYPE_GEOMETRY) ||
        (theNode->getNodeType() == VS_NODE_TYPE_DYNAMIC_GEOMETRY))
    {
        printf("vsDecalAttribute::attach: Can't attach decal attributes "
            "to geometry nodes\n");
        return;
    }
    
    // Replace the bottom group with a new layer group
    performerLayer = new pfLayer();
    ((vsComponent *)theNode)->replaceBottomGroup(performerLayer);

    // Set the mode of the pfLayer object to use the 'displacement with
    // constant offset' depth value manipulation mode
    performerLayer->setMode(PFDECAL_BASE_DISPLACE | PFDECAL_LAYER_OFFSET);

    // Mark this attribute as attached
    attachedCount = 1;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsDecalAttribute::detach(vsNode *theNode)
{
    pfGroup *newGroup;

    // Can't detach an attribute that is not attached
    if (!attachedCount)
    {
        printf("vsDecalAttribute::attach: Attribute is not attached\n");
        return;
    }
    
    // Replace the layer with an ordinary group
    newGroup = new pfGroup();
    ((vsComponent *)theNode)->replaceBottomGroup(newGroup);
    performerLayer = NULL;
    
    // Mark this attribute as unattached
    attachedCount = 0;
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsDecalAttribute::attachDuplicate(vsNode *theNode)
{
    vsDecalAttribute *newAttrib;

    // Create a duplicate decal attribute
    newAttrib = new vsDecalAttribute();

    // Attach the duplicate attribute to the specified node
    theNode->addAttribute(newAttrib);
}
