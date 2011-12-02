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
//    VESS Module:  vsOSGNode.c++
//
//    Description:  vsObject wrapper for osg::Node objects (and
//                  descendants)
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsOSGNode.h++"

// ------------------------------------------------------------------------
// Constructor.  Sets up the vsObject wrapper and references the OSG Node
// ------------------------------------------------------------------------
vsOSGNode::vsOSGNode(osg::Node *theNode)
{
    // Store and reference the OSG node
    osgNode = theNode;
    osgNode->ref();
}

// ------------------------------------------------------------------------
// Destructor.  Unreferences the wrapped OSG Node
// ------------------------------------------------------------------------
vsOSGNode::~vsOSGNode()
{
    // Unreference the OSG node
    osgNode->unref();
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsOSGNode::getClassName()
{
    return "vsOSGNode";
}

// ------------------------------------------------------------------------
// Return the OSG node instance
// ------------------------------------------------------------------------
osg::Node *vsOSGNode::getNode()
{
    return osgNode;
}

// ------------------------------------------------------------------------
// See if this OSG Node is the same as the given one
// ------------------------------------------------------------------------
bool vsOSGNode::equals(atItem *otherItem)
{
    vsOSGNode *otherNode;

    // First, make sure the other item is an OSG Node object
    otherNode = dynamic_cast<vsOSGNode *>(otherItem);
    if (otherNode == NULL)
    {
        // Can't be equivalent
        return false;
    }
    else
    {
        // We're interested in the wrapped OSG Node objects, and not their
        // VESS wrappers, so compare the addresses of the OSG Nodes.
        // We need to cast these to the intptr type to prevent a number of
        // potential pitfalls regarding address comparisons and overloaded
        // operators on our wrapped objects.
        if (((intptr_t)this->getNode()) == ((intptr_t)otherNode->getNode()))
            return true;
    }

    // If we get this far, they're not equivalent
    return false;
}

// ------------------------------------------------------------------------
// Compare this vsOSGNode to the given one.  In this case, this means
// comparing the addresses of the wrapped OSG Nodes
// ------------------------------------------------------------------------
int vsOSGNode::compare(atItem *otherItem)
{
    vsOSGNode *otherNode;

    // First, make sure the other item is an OSG Node object
    otherNode = dynamic_cast<vsOSGNode *>(otherItem);
    if (otherNode == NULL)
    {
        // Not comparable as vsOSGNodes.  Use the parent class method to
        // compare them
        return vsObject::compare(otherItem);
    }
    else
    {
        // We're interested in the wrapped OSG Node objects, and not their
        // VESS wrappers, so compare the addresses of the OSG Node objects.
        // We need to cast these to the intptr type to prevent a number of
        // potential pitfalls regarding address comparisons and overloaded
        // operators on our wrapped objects.
        return (((intptr_t)otherNode->getNode()) - ((intptr_t)this->getNode()));
    }
}

