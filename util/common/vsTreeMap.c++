
#include <stdlib.h>
#include <stdio.h>
#include "atGlobals.h++"
#include "atString.h++"

#include "vsTreeMap.h++"

// ------------------------------------------------------------------------
// Constructor - Sets the tree to empty
// ------------------------------------------------------------------------
vsTreeMap::vsTreeMap()
{
    // Tree initially has not root node and no nodes
    treeRoot = NULL;
    treeSize = 0;
}

// ------------------------------------------------------------------------
// Destructor - Deletes the contents of the tree
// ------------------------------------------------------------------------
vsTreeMap::~vsTreeMap()
{
    // Delete all tree entries
    clear();
}

// ------------------------------------------------------------------------
// Adds a new mapping from key to value to the tree. Returns true if
// successful, or false if a mapping for that key already exists.
// ------------------------------------------------------------------------
bool vsTreeMap::addEntry(atItem * key, atItem * value)
{
    bool found;

    // Make sure that a node with the given key isn't already in
    // the tree.
    if (containsKey(key))
        return false;

    // Create the new node using the given key and value. New nodes
    // are initially colored red.
    vsTreeMapNode *newNode = new vsTreeMapNode;
    newNode->leftChild = NULL;
    newNode->rightChild = NULL;
    newNode->parent = NULL;
    newNode->color = VS_TREE_MAP_RED;
    newNode->nodeKey = key;
    newNode->nodeValue = value;
    
    // If the tree is empty, then the new node becomes the root.
    if (treeRoot == NULL)
    {
        // Set the root node, color it black (root nodes are always black),
        // increment the entry count, and return success.
        treeRoot = newNode;
        treeRoot->color = VS_TREE_MAP_BLACK;
        treeSize++;
        return true;
    }
    
    // The tree isn't empty. Do a binary search on the tree to determine
    // the correct location for the new node.
    vsTreeMapNode *nodeParent = treeRoot;
    found = false;
    while (!found)
    {
        // Branch left or right based on key comparison
        if (newNode->nodeKey->compare(nodeParent->nodeKey) < 0)
        {
            // Left subtree
            if (nodeParent->leftChild == NULL)
            {
                // Place the new node as the left child
                nodeParent->leftChild = newNode;
                newNode->parent = nodeParent;
                found = true;
            }
            else
            {
                // Move to the left child and keep searching
                nodeParent = nodeParent->leftChild;
            }
        }
        else
        {
            // Check for collision and print a loud warning if detected
            if (newNode->nodeKey->equals(nodeParent->nodeKey))
               notify(AT_WARN, "Key collision detected in map!");

            // Right subtree
            if (nodeParent->rightChild == NULL)
            {
                // Place the new node as the right child
                nodeParent->rightChild = newNode;
                newNode->parent = nodeParent;
                found = true;
            }
            else
            {
                // Move to the right child and keep searching
                nodeParent = nodeParent->rightChild;
            }
        }
    }
    
    // Clean up the tree after the insertion
    rebalanceInsert(newNode);
    treeRoot->color = VS_TREE_MAP_BLACK;

    // Increase entry count by one and return success
    treeSize++;
    return true;
}

// ------------------------------------------------------------------------
// Removes the entry specified by the key from the map, returning the
// value.  Yields ownership of both the key and value (neither are 
// deleted).  Returns NULL if there is no entry with the given key.
// ------------------------------------------------------------------------
atPair * vsTreeMap::removeEntry(atItem * key)
{
    vsTreeMapNode * targetNode;
    atItem * targetKey;
    atItem * targetValue;
    
    // Find the node in the tree with the given key. Abort if there
    // is no such node.
    targetNode = findNode(treeRoot, key);
    if (targetNode == NULL)
        return NULL;

    // Get the node's key and value entries before we remove the node
    // (we'll need them later)
    targetKey = targetNode->nodeKey;
    targetValue = targetNode->nodeValue;

    // Call an internal function to do the actual node removal
    removeNode(targetNode);

    // The last part of cleaning up the tree, which is the only part that
    // removeNode() doesn't do by itself, is forcing the root node to be
    // black.
    if (treeRoot)
        treeRoot->color = VS_TREE_MAP_BLACK;

    // Decrease entry count by one
    treeSize--;

    // Create an atPair for the removed key and value, and return it
    return new atPair(targetKey, targetValue);
}

