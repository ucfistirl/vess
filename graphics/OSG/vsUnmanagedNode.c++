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
//    VESS Module:  vsUnmanagedNode.c++
//
//    Description:  A wrapper for base library objects that can be
//                  visualized in a VESS scene graph but not manipulated.
//
//    Author(s):    Bryan Kline, Casey Thurston
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsUnmanagedNode.h++"
#include <osg/MatrixTransform>
#include <osg/StateAttribute>

// ------------------------------------------------------------------------
// Constructor -
// ------------------------------------------------------------------------
vsUnmanagedNode::vsUnmanagedNode(osg::Node *newNode) : parentList(5, 5)
{
    // Initialize the number of parents to zero
    parentCount = 0;

    // Create an osg::Node
    osgNode = newNode;
    osgNode->ref();

    // Register this unmanaged node and osg::Node in the node map
    getMap()->registerLink(this, osgNode);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsUnmanagedNode::~vsUnmanagedNode()
{
    // Unregister this node, then allow the node to be deleted by losing the
    // reference to it.
    getMap()->removeLink(this, VS_OBJMAP_FIRST_LIST);
    osgNode->unref();
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsUnmanagedNode::getClassName()
{
    return "vsUnmanagedNode";
}

// ------------------------------------------------------------------------
// 'Clones' the tree rooted at this node, duplicating the portion of the
// scene graph rooted at this node, down to but not including leaf nodes.
// (Leaf nodes are instanced instead.)
// In this case, we assume that this version of the function will be called
// by default on leaf node subtypes, meaning that no duplication takes
// place; return the original node.
// ------------------------------------------------------------------------
vsNode *vsUnmanagedNode::cloneTree()
{
    return this;
}

// ------------------------------------------------------------------------
// Destroys the entire scene graph rooted at this node, up to but not
// including this node itself. Deletes any objects whose reference counts
// reach zero. Since this type of node cannot have any children, this
// method does nothing.
// ------------------------------------------------------------------------
void vsUnmanagedNode::deleteTree()
{
}

// ------------------------------------------------------------------------
// Retrieves the type of this node
// ------------------------------------------------------------------------
int vsUnmanagedNode::getNodeType()
{
    return VS_NODE_TYPE_UNMANAGED;
}

// ------------------------------------------------------------------------
// Add a node to this node's child list. This type of node cannot have
// children, so it always returns false.
// ------------------------------------------------------------------------
bool vsUnmanagedNode::addChild(vsNode *newChild)
{
    return false;
}

// ------------------------------------------------------------------------
// Insert a node into this node's child list at the specified index. This
// type of node cannot have children, so it always returns false.
// ------------------------------------------------------------------------
bool vsUnmanagedNode::insertChild(vsNode *newChild, int index)
{
    return false;
}

// ------------------------------------------------------------------------
// Remove a node from this node's child list. This type of node cannot have
// children, so it always returns false.
// ------------------------------------------------------------------------
bool vsUnmanagedNode::removeChild(vsNode *targetChild)
{
    return false;
}

// ------------------------------------------------------------------------
// Replace a node in this node's child list with a new node. This type of
// node cannot have children, so it always returns false.
// ------------------------------------------------------------------------
bool vsUnmanagedNode::replaceChild(vsNode *targetChild, vsNode *newChild)
{
    return false;
}

// ------------------------------------------------------------------------
// Retrieves the number of parent nodes for this node
// ------------------------------------------------------------------------
int vsUnmanagedNode::getParentCount()
{
    return parentCount;
}

// ------------------------------------------------------------------------
// Retrieves one of the parent nodes of this node, specified by index.
// The index of the first parent is 0.
// ------------------------------------------------------------------------
vsNode *vsUnmanagedNode::getParent(int index)
{
    // Check the index to make sure it refers to a valid parent, complain 
    // and return NULL if not
    if ((index < 0) || (index >= parentCount))
    {
        printf("vsUnmanagedNode::getParent: Bad parent index\n");
        return NULL;
    }
    
    // Return the requested parent
    return (vsNode *)(parentList[index]);
}

// ------------------------------------------------------------------------
// Retrieves the number of child nodes for this node. Since this is an
// unmanaged node, its child count should be reflected as 0, regardless of
// the number of children the base library object may have.
// ------------------------------------------------------------------------
int vsUnmanagedNode::getChildCount()
{
    return 0;
}

// ------------------------------------------------------------------------
// Retrieves one of the child nodes of this node, specified by index. This
// type of node cannot have children, so it always returns NULL.
// ------------------------------------------------------------------------
vsNode *vsUnmanagedNode::getChild(int index)
{
    return NULL;
}

// ------------------------------------------------------------------------
// Retrieves the center point and radius of a sphere that encompasses all
// of the geometry within this object.
// ------------------------------------------------------------------------
void vsUnmanagedNode::getBoundSphere(atVector *centerPoint, double *radius)
{
    osg::BoundingSphere boundSphere;
    osg::Vec3 center;

    // Get the bounding sphere from OSG
    boundSphere = osgNode->getBound();

    // Copy the sphere center point to the result vector, if there is one
    if (centerPoint)
    {
        center = boundSphere.center();
        centerPoint->set(center[0], center[1], center[2]);
    }

    // Copy the sphere radius to the result value, if there is one
    if (radius)
        *radius = boundSphere.radius();
}

// ------------------------------------------------------------------------
// Computes the global coordinate transform at this node by multiplying
// together all of the transforms at nodes at and above this one.
// ------------------------------------------------------------------------
atMatrix vsUnmanagedNode::getGlobalXform()
{
    osg::Node *nodePtr;
    osg::Matrixd xform;
    osg::Matrixd matRef;
    atMatrix result;
    int loop, sloop;

    // Start at this node, computing all transforms up to the root.
    xform.makeIdentity();
    nodePtr = osgNode;

    // Check the parent count to determine if we're at the top of the tree
    while (nodePtr->getNumParents() > 0)
    {
        if (dynamic_cast<osg::MatrixTransform *>(nodePtr))
        {
            // Multiply this Transform's matrix into the accumulated
            // transform
            matRef = ((osg::MatrixTransform *)nodePtr)->getMatrix();
            xform.postMult(matRef);
        }
        
        // Move to the node's (first) parent
        nodePtr = nodePtr->getParent(0);
    }

    // Transpose the matrix when going from OSG to VESS
    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            result[loop][sloop] = xform(sloop, loop);

    // Return the resulting matrix
    return result;
}

// ------------------------------------------------------------------------
// Sets the intersection value for this node. During an intersection run,
// at each component a bitwise AND of the intersection's mask and the nodes
// value is performed; if the result of the AND is zero, the intersection
// ignores this node and all of its children.
// ------------------------------------------------------------------------
void vsUnmanagedNode::setIntersectValue(unsigned int newValue)
{
    osgNode->setNodeMask(newValue);
}

// ------------------------------------------------------------------------
// Retrieves the intersection value for this node.
// ------------------------------------------------------------------------
unsigned int vsUnmanagedNode::getIntersectValue()
{
    return (osgNode->getNodeMask());
}

// ------------------------------------------------------------------------
// For the time being, don't allow any attributes to be added to this node.
// FIXME: Don't do this.
// ------------------------------------------------------------------------
void vsUnmanagedNode::addAttribute(vsAttribute *newAttribute)
{
    printf("vsUnmanagedNode::addAttribute: This type of node cannot accept "
           "attributes of this type\n");
    return;
}

// ------------------------------------------------------------------------
// Removes the specified attribute from the node's list, and notifies the
// attribute that it has been removed. For now, don't allow any attributes
// to be added to this node.  FIXME: Don't do this.
// ------------------------------------------------------------------------
void vsUnmanagedNode::removeAttribute(vsAttribute *targetAttribute)
{
    // Print an error if we don't find it
    printf("vsUnmanagedNode::removeAttribute: Specified attribute isn't part "
           "of this node\n");
    return;
}

// ------------------------------------------------------------------------
// Enables lit rendering for this node.
// ------------------------------------------------------------------------
void vsUnmanagedNode::enableLighting()
{
    osg::StateSet *osgStateSet;
    
    // Enable the GL lighting mode on the Geode's StateSet
    osgStateSet = osgNode->getOrCreateStateSet();
    osgStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);
}

// ------------------------------------------------------------------------
// Disables lit rendering for this node.
// ------------------------------------------------------------------------
void vsUnmanagedNode::disableLighting()
{
    osg::StateSet *osgStateSet;
    
    // Disable the GL lighting mode on the Geode's StateSet
    osgStateSet = osgNode->getOrCreateStateSet();
    osgStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
}

// ------------------------------------------------------------------------
// Enables culling on this node and its children
// ------------------------------------------------------------------------
void vsUnmanagedNode::enableCull()
{
    osgNode->setCullingActive(true);
}

// ------------------------------------------------------------------------
// Disables culling on this node and its children
// ------------------------------------------------------------------------
void vsUnmanagedNode::disableCull()
{
    osgNode->setCullingActive(false);
}

// ------------------------------------------------------------------------
// Returns the underlying object represented by this class.
// ------------------------------------------------------------------------
osg::Node *vsUnmanagedNode::getBaseLibraryObject()
{
    return osgNode;
}

// ------------------------------------------------------------------------
// Internal function
// Adds a node to this node's list of parent nodes
// ------------------------------------------------------------------------
bool vsUnmanagedNode::addParent(vsNode *newParent)
{
    // Add the parent to our parent list and reference it
    parentList[parentCount++] = newParent;
    
    // Return success
    return true;
}

// ------------------------------------------------------------------------
// Internal function
// Removes a node from this node's list of parent nodes
// ------------------------------------------------------------------------
bool vsUnmanagedNode::removeParent(vsNode *targetParent)
{
    int loop, sloop;

    // Look for the given "parent" in the parent list
    for (loop = 0; loop < parentCount; loop++)
    {
        // See if this is the parent we're looking for
        if (targetParent == parentList[loop])
        {
            // 'Slide' the parents down to cover up the removed one
            for (sloop = loop; sloop < parentCount-1; sloop++)
                parentList[sloop] = parentList[sloop+1];

            // Remove the given parent
            parentCount--;

            // Return that the remove succeeded
            return true;
        }
    }

    // Return failure if the specified parent isn't found
    return false;
}

// ------------------------------------------------------------------------
// Internal function
// Calls the saveCurrent function on all attached attributes
// ------------------------------------------------------------------------
void vsUnmanagedNode::saveCurrentAttributes()
{
}

// ------------------------------------------------------------------------
// Internal function
// Calls the apply function on all attached attributes
// ------------------------------------------------------------------------
void vsUnmanagedNode::applyAttributes()
{
}

// ------------------------------------------------------------------------
// Internal function
// Calls the restoreSaved function on all attached attributes
// ------------------------------------------------------------------------
void vsUnmanagedNode::restoreSavedAttributes()
{
}

// ------------------------------------------------------------------------
// Internal function
// Does nothing.
// ------------------------------------------------------------------------
void vsUnmanagedNode::getAxisAlignedBoxBounds(atVector  *minValues,
    atVector *maxValues)
{
}
