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
// Constructor - Registers this attribute with the specified view object,
// and initializes the adjustment matrix.
// ------------------------------------------------------------------------
vsViewpointAttribute::vsViewpointAttribute(vsView *theView)
{
    viewObject = theView;
    if (!(viewObject->attachViewAttribute(this)))
        viewObject = NULL;

    offsetMatrix.setIdentity();
    parentComponent = NULL;
}

// ------------------------------------------------------------------------
// Destructor - Detatched this attribute from its associated view object
// ------------------------------------------------------------------------
vsViewpointAttribute::~vsViewpointAttribute()
{
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
    if (attachedFlag)
    {
        printf("vsViewpointAttribute::attach: Attribute is already attached\n");
        return;
    }

    if (theNode->getNodeType() == VS_NODE_TYPE_GEOMETRY)
    {
        printf("vsViewpointAttribute::attach: Can't attach viewpoint "
            "attributes to geometry nodes\n");
        return;
    }
    
    parentComponent = (vsComponent *)theNode;
    
    attachedFlag = 1;
}

// ------------------------------------------------------------------------
// VESS internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsViewpointAttribute::detach(vsNode *theNode)
{
    if (!attachedFlag)
    {
        printf("vsViewpointAttribute::detach: Attribute is not attached\n");
        return;
    }

    parentComponent = NULL;
    
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

    if (!attachedFlag)
        return;
    if (!viewObject)
        return;

    result = parentComponent->getGlobalXform();

    result = result * offsetMatrix;
    
    viewObject->setViewpoint(result[0][3], result[1][3], result[2][3]);
    viewObject->setDirectionFromRotation(result);
}
