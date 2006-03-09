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
//    VESS Module:  vsMenuTree.c++
//
//    Description:  The menu tree class describes a menu structure that is
//                  used by the vsMenuSystem for navigation. The tree is
//                  stored in first-child-next-sibling format.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsMenuTree.h++"

// ------------------------------------------------------------------------
// Constructor - This constructor builds a vsMenuTree with no object nodes.
// ------------------------------------------------------------------------
vsMenuTree::vsMenuTree()
{
    // Create a blank node for the root and initialize its fields to NULL
    rootNode = (vsMenuTreeNode *)malloc(sizeof(vsMenuTreeNode));
    rootNode->object = NULL;
    rootNode->child = NULL;
    rootNode->sibling = NULL;
    rootNode->parent = NULL;
}

// ------------------------------------------------------------------------
// Destructor - The destructor deletes the nodes that make up the tree,
// freeing any vsMenuObjects that are unreferenced after their removal
// from the tree
// ------------------------------------------------------------------------
vsMenuTree::~vsMenuTree()
{
    destroyTree(rootNode);
}

// ------------------------------------------------------------------------
// This method adds an object as a child to the node at the location
// specified by the vsMenuFrame. It returns true if it was successful and
// false otherwise (if the frame was invalid, for example). The new node is
// added to the end of the child list, so it won't invalidate or confuse
// existing frames. The object added has its reference count incremented.
// ------------------------------------------------------------------------
bool vsMenuTree::addObject(vsMenuFrame *frame, vsMenuObject *object)
{
    vsMenuTreeNode *parent;
    vsMenuTreeNode *lastSibling;
    vsMenuTreeNode *newNode;

    // Get the new nodes parent
    parent = getNode(frame);

    // Make sure the parent location is valid
    if (parent == NULL)
        return false;

    // Create the new node and fill in its fields
    newNode = (vsMenuTreeNode *)malloc(sizeof(vsMenuTreeNode));
    newNode->parent = parent;
    newNode->child = NULL;
    newNode->sibling = NULL;
    newNode->object = object;

    // If the parent has no children, the node will be its only child
    if (parent->child == NULL)
    {
        parent->child = newNode;
    }
    else
    {
        // Start at the first sibling of this new node
        lastSibling = parent->child;

        // Move to the end of the sibling list
        while (lastSibling->sibling != NULL)
            lastSibling = lastSibling->sibling;

        // Attach the new node to the end of the list
        lastSibling->sibling = newNode;
    }

    // Reference the object that was just added
    object->ref();

    // Indicate that the object was successfully added
    return true;
}

// ------------------------------------------------------------------------
// This method removes the node at the location specified by the frame,
// deleting that nodes object if it is no longer referenced. If the node
// has children, all of those nodes and objects are recursively deleted.
// ------------------------------------------------------------------------
bool vsMenuTree::removeObject(vsMenuFrame *frame)
{
    vsMenuTreeNode *node;

    // Get the node to be deleted
    node = getNode(frame);

    // Make sure the location is valid, ensuring that the user does not
    // attempt to delete the root node
    if ((node == NULL) || (node == rootNode))
        return false;

    // Delete this node along with any others
    destroyTree(node);

    // Indicate that the object was successfully removed
    return true;
}

// ------------------------------------------------------------------------
// This function will count the children of the node that the given frame
// points to.
// ------------------------------------------------------------------------
int vsMenuTree::getChildCount(vsMenuFrame *frame)
{
    vsMenuTreeNode *node;
    int count;

    // Get the node at the specified location.
    node = getNode(frame);

    // Confirm that the node exists, and that it has at least one child.
    if (node && node->child)
    {
        // Begin at the first child and accumulate.
        node = node->child;
        count = 0;
        while (node)
        {
            count++;
            node = node->sibling;
        }

        return count;
    }

    return 0;
}

// ------------------------------------------------------------------------
// This method returns the object at the location in the tree specified by
// the given frame, or NULL if the frame is invalid.
// ------------------------------------------------------------------------
vsMenuObject *vsMenuTree::getObject(vsMenuFrame *frame)
{
    vsMenuTreeNode *node;

    // Get the node at the specified location.
    node = getNode(frame);

    // If the node exists
    if (node)
    {
       return node->object;
    }

    // Return 0 if the node doesn't exist or has no children.
    return NULL;
}

// ------------------------------------------------------------------------
// Internal Function
// This method returns the vsMenuTreeNode in the tree at the location given
// by the specified frame, or NULL if the frame is invalid for this tree.
// ------------------------------------------------------------------------
vsMenuTreeNode *vsMenuTree::getNode(vsMenuFrame *frame)
{
    vsMenuTreeNode *currentNode;
    int depth;
    int pos;

    // Begin at the root node of this tree
    currentNode = rootNode;

    // If the menu frame is null just break with the root node
    if (frame)
    {
        for (depth = 0; depth < frame->getDepth(); depth++)
        {
            // Descend one level for each index described by the frame
            currentNode = currentNode->child;

            // Make sure the node is still valid
            if (currentNode == NULL)
                return NULL;

            // Move over by the indicated number of children
            for (pos = 0; pos < frame->getIndex(depth); pos++)
            {
                currentNode = currentNode->sibling;

                // Make sure the node is still valid
                if (currentNode == NULL)
                    return NULL;
            }
        }
    }

    return currentNode;
}

// ------------------------------------------------------------------------
// Private Function
// This method recursively deletes any nodes that are children of the
// current node, then deletes the current node itself. The objects that are
// associated with these nodes are deleted if they are no longer referenced
// after having their count decremented by this function.
// ------------------------------------------------------------------------
void vsMenuTree::destroyTree(vsMenuTreeNode *node)
{
    vsMenuTreeNode *child;
    vsMenuTreeNode *parent;
    vsMenuTreeNode *lastSibling;

    // First, recurse on each of the children of this node
    child = node->child;
    while (child)
    {
        // Destroy the child
        destroyTree(child);

        // Move to the next child of this node
        child = node->child;
    }

    parent = node->parent;

    // Make sure the parent exists; if not, this is the root node, so it has
    // no siblings or parents that require extra handling
    if (parent)
    {
        // Now, see if this node is the first child of the parent node
        if (parent->child == node)
        {
            // Detach the current node from the tree
            parent->child = node->sibling;
        }
        else
        {
            // Otherwise, find the previous sibling of this node
            lastSibling = parent->child;
            while (lastSibling->sibling != node)
                lastSibling = lastSibling->sibling;

            // Detach the current node from the tree
            lastSibling->sibling = node->sibling;
        }

        // Unreference and attempt to delete the removed object
        vsObject::unrefDelete(node->object);
    }

    // Free the memory used for the node itself
    free(node);
}
