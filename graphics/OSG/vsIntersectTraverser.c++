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
//    VESS Module:  vsIntersectTraverser
//
//    Description:  OSG NodeVisitor traversal visitor to control how
//                  switches, sequences, and LOD nodes are traversed
//                  during an IntersectVisitor traversal
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsIntersectTraverser.h++"

// ------------------------------------------------------------------------
// Internal function
// Constructor.  Initializes the NodeVisitor
// ------------------------------------------------------------------------
vsIntersectTraverser::vsIntersectTraverser()
{
    // Except for switches, sequences, and LOD nodes, we want to traverse
    // all children of a node.
    _traversalMode = osg::NodeVisitor::TRAVERSE_ALL_CHILDREN;

    // Default all traversal modes to CURRENT or FIRST
    sequenceTravMode = VS_INTERSECT_SEQUENCE_CURRENT;
    switchTravMode = VS_INTERSECT_SWITCH_CURRENT;
    lodTravMode = VS_INTERSECT_LOD_FIRST;
}

// ------------------------------------------------------------------------ 
// Internal function
// Destructor.  Does nothing
// ------------------------------------------------------------------------
vsIntersectTraverser::~vsIntersectTraverser()
{
}

// ------------------------------------------------------------------------
// Internal function
// Sets the traversal mode for sequences
// ------------------------------------------------------------------------
void vsIntersectTraverser::setSequenceTravMode(int newMode)
{
    sequenceTravMode = newMode;
}

// ------------------------------------------------------------------------
// Internal function
// Gets the traversal mode for sequences
// ------------------------------------------------------------------------
int vsIntersectTraverser::getSequenceTravMode()
{
    return sequenceTravMode;
}

// ------------------------------------------------------------------------
// Internal function
// Sets the traversal mode for switches
// ------------------------------------------------------------------------
void vsIntersectTraverser::setSwitchTravMode(int newMode)
{
    switchTravMode = newMode;
}

// ------------------------------------------------------------------------
// Internal function
// Gets the traversal mode for switches
// ------------------------------------------------------------------------
int vsIntersectTraverser::getSwitchTravMode()
{
    return switchTravMode;
}

// ------------------------------------------------------------------------
// Internal function
// Sets the traversal mode for LOD's
// ------------------------------------------------------------------------
void vsIntersectTraverser::setLODTravMode(int newMode)
{
    lodTravMode = newMode;
}

// ------------------------------------------------------------------------
// Internal function
// Gets the traversal mode for LOD's
// ------------------------------------------------------------------------
int vsIntersectTraverser::getLODTravMode()
{
    return lodTravMode;
}

// ------------------------------------------------------------------------
// Internal function (called by OSG)
// Apply this traverser to an osg::Sequence
// ------------------------------------------------------------------------
void vsIntersectTraverser::apply(osg::Sequence &node)
{
    int i;

    // If sequence traversals are set to NONE, just return immediately
    // so no nodes are traversed
    if (sequenceTravMode == VS_INTERSECT_SEQUENCE_NONE)
        return;

    // Otherwise, iterate over the Sequence's children
    for (i = 0; i < (int)node.getNumChildren(); i++)
    {
        // See if we're set to only traverse the current frame
        if (sequenceTravMode == VS_INTERSECT_SEQUENCE_CURRENT)
        {
            // Check to see if this child is active, and apply the
            // IntersectVisitor to it if so
            if (node.getValue() == i)
            {
                node.getChild(i)->accept(*this);
            }
        }
        else
        {
            // We're traversing all frames, apply the IntersectVisitor to
            // the current frame with no check
            node.getChild(i)->accept(*this);
        }
    }
}

// ------------------------------------------------------------------------
// Internal function (called by OSG)
// Apply this traverser to an osg::Switch
// ------------------------------------------------------------------------
void vsIntersectTraverser::apply(osg::Switch &node)
{
    int i;

    // If switch traversals are set to NONE, just return immediately
    // so no nodes are traversed
    if (switchTravMode == VS_INTERSECT_SWITCH_NONE)
        return;

    // Otherwise, iterate over the Switch's children
    for (i = 0; i < (int)node.getNumChildren(); i++)
    {
        // See if we're set to only traverse active children
        if (switchTravMode == VS_INTERSECT_SWITCH_CURRENT)
        {
            // Check to see if this child is active, and apply the
            // IntersectVisitor to it if so
            if (node.getValue(i))
            {
                node.getChild(i)->accept(*this);
            }
        }
        else
        {
            // We're traversing all children, apply the IntersectVisitor to
            // the current child with no check
            node.getChild(i)->accept(*this);
        }
    }
}

// ------------------------------------------------------------------------
// Internal function (called by OSG)
// Apply this traverser to an osg::Node
// ------------------------------------------------------------------------
void vsIntersectTraverser::apply(osg::LOD &node)
{
    int i;

    // If LOD traversals are set to NONE, just return immediately
    // so no nodes are traversed
    if (lodTravMode == VS_INTERSECT_LOD_NONE)
        return;

    // If LOD traversals are set to FIRST, only traverse the LOD's first
    // child
    if (lodTravMode == VS_INTERSECT_LOD_FIRST)
    {
        node.getChild(0)->accept(*this);
        return;
    }

    // Otherwise, iterate over the LOD's children
    for (i = 0; i < (int)node.getNumChildren(); i++)
    {
        // We're traversing all children, apply the IntersectVisitor to
        // the current child with no check
        node.getChild(i)->accept(*this);
    }
}