// ------------------------------------------------------------------------
// Removes all map entries, leaving the contents intact
// ------------------------------------------------------------------------
void vsTreeMap::removeAllEntries()
{
    // No work to do if the tree is already empty
    if (treeRoot == NULL)
        return;

    // The deleteTree function does all of the actual work
    deleteTree(treeRoot, false);
    
    // Set the tree to empty
    treeRoot = NULL;
    treeSize = 0;
}

// ------------------------------------------------------------------------
// Returns the number of mappings contained in this tree
// ------------------------------------------------------------------------
u_long vsTreeMap::getNumEntries()
{
    return treeSize;
}

// ------------------------------------------------------------------------
// Checks if a mapping for the given key is present in the tree. Returns
// true if so, false if not.
// ------------------------------------------------------------------------
bool vsTreeMap::containsKey(atItem * key)
{
    // Call our helper function to find the node with the given key
    if (findNode(treeRoot, key) != NULL)
        return true;
    else
        return false;
}

// ------------------------------------------------------------------------
// Returns the value associated with the given key, or NULL if that key is
// not present within the tree.
// ------------------------------------------------------------------------
atItem * vsTreeMap::getValue(atItem * key)
{
    vsTreeMapNode *node;
    
    // Call our helper function to find the node with the given key
    node = findNode(treeRoot, key);
    if (node)
        return (node->nodeValue);

    // Return NULL if the target key wasn't found
    return NULL;
}

// ------------------------------------------------------------------------
// Attempts to change the value associated with the given key to newValue.
// Return the old value if successful, NULL if the given key is not present
// within the tree.
// ------------------------------------------------------------------------
atItem * vsTreeMap::changeValue(atItem * key, atItem * newValue)
{
    vsTreeMapNode *node;
    atItem *oldValue;
    
    // Call our helper function to find the node with the given key
    node = findNode(treeRoot, key);
    if (!node)
        return NULL;

    // If found, remove the current value, set the new value and return the 
    // old value
    oldValue = node->nodeValue;
    node->nodeValue = newValue;
    return oldValue;
}

// ------------------------------------------------------------------------
// Fills the keyList and valueList with the keys and values from the tree,
// respectively. Each element of one list corresponds to the element with
// the same index from the other list. Corresponding element pairs are
// sorted in ascending key order.
// ------------------------------------------------------------------------
void vsTreeMap::getSortedList(atList * keyList, atList * valueList)
{
    // No work to do if the tree is empty
    if (treeSize == 0)
        return;

    // Call a helper function to copy the tree data to the arrays
    fillLists(treeRoot, keyList, valueList);

    // Error checking
    if (keyList != NULL)
    {
        // Make sure the key list makes sense
        if (keyList->getNumEntries() != treeSize)       
        {      
            notify(AT_ERROR, "vsTreeMap::getSortedList: Map Inconsistency:\n");   
            notify(AT_ERROR, "   Number of entries in map %d is not equal to "
                "the map's stated size %d\n", keyList->getNumEntries(),
                treeSize);      
        }
    }
    else if (valueList != NULL)
    {
        // Make sure the value list makes sense
        if (valueList->getNumEntries() != treeSize)       
        {      
            notify(AT_ERROR, "vsTreeMap::getSortedList: Map Inconsistency:\n");   
            notify(AT_ERROR, "   Number of entries in map %d is not equal to "
                "the map's stated size %d\n", valueList->getNumEntries(),
                treeSize);      
        }
    }
}

// ------------------------------------------------------------------------
// Private Function:
// Removes all mappings from the tree and deletes all contents
// ------------------------------------------------------------------------
void vsTreeMap::clear()
{
    // No work to do if the tree is already empty
    if (treeRoot == NULL)
        return;

    // The deleteTree function does all of the actual work
    deleteTree(treeRoot, true);
    
    // Set the tree to empty
    treeRoot = NULL;
    treeSize = 0;
}

