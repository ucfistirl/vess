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
//    VESS Module:  vsNode.c++
//
//    Description:  Abstract parent class for all objects that can be a
//                  part of a VESS scene graph
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsNode.h++"
#include "vsComponent.h++"
#include "vsBackfaceAttribute.h++"
#include "vsFogAttribute.h++"
#include "vsMaterialAttribute.h++"
#include "vsTextureAttribute.h++"
#include "vsTransparencyAttribute.h++"
#include "vsShadingAttribute.h++"
#include "vsWireframeAttribute.h++"
#include "vsLightAttribute.h++"

vsObjectMap *vsNode::nodeMap = NULL;

// ------------------------------------------------------------------------
// Constructor - Clears the node's name
// ------------------------------------------------------------------------
vsNode::vsNode() : attributeList(10, 5)
{
    // Initialize node name to empty
    nodeName[0] = 0;

    // Start with no attributes
    attributeCount = 0;

    // Start dirty (require a preFrameTraversal from the beginning)
    dirtyFlag = true;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsNode::~vsNode()
{
//    vsAttribute *attr;

    // Remove all attached attributes; destroy those that aren't being
    // used by other nodes.
//    while (getAttributeCount() > 0)
//    {
//        attr = getAttribute(0);
//        removeAttribute(attr);
//        vsObject::checkDelete(attr);
//    }

    // The node shouldn't have any more attributes, parents, or children.
    // It's the derived class' responsibility to get rid of all of those
    // in its destructor. We can't check the number of children or parents
    // left on the node, because the data structures that contain that
    // information may have already been deleted by the derived class'
    // destructor. However, we can check the number of attributes currently
    // on the node, because that information is stored here. Make sure that
    // there aren't any attributes left; signal an error if there are.
    if (getAttributeCount() > 0)
        printf("vsNode::~vsNode: Node contains unremoved attributes\n");
}

// ------------------------------------------------------------------------
// 'Clones' the tree rooted at this node, duplicating the portion of the
// scene graph rooted at this node, down to but not including leaf nodes.
// (Leaf nodes are instanced instead.)
// In this case, we assume that this version of the function will be called
// by default on leaf node subtypes, meaning that no duplication takes
// place; return the original node.
// ------------------------------------------------------------------------
vsNode *vsNode::cloneTree()
{
    return this;
}

// ------------------------------------------------------------------------
// Destroys the entire scene graph rooted at this node, up to but not
// including this node itself. Deletes any objects whose reference counts
// reach zero.
// ------------------------------------------------------------------------
void vsNode::deleteTree()
{
    vsNode *node;
    
    // Delete all children of this node
    while (getChildCount() > 0)
    {
	// We can always get the first child, because removing a child
	// causes all of the other children to slide over to fill the
	// gap.
        node = getChild(0);

	// Delete the subgraph below the selected child
        if (node->getNodeType() == VS_NODE_TYPE_COMPONENT)
            ((vsComponent *)node)->deleteTree();

        // Remove the child from this node
        removeChild(node);

        // Delete the child if it's now unowned
        vsObject::checkDelete(node);
    }
}

// ------------------------------------------------------------------------
// Add a node to this node's child list
// ------------------------------------------------------------------------
bool vsNode::addChild(vsNode *newChild)
{
    return false;
}

// ------------------------------------------------------------------------
// Insert a node into this node's child list at the specified index
// ------------------------------------------------------------------------
bool vsNode::insertChild(vsNode *newChild, int index)
{
    return false;
}

// ------------------------------------------------------------------------
// Remove a node from this node's child list
// ------------------------------------------------------------------------
bool vsNode::removeChild(vsNode *targetChild)
{
    return false;
}

// ------------------------------------------------------------------------
// Replace a node in this node's child list with a new node
// ------------------------------------------------------------------------
bool vsNode::replaceChild(vsNode *targetChild, vsNode *newChild)
{
    return false;
}

// ------------------------------------------------------------------------
// Retrieves the number of parent nodes for this node
// ------------------------------------------------------------------------
int vsNode::getParentCount()
{
    return 0;
}

// ------------------------------------------------------------------------
// Retrieves one of the parent nodes of this node, specified by index.
// The index of the first parent is 0.
// ------------------------------------------------------------------------
vsNode *vsNode::getParent(int index)
{
    return NULL;
}

// ------------------------------------------------------------------------
// Retrieves the number of child nodes for this node
// ------------------------------------------------------------------------
int vsNode::getChildCount()
{
    return 0;
}

// ------------------------------------------------------------------------
// Retrieves one of the child nodes of this node, specified by index.
// The index of the first child is 0.
// ------------------------------------------------------------------------
vsNode *vsNode::getChild(int index)
{
    return NULL;
}

// ------------------------------------------------------------------------
// Sets the name of this node to the specified name
// ------------------------------------------------------------------------
void vsNode::setName(const char *newName)
{
    // Deep copy the given node name and make sure we don't overrun the
    // name buffer
    strncpy(nodeName, newName, VS_NODE_NAME_MAX_LENGTH);
    nodeName[VS_NODE_NAME_MAX_LENGTH - 1] = 0;
}

// ------------------------------------------------------------------------
// Returns a constant pointer to this node's name
// ------------------------------------------------------------------------
const char *vsNode::getName()
{
    return nodeName;
}

// ------------------------------------------------------------------------
// Checks this node (and its children, if applicable) for nodes with the
// given name, and returns the first such node found, if it exists. Returns
// NULL otherwise.
// ------------------------------------------------------------------------
vsNode *vsNode::findNodeByName(const char *targetName)
{
    int idx = 0;

    // Search for the first occurrence of the given name
    return nodeSearch(targetName, &idx);
}

// ------------------------------------------------------------------------
// Checks this node (and its children, if applicable) for nodes with the
// given name, and returns the index'th such node found, if it exists.
// Returns NULL otherwise.
// ------------------------------------------------------------------------
vsNode *vsNode::findNodeByName(const char *targetName, int index)
{
    int idx = index;

    // Search for the index'th occurrence of the given name
    return nodeSearch(targetName, &idx);
}

// ------------------------------------------------------------------------
// Adds the specified attribute to the node's list, and notifies the
// attribute that it has been added.
// ------------------------------------------------------------------------
void vsNode::addAttribute(vsAttribute *newAttribute)
{
    // Add the attribute to this node's attributeList
    attributeList[attributeCount] = newAttribute;

    // Increment the attribute count
    attributeCount++;

    // Reference the attribute
    newAttribute->ref();

    // Call the attribute's attach() method with this node
    newAttribute->attach(this);
}

// ------------------------------------------------------------------------
// Removes the specified attribute from the node's list, and notifies the
// attribute that it has been removed.
// ------------------------------------------------------------------------
void vsNode::removeAttribute(vsAttribute *targetAttribute)
{
    int loop, sloop;

    // Search for the attribute in our attributeList
    for (loop = 0; loop < attributeCount; loop++)
    {
        // Check the current attribute to see if it's the one we're removing
        if (attributeList[loop] == targetAttribute)
        {
            // Detach the attribute
            targetAttribute->detach(this);
            
            // Slide the remaining attributes in the list down by one
            for (sloop = loop; sloop < attributeCount-1; sloop++)
                attributeList[sloop] = attributeList[sloop+1];

            // Decrement the node's attribute count
            attributeCount--;

            // Unreference the removed attribute
            targetAttribute->unref();
            
            // Nothing more to do, return immediately
            return;
        }
    }

    // Print an error if we don't find it
    printf("vsNode::removeAttribute: Specified attribute isn't part of "
        "this node\n");
}

// ------------------------------------------------------------------------
// Retrieves the number of attributes currently in this list
// ------------------------------------------------------------------------
int vsNode::getAttributeCount()
{
    return attributeCount;
}

// ------------------------------------------------------------------------
// Retrieves the attribute specified by index from the list. The index of
// the first attribute is 0.
// ------------------------------------------------------------------------
vsAttribute *vsNode::getAttribute(int index)
{
    // Make sure the given index is valid
    if ((index < 0) || (index >= attributeCount))
    {
        printf("vsNode::getAttribute: Index out of bounds\n");
        return NULL;
    }

    // Return the attribute at the given index in the attribute list
    return (vsAttribute *)(attributeList[index]);
}

// ------------------------------------------------------------------------
// Retrieves the attribute specified by the attribute type attribType and
// index from the list. The index of the first attribute of the given
// type in the list is 0.
// ------------------------------------------------------------------------
vsAttribute *vsNode::getTypedAttribute(int attribType, int index)
{
    int loop, count;

    // Initialize the count to 0, this will keep track of the number
    // of the given type of attribute we've found in the list so far
    count = 0;

    // Iterate through the attribute list
    for (loop = 0; loop < attributeCount; loop++)
    {
        // Check the current attribute's type
        if (attribType ==
            ((vsAttribute *)(attributeList[loop]))->getAttributeType())
        {
            // Match, check the count to see if this is the attribute we
            // want.  If so, return it, otherwise, just increment the count
            // and keep looking.
            if (index == count)
                return (vsAttribute *)(attributeList[loop]);
            else
                count++;
        }
    }

    // If we didn't find the one we want, return NULL
    return NULL;
}

// ------------------------------------------------------------------------
// Retrieves the attribute specified by the attribute category
// attribCategory and index from the list. The index of the first attribute
// of the given category in the list is 0.
// ------------------------------------------------------------------------
vsAttribute *vsNode::getCategoryAttribute(int attribCategory, int index)
{
    int loop, count;

    // Initialize the count to 0, this will keep track of the number
    // of the given type of attribute we've found in the list so far
    count = 0;

    // Iterate through the attribute list
    for (loop = 0; loop < attributeCount; loop++)
    {
        // Check the current attribute's category
        if (attribCategory ==
            ((vsAttribute *)(attributeList[loop]))->getAttributeCategory())
        {
            // Match, check the count to see if this is the attribute we
            // want.  If so, return it, otherwise, just increment the count
            // and keep looking.
            if (index == count)
                return (vsAttribute *)(attributeList[loop]);
            else
                count++;
        }
    }

    // If we didn't find the one we want, return NULL
    return NULL;
}

// ------------------------------------------------------------------------
// Retrieves the attribute with the given name from the list
// ------------------------------------------------------------------------
vsAttribute *vsNode::getNamedAttribute(char *attribName)
{
    int loop;

    // Iterate through the attribute list
    for (loop = 0; loop < attributeCount; loop++)
    {
        // Check the current attribute's name against the target name.
        // Return the attribute if they match.
        if (!strcmp(attribName,
            ((vsAttribute *)(attributeList[loop]))->getName()))
            return (vsAttribute *)(attributeList[loop]);
    }

    return NULL;
}

// ------------------------------------------------------------------------
// Private function
// Searches this node and its children for the idx'th occurrence of a node
// with the given name. The idx value is decremented after each match;
// success only occurs once idx reaches zero. Returns a pointer to this
// node if a match is found and idx is zero, NULL otherwise.
// ------------------------------------------------------------------------
vsNode *vsNode::nodeSearch(const char *name, int *idx)
{
    int loop;
    vsNode *result;

    // Check if this node is the one we're looking for
    if (!strcmp(name, getName()))
    {
        // Return this node if it happens to be the idx'th such
        // node, otherwise mark that we've found one and keep looking.
        if ((*idx) > 0)
            (*idx)--;
        else
            return this;
    }

    // Search the children of this node in the same way
    for (loop = 0; loop < getChildCount(); loop++)
        if (result = getChild(loop)->nodeSearch(name, idx))
            return result;

    // Return NULL if we make it this far without finding what we want
    return NULL;
}

// ------------------------------------------------------------------------
// Protected function
// Removes this node from all of its parents
// ------------------------------------------------------------------------
void vsNode::detachFromParents()
{
    vsNode *parentNode;

    // While there are parents left, remove this node from the first one
    while (getParentCount() > 0)
    {
        parentNode = getParent(0);
        parentNode->removeChild(this);
    }
}

// ------------------------------------------------------------------------
// Protected function
// Removes all attributes from this node, and deletes those not otherwise
// in use
// ------------------------------------------------------------------------
void vsNode::deleteAttributes()
{
    vsAttribute *attribute;

    // While there are attributes left, remove and release the first one
    while (getAttributeCount() > 0)
    {
        attribute = getAttribute(0);
        removeAttribute(attribute);
        vsObject::checkDelete(attribute);
    }
}

// ------------------------------------------------------------------------
// Static internal function
// Gets the object map that holds the node mappings, creating it if needed
// ------------------------------------------------------------------------
vsObjectMap *vsNode::getMap()
{
    // If no nodeMap has been created, create it now
    if (!nodeMap)
        nodeMap = new vsObjectMap();

    // Return the node map
    return nodeMap;
}

// ------------------------------------------------------------------------
// Static internal function
// Deletes the object map that holds the node mappings, if it exists
// ------------------------------------------------------------------------
void vsNode::deleteMap()
{
    // Check to see if the node map exists
    if (nodeMap)
    {
        // Delete the node map
        delete nodeMap;
        nodeMap = NULL;
    }
}

// ------------------------------------------------------------------------
// Internal function
// Adds a node to this node's list of parent nodes
// ------------------------------------------------------------------------
bool vsNode::addParent(vsNode *newParent)
{
    return false;
}

// ------------------------------------------------------------------------
// Internal function
// Removes a node from this node's list of parent nodes
// ------------------------------------------------------------------------
bool vsNode::removeParent(vsNode *targetParent)
{
    return false;
}

// ------------------------------------------------------------------------
// Internal function
// Calls the saveCurrent function on all attached attributes
// ------------------------------------------------------------------------
void vsNode::saveCurrentAttributes()
{
    int loop;

    // Iterate through the attribute list, and tell each attribute to
    // save its current state
    for (loop = 0; loop < attributeCount; loop++)
        ((vsAttribute *)(attributeList[loop]))->saveCurrent();
}

// ------------------------------------------------------------------------
// Internal function
// Calls the apply function on all attached attributes
// ------------------------------------------------------------------------
void vsNode::applyAttributes()
{
    int loop;

    // Iterate through the attribute list, and instruct each attribute
    // to apply its settings to the current graphics state
    for (loop = 0; loop < attributeCount; loop++)
        ((vsAttribute *)(attributeList[loop]))->apply();
}

// ------------------------------------------------------------------------
// Internal function
// Calls the restoreSaved function on all attached attributes
// ------------------------------------------------------------------------
void vsNode::restoreSavedAttributes()
{
    int loop;

    // Iterate through the attribute list, and tell each attribute to
    // restore its previous state
    for (loop = 0; loop < attributeCount; loop++)
        ((vsAttribute *)(attributeList[loop]))->restoreSaved();
}

// ------------------------------------------------------------------------
// Internal function
// Marks this node dirty, as well as every other node above and below it
// in the tree. This is done because only dirty nodes are traversed during
// the VESS traversal; nodes above to get to this node, and nodes below
// to allow any state changes to propogate down.
// ------------------------------------------------------------------------
void vsNode::dirty()
{
    // Traverse parents and mark them all dirty
    dirtyUp();

    // Traverse children and mark them all dirty
    dirtyDown();
}

// ------------------------------------------------------------------------
// Internal function
// Marks this node as clean if all of its parents are clean
// ------------------------------------------------------------------------
void vsNode::clean()
{
    int loop;
    bool flag;

    // Assume clean to begin with
    flag = true;

    // Check the dirty flag on all parents, if any are dirty, we can't
    // clean this node
    for (loop = 0; loop < getParentCount(); loop++)
        if (getParent(loop)->isDirty())
            flag = false;

    // If all parents are clean, this node can be marked clean
    if (flag)
        dirtyFlag = false;
}

// ------------------------------------------------------------------------
// Internal function
// Determines if this node is dirty or not
// ------------------------------------------------------------------------
bool vsNode::isDirty()
{
    return dirtyFlag;
}

// ------------------------------------------------------------------------
// Internal function
// Marks this node and each node above this one as dirty
// ------------------------------------------------------------------------
void vsNode::dirtyUp()
{
    int loop;

    // Mark this node dirty
    dirtyFlag = true;

    // Traverse all parents of this node, and mark them dirty as well
    for (loop = 0; loop < getParentCount(); loop++)
        getParent(loop)->dirtyUp();
}

// ------------------------------------------------------------------------
// Internal function
// Marks this node as dirty
// ------------------------------------------------------------------------
void vsNode::dirtyDown()
{
    int loop;

    // Mark this node dirty
    dirtyFlag = true;

    // Check the attribute list for light attributes.  If we find any,
    // clear their vsScene pointer, so it can be reset on the 
    // next preFrameTraversal.   This is necessary in case a node with a 
    // light attribute is being moved to a different scene graph.
    for (loop = 0; loop < attributeCount; loop++)
    {
        if (((vsAttribute *)attributeList[loop])->getAttributeType() ==
            VS_ATTRIBUTE_TYPE_LIGHT)
        {
            ((vsLightAttribute *)attributeList[loop])->setScene(NULL);
        }
    }

    // Traverse all children and mark them dirty as well
    for (loop = 0; loop < getChildCount(); loop++)
        getChild(loop)->dirtyDown();
}
