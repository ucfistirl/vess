// File vsComponent.c++

#include "vsComponent.h++"

#include "vsTransformAttribute.h++"
#include "vsSwitchAttribute.h++"
#include "vsSequenceAttribute.h++"
#include "vsLODAttribute.h++"
#include "vsDecalAttribute.h++"
#include "vsBillboardAttribute.h++"
#include "vsGeometry.h++"
#include "vsSystem.h++"

// ------------------------------------------------------------------------
// Default Constructor - Sets up the Performer objects associated with
// this component
// ------------------------------------------------------------------------
vsComponent::vsComponent() : childList(32, 32)
{
    // Create the Performer group objects and tie them together
    topGroup = new pfGroup();
    topGroup->ref();
    lightHook = new pfGroup();
    lightHook->ref();
    bottomGroup = new pfGroup();
    bottomGroup->ref();
    topGroup->addChild(lightHook);
    lightHook->addChild(bottomGroup);

    childCount = 0;
    ((vsSystem::systemObject)->getNodeMap())->registerLink(this, topGroup);
}

// ------------------------------------------------------------------------
// VESS internal function
// Constructor - Creates a VESS component (hierarchy) based on the info
// contained in the given Performer scene graph
// ------------------------------------------------------------------------
vsComponent::vsComponent(pfGroup *targetGraph, vsDatabaseLoader *nameDirectory)
    : childList(32, 32)
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
    vsDecalAttribute *decalAttrib;
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
    // a switch, sequence, LOD, or decal. pfNodes, nodes with multiple parents,
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
        else if (currentNode->isOfType(pfLayer::getClassType()))
        {
            // Decal attribute
            decalAttrib = new vsDecalAttribute((pfLayer *)currentNode);
            vsAttributeList::addAttribute(decalAttrib);
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

    topGroup->ref();
    lightHook->ref();
    bottomGroup->ref();
}

// ------------------------------------------------------------------------
// Destructor - Disconnects this component from its Performer counterpart.
// Also removes all attributes, destroying those that aren't in use
// somewhere else. Additionally, removes all remaining children, though
// it does not delete any of them.
// ------------------------------------------------------------------------
vsComponent::~vsComponent()
{
    vsAttribute *attr;
    vsNode *child;
    vsComponent *parent;

    // Remove all attached attributes; destroy those that aren't being
    // used by other nodes.
    while (getAttributeCount() > 0)
    {
        attr = getAttribute(0);
        removeAttribute(attr);
        if (!(attr->isAttached()))
            delete attr;
    }
    
    // Remove this node from its parents
    while (getParentCount() > 0)
    {
        parent = getParent(0);
        parent->removeChild(this);
    }

    // Detach all remaining children; don't delete any.
    while (getChildCount() > 0)
    {
        child = getChild(0);
        removeChild(child);
    }

    ((vsSystem::systemObject)->getNodeMap())->removeLink(this,
        VS_OBJMAP_FIRST_LIST);


    // Unlink and destroy the Performer objects
    topGroup->removeChild(lightHook);
    lightHook->removeChild(bottomGroup);
    topGroup->unref();
    pfDelete(topGroup);
    lightHook->unref();
    pfDelete(lightHook);
    bottomGroup->unref();
    pfDelete(bottomGroup);
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
        if (result = ((vsNode *)(childList[loop]))->findNodeByName(targetName))
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
    childList[childCount++] = newChild;
    
    newChild->addParent(this);
    
    // Finally, mark the entire tree above and below this node as needing
    // of an update
    newChild->dirty();
}

