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
//    VESS Module:  vsSwitchAttribute.c++
//
//    Description:  Attribute that specifies which of the children of this
//                  component are to be drawn
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsSwitchAttribute.h++"
#include "vsComponent.h++"

// ------------------------------------------------------------------------
// Default constructor
// ------------------------------------------------------------------------
vsSwitchAttribute::vsSwitchAttribute()
{
    osgSwitch = NULL;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsSwitchAttribute::~vsSwitchAttribute()
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsSwitchAttribute::getClassName()
{
    return "vsSwitchAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsSwitchAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_SWITCH;
}

// ------------------------------------------------------------------------
// Retrieves the category of this attribute
// ------------------------------------------------------------------------
int vsSwitchAttribute::getAttributeCategory()
{
    return VS_ATTRIBUTE_CATEGORY_GROUPING;
}

// ------------------------------------------------------------------------
// Enables display of one of the children of the parent component. The
// index of the first child is 0.
// ------------------------------------------------------------------------
void vsSwitchAttribute::enableOne(int index)
{
    if (!attachedFlag)
    {
        printf("vsSwitchAttribute::enableOne: Attribute must be attached "
            "before switch can be manipulated\n");
        return;
    }

    // Insure the given index is valid.
    if ((index < 0) || (index >= osgSwitch->getNumChildren()))
    {
        printf("vsSwitchAttribute::enableOne: Index out of bounds\n");
        return;
    }

    osgSwitch->setValue(index, VS_TRUE);
}

// ------------------------------------------------------------------------
// Disables display of one of the children of the parent component. The
// index of the first child is 0.
// ------------------------------------------------------------------------
void vsSwitchAttribute::disableOne(int index)
{
    if (!attachedFlag)
    {
        printf("vsSwitchAttribute::disableOne: Attribute must be attached "
            "before switch can be manipulated\n");
        return;
    }

    // Insure the given index is valid.
    if ((index < 0) || (index >= osgSwitch->getNumChildren()))
    {
        printf("vsSwitchAttribute::disableOne: Index out of bounds\n");
        return;
    }

    osgSwitch->setValue(index, VS_FALSE);
}

// ------------------------------------------------------------------------
// Enables display of all of the children attached to the parent component
// ------------------------------------------------------------------------
void vsSwitchAttribute::enableAll()
{
    if (!attachedFlag)
    {
        printf("vsSwitchAttribute::enableAll: Attribute must be attached "
            "before switch can be manipulated\n");
        return;
    }

    osgSwitch->setValue(osg::Switch::ALL_CHILDREN_ON);
}

// ------------------------------------------------------------------------
// Disables display of all of the children attached to the parent component
// ------------------------------------------------------------------------
void vsSwitchAttribute::disableAll()
{
    if (!attachedFlag)
    {
        printf("vsSwitchAttribute::disableAll: Attribute must be attached "
            "before switch can be manipulated\n");
        return;
    }

    osgSwitch->setValue(osg::Switch::ALL_CHILDREN_OFF);
}

// ------------------------------------------------------------------------
// Returns a flag indicating if the child with the specified index is
// enabled. The index of the first child is 0.
// ------------------------------------------------------------------------
int vsSwitchAttribute::isEnabled(int index)
{
    if (!attachedFlag)
    {
        printf("vsSwitchAttribute::isEnabled: Attribute must be attached "
            "before switch can be manipulated\n");
        return VS_FALSE;
    }

    // Insure the given index is valid.
    if ((index < 0) || (index >= osgSwitch->getNumChildren()))
    {
        printf("vsSwitchAttribute::isEnabled: Index out of bounds\n");
        return VS_FALSE;
    }

    return osgSwitch->getValue(index);
}

// ------------------------------------------------------------------------
// Internal function
// Returns if this attribute is available to be attached to a node
// ------------------------------------------------------------------------
int vsSwitchAttribute::canAttach()
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
void vsSwitchAttribute::attach(vsNode *theNode)
{
    if (attachedFlag)
    {
        printf("vsSwitchAttribute::attach: Attribute is already attached\n");
        return;
    }

    if (theNode->getNodeType() != VS_NODE_TYPE_COMPONENT)
    {
        printf("vsSwitchAttribute::attach: Can only attach switch "
            "attributes to vsComponents\n");
        return;
    }
    
    // Replace the bottom group with a switch group
    osgSwitch = new osg::Switch();
    osgSwitch->setValue(osg::Switch::ALL_CHILDREN_OFF);
    ((vsComponent *)theNode)->replaceBottomGroup(osgSwitch);

    attachedFlag = 1;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsSwitchAttribute::detach(vsNode *theNode)
{
    osg::Group *newGroup;

    if (!attachedFlag)
    {
        printf("vsSwitchAttribute::detach: Attribute is not attached\n");
        return;
    }
    
    // Replace the switch group with an ordinary group
    newGroup = new osg::Group();
    ((vsComponent *)theNode)->replaceBottomGroup(newGroup);
    osgSwitch = NULL;
    
    attachedFlag = 0;
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsSwitchAttribute::attachDuplicate(vsNode *theNode)
{
    vsSwitchAttribute *newAttrib;
    int switchVal, loop;
    int sourceChildCount, targetChildCount, childCount;

    // Can only add switches to components
    if (theNode->getNodeType() != VS_NODE_TYPE_COMPONENT)
        return;

    newAttrib = new vsSwitchAttribute();

    theNode->addAttribute(newAttrib);
    
    // Figure out which of the source or the target has the fewest
    // children, and use that number of children for the number of
    // switch states to duplicate
    sourceChildCount = osgSwitch->getNumChildren();
    targetChildCount = ((vsComponent *)theNode)->getChildCount();
    if (sourceChildCount > targetChildCount)
        childCount = targetChildCount;
    else
        childCount = sourceChildCount;
    
    // Copy the switch values
    for (loop = 0; loop < childCount; loop++)
    {
        if (osgSwitch->getValue(loop))
            newAttrib->enableOne(loop);
        else
            newAttrib->disableOne(loop);
    }

    // If the target has more children than the source, switch off all
    // of the excess children
    for (loop = childCount; loop < targetChildCount; loop++)
        newAttrib->disableOne(loop);
}
