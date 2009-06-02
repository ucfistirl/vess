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
//                  during an IntersectionVisitor traversal
//
//    Author(s):    Jason Daly, Casey Thurston
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
// Updates the traversal mode for a Node. This method is necessary so that
// the traversal mode for the Node is correct when the intersection
// visitor code is executed.
// ------------------------------------------------------------------------
void vsIntersectTraverser::apply(osg::Node &node)
{
    TraversalMode previousMode;

    // Use the default traversal mode.
    previousMode = updateTraversalMode(VS_INTERSECT_DEFAULT_TRAV_MODE);

    // Perform the IntersectionVisitor traversal using the new mode.
    osgUtil::IntersectionVisitor::apply(node);

    // Reapply the previous mode.
    updateTraversalMode(previousMode);
}


// ------------------------------------------------------------------------
// Internal function (called by OSG)
// Updates the traversal mode for a Geode. This method is necessary so that
// the traversal mode for the Geode is correct when the intersection
// visitor code is executed.
// ------------------------------------------------------------------------
void vsIntersectTraverser::apply(osg::Geode &node)
{
    TraversalMode previousMode;

    // Use the default traversal mode.
    previousMode = updateTraversalMode(VS_INTERSECT_DEFAULT_TRAV_MODE);

    // Perform the IntersectionVisitor traversal using the new mode.
    osgUtil::IntersectionVisitor::apply(node);

    // Reapply the previous mode.
    updateTraversalMode(previousMode);
}


// ------------------------------------------------------------------------
// Internal function (called by OSG)
// Updates the traversal mode for a Billboard. This method is necessary so
// that the traversal mode for the Billboard is correct when the
// intersection visitor code is executed.
// ------------------------------------------------------------------------
void vsIntersectTraverser::apply(osg::Billboard &node)
{
    TraversalMode previousMode;

    // Use the default traversal mode.
    previousMode = updateTraversalMode(VS_INTERSECT_DEFAULT_TRAV_MODE);

    // Perform the IntersectionVisitor traversal using the new mode.
    osgUtil::IntersectionVisitor::apply(node);

    // Reapply the previous mode.
    updateTraversalMode(previousMode);
}


// ------------------------------------------------------------------------
// Internal function (called by OSG)
// Updates the traversal mode for a Group. This method is necessary so that
// the traversal mode for the Group is correct when the intersection
// visitor code is executed.
// ------------------------------------------------------------------------
void vsIntersectTraverser::apply(osg::Group &node)
{
    TraversalMode previousMode;

    // Use the default traversal mode.
    previousMode = updateTraversalMode(VS_INTERSECT_DEFAULT_TRAV_MODE);

    // Perform the IntersectionVisitor traversal using the new mode.
    osgUtil::IntersectionVisitor::apply(node);

    // Reapply the previous mode.
    updateTraversalMode(previousMode);
}


// ------------------------------------------------------------------------
// Internal function (called by OSG)
// ------------------------------------------------------------------------
void vsIntersectTraverser::apply(osgSim::MultiSwitch &node)
{
    TraversalMode previousMode;

    // Store the current traversal mode.
    switch (sequenceTravMode)
    {
        case VS_INTERSECT_SWITCH_NONE:
        {
            // This corresponds to TRAVERSE_NONE.
            previousMode = updateTraversalMode(TRAVERSE_NONE);
            break;
        }

        case VS_INTERSECT_SWITCH_CURRENT:
        {
            // This corresponds to TRAVERSE_ACTIVE_CHILDREN.
            previousMode = updateTraversalMode(TRAVERSE_ACTIVE_CHILDREN);
            break;
        }

        case VS_INTERSECT_SWITCH_ALL:
        {
            // This corresponds to TRAVERSE_ALL_CHILDREN.
            previousMode = updateTraversalMode(TRAVERSE_ALL_CHILDREN);
            break;
        }
    }

    // Perform the IntersectionVisitor traversal using the new mode.
    osgUtil::IntersectionVisitor::apply(node);

    // Reapply the previous mode.
    updateTraversalMode(previousMode);
}


