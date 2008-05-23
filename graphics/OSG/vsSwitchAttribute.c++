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
    // Start with a NULL osgSim::MultiSwitch (this will be created in the 
    // attach() method)
    osgSwitch = NULL;
    
    // By default, we start with all children enabled.
    allEnabled = true;
    allDisabled = false;
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
// Returns a clone of this attribute
// ------------------------------------------------------------------------
vsAttribute *vsSwitchAttribute::clone()
{
    vsSwitchAttribute *newAttrib;

    // The state of a switch attribute depends on the children of the node
    // to which it's attached, so we can't clone a switch attribute
    // directly.  Just create a new switch attribute and return it
    newAttrib = new vsSwitchAttribute();
    return newAttrib;
}

// ------------------------------------------------------------------------
// Enables display of one of the children of the parent component. The
// index of the first child is 0.
// ------------------------------------------------------------------------
void vsSwitchAttribute::enableOne(int index)
{
    // Make sure we're attached to a node, bail out if not
    if (!attachedCount)
    {
        printf("vsSwitchAttribute::enableOne: Attribute must be attached "
            "before switch can be manipulated\n");
        return;
    }

    // Ensure the given index is valid.
    if ((index < 0) || (index >= osgSwitch->getSwitchSetList().size() - 1))
    {
        printf("vsSwitchAttribute::enableOne: Index out of bounds (%d >= %d)\n",
            index, osgSwitch->getSwitchSetList().size() - 1);
        return;
    }
    
    // Activate the requested switch mask on the osgSim::MultiSwitch
    osgSwitch->setActiveSwitchSet(index);

    // Adjust the all-children states appropriately, so the switch attribute
    // remembers that only a subset of children are enabled
    allEnabled = false;
    allDisabled = false;
}

// ------------------------------------------------------------------------
// Disables display of one of the children of the parent component. The
// index of the first child is 0.
// ------------------------------------------------------------------------
void vsSwitchAttribute::disableOne(int index)
{
    // Make sure we're attached to a node, bail out if not
    if (!attachedCount)
    {
        printf("vsSwitchAttribute::disableOne: Attribute must be attached "
            "before switch can be manipulated\n");
        return;
    }

    // Ensure the given index is valid.
    if ((index < 0) || (index >= osgSwitch->getSwitchSetList().size() - 1))
    {
        printf("vsSwitchAttribute::disableOne: Index out of bounds (%d >= %d)"
            "\n", index, osgSwitch->getSwitchSetList().size() - 1);
        return;
    }

    // If the requested switch mask is active, turn it off (showing no
    // children).  We can do this by setting the active switch set to
    // a negative value.
    if (osgSwitch->getActiveSwitchSet() == index)
        disableAll();

    // Adjust the all-children states appropriately, so the switch attribute
    // remembers that only a subset of children are enabled
    allEnabled = false;
    allDisabled = false;
}

// ------------------------------------------------------------------------
// Enables display of all of the children attached to the parent component
// ------------------------------------------------------------------------
void vsSwitchAttribute::enableAll()
{
    // Make sure we're attached to a node, bail out if not
    if (!attachedCount)
    {
        printf("vsSwitchAttribute::enableAll: Attribute must be attached "
            "before switch can be manipulated\n");
        return;
    }

    // Turn all children on on the osgSim::MultiSwitch.  To do this, we use
    // the dummy switch set that we add to the end of the switch set list
    // during the attach() process.
    osgSwitch->setActiveSwitchSet(osgSwitch->getSwitchSetList().size() - 1);
    osgSwitch->setAllChildrenOn(osgSwitch->getSwitchSetList().size() - 1);

    // Finally, we reset the default new child value (which determines the
    // state of new nodes added to the switch) to false, because the call
    // to setAllChildrenOn() above has the side effect of changing this value.
    osgSwitch->setNewChildDefaultValue(false);
    
    // Adjust the all-children states appropriately, so the switch attribute
    // remembers that all children are enabled
    allEnabled = true;
    allDisabled = false;
}

// ------------------------------------------------------------------------
// Disables display of all of the children attached to the parent component
// ------------------------------------------------------------------------
void vsSwitchAttribute::disableAll()
{
    // Make sure we're attached to a node, bail out if not
    if (!attachedCount)
    {
        printf("vsSwitchAttribute::disableAll: Attribute must be attached "
            "before switch can be manipulated\n");
        return;
    }

    // Turn all children off on the osgSim::MultiSwitch.  To do this, we use
    // the dummy switch set that we add to the end of the switch set list
    // during the attach() process.
    osgSwitch->setActiveSwitchSet(osgSwitch->getSwitchSetList().size() - 1);
    osgSwitch->setAllChildrenOff(osgSwitch->getSwitchSetList().size() - 1);
    
    // Adjust the all-children states appropriately, so the switch attribute
    // remembers that all children are disabled
    allDisabled = true;
    allEnabled = false;
}

