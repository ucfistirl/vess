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

#include <stdio.h>
#include "vsTransformAttribute.h++"
#include "vsSwitchAttribute.h++"
#include "vsSequenceAttribute.h++"
#include "vsLODAttribute.h++"
#include "vsDecalAttribute.h++"
#include "vsBillboardAttribute.h++"
#include "vsGeometry.h++"
#include "vsDynamicGeometry.h++"

// ------------------------------------------------------------------------
// Default Constructor - Sets up the OSG objects associated with
// this component
// ------------------------------------------------------------------------
vsComponent::vsComponent() : childList(32, 32)
{
    // Create the OSG group objects and tie them together
    topGroup = new osg::Group();
    topGroup->ref();
    lightHook = new osg::Group();
    lightHook->ref();
    bottomGroup = new osg::Group();
    bottomGroup->ref();
    topGroup->addChild(lightHook);
    lightHook->addChild(bottomGroup);

    childCount = 0;
    parentNode = NULL;

    // Add a node map entry that relates this component to its osg Groups
    getMap()->registerLink(this, topGroup);
}

// ------------------------------------------------------------------------
// Destructor - Disconnects this component from its OSG counterpart.
// Also removes all attributes, destroying those that aren't in use
// somewhere else. Additionally, removes all remaining children, though
// it does not delete any of them.
// ------------------------------------------------------------------------
vsComponent::~vsComponent()
{
    vsAttribute *attr;
    vsNode *child, *parent;

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

    // Remove the node map entry that relates the component to its Groups
    getMap()->removeLink(this, VS_OBJMAP_FIRST_LIST);

    // Unlink and destroy the OSG objects
    topGroup->removeChild(lightHook);
    lightHook->removeChild(bottomGroup);
    topGroup->unref();
    lightHook->unref();
    bottomGroup->unref();
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsComponent::getClassName()
{
    return "vsComponent";
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
    
    return result;
}

// ------------------------------------------------------------------------
// Destroys the entire scene graph rooted at this node, up to but not
// including this node itself. Won't delete instanced nodes unless all
// of the parents of the node are being deleted as well.
// ------------------------------------------------------------------------
void vsComponent::deleteTree()
{
    vsNode *node;
    
    while (getChildCount() > 0)
    {
	// We can always get the first child, because removing a child
	// causes all of the other children to slide over to fill the
	// gap.
        node = getChild(0);

	// Delete the subgraph below the selected child
        if (node->getNodeType() == VS_NODE_TYPE_COMPONENT)
            ((vsComponent *)node)->deleteTree();

	// Remove the child from the component, and delete it if
	// it is no longer being used
        removeChild(node);

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
int vsComponent::addChild(vsNode *newChild)
{
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    vsDynamicGeometry *childDynamicGeometry;

    // Notify the newChild node that it is getting a new parent. This might
    // fail, as the child node is permitted to object to getting a parent.
    if (newChild->addParent(this) == VS_FALSE)
    {
	printf("vsComponent::addChild: 'newChild' node may not have any "
	    "more parent nodes\n");
	return VS_FALSE;
    }

    // Connect the OSG nodes together. The type can't be a vsScene, because
    // a scene node would never consent to getting a parent.
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

    // Add the newChild node to our child node list
    childList[childCount++] = newChild;
    newChild->ref();

    // Mark the entire tree above and below this node as needing an update
    newChild->dirty();
    
    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Adds the given node as a child of this component, at the given index in
// the component's child list. All children currently in the list at that
// index or greater are moved over by one.
// ------------------------------------------------------------------------
int vsComponent::insertChild(vsNode *newChild, int index)
{
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    vsDynamicGeometry *childDynamicGeometry;
    osg::Node *newNode, *displacedNode;
    int loop;
    
    if (index < 0)
    {
        printf("vsComponent::insertChild: Index out of bounds\n");
        return VS_FALSE;
    }

    // Notify the newChild node that it is getting a new parent. This might
    // fail, as the child node is permitted to object to getting a parent.
    if (newChild->addParent(this) == VS_FALSE)
    {
	printf("vsComponent::insertChild: 'newChild' node may not have any "
	    "more parent nodes\n");
	return VS_FALSE;
    }
    
    // If the index is greater than the current number of children on this
    // component, simply add the new child on the end normally
    if (index >= childCount)
        return addChild(newChild);

    // First connect the OSG nodes together.  OSG doesn't have an "insertChild"
    // method, so we have to do it the hard way.  First, we get the appropriate
    // OSG node based on the type of VESS node. The type can't be a vsScene,
    // because a scene node would never consent to getting a parent.
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

    // Now, we replace the node at index and shove the rest of the nodes
    // over
    for (loop = index; loop < bottomGroup->getNumChildren(); loop++)
    {
        // Keep a pointer to the node we're about to replace
        displacedNode = bottomGroup->getChild(loop);
        
        // Replace the current node with the new child
        bottomGroup->replaceChild(displacedNode, newNode);

        // Make the displaced node the new node, so it can displace
        // the next child over on the next iteration
        newNode = displacedNode;
    }

    // Add the child that was last displaced onto the end of the group's
    // child list
    bottomGroup->addChild(newNode);

    // Then make the connection in the VESS nodes
    for (loop = childCount; loop > index; loop--)
        childList[loop] = childList[loop-1];
    childList[index] = newChild;
    childCount++;
    newChild->ref();
    
    // Finally, mark the entire tree above and below this node as needing
    // of an update
    newChild->dirty();
    
    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Removes the given node from the list of children for this component
// ------------------------------------------------------------------------
int vsComponent::removeChild(vsNode *targetChild)
{
    int loop, sloop;
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    vsDynamicGeometry *childDynamicGeometry;
    
    for (loop = 0; loop < childCount; loop++)
        if (targetChild == childList[loop])
        {
            // Mark the entire portion of the tree that has any connection
            // to this node as needing of an update
            targetChild->dirty();
        
            // Detach the OSG nodes; checks for the type of the
            // component because the getBaseLibraryObject call is
            // not virtual. The type can't be a vsScene, because
	    // a scene node would never have a parent.
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
	    
	    // Check for errors as we remove this component from the
	    // child's parent list
	    if (targetChild->removeParent(this) == VS_FALSE)
		printf("vsComponent::removeChild: Scene graph inconsistency: "
		    "child to be removed does not have this component as "
		    "a parent\n");

            return VS_TRUE;
        }

    return VS_FALSE;
}

// ------------------------------------------------------------------------
// Replaces the target node with the new node in the list of children for
// this component. The new node occupies the same idex that the previous
// node did.
// ------------------------------------------------------------------------
int vsComponent::replaceChild(vsNode *targetChild, vsNode *newChild)
{
    int loop;
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    vsDynamicGeometry *childDynamicGeometry;
    osg::Node *oldNode, *newNode;
    
    for (loop = 0; loop < childCount; loop++)
        if (targetChild == childList[loop])
        {
	    // Notify the newChild node that it is getting a new parent.
	    // This might fail, as the child node is permitted to object to
	    // getting a parent.
	    if (newChild->addParent(this) == VS_FALSE)
	    {
		printf("vsComponent::replaceChild: 'newChild' node may not "
		    "have any more parent nodes\n");
		return VS_FALSE;
	    }

            // Mark the entire portion of the tree that has any connection
            // to the old node as needing of an update
            targetChild->dirty();
        
            // Replace the OSG nodes; checks for the type of the
            // component because the getBaseLibraryObject call is
            // not virtual. The type can't be a vsScene, because
	    // a scene node would never consent to getting a parent.
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
            
            bottomGroup->replaceChild(oldNode, newNode);
            
            // Change the connection in the VESS nodes
            childList[loop] = newChild;
            
            targetChild->unref();
            newChild->ref();

	    // Check for errors as we remove this component from the
	    // child's parent list
	    if (targetChild->removeParent(this) == VS_FALSE)
		printf("vsComponent::replaceChild: Scene graph inconsistency: "
		    "child to be removed does not have this component as "
		    "a parent\n");
            
            // Mark the entire portion of the tree that has any connection
            // to the new node as needing of an update
            newChild->dirty();

            return VS_TRUE;
        }

    return VS_FALSE;
}

// ------------------------------------------------------------------------
// Retrieves the number of parent nodes for this node
// ------------------------------------------------------------------------
int vsComponent::getParentCount()
{
    if (parentNode)
	return 1;

    return 0;
}

// ------------------------------------------------------------------------
// Retrieves one of the parent nodes of this node, specified by index.
// The index of the first parent is 0.
// ------------------------------------------------------------------------
vsNode *vsComponent::getParent(int index)
{
    if (index != 0)
	return NULL;

    return parentNode;
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
    osg::BoundingSphere boundSphere;
    osg::Vec3 center;
    
    // Get the bounding sphere from OSG
    boundSphere = topGroup->getBound();
    
    // Convert the center to a vsVector
    if (centerPoint)
    {
        center = boundSphere.center();
        centerPoint->set(center[0], center[1], center[2]);
    }

    if (radius)
        *radius = boundSphere.radius();
}

// ------------------------------------------------------------------------
// Computes the global coordinate transform at this component by
// multiplying together all of the transforms at nodes at and above this
// one.
// ------------------------------------------------------------------------
vsMatrix vsComponent::getGlobalXform()
{
    osg::Node *nodePtr;
    osg::Matrix xform;
    osg::Matrix matRef;
    vsMatrix result;
    int loop, sloop;

    // Start at the group on the bottom of this component, and work
    // our way up the OSG tree

    xform.makeIdentity();
    nodePtr = bottomGroup;
    
    while (nodePtr->getNumParents() > 0)
    {
        if (dynamic_cast<osg::MatrixTransform *>(nodePtr))
        {
	    // Multiply this Transform's matrix into the accumulated
	    // transform
            matRef = ((osg::MatrixTransform *)nodePtr)->getMatrix();
            xform.postMult(matRef);
        }
        
        nodePtr = nodePtr->getParent(0);
    }
    
    // Transpose the matrix when going from OSG to VESS
    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            result[loop][sloop] = xform(sloop, loop);

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
    topGroup->setNodeMask(newValue);
}

// ------------------------------------------------------------------------
// Retrieves the intersection value for this component.
// ------------------------------------------------------------------------
unsigned int vsComponent::getIntersectValue()
{
    return (topGroup->getNodeMask());
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
	    break;

        // Component may only contain one of any of these
        case VS_ATTRIBUTE_CATEGORY_XFORM:
            if (getCategoryAttribute(VS_ATTRIBUTE_CATEGORY_XFORM, 0))
            {
                printf("vsComponent::addAttribute: Component may only "
                    "contain one transform category attribute at a time\n");
                return;
            }
	    break;
    }

    // If we made it this far, it must be okay to add the attribute in
    vsNode::addAttribute(newAttribute);
}

// ------------------------------------------------------------------------
// Returns the OSG object associated with this object
// ------------------------------------------------------------------------
osg::Group *vsComponent::getBaseLibraryObject()
{
    return topGroup;
}

// ------------------------------------------------------------------------
// Internal function
// Retrieves the topmost OSG group associated with this component
// ------------------------------------------------------------------------
osg::Group *vsComponent::getTopGroup()
{
    return topGroup;
}

// ------------------------------------------------------------------------
// Internal function
// Retrieves the center OSG group associated with this component
// ------------------------------------------------------------------------
osg::Group *vsComponent::getLightHook()
{
    return lightHook;
}

// ------------------------------------------------------------------------
// Internal function
// Retrieves the bottommost OSG group associated with this component
// ------------------------------------------------------------------------
osg::Group *vsComponent::getBottomGroup()
{
    return bottomGroup;
}

// ------------------------------------------------------------------------
// Internal function
// Replaces the bottommost OSG group of the component with the
// indicated group. Used to enable the operation of certain attributes.
// ------------------------------------------------------------------------
void vsComponent::replaceBottomGroup(osg::Group *newGroup)
{
    osg::Node *childNode;
    osg::Group *parentGroup;

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
    
    // Delete the old bottom group and keep the new one
    bottomGroup->unref();
    bottomGroup = newGroup;
    bottomGroup->ref();
}

// ------------------------------------------------------------------------
// Internal function
// Adds a node to this node's list of parent nodes
// ------------------------------------------------------------------------
int vsComponent::addParent(vsNode *newParent)
{
    if (parentNode)
	return VS_FALSE;

    parentNode = newParent;
    parentNode->ref();
    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Internal function
// Removes a node from this node's list of parent nodes
// ------------------------------------------------------------------------
int vsComponent::removeParent(vsNode *targetParent)
{
    if (parentNode != targetParent)
	return VS_FALSE;

    parentNode->unref();
    parentNode = NULL;
    return VS_TRUE;
}