// ------------------------------------------------------------------------
// Private function
// Searches the subtree rooted at 'node' for a node with the given key.
// Returns that node, or NULL if it can't find it.
// ------------------------------------------------------------------------
vsTreeMapNode *vsTreeMap::findNode(vsTreeMapNode * node, atItem * key)
{
    // If the target node is NULL, then this definitely isn't the
    // node we're looking for
    if (node == NULL)
        return NULL;

    // If the keys match, return the target node
    if (node->nodeKey->equals(key))
        return node;

    // Otherwise, search a child for the key; which child to search is
    // determined by comparing key values
    if (key->compare(node->nodeKey) > 0)
        return findNode(node->rightChild, key);
    else
        return findNode(node->leftChild, key);
}

// ------------------------------------------------------------------------
// Private function
// Rebalances the tree after an insertion operation. After inserting, since
// new nodes are colored red, only check for red-red rule violations; the
// black-balance rule can't have been violated.
// ------------------------------------------------------------------------
void vsTreeMap::rebalanceInsert(vsTreeMapNode * node)
{
    vsTreeMapNode *parent, *grandparent, *uncle;
    int nodeChildType, parentChildType;

    // If this node is black, there's no work to do
    if (node->color == VS_TREE_MAP_BLACK)
        return;

    // If the parent is black (or nonexistant), there's no work to do
    parent = node->parent;
    if (parent == NULL)
        return;
    if (parent->color == VS_TREE_MAP_BLACK)
        return;

    // If there's no grandparent node, then there's no work to do here.
    // Both this node and its parent are red, which should be a violation,
    // but if there's no grandparent then the parent must be the tree's
    // root node, and the root is automatically set to black as the last
    // step of insertion cleanup.
    grandparent = parent->parent;
    if (grandparent == NULL)
        return;

    // If this node's 'uncle' is red, then balance can be restored by
    // simply 'splitting' the grandparent's black value; parent and
    // uncle become black, and grandparent becomes red, which fixes
    // the red-red violation without affecting the black-balance. However,
    // this can cause a red-red violation at grandparent if it is changed
    // to red, so the rebalancing process must iterate again up the tree.
    parentChildType = getChildType(parent);
    if (parentChildType == VS_TREE_MAP_LEFTCHILD)
        uncle = grandparent->rightChild;
    else
        uncle = grandparent->leftChild;
    if (uncle && (uncle->color == VS_TREE_MAP_RED))
    {
        grandparent->color = VS_TREE_MAP_RED;
        parent->color = VS_TREE_MAP_BLACK;
        uncle->color = VS_TREE_MAP_BLACK;
        rebalanceInsert(grandparent);
        return;
    }

    // At this point, a rotation or two and some strategic node recoloring
    // should fix the problem.
    nodeChildType = getChildType(node);
    if (parentChildType == VS_TREE_MAP_LEFTCHILD)
    {
        // Force node to be a left-child, if it isn't already
        if (nodeChildType == VS_TREE_MAP_RIGHTCHILD)
        {
            rotateLeft(parent);
            node = parent;
            parent = node->parent;
        }

        // A right rotation at grandparent and a color swap should fix
        // the red-red problem without introducing any other problems
        rotateRight(grandparent);
        parent->color = VS_TREE_MAP_BLACK;
        grandparent->color = VS_TREE_MAP_RED;
    }
    else
    {
        // Force node to be a right-child, if it isn't already
        if (nodeChildType == VS_TREE_MAP_LEFTCHILD)
        {
            rotateRight(parent);
            node = parent;
            parent = node->parent;
        }

        // A left rotation at grandparent and a color swap should fix
        // the red-red problem without introducing any other problems
        rotateLeft(grandparent);
        parent->color = VS_TREE_MAP_BLACK;
        grandparent->color = VS_TREE_MAP_RED;
    }
}