// ------------------------------------------------------------------------
// Adds the given node as a child of this component, at the given index in
// the component's child list. All children currently in the list at that
// index or greater are moved over by one.
// ------------------------------------------------------------------------
void vsComponent::insertChild(vsNode *newChild, int index)
{
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    int loop;
    
    if (index < 0)
    {
        printf("vsComponent::insertChild: Index out of bounds\n");
        return;
    }

    // If the index is greater than the current number of children on this
    // component, simply add the new child on the end normally
    if (index >= childCount)
    {
        addChild(newChild);
        return;
    }

    // First connect the Performer nodes together
    if (newChild->getNodeType() == VS_NODE_TYPE_COMPONENT)
    {
        childComponent = (vsComponent *)newChild;
        bottomGroup->insertChild(index, childComponent->getBaseLibraryObject());
    }
    else
    {
        childGeometry = (vsGeometry *)newChild;
        bottomGroup->insertChild(index, childGeometry->getBaseLibraryObject());
    }

    // Then make the connection in the VESS nodes
    for (loop = childCount; loop > index; loop--)
        childList[loop] = childList[loop-1];
    childList[index] = newChild;
    childCount++;
    
    newChild->addParent(this);
    
    // Finally, mark the entire tree above and below this node as needing
    // of an update
    newChild->dirty();
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
            // Mark the entire portion of the tree that has any connection
            // to this node as needing of an update
            targetChild->dirty();
        
            // Detach the Performer nodes; checks for the type of the
            // component because the getBaseLibraryObject call is
            // not virtual.
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

            // 'Slide' the rest of the children down to fill in the gap
            for (sloop = loop; sloop < childCount-1; sloop++)
                childList[sloop] = childList[sloop+1];
        
            // Finish the VESS detachment
            childCount--;
            targetChild->removeParent(this);
            return;
        }
}

