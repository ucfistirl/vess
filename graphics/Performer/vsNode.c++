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

vsObjectMap *vsNode::nodeMap = NULL;

// ------------------------------------------------------------------------
// Constructor - Clears the node's name
// ------------------------------------------------------------------------
vsNode::vsNode() : attributeList(10, 5)
{
    nodeName[0] = 0;
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
// Destroys the entire scene graph rooted at this node, up to but not
// including this node itself. Won't delete instanced nodes unless all
// of the parents of the node are being deleted as well.
// As nodes don't have any children by default, this version of the
// function does nothing.
// ------------------------------------------------------------------------
void vsNode::deleteTree()
{
}

// ------------------------------------------------------------------------
// Add a node to this node's child list
// ------------------------------------------------------------------------
int vsNode::addChild(vsNode *newChild)
{
    return VS_FALSE;
}

// ------------------------------------------------------------------------
// Insert a node into this node's child list at the specified index
// ------------------------------------------------------------------------
int vsNode::insertChild(vsNode *newChild, int index)
{
    return VS_FALSE;
}

// ------------------------------------------------------------------------
// Remove a node from this node's child list
// ------------------------------------------------------------------------
int vsNode::removeChild(vsNode *targetChild)
{
    return VS_FALSE;
}

// ------------------------------------------------------------------------
// Replace a node in this node's child list with a new node
// ------------------------------------------------------------------------
int vsNode::replaceChild(vsNode *targetChild, vsNode *newChild)
{
    return VS_FALSE;
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

    return nodeSearch(targetName, &idx);
}

// ------------------------------------------------------------------------
// Adds the specified attribute to the node's list, and notifies the
// attribute that it has been added.
// ------------------------------------------------------------------------
void vsNode::addAttribute(vsAttribute *newAttribute)
{
    attributeList[attributeCount] = newAttribute;
    attributeCount++;
    newAttribute->ref();

    newAttribute->attach(this);
}

// ------------------------------------------------------------------------
// Removes the specified attribute from the node's list, and notifies the
// attribute that it has been removed.
// ------------------------------------------------------------------------
void vsNode::removeAttribute(vsAttribute *targetAttribute)
{
    int loop, sloop;

    for (loop = 0; loop < attributeCount; loop++)
        if (attributeList[loop] == targetAttribute)
        {
            targetAttribute->detach(this);
            
            for (sloop = loop; sloop < attributeCount-1; sloop++)
                attributeList[sloop] = attributeList[sloop+1];
            attributeCount--;
            targetAttribute->unref();
            
            return;
        }

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
    if ((index < 0) || (index >= attributeCount))
    {
        printf("vsNode::getAttribute: Index out of bounds\n");
        return NULL;
    }

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

    count = 0;
    for (loop = 0; loop < attributeCount; loop++)
        if (attribType ==
            ((vsAttribute *)(attributeList[loop]))->getAttributeType())
        {
            if (index == count)
                return (vsAttribute *)(attributeList[loop]);
            else
                count++;
        }

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

    count = 0;
    for (loop = 0; loop < attributeCount; loop++)
        if (attribCategory ==
            ((vsAttribute *)(attributeList[loop]))->getAttributeCategory())
        {
            if (index == count)
                return (vsAttribute *)(attributeList[loop]);
            else
                count++;
        }

    return NULL;
}

// ------------------------------------------------------------------------
// Retrieves the attribute with the given name from the list
// ------------------------------------------------------------------------
vsAttribute *vsNode::getNamedAttribute(char *attribName)
{
    int loop;

    for (loop = 0; loop < attributeCount; loop++)
        if (!strcmp(attribName,
            ((vsAttribute *)(attributeList[loop]))->getName()))
            return (vsAttribute *)(attributeList[loop]);

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

    return NULL;
}

// ------------------------------------------------------------------------
// Static internal function
// Gets the object map that holds the node mappings, creating it if needed
// ------------------------------------------------------------------------
vsObjectMap *vsNode::getMap()
{
    if (!nodeMap)
        nodeMap = new vsObjectMap();

    return nodeMap;
}

// ------------------------------------------------------------------------
// Static internal function
// Deletes the object map that holds the node mappings, if it exists
// ------------------------------------------------------------------------
void vsNode::deleteMap()
{
    if (nodeMap)
    {
        delete nodeMap;
        nodeMap = NULL;
    }
}

// ------------------------------------------------------------------------
// Internal function
// Adds a node to this node's list of parent nodes
// ------------------------------------------------------------------------
int vsNode::addParent(vsNode *newParent)
{
    return VS_FALSE;
}

// ------------------------------------------------------------------------
// Internal function
// Removes a node from this node's list of parent nodes
// ------------------------------------------------------------------------
int vsNode::removeParent(vsNode *targetParent)
{
    return VS_FALSE;
}

// ------------------------------------------------------------------------
// Internal function
// Calls the saveCurrent function on all attached attributes
// ------------------------------------------------------------------------
void vsNode::saveCurrentAttributes()
{
    int loop;

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
    dirtyUp();
    dirtyDown();
}

// ------------------------------------------------------------------------
// Internal function
// Marks this node as clean if all of its parents are clean
// ------------------------------------------------------------------------
void vsNode::clean()
{
    int loop, flag;

    flag = 1;
    for (loop = 0; loop < getParentCount(); loop++)
        if (getParent(loop)->isDirty())
            flag = 0;

    if (flag)
        dirtyFlag = VS_FALSE;
}

// ------------------------------------------------------------------------
// Internal function
// Determines if this node is dirty or not
// ------------------------------------------------------------------------
int vsNode::isDirty()
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

    dirtyFlag = VS_TRUE;

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

    dirtyFlag = VS_TRUE;

    for (loop = 0; loop < getChildCount(); loop++)
        getChild(loop)->dirtyDown();
}
