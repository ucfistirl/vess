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

#include "vsSwitchAttribute.h++"

#include "vsComponent.h++"

// ------------------------------------------------------------------------
// Default constructor
// ------------------------------------------------------------------------
vsSwitchAttribute::vsSwitchAttribute()
{
    // Initialize the Performer switch pointer to NULL
    performerSwitch = NULL;
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
    // Unattached switches can't be manipulated
    if (!attachedFlag)
    {
        printf("vsSwitchAttribute::enableOne: Attribute must be attached "
            "before switch can be manipulated\n");
        return;
    }

    // Bounds checking
    if ((index < 0) || (index >= performerSwitch->getNumChildren()))
    {
        printf("vsSwitchAttribute::enableOne: Index out of bounds\n");
        return;
    }

    // Set the desired child to be active in the Performer switch
    performerSwitch->setVal(index);
}

// ------------------------------------------------------------------------
// Disables display of one of the children of the parent component. The
// index of the first child is 0.
// ------------------------------------------------------------------------
void vsSwitchAttribute::disableOne(int index)
{
    // Unattached switches can't be manipulated
    if (!attachedFlag)
    {
        printf("vsSwitchAttribute::disableOne: Attribute must be attached "
            "before switch can be manipulated\n");
        return;
    }

    // Bounds checking
    if ((index < 0) || (index >= performerSwitch->getNumChildren()))
    {
        printf("vsSwitchAttribute::disableOne: Index out of bounds\n");
        return;
    }

    // If the specified child is the active child, deactivate it on
    // the Performer switch
    if (index == performerSwitch->getVal())
        performerSwitch->setVal(PFSWITCH_OFF);
}

// ------------------------------------------------------------------------
// Enables display of all of the children attached to the parent component
// ------------------------------------------------------------------------
void vsSwitchAttribute::enableAll()
{
    // Unattached switches can't be manipulated
    if (!attachedFlag)
    {
        printf("vsSwitchAttribute::enableAll: Attribute must be attached "
            "before switch can be manipulated\n");
        return;
    }

    // Activate all of the children on the Performer switch
    performerSwitch->setVal(PFSWITCH_ON);
}

// ------------------------------------------------------------------------
// Disables display of all of the children attached to the parent component
// ------------------------------------------------------------------------
void vsSwitchAttribute::disableAll()
{
    // Unattached switches can't be manipulated
    if (!attachedFlag)
    {
        printf("vsSwitchAttribute::disableAll: Attribute must be attached "
            "before switch can be manipulated\n");
        return;
    }

    // Deactivate all of the children on the Performer switch
    performerSwitch->setVal(PFSWITCH_OFF);
}

// ------------------------------------------------------------------------
// Returns a flag indicating if the child with the specified index is
// enabled. The index of the first child is 0.
// ------------------------------------------------------------------------
bool vsSwitchAttribute::isEnabled(int index)
{
    // Unattached switches can't be manipulated
    if (!attachedFlag)
    {
        printf("vsSwitchAttribute::isEnabled: Attribute must be attached "
            "before switch can be manipulated\n");
        return false;
    }

    // Bounds check
    if ((index < 0) || (index >= performerSwitch->getNumChildren()))
    {
        printf("vsSwitchAttribute::isEnabled: Index out of bounds\n");
        return false;
    }

    // The child is on if that one child is on or if they are all on
    if ((performerSwitch->getVal() == PFSWITCH_ON) ||
        (performerSwitch->getVal() == index))
        return true;
    else
        return false;
}

// ------------------------------------------------------------------------
// Internal function
// Returns if this attribute is available to be attached to a node
// ------------------------------------------------------------------------
bool vsSwitchAttribute::canAttach()
{
    // This attribute is not available to be attached if it is already
    // attached to another node
    if (attachedFlag)
        return false;

    return true;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsSwitchAttribute::attach(vsNode *theNode)
{
    // Verify that we're not already attached to something
    if (attachedFlag)
    {
        printf("vsSwitchAttribute::attach: Attribute is already attached\n");
        return;
    }

    // Switch attributes may not be attached to geometry nodes
    if ((theNode->getNodeType() == VS_NODE_TYPE_GEOMETRY) ||
        (theNode->getNodeType() == VS_NODE_TYPE_DYNAMIC_GEOMETRY))
    {
        printf("vsSwitchAttribute::attach: Can't attach switch attributes to "
            "geometry nodes\n");
        return;
    }
    
    // Replace the bottom group with a switch group
    performerSwitch = new pfSwitch();
    performerSwitch->setVal(PFSWITCH_OFF);
    ((vsComponent *)theNode)->replaceBottomGroup(performerSwitch);
    
    // Mark this attribute as attached
    attachedFlag = 1;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsSwitchAttribute::detach(vsNode *theNode)
{
    pfGroup *newGroup;

    // Can't detach an attribute that is not attached
    if (!attachedFlag)
    {
        printf("vsSwitchAttribute::detach: Attribute is not attached\n");
        return;
    }
    
    // Replace the switch group with an ordinary group
    newGroup = new pfGroup();
    ((vsComponent *)theNode)->replaceBottomGroup(newGroup);
    performerSwitch = NULL;
    
    // Mark this attribute as unattached
    attachedFlag = 0;
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsSwitchAttribute::attachDuplicate(vsNode *theNode)
{
    vsSwitchAttribute *newAttrib;
    int switchVal;

    // Create a duplicate switch attribute
    newAttrib = new vsSwitchAttribute();

    // Attach the duplicate attribute to the specified node first, so that
    // we can manipulate its values
    theNode->addAttribute(newAttrib);
    
    // Set the children that are enabled on the duplicate to be the same
    // as the children that are enabled on this one
    switchVal = (int)(performerSwitch->getVal());
    switch (switchVal)
    {
        case PFSWITCH_ON:
            newAttrib->enableAll();
            break;
        case PFSWITCH_OFF:
            newAttrib->disableAll();
            break;
        default:
            newAttrib->enableOne(switchVal);
            break;
    }
}
