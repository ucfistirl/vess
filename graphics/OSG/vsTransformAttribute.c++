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
//    VESS Module:  vsTransformAttribute.c++
//
//    Description:  Attribute that specifies a geometric transformation
//                  that should be applied to all of the children of the
//                  component
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsTransformAttribute.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes the internal transforms
// ------------------------------------------------------------------------
vsTransformAttribute::vsTransformAttribute()
{
    componentTop = NULL;
    transform = NULL;
    
    // Initialize the three core transform matrices to the identity.
    preMatrix.setIdentity();
    dynMatrix.setIdentity();
    postMatrix.setIdentity();
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsTransformAttribute::~vsTransformAttribute()
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsTransformAttribute::getClassName()
{
    return "vsTransformAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsTransformAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_TRANSFORM;
}

// ------------------------------------------------------------------------
// Retrieves the category of this attribute
// ------------------------------------------------------------------------
int vsTransformAttribute::getAttributeCategory()
{
    return VS_ATTRIBUTE_CATEGORY_XFORM;
}

// ------------------------------------------------------------------------
// Sets the pre-transform matrix
// ------------------------------------------------------------------------
void vsTransformAttribute::setPreTransform(atMatrix newTransform)
{
    preMatrix = newTransform;
    
    // If not attached, return.
    if (!attachedCount)
        return;
    
    // Apply the three transformations.
    applyTransformations();   
}

// ------------------------------------------------------------------------
// Retrieves the pre-transform matrix
// ------------------------------------------------------------------------
atMatrix vsTransformAttribute::getPreTransform()
{
    return preMatrix;
}

// ------------------------------------------------------------------------
// Sets the dynamic transform matrix
// ------------------------------------------------------------------------
void vsTransformAttribute::setDynamicTransform(atMatrix newTransform)
{
    dynMatrix = newTransform;
    
    // If not attached, return.
    if (!attachedCount)
        return;
    
    // Apply the three transformations.
    applyTransformations();   
}

// ------------------------------------------------------------------------
// Retrieves the dynamic transform matrix
// ------------------------------------------------------------------------
atMatrix vsTransformAttribute::getDynamicTransform()
{
    return dynMatrix;
}

// ------------------------------------------------------------------------
// Sets the post-transform matrix
// ------------------------------------------------------------------------
void vsTransformAttribute::setPostTransform(atMatrix newTransform)
{
    postMatrix = newTransform;
    
    // If not attached, return.
    if (!attachedCount)
        return;

    // Apply the three transformations.
    applyTransformations();   
}

// ------------------------------------------------------------------------
// Retrieves the post-transform matrix
// ------------------------------------------------------------------------
atMatrix vsTransformAttribute::getPostTransform()
{
    return postMatrix;
}

// ------------------------------------------------------------------------
// Private function
// Multiplies the threes matrices (pre, dyn, post), switches to columns and
// rows to conform with the osg matrix format, and updates the transform.
// ------------------------------------------------------------------------
void vsTransformAttribute::applyTransformations()
{
    atMatrix identityMatrix;
    atMatrix productMatrix;
    osg::Matrix osgMatrix;
    int loop, sloop;

    // Initialize the identity matrix.
    identityMatrix.setIdentity();

    // Set this to the identity matrix now after storing the value above.
    // This matrix is used to calculate the product of the three matrices.
    productMatrix.setIdentity();

    // Multiply the three matrices together.
    // If any happen to be the identity matrix, don't multiply it.
    if (!(preMatrix == identityMatrix))
        productMatrix = productMatrix * preMatrix;
    if (!(dynMatrix == identityMatrix))
        productMatrix = productMatrix * dynMatrix;
    if (!(postMatrix == identityMatrix))
        productMatrix = productMatrix * postMatrix;

    // Yes, the index order is reversed. OSG does its
    // multiplication the opposite of the way we do.
    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            osgMatrix(loop, sloop) = productMatrix[sloop][loop];

    // Update the osg MatrixTransform with the new value.
    transform->setMatrix(osgMatrix);
}

// ------------------------------------------------------------------------
// Internal function
// Returns if this attribute is available to be attached to a node
// ------------------------------------------------------------------------
bool vsTransformAttribute::canAttach()
{
    if (attachedCount)
        return false;

    return true;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsTransformAttribute::attach(vsNode *theNode)
{
    osg::Group *aboveGroup, *belowGroup;

    if (attachedCount)
    {
        printf("vsTransformAttribute::attach: Attribute is already attached\n");
        return;
    }
    
    if (theNode->getNodeType() != VS_NODE_TYPE_COMPONENT)
    {
        printf("vsTransformAttribute::attach: Can only attach transform "
            "attributes to vsComponents\n");
        return;
    }

    // Get the top group of the vsComponent which this attribute is being
    // attached to.
    componentTop = ((vsComponent *)theNode)->getTopGroup();

    // Create the MatrixTransform to be placed in the osg scenegraph.
    transform = new osg::MatrixTransform();
    transform->ref();

    // Move the top groups child group to be children of transform.
    aboveGroup = componentTop;
    belowGroup = (osg::Group *) (aboveGroup->getChild(0));
    aboveGroup->removeChild(belowGroup);
    aboveGroup->addChild(transform);
    transform->addChild(belowGroup);

    // Set attached flag to true.
    attachedCount = 1;

    // Apply the three transformations.
    applyTransformations();   
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsTransformAttribute::detach(vsNode *theNode)
{
    osg::Group *aboveGroup;
    osg::Node *belowNode;

    if (!attachedCount)
    {
        printf("vsTransformAttribute::detach: Attribute is not attached\n");
        return;
    }

    // Update the attached flag to false.
    attachedCount = 0;

    // Replace the transform child in the aboveGroup with its child group.
    aboveGroup = componentTop;
    belowNode = transform->getChild(0);
    transform->removeChild(belowNode);
    aboveGroup->removeChild(transform);
    aboveGroup->addChild(belowNode);

    // Forget the stored componentTop.
    componentTop = NULL;

    // Delete and forget the transform group.
    transform->unref();
    transform = NULL;
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsTransformAttribute::attachDuplicate(vsNode *theNode)
{
    vsTransformAttribute *newAttrib;
    atMatrix xformMat;
    
    // Create a new transformAttribute.
    newAttrib = new vsTransformAttribute();
    
    // Set the new tranfromAttribute's tranforms to the values for this one.
    newAttrib->setPreTransform(getPreTransform());
    newAttrib->setDynamicTransform(getDynamicTransform());
    newAttrib->setPostTransform(getPostTransform());

    // Attach it.
    theNode->addAttribute(newAttrib);
}

// ------------------------------------------------------------------------
// Internal function
// Returns the combined transform matrix (pre/dynamic/post).  Useful to
// obtain the entire transform without multiplying the three parts.
// ------------------------------------------------------------------------
atMatrix vsTransformAttribute::getCombinedTransform()
{
    int loop, sloop;
    osg::Matrix osgMatrix;
    atMatrix resultMatrix;

    // Update the osg MatrixTransform with the new value.
    osgMatrix = transform->getMatrix();

    // Transpose the matrix to the VESS standard and return it
    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            resultMatrix[sloop][loop] = osgMatrix(loop, sloop);

    return resultMatrix;
}
