// File vsNode.c++

#include "vsNode.h++"

// ------------------------------------------------------------------------
// Constructor - Clears the node's name
// ------------------------------------------------------------------------
vsNode::vsNode()
{
    nodeName[0] = 0;
    parentCount = 0;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsNode::~vsNode()
{
}

// ------------------------------------------------------------------------
// Retrieves one of the parent components of this node, specified by index.
// The index of the first parent is 0.
// ------------------------------------------------------------------------
vsNode *vsNode::getParent(int index)
{
    if ((index < 0) || (index >= parentCount))
    {
        printf("vsNode::getParent: Bad parent index\n");
        return NULL;
    }
    
    return parentList[index];
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
// VESS internal function
// Adds a node to this node's list of parent nodes
// ------------------------------------------------------------------------
void vsNode::addParent(vsNode *newParent)
{
    parentList[parentCount++] = newParent;
}

// ------------------------------------------------------------------------
// VESS internal function
// Removes a node from this node's list of parent nodes
// ------------------------------------------------------------------------
void vsNode::removeParent(vsNode *targetParent)
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
        (attributeList[loop])->saveCurrent();
}

// ------------------------------------------------------------------------
// VESS internal function
// Calls the apply function on all attached attributes
// ------------------------------------------------------------------------
void vsNode::applyAttributes()
{
    int loop;
    
    for (loop = 0; loop < attributeCount; loop++)
        (attributeList[loop])->apply();
}

// ------------------------------------------------------------------------
// VESS internal function
// Calls the restoreSaved function on all attached attributes
// ------------------------------------------------------------------------
void vsNode::restoreSavedAttributes()
{
    int loop;
    
    for (loop = 0; loop < attributeCount; loop++)
        (attributeList[loop])->restoreSaved();
}

// ------------------------------------------------------------------------
// static VESS internal function - Performer callback
// Saves and changes the current graphics state before a node is
// traversed during a Performer DRAW traversal
// ------------------------------------------------------------------------
int vsNode::preDrawCallback(pfTraverser *_trav, void *_userData)
{
    ((vsNode *)_userData)->saveCurrentAttributes();
    ((vsNode *)_userData)->applyAttributes();
    
    return PFTRAV_CONT;
}

// ------------------------------------------------------------------------
// static VESS internal function - Performer callback
// Restores changes to the graphics state after a node is traversed during
// a Performer DRAW traversal
// ------------------------------------------------------------------------
int vsNode::postDrawCallback(pfTraverser *_trav, void *_userData)
{
    ((vsNode *)_userData)->restoreSavedAttributes();

    return PFTRAV_CONT;
}
