// File vsBillboardAttribute.c++

#include "vsBillboardAttribute.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes the billboard settings
// ------------------------------------------------------------------------
vsBillboardAttribute::vsBillboardAttribute()
{
    centerPoint.set(0.0, 0.0, 0.0);
    preTranslate.setIdentity();
    postTranslate.setIdentity();
    frontDirection.set(0.0, -1.0, 0.0);
    upAxis.set(0.0, 0.0, 1.0);
    billboardTransform = NULL;
    billboardMode = VS_BILLBOARD_ROT_AXIS;
    
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
    
    // Copy the billboard data into our internal storage
    billboard->getPos(0, data);
    centerPoint.set(data[0], data[1], data[2]);
    preTranslate.setTranslation(-centerPoint[0], -centerPoint[1],
        -centerPoint[2]);
    postTranslate.setTranslation(centerPoint[0], centerPoint[1],
        centerPoint[2]);
    frontDirection.set(0.0, -1.0, 0.0);
    billboard->getAxis(data);
    upAxis.set(data[0], data[1], data[2]);
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
    
    // Go through the regular attach mechanism to hook this one in
    attachedFlag = 0;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsBillboardAttribute::~vsBillboardAttribute()
{
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsBillboardAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_BILLBOARD;
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
vsVector vsBillboardAttribute::getCenterPoint()
{
    return centerPoint;
}

// ------------------------------------------------------------------------
// Sets the 'forward' direction vector for the billboard
// ------------------------------------------------------------------------
void vsBillboardAttribute::setFrontDirection(vsVector newFront)
{
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
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsBillboardAttribute::attach(vsNode *theNode)
{
    pfGroup *lightHook, *childGroup;

    if (attachedFlag)
    {
        printf("vsBillboardAttribute::attach: Attribute is already attached\n");
        return;
    }
    
    if (theNode->getNodeType() == VS_NODE_TYPE_GEOMETRY)
    {
        printf("vsBillboardAttribute::attach: Can't attach billboard "
            "attributes to geometry nodes\n");
        return;
    }
    
    lightHook = ((vsComponent *)theNode)->getLightHook();
    billboardTransform = new pfDCS();
    childGroup = (pfGroup *)(lightHook->getChild(0));
    lightHook->replaceChild(childGroup, billboardTransform);
    billboardTransform->addChild(childGroup);
    lightHook->setTravFuncs(PFTRAV_APP, travCallback, NULL);
    lightHook->setTravData(PFTRAV_APP, this);
    
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

    if (!attachedFlag)
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
    pfDelete(billboardTransform);
    
    attachedFlag = 0;
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
        dotValue = front.getDotProduct(up);
        front = front - (up * dotValue);

        // Calculate the angle between the view vector and the object's
        // forward vector; adjust for the sign change when the cross
        // product of the two goes negative. (The vsVector.getAngleBetween
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
        // vsVector.getAngleBetween function doesn't take this into account.)
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

    resultMat.postMultiply(preTranslate);
    resultMat.preMultiply(postTranslate);

    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            performerMat[loop][sloop] = resultMat[sloop][loop];

    billboardTransform->setMat(performerMat);
}