// ------------------------------------------------------------------------
// Private function
// Rebalance the tree after a deletion operation. Deletion operations can
// violate both the red-red rule and the black-balance rule, but since this
// function is only called after a black node was deleted then concentrate
// on restoring the black-balance and any red-red violations will get
// cleaned up at the same time.
// ------------------------------------------------------------------------
void vsTreeMap::rebalanceDelete(vsTreeMapNode * parent, int deletedChildType)
{
    vsTreeMapNode *child, *sibling;
    
    // If we deleted the root node, there's no rebalancing work to do
    if (deletedChildType == VS_TREE_MAP_ROOTNODE)
        return;

    // If the child that took the place of the deleted node exists (isn't
    // NULL) and is red, then changing it to black will restore the 
    // black-balance without doing any other damage.
    if (deletedChildType == VS_TREE_MAP_LEFTCHILD)
        child = parent->leftChild;
    else
        child = parent->rightChild;
    if (child && (child->color == VS_TREE_MAP_RED))
    {
        child->color = VS_TREE_MAP_BLACK;
        return;
    }
    
    // If we got this far, then we have to do it the hard way. Obtain
    // the 'sibling' (parent's other child) of the deleted node and
    // manipulate that in order to restore the black-balance. This sibling
    // node _must_ exist (can't be NULL) if a black node was deleted,
    // because otherwise the tree wouldn't have been black-balanced before
    // the deletion.
    if (deletedChildType == VS_TREE_MAP_LEFTCHILD)
    {
        // Get the sibling
        sibling = parent->rightChild;

        // If it isn't already, force the sibling to be black by rotatng
        // the subtree and swapping colors around.
        if (sibling->color == VS_TREE_MAP_RED)
        {
            rotateLeft(parent);
            parent->color = VS_TREE_MAP_RED;
            sibling->color = VS_TREE_MAP_BLACK;
            sibling = parent->rightChild;
        }
        
        // Case 1: Sibling's children are both black

        // If both of the children of the sibling node are black (or
        // nonexistant), then we can color the sibling red. However,
        // this effectively chases the problem farther up the tree,
        // so rebalance there.
        if ( ((sibling->leftChild == NULL) ||
              (sibling->leftChild->color == VS_TREE_MAP_BLACK)) &&
             ((sibling->rightChild == NULL) ||
              (sibling->rightChild->color == VS_TREE_MAP_BLACK)) )
        {
            sibling->color = VS_TREE_MAP_RED;
            rebalanceDelete(parent->parent, getChildType(parent));
            return;
        }
        
        // Case 2: At least one of sibling's children is red

        // If sibling's left child is red, then manipulate the
        // tree so that only the right child is red. This can
        // temporarily create a red-red violation, but the next block
        // of code will fix that.
        if ((sibling->leftChild) &&
            (sibling->leftChild->color == VS_TREE_MAP_RED))
        {
            sibling->leftChild->color = VS_TREE_MAP_BLACK;
            sibling->color = VS_TREE_MAP_RED;
            rotateRight(sibling);
            sibling = parent->rightChild;
        }
        
        // Sibling's right child must be red; the imbalance can be
        // repaired here by a rotation and some color swapping.
        rotateLeft(parent);
        sibling->color = parent->color;
        parent->color = VS_TREE_MAP_BLACK;
        sibling->rightChild->color = VS_TREE_MAP_BLACK;
    }
    else
    {
        // Get the sibling
        sibling = parent->leftChild;

        // If it isn't already, force the sibling to be black by rotatng
        // the subtree and swapping colors around.
        if (sibling->color == VS_TREE_MAP_RED)
        {
            rotateRight(parent);
            parent->color = VS_TREE_MAP_RED;
            sibling->color = VS_TREE_MAP_BLACK;
            sibling = parent->leftChild;
        }
        
        // Case 1: Sibling's children are both black

        // If both of the children of the sibling node are black (or
        // nonexistant), then we can color the sibling red. However,
        // this effectively chases the problem farther up the tree,
        // so rebalance there.
        if ( ((sibling->leftChild == NULL) ||
              (sibling->leftChild->color == VS_TREE_MAP_BLACK)) &&
             ((sibling->rightChild == NULL) ||
              (sibling->rightChild->color == VS_TREE_MAP_BLACK)) )
        {
            sibling->color = VS_TREE_MAP_RED;
            rebalanceDelete(parent->parent, getChildType(parent));
            return;
        }
        
        // Case 2: At least one of sibling's children is red

        // If sibling's right child is red, then manipulate the
        // tree so that only the left child is red. This can
        // temporarily create a red-red violation, but the next block
        // of code will fix that.
        if ((sibling->rightChild) &&
            (sibling->rightChild->color == VS_TREE_MAP_RED))
        {
            sibling->rightChild->color = VS_TREE_MAP_BLACK;
            sibling->color = VS_TREE_MAP_RED;
            rotateLeft(sibling);
            sibling = parent->leftChild;
        }
        
        // Sibling's left child must be red; the imbalance can be
        // repaired here by a rotation and some color swapping.
        rotateRight(parent);
        sibling->color = parent->color;
        parent->color = VS_TREE_MAP_BLACK;
        sibling->leftChild->color = VS_TREE_MAP_BLACK;
    }
}