// ------------------------------------------------------------------------
// Replaces the target node with the new node in the list of children for
// this component. The new node occupies the same idex that the previous
// node did.
// ------------------------------------------------------------------------
void vsComponent::replaceChild(vsNode *targetChild, vsNode *newChild)
{
    int loop;
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    pfNode *oldNode, *newNode;
    
    for (loop = 0; loop < childCount; loop++)
        if (targetChild == childList[loop])
        {
            // Mark the entire portion of the tree that has any connection
            // to the old node as needing of an update
            targetChild->dirty();
        
            // Replace the Performer nodes; checks for the type of the
            // component because the getBaseLibraryObject call is
            // not virtual.
            if (targetChild->getNodeType() == VS_NODE_TYPE_COMPONENT)
            {
                childComponent = (vsComponent *)targetChild;
                oldNode = childComponent->getBaseLibraryObject();
            }
            else
            {
                childGeometry = (vsGeometry *)targetChild;
                oldNode = childGeometry->getBaseLibraryObject();
            }

            if (newChild->getNodeType() == VS_NODE_TYPE_COMPONENT)
            {
                childComponent = (vsComponent *)newChild;
                newNode = childComponent->getBaseLibraryObject();
            }
            else
            {
                childGeometry = (vsGeometry *)newChild;
                newNode = childGeometry->getBaseLibraryObject();
            }
            
            bottomGroup->replaceChild(oldNode, newNode);
            
            // Change the connection in the VESS nodes
            childList[loop] = newChild;

            targetChild->removeParent(this);
            newChild->addParent(this);
            
            // Mark the entire portion of the tree that has any connection
            // to the new node as needing of an update
            newChild->dirty();
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
    
    return (vsNode *)(childList[index]);
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
// Computes the global coordinate transform at this component by
// multiplying together all of the transforms at nodes at and above this
// one.
// ------------------------------------------------------------------------
vsMatrix vsComponent::getGlobalXform()
{
    pfNode *nodePtr;
    pfMatrix xform;
    const pfMatrix *scsMatPtr;
    vsMatrix result;
    int loop, sloop;

    xform.makeIdent();
    nodePtr = bottomGroup;
    
    while (nodePtr->getNumParents() > 0)
    {
        if (nodePtr->isOfType(pfSCS::getClassType()))
        {
            scsMatPtr = ((pfSCS *)nodePtr)->getMatPtr();
            xform.postMult(*scsMatPtr);
        }
        
        nodePtr = nodePtr->getParent(0);
    }
    
    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            result[loop][sloop] = xform[sloop][loop];

    return result;
}

// ------------------------------------------------------------------------
// Sets the intersection mask for this component. During an intersection
// run, at each component a bitwise AND of the intersection's mask and the
// component's mask is performed; if the result of the AND is zero, the
// intersection ignores this component and all of its children.
// ------------------------------------------------------------------------
void vsComponent::setIntersectMask(unsigned int newMask)
{
    topGroup->setTravMask(PFTRAV_ISECT, newMask, PFTRAV_SELF, PF_SET);
}

// ------------------------------------------------------------------------
// Retrieves the intersection mask for this component.
// ------------------------------------------------------------------------
unsigned int vsComponent::getIntersectMask()
{
    return (topGroup->getTravMask(PFTRAV_ISECT));
}

// ------------------------------------------------------------------------
// Attempts to add the given attribute to the component's list of
// attributes. If successful, also notifies the attribute that it has been
// added to a component.
// ------------------------------------------------------------------------
void vsComponent::addAttribute(vsAttribute *newAttribute)
{
    int attrType, attrCat;
    int loop;

    // Ask the attribute if it's willing to be added; if it returns false,
    // it's probably already attached somewhere else.
    if (!(newAttribute->canAttach()))
    {
        printf("vsComponent::addAttribute: Attribute is already in use\n");
        return;
    }
    
    attrCat = newAttribute->getAttributeCategory();
    attrType = newAttribute->getAttributeType();
    switch (attrCat)
    {
        // Component may only contain one of each of these
        case VS_ATTRIBUTE_CATEGORY_STATE:
        case VS_ATTRIBUTE_CATEGORY_XFORM:
            for (loop = 0; loop < getAttributeCount(); loop++)
                if ((getAttribute(loop))->getAttributeType() == attrType)
                {
                    printf("vsComponent::addAttribute: Component already "
                        "contains that type of attribute\n");
                    return;
                }
            break;

        // Component may only contain one of any of these
        case VS_ATTRIBUTE_CATEGORY_GROUPING:
            if (getCategoryAttribute(VS_ATTRIBUTE_CATEGORY_GROUPING, 0))
            {
                printf("vsComponent::addAttribute: Component may only "
                    "contain one grouping category attribute at a time\n");
                return;
            }
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
// component's name if it does not already have one. Returns VS_TRUE if the
// node can be safely encapsulated, VS_FALSE if it requires special
// attention.
// ------------------------------------------------------------------------
int vsComponent::handleName(pfNode *targetNode, vsDatabaseLoader *nameDirectory)
{
    // If the node in question doesn't have a name, we're safe.
    if (targetNode->getName() == NULL)
        return VS_TRUE;

    // The node in question has a name, but this component has the
    // same name; we're safe.
    if (!strcmp(targetNode->getName(), getName()))
        return VS_TRUE;

    // If the node isn't considered important, either by its name or
    // that it's an unimportant DCS, then we're safe.
    if (!(nameDirectory->importanceCheck(targetNode)))
        return VS_TRUE;

    // The node must be important, but this component doesn't have a
    // name yet; we're off the hook. Assign the node's name to
    // this component and return that we're safe.
    if (getName()[0] == 0)
    {
        setName(targetNode->getName());
        return VS_TRUE;
    }

    // The node in question is important, but this component already
    // has something else important; fail.
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

    // Move the children of the current bottomGroup to the newGroup
    while (bottomGroup->getNumChildren() > 0)
    {
        childNode = bottomGroup->getChild(0);
        bottomGroup->removeChild(childNode);
        newGroup->addChild(childNode);
    }

    // Replace bottomGroup with newGroup
    parentGroup = bottomGroup->getParent(0);
    parentGroup->replaceChild(bottomGroup, newGroup);
    
    bottomGroup->unref();
    pfDelete(bottomGroup);
    bottomGroup = newGroup;
    bottomGroup->ref();
}

// ------------------------------------------------------------------------
// VESS internal function
// Marks this node and each node below this one as dirty
// ------------------------------------------------------------------------
void vsComponent::dirtyDown()
{
    int loop;
    
    dirtyFlag = VS_TRUE;
    
    for (loop = 0; loop < childCount; loop++)
        ((vsNode *)(childList[loop]))->dirtyDown();
}
