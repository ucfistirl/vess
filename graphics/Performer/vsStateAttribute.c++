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
//    VESS Module:  vsStateAttribute.c++
//
//    Description:  Abstract base class for all state-category attributes
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsNode.h++"
#include "vsStateAttribute.h++"

// ------------------------------------------------------------------------
// Constructor - Initializes save and owner lists
// ------------------------------------------------------------------------
vsStateAttribute::vsStateAttribute() : attrSaveList(1, 1), ownerList(10, 50)
{
    // Initialize the number of saved attributes and parent nodes to zero,
    // and set the override to the default of false
    attrSaveCount = 0;
    ownerCount = 0;
    overrideFlag = false;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsStateAttribute::~vsStateAttribute()
{
}

// ------------------------------------------------------------------------
// Retrieves the category of this attribute
// ------------------------------------------------------------------------
int vsStateAttribute::getAttributeCategory()
{
    return VS_ATTRIBUTE_CATEGORY_STATE;
}

// ------------------------------------------------------------------------
// Sets the value of the override flag for this graphics state
// ------------------------------------------------------------------------
void vsStateAttribute::setOverride(int override)
{
    // If there's a change in the override value, then mark the nodes
    // that have this attribute attached as dirty
    if (overrideFlag != override)
        markOwnersDirty();

    // Store the new override value
    overrideFlag = override;
}

// ------------------------------------------------------------------------
// Gets the value of the override flag for this graphics state
// ------------------------------------------------------------------------
bool vsStateAttribute::getOverride()
{
    return overrideFlag;
}

// ------------------------------------------------------------------------
// Protected function
// Marks each node that has this attribute attached as 'dirty'; dirty nodes
// get attention the next time vsSystem::drawFrame is called.
// ------------------------------------------------------------------------
void vsStateAttribute::markOwnersDirty()
{
    int loop;
    
    // Mark each node that has this attribute attached as dirty
    for (loop = 0; loop < ownerCount; loop++)
        ((vsNode *)(ownerList[loop]))->dirty();
}

// ------------------------------------------------------------------------
// Internal function
// Adds the specified node to the list of nodes that have this attribute
// attached
// ------------------------------------------------------------------------
void vsStateAttribute::attach(vsNode *theNode)
{
    // Add the specified node to our list of owner nodes and mark that
    // node as dirty.
    ownerList[ownerCount] = theNode;
    ownerCount++;
    theNode->dirty();
    
    // Call the inherited version of this function
    vsAttribute::attach(theNode);
}

// ------------------------------------------------------------------------
// Internal function
// Removes the specified node from the list of nodes that have this
// attribute attached
// ------------------------------------------------------------------------
void vsStateAttribute::detach(vsNode *theNode)
{
    int loop;
    
    // Search our owner list for the specified node
    for (loop = 0; loop < ownerCount; loop++)
    {
        if (theNode == ownerList[loop])
        {
            // Remove the specified node from our owner list by moving the
	    // last owner over top of it and shrinking the size of the list
	    // by one
            ownerList[loop] = ownerList[ownerCount-1];
            ownerCount--;
            theNode->dirty();

	    // Call the inherited version of this function
            vsAttribute::detach(theNode);
            return;
        }
    }
}
