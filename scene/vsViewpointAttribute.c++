// File vsViewpointAttribute.c++

#include "vsViewpointAttribute.h++"

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
    componentMiddle = NULL;
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
    
    componentMiddle = ((vsComponent *)theNode)->getLightHook();
    
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

    componentMiddle = NULL;
    
    attachedFlag = 0;
}

// ------------------------------------------------------------------------
// VESS internal function
// Causes the attribute to calculate the total transform to its parent
// node, and assign that data to its associated view object.
// ------------------------------------------------------------------------
void vsViewpointAttribute::update()
{
    pfGroup *groupPtr;
    pfMatrix xform;
    const pfMatrix *scsMatPtr;
    vsMatrix result;
    vsQuat tempQuat;
    int loop, sloop;

    if (!attachedFlag)
        return;

    xform.makeIdent();
    groupPtr = componentMiddle;
    
    while (groupPtr->getNumParents() > 0)
    {
        if (groupPtr->isOfType(pfSCS::getClassType()))
        {
            scsMatPtr = ((pfSCS *)groupPtr)->getMatPtr();
            xform.preMult(*scsMatPtr);
        }
        
        groupPtr = groupPtr->getParent(0);
    }
    
    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            result[loop][sloop] = xform[sloop][loop];

    result = offsetMatrix * result;
    
    viewObject->setViewpoint(result[0][3], result[1][3], result[2][3]);
    tempQuat.setMatrixRotation(result);
    result.setQuatRotation(tempQuat);
    viewObject->setDirectionFromRotation(result);
}
