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

#include "vsBillboardAttribute.h++"

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
    frontDirection.set(0.0, 1.0, 0.0);
    upAxis.set(0.0, 0.0, 1.0);

    // No transform node yet, default mode axis-rotation
    billboardTransform = NULL;
    billboardMode = VS_BILLBOARD_ROT_AXIS;
    
    // Start off unattached
    attachedFlag = 0;
}

// ------------------------------------------------------------------------
// VESS internal function
// Constructor - Creates the initial billboard settings using the data
// contained within a Performer billboard object
// ------------------------------------------------------------------------
vsBillboardAttribute::vsBillboardAttribute(pfBillboard *billboard)
{
    pfVec3 data;
    
    // * Copy the billboard data into our internal storage

    // Center point (point around which rotation occurs, in
    // component-local coordinates
    billboard->getPos(0, data);
    centerPoint.set(data[0], data[1], data[2]);
    preTranslate.setTranslation(-centerPoint[0], -centerPoint[1],
        -centerPoint[2]);
    postTranslate.setTranslation(centerPoint[0], centerPoint[1],
        centerPoint[2]);

    // Front direction (defaults to positive-y)
    frontDirection.set(0.0, 1.0, 0.0);

    // Up direction (axis of rotation, or world-up direction, depending
    // on the rotation mode)
    billboard->getAxis(data);
    upAxis.set(data[0], data[1], data[2]);

    // Rotation mode
    switch (billboard->getMode(PFBB_ROT))
    {
        case PFBB_AXIAL_ROT:
            billboardMode = VS_BILLBOARD_ROT_AXIS;
            break;
        case PFBB_POINT_ROT_EYE:
            billboardMode = VS_BILLBOARD_ROT_POINT_EYE;
            break;
        case PFBB_POINT_ROT_WORLD:
            billboardMode = VS_BILLBOARD_ROT_POINT_WORLD;
            break;
        default:
            billboardMode = VS_BILLBOARD_ROT_AXIS;
            break;
    }
    
    // Start off UNattached; we'll go through the regular attach mechanism
    // to hook this one in
    attachedFlag = 0;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsBillboardAttribute::~vsBillboardAttribute()
{
    // Detach before deleting
    if (isAttached())
        detach(NULL);
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
// VESS internal function
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
// VESS internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsBillboardAttribute::attach(vsNode *theNode)
{
    pfGroup *lightHook, *childGroup;

    // Verify that we're not already attached to something
    if (attachedFlag)
    {
        printf("vsBillboardAttribute::attach: Attribute is already attached\n");
        return;
    }
    
    // Billboard attributes may not be attached to geometry nodes
    if ((theNode->getNodeType() == VS_NODE_TYPE_GEOMETRY) ||
        (theNode->getNodeType() == VS_NODE_TYPE_DYNAMIC_GEOMETRY))
    {
        printf("vsBillboardAttribute::attach: Can't attach billboard "
            "attributes to geometry nodes\n");
        return;
    }
    
    // Create a new pfDCS to hold the transform matrix associated with
    // the billboard, and place it between the lightHook group and
    // the bottomGroup of the vsComponent.
    lightHook = ((vsComponent *)theNode)->getLightHook();
    billboardTransform = new pfDCS();
    billboardTransform->ref();
    childGroup = (pfGroup *)(lightHook->getChild(0));
    lightHook->replaceChild(childGroup, billboardTransform);
    billboardTransform->addChild(childGroup);

    // Configure Performer's APP callback to call this object when
    // the lightHook group gets traversed, in order to adjust the
    // billboard's transform before it gets rendered.
    lightHook->setTravFuncs(PFTRAV_APP, travCallback, NULL);
    lightHook->setTravData(PFTRAV_APP, this);
    
    // Mark this attribute as attached
    attachedFlag = 1;
}

// ------------------------------------------------------------------------
// VESS internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsBillboardAttribute::detach(vsNode *theNode)
{
    pfGroup *lightHook, *childGroup;

    // Can't detach an attribute that is not attached
    if (!attachedFlag)
    {
        printf("vsBillboardAttribute::detach: Attribute is not attached\n");
        return;
    }

    // Remove the billboard's pfDCS from between the lightHook and
    // bottomGroup pfGroups
    lightHook = billboardTransform->getParent(0);
    childGroup = (pfGroup *)(billboardTransform->getChild(0));
    billboardTransform->removeChild(childGroup);
    lightHook->replaceChild(billboardTransform, childGroup);

    // Unset the Performer APP traversal callback from the lightHook group
    lightHook->setTravFuncs(PFTRAV_APP, NULL, NULL);
    lightHook->setTravData(PFTRAV_APP, NULL);

    // Destroy the pfDCS
    billboardTransform->unref();
    pfDelete(billboardTransform);
    
    // Mark this attribute as unattached
    attachedFlag = 0;
}

// ------------------------------------------------------------------------
// VESS internal function
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
// static VESS internal function - Passed to Performer as a callback
// During Performer's APP traversal, determines the viewpoint and directs
// the billboard object to face the viewer.
// ------------------------------------------------------------------------
int vsBillboardAttribute::travCallback(pfTraverser *_trav, void *_userData)
{
    pfChannel *currentChannel;
    pfMatrix performerMatrix;
    vsMatrix viewMatrix, xformMatrix;
    int loop, sloop;
    
    // Obtain the view transform matrix from the Performer channel
    currentChannel = _trav->getChan();
    currentChannel->getViewMat(performerMatrix);

    // Copy the Performer matrix to a VESS matrix, transposing as we go
    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            viewMatrix[loop][sloop] = performerMatrix[sloop][loop];

    // Obtain the current global transform from the traversal object
    _trav->getMat(performerMatrix);

    // Copy the Performer matrix to a VESS matrix, transposing as we go
    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            xformMatrix[loop][sloop] = performerMatrix[sloop][loop];

    // Prompt the billboard attribute to recompute its rotation using
    // the calculated view projection and global transform matrices
    ((vsBillboardAttribute *)_userData)->adjustTransform(viewMatrix,
        xformMatrix);

    // Prompt Performer to continue its app traversal
    return PFTRAV_CONT;
}

// ------------------------------------------------------------------------
// VESS internal function
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
    pfMatrix performerMat;
    int loop, sloop;
    double dotValue;
    vsVector cross;
    vsMatrix invMat;
    vsVector viewUp;

    // Transform each important data value about the billboarded object
    // by the component's global transform
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

    // Transpose the VESS matrix to get a Performer matrix
    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            performerMat[loop][sloop] = resultMat[sloop][loop];

    // Set the final billboard transformation on the billboard's
    // Performer transform node
    billboardTransform->setMat(performerMat);
}
