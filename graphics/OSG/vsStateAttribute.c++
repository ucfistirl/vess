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

#include "vsStateAttribute.h++"
#include "vsComponent.h++"
#include "vsGeometry.h++"
#include "vsDynamicGeometry.h++"
#include "vsScene.h++"

// ------------------------------------------------------------------------
// Constructor - Initializes save and owner lists
// ------------------------------------------------------------------------
vsStateAttribute::vsStateAttribute() : attrSaveList(1, 1), ownerList(10, 50)
{
    attrSaveCount = 0;
    ownerCount = 0;
    overrideFlag = VS_FALSE;
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
    overrideFlag = override;
    setAllOwnersOSGAttrModes();
}

// ------------------------------------------------------------------------
// Gets the value of the override flag for this graphics state
// ------------------------------------------------------------------------
int vsStateAttribute::getOverride()
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
    
    for (loop = 0; loop < ownerCount; loop++)
        ((vsNode *)(ownerList[loop]))->dirty();
}

// ------------------------------------------------------------------------
// Protected function
// Retrieves the OSG StateSet object associated with the given node
// ------------------------------------------------------------------------
osg::StateSet *vsStateAttribute::getOSGStateSet(vsNode *node)
{
    osg::Node *osgNode;

    switch (node->getNodeType())
    {
        case VS_NODE_TYPE_COMPONENT:
            osgNode = ((vsComponent *)node)->getBaseLibraryObject();
            break;
        case VS_NODE_TYPE_GEOMETRY:
            osgNode = ((vsGeometry *)node)->getBaseLibraryObject();
            break;
        case VS_NODE_TYPE_DYNAMIC_GEOMETRY:
            osgNode = ((vsDynamicGeometry *)node)->getBaseLibraryObject();
            break;
        case VS_NODE_TYPE_SCENE:
            osgNode = ((vsScene *)node)->getBaseLibraryObject();
            break;
    }

    return (osgNode->getOrCreateStateSet());
}

// ------------------------------------------------------------------------
// Protected function
// Calls the setOSGAttrModes function on every node that has this attribute
// attached
// ------------------------------------------------------------------------
void vsStateAttribute::setAllOwnersOSGAttrModes()
{
    int loop;
    
    for (loop = 0; loop < ownerCount; loop++)
        setOSGAttrModes((vsNode *)(ownerList[loop]));
}

// ------------------------------------------------------------------------
// Internal function
// Adds the specified node to the list of nodes that have this attribute
// attached
// ------------------------------------------------------------------------
void vsStateAttribute::attach(vsNode *theNode)
{
    ownerList[ownerCount] = theNode;
    ownerCount++;
    theNode->ref();
    theNode->dirty();
    
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
    
    for (loop = 0; loop < ownerCount; loop++)
    {
        if (theNode == ownerList[loop])
        {
            ownerList[loop] = ownerList[ownerCount-1];
            ownerCount--;
            theNode->unref();
            theNode->dirty();

            vsAttribute::detach(theNode);
            return;
        }
    }
}
