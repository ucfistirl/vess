// File vsNode.c++

#include "vsNode.h++"

// ------------------------------------------------------------------------
// Constructor - Clears the node's name
// ------------------------------------------------------------------------
vsNode::vsNode() : parentList(1, 5)
{
    nodeName[0] = 0;
    parentCount = 0;
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
        attr = getAttribute(0);
        removeAttribute(attr);
        if (!(attr->isAttached()))
            delete attr;
    }
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
    if ((index < 0) || (index >= parentCount))
    {
        printf("vsNode::getParent: Bad parent index\n");
        return NULL;
    }
    
    return (vsComponent *)(parentList[index]);
}

// ------------------------------------------------------------------------
// Sets the name of this node to the specified name
// ------------------------------------------------------------------------
void vsNode::setName(const char *newName)
{
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
// Adds the specified attribute to the node's list, and notifies the
// attribute that it has been added.
// ------------------------------------------------------------------------
void vsNode::addAttribute(vsAttribute *newAttribute)
{
    vsAttributeList::addAttribute(newAttribute);
    newAttribute->attach(this);
}

// ------------------------------------------------------------------------
// Removes the specified attribute from the node's list, and notifies the
// attribute that it has been removed.
// ------------------------------------------------------------------------
void vsNode::removeAttribute(vsAttribute *targetAttribute)
{
    targetAttribute->detach(this);
    vsAttributeList::removeAttribute(targetAttribute);
}

// ------------------------------------------------------------------------
// VESS internal function
// Adds a node to this node's list of parent nodes
// ------------------------------------------------------------------------
void vsNode::addParent(vsComponent *newParent)
{
    parentList[parentCount++] = newParent;
}

// ------------------------------------------------------------------------
// VESS internal function
// Removes a node from this node's list of parent nodes
// ------------------------------------------------------------------------
void vsNode::removeParent(vsComponent *targetParent)
{
    int loop, sloop;
    
    for (loop = 0; loop < parentCount; loop++)
        if (targetParent == parentList[loop])
        {
            for (sloop = loop; sloop < parentCount-1; sloop++)
                parentList[sloop] = parentList[sloop+1];
            parentCount--;
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
    
    flag = 1;
    for (loop = 0; loop < parentCount; loop++)
	if (((vsNode *)(parentList[loop]))->isDirty())
	    flag = 0;

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
    
    dirtyFlag = VS_TRUE;
    
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
