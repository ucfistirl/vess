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
//    VESS Module:  vsTreeMap.c++
//
//    Description:  Utility class that implements a mapping from key
//                  values to data values, stored using a red-black
//                  tree algorithm
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include "vsGlobals.h++"

#include "vsTreeMap.h++"

// ------------------------------------------------------------------------
// Constructor - Sets the tree to empty
// ------------------------------------------------------------------------
vsTreeMap::vsTreeMap()
{
    treeRoot = NULL;
    treeSize = 0;
}

// ------------------------------------------------------------------------
// Destructor - Deletes the contents of the tree
// ------------------------------------------------------------------------
vsTreeMap::~vsTreeMap()
{
    clear();
}

// ------------------------------------------------------------------------
// Adds a new mapping from key to value to the tree. Returns true if
// successful, or false if a mapping for that key already exists.
// ------------------------------------------------------------------------
int vsTreeMap::addEntry(void *key, void *value)
{
    // Make sure that a node with the given key isn't already in
    // the tree.
    if (containsKey(key))
        return VS_FALSE;

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
        treeRoot = newNode;
        treeRoot->color = VS_TREE_MAP_BLACK;
	treeSize++;
        return VS_TRUE;
    }
    
    // The tree isn't empty. Do a binary search on the tree to determine
    // the correct location for the new node.
    vsTreeMapNode *nodeParent = treeRoot;
    while (1)
    {
        if (newNode->nodeKey < nodeParent->nodeKey)
        {
            // Left subtree
            if (nodeParent->leftChild == NULL)
            {
                nodeParent->leftChild = newNode;
                newNode->parent = nodeParent;
                break;
            }
            else
                nodeParent = nodeParent->leftChild;
        }
        else
        {
            // Right subtree
            if (nodeParent->rightChild == NULL)
            {
                nodeParent->rightChild = newNode;
                newNode->parent = nodeParent;
                break;
            }
            else
                nodeParent = nodeParent->rightChild;
        }
    }
    
    // Clean up the tree after the insertion
    rebalanceInsert(newNode);
    treeRoot->color = VS_TREE_MAP_BLACK;

    treeSize++;
    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Removes the mapping associated with the given key from the tree. Returns
