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

#include "vsTransformAttribute.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes the internal transforms
// ------------------------------------------------------------------------
vsTransformAttribute::vsTransformAttribute()
{
    componentTop = NULL;
    preTransform = NULL;
    dynTransform = NULL;
    postTransform = NULL;
    
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
void vsTransformAttribute::setPreTransform(vsMatrix newTransform)
{
    pfSCS *performerTransform;
    vsMatrix identityMatrix;
    pfMatrix performerMatrix;
    pfGroup *aboveGroup, *belowGroup;
    int loop, sloop;
    
    preMatrix = newTransform;
    
    if (!attachedFlag)
        return;
    
    identityMatrix.setIdentity();
    if (newTransform == identityMatrix)
    {
        if (preTransform)
        {
            // Delete the pre-transform matrix
            aboveGroup = preTransform->getParent(0);
            belowGroup = (pfGroup *)(preTransform->getChild(0));
            aboveGroup->removeChild(preTransform);
            preTransform->removeChild(belowGroup);
            aboveGroup->addChild(belowGroup);
            pfDelete(preTransform);
            preTransform = NULL;
        }
    }
    else
    {
        // Yes, the index order is reversed. Performer does its
        // multiplication the opposite of the way we do.
        for (loop = 0; loop < 4; loop++)
            for (sloop = 0; sloop < 4; sloop++)
                performerMatrix[loop][sloop] = newTransform[sloop][loop];
        performerTransform = new pfSCS(performerMatrix);
    
        if (preTransform)
        {
            // Replace the current transform
            aboveGroup = preTransform->getParent(0);
            belowGroup = (pfGroup *)(preTransform->getChild(0));
            aboveGroup->removeChild(preTransform);
            preTransform->removeChild(belowGroup);
            aboveGroup->addChild(performerTransform);
            performerTransform->addChild(belowGroup);
            pfDelete(preTransform);
            preTransform = performerTransform;
        }
        else
        {
            // Insert a new transform
            aboveGroup = componentTop;
            belowGroup = (pfGroup *)(aboveGroup->getChild(0));
            aboveGroup->removeChild(belowGroup);
            aboveGroup->addChild(performerTransform);
            performerTransform->addChild(belowGroup);
            preTransform = performerTransform;
        }
    }
}

// ------------------------------------------------------------------------
// Retrieves the pre-transform matrix
// ------------------------------------------------------------------------
vsMatrix vsTransformAttribute::getPreTransform()
{
    return preMatrix;
}

// ------------------------------------------------------------------------
// Sets the dynamic transform matrix
// ------------------------------------------------------------------------
void vsTransformAttribute::setDynamicTransform(vsMatrix newTransform)
{
    pfDCS *performerTransform;
    vsMatrix identityMatrix;
    pfMatrix performerMatrix;
    pfGroup *aboveGroup, *belowGroup;
    int loop, sloop;
    
    dynMatrix = newTransform;
    
    if (!attachedFlag)
        return;
    
    identityMatrix.setIdentity();
    if (newTransform == identityMatrix)
    {
        if (dynTransform)
        {
            // Delete the dynamic transform matrix
            aboveGroup = dynTransform->getParent(0);
            belowGroup = (pfGroup *)(dynTransform->getChild(0));
            aboveGroup->removeChild(dynTransform);
            dynTransform->removeChild(belowGroup);
            aboveGroup->addChild(belowGroup);
            pfDelete(dynTransform);
            dynTransform = NULL;
        }
    }
    else
    {
        // Yes, the index order is reversed. Performer does its
        // multiplication the opposite of the way we do.
        for (loop = 0; loop < 4; loop++)
            for (sloop = 0; sloop < 4; sloop++)
                performerMatrix[loop][sloop] = newTransform[sloop][loop];

        if (dynTransform)
        {
            // Replace the current transform
            dynTransform->setMat(performerMatrix);
        }
        else
        {
            // Insert a new transform
            if (preTransform)
                aboveGroup = preTransform;
            else
                aboveGroup = componentTop;
            performerTransform = new pfDCS();
            performerTransform->setMat(performerMatrix);
            belowGroup = (pfGroup *)(aboveGroup->getChild(0));
            aboveGroup->removeChild(belowGroup);
            aboveGroup->addChild(performerTransform);
            performerTransform->addChild(belowGroup);
            dynTransform = performerTransform;
        }
    }
}

// ------------------------------------------------------------------------
// Retrieves the dynamic transform matrix
// ------------------------------------------------------------------------
vsMatrix vsTransformAttribute::getDynamicTransform()
{
    return dynMatrix;
}

// ------------------------------------------------------------------------
// Sets the post-transform matrix
// ------------------------------------------------------------------------
void vsTransformAttribute::setPostTransform(vsMatrix newTransform)
{
    pfSCS *performerTransform;
    vsMatrix identityMatrix;
    pfMatrix performerMatrix;
    pfGroup *aboveGroup, *belowGroup;
    int loop, sloop;
    
    postMatrix = newTransform;
    
    if (!attachedFlag)
        return;
    
    identityMatrix.setIdentity();
    if (newTransform == identityMatrix)
    {
        if (postTransform)
        {
            // Delete the post-transform matrix
            aboveGroup = postTransform->getParent(0);
            belowGroup = (pfGroup *)(postTransform->getChild(0));
            aboveGroup->removeChild(postTransform);
            postTransform->removeChild(belowGroup);
            aboveGroup->addChild(belowGroup);
            pfDelete(postTransform);
            postTransform = NULL;
        }
    }
    else
    {
        // Yes, the index order is reversed. Performer does its
        // multiplication the opposite of the way we do.
        for (loop = 0; loop < 4; loop++)
            for (sloop = 0; sloop < 4; sloop++)
                performerMatrix[loop][sloop] = newTransform[sloop][loop];
        performerTransform = new pfSCS(performerMatrix);
    
        if (postTransform)
        {
            // Replace the current transform
            aboveGroup = postTransform->getParent(0);
            belowGroup = (pfGroup *)(postTransform->getChild(0));
            aboveGroup->removeChild(postTransform);
            postTransform->removeChild(belowGroup);
            aboveGroup->addChild(performerTransform);
            performerTransform->addChild(belowGroup);
            pfDelete(postTransform);
            postTransform = performerTransform;
        }
        else
        {
            // Insert a new transform
            if (dynTransform)
                aboveGroup = dynTransform;
            else if (preTransform)
                aboveGroup = preTransform;
            else
                aboveGroup = componentTop;
            belowGroup = (pfGroup *)(aboveGroup->getChild(0));
            aboveGroup->removeChild(belowGroup);
            aboveGroup->addChild(performerTransform);
            performerTransform->addChild(belowGroup);
            postTransform = performerTransform;
        }
    }
}

// ------------------------------------------------------------------------
// Retrieves the post-transform matrix
// ------------------------------------------------------------------------
vsMatrix vsTransformAttribute::getPostTransform()
{
    return postMatrix;
}

// ------------------------------------------------------------------------
// Private function
// "Pushes" the connections to the indicated group down by assigning all of
// the children of that group to a new group and making the new group the
// only child of the original group.
// ------------------------------------------------------------------------
void vsTransformAttribute::pushBottom(pfGroup *splitGroup)
{
    pfGroup *newGroup;
    pfNode *childNode;
    
    newGroup = new pfGroup();
    while (splitGroup->getNumChildren() > 0)
    {
        childNode = splitGroup->getChild(0);
        splitGroup->removeChild(childNode);
        newGroup->addChild(childNode);
    }
    
    splitGroup->addChild(newGroup);
}

// ------------------------------------------------------------------------
// Internal function
// Returns if this attribute is available to be attached to a node
// ------------------------------------------------------------------------
int vsTransformAttribute::canAttach()
{
    if (attachedFlag)
        return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsTransformAttribute::attach(vsNode *theNode)
{
    if (attachedFlag)
    {
        printf("vsTransformAttribute::attach: Attribute is already attached\n");
        return;
    }
    
    if ((theNode->getNodeType() == VS_NODE_TYPE_GEOMETRY) ||
        (theNode->getNodeType() == VS_NODE_TYPE_DYNAMIC_GEOMETRY))
    {
        printf("vsTransformAttribute::attach: Can't attach transform "
            "attributes to geometry nodes\n");
        return;
    }
    
    componentTop = ((vsComponent *)theNode)->getTopGroup();
    
    attachedFlag = 1;
    
    setPreTransform(preMatrix);
    setDynamicTransform(dynMatrix);
    setPostTransform(postMatrix);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsTransformAttribute::detach(vsNode *theNode)
{
    vsMatrix tempPre, tempDyn, tempPost, identityMatrix;

    if (!attachedFlag)
    {
        printf("vsTransformAttribute::detach: Attribute is not attached\n");
        return;
    }

    identityMatrix.setIdentity();
    tempPre = preMatrix;
    tempDyn = dynMatrix;
    tempPost = postMatrix;
    
    setPreTransform(identityMatrix);
    setDynamicTransform(identityMatrix);
    setPostTransform(identityMatrix);
    
    preMatrix = tempPre;
    dynMatrix = tempDyn;
    postMatrix = tempPost;

    componentTop = NULL;
    attachedFlag = 0;
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsTransformAttribute::attachDuplicate(vsNode *theNode)
{
    vsTransformAttribute *newAttrib;
    vsMatrix xformMat;
    
    newAttrib = new vsTransformAttribute();
    
    newAttrib->setPreTransform(getPreTransform());
    newAttrib->setDynamicTransform(getDynamicTransform());
    newAttrib->setPostTransform(getPostTransform());

    theNode->addAttribute(newAttrib);
}