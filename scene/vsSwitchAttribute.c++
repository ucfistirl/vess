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
    performerSwitch = NULL;
}

// ------------------------------------------------------------------------
// VESS internal function
// Constructor - Sets this attribute up at 'attached', using the indicated
// Performer switch
// ------------------------------------------------------------------------
vsSwitchAttribute::vsSwitchAttribute(pfSwitch *switchGroup)
{
    performerSwitch = switchGroup;
    performerSwitch->ref();
    
    attachedFlag = 1;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsSwitchAttribute::~vsSwitchAttribute()
{
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

    if ((index < 0) || (index >= performerSwitch->getNumChildren()))
    {
        printf("vsSwitchAttribute::enableOne: Index out of bounds\n");
        return;
    }

    performerSwitch->setVal(index);
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

    if ((index < 0) || (index >= performerSwitch->getNumChildren()))
    {
        printf("vsSwitchAttribute::disableOne: Index out of bounds\n");
        return;
    }

    if (index == performerSwitch->getVal())
        performerSwitch->setVal(PFSWITCH_OFF);
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

    performerSwitch->setVal(PFSWITCH_ON);
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

    performerSwitch->setVal(PFSWITCH_OFF);
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

    if ((index < 0) || (index >= performerSwitch->getNumChildren()))
    {
        printf("vsSwitchAttribute::isEnabled: Index out of bounds\n");
        return VS_FALSE;
    }

    if (index == performerSwitch->getVal())
        return VS_TRUE;
    else
        return VS_FALSE;
}

// ------------------------------------------------------------------------
// VESS internal function
// Returns if this attribute is available to be attached to a node
// ------------------------------------------------------------------------
int vsSwitchAttribute::canAttach()
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
void vsSwitchAttribute::attach(vsNode *theNode)
{

    if (attachedFlag)
    {
        printf("vsSwitchAttribute::attach: Attribute is already attached\n");
        return;
    }

    if (theNode->getNodeType() == VS_NODE_TYPE_GEOMETRY)
    {
        printf("vsSwitchAttribute::attach: Can't attach switch attributes to "
            "geometry nodes\n");
        return;
    }
    
    // Replace the bottom group with a switch group
    performerSwitch = new pfSwitch();
    performerSwitch->setVal(PFSWITCH_OFF);
    ((vsComponent *)theNode)->replaceBottomGroup(performerSwitch);
    
    attachedFlag = 1;
}

// ------------------------------------------------------------------------
// VESS internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsSwitchAttribute::detach(vsNode *theNode)
{
    pfGroup *newGroup;

    if (!attachedFlag)
    {
        printf("vsSwitchAttribute::detach: Attribute is not attached\n");
        return;
    }
    
    // Replace the switch group with an ordinary group
    newGroup = new pfGroup();
    ((vsComponent *)theNode)->replaceBottomGroup(newGroup);
    performerSwitch = NULL;
    
    attachedFlag = 0;
}

// ------------------------------------------------------------------------
// VESS internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsSwitchAttribute::attachDuplicate(vsNode *theNode)
{
    vsSwitchAttribute *newAttrib;
    int switchVal;

    newAttrib = new vsSwitchAttribute();

    theNode->addAttribute(newAttrib);
    
    switchVal = performerSwitch->getVal();
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
