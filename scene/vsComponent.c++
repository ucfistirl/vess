// File vsComponent.c++

#include "vsComponent.h++"

// ------------------------------------------------------------------------
// Default Constructor - Sets up the Performer objects associated with
// this component
// ------------------------------------------------------------------------
vsComponent::vsComponent()
{
    childCount = 0;
    topGroup = new pfGroup();
    lightHook = new pfGroup();
    bottomGroup = new pfGroup();
    topGroup->addChild(lightHook);
    lightHook->addChild(bottomGroup);
    
    ((vsSystem::systemObject)->getNodeMap())->registerLink(this, topGroup);
    
    lightHook->setTravFuncs(PFTRAV_DRAW, preDrawCallback, postDrawCallback);
    lightHook->setTravData(PFTRAV_DRAW, this);
}

// ------------------------------------------------------------------------
// VESS internal function
// Constructor - Creates a VESS component (hierarchy) based on the info
// contained in the given Performer scene graph
// ------------------------------------------------------------------------
vsComponent::vsComponent(pfGroup *targetGraph, vsDatabaseLoader *nameDirectory)
{
    pfNode *currentNode;
    pfGroup *parentGroup, *previousGroup;
    pfGroup *newGroup;
    int loop;
    vsNode *myNode;
    vsTransformAttribute *xformAttrib;
    vsSwitchAttribute *switchAttrib;
    vsSequenceAttribute *seqAttrib;
    vsLODAttribute *detailAttrib;
    vsBillboardAttribute *billAttrib;
    
    handleName(targetGraph, nameDirectory);

    // Set the 'topGroup' group. If the group at the top of the targetGraph
    // matches our needs (must be a pfGroup and not a derived class, no
    // more than one child), we can use that, else create a new top group
    // and push everything else down.
    if ( (targetGraph->isExactType(pfGroup::getClassType())) &&
         (targetGraph->getNumChildren() == 1) )
        topGroup = targetGraph;
    else
    {
        // Add a new 'buffer' group at the top; watch for multiple parents
        topGroup = new pfGroup();
        while (targetGraph->getNumParents() > 0)
        {
            parentGroup = targetGraph->getParent(0);
            parentGroup->replaceChild(targetGraph, topGroup);
        }
        topGroup->addChild(targetGraph);
    }

    // Check for a transform at this point; create a transform attribute
    // if there is one.
    previousGroup = topGroup;
    currentNode = topGroup->getChild(0);
    if ( currentNode->isOfType(pfSCS::getClassType()) &&
         handleName(currentNode, nameDirectory) &&
         (currentNode->getNumParents() < 2) )
    {
        xformAttrib = new vsTransformAttribute((pfSCS *)currentNode,
            this, nameDirectory);
        vsAttributeList::addAttribute(xformAttrib);
        while (currentNode->isOfType(pfSCS::getClassType()))
        {
            previousGroup = (pfGroup *)currentNode;
            currentNode = ((pfGroup *)currentNode)->getChild(0);
        }
    }
    
    // Set the 'lightHook' middle group. Same restrictions as the top group,
    // with added checks for name and multiple parents
    if ( currentNode->isExactType(pfGroup::getClassType()) &&
         (((pfGroup *)currentNode)->getNumChildren() == 1) &&
         (currentNode->getNumParents() < 2) &&
         handleName(currentNode, nameDirectory) )
    {
        lightHook = (pfGroup *)currentNode;
        previousGroup = lightHook;
        currentNode = lightHook->getChild(0);
    }
    else
    {
        // Add a new 'buffer' group
        lightHook = new pfGroup();
        previousGroup->replaceChild(currentNode, lightHook);
        lightHook->addChild(currentNode);
        previousGroup = lightHook;
    }

    // Finally, set the bottom group. Check for type; groups, switches,
    // sequences, and LODs are permitted here. Multiple children are also
    // permitted. Unrecognized pfGroup sub-types also get assigned to the
    // bottom group but might get trampled on if the user subsequently sets
    // a switch, sequence, or LOD. pfNodes, nodes with multiple parents,
    // stray transforms, and important named groups get swept forward into
    // the next component.
    if ( (!(currentNode->isOfType(pfGroup::getClassType()))) ||
         (currentNode->isOfType(pfSCS::getClassType())) ||
         (currentNode->getNumParents() > 1) ||
         (!(handleName(currentNode, nameDirectory))) )
    {
        // Sweep the offending node past the bottom group and into the
        // next component
        bottomGroup = new pfGroup();
        previousGroup->replaceChild(currentNode, bottomGroup);
        bottomGroup->addChild(currentNode);
    }
    else
    {
        if (currentNode->isOfType(pfSwitch::getClassType()))
        {
            // Switch attribute
            switchAttrib = new vsSwitchAttribute((pfSwitch *)currentNode);
            vsAttributeList::addAttribute(switchAttrib);
            bottomGroup = (pfGroup *)currentNode;
        }
        else if (currentNode->isOfType(pfSequence::getClassType()))
        {
            // Sequence attribute
            seqAttrib = new vsSequenceAttribute((pfSequence *)currentNode);
            vsAttributeList::addAttribute(seqAttrib);
            bottomGroup = (pfGroup *)currentNode;
        }
        else if (currentNode->isOfType(pfLOD::getClassType()))
        {
            // LOD attribute
            detailAttrib = new vsLODAttribute((pfLOD *)currentNode);
            vsAttributeList::addAttribute(detailAttrib);
            bottomGroup = (pfGroup *)currentNode;
        }
        else
            bottomGroup = (pfGroup *)currentNode;
    }

    // The component is finished. Register this component with the system,
    // create components (or geometries) for all of the children of this
    // component, and attach everything together.
    
    ((vsSystem::systemObject)->getNodeMap())->registerLink(this, topGroup);

    childCount = 0;
    for (loop = 0; loop < (bottomGroup->getNumChildren()); loop++)
    {
        currentNode = bottomGroup->getChild(loop);
        myNode = (vsNode *)(((vsSystem::systemObject)->getNodeMap())->
            mapSecondToFirst(currentNode));
        
        if (!myNode)
        {
            if (currentNode->isOfType(pfBillboard::getClassType()))
            {
                // * Deal with the billboard here
                
                // First, check to see if the component above has more than
                // one child; billboard attributes must get moved up to
                // their parent components but should not affect the other
                // children of that component
                if (bottomGroup->getNumChildren() > 1)
                {
                    // Add a group between the parent and the billboard; the
                    // rest of the code will take care of changing the group
                    // into a component.
                    newGroup = new pfGroup();
                    bottomGroup->replaceChild(currentNode, newGroup);
                    newGroup->addChild(currentNode);
                    currentNode = newGroup;
                }
                else
                {
                    // Create a billboard attribute from the pfBillboard and
                    // add it to this component
                    billAttrib = new vsBillboardAttribute(
                        (pfBillboard *)currentNode);
                    addAttribute(billAttrib);
                }
            }
        
            if (currentNode->isOfType(pfGroup::getClassType()))
            {
                // Create new component
                myNode = new vsComponent((pfGroup *)currentNode,
                    nameDirectory);
            }
            else if (currentNode->isOfType(pfGeode::getClassType()))
            {
                // Create new geometry
                myNode = new vsGeometry((pfGeode *)currentNode);
            }
        }
        
        // Parent and child are already connected in the Performer scene;
        // make new connections only in our own objects.
        childList[childCount++] = myNode;
        myNode->addParent(this);
    }

    lightHook->setTravFuncs(PFTRAV_DRAW, preDrawCallback, postDrawCallback);
    lightHook->setTravData(PFTRAV_DRAW, this);
}