// ------------------------------------------------------------------------
// Private function
// Removes the specified node from the tree, calling the function to
// restore the tree balance afterwards if needed.  Deletes the node
// structure, but leaves the node's contents alone (it's assumed that the
// calling function is keeping track of the node's contents).
// ------------------------------------------------------------------------
void vsTreeMap::removeNode(vsTreeMapNode * node)
{
    int childType = getChildType(node);
    vsTreeMapNode *parent = node->parent;
    vsTreeMapNode *child;
    atItem * tempItem;

    // Switch based on the number of children the node has
    if ((node->leftChild == NULL) && (node->rightChild == NULL))
    {
        // Case 1: node to delete has no children
        // Remove the node and rebalance

        // Remove the node
        if (childType == VS_TREE_MAP_LEFTCHILD)
            parent->leftChild = NULL;
        else if (childType == VS_TREE_MAP_RIGHTCHILD)
            parent->rightChild = NULL;
        else
            treeRoot = NULL;
        
        // Rebalance the tree if needed
        if (node->color == VS_TREE_MAP_BLACK)
            rebalanceDelete(parent, childType);
        
        // Delete the detached node
        delete node;
    }
    else if ((node->leftChild == NULL) || (node->rightChild == NULL))
    {
        // Case 2: node to delete has one child
        // Move the child node into the location that the node to
        // be deleted is in, and rebalance

        // Get the child node
        if (node->leftChild)
            child = node->leftChild;
        else
            child = node->rightChild;

        // Reparent the child node
        child->parent = parent;

        // Rechild the parent node
        if (childType == VS_TREE_MAP_LEFTCHILD)
            parent->leftChild = child;
        else if (childType == VS_TREE_MAP_RIGHTCHILD)
            parent->rightChild = child;
        else
            treeRoot = child;
        
        // Rebalance the tree if needed
        if (node->color == VS_TREE_MAP_BLACK)
            rebalanceDelete(parent, childType);

        // Delete the detached node
        delete node;
    }
    else
    {
        // Case 3: node to delete has two children
        // Rather than deleting the node, instead find the node with
        // the next-higher key value, transplant that value into the
        // node that would have been deleted, and delete that other node.

        // Find the node with the 'next' value
        child = getInorderSuccessor(node);
        
        // Swap the keys
        tempItem = node->nodeKey;
        node->nodeKey = child->nodeKey;
        child->nodeKey = tempItem;

        // Swap the values
        tempItem = node->nodeValue;
        node->nodeValue = child->nodeValue;
        child->nodeValue = tempItem;
        
        // Remove the 'next' node instead
        removeNode(child);
    }
}

// ------------------------------------------------------------------------
// Private function
// Searches the tree for the node with the next-higher key than the given
// node's key. Returns NULL if no such node exists.
// ------------------------------------------------------------------------
vsTreeMapNode *vsTreeMap::getInorderSuccessor(vsTreeMapNode * node)
{
    // If there is no node with a greater key, abort.
    if (node->rightChild == NULL)
        return NULL;

    // The node with the next highest key must be the node with the
    // smallest key in the original node's right subtree. 
    vsTreeMapNode *result = node->rightChild;
    while (result->leftChild)
        result = result->leftChild;

    // Return the node
    return result;
}

// ------------------------------------------------------------------------
// Private function
// Performs a left rotation at the subtree rooted at the given node. A
// left rotation rearranges nodes in this pattern:
//
//   parent                   parent
//     |                         |
//    left(= node)             right
//   /    \         ->        /     \
//  *      right          left       *
//        /     \        /    \
//   child       *      *      child
//
// 'parent' and 'child' may be NULL, 'left' and 'right' must not be.
// ------------------------------------------------------------------------
void vsTreeMap::rotateLeft(vsTreeMapNode * node)
{
    vsTreeMapNode *left, *right, *child, *parent;
    int childType;
    
    // 'right' must not be NULL
    if (node->rightChild == NULL)
    {
        notify(AT_ERROR, "vsTreeMap::rotateLeft: Can't rotate left on a node "
            "with no right child\n");
        return;
    }
    
    // Assign temporary pointers
    left = node;
    right = left->rightChild;
    child = right->leftChild;
    parent = left->parent;
    
    // Determine what kind of child the target node is
    childType = getChildType(node);
    
    // Perform the rotation
    left->rightChild = child;
    left->parent = right;
    right->leftChild = left;
    right->parent = parent;
    if (child)
        child->parent = left;
    
    // Correct which node the parent points to
    if (childType == VS_TREE_MAP_LEFTCHILD)
        parent->leftChild = right;
    else if (childType == VS_TREE_MAP_RIGHTCHILD)
        parent->rightChild = right;
    else
        treeRoot = right;
}

// ------------------------------------------------------------------------
// Private function
// Performs a right rotation at the subtree rooted at the given node. A
// right rotation rearranges nodes in this pattern:
//
//        parent               parent
//           |                   |
//         right(= node)        left
//        /     \         ->   /    \
//    left       *            *      right
//   /    \                         /     \
//  *      child               child       *
//
// 'parent' and 'child' may be NULL, 'right' and 'left' must not be.
// ------------------------------------------------------------------------
void vsTreeMap::rotateRight(vsTreeMapNode * node)
{
    vsTreeMapNode *left, *right, *child, *parent;
    int childType;
    
    // 'left' must not be NULL
    if (node->leftChild == NULL)
    {
        notify(AT_ERROR, "vsTreeMap::rotateRight: Can't rotate right on a node "
            "with no left child\n");
        return;
    }
    
    // Assign temporary pointers
    right = node;
    left = right->leftChild;
    child = left->rightChild;
    parent = right->parent;
    
    // Determine what kind of child the target node is
    childType = getChildType(node);

    // Perform the rotation
    right->leftChild = child;
    right->parent = left;
    left->rightChild = right;
    left->parent = parent;
    if (child)
        child->parent = right;
    
    // Correct which node the parent points to
    if (childType == VS_TREE_MAP_LEFTCHILD)
        parent->leftChild = left;
    else if (childType == VS_TREE_MAP_RIGHTCHILD)
        parent->rightChild = left;
    else
        treeRoot = left;
}

// ------------------------------------------------------------------------
// Private function
// Destroys the subtree rooted at the given node, without any cleaning-up
// of the tree afterwards. Called by the clear() and removeAllEntries()
// methods.
// ------------------------------------------------------------------------
void vsTreeMap::deleteTree(vsTreeMapNode * node, bool deleteContents)
{
    // No tree, no work
    if (node == NULL)
        return;

    // Recurse on the node's children
    deleteTree(node->leftChild, deleteContents);
    deleteTree(node->rightChild, deleteContents);
    
    // See if we're supposed to delete the node's contents
    if (deleteContents)
    {
        if (node->nodeKey != NULL)
            delete node->nodeKey;
        if (node->nodeValue != NULL)
            delete node->nodeValue;
    }

    // Destroy this node
    delete node;
}

// ------------------------------------------------------------------------
// Private function
// Determines the child type of the given node. A child's type indicates
// whether it is the left or right child of its parent, or doesn't have
// a parent at all (and is the root of the tree).
// ------------------------------------------------------------------------
int vsTreeMap::getChildType(vsTreeMapNode * node)
{
    vsTreeMapNode *parent = node->parent;
    
    // Figure out which child of its parent the node is
    if (parent == NULL)
        return VS_TREE_MAP_ROOTNODE;
    if (parent->leftChild == node)
        return VS_TREE_MAP_LEFTCHILD;
    if (parent->rightChild == node)
        return VS_TREE_MAP_RIGHTCHILD;

    // Error checking
    notify(AT_ERROR, "vsTreeMap::getChildType: Map Inconsistency: 'node' is not "
        "a child of its own parent!\n");
    return -1;
}

// ------------------------------------------------------------------------
// Private function
// Traverses the tree rooted at the given node, copying map entries into
// the given two lists
// ------------------------------------------------------------------------
void vsTreeMap::fillLists(vsTreeMapNode * node, atList * keyList, atList * valueList)
{
    // No work to do if there's no tree
    if (!node)
        return;

    // Inorder - traverse left child
    fillLists(node->leftChild, keyList, valueList);
    
    // Add the key and value to their respective lists
    if (keyList != NULL)
        keyList->addEntry(node->nodeKey);
    if (valueList != NULL)
        valueList->addEntry(node->nodeValue);
    
    // Inorder - traverse right child
    fillLists(node->rightChild, keyList, valueList);
}

