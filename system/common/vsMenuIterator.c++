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
//    VESS Module:  vsMenuIterator.c++
//
//    Description:  This object represents a traversal of all of the
//                  children of a given node in a vsMenuTree structure.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsMenuIterator.h++"

// ------------------------------------------------------------------------
// Constructor - The constructor creates an iterator over all of the
// children of the node in the vsMenuTree specified by the vsMenuFrame.
// ------------------------------------------------------------------------
vsMenuIterator::vsMenuIterator(vsMenuTree *tree, vsMenuFrame *frame)
{
    // Store the parent node
    parentNode = tree->getNode(frame);

    // Assign the current node to the first child of the parent
    currentNode = parentNode->child;
}

// ------------------------------------------------------------------------
// Destructor - The destructor does nothing
// ------------------------------------------------------------------------
vsMenuIterator::~vsMenuIterator()
{
}

// ------------------------------------------------------------------------
// This method advances the current node to the next sibling in the list
// ------------------------------------------------------------------------
void vsMenuIterator::advance()
{
    if (currentNode)
        currentNode = currentNode->sibling;
}

// ------------------------------------------------------------------------
// This method advances the current node to the next sibling in the list
// ------------------------------------------------------------------------
void vsMenuIterator::reset()
{
    currentNode = parentNode->child;
}

// ------------------------------------------------------------------------
// This method counts the nodes in the current child iteration
// ------------------------------------------------------------------------
int vsMenuIterator::getLength()
{
    vsMenuTreeNode *node;
    int returnValue = 0;

    // Start at the first child
    node = parentNode->child;

    // Run through every node and count it
    while (node)
    {
        returnValue++;
        node = node->sibling;
    }

    return returnValue;
}

// ------------------------------------------------------------------------
// This method returns the object at the position currently indicated by
// this iterator, or NULL if the iterator is invalid
// ------------------------------------------------------------------------
vsMenuObject *vsMenuIterator::getObject()
{
    if (currentNode)
        return currentNode->object;

    return NULL;
}