// ------------------------------------------------------------------------
// Destructor - Disconnects this component from its Performer counterpart
// ------------------------------------------------------------------------
vsComponent::~vsComponent()
{
    ((vsSystem::systemObject)->getNodeMap())->removeLink(this,
	VS_OBJMAP_FIRST_LIST);
}

// ------------------------------------------------------------------------
// Retrieves the type of this node
// ------------------------------------------------------------------------
int vsComponent::getNodeType()
{
    return VS_NODE_TYPE_COMPONENT;
}

// ------------------------------------------------------------------------
// Searches the component and the hierarchy under it for a node with the
// given name
// ------------------------------------------------------------------------
vsNode *vsComponent::findNodeByName(const char *targetName)
{
    int loop;
    vsNode *result;

    if (!strcmp(targetName, getName()))
        return this;

    for (loop = 0; loop < childCount; loop++)
        if (result = (childList[loop])->findNodeByName(targetName))
            return result;

    return NULL;
}

// ------------------------------------------------------------------------
// Adds the given node as a child of this component
// ------------------------------------------------------------------------
void vsComponent::addChild(vsNode *newChild)
{
    vsComponent *childComponent;
    vsGeometry *childGeometry;

    if (childCount >= VS_COMPONENT_MAX_CHILDREN)
    {
        printf("vsComponent::addChild: Child list id full\n");
        return;
    }
    
    // First connect the Performer nodes together
    if (newChild->getNodeType() == VS_NODE_TYPE_COMPONENT)
    {
        childComponent = (vsComponent *)newChild;
        bottomGroup->addChild(childComponent->getBaseLibraryObject());
    }
    else
    {
        childGeometry = (vsGeometry *)newChild;
        bottomGroup->addChild(childGeometry->getBaseLibraryObject());
    }

    // Then make the connection in the VESS nodes
    childList[childCount] = newChild;
    childCount++;
    
    newChild->addParent(this);
}