// ------------------------------------------------------------------------
// Internal function (called by OSG)
// Updates the traversal mode for a Transform. This method is necessary so
// that the traversal mode for the Transform is correct when the
// intersection visitor code is executed.
// ------------------------------------------------------------------------
void vsIntersectTraverser::apply(osg::Transform &node)
{
    TraversalMode previousMode;

    // Use the default traversal mode.
    previousMode = updateTraversalMode(VS_INTERSECT_DEFAULT_TRAV_MODE);

    // Perform the IntersectionVisitor traversal using the new mode.
    osgUtil::IntersectionVisitor::apply(node);

    // Reapply the previous mode.
    updateTraversalMode(previousMode);
}


// ------------------------------------------------------------------------
// Internal function (called by OSG)
// Updates the traversal mode for a Projection. This method is necessary so
// that the traversal mode for the Projection is correct when the
// intersection visitor code is executed.
// ------------------------------------------------------------------------
void vsIntersectTraverser::apply(osg::Projection &node)
{
    TraversalMode previousMode;

    // Use the default traversal mode.
    previousMode = updateTraversalMode(VS_INTERSECT_DEFAULT_TRAV_MODE);

    // Perform the IntersectionVisitor traversal using the new mode.
    osgUtil::IntersectionVisitor::apply(node);

    // Reapply the previous mode.
    updateTraversalMode(previousMode);
}


// ------------------------------------------------------------------------
// Internal function (called by OSG)
// Updates the traversal mode for a Camera. This method is necessary so
// that the traversal mode for the Camera is correct when the intersection
// visitor code is executed.
// ------------------------------------------------------------------------
void vsIntersectTraverser::apply(osg::Camera &node)
{
    TraversalMode previousMode;

    // Use the default traversal mode.
    previousMode = updateTraversalMode(VS_INTERSECT_DEFAULT_TRAV_MODE);

    // Perform the IntersectionVisitor traversal using the new mode.
    osgUtil::IntersectionVisitor::apply(node);

    // Reapply the previous mode.
    updateTraversalMode(previousMode);
}


// ------------------------------------------------------------------------
// Internal function (called by OSG)
// Updates the traversal mode for a LOD. This method is necessary because
// the traversal mode for LOD nodes is configurable.
// ------------------------------------------------------------------------
void vsIntersectTraverser::apply(osg::LOD &node)
{
    TraversalMode previousMode;

    // Store the current traversal mode.
    switch (lodTravMode)
    {
        case VS_INTERSECT_LOD_NONE:
        {
            // This corresponds to TRAVERSE_NONE.
            previousMode = updateTraversalMode(TRAVERSE_NONE);
            break;
        }

        // TODO: LOD_FIRST needs to be custom implemented. Use CURRENT for now.
        case VS_INTERSECT_LOD_FIRST:
        case VS_INTERSECT_LOD_CURRENT:
        {
            // This corresponds to TRAVERSE_ACTIVE_CHILDREN.
            previousMode = updateTraversalMode(TRAVERSE_ACTIVE_CHILDREN);
            break;
        }

        case VS_INTERSECT_LOD_ALL:
        {
            // This corresponds to TRAVERSE_ALL_CHILDREN.
            previousMode = updateTraversalMode(TRAVERSE_ALL_CHILDREN);
            break;
        }
    }

    // Perform the IntersectionVisitor traversal using the new mode.
    osgUtil::IntersectionVisitor::apply(node);

    // Reapply the previous mode.
    updateTraversalMode(previousMode);
}


// ------------------------------------------------------------------------
// Internal function (called by OSG)
// Updates the traversal mode for a PagedLOD. This method is necessary
// because the traversal mode for PagedLOD nodes is configurable.
// ------------------------------------------------------------------------
void vsIntersectTraverser::apply(osg::PagedLOD &node)
{
    TraversalMode previousMode;

    // Store the current traversal mode.
    switch (lodTravMode)
    {
        case VS_INTERSECT_LOD_NONE:
        {
            // This corresponds to TRAVERSE_NONE.
            previousMode = updateTraversalMode(TRAVERSE_NONE);
            break;
        }

        // TODO: LOD_FIRST needs to be custom implemented. Use CURRENT for now.
        case VS_INTERSECT_LOD_FIRST:
        case VS_INTERSECT_LOD_CURRENT:
        {
            // This corresponds to TRAVERSE_ACTIVE_CHILDREN.
            previousMode = updateTraversalMode(TRAVERSE_ACTIVE_CHILDREN);
            break;
        }

        case VS_INTERSECT_LOD_ALL:
        {
            // This corresponds to TRAVERSE_ALL_CHILDREN.
            previousMode = updateTraversalMode(TRAVERSE_ALL_CHILDREN);
            break;
        }
    }

    // Perform the IntersectionVisitor traversal using the new mode.
    osgUtil::IntersectionVisitor::apply(node);

    // Reapply the previous mode.
    updateTraversalMode(previousMode);
}


// ------------------------------------------------------------------------
// Internal function (called by OSG)
// Updates the traversal mode for a Sequence. This method is necessary
// because the traversal mode for Sequence nodes is configurable.
// ------------------------------------------------------------------------
void vsIntersectTraverser::apply(osg::Sequence &node)
{
    TraversalMode previousMode;

    // Store the current traversal mode.
    switch (sequenceTravMode)
    {
        case VS_INTERSECT_SEQUENCE_NONE:
        {
            // This corresponds to TRAVERSE_NONE.
            previousMode = updateTraversalMode(TRAVERSE_NONE);
            break;
        }

        case VS_INTERSECT_SEQUENCE_CURRENT:
        {
            // This corresponds to TRAVERSE_ACTIVE_CHILDREN.
            previousMode = updateTraversalMode(TRAVERSE_ACTIVE_CHILDREN);
            break;
        }

        case VS_INTERSECT_SEQUENCE_ALL:
        {
            // This corresponds to TRAVERSE_ALL_CHILDREN.
            previousMode = updateTraversalMode(TRAVERSE_ALL_CHILDREN);
            break;
        }
    }

    // Perform the IntersectionVisitor traversal using the new mode.
    osgUtil::IntersectionVisitor::apply(node);

    // Reapply the previous mode.
    updateTraversalMode(previousMode);
}


// ------------------------------------------------------------------------
// Internal function (called by OSG)
// Updates the traversal mode for a Switch. This method is necessary
// because the traversal mode for Switch nodes is configurable.
// ------------------------------------------------------------------------
void vsIntersectTraverser::apply(osg::Switch &node)
{
    TraversalMode previousMode;

    // Store the current traversal mode.
    switch (sequenceTravMode)
    {
        case VS_INTERSECT_SWITCH_NONE:
        {
            // This corresponds to TRAVERSE_NONE.
            previousMode = updateTraversalMode(TRAVERSE_NONE);
            break;
        }

        case VS_INTERSECT_SWITCH_CURRENT:
        {
            // This corresponds to TRAVERSE_ACTIVE_CHILDREN.
            previousMode = updateTraversalMode(TRAVERSE_ACTIVE_CHILDREN);
            break;
        }

        case VS_INTERSECT_SWITCH_ALL:
        {
            // This corresponds to TRAVERSE_ALL_CHILDREN.
            previousMode = updateTraversalMode(TRAVERSE_ALL_CHILDREN);
            break;
        }
    }

    // Perform the IntersectionVisitor traversal using the new mode.
    osgUtil::IntersectionVisitor::apply(node);

    // Reapply the previous mode.
    updateTraversalMode(previousMode);
}


// ------------------------------------------------------------------------
// Private function
// Sets the NodeVisitor traversal mode. This method returns the previous
// mode so it may be restored later.
// ------------------------------------------------------------------------
osg::NodeVisitor::TraversalMode vsIntersectTraverser::updateTraversalMode(
    osg::NodeVisitor::TraversalMode mode)
{
    TraversalMode previousMode;

    // Store the current traversal mode.
    previousMode = osg::NodeVisitor::getTraversalMode();

    // Perform the update.
    osg::NodeVisitor::setTraversalMode(mode);

    // Return the previous mode.
    return previousMode;
}

