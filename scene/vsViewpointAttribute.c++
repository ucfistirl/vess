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

#include <Performer/pf/pfSCS.h>
#include "vsComponent.h++"

// ------------------------------------------------------------------------
// Constructor - Initializes the attribute's adjustment matrix
// ------------------------------------------------------------------------
vsViewpointAttribute::vsViewpointAttribute()
{
    // Initialize the vsView object to none
    viewObject = NULL;

    // Initialize the offset matrix to identity
    offsetMatrix.setIdentity();

    // Mark this attribute as not attached to any node
    parentComponent = NULL;
}

// ------------------------------------------------------------------------
// Constructor - Registers this attribute with the specified view object,
// and initializes the adjustment matrix.
// ------------------------------------------------------------------------
vsViewpointAttribute::vsViewpointAttribute(vsView *theView)
{
    // Store the given vsView object
    viewObject = theView;

    // Attempt to register ourselves with the given vsView object; if
    // that object refuses, then remove our attachment to it
    if (!(viewObject->attachViewAttribute(this)))
        viewObject = NULL;

    // Initialize the offset matrix to identity
    offsetMatrix.setIdentity();

    // Mark this attribute as not attached to any node
    parentComponent = NULL;
}

// ------------------------------------------------------------------------
// Destructor - Detatched this attribute from its associated view object
// ------------------------------------------------------------------------
vsViewpointAttribute::~vsViewpointAttribute()
{
    // If we're associated with a vsView object, then remove that
    // association
    if (viewObject)
        viewObject->detachViewAttribute();
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
// Sets the view object associated with this attribute
// ------------------------------------------------------------------------
void vsViewpointAttribute::setView(vsView *theView)
{
    // Remove the connection to the old view object, if there is one
    if (viewObject)
        viewObject->detachViewAttribute();

    // Attempt to connect to the new view object; this may fail if
    // the view object is already associated with another view
    // attribute
    if (theView->attachViewAttribute(this))
        viewObject = theView;
    else
        viewObject = NULL;
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
void vsViewpointAttribute::setOffsetMatrix(vsMatrix newMatrix)
{
    offsetMatrix = newMatrix;
}

// ------------------------------------------------------------------------
// Retrieves the offset matrix for this attribute
// ------------------------------------------------------------------------
vsMatrix vsViewpointAttribute::getOffsetMatrix()
{
    return offsetMatrix;
}

// ------------------------------------------------------------------------
// VESS internal function
// Returns if this attribute is available to be attached to a node
// ------------------------------------------------------------------------
int vsViewpointAttribute::canAttach()
{
    // This attribute is not available to be attached if it is already
    // attached to another node
    if (attachedFlag)
        return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// VESS internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsViewpointAttribute::attach(vsNode *theNode)
{
    // Verify that we're not already attached to something
    if (attachedFlag)
    {
        printf("vsViewpointAttribute::attach: Attribute is already attached\n");
        return;
    }

    // Viewpoint attributes may not be attached to geometry nodes
    if ((theNode->getNodeType() == VS_NODE_TYPE_GEOMETRY) ||
        (theNode->getNodeType() == VS_NODE_TYPE_DYNAMIC_GEOMETRY))
    {
        printf("vsViewpointAttribute::attach: Can't attach viewpoint "
            "attributes to geometry nodes\n");
        return;
    }
    
    // Store a pointer to the parent component
    parentComponent = (vsComponent *)theNode;
    
    // Mark this attribute as attached
    attachedFlag = 1;
}

// ------------------------------------------------------------------------
// VESS internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsViewpointAttribute::detach(vsNode *theNode)
{
    // Can't detach an attribute that is not attached
    if (!attachedFlag)
    {
        printf("vsViewpointAttribute::detach: Attribute is not attached\n");
        return;
    }

    // Clear the parent component pointer
    parentComponent = NULL;
    
    // Mark this attribute as unattached
    attachedFlag = 0;
}

// ------------------------------------------------------------------------
// VESS internal function
// Intended to attach a duplicate of this attribute to the given node. This
// operation is not possible for this type of attribute because the
// contained object (vsView) can only conceptually have one location and
// thus one container on the tree.
// ------------------------------------------------------------------------
void vsViewpointAttribute::attachDuplicate(vsNode *theNode)
{
}

// ------------------------------------------------------------------------
// VESS internal function
// Causes the attribute to calculate the total transform to its parent
// node, and assign that data to its associated view object.
// ------------------------------------------------------------------------
void vsViewpointAttribute::update()
{
    pfMatrix xform;
    vsMatrix result;

    // An update on an unattached viewpoint attribute does nothing
    if (!attachedFlag)
        return;

    // An update on a viewpoint attribute that doesn't have an associated
    // vsView object does nothing
    if (!viewObject)
        return;

    // Update the associated vsView's position and location by getting the
    // global transform down to the component where the attribute is
    // attached, modifying it by the user-specified view offset matrix, and
    // then applying the result to the view object.
    result = parentComponent->getGlobalXform();
    result = result * offsetMatrix;
    viewObject->setViewpoint(result[0][3], result[1][3], result[2][3]);
    viewObject->setDirectionFromRotation(result);
}