// ------------------------------------------------------------------------
// Removes the given node from the list of children for this component
// ------------------------------------------------------------------------
void vsComponent::removeChild(vsNode *targetChild)
{
    int loop, sloop;
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    
    for (loop = 0; loop < childCount; loop++)
        if (targetChild == childList[loop])
        {
            if (targetChild->getNodeType() == VS_NODE_TYPE_COMPONENT)
            {
                childComponent = (vsComponent *)targetChild;
                bottomGroup->removeChild(
                    childComponent->getBaseLibraryObject());
            }
            else
            {
                childGeometry = (vsGeometry *)targetChild;
                bottomGroup->removeChild(
                    childGeometry->getBaseLibraryObject());
            }

            for (sloop = loop; sloop < childCount-1; sloop++)
                childList[sloop] = childList[sloop+1];
        
            childCount--;
            return;
        }
}

// ------------------------------------------------------------------------
// Retrieves the number of child nodes attached to this component
// ------------------------------------------------------------------------
int vsComponent::getChildCount()
{
    return childCount;
}

// ------------------------------------------------------------------------
// Retrieves the child with the given index from this component. The index
// of the first child is 0.
// ------------------------------------------------------------------------
vsNode *vsComponent::getChild(int index)
{
    if ((index < 0) || (index >= childCount))
    {
        printf("vsComponent::getChild: Bad child index\n");
        return NULL;
    }
    
    return childList[index];
}

// ------------------------------------------------------------------------
// Retrieves the center point and radius of a sphere that encompasses all
// of the geometry within this object.
// ------------------------------------------------------------------------
void vsComponent::getBoundSphere(vsVector *centerPoint, double *radius)
{
    pfSphere boundSphere;
    
    topGroup->getBound(&boundSphere);
    
    if (centerPoint)
	centerPoint->set(boundSphere.center[PF_X], boundSphere.center[PF_Y],
	    boundSphere.center[PF_Z]);

    if (radius)
	*radius = boundSphere.radius;
}

