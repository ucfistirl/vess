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

#include "vsNode.h++"
#include "vsComponent.h++"

// ------------------------------------------------------------------------
// Constructor - Clears the node's name
// ------------------------------------------------------------------------
vsNode::vsNode() : parentList(1, 5), attributeList(10, 5)
{
    // Initialize: no node name, no parents, no attributes, dirty
    nodeName[0] = 0;
    parentCount = 0;
    attributeCount = 0;
    dirtyFlag = VS_TRUE;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsNode::~vsNode()
{
    vsAttribute *attr;

    // Remove all attached attributes; destroy those that aren't being
    // used by other nodes.
    while (getAttributeCount() > 0)
    {
        // Remove the first attribute; delete it if it's unused.
        attr = getAttribute(0);
        removeAttribute(attr);
        if (!(attr->isAttached()))
            delete attr;
    }
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
// Retrieves the number of parent components for this node
// ------------------------------------------------------------------------
int vsNode::getParentCount()
{
    return parentCount;
}

// ------------------------------------------------------------------------
// Retrieves one of the parent components of this node, specified by index.
// The index of the first parent is 0.
// ------------------------------------------------------------------------
vsComponent *vsNode::getParent(int index)
{
    // Bounds check
    if ((index < 0) || (index >= parentCount))
    {
        printf("vsNode::getParent: Bad parent index\n");
        return NULL;
    }
    
    // Return the desired parent
    return (vsComponent *)(parentList[index]);
}

// ------------------------------------------------------------------------
// Sets the name of this node to the specified name
// ------------------------------------------------------------------------
void vsNode::setName(const char *newName)
{
    // Copy the node's new name from the specified location into our
    // name buffer, clipping the name at the designated maximum length.
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

    // Call our private search function to do the work
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

    // Call our private search function to do the work
    return nodeSearch(targetName, &idx);
}

// ------------------------------------------------------------------------
// Adds the specified attribute to the node's list, and notifies the
// attribute that it has been added.
// ------------------------------------------------------------------------
void vsNode::addAttribute(vsAttribute *newAttribute)
{
    // Add the attribute pointer to our list of attributes
    attributeList[attributeCount] = newAttribute;
    attributeCount++;
    newAttribute->ref();

    // Let the attribute know that it has a new owner
    newAttribute->attach(this);
}

// ------------------------------------------------------------------------
// Removes the specified attribute from the node's list, and notifies the
// attribute that it has been removed.
// ------------------------------------------------------------------------
void vsNode::removeAttribute(vsAttribute *targetAttribute)
{
    int loop, sloop;

    // Search the attribute list for the specified attribute
    for (loop = 0; loop < attributeCount; loop++)
        if (attributeList[loop] == targetAttribute)
        {
            // Let the attribute know that it's losing an owner
            targetAttribute->detach(this);
            
            // Remove the attribute form the list; slide the
	    // other attributes down to fill the gap
            for (sloop = loop; sloop < attributeCount-1; sloop++)
                attributeList[sloop] = attributeList[sloop+1];
            attributeCount--;
            targetAttribute->unref();
            
            // Done
            return;
        }

    // Signal an error if the attribute was not found
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
    // Bounds check
    if ((index < 0) || (index >= attributeCount))
    {
        printf("vsNode::getAttribute: Index out of bounds\n");
        return NULL;
    }

    // Return the desired attribute
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

    // Search for attributes of the given type
    count = 0;
    for (loop = 0; loop < attributeCount; loop++)
    {
        // Check the type of the loop'th attribute
        if (attribType ==
            ((vsAttribute *)(attributeList[loop]))->getAttributeType())
        {
            // If the types match, then check if we have the right
	    // index as well; return the attribute if so, increment the
	    // index if not.
            if (index == count)
                return (vsAttribute *)(attributeList[loop]);
            else
                count++;
        }
    }

    // Return NULL if the attribute wasn't found
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

    // Search for attributes of the given category
    count = 0;
    for (loop = 0; loop < attributeCount; loop++)
    {
        // Check the category of the loop'th attribute
        if (attribCategory ==
            ((vsAttribute *)(attributeList[loop]))->getAttributeCategory())
        {
            // If the categories match, then check if we have the right
	    // index as well; return the attribute if so, increment the
	    // index if not.
            if (index == count)
                return (vsAttribute *)(attributeList[loop]);
            else
                count++;
        }
    }

    // Return NULL if the attribute wasn't found
    return NULL;
}

// ------------------------------------------------------------------------
// Retrieves the attribute with the given name from the list
// ------------------------------------------------------------------------
vsAttribute *vsNode::getNamedAttribute(char *attribName)
{
    int loop;

    // Compare the given name with the name of every attribute in the list
    for (loop = 0; loop < attributeCount; loop++)
        if (!strcmp(attribName,
            ((vsAttribute *)(attributeList[loop]))->getName()))
            return (vsAttribute *)(attributeList[loop]);

    // Return NULL if the attribute wasn't found
    return NULL;
}

// ------------------------------------------------------------------------
// VESS internal function
// Adds a node to this node's list of parent nodes
// ------------------------------------------------------------------------
void vsNode::addParent(vsComponent *newParent)
{
    // Add the parent to the parent list
    parentList[parentCount++] = newParent;
    newParent->ref();
}

// ------------------------------------------------------------------------
// VESS internal function
// Removes a node from this node's list of parent nodes
// ------------------------------------------------------------------------
void vsNode::removeParent(vsComponent *targetParent)
{
    int loop, sloop;

    // Search the parent list for the specified parent
    for (loop = 0; loop < parentCount; loop++)
        if (targetParent == parentList[loop])
        {
            // Slide the list of parents down over the parent that is
	    // being removed
            for (sloop = loop; sloop < parentCount-1; sloop++)
                parentList[sloop] = parentList[sloop+1];
            parentCount--;

            // Clean up and finish
            targetParent->unref();
            return;
        }
}

// ------------------------------------------------------------------------
// VESS internal function
// Calls the saveCurrent function on all attached attributes
// ------------------------------------------------------------------------
void vsNode::saveCurrentAttributes()
{
    int loop;

    for (loop = 0; loop < attributeCount; loop++)
        ((vsAttribute *)(attributeList[loop]))->saveCurrent();
}

// ------------------------------------------------------------------------
// VESS internal function
// Calls the apply function on all attached attributes
// ------------------------------------------------------------------------
void vsNode::applyAttributes()
{
    int loop;

    for (loop = 0; loop < attributeCount; loop++)
        ((vsAttribute *)(attributeList[loop]))->apply();
}

// ------------------------------------------------------------------------
// VESS internal function
// Calls the restoreSaved function on all attached attributes
// ------------------------------------------------------------------------
void vsNode::restoreSavedAttributes()
{
    int loop;

    for (loop = 0; loop < attributeCount; loop++)
        ((vsAttribute *)(attributeList[loop]))->restoreSaved();
}

// ------------------------------------------------------------------------
// VESS internal function
// Marks this node dirty, as well as every other node above and below it
// in the tree. This is done because only dirty nodes are traversed during
// the VESS traversal; nodes above to get to this node, and nodes below
// to allow any state changes to propogate down.
// ------------------------------------------------------------------------
void vsNode::dirty()
{
    // Dirty all nodes above and below this one
    dirtyUp();
    dirtyDown();
}

// ------------------------------------------------------------------------
// VESS internal function
// Marks this node as clean if all of its parents are clean
// ------------------------------------------------------------------------
void vsNode::clean()
{
    int loop, flag;

    // Check to see if any of the parents of this node is still dirty
    flag = 1;
    for (loop = 0; loop < parentCount; loop++)
        if (((vsNode *)(parentList[loop]))->isDirty())
            flag = 0;

    // If the parents are clean, clean this node
    if (flag)
        dirtyFlag = VS_FALSE;
}

// ------------------------------------------------------------------------
// VESS internal function
// Determines if this node is dirty or not
// ------------------------------------------------------------------------
int vsNode::isDirty()
{
    return dirtyFlag;
}

// ------------------------------------------------------------------------
// VESS internal function
// Marks this node and each node above this one as dirty
// ------------------------------------------------------------------------
void vsNode::dirtyUp()
{
    int loop;

    // Dirty this node
    dirtyFlag = VS_TRUE;

    // Dirty the parents
    for (loop = 0; loop < parentCount; loop++)
        ((vsNode *)(parentList[loop]))->dirtyUp();
}

// ------------------------------------------------------------------------
// VESS internal function
// Marks this node as dirty
// ------------------------------------------------------------------------
void vsNode::dirtyDown()
{
    dirtyFlag = VS_TRUE;
}
