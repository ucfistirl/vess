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
//    VESS Module:  vsViewpointAttribute.c++
//
//    Description:  Attribute that binds a vsView object to a certain
//                  node in the scene. The vsView is automatically updated
//                  with the transform affecting the node every frame.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsViewpointAttribute.h++"

// Map from vsViewpointAttribute objects to vsView objects
vsObjectMap *vsViewpointAttribute::viewObjectMap = NULL;

// ------------------------------------------------------------------------
// Constructor - Initializes the attribute's adjustment matrix
// ------------------------------------------------------------------------
vsViewpointAttribute::vsViewpointAttribute()
{
    // Start with no view object attached
    viewObject = NULL;

    // Initialize the viewpoint offset matrix   
    offsetMatrix.setIdentity();

    // Not attached to a component yet
    parentComponent = NULL;
}

// ------------------------------------------------------------------------
// Constructor - Registers this attribute with the specified view object,
// and initializes the adjustment matrix.
// ------------------------------------------------------------------------
vsViewpointAttribute::vsViewpointAttribute(vsView *theView)
{
    // Verify that the specified view object isn't already being used
    // by another viewpoint attribute; if it is, then set our view
    // object to NULL instead.
    if (getMap()->mapFirstToSecond(theView) == NULL)
    {
        viewObject = theView;
        getMap()->registerLink(theView, this);
    }
    else
    {
        printf("vsViewpointAttribute::vsViewpointAttribute: View object is "
            "already in use by another viewpoint attribute\n");
        viewObject = NULL;
    }

    // Initialize the viewpoint offset matrix
    offsetMatrix.setIdentity();

    // Not attached to a component yet
    parentComponent = NULL;
}

