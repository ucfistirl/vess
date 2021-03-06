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
#include "vsSkeletonMeshGeometry.h++"
#include "vsScene.h++"
#include "vsUnmanagedNode.h++"

// ------------------------------------------------------------------------
// Constructor - Initializes save and owner lists
// ------------------------------------------------------------------------
vsStateAttribute::vsStateAttribute()
{
    // Initialize class members
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
void vsStateAttribute::setOverride(bool override)
{
    // Set the attribute's override flag to the given value
    overrideFlag = override;

    // Set the osg::StateAttribute modes on all attached nodes to
    // use the new override setting
    setAllOwnersOSGAttrModes();
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
    
    // Iterate through the list of attached nodes, marking each one dirty
    for (loop = 0; loop < ownerList.getNumEntries(); loop++)
        ((vsNode *)(ownerList.getEntry(loop)))->dirty();
}

// ------------------------------------------------------------------------
// Protected function
// Retrieves the OSG StateSet object associated with the given node
// ------------------------------------------------------------------------
osg::StateSet *vsStateAttribute::getOSGStateSet(vsNode *node)
{
    osg::Node *osgNode;

    // Get the osg::Node associated with the given node, the type
    // checking is done because the getBaseLibraryObject() method is
    // not virtual
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
        case VS_NODE_TYPE_SKELETON_MESH_GEOMETRY:
            osgNode = ((vsSkeletonMeshGeometry *)node)->getBaseLibraryObject();
            break;
        case VS_NODE_TYPE_SCENE:
            osgNode = ((vsScene *)node)->getBaseLibraryObject();
            break;
        case VS_NODE_TYPE_UNMANAGED:
            osgNode = ((vsUnmanagedNode *)node)->getBaseLibraryObject();
            break;
    }

    // Get the StateSet attached to the osg::Node, creating it if
    // necessary
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
    
    // Iterate through the list of nodes attached to this attribute,
    // calling the setOSGAttrModes() method on each node
    for (loop = 0; loop < ownerList.getNumEntries(); loop++)
        setOSGAttrModes((vsNode *)(ownerList.getEntry(loop)));
}

// ------------------------------------------------------------------------
// Internal function
// Adds the specified node to the list of nodes that have this attribute
// attached
// ------------------------------------------------------------------------
void vsStateAttribute::attach(vsNode *theNode)
{
    // Add the node to our owner list and increment the owner count
    ownerList.addEntry(theNode);

    // Mark the given node dirty
    theNode->dirty();
    
    // Do standard vsAttribute attaching
    vsAttribute::attach(theNode);
}

// ------------------------------------------------------------------------
// Internal function
// Removes the specified node from the list of nodes that have this
// attribute attached
// ------------------------------------------------------------------------
void vsStateAttribute::detach(vsNode *theNode)
{
    // Try to remove the given node (keep a temporary reference to it, so
    // it doesn't get deleted in the process)
    theNode->ref();
    ownerList.removeEntry(theNode);
    theNode->unref();
}