// ------------------------------------------------------------------------
// Returns a flag indicating if the child with the specified index is
// enabled. The index of the first child is 0.
// ------------------------------------------------------------------------
bool vsSwitchAttribute::isEnabled(int index)
{
    // Make sure we're attached to a node, bail out if not
    if (!attachedCount)
    {
        printf("vsSwitchAttribute::isEnabled: Attribute must be attached "
            "before switch can be manipulated\n");
        return false;
    }

    // Ensure the given index is valid.
    if ((index < 0) || (index >= osgSwitch->getSwitchSetList().size() - 1))
    {
        printf("vsSwitchAttribute::isEnabled: Index out of bounds (%d >= %d)"
            "\n", index, osgSwitch->getSwitchSetList().size() - 1);
        return true;
    }

    // Return true if the given switch mask is the active switch set on
    // the OSG switch
    if (allEnabled || (index == osgSwitch->getActiveSwitchSet()))
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
    // Make sure we're not already attached to a node, if we are, we can't
    // be attached to another
    if (attachedCount)
        return false;

    // Otherwise, we can be attached
    return true;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsSwitchAttribute::attach(vsNode *theNode)
{
    vsComponent *theComponent;
    int maskCount;
    int i;

    // Make sure we're not already attached to a node, bail out if we are
    if (attachedCount)
    {
        printf("vsSwitchAttribute::attach: Attribute is already attached\n");
        return;
    }

    // Only components can receive switch attributes (no other node makes
    // sense)
    if (theNode->getNodeType() != VS_NODE_TYPE_COMPONENT)
    {
        printf("vsSwitchAttribute::attach: Can only attach switch "
            "attributes to vsComponents\n");
        return;
    }
    
    // Replace the component's bottom group with an osgSim::MultiSwitch group
    theComponent = (vsComponent *)theNode;
    osgSwitch = new osgSim::MultiSwitch();
    osgSwitch->ref();
    osgSwitch->setNewChildDefaultValue(false);
    theComponent->replaceBottomGroup(osgSwitch);

    // Set up a default set of switch masks (switch sets in OSG terms).  This 
    // list may be changed later by the vsDatabaseLoader (or by the user with 
    // OSG calls).  Currently, there is no VESS API to change switch masks,
    // because they can't be changed under Performer.  The default set of
    // switch masks is one mask for each child, with only that child active.
    for (i = 0; i < theComponent->getChildCount(); i++)
        osgSwitch->setValue(i, i, true);

    // Add an artificial switch set to the end of the switch that has
    // all of the children on.  We'll use this mask for the enableAll()
    // call, since the osgSim::MultiSwitch doesn't have this capability.
    maskCount = osgSwitch->getSwitchSetList().size();
    osgSwitch->setAllChildrenOn(maskCount);
    
    // Flag the attribute as attached
    attachedCount = 1;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsSwitchAttribute::detach(vsNode *theNode)
{
    osg::Group *newGroup;

    // Make sure we're attached to a node, bail out if not
    if (!attachedCount)
    {
        printf("vsSwitchAttribute::detach: Attribute is not attached\n");
        return;
    }
    
    // Replace the component's switch group with an ordinary group
    newGroup = new osg::Group();
    ((vsComponent *)theNode)->replaceBottomGroup(newGroup);
    osgSwitch->unref();
    osgSwitch = NULL;
    
    // Flag the attribute as not attached
    attachedCount = 0;
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsSwitchAttribute::attachDuplicate(vsNode *theNode)
{
    vsSwitchAttribute *newAttrib;
    int loop, sloop;
    int sourceChildCount, targetChildCount, childCount;

    // Can only add switches to components (no other node makes sense)
    if (theNode->getNodeType() != VS_NODE_TYPE_COMPONENT)
        return;

    // Create a new switch attribute and attach it to the given node
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
    for (loop = 0; loop < osgSwitch->getSwitchSetList().size(); loop++)
    {
        for (sloop = 0; sloop < childCount; sloop++)
        {
            if (osgSwitch->getValue(loop, sloop))
                newAttrib->osgSwitch->setValue(loop, sloop, true);
            else
                newAttrib->osgSwitch->setValue(loop, sloop, false);
        }
    }
}

// ------------------------------------------------------------------------
// Internal function.
// Called by the addChild(), and insertChild() methods of vsComponent
// (as well as the attach() method of this class) to set up an 
// osgSim::MultiSwitch switch set for the given child
// ------------------------------------------------------------------------
void vsSwitchAttribute::addMask(vsComponent *parent, vsNode *newChild)
{
    int childIndex;
    int maskListSize;
    bool maskValue;
    int i, j;

    // Bail out if we're not attached (no switch to manipulate)
    if (attachedCount <= 0)
        return;

    // Get the index of the given child
    childIndex = 0;
    while ((parent->getChild(childIndex) != newChild) &&
           (childIndex < parent->getChildCount()))
        childIndex++;

    // Insert a new mask in the switch's switch set corresponding to this
    // child
    maskListSize = osgSwitch->getSwitchSetList().size();

    // If the childIndex is not greater than the size of the switch mask list,
    // then we have to make room in the list for this child's mask.
    if (childIndex < maskListSize)
    {
        // Slide all of the switch masks down by one position
        for (i = childIndex; i < maskListSize; i++)
        {   
            for (j = 0; j < parent->getChildCount(); j++)
            {   
                // Get the value for the mask in this position
                maskValue = osgSwitch->getValue(i, j);

                // Set this value for the corresponding value in the
                // next position's mask
                osgSwitch->setValue(i+1, j, maskValue);
            }
        }
    }

    // Now that we've made sure we have room for this child's switch mask, 
    // set it up to have only this child on.
    osgSwitch->setSingleChildOn(childIndex, childIndex);

    // Get the new size of the switch set list
    maskListSize = osgSwitch->getSwitchSetList().size();

    // Adjust the "all children" mask to also contain the new child. Use the
    // allEnabled value to tell whether the mask should be enabled or
    // disabled.
    if (allEnabled)
        osgSwitch->setAllChildrenOn(maskListSize);
    else
        osgSwitch->setAllChildrenOff(maskListSize);

    // If we previously had all children on or off, make sure we still do
    // by activating the "all children" mask at the end of the OSG switch's
    // mask list
    if (allEnabled || allDisabled)
        osgSwitch->setActiveSwitchSet(maskListSize);
}

// ------------------------------------------------------------------------
// Internal function.
// Called by the removeChild() method of vsComponent to update the switch
// masks when a child is removed.
// ------------------------------------------------------------------------
void vsSwitchAttribute::pruneMasks(vsComponent *parent)
{
    int maskListSize;
    osgSim::MultiSwitch *newSwitch;
    osgSim::MultiSwitch::ValueList switchMask;
    int newMaskCount;
    int i, j;
    bool empty;

    // Bail out if we're not attached (no switch to manipulate)
    if (attachedCount <= 0)
        return;

    // Get the current switch set (mask).  If it's the "all children" mask
    // we need to make sure that we adjust the active switch set to the new 
    // "all children" value after the list of masks is updated.
    maskListSize = osgSwitch->getSwitchSetList().size();

    // If we find any empty masks in the MultiSwitch, then we should
    // remove them (it serves no purpose any longer). Create a new 
    // osgSim::MultiSwitch, which will contain the reduced set of switch 
    // masks
    newSwitch = new osgSim::MultiSwitch();
    newMaskCount = 0;
    for (i = 0; i < osgSwitch->getSwitchSetList().size(); i++)
    {
        // Get the next switch mask from the switch
        switchMask = osgSwitch->getValueList(i);
        
        // Check the values in the switch mask to make sure it's not
        // empty
        empty = true;
        for (j = 0; j < switchMask.size(); j++)
        {
            if (osgSwitch->getValue(i, j))
                empty = false;
        }

        // If the switch mask is not empty, copy the mask to the new
        // switch
        if (!empty)
        {
            for (j = 0; j < switchMask.size(); j++)
                newSwitch->setValue(newMaskCount, j, 
                    osgSwitch->getValue(i, j));
        }
    }
    
    // Now, replace the bottom group of the component with the new
    // switch, which has the updated switch masks
    parent->replaceBottomGroup(newSwitch);
    osgSwitch->unref();
    osgSwitch = newSwitch;
    osgSwitch->ref();

    // Now, we need to adjust the new switch to be enabled or
    // disabled, based upon the isSwitchEnabled value.
    maskListSize = osgSwitch->getSwitchSetList().size();
    if (allEnabled)
    {
        // Make sure that the number of switch masks reported is valid.
        if(maskListSize > 0)
        {
           // All children are enabled, so enable them all on the switch
           osgSwitch->setAllChildrenOn(maskListSize - 1);
        }
        else
        {
           // All children are enabled, so enable them all on the switch
           osgSwitch->setAllChildrenOn(0);
        }
    }
    else
    {
        // Make sure that the number of switch masks reported is valid.
        if(maskListSize > 0)
        {
           // Disable all the children in the "all children" mask
           osgSwitch->setAllChildrenOff(maskListSize - 1);
        }
        else
        {
           // Disable all the children in the "all children" mask
           osgSwitch->setAllChildrenOff(0);
        }
    }

    // Finally, if we previously had all children on or off, make sure we
    // still do
    if (allEnabled || allDisabled)
    {
        // Make sure that the number of switch masks reported is valid.
        if(maskListSize > 0)
        {
           osgSwitch->setActiveSwitchSet(maskListSize - 1);
        }
        else
        {
           osgSwitch->setActiveSwitchSet(0);
        }
    }
}

// ------------------------------------------------------------------------
// Internal function
// Add the child at the given index to the given switch mask.
// ------------------------------------------------------------------------
void vsSwitchAttribute::setMaskValue(int maskIndex, int childIndex, bool value)
{
    if (attachedCount <= 0)
       return;

    // Enable the given child on the given mask
    osgSwitch->setValue(maskIndex, childIndex, value);
}