// ------------------------------------------------------------------------
// Destructor - Detatch this attribute from its associated view object
// ------------------------------------------------------------------------
vsViewpointAttribute::~vsViewpointAttribute()
{
    // If we're attached to a vsView, remove the link from it to this
    // viewpoint attribute
    if (viewObject)
        getMap()->removeLink(viewObject, VS_OBJMAP_FIRST_LIST);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsViewpointAttribute::getClassName()
{
    return "vsViewpointAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsViewpointAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_VIEWPOINT;
}

// ------------------------------------------------------------------------
// Retrieves the category of this attribute
// ------------------------------------------------------------------------
int vsViewpointAttribute::getAttributeCategory()
{
    return VS_ATTRIBUTE_CATEGORY_CONTAINER;
}

// ------------------------------------------------------------------------
// Returns a clone of this attribute
// ------------------------------------------------------------------------
vsAttribute *vsViewpointAttribute::clone()
{
    vsViewpointAttribute *newAttr;

    // Cloning a viewpoint attribute isn't allowed because the view that
    // this attribute contains can only have one location.  Instead, we'll
    // return a new, empty viewpoint attribute
    newAttr = new vsViewpointAttribute();
    return newAttr;
}

// ------------------------------------------------------------------------
// Sets the view object associated with this attribute
// ------------------------------------------------------------------------
void vsViewpointAttribute::setView(vsView *theView)
{
    // If we're currently attached to another vsView, remove this link
    // in the viewpoint map first
    if (viewObject)
        getMap()->removeLink(viewObject, VS_OBJMAP_FIRST_LIST);

    // Verify that the specified view object isn't already being used
    // by another viewpoint attribute; if it is, then set our view
    // object to NULL instead.
    if (getMap()->mapFirstToSecond(theView) == NULL)
    {
        viewObject = theView;
        getMap()->registerLink(theView, this);
    }
    else
    {
        printf("vsViewpointAttribute::setView: View object is already in "
            "use by another viewpoint attribute\n");
        viewObject = NULL;
    }
}

// ------------------------------------------------------------------------
// Gets the view object associated with this attribute
// ------------------------------------------------------------------------
vsView *vsViewpointAttribute::getView()
{
    return viewObject;
}

// ------------------------------------------------------------------------
// Sets the offset matrix for this attribute. The offset matrix is
// multiplied into the view matrix before it is assigned to the view
// object.
// ------------------------------------------------------------------------
void vsViewpointAttribute::setOffsetMatrix(atMatrix newMatrix)
{
    offsetMatrix = newMatrix;
}

// ------------------------------------------------------------------------
// Retrieves the offset matrix for this attribute
// ------------------------------------------------------------------------
atMatrix vsViewpointAttribute::getOffsetMatrix()
{
    return offsetMatrix;
}

// ------------------------------------------------------------------------
// Static internal function
// Returns a pointer to the viewpoint attribute's view object map, creating
// it first if necessary
// ------------------------------------------------------------------------
vsObjectMap *vsViewpointAttribute::getMap()
{
    // Construct the view object map, if necessary
    if (!viewObjectMap)
        viewObjectMap = new vsObjectMap();

    // Return the map
    return viewObjectMap;
}

// ------------------------------------------------------------------------
// Static internal function
// Deletes the viewpoint attribute's map, if it has been created
// ------------------------------------------------------------------------
void vsViewpointAttribute::deleteMap()
{
    // Delete the view object map, if it exists
    if (viewObjectMap)
    {
        delete viewObjectMap;
        viewObjectMap = NULL;
    }
}

// ------------------------------------------------------------------------
// Internal function
// Returns if this attribute is available to be attached to a node
// ------------------------------------------------------------------------
bool vsViewpointAttribute::canAttach()
{
    // Return false if we're already attached to a component
    if (attachedCount)
        return false;

    // Otherwise, return true
    return true;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsViewpointAttribute::attach(vsNode *theNode)
{
    // Fail if we're already attached to a node
    if (attachedCount)
    {
        printf("vsViewpointAttribute::attach: Attribute is already attached\n");
        return;
    }

    // This attribute can only be attached to component nodes, so fail if
    // the specified node is not a component
    if (theNode->getNodeType() != VS_NODE_TYPE_COMPONENT)
    {
        printf("vsViewpointAttribute::attach: Can only attach viewpoint "
            "attributes to vsComponents\n");
        return;
    }
    
    // We now know that the node is a component, so keep track of the
    // component we're attaching to
    parentComponent = (vsComponent *)theNode;
    
    // Flag the attribute as attached to a component
    attachedCount = 1;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsViewpointAttribute::detach(vsNode *theNode)
{
    // Fail if we're not attached to anything
    if (!attachedCount)
    {
        printf("vsViewpointAttribute::detach: Attribute is not attached\n");
        return;
    }

    // Let go of whatever component we were attached to
    parentComponent = NULL;
    
    // Flag the attribute as not attached
    attachedCount = 0;
}

// ------------------------------------------------------------------------
// Internal function
// Intended to attach a duplicate of this attribute to the given node. This
// operation is not possible for this type of attribute because the
// contained object (vsView) can only conceptually have one location and
// thus one container on the tree.
// ------------------------------------------------------------------------
void vsViewpointAttribute::attachDuplicate(vsNode *theNode)
{
}

// ------------------------------------------------------------------------
// Internal function
// Causes the attribute to calculate the total transform to its parent
// node, and assign that data to its associated view object.
// ------------------------------------------------------------------------
void vsViewpointAttribute::update()
{
    atMatrix result;

    // No work to do if we're not attached, or we don't have a valid 
    // view object
    if (!attachedCount)
        return;
    if (!viewObject)
        return;

    // Get the position and orientation of the parent component in world 
    // coordinates
    result = parentComponent->getGlobalXform();

    // Apply the viewpoint offset matrix
    result = result * offsetMatrix;
    
    // Adjust the view object using the resulting position and orientation
    viewObject->setViewpoint(result[0][3], result[1][3], result[2][3]);
    viewObject->setDirectionFromRotation(result);
}
