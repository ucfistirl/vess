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
// VESS internal function
// Constructor - Stores the Performer Layer object
// ------------------------------------------------------------------------
vsDecalAttribute::vsDecalAttribute(pfLayer *layerGroup)
{
    performerLayer = layerGroup;
    performerLayer->ref();
    
    attachedFlag = 1;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsDecalAttribute::~vsDecalAttribute()
{
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
// VESS internal function
// Returns if this attribute is available to be attached to a node
// ------------------------------------------------------------------------
int vsDecalAttribute::canAttach()
{
    if (attachedFlag)
        return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// VESS internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsDecalAttribute::attach(vsNode *theNode)
{
    if (attachedFlag)
    {
        printf("vsDecalAttribute::attach: Attribute is already attached\n");
        return;
    }

    if (theNode->getNodeType() == VS_NODE_TYPE_GEOMETRY)
    {
        printf("vsDecalAttribute::attach: Can't attach decal attributes "
            "to geometry nodes\n");
        return;
    }
    
    // Replace the bottom group with a layer group
    performerLayer = new pfLayer();
    ((vsComponent *)theNode)->replaceBottomGroup(performerLayer);

    performerLayer->setMode(PFDECAL_BASE_DISPLACE | PFDECAL_LAYER_OFFSET);

    attachedFlag = 1;
}

// ------------------------------------------------------------------------
// VESS internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsDecalAttribute::detach(vsNode *theNode)
{
    pfGroup *newGroup;

    if (!attachedFlag)
    {
        printf("vsDecalAttribute::attach: Attribute is not attached\n");
        return;
    }
    
    // Replace the layer with an ordinary group
    newGroup = new pfGroup();
    ((vsComponent *)theNode)->replaceBottomGroup(newGroup);
    performerLayer = NULL;
    
    attachedFlag = 0;
}
