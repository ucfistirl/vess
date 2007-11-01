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

#include "atQuat.h++"
#include "vsComponent.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes the billboard settings
// ------------------------------------------------------------------------
vsBillboardAttribute::vsBillboardAttribute()
{
    centerPoint.set(0.0, 0.0, 0.0);
    preTranslate.setIdentity();
    postTranslate.setIdentity();
    frontDirection.set(0.0, 1.0, 0.0);
    upAxis.set(0.0, 0.0, 1.0);
    billboardTransform = NULL;
    billboardMode = VS_BILLBOARD_ROT_AXIS;
    
    attachedCount = 0;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsBillboardAttribute::~vsBillboardAttribute()
{
    if (isAttached())
        detach(NULL);
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
void vsBillboardAttribute::setCenterPoint(atVector newCenter)
{
    centerPoint.clearCopy(newCenter);
    centerPoint.setSize(3);
    preTranslate.setTranslation(-centerPoint[0], -centerPoint[1],
        -centerPoint[2]);
    postTranslate.setTranslation(centerPoint[0], centerPoint[1],
        centerPoint[2]);
}

// ------------------------------------------------------------------------
// Retrieves the center point of the billboard
// ------------------------------------------------------------------------
atVector vsBillboardAttribute::getCenterPoint()
{
    return centerPoint;
}

// ------------------------------------------------------------------------
// Sets the 'forward' direction vector for the billboard
// ------------------------------------------------------------------------
void vsBillboardAttribute::setFrontDirection(atVector newFront)
{
    frontDirection.clearCopy(newFront);
    frontDirection.setSize(3);
    frontDirection.normalize();
}

// ------------------------------------------------------------------------
// Retrieves the 'forward' direction vector for the billboard
// ------------------------------------------------------------------------
atVector vsBillboardAttribute::getFrontDirection()
{
    return frontDirection;
}

// ------------------------------------------------------------------------
// Sets the axis value for the billboard; this value is interpreted as
// either an axis of rotation, for axially rotating billboards, or an 'up'
// direction vector, for point rotation billboards.
// ------------------------------------------------------------------------
void vsBillboardAttribute::setAxis(atVector newAxis)
{
    upAxis.clearCopy(newAxis);
    upAxis.setSize(3);
    upAxis.normalize();
}

// ------------------------------------------------------------------------
// Retrieves the axis value for the billboard
// ------------------------------------------------------------------------
atVector vsBillboardAttribute::getAxis()
{
    return upAxis;
}

// ------------------------------------------------------------------------
// Internal function
// Returns if this attribute is available to be attached to a node
// ------------------------------------------------------------------------
bool vsBillboardAttribute::canAttach()
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
void vsBillboardAttribute::attach(vsNode *theNode)
{
    pfGroup *lightHook, *childGroup;

    if (attachedCount)
    {
        printf("vsBillboardAttribute::attach: Attribute is already attached\n");
        return;
    }
    
    if ((theNode->getNodeType() == VS_NODE_TYPE_GEOMETRY) ||
        (theNode->getNodeType() == VS_NODE_TYPE_DYNAMIC_GEOMETRY) ||
        (theNode->getNodeType() == VS_NODE_TYPE_SKELETON_MESH_GEOMETRY))
    {
        printf("vsBillboardAttribute::attach: Can't attach billboard "
            "attributes to geometry nodes\n");
        return;
    }
    
    lightHook = ((vsComponent *)theNode)->getLightHook();
    billboardTransform = new pfDCS();
    billboardTransform->ref();
    childGroup = (pfGroup *)(lightHook->getChild(0));
    lightHook->replaceChild(childGroup, billboardTransform);
    billboardTransform->addChild(childGroup);
    lightHook->setTravFuncs(PFTRAV_APP, travCallback, NULL);
    lightHook->setTravData(PFTRAV_APP, this);
    
    attachedCount = 1;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsBillboardAttribute::detach(vsNode *theNode)
{
    pfGroup *lightHook, *childGroup;

    if (!attachedCount)
    {
        printf("vsBillboardAttribute::detach: Attribute is not attached\n");
        return;
    }

    lightHook = billboardTransform->getParent(0);
    childGroup = (pfGroup *)(billboardTransform->getChild(0));
    billboardTransform->removeChild(childGroup);
    lightHook->replaceChild(billboardTransform, childGroup);
    lightHook->setTravFuncs(PFTRAV_APP, NULL, NULL);
    lightHook->setTravData(PFTRAV_APP, NULL);
    billboardTransform->unref();
    pfDelete(billboardTransform);
    
    attachedCount = 0;
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsBillboardAttribute::attachDuplicate(vsNode *theNode)
{
    vsBillboardAttribute *newAttrib;
    
    newAttrib = new vsBillboardAttribute();
    
    newAttrib->setMode(getMode());
    newAttrib->setCenterPoint(getCenterPoint());
    newAttrib->setFrontDirection(getFrontDirection());
    newAttrib->setAxis(getAxis());
    
    theNode->addAttribute(newAttrib);
}

// ------------------------------------------------------------------------
// static Internal function - Passed to Performer as a callback
// During Performer's APP traversal, determines the viewpoint and directs
// the billboard object to face the viewer.
// ------------------------------------------------------------------------
int vsBillboardAttribute::travCallback(pfTraverser *_trav, void *_userData)
{
    pfChannel *currentChannel;
    pfMatrix performerMatrix;
    atMatrix viewMatrix, xformMatrix;
    int loop, sloop;
    
    currentChannel = _trav->getChan();
    currentChannel->getViewMat(performerMatrix);
    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            viewMatrix[loop][sloop] = performerMatrix[sloop][loop];

    _trav->getMat(performerMatrix);
    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            xformMatrix[loop][sloop] = performerMatrix[sloop][loop];

    ((vsBillboardAttribute *)_userData)->adjustTransform(viewMatrix,
        xformMatrix);

    return PFTRAV_CONT;
}

// ------------------------------------------------------------------------
// Internal function
// Finds the optimal rotation to cause the billboard to face the viewer,
// and sets the Performer transform to that rotation.
// ------------------------------------------------------------------------
void vsBillboardAttribute::adjustTransform(atMatrix viewMatrix,
                                           atMatrix currentXform)
{
    atVector viewpoint, viewDir;
    atVector center, front, up;
    atVector midAxis, worldUp;
    double theta;
    atQuat resultQuat;
    atMatrix resultMat, tempMat;
    pfMatrix performerMat;
    int loop, sloop;
    double dotValue;
    atVector cross;
    atMatrix invMat;
    atVector viewUp;

    // Transform each important data value about the billboarded object
    // by the series of transforms in the scene above this component
    center = currentXform.getPointXform(centerPoint);
    front = currentXform.getVectorXform(frontDirection);
    front.normalize();
    up = currentXform.getVectorXform(upAxis);
    up.normalize();

    viewpoint.set(0.0, 0.0, 0.0);
    viewpoint = viewMatrix.getPointXform(viewpoint);
    viewDir = viewpoint - center;
    viewDir.normalize();

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
        // product of the two goes negative. (The atVector.getAngleBetween
        // function doesn't take this into account.)
        theta = front.getAngleBetween(viewDir);
        cross = front.getCrossProduct(viewDir);
        cross.normalize();
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
        up = resultMat.getVectorXform(up);
        up.normalize();
        worldUp.set(0.0, 0.0, 1.0);
        // In point-eye mode, the world 'up' direction is based on the
        // viewpoint of the user rather than just the z-axis.
        if (billboardMode == VS_BILLBOARD_ROT_POINT_EYE)
            worldUp = viewMatrix.getVectorXform(worldUp);
        worldUp.normalize();

        // Project both 'up' vectors onto the plane specified by the
        // center point of the billboard and the normal vector as the
        // vector from the object to the viewpoint
        dotValue = worldUp.getDotProduct(viewDir);
        worldUp = worldUp - (viewDir * dotValue);
        worldUp.normalize();
        dotValue = up.getDotProduct(viewDir);
        up = up - (viewDir * dotValue);
        up.normalize();

        // Calculate the angle between the two 'up' vectors; adjust for the
        // sign change when the cross product of the two goes negative. (The
        // atVector.getAngleBetween function doesn't take this into account.)
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

    // Strip the translation from the current transform matrix
    center.set(0.0, 0.0, 0.0);
    center = currentXform.getPointXform(center);
    tempMat.setTranslation(-center[0], -center[1], -center[2]);
    tempMat = tempMat * currentXform;

    // Transform the result rotation into the local coordinate system
    // of the component, using the new current transform
    invMat = tempMat.getInverse();
    resultMat = invMat * resultMat * tempMat;

    // Factor in the center point of the object
    resultMat.postMultiply(preTranslate);
    resultMat.preMultiply(postTranslate);

    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            performerMat[loop][sloop] = resultMat[sloop][loop];

    billboardTransform->setMat(performerMat);
}
