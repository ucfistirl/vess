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
    attrSaveCount = 0;
    ownerCount = 0;
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
// Protected function
// Marks each node that has this attribute attached as 'dirty'; dirty nodes
// get attention the next time vsSystem::drawFrame is called.
// ------------------------------------------------------------------------
void vsStateAttribute::markOwnersDirty()
{
    int loop;
    
    for (loop = 0; loop < ownerCount; loop++)
        ((vsNode *)(ownerList[loop]))->dirty();
}

// ------------------------------------------------------------------------
// VESS internal function
// Adds the specified node to the list of nodes that have this attribute
// attached
// ------------------------------------------------------------------------
void vsStateAttribute::attach(vsNode *theNode)
{
    ownerList[ownerCount] = theNode;
    ownerCount++;
    theNode->dirty();
    
    vsAttribute::attach(theNode);
}

// ------------------------------------------------------------------------
// VESS internal function
// Removes the specified node from the list of nodes that have this
// attribute attached
// ------------------------------------------------------------------------
void vsStateAttribute::detach(vsNode *theNode)
{
    int loop;
    
    for (loop = 0; loop < ownerCount; loop++)
    {
        if (theNode == ownerList[loop])
        {
            ownerList[loop] = ownerList[ownerCount-1];
            ownerCount--;
            theNode->dirty();

            vsAttribute::detach(theNode);
            return;
        }
    }
}
