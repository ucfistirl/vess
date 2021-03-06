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
#include "vsDecalCallback.h++"

// ------------------------------------------------------------------------
// Default Constructor
// ------------------------------------------------------------------------
vsDecalAttribute::vsDecalAttribute()
{
    // Create a callback object and set it to use this attribute
    decalCallback = new vsDecalCallback(this);
    decalCallback->ref();
    
    bottomGroup = NULL;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsDecalAttribute::~vsDecalAttribute()
{
    if (isAttached())
        detach(NULL);

    decalCallback->unref();
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
// Returns a clone of this attribute
// ------------------------------------------------------------------------
vsAttribute *vsDecalAttribute::clone()
{
    vsDecalAttribute *newAttrib;

    // This attribute has no state of its own, so just create a new one
    // and return it
    newAttrib = new vsDecalAttribute();
    return newAttrib;
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

    if (theNode->getNodeType() != VS_NODE_TYPE_COMPONENT)
    {
        printf("vsDecalAttribute::attach: Can only attach decal "
            "attributes to vsComponents\n");
        return;
    }
    
    // Grab the bottom osg group of the component and attach the decal
    // callback object to it
    bottomGroup = ((vsComponent *)theNode)->getBottomGroup();
    bottomGroup->ref();
    bottomGroup->setCullCallback(decalCallback);

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
    // Can't detach an unattached attribute
    if (!attachedCount)
    {
        printf("vsDecalAttribute::detach: Attribute is not attached\n");
        return;
    }
    
    // Remove the decal's callback hook
    bottomGroup->setCullCallback(NULL);
    
    // Release the bottom group
    bottomGroup->unref();
    bottomGroup = NULL;

    attachedCount = 0;
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsDecalAttribute::attachDuplicate(vsNode *theNode)
{
    // Attach a clone of this attribute to the specified node
    theNode->addAttribute(this->clone());
}
