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
    // Initialize the parent component pointer to NULL
    componentTop = NULL;

    // Initialize the three pfDCS pointers to NULL
    preTransform = NULL;
    dynTransform = NULL;
    postTransform = NULL;
    
    // Set the three transform matrices to identity
    preMatrix.setIdentity();
    dynMatrix.setIdentity();
    postMatrix.setIdentity();
}

// ------------------------------------------------------------------------
// VESS internal function
// Constructor - Attempts to set up the transform attribute to conform to
// the Performer scene graph rooted at the specified SCS. May modify the
// scene graph to cause it to conform to VESS's idea of the Performer scene
// that should be associated with a vsComponent. 
// ------------------------------------------------------------------------
vsTransformAttribute::vsTransformAttribute(pfSCS *transformGroup,
                                           vsComponent *targetComponent,
                                           vsDatabaseLoader *nameDirectory)
{
    // A vsTransformAttribute encompasses a pfSCS-pfDCS-pfSCS triplet.
    // This constructor attempts to manipulate the Performer scene to
    // have the transforms in that order, with an ordinary pfGroup marking
    // the end of the transform attribute's territory.

    pfGroup *previousGroup = NULL;
    pfGroup *newGroup;
    pfNode *currentNode;
    int loop, sloop;
    pfMatrix scsMatrix;

    // Store the pointer to the parent component
    componentTop = targetComponent->getTopGroup();

    // Initialize the three pfDCS pointers to NULL
    preTransform = NULL;
    dynTransform = NULL;
    postTransform = NULL;

    // When using the conversion constructor, the attribute starts
    // off attached
    attachedFlag = 1;

    // Set the three transform matrices to identity
    preMatrix.setIdentity();
    dynMatrix.setIdentity();
    postMatrix.setIdentity();

    // * First group: static transform
    // We know it's an SCS already so just check to make sure it's not a DCS
    if (!(transformGroup->isOfType(pfDCS::getClassType())))
    {
	// If this SCS has something other than one child, create a new
	// group below it and move its children there
        if (transformGroup->getNumChildren() != 1)
            pushBottom(transformGroup);

	// Retrieve the SCS's transform
        preTransform = transformGroup;
        preTransform->getMat(scsMatrix);

        // Copy the transform matrix from the pfSCS to our matrix; the
	// index order is reversed when going from Performer to VESS.
        for (loop = 0; loop < 4; loop++)
            for (sloop = 0; sloop < 4; sloop++)
                preMatrix[loop][sloop] = scsMatrix[sloop][loop];

	// Set the group that we next have interest in to the child of
	// the current group, and the previous group to the one we
	// just finished looking at.
        previousGroup = transformGroup;
        currentNode = transformGroup->getChild(0);
    }
    else
    {
	// It's a DCS...  wait for the next section of code (dynamic
	// transform) to handle it; set the 'next interesting group'
	// pointer to it.
        currentNode = transformGroup;
    }

    // Break at multiple parents; this won't happen if currentNode is
    // transformGroup, as multiple parents were already checked for before
    // this constructor was called.
    if (currentNode->getNumParents() > 1)
    {
	// If there are multiple parents here, then this node is
	// unsuitable for the middle of a component; 'push' the offending
	// node down by creating a new group between the last one and
	// the multiple-parent one, and return to let the component
	// constructor handle the problem.
        newGroup = new pfGroup();
        previousGroup->replaceChild(currentNode, newGroup);
        newGroup->addChild(currentNode);
        return;
    }

    // * Second group: dynamic transform
    // Check if the group is a pfDCS, and if it's name isn't important
    if ( (currentNode->isOfType(pfDCS::getClassType())) &&
         (targetComponent->handleName(currentNode, nameDirectory)) )
    {
	// If this DCS has something other than one child, create a new
	// group below it and move its children there
        if (((pfGroup *)currentNode)->getNumChildren() != 1)
            pushBottom((pfGroup *)currentNode);

	// Retrieve the DCS's transform
        dynTransform = (pfDCS *)currentNode;
        dynTransform->getMat(scsMatrix);

        // Copy the transform matrix from the pfDCS to our matrix; the
	// index order is reversed when going from Performer to VESS.
        for (loop = 0; loop < 4; loop++)
            for (sloop = 0; sloop < 4; sloop++)
                dynMatrix[loop][sloop] = scsMatrix[sloop][loop];

	// Set the group that we next have interest in to the child of
	// the current group, and the previous group to the one we
	// just finished looking at.
        previousGroup = (pfGroup *)currentNode;
        currentNode = previousGroup->getChild(0);
    }

    // Break at multiple parents
    if (currentNode->getNumParents() > 1)
    {
	// If there are multiple parents here, then this node is
	// unsuitable for the middle of a component; 'push' the offending
	// node down by creating a new group between the last one and
	// the multiple-parent one, and return to let the component
	// constructor handle the problem.
        newGroup = new pfGroup();
        previousGroup->replaceChild(currentNode, newGroup);
        newGroup->addChild(currentNode);
        return;
    }

    // * Third group: static transform (post)
    // Check that the group is a pfSCS but not a pfDCS, and that it's
    // name isn't important
    if ( (currentNode->isOfType(pfSCS::getClassType())) &&
         (!(currentNode->isOfType(pfDCS::getClassType()))) &&
         (targetComponent->handleName(currentNode, nameDirectory)) )
    {
	// If this SCS has something other than one child, create a new
	// group below it and move its children there
        if (((pfGroup *)currentNode)->getNumChildren() != 1)
            pushBottom((pfGroup *)currentNode);

	// Retrieve the SCS's transform
        postTransform = (pfSCS *)currentNode;
        postTransform->getMat(scsMatrix);

        // Copy the transform matrix from the pfSCS to our matrix; the
	// index order is reversed when going from Performer to VESS.
        for (loop = 0; loop < 4; loop++)
            for (sloop = 0; sloop < 4; sloop++)
                postMatrix[loop][sloop] = scsMatrix[sloop][loop];

	// Set the group that we next have interest in to the child of
	// the current group, and the previous group to the one we
	// just finished looking at.
        previousGroup = (pfGroup *)currentNode;
        currentNode = previousGroup->getChild(0);
    }
    
    // Clean up: If we're still standing on a SCS (or DCS) after all of that,
    // push it forward so the component builder doesn't mistake it for part
    // of this transform attribute.
    if (currentNode->isOfType(pfSCS::getClassType()))
    {
        // Create a new pfGroup between the last pfSCS we worked with
	// and this newly-discovered one
        newGroup = new pfGroup();
        previousGroup->replaceChild(currentNode, newGroup);
        newGroup->addChild(currentNode);
    }
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsTransformAttribute::~vsTransformAttribute()
{
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
    
    // Copy the transform matrix
    preMatrix = newTransform;
    
    // If we're not atatched, there no other work to do
    if (!attachedFlag)
        return;
    
    // Set the working identity matrix
    identityMatrix.setIdentity();

    // Check if we're trying to set this matrix to identity or not
    if (newTransform == identityMatrix)
    {
        // Check if the preTransform matrix currently exists; get rid
	// of it if it does.
        if (preTransform)
        {
	    // We don't want an identity matrix in the scene graph,
	    // because it doesn't contribute anything; remove it from
	    // between its parent and child groups and delete it.
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

        // Create a new pfSCS to hold the transform
        performerTransform = new pfSCS(performerMatrix);
    
        // Check if there is already a pfSCS in the preTransform position
        if (preTransform)
        {
            // Replace the current transform by setting the parent of
	    // the current transform to point to the new one instead, 
	    // setting the new transform to point to the current transform's
	    // child, destroying the current transform, and setting the
	    // new transform as the current one.
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
            // Insert a new transform between the component's topGroup
	    // and whatever that group's child is.
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
    
    // Copy the transform matrix
    dynMatrix = newTransform;
    
    // If we're not atatched, there no other work to do
    if (!attachedFlag)
        return;
    
    // Set the working identity matrix
    identityMatrix.setIdentity();

    // Check if we're trying to set this matrix to identity or not
    if (newTransform == identityMatrix)
    {
        // Check if the dynTransform matrix currently exists; get rid
	// of it if it does.
        if (dynTransform)
        {
	    // We don't want an identity matrix in the scene graph,
	    // because it doesn't contribute anything; remove it from
	    // between its parent and child groups and delete it.
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

        // Check if there is already a pfDCS in the dynTransform position
        if (dynTransform)
        {
            // Replace the current transform matrix in the pfDCS
            dynTransform->setMat(performerMatrix);
        }
        else
        {
            // Insert a new transform by figuring out which group should
	    // be the parent (the preTransform, if it exists, else the
	    // topGroup), and adding a pfDCS as a child of that group,
	    // setting the current child of that group to be a child
	    // of the pfDCS.
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
    
    // Copy the transform matrix
    postMatrix = newTransform;
    
    // If we're not atatched, there no other work to do
    if (!attachedFlag)
        return;
    
    // Set the working identity matrix
    identityMatrix.setIdentity();

    // Check if we're trying to set this matrix to identity or not
    if (newTransform == identityMatrix)
    {
        // Check if the postTransform matrix currently exists; get rid
	// of it if it does.
        if (postTransform)
        {
	    // We don't want an identity matrix in the scene graph,
	    // because it doesn't contribute anything; remove it from
	    // between its parent and child groups and delete it.
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

        // Create a new pfSCS to hold the transform
        performerTransform = new pfSCS(performerMatrix);
    
        // Check if there is already a pfSCS in the preTransform position
        if (postTransform)
        {
            // Replace the current transform by setting the parent of
	    // the current transform to point to the new one instead, 
	    // setting the new transform to point to the current transform's
	    // child, destroying the current transform, and setting the
	    // new transform as the current one.
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
            // Insert a new transform by figuring out which group should
	    // be the parent (the dynTransform, if it exists, else the
	    // preTransform, if that exists, otherwise the topGroup), and
	    // adding a pfDCS as a child of that group, setting the current
	    // child of that group to be a child of the pfDCS.
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
    
    // Create a new group
    newGroup = new pfGroup();

    // Move all of the children of splitGroup to the new group
    while (splitGroup->getNumChildren() > 0)
    {
        // Move the first child of the splitGroup to the new group
        childNode = splitGroup->getChild(0);
        splitGroup->removeChild(childNode);
        newGroup->addChild(childNode);
    }
    
    // Add the new group as the only child of the splitGroup
    splitGroup->addChild(newGroup);
}

// ------------------------------------------------------------------------
// VESS internal function
// Returns if this attribute is available to be attached to a node
// ------------------------------------------------------------------------
int vsTransformAttribute::canAttach()
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
void vsTransformAttribute::attach(vsNode *theNode)
{
    // Verify that we're not already attached to something
    if (attachedFlag)
    {
        printf("vsTransformAttribute::attach: Attribute is already attached\n");
        return;
    }
    
    // Transform attributes may not be attached to geometry nodes
    if ((theNode->getNodeType() == VS_NODE_TYPE_GEOMETRY) ||
        (theNode->getNodeType() == VS_NODE_TYPE_DYNAMIC_GEOMETRY))
    {
        printf("vsTransformAttribute::attach: Can't attach transform "
            "attributes to geometry nodes\n");
        return;
    }
    
    // Store a pointer to the topGroup of the component
    componentTop = ((vsComponent *)theNode)->getTopGroup();
    
    // Mark this attribute as attached
    attachedFlag = 1;
    
    // Create the transform nodes on the Performer scene
    setPreTransform(preMatrix);
    setDynamicTransform(dynMatrix);
    setPostTransform(postMatrix);
}

// ------------------------------------------------------------------------
// VESS internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsTransformAttribute::detach(vsNode *theNode)
{
    vsMatrix tempPre, tempDyn, tempPost, identityMatrix;

    // Can't detach an attribute that is not attached
    if (!attachedFlag)
    {
        printf("vsTransformAttribute::detach: Attribute is not attached\n");
        return;
    }

    // To utilize code already written, the detachment process sets each
    // matrix to an identity matrix, which causes the various set matrix
    // functions to remove the transform nodes. The actual matrix data
    // should be retained, though, and so is stored until after the
    // transforms are removed.

    identityMatrix.setIdentity();

    // Copy the current matrix data
    tempPre = preMatrix;
    tempDyn = dynMatrix;
    tempPost = postMatrix;
    
    // Set the transforms to identity, which should automatically
    // remove the Performer transforms from the component
    setPreTransform(identityMatrix);
    setDynamicTransform(identityMatrix);
    setPostTransform(identityMatrix);
    
    // Put the matrix data back, as setting the transforms to identity
    // also set the three matrices to identity
    preMatrix = tempPre;
    dynMatrix = tempDyn;
    postMatrix = tempPost;

    // Clear the component top group pointer, and mark this attribute
    // as unattached
    componentTop = NULL;
    attachedFlag = 0;
}

// ------------------------------------------------------------------------
// VESS internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsTransformAttribute::attachDuplicate(vsNode *theNode)
{
    vsTransformAttribute *newAttrib;
    vsMatrix xformMat;
    
    // Create a duplicate transform attribute
    newAttrib = new vsTransformAttribute();
    
    // Copy the matrix data
    newAttrib->setPreTransform(getPreTransform());
    newAttrib->setDynamicTransform(getDynamicTransform());
    newAttrib->setPostTransform(getPostTransform());

    // Attach the duplicate attribute to the specified node
    theNode->addAttribute(newAttrib);
}
