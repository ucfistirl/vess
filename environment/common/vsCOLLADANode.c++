
#include "vsCOLLADANode.h++"

// ------------------------------------------------------------------------
// Constructs a COLLADA node, setting the various identifying strings and
// the node type
// ------------------------------------------------------------------------
vsCOLLADANode::vsCOLLADANode(atString id, atString name, atString sid,
                             vsCOLLADANodeType type)
{
    // Set the node's name (this is the name field of the vsComponent,
    // so we currently need to set this as a char *)
    setName(name.getString());

    // Set the ID and scoped ID
    nodeID = id.clone();
    nodeSID = sid.clone();

    // Set the node type (whether this is a regular node or a skeleton
    // joint)
    colladaNodeType = type;

    // Construct the transform list
    transformList = new atList();
}

// ------------------------------------------------------------------------
// Destructor, cleans up the list of transforms
// ------------------------------------------------------------------------
vsCOLLADANode::~vsCOLLADANode()
{
    // Clean up the transform list
    delete transformList;
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsCOLLADANode::getClassName()
{
    return "vsCOLLADANode";
}

// ------------------------------------------------------------------------
// Return the unique COLLADA identifier for this node
// ------------------------------------------------------------------------
atString vsCOLLADANode::getID()
{
    return nodeID.clone();
}

// ------------------------------------------------------------------------
// Return the COLLADA scoped identifier for this node.  Scoped ID's are
// used to allow multiple copies of node hierarchies to share a common
// structure.  For example a skin controller could use any one of a number
// of skeleton hierarchies, provided the skeletons each contain a set of
// joints with the same scoped ID's
// ------------------------------------------------------------------------
atString vsCOLLADANode::getSID()
{
    return nodeSID.clone();
}

// ------------------------------------------------------------------------
// Return the type of COLLADA node.  This can be either NODE for a regular
// node or JOINT for a skeleton joint.
// ------------------------------------------------------------------------
vsCOLLADANodeType vsCOLLADANode::getCOLLADANodeType()
{
    return colladaNodeType;
}

// ------------------------------------------------------------------------
// Searches this node and it's descendants for a node with the given ID
// ------------------------------------------------------------------------
vsCOLLADANode *vsCOLLADANode::findNodeByID(atString id)
{
    int i;
    vsCOLLADANode *colladaChild;
    vsCOLLADANode *result;

    // If our id matches the requested ID, return this node as the result
    if (getID().equals(&id))
        return this;

    // Otherwise, recurse on our children
    for (i = 0; i < getChildCount(); i++)
    {
        // Since this is a special search that works only for COLLADA nodes,
        // make sure this child is a COLLADA node before proceeding
        colladaChild = dynamic_cast<vsCOLLADANode *>(getChild(i));
        if (colladaChild != NULL)
        {
            // If the node we want is a descendant of this child, return it,
            // otherwise keep looking
            result = colladaChild->findNodeByID(id);
            if (result != NULL)
                return result;
        }
    }

    // We were unable to find the desired node among any of our descendants
    return NULL;
}

// ------------------------------------------------------------------------
// Searches this node and it's descendants for a node with the given scoped
// ID
// ------------------------------------------------------------------------
vsCOLLADANode *vsCOLLADANode::findNodeBySID(atString sid)
{
    int i;
    vsCOLLADANode *colladaChild;
    vsCOLLADANode *result;

    // If our id matches the requested ID, return this node as the result
    if (getSID().equals(&sid))
        return this;

    // Otherwise, recurse on our children
    for (i = 0; i < getChildCount(); i++)
    {
        // Since this is a special search that works only for COLLADA nodes,
        // make sure this child is a COLLADA node before proceeding
        colladaChild = dynamic_cast<vsCOLLADANode *>(getChild(i));
        if (colladaChild != NULL)
        {
            // If the node we want is a descendant of this child, return it,
            // otherwise keep looking
            result = colladaChild->findNodeBySID(sid);
            if (result != NULL)
                return result;
        }
    }

    // We were unable to find the desired node among any of our descendants
    return NULL;
}

// ------------------------------------------------------------------------
// Add a COLLADA transform to this node's transform list.  Normally, we
// would simply load all the transforms, convert them to matrices, and 
// combine them all together in a vsTransformAttribute.  However, since
// COLLADA can in theory animate any individual transform (or even any
// individual parameter inside a transform), we need to keep them distinct
// until we load the animations and construct the needed path motion
// objects
// ------------------------------------------------------------------------
void vsCOLLADANode::addTransform(vsCOLLADATransform *xform)
{
    // Add the transform to our list
    if (xform != NULL)
        transformList->addEntry(xform);
}

// ------------------------------------------------------------------------
// Fetches the transform with the given scoped identifier (this is needed
// when loading animations)
// ------------------------------------------------------------------------
vsCOLLADATransform *vsCOLLADANode::getTransform(atString sid)
{
    vsCOLLADATransform *xform;

    // If no SID is given, return NULL (there may be transforms with
    // empty SID's in our list, but we don't consider these searchable)
    if (sid.getLength() == 0)
        return NULL;

    // Search the transform list for the transform with the given SID
    xform = (vsCOLLADATransform *)transformList->getFirstEntry();
    while ((xform != NULL) && (!xform->getSID().equals(&sid)))
        xform = (vsCOLLADATransform *)transformList->getNextEntry();
    
    // Return the transform that we found (or NULL if we didn't find one)
    return xform;
}

// ------------------------------------------------------------------------
// Fetches the first transform in our list of transforms
// ------------------------------------------------------------------------
vsCOLLADATransform *vsCOLLADANode::getFirstTransform()
{
    return (vsCOLLADATransform *)transformList->getFirstEntry();
}

// ------------------------------------------------------------------------
// Fetches the next transform in our list of transforms
// ------------------------------------------------------------------------
vsCOLLADATransform *vsCOLLADANode::getNextTransform()
{
    return (vsCOLLADATransform *)transformList->getNextEntry();
}

// ------------------------------------------------------------------------
// Concatenates all of our transforms together into a single matrix, and
// returns it
// ------------------------------------------------------------------------
atMatrix vsCOLLADANode::getCombinedTransform()
{
    atMatrix result;
    vsCOLLADATransform *xform;

    // Start with an identity matrix
    result.setIdentity();

    // Iterate over the transforms in our list and concantenate the
    // resulting matrices together
    xform = (vsCOLLADATransform *)transformList->getFirstEntry();
    while (xform != NULL)
    {
        // Post-multiply this transform's matrix with our current
        // transform
        result = result * xform->getMatrix();

        // Move on to the next transform
        xform = (vsCOLLADATransform *)transformList->getNextEntry();
    }

    // Return the result
    return result;
}

// ------------------------------------------------------------------------
// Concatenates the translation component of all of our transforms together
// into a single position vector and returns it
// ------------------------------------------------------------------------
atVector vsCOLLADANode::getCombinedPosition()
{
    atVector result;
    vsCOLLADATransform *xform;

    // Start with an identity matrix
    result.setSize(3);
    result.clear();

    // Iterate over the transforms in our list and concantenate the
    // resulting matrices together
    xform = (vsCOLLADATransform *)transformList->getFirstEntry();
    while (xform != NULL)
    {
        // Add this transform's translation component to our vector
        result = result + xform->getPosition();

        // Move on to the next transform
        xform = (vsCOLLADATransform *)transformList->getNextEntry();
    }

    // Return the result
    return result;
}

// ------------------------------------------------------------------------
// Concatenates the rotation component of all of our transforms together
// into a single quaternion and returns it
// ------------------------------------------------------------------------
atQuat vsCOLLADANode::getCombinedOrientation()
{
    atQuat result;
    vsCOLLADATransform *xform;

    // Start with an identity quaternion
    result.set(0.0, 0.0, 0.0, 1.0);

    // Iterate over the transforms in our list and concantenate the
    // resulting quaternions together
    xform = (vsCOLLADATransform *)transformList->getFirstEntry();
    while (xform != NULL)
    {
        // Post-multiply this transform's orienation with our current
        // orientation
        result = result * xform->getOrientation();

        // Move on to the next transform
        xform = (vsCOLLADATransform *)transformList->getNextEntry();
    }

    // Return the result
    return result;
}