void vsTreeMap::print()
{
    // Print a header for the map tree first
    notify(AT_INFO, "vsTreeMap %p (%lu entries):\n", this, treeSize);

    // Call the recursive printTree() method with the root of the red-black
    // tree and a zero indent
    if (treeRoot != NULL)
        printTree(treeRoot, 0);
}

void vsTreeMap::printTree(vsTreeMapNode *node, int indent)
{
    int i;
    char buf[512];
    int idx;
    atString * itemStr;

    // Print this node's information
    // Start with an opening brace
    idx = 0;
    for (i = 0; i < indent; i++)
        idx += sprintf(&buf[idx], " ");
    sprintf(&buf[idx], "{\n");

    notify(AT_INFO, buf);
    idx = 0;

    // Print the node's address
    for (i = 0; i < indent+2; i++)
        idx += sprintf(&buf[idx], " ");
    idx += sprintf(&buf[idx], "Node         %p\n", node);

    notify(AT_INFO, buf);
    idx = 0;

    // Print the node's color
    for (i = 0; i < indent+2; i++)
        idx += sprintf(&buf[idx], " ");
    switch(node->color)
    {
        case VS_TREE_MAP_BLACK:
            idx += sprintf(&buf[idx], "Color        %s\n", "BLACK");
            break;
        case VS_TREE_MAP_RED:
            idx += sprintf(&buf[idx], "Color        %s\n", "RED");
            break;
    };

    notify(AT_INFO, buf);
    idx = 0;

    // Print the node's key item pointer
    for (i = 0; i < indent+2; i++)
        idx += sprintf(&buf[idx], " ");
    idx += sprintf(&buf[idx], "Key          %p", node->nodeKey);

    // If the node key is an atString, append the string to the printout
    // for additional information
    if (itemStr = dynamic_cast<atString *>(node->nodeKey))
        idx += sprintf(&buf[idx], "  \"%s\"\n", itemStr->getString());
    else
        idx += sprintf(&buf[idx], "\n");

    notify(AT_INFO, buf);
    idx = 0;

    // Print the node's value item pointer
    for (i = 0; i < indent+2; i++)
        idx += sprintf(&buf[idx], " ");
    idx += sprintf(&buf[idx], "Value        %p", node->nodeValue);

    // If the node value is an atString, append the string to the printout
    // for additional information
    if (itemStr = dynamic_cast<atString *>(node->nodeValue))
        idx += sprintf(&buf[idx], "  \"%s\"\n", itemStr->getString());
    else
        idx += sprintf(&buf[idx], "\n");

    // Leave a blank line before printing the linkage information
    idx += sprintf(&buf[idx], "\n");

    notify(AT_INFO, buf);
    idx = 0;

    // Print the node's parent node pointer
    for (i = 0; i < indent+2; i++)
        idx += sprintf(&buf[idx], " ");
    idx += sprintf(&buf[idx], "Parent       %p\n", node->parent);

    notify(AT_INFO, buf);
    idx = 0;

    // Now, traverse and print the subtrees
    // First, the left child
    for (i = 0; i < indent+2; i++)
        idx += sprintf(&buf[idx], " ");
    if (node->leftChild != NULL)
    {
        idx += sprintf(&buf[idx], "Left Child:  %p\n", node->leftChild);
        printTree(node->leftChild, indent + 2);
    }
    else
        idx += sprintf(&buf[idx], "Left Child:  (none)\n");

    notify(AT_INFO, buf);
    idx = 0;

    // Then, the right child
    for (i = 0; i < indent+2; i++)
        idx += sprintf(&buf[idx], " ");
    if (node->rightChild != NULL)
    {
        idx += sprintf(&buf[idx], "Right Child: %p\n", node->rightChild);
        printTree(node->rightChild, indent + 2);
    }
    else
        idx += sprintf(&buf[idx], "Right Child: (none)\n");

    notify(AT_INFO, buf);
    idx = 0;

    // Finish with a closing brace
    for (i = 0; i < indent; i++)
        idx += sprintf(&buf[idx], " ");
    idx += sprintf(&buf[idx], "}\n");

    notify(AT_INFO, buf);
}

