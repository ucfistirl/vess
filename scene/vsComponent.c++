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
//    VESS Module:  vsComponent.c++
//
//    Description:  vsNode subclass that acts as a non-leaf part of a
//                  VESS scene graph
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsComponent.h++"

#include "vsTransformAttribute.h++"
#include "vsSwitchAttribute.h++"
#include "vsSequenceAttribute.h++"
#include "vsLODAttribute.h++"
#include "vsDecalAttribute.h++"
#include "vsBillboardAttribute.h++"
#include "vsGeometry.h++"
#include "vsDynamicGeometry.h++"
#include "vsSystem.h++"

// ------------------------------------------------------------------------
// Default Constructor - Sets up the Performer objects associated with
// this component
// ------------------------------------------------------------------------
vsComponent::vsComponent() : childList(32, 32)
{
    // Create the Performer group objects and tie them together
    // (topGroup) -> (lightHook) -> (bottomGroup)
    topGroup = new pfGroup();
    topGroup->ref();
    lightHook = new pfGroup();
    lightHook->ref();
    bottomGroup = new pfGroup();
    bottomGroup->ref();
    topGroup->addChild(lightHook);
    lightHook->addChild(bottomGroup);

    childCount = 0;

    // Register a connection between this node and its Performer node
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

    // Call a helper function to copy the Performer node's name to this
    // node if that name is conspdered important
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
            // Replace the target group with a new one in the parent's
	    // child list
            parentGroup = targetGraph->getParent(0);
            parentGroup->replaceChild(targetGraph, topGroup);
        }

        // Add the target group as a child of the new group
        topGroup->addChild(targetGraph);
    }

    // Check for a transform at this point; create a transform attribute
    // if there is one.
    previousGroup = topGroup;
    currentNode = topGroup->getChild(0);

    // Besides being a transform, the target node must not be important
    // if a previous node was (limit one important node per VESS node),
    // and must have no more than one parent
    if ( currentNode->isOfType(pfSCS::getClassType()) &&
         handleName(currentNode, nameDirectory) &&
         (currentNode->getNumParents() < 2) )
    {
        // Call the transform attribute's copy constructor to manipulate
	// the node at an below the target node into the shape that
	// a transform attribute likes
        xformAttrib = new vsTransformAttribute((pfSCS *)currentNode,
            this, nameDirectory);

        // Add the transform attribute to this component without going
	// through the usual attach mechanism
        attributeList[attributeCount++] = xformAttrib;
        xformAttrib->ref();
        
        // Step past all pfSCS (or derivative classes) nodes in our path,
	// under the assumption that the transform attribute has dealt
	// with them already
        while (currentNode->isOfType(pfSCS::getClassType()))
        {
            previousGroup = (pfGroup *)currentNode;
            currentNode = ((pfGroup *)currentNode)->getChild(0);
        }
    }
    
    // Set the 'lightHook' middle group. Same restrictions as the top group,
    // with added checks for name, multiple parents, and multiple children
    if ( currentNode->isExactType(pfGroup::getClassType()) &&
         (((pfGroup *)currentNode)->getNumChildren() == 1) &&
         (currentNode->getNumParents() < 2) &&
         handleName(currentNode, nameDirectory) )
    {
        // Store the lightHook group pointer, and set our focus to
	// the next node down
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
	// Create a new node between the last one and the one we're currently
	// looking at. This effectively 'pushes' the unwanted node out of
	// this component and into unprocessed space so that the next
	// iteration of the conversion process will examine the node again
	// when it's better prepared to handle it.
        bottomGroup = new pfGroup();
        previousGroup->replaceChild(currentNode, bottomGroup);
        bottomGroup->addChild(currentNode);
    }
    else
    {
        // Check for grouping-category attribute stuff
        if (currentNode->isOfType(pfSwitch::getClassType()))
        {
            // Switch attribute
            switchAttrib = new vsSwitchAttribute((pfSwitch *)currentNode);
            attributeList[attributeCount++] = switchAttrib;
            switchAttrib->ref();

            // Store the current node at the bottom group of the component
            bottomGroup = (pfGroup *)currentNode;
        }
        else if (currentNode->isOfType(pfSequence::getClassType()))
        {
            // Sequence attribute
            seqAttrib = new vsSequenceAttribute((pfSequence *)currentNode);
            attributeList[attributeCount++] = seqAttrib;
            seqAttrib->ref();

            // Store the current node at the bottom group of the component
            bottomGroup = (pfGroup *)currentNode;
        }
        else if (currentNode->isOfType(pfLOD::getClassType()))
        {
            // LOD attribute
            detailAttrib = new vsLODAttribute((pfLOD *)currentNode);
            attributeList[attributeCount++] = detailAttrib;
            detailAttrib->ref();

            // Store the current node at the bottom group of the component
            bottomGroup = (pfGroup *)currentNode;
        }
        else if (currentNode->isOfType(pfLayer::getClassType()))
        {
            // Decal attribute
            decalAttrib = new vsDecalAttribute((pfLayer *)currentNode);
            attributeList[attributeCount++] = decalAttrib;
            decalAttrib->ref();

            // Store the current node at the bottom group of the component
            bottomGroup = (pfGroup *)currentNode;
        }
        else
        {
            // Store the current node at the bottom group of the component
            bottomGroup = (pfGroup *)currentNode;
        }
    }

    // The component is finished. Register this component with the system,
    // create components (or geometries) for all of the children of this
    // component, and attach everything together.
    
    ((vsSystem::systemObject)->getNodeMap())->registerLink(this, topGroup);

    // Process all of the children of this node
    childCount = 0;
    for (loop = 0; loop < (bottomGroup->getNumChildren()); loop++)
    {
        // Get the loop'th child of the bottom Performer group
        currentNode = bottomGroup->getChild(loop);

        // Check to see if we've encountered this node before
        myNode = (vsNode *)(((vsSystem::systemObject)->getNodeMap())->
            mapSecondToFirst(currentNode));
        
        // Process this node if it's unfamiliar
        if (!myNode)
        {
            // Check for a pfBillboard node
            if (currentNode->isOfType(pfBillboard::getClassType()))
            {
                // * Deal with the billboard here
                
                // First, check to see if the component above has more than
                // one child; billboard attributes must get moved up to
                // their parent components but they should not affect the
		// other children of that component. (In VESS, billboards
		// go on components, not geometry like in Performer.)
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
                    // add it to this component to compensate for the
		    // (eventual) removal of the billboard from the
		    // Performer geometry.
                    billAttrib = new vsBillboardAttribute(
                        (pfBillboard *)currentNode);
                    addAttribute(billAttrib);
                }
            }
        
            // Figure out what type of node we're looking at and handle
	    // it accordingly
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
            else // unrecognized type
            {
                // This node is a type we don't recognize and can't handle;
                // discard it so that it doesn't get in the way.
                bottomGroup->removeChild(currentNode);
                loop--;
                printf("vsComponent::vsComponent (conversion constructor): Discarding unrecognized "
                        "Performer node of type '%s'\n",
                        ((pfObject *)(currentNode))->getType()->getName());
                pfDelete(currentNode);
                continue;
            }
        }
        
        // Parent and child are already connected in the Performer scene;
        // make new connections only in our own objects.
        childList[childCount++] = myNode;
        myNode->ref();
        myNode->addParent(this);
    }

    // Reference the Performer objects to lessen the chance that someone
    // else will delete them without us knowing
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

    // Detach all remaining children; don't delete any (in case someone
    // else is using them).
    while (getChildCount() > 0)
    {
        child = getChild(0);
        removeChild(child);
    }

    // Remove the link between this VESS node and the corresponding
    // Performer node
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
// 'Clones' the tree rooted at this node, duplicating the portion of the
// scene graph rooted at this node, down to but not including leaf nodes.
// (Leaf nodes are instanced instead.)
// ------------------------------------------------------------------------
vsNode *vsComponent::cloneTree()
{
    vsComponent *result;
    vsNode *childClone;
    vsAttribute *attr;
    int loop;
    
    // Create a new component
    result = new vsComponent();
    
    // Copy the name and intersection value (all other data members should
    // be taken care of automatically)
    result->setName(getName());
    result->setIntersectValue(getIntersectValue());
    result->setVisibilityValue(getVisibilityValue());
   
    // Clone the children of this component and add them to the new
    // component
    for (loop = 0; loop < getChildCount(); loop++)
    {
        childClone = getChild(loop)->cloneTree();
        result->addChild(childClone);
    }
    
    // Replicate the attributes on this component and add them to the
    // new component as well
    for (loop = 0; loop < getAttributeCount(); loop++)
    {
        attr = getAttribute(loop);
        attr->attachDuplicate(result);
    }
    
    // Return the cloned tree
    return result;
}

