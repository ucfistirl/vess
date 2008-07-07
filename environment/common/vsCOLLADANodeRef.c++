
#include "vsCOLLADANodeRef.h++"

// ------------------------------------------------------------------------
// Constructs a COLLADA node reference, saving the node pointer and
// incrementing its reference count
// ------------------------------------------------------------------------
vsCOLLADANodeRef::vsCOLLADANodeRef(vsCOLLADANode *node)
{
    // Save the node and reference it
    colladaNode = node;
    colladaNode->ref();
}

// ------------------------------------------------------------------------
// Destructor, unreferences the node (doesn't delete it as this object is
// only a reference to the real one)
// ------------------------------------------------------------------------
vsCOLLADANodeRef::~vsCOLLADANodeRef()
{
    // Unreference the node
    colladaNode->unref();
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsCOLLADANodeRef::getClassName()
{
    return "vsCOLLADANodeRef";
}

// ------------------------------------------------------------------------
// Return the node we're referencing
// ------------------------------------------------------------------------
vsCOLLADANode *vsCOLLADANodeRef::getNode()
{
    return colladaNode;
}

