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
//    VESS Module:  vsBillboardAttribute.c++
//
//    Description:  Attribute that specifies that the geometry below the
//                  component be rotated to face the viewer at all times
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsBillboardAttribute.h++"
#include "vsBillboardCallback.h++"
#include "vsQuat.h++"
#include "vsComponent.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes the billboard settings
// ------------------------------------------------------------------------
vsBillboardAttribute::vsBillboardAttribute()
{
    // Set to default values (center at 0,0,0; no translations; front
    // direction positive-y; up direction positive-z)
    centerPoint.set(0.0, 0.0, 0.0);
    preTranslate.setIdentity();
    postTranslate.setIdentity();

    // Default forward direction = +Y axis
    frontDirection.set(0.0, 1.0, 0.0);

    // Default up direction = +Z axis
    upAxis.set(0.0, 0.0, 1.0);

    // No transform node yet, default mode axis-rotation
    billboardTransform = NULL;
    billboardMode = VS_BILLBOARD_ROT_AXIS;
    
    // Start off unattached
    attachedFlag = 0;
    
    // Create a callback object and set it to use this attribute
    billboardCallback = new vsBillboardCallback(this);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsBillboardAttribute::~vsBillboardAttribute()
{
    // Detach before deleting
    if (isAttached())
        detach(NULL);

    delete billboardCallback;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsBillboardAttribute::getClassName()
{
    return "vsBillboardAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsBillboardAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_BILLBOARD;
}

// ------------------------------------------------------------------------
// Retrieves the category of this attribute
// ------------------------------------------------------------------------
int vsBillboardAttribute::getAttributeCategory()
{
    return VS_ATTRIBUTE_CATEGORY_XFORM;
}

// ------------------------------------------------------------------------
// Sets the rotation mode of the billboard
// ------------------------------------------------------------------------
void vsBillboardAttribute::setMode(int mode)
{
    billboardMode = mode;
}

// ------------------------------------------------------------------------
// Retrieves the rotation mode of the billboard
// ------------------------------------------------------------------------
int vsBillboardAttribute::getMode()
{
    return billboardMode;
}

// ------------------------------------------------------------------------
// Sets the center point of the billboard
// ------------------------------------------------------------------------
void vsBillboardAttribute::setCenterPoint(vsVector newCenter)
{
    // Force our copy of the center point to be size 3
    centerPoint.clearCopy(newCenter);
    centerPoint.setSize(3);

    // Copy the new center point into the transform matrices
    preTranslate.setTranslation(-centerPoint[0], -centerPoint[1],
        -centerPoint[2]);
    postTranslate.setTranslation(centerPoint[0], centerPoint[1],
        centerPoint[2]);
}

// ------------------------------------------------------------------------
// Retrieves the center point of the billboard
// ------------------------------------------------------------------------
vsVector vsBillboardAttribute::getCenterPoint()
{
    return centerPoint;
}

// ------------------------------------------------------------------------
// Sets the 'forward' direction vector for the billboard
// ------------------------------------------------------------------------
void vsBillboardAttribute::setFrontDirection(vsVector newFront)
{
    // Force our copy of the front direction to be of size 3 and
    // of unit length
    frontDirection.clearCopy(newFront);
    frontDirection.setSize(3);
    frontDirection.normalize();
}

// ------------------------------------------------------------------------
// Retrieves the 'forward' direction vector for the billboard
// ------------------------------------------------------------------------
vsVector vsBillboardAttribute::getFrontDirection()
{
    return frontDirection;
}

// ------------------------------------------------------------------------
// Sets the axis value for the billboard; this value is interpreted as
// either an axis of rotation, for axially rotating billboards, or an 'up'
// direction vector, for point rotation billboards.
// ------------------------------------------------------------------------
void vsBillboardAttribute::setAxis(vsVector newAxis)
{
    // Force our copy of the up direction to be of size 3 and
    // of unit length
    upAxis.clearCopy(newAxis);
    upAxis.setSize(3);
    upAxis.normalize();
}

// ------------------------------------------------------------------------
// Retrieves the axis value for the billboard
// ------------------------------------------------------------------------
vsVector vsBillboardAttribute::getAxis()
{
    return upAxis;
}

// ------------------------------------------------------------------------
// Internal function
// Returns if this attribute is available to be attached to a node
// ------------------------------------------------------------------------
int vsBillboardAttribute::canAttach()
{
    // This attribute is not available to be attached if it is already
    // attached to another node
    if (attachedFlag)
        return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsBillboardAttribute::attach(vsNode *theNode)
{
    osg::Group *lightHook, *childGroup;

    // Verify that we're not already attached to something
    if (attachedFlag)
    {
        printf("vsBillboardAttribute::attach: Attribute is already attached\n");
        return;
    }
    
    // Verify that we're getting a component to attach to
    if (theNode->getNodeType() != VS_NODE_TYPE_COMPONENT)
    {
        printf("vsBillboardAttribute::attach: Can only attach billboard "
            "attributes to vsComponents\n");
        return;
    }
    
    // Create the MatrixTransform object that will hold the billboard's
    // rotation, and position it between the lightHook and bottomGroup
    // Groups of the component.
    billboardTransform = new osg::MatrixTransform();
    billboardTransform->ref();

    lightHook = ((vsComponent *)theNode)->getLightHook();
    childGroup = (osg::Group *)(lightHook->getChild(0));
    lightHook->replaceChild(childGroup, billboardTransform);
    billboardTransform->addChild(childGroup);

    // Set the billboard's callback object as the cull callback for
    // the component
    lightHook->setCullCallback(billboardCallback);
    
    // Mark this attribute as attached
    attachedFlag = 1;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsBillboardAttribute::detach(vsNode *theNode)
{
    osg::Group *lightHook, *childGroup;

    // Can't detach an attribute that is not attached
    if (!attachedFlag)
    {
        printf("vsBillboardAttribute::detach: Attribute is not attached\n");
        return;
    }

    // Remove the billboard's MatrixTransform object from between
    // the component's lightHook and bottomGroup Groups.
    lightHook = billboardTransform->getParent(0);
    childGroup = (osg::Group *)(billboardTransform->getChild(0));
    billboardTransform->removeChild(childGroup);
    lightHook->replaceChild(billboardTransform, childGroup);
    billboardTransform->unref();

    // Remove the billboard's callback hook
    lightHook->setCullCallback(NULL);

    attachedFlag = 0;
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsBillboardAttribute::attachDuplicate(vsNode *theNode)
{
    vsBillboardAttribute *newAttrib;
    
    // Create a duplicate switch attribute
    newAttrib = new vsBillboardAttribute();
    
    // Copy the billboard parameters to the new attribute
    newAttrib->setMode(getMode());
    newAttrib->setCenterPoint(getCenterPoint());
    newAttrib->setFrontDirection(getFrontDirection());
    newAttrib->setAxis(getAxis());
    
    // Attach the duplicate attribute to the specified node
    theNode->addAttribute(newAttrib);
}

// ------------------------------------------------------------------------
// Internal function
// Finds the optimal rotation to cause the billboard to face the viewer,
// and sets the Performer transform to that rotation.
// ------------------------------------------------------------------------
void vsBillboardAttribute::adjustTransform(vsMatrix viewMatrix,
                                           vsMatrix currentXform)
{
    vsVector viewpoint, viewDir;
    vsVector center, front, up;
    vsVector midAxis, worldUp;
    double theta;
    vsQuat resultQuat;
    vsMatrix resultMat, tempMat;
    osg::Matrix osgMat;
    int loop, sloop;
    double dotValue;
    vsVector cross;
    vsMatrix invMat;
    vsVector viewUp;

    // Transform each important data value about the billboarded object
    // by the series of transforms in the scene above this component
    center = currentXform.getPointXform(centerPoint);
    front = currentXform.getVectorXform(frontDirection);
    front.normalize();
    up = currentXform.getVectorXform(upAxis);
    up.normalize();

    // Construct the direction from the viewpoint to the billboarded object
    // by determining the viewpoint and subtracting the object's center
    // point from it, and normalizing the result.
    viewpoint.set(0.0, 0.0, 0.0);
    viewpoint = viewMatrix.getPointXform(viewpoint);
    viewDir = viewpoint - center;
    viewDir.normalize();

    // Determine which rotation mode is in use
    if (billboardMode == VS_BILLBOARD_ROT_AXIS)
    {
        // * Axis rotation mode
        // Project both the 'view' and 'front' vectors onto the plane
        // specified by the center point of the billboard and the normal
        // vector 'up'.
        dotValue = viewDir.getDotProduct(up);
        viewDir = viewDir - (up * dotValue);
        viewDir.normalize();
        dotValue = front.getDotProduct(up);
        front = front - (up * dotValue);
        front.normalize();

        // Calculate the angle between the view vector and the object's
        // forward vector; adjust for the sign change when the cross
        // product of the two goes negative. (The vsVector.getAngleBetween
        // function doesn't take this into account.)
        theta = front.getAngleBetween(viewDir);
        cross = front.getCrossProduct(viewDir);
        cross.normalize();
        // The 'up' direction is our positive direction for this purpose;
	// if the cross product points the other direction instead, then
	// it's considered negative.
        if (!(cross == up))
            theta *= -1.0;

        // Create the rotation matrix
        resultQuat.setAxisAngleRotation(up[0], up[1], up[2], theta);
        resultMat.setQuatRotation(resultQuat);
    }
    else
    {
        // * Point rotation mode
        // First, create the rotation that rotates the object's 'forward'
        // vector to the vector from the object to the viewpoint.
        midAxis = front.getCrossProduct(viewDir);
        theta = front.getAngleBetween(viewDir);
        resultQuat.setAxisAngleRotation(midAxis[0], midAxis[1], midAxis[2],
            theta);
        resultMat.setQuatRotation(resultQuat);

        // Second, find the rotation that rotates the 'up' directions of
        // the object and the world to be as close together as possible.
	// (They can't always coincide, if the plane perpendicular to the
	// forward view direction doesn't contain the world up direction.)

        up = resultMat.getVectorXform(up);
        up.normalize();
        worldUp.set(0.0, 0.0, 1.0);
        // In point-eye mode, the world 'up' direction is based on the
        // viewpoint of the user rather than just the z-axis.
        if (billboardMode == VS_BILLBOARD_ROT_POINT_EYE)
            worldUp = viewMatrix.getVectorXform(worldUp);
        worldUp.normalize();

        // Project both 'up' vectors onto the plane specified by a
        // center point (the center point of the billboard) and a normal
	// vector (the vector from the object to the viewpoint). This
	// allows us to get as close as possible in the case that the
	// plane perpendicular to the forward direction doesn't contain
	// the world up direction.
        dotValue = worldUp.getDotProduct(viewDir);
        worldUp = worldUp - (viewDir * dotValue);
        worldUp.normalize();
        dotValue = up.getDotProduct(viewDir);
        up = up - (viewDir * dotValue);
        up.normalize();

        // Calculate the angle between the two 'up' vectors to get the roll
	// rotation value; adjust for the sign change when the cross product
	// of the two goes negative. (The vsVector.getAngleBetween function
	// doesn't take this into account.)
        theta = up.getAngleBetween(worldUp);
        cross = up.getCrossProduct(worldUp);
        cross.normalize();
        if (!(cross == viewDir))
            theta *= -1.0;

        // Finally, set the result matrix to the product of the two
        // computed rotation matrices.
        resultQuat.setAxisAngleRotation(viewDir[0], viewDir[1], viewDir[2],
            theta);
        tempMat.setQuatRotation(resultQuat);
        resultMat.preMultiply(tempMat);
    }

    // Strip the translation from the current transform matrix; for the
    // next part, we want a global-rotation only matrix.
    center.set(0.0, 0.0, 0.0);
    center = currentXform.getPointXform(center);
    tempMat.setTranslation(-center[0], -center[1], -center[2]);
    tempMat = tempMat * currentXform;

    // The function result matrix is in the global coordinate system;
    // transform the result rotation into the local coordinate system
    // of the component, using the new current transform.
    invMat = tempMat.getInverse();
    resultMat = invMat * resultMat * tempMat;

    // Factor in the center point of the object so that rotations seem
    // to be around this center point rather than just the origin of
    // the billboard's component
    resultMat.postMultiply(preTranslate);
    resultMat.preMultiply(postTranslate);

    // Invert when going from VESS to OSG
    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            osgMat(loop, sloop) = resultMat[sloop][loop];

    // Set the billboard's transform matrix
    billboardTransform->setMatrix(osgMat);
}