// ------------------------------------------------------------------------
// Destroys the entire scene graph rooted at this component, up to but not
// including this component itself. Won't delete instanced nodes unless all
// of the parents of the node are being deleted as well.
// ------------------------------------------------------------------------
void vsComponent::deleteTree()
{
    vsNode *node;
    
    // Delete all children of this node
    while (getChildCount() > 0)
    {
        // Get the first child
        node = getChild(0);

        // If it's a component, recurse
        if (node->getNodeType() == VS_NODE_TYPE_COMPONENT)
            ((vsComponent *)node)->deleteTree();

        // Remove the child from this node
        removeChild(node);

        // Delete the child if it's now unowned
        if (node->getParentCount() == 0)
            delete node;
    }
}

// ------------------------------------------------------------------------
// Retrieves the type of this node
// ------------------------------------------------------------------------
int vsComponent::getNodeType()
{
    return VS_NODE_TYPE_COMPONENT;
}

// ------------------------------------------------------------------------
// Adds the given node as a child of this component
// ------------------------------------------------------------------------
void vsComponent::addChild(vsNode *newChild)
{
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    vsDynamicGeometry *childDynamicGeometry;

    // First, connect the Performer nodes together: determine what type
    // the child node is, get the corresponding Performer node, and add
    // that Performer node as a child of this component's bottom group.
    if (newChild->getNodeType() == VS_NODE_TYPE_COMPONENT)
    {
        childComponent = (vsComponent *)newChild;
        bottomGroup->addChild(childComponent->getBaseLibraryObject());
    }
    else if (newChild->getNodeType() == VS_NODE_TYPE_GEOMETRY)
    {
        childGeometry = (vsGeometry *)newChild;
        bottomGroup->addChild(childGeometry->getBaseLibraryObject());
    }
    else if (newChild->getNodeType() == VS_NODE_TYPE_DYNAMIC_GEOMETRY)
    {
        childDynamicGeometry = (vsDynamicGeometry *)newChild;
        bottomGroup->addChild(childDynamicGeometry->getBaseLibraryObject());
    }

    // Then make the connection in the VESS nodes
    childList[childCount++] = newChild;
    newChild->ref();
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
    vsDynamicGeometry *childDynamicGeometry;
    int loop;
    
    // Bounds check
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

    // First, connect the Performer nodes together: determine what type
    // the child node is, get the corresponding Performer node, and insert
    // that Performer node as a child of this component's bottom group.
    if (newChild->getNodeType() == VS_NODE_TYPE_COMPONENT)
    {
        childComponent = (vsComponent *)newChild;
        bottomGroup->insertChild(index, childComponent->getBaseLibraryObject());
    }
    else if (newChild->getNodeType() == VS_NODE_TYPE_GEOMETRY)
    {
        childGeometry = (vsGeometry *)newChild;
        bottomGroup->insertChild(index, childGeometry->getBaseLibraryObject());
    }
    else if (newChild->getNodeType() == VS_NODE_TYPE_DYNAMIC_GEOMETRY)
    {
        childDynamicGeometry = (vsDynamicGeometry *)newChild;
        bottomGroup->insertChild(index, 
            childDynamicGeometry->getBaseLibraryObject());
    }

    // Mkae room in the component's child list for the new child
    for (loop = childCount; loop > index; loop--)
        childList[loop] = childList[loop-1];

    // Then make the connection in the VESS nodes
    childList[index] = newChild;
    childCount++;
    newChild->ref();
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
    vsDynamicGeometry *childDynamicGeometry;
    
    // Search the child list for the target child
    for (loop = 0; loop < childCount; loop++)
        if (targetChild == childList[loop])
        {
            // Mark the entire portion of the tree that has any connection
            // to this node as needing of an update
            targetChild->dirty();
        
            // Detach the Performer nodes; check for the type of the
            // component because the getBaseLibraryObject call is
            // not virtual. Then get the Performer node from the
	    // child node and remove that from the component's bottom
	    // group.
            if (targetChild->getNodeType() == VS_NODE_TYPE_COMPONENT)
            {
                childComponent = (vsComponent *)targetChild;
                bottomGroup->removeChild(
                    childComponent->getBaseLibraryObject());
            }
            else if (targetChild->getNodeType() == VS_NODE_TYPE_GEOMETRY)
            {
                childGeometry = (vsGeometry *)targetChild;
                bottomGroup->removeChild(
                    childGeometry->getBaseLibraryObject());
            }
            else if (targetChild->getNodeType() == 
                        VS_NODE_TYPE_DYNAMIC_GEOMETRY)
            {
                childDynamicGeometry = (vsDynamicGeometry *)targetChild;
                bottomGroup->removeChild(
                    childDynamicGeometry->getBaseLibraryObject());
            }

            // 'Slide' the rest of the children down to fill in the gap
            for (sloop = loop; sloop < childCount-1; sloop++)
                childList[sloop] = childList[sloop+1];
        
            // Finish the VESS detachment
            childCount--;
            targetChild->unref();
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
    vsDynamicGeometry *childDynamicGeometry;
    pfNode *oldNode, *newNode;
    
    // Search the child list for the target child
    for (loop = 0; loop < childCount; loop++)
        if (targetChild == childList[loop])
        {
            // Mark the entire portion of the tree that has any connection
            // to the old node as needing of an update
            targetChild->dirty();
        
	    // Get the Performer node corresponding to the child to be
	    // removed; we need to check the node type because the
	    // getBaseLibraryObject call is not virtual
            if (targetChild->getNodeType() == VS_NODE_TYPE_COMPONENT)
            {
                childComponent = (vsComponent *)targetChild;
                oldNode = childComponent->getBaseLibraryObject();
            }
            else if (targetChild->getNodeType() == VS_NODE_TYPE_GEOMETRY)
            {
                childGeometry = (vsGeometry *)targetChild;
                oldNode = childGeometry->getBaseLibraryObject();
            }
            else if (targetChild->getNodeType() == 
                        VS_NODE_TYPE_DYNAMIC_GEOMETRY)
            {
                childDynamicGeometry = (vsDynamicGeometry *)targetChild;
                oldNode = childDynamicGeometry->getBaseLibraryObject();
            }

	    // Get the Performer node corresponding to the child to be
	    // added; we need to check the node type because the
	    // getBaseLibraryObject call is not virtual
            if (newChild->getNodeType() == VS_NODE_TYPE_COMPONENT)
            {
                childComponent = (vsComponent *)newChild;
                newNode = childComponent->getBaseLibraryObject();
            }
            else if (newChild->getNodeType() == VS_NODE_TYPE_GEOMETRY)
            {
                childGeometry = (vsGeometry *)newChild;
                newNode = childGeometry->getBaseLibraryObject();
            }
            else if (newChild->getNodeType() == VS_NODE_TYPE_DYNAMIC_GEOMETRY)
            {
                childDynamicGeometry = (vsDynamicGeometry *)newChild;
                newNode = childDynamicGeometry->getBaseLibraryObject();
            }
            
            // Replace the old child with the new one on this component's
	    // bottom group
            bottomGroup->replaceChild(oldNode, newNode);
            
            // Adjust the child list in this component, as well as the
	    // parent lists in the two children, to reflect the change
            childList[loop] = newChild;
            targetChild->unref();
            newChild->ref();
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
    // Bounds check
    if ((index < 0) || (index >= childCount))
    {
        printf("vsComponent::getChild: Bad child index\n");
        return NULL;
    }
    
    // Return the requested child
    return (vsNode *)(childList[index]);
}

// ------------------------------------------------------------------------
// Retrieves the center point and radius of a sphere that encompasses all
// of the geometry within this object.
// ------------------------------------------------------------------------
void vsComponent::getBoundSphere(vsVector *centerPoint, double *radius)
{
    pfSphere boundSphere;
    
    // Get the geometry bounding sphere from the Performer group
    topGroup->getBound(&boundSphere);
    
    // Copy the sphere center point to the result vector, if there is one
    if (centerPoint)
        centerPoint->set(boundSphere.center[PF_X], boundSphere.center[PF_Y],
            boundSphere.center[PF_Z]);

    // Copy the sphere radius to the result value, if there is one
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

    // Start at this component's bottomGroup with an identity matrix
    xform.makeIdent();
    nodePtr = bottomGroup;
    
    // Starting at this component's bottomGroup, run through all of the
    // nodes in the Performer scene graph and accumulate transforms
    // from every pfSCS (or pfDCS, which is derived from pfSCS) along
    // the way. The assumption here is that each node will only have
    // one parent. (Not always the case, but if there is more then we
    // wouldn't know which one to use anyway.)
    while (nodePtr->getNumParents() > 0)
    {
        // Check if the node is a pfSCS (or subclass of one)
        if (nodePtr->isOfType(pfSCS::getClassType()))
        {
            // Multiply the pfSCS's matrix into our matrix
            scsMatPtr = ((pfSCS *)nodePtr)->getMatPtr();
            xform.postMult(*scsMatPtr);
        }
        
        // Move to the node's (first) parent
        nodePtr = nodePtr->getParent(0);
    }
    
    // Copy the resulting Performer matrix to a VESS one, transposing as
    // we go
    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            result[loop][sloop] = xform[sloop][loop];

    // Return the resulting matrix
    return result;
}

// ------------------------------------------------------------------------
// Sets the intersection value for this component. During an intersection
// run, at each component a bitwise AND of the intersection's mask and the
// component's value is performed; if the result of the AND is zero, the
// intersection ignores this component and all of its children.
// ------------------------------------------------------------------------
void vsComponent::setIntersectValue(unsigned int newValue)
{
    // Set the intersection mask on the Performer node
    topGroup->setTravMask(PFTRAV_ISECT, newValue, PFTRAV_SELF, PF_SET);
}

// ------------------------------------------------------------------------
// Retrieves the intersection value for this component.
// ------------------------------------------------------------------------
unsigned int vsComponent::getIntersectValue()
{
    // Get the intersection mask from the Performer node
    return (topGroup->getTravMask(PFTRAV_ISECT));
}

// ------------------------------------------------------------------------
// Sets the visibility value for this component. During the culling portion
// of a frame drawing cycle, a bitwise AND of the pane's visibility mask
// and the node's visibility value is performed; if the result of the AND
// is zero, the node (and all other nodes under it) are culled, not to be
// drawn.
// ------------------------------------------------------------------------
void vsComponent::setVisibilityValue(unsigned int newValue)
{
    // Set the visibility mask on the Performer node
    topGroup->setTravMask(PFTRAV_DRAW, newValue, PFTRAV_SELF, PF_SET);
}

// ------------------------------------------------------------------------
// Retrieves the visibility value for this component.
// ------------------------------------------------------------------------
unsigned int vsComponent::getVisibilityValue()
{
    // Get the visibility mask from the Performer node
    return (topGroup->getTravMask(PFTRAV_DRAW));
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
    
    // Check for a conflict between the attribute to be added and the
    // ones already on the component
    attrCat = newAttribute->getAttributeCategory();
    attrType = newAttribute->getAttributeType();
    switch (attrCat)
    {
        // Component may only contain one of each of these; if the new
	// attribute is one of these categories, make sure there's not
	// another one of the same type already
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

        // Component may only contain one of any of these; if the new
	// attribute is this category, make sure there's not another one
	// of the same category already
        case VS_ATTRIBUTE_CATEGORY_GROUPING:
            if (getCategoryAttribute(VS_ATTRIBUTE_CATEGORY_GROUPING, 0))
            {
                printf("vsComponent::addAttribute: Component may only "
                    "contain one grouping category attribute at a time\n");
                return;
            }
    }

    // If we made it this far, it must be okay to add the attribute in
    vsNode::addAttribute(newAttribute);
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
// Searches this node and its children for the idx'th occurrence of a node
// with the given name. The idx value is decremented after each match;
// success only occurs once idx reaches zero. Returns a pointer to this
// node if a match is found and idx is zero, NULL otherwise.
// ------------------------------------------------------------------------
vsNode *vsComponent::nodeSearch(const char *name, int *idx)
{
    int loop;
    vsNode *result;

    // Check if this is the node we're looking for
    if (!strcmp(name, getName()))
    {
	// Check if this is the desired instance of the nodes with the
	// target name by examining the idx value
        if ((*idx) > 0)
            (*idx)--;
        else
            return this;
    }

    // If not found, search the children
    for (loop = 0; loop < childCount; loop++)
        if (result = ((vsNode *)(childList[loop]))->nodeSearch(name, idx))
            return result;

    // NULL result if the node was not found
    return NULL;
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
    
    // Delete the old bottom group, and set the bottomGroup pointer to
    // point to the new one
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
    
    // Dirty this node
    dirtyFlag = VS_TRUE;
    
    // Dirty the children of this node
    for (loop = 0; loop < childCount; loop++)
        ((vsNode *)(childList[loop]))->dirtyDown();
}