// true if successful, or false if the key is not in the tree.
// ------------------------------------------------------------------------
int vsTreeMap::deleteEntry(void *key)
{
    vsTreeMapNode *targetNode;
    
    // Find the node in the tree with the given key. Abort if there
    // is no such node.
    targetNode = findNode(treeRoot, key);
    if (targetNode == NULL)
        return VS_FALSE;

    // Call an internal function to do the actual deletion
    deleteNode(targetNode);

    // The last part of cleaning up the tree, which is the only part that
    // deleteNode() doesn't do by itself, is forcing the root node to be
    // black.
    if (treeRoot)
        treeRoot->color = VS_TREE_MAP_BLACK;

    treeSize--;
    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Returns the number of mappings contained in this tree
// ------------------------------------------------------------------------
int vsTreeMap::getEntryCount()
{
    return treeSize;
}

// ------------------------------------------------------------------------
// Checks if a mapping for the given key is present in the tree. Returns
// true if so, false if not.
// ------------------------------------------------------------------------
int vsTreeMap::containsKey(void *key)
{
    if (findNode(treeRoot, key) != NULL)
	return VS_TRUE;
    else
	return VS_FALSE;
}

// ------------------------------------------------------------------------
// Returns the value associated with the given key, or NULL if that key is
// not present within the tree.
// ------------------------------------------------------------------------
void *vsTreeMap::getValue(void *key)
{
    vsTreeMapNode *node;
    
    node = findNode(treeRoot, key);
    if (node)
        return (node->nodeValue);

    return NULL;
}

// ------------------------------------------------------------------------
// Attempts to change the value associated with the given key to newValue.
// Return true if successful, false if the given key is not present within
// the tree.
// ------------------------------------------------------------------------
int vsTreeMap::changeValue(void *key, void *newValue)
{
    vsTreeMapNode *node;
    
    node = findNode(treeRoot, key);
    if (!node)
        return VS_FALSE;

    node->nodeValue = newValue;
    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Removes all mappings from the tree
// ------------------------------------------------------------------------
void vsTreeMap::clear()
{
    if (treeRoot == NULL)
        return;

    // The deleteTree function does all of the actual work
    deleteTree(treeRoot);
    
    treeRoot = NULL;
    treeSize = 0;
}

// ------------------------------------------------------------------------
// Private function
// Searches the subtree rooted at 'node' for a node with the given key.
// Returns that node, or NULL if it can't find it.
// ------------------------------------------------------------------------
vsTreeMapNode *vsTreeMap::findNode(vsTreeMapNode *node, void *key)
{
    if (node == NULL)
        return NULL;

    if (node->nodeKey == key)
        return node;

    if (key > node->nodeKey)
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
void vsTreeMap::rebalanceInsert(vsTreeMapNode *node)
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
void vsTreeMap::rebalanceDelete(vsTreeMapNode *parent, int deletedChildType)
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

	// If sibling's left child is red, then manipulate the
	// tree so that only the right child is red. This can
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
// Deletes the specified node from the tree, calling the function to
// restore the tree balance afterwards if needed.
// ------------------------------------------------------------------------
void vsTreeMap::deleteNode(vsTreeMapNode *node)
{
    int childType = getChildType(node);
    vsTreeMapNode *parent = node->parent;
    vsTreeMapNode *child;

    if ((node->leftChild == NULL) && (node->rightChild == NULL))
    {
	// Case 1: node to delete has no children
	// Remove the node and rebalance
    
        if (childType == VS_TREE_MAP_LEFTCHILD)
            parent->leftChild = NULL;
        else if (childType == VS_TREE_MAP_RIGHTCHILD)
            parent->rightChild = NULL;
        else
            treeRoot = NULL;
        
        if (node->color == VS_TREE_MAP_BLACK)
            rebalanceDelete(parent, childType);
        
        delete node;
    }
    else if ((node->leftChild == NULL) || (node->rightChild == NULL))
    {
	// Case 2: node to delete has one child
	// Move the child node into the location that the node to
	// be deleted is in, and rebalance

        if (node->leftChild)
            child = node->leftChild;
        else
            child = node->rightChild;

        child->parent = parent;

        if (childType == VS_TREE_MAP_LEFTCHILD)
            parent->leftChild = child;
        else if (childType == VS_TREE_MAP_RIGHTCHILD)
            parent->rightChild = child;
        else
            treeRoot = child;
        
        if (node->color == VS_TREE_MAP_BLACK)
            rebalanceDelete(parent, childType);

        delete node;
    }
    else
    {
	// Case 3: node to delete has two children
	// Rather than deleting the node, instead find the node with
	// the next-higher key value, transplant that value into the
	// node that would have been deleted, and delete that other node.

        child = getInorderSuccessor(node);
        
        node->nodeKey = child->nodeKey;
        node->nodeValue = child->nodeValue;
        
        deleteNode(child);
    }
}

// ------------------------------------------------------------------------
// Private function
// Searches the tree for the node with the next-higher key than the given
// node's key. Returns NULL if no such node exists.
// ------------------------------------------------------------------------
vsTreeMapNode *vsTreeMap::getInorderSuccessor(vsTreeMapNode *node)
{
    // If there is no node with a greater key, abort.
    if (node->rightChild == NULL)
        return NULL;

    // The node with the next highest key must be the node with the
    // smallest key in the original node's right subtree. 
    vsTreeMapNode *result = node->rightChild;
    while (result->leftChild)
        result = result->leftChild;

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
void vsTreeMap::rotateLeft(vsTreeMapNode *node)
{
    vsTreeMapNode *left, *right, *child, *parent;
    int childType;
    
    if (node->rightChild == NULL)
    {
        printf("vsTreeMap::rotateLeft: Can't rotate left on a node with no "
            "right child\n");
        return;
    }
    
    left = node;
    right = left->rightChild;
    child = right->leftChild;
    parent = left->parent;
    
    childType = getChildType(node);
    
    left->rightChild = child;
    left->parent = right;
    right->leftChild = left;
    right->parent = parent;
    if (child)
        child->parent = left;
    
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
void vsTreeMap::rotateRight(vsTreeMapNode *node)
{
    vsTreeMapNode *left, *right, *child, *parent;
    int childType;
    
    if (node->leftChild == NULL)
    {
        printf("vsTreeMap::rotateRight: Can't rotate right on a node with "
            "no left child\n");
        return;
    }
    
    right = node;
    left = right->leftChild;
    child = left->rightChild;
    parent = right->parent;
    
    childType = getChildType(node);

    right->leftChild = child;
    right->parent = left;
    left->rightChild = right;
    left->parent = parent;
    if (child)
        child->parent = right;
    
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
// of the tree afterwards. Called by the clear() method.
// ------------------------------------------------------------------------
void vsTreeMap::deleteTree(vsTreeMapNode *node)
{
    if (node == NULL)
        return;

    deleteTree(node->leftChild);
    deleteTree(node->rightChild);
    
    delete node;
}

// ------------------------------------------------------------------------
// Private function
// Determines the child type of the given node. A child's type indicates
// whether it is the left or right child of its parent, or doesn't have
// a parent at all (and is the root of the tree).
// ------------------------------------------------------------------------
int vsTreeMap::getChildType(vsTreeMapNode *node)
{
    vsTreeMapNode *parent = node->parent;
    
    if (parent == NULL)
        return VS_TREE_MAP_ROOTNODE;
    if (parent->leftChild == node)
        return VS_TREE_MAP_LEFTCHILD;
    if (parent->rightChild == node)
        return VS_TREE_MAP_RIGHTCHILD;

    printf("vsTreeMap::getChildType: Tree Inconsistency: 'node' is not "
        "a child of its own parent!\n");
    return -1;
}