// ------------------------------------------------------------------------
// Attempts to add the given attribute to the component's list of
// attributes. If successful, also notifies the attribute that it has been
// added to a component.
// ------------------------------------------------------------------------
void vsComponent::addAttribute(vsAttribute *newAttribute)
{
    int attributeType;
    int loop;

    if (newAttribute->isAttached())
    {
        printf("vsComponent::addAttribute: Attribute is already in use\n");
        return;
    }
    
    attributeType = newAttribute->getAttributeType();
    switch (attributeType)
    {
        // Component may contain many lights
        case VS_ATTRIBUTE_TYPE_LIGHT:
            break;
        // Component may only contain one of each of these
        case VS_ATTRIBUTE_TYPE_TRANSFORM:
        case VS_ATTRIBUTE_TYPE_FOG:
        case VS_ATTRIBUTE_TYPE_MATERIAL:
        case VS_ATTRIBUTE_TYPE_TEXTURE:
        case VS_ATTRIBUTE_TYPE_TRANSPARENCY:
        case VS_ATTRIBUTE_TYPE_BILLBOARD:
        case VS_ATTRIBUTE_TYPE_BACKFACE:
            for (loop = 0; loop < getAttributeCount(); loop++)
                if ((getAttribute(loop))->getAttributeType() == attributeType)
                {
                    printf("vsComponent::addAttribute: Component may only "
                        "contain one attribute of the type of the given "
                        "attribute\n");
                    return;
                }
            break;
        // Component may only contain one of any of these
        case VS_ATTRIBUTE_TYPE_SWITCH:
        case VS_ATTRIBUTE_TYPE_SEQUENCE:
        case VS_ATTRIBUTE_TYPE_LOD:
            for (loop = 0; loop < getAttributeCount(); loop++)
            {
                attributeType = (getAttribute(loop))->getAttributeType();
                if ((attributeType == VS_ATTRIBUTE_TYPE_SWITCH) ||
                    (attributeType == VS_ATTRIBUTE_TYPE_SEQUENCE) ||
                    (attributeType == VS_ATTRIBUTE_TYPE_LOD))
                {
                    printf("vsComponent::addAttribute: Component may only "
                        "contain one of vsSwitchAttribute, "
                        "vsSequenceAttribute, or vsLODAttribute at any one "
                        "time\n");
                    return;
                }
            }
            break;
    }

    // If we made it this far, it must be okay to add the attribute in
    vsAttributeList::addAttribute(newAttribute);
    newAttribute->attach(this);
}

// ------------------------------------------------------------------------
// Removes the specified attribute from the component's list of attributes,
// and notifies the attribute that it has been removed.
// ------------------------------------------------------------------------
void vsComponent::removeAttribute(vsAttribute *targetAttribute)
{
    targetAttribute->detach(this);
    vsAttributeList::removeAttribute(targetAttribute);
}

// ------------------------------------------------------------------------
// Returns the Performer object associated with this object
// ------------------------------------------------------------------------
pfGroup *vsComponent::getBaseLibraryObject()
{
    return topGroup;
}

// ------------------------------------------------------------------------
// VESS internal function
// Retrieves the topmost Performer group associated with this component
// ------------------------------------------------------------------------
pfGroup *vsComponent::getTopGroup()
{
    return topGroup;
}

// ------------------------------------------------------------------------
// VESS internal function
// Retrieves the center Performer group associated with this component
// ------------------------------------------------------------------------
pfGroup *vsComponent::getLightHook()
{
    return lightHook;
}

// ------------------------------------------------------------------------
// VESS internal function
// Retrieves the bottommost Performer group associated with this component
// ------------------------------------------------------------------------
pfGroup *vsComponent::getBottomGroup()
{
    return bottomGroup;
}

// ------------------------------------------------------------------------
// VESS internal function
// Sets the bottommost Performer group associated with this component
// ------------------------------------------------------------------------
void vsComponent::setBottomGroup(pfGroup *newBottom)
{
    bottomGroup = newBottom;
}

// ------------------------------------------------------------------------
// VESS internal function
// Checks to see if a node name is important enough to merit getting its
// own component during the VESS graph construction process. May set the
// component's name if it does not already have one.
// ------------------------------------------------------------------------
int vsComponent::handleName(pfNode *targetNode, vsDatabaseLoader *nameDirectory)
{
    if (targetNode->getName() == NULL)
        return VS_TRUE;

    if (!strcmp(targetNode->getName(), getName()))
        return VS_TRUE;

    if (!(nameDirectory->checkName(targetNode->getName())))
        return VS_TRUE;

    if (getName()[0] == 0)
    {
        setName(targetNode->getName());
        return VS_TRUE;
    }

    return VS_FALSE;
}

// ------------------------------------------------------------------------
// VESS internal function
// Replaces the bottommost Performer group of the component with the
// indicated group. Used to enable the operation of certain attributes.
// ------------------------------------------------------------------------
void vsComponent::replaceBottomGroup(pfGroup *newGroup)
{
    pfNode *childNode;
    pfGroup *parentGroup;

    while (bottomGroup->getNumChildren() > 0)
    {
        childNode = bottomGroup->getChild(0);
        bottomGroup->removeChild(childNode);
        newGroup->addChild(childNode);
    }

    parentGroup = bottomGroup->getParent(0);
    parentGroup->replaceChild(bottomGroup, newGroup);
    
    pfDelete(bottomGroup);
    bottomGroup = newGroup;
}
