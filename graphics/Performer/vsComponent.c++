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
#include "vsSkeletonMeshGeometry.h++"
#include "vsTextureAttribute.h++"
#include "vsTextureCubeAttribute.h++"
#include "vsTextureRectangleAttribute.h++"

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

    // Start with no parent or children
    childCount = 0;
    parentNode = NULL;

    // Add a node map entry that relates this component to its Performer Groups
    getMap()->registerLink(this, topGroup);
}

// ------------------------------------------------------------------------
// Destructor - Disconnects this component from its Performer counterpart.
// Also removes all attributes, destroying those that aren't in use
// somewhere else. Additionally, removes all remaining children, though
// it does not delete any of them.
// ------------------------------------------------------------------------
vsComponent::~vsComponent()
{
    // Remove all parents
    detachFromParents();

    // Remove all children
    deleteTree();

    // Remove all attributes
    deleteAttributes();

    // Remove the node map entry that relates the component to its pfGroups
    getMap()->removeLink(this, VS_OBJMAP_FIRST_LIST);

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
    
    // Return the cloned tree
    return result;
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
bool vsComponent::addChild(vsNode *newChild)
{
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    vsDynamicGeometry *childDynamicGeometry;
    vsSkeletonMeshGeometry *childSkeletonMeshGeometry;

    // Notify the newChild node that it is getting a new parent. This might
    // fail, as the child node is permitted to object to getting a parent.
    if (newChild->addParent(this) == false)
    {
	printf("vsComponent::addChild: 'newChild' node may not have any "
	    "more parent nodes\n");
	return false;
    }

    // Connect the Performer nodes together. The type can't be a vsScene,
    // because a scene node would never consent to getting a parent.
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
    else if (newChild->getNodeType() == VS_NODE_TYPE_SKELETON_MESH_GEOMETRY)
    {
        childSkeletonMeshGeometry = (vsSkeletonMeshGeometry *)newChild;
        bottomGroup->addChild(
            childSkeletonMeshGeometry->getBaseLibraryObject());
    }

    // Add the newChild node to our child node list
    childList[childCount++] = newChild;
    newChild->ref();

    // Mark the entire tree above and below this node as needing an update
    newChild->dirty();
    
    // Return success
    return true;
}

// ------------------------------------------------------------------------
// Adds the given node as a child of this component, at the given index in
// the component's child list. All children currently in the list at that
// index or greater are moved over by one.
// ------------------------------------------------------------------------
bool vsComponent::insertChild(vsNode *newChild, int index)
{
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    vsDynamicGeometry *childDynamicGeometry;
    vsSkeletonMeshGeometry *childSkeletonMeshGeometry;
    int loop;
    
    // Bounds check
    if (index < 0)
    {
        printf("vsComponent::insertChild: Index out of bounds\n");
        return false;
    }

    // Notify the newChild node that it is getting a new parent. This might
    // fail, as the child node is permitted to object to getting a parent.
    if (newChild->addParent(this) == false)
    {
	printf("vsComponent::insertChild: 'newChild' node may not have any "
	    "more parent nodes\n");
	return false;
    }
    
    // If the index is greater than the current number of children on this
    // component, simply add the new child on the end normally
    if (index >= childCount)
        return addChild(newChild);

    // First connect the Performer nodes together
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
    else if (newChild->getNodeType() == VS_NODE_TYPE_SKELETON_MESH_GEOMETRY)
    {
        childSkeletonMeshGeometry = (vsSkeletonMeshGeometry *)newChild;
        bottomGroup->insertChild(index, 
            childSkeletonMeshGeometry->getBaseLibraryObject());
    }

    // Mkae room in the component's child list for the new child
    for (loop = childCount; loop > index; loop--)
        childList[loop] = childList[loop-1];

    // Then make the connection in the VESS nodes
    childList[index] = newChild;
    childCount++;
    newChild->ref();
    
    // Finally, mark the entire tree above and below this node as needing
    // of an update
    newChild->dirty();
    
    // Return success
    return true;
}

// ------------------------------------------------------------------------
// Removes the given node from the list of children for this component
// ------------------------------------------------------------------------
bool vsComponent::removeChild(vsNode *targetChild)
{
    int loop, sloop;
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    vsDynamicGeometry *childDynamicGeometry;
    vsSkeletonMeshGeometry *childSkeletonMeshGeometry;
    
    // Search the child list for the target child
    for (loop = 0; loop < childCount; loop++)
        if (targetChild == childList[loop])
        {
            // Mark the entire portion of the tree that has any connection
            // to this node as needing of an update
            targetChild->dirty();
        
            // Detach the Performer nodes; check for the type of the
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
            else if (targetChild->getNodeType() == 
                        VS_NODE_TYPE_SKELETON_MESH_GEOMETRY)
            {
                childSkeletonMeshGeometry =
                    (vsSkeletonMeshGeometry *)targetChild;
                bottomGroup->removeChild(
                    childSkeletonMeshGeometry->getBaseLibraryObject());
            }

            // 'Slide' the rest of the children down to fill in the gap
            for (sloop = loop; sloop < childCount-1; sloop++)
                childList[sloop] = childList[sloop+1];
        
            // Finish the VESS detachment
            childCount--;
            targetChild->unref();
	    
	    // Check for errors as we remove this component from the
	    // child's parent list
	    if (targetChild->removeParent(this) == false)
		printf("vsComponent::removeChild: Scene graph inconsistency: "
		    "child to be removed does not have this component as "
		    "a parent\n");

            // Return success
            return true;
        }

    // Couldn't find the child, return failure
    return false;
}

// ------------------------------------------------------------------------
// Replaces the target node with the new node in the list of children for
// this component. The new node occupies the same idex that the previous
// node did.
// ------------------------------------------------------------------------
bool vsComponent::replaceChild(vsNode *targetChild, vsNode *newChild)
{
    int loop;
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    vsDynamicGeometry *childDynamicGeometry;
    vsSkeletonMeshGeometry *childSkeletonMeshGeometry;
    pfNode *oldNode, *newNode;
    
    // Search the child list for the target child
    for (loop = 0; loop < childCount; loop++)
        if (targetChild == childList[loop])
        {
	    // Notify the newChild node that it is getting a new parent.
	    // This might fail, as the child node is permitted to object to
	    // getting a parent.
	    if (newChild->addParent(this) == false)
	    {
		printf("vsComponent::replaceChild: 'newChild' node may not "
		    "have any more parent nodes\n");
		return false;
	    }

            // Mark the entire portion of the tree that has any connection
            // to the old node as needing of an update
            targetChild->dirty();
        
            // Replace the Performer nodes; checks for the type of the
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
            else if (targetChild->getNodeType() == 
                        VS_NODE_TYPE_SKELETON_MESH_GEOMETRY)
            {
                childSkeletonMeshGeometry =
                    (vsSkeletonMeshGeometry *)targetChild;
                oldNode = childSkeletonMeshGeometry->getBaseLibraryObject();
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
            else if (newChild->getNodeType() ==
                     VS_NODE_TYPE_SKELETON_MESH_GEOMETRY)
            {
                childSkeletonMeshGeometry = (vsSkeletonMeshGeometry *)newChild;
                newNode = childSkeletonMeshGeometry->getBaseLibraryObject();
            }
            
            // Replace the old child with the new one on this component's
	    // bottom group
            bottomGroup->replaceChild(oldNode, newNode);
            
            // Change the connection in the VESS nodes
            childList[loop] = newChild;
            targetChild->unref();
            newChild->ref();

	    // Check for errors as we remove this component from the
	    // child's parent list
	    if (targetChild->removeParent(this) == false)
		printf("vsComponent::replaceChild: Scene graph inconsistency: "
		    "child to be removed does not have this component as "
		    "a parent\n");
            
            // Mark the entire portion of the tree that has any connection
            // to the new node as needing of an update
            newChild->dirty();

            // Return success
            return true;
        }

    // Couldn't find the target child, return failure
    return false;
}

// ------------------------------------------------------------------------
// Retrieves the number of parent nodes for this node
// ------------------------------------------------------------------------
int vsComponent::getParentCount()
{
    // Return 1 if the parent is valid
    if (parentNode)
	return 1;

    // Otherwise, return 0
    return 0;
}

// ------------------------------------------------------------------------
// Retrieves one of the parent nodes of this node, specified by index.
// The index of the first parent is 0.
// ------------------------------------------------------------------------
vsNode *vsComponent::getParent(int index)
{
    // Only an index of 0 is valid, return NULL if something else is given
    if (index != 0)
	return NULL;

    // Return the parentNode pointer (even if it is NULL)
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
// Attempts to add the given attribute to the component's list of
// attributes. If successful, also notifies the attribute that it has been
// added to a component.
// ------------------------------------------------------------------------
void vsComponent::addAttribute(vsAttribute *newAttribute)
{
    int newAttrType, newAttrCat;
    int attrType;
    int loop;
    int textureUnit, newTextureUnit;
    vsAttribute *attribute;

    // Ask the attribute if it's willing to be added; if it returns false,
    // it's probably already attached somewhere else.
    if (!(newAttribute->canAttach()))
    {
        printf("vsComponent::addAttribute: Attribute is already in use\n");
        return;
    }
    
    // Check for a conflict between the attribute to be added and the
    // ones already on the component
    newAttrCat = newAttribute->getAttributeCategory();
    newAttrType = newAttribute->getAttributeType();
    switch (newAttrCat)
    {
        // Component may only contain one of each of these (with the
        // exception of texture-type attributes on different texture units);
        // if the new attribute is one of these categories, make sure there's 
        // not another one of the same type already
        case VS_ATTRIBUTE_CATEGORY_STATE:
            if ((newAttrType == VS_ATTRIBUTE_TYPE_TEXTURE) ||
                (newAttrType == VS_ATTRIBUTE_TYPE_TEXTURE_CUBE) ||
                (newAttrType == VS_ATTRIBUTE_TYPE_TEXTURE_RECTANGLE))
            {
                // Initialize the texture unit to invalid maximum.
                textureUnit = VS_MAXIMUM_TEXTURE_UNITS;
                newTextureUnit = VS_MAXIMUM_TEXTURE_UNITS+1;
                                                                                
                // Get the new attribute's type.
                newAttrType = newAttribute->getAttributeType();
                                                                                
                // Get the texture unit of the new attribute, if it is 
                // a texture attribute.
                if (newAttrType == VS_ATTRIBUTE_TYPE_TEXTURE)
                {
                    newTextureUnit =
                        ((vsTextureAttribute *) newAttribute)->getTextureUnit();
                }
                else if (newAttrType == VS_ATTRIBUTE_TYPE_TEXTURE_CUBE)
                {
                    newTextureUnit = ((vsTextureCubeAttribute *) 
                        newAttribute)->getTextureUnit();
                }
                else if (newAttrType == VS_ATTRIBUTE_TYPE_TEXTURE_RECTANGLE)
                {
                    newTextureUnit = ((vsTextureRectangleAttribute *) 
                        newAttribute)->getTextureUnit();
                }
                                                                                
                // Check each attribute we have.
                for (loop = 0; loop < getAttributeCount(); loop++)
                {
                    attribute = getAttribute(loop);
                    attrType = attribute->getAttributeType();
                                                                                
                    // Get the texture unit of the current attribute, if it 
                    // is a texture attribute.
                    if (attrType == VS_ATTRIBUTE_TYPE_TEXTURE)
                    {
                        textureUnit = ((vsTextureAttribute *) 
                            attribute)->getTextureUnit();
                    }
                    else if (attrType == VS_ATTRIBUTE_TYPE_TEXTURE_CUBE)
                    {
                        textureUnit = ((vsTextureCubeAttribute *) 
                            attribute)->getTextureUnit();
                    }
                    else if (attrType == VS_ATTRIBUTE_TYPE_TEXTURE_RECTANGLE)
                    {
                        textureUnit = ((vsTextureRectangleAttribute *) 
                            attribute)->getTextureUnit();
                    }
                                                                                
                    // If the texture units are equal then they both must 
                    // have been texture type attributes and had the same 
                    // unit.  We don't want that to be allowed so print 
                    // error and return.
                    if (textureUnit == newTextureUnit)
                    {
                        printf("vsComponent::addAttribute: Geometry node "
                            "already contains a texture attribute on unit %d\n",
                            textureUnit);
                        return;
                    }
                }
            }
            else
                for (loop = 0; loop < getAttributeCount(); loop++)
                    if ((getAttribute(loop))->getAttributeType() == newAttrType)
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
// Enables culling (view frustum and otherwise) on this node
// ------------------------------------------------------------------------
void vsComponent::enableCull()
{
    topGroup->setTravMask(PFTRAV_CULL, 0xFFFFFFFF,
        PFTRAV_SELF | PFTRAV_DESCEND, PF_SET);
}

// ------------------------------------------------------------------------
// Disables culling (view frustum and otherwise) on this node
// ------------------------------------------------------------------------
void vsComponent::disableCull()
{
    topGroup->setTravMask(PFTRAV_CULL, 0x0, PFTRAV_SELF | PFTRAV_DESCEND,
        PF_SET);
}

// ------------------------------------------------------------------------
// Returns the Performer object associated with this object
// ------------------------------------------------------------------------
pfGroup *vsComponent::getBaseLibraryObject()
{
    return topGroup;
}

// ------------------------------------------------------------------------
// Internal function
// Retrieves the topmost Performer group associated with this component
// ------------------------------------------------------------------------
pfGroup *vsComponent::getTopGroup()
{
    return topGroup;
}

// ------------------------------------------------------------------------
// Internal function
// Retrieves the center Performer group associated with this component
// ------------------------------------------------------------------------
pfGroup *vsComponent::getLightHook()
{
    return lightHook;
}

// ------------------------------------------------------------------------
// Internal function
// Retrieves the bottommost Performer group associated with this component
// ------------------------------------------------------------------------
pfGroup *vsComponent::getBottomGroup()
{
    return bottomGroup;
}

// ------------------------------------------------------------------------
// Internal function
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
// Internal function
// Adds a node to this node's list of parent nodes
// ------------------------------------------------------------------------
bool vsComponent::addParent(vsNode *newParent)
{
    // If we already have a parent, fail
    if (parentNode)
	return false;

    // Otherwise, set the new parent and return success
    parentNode = newParent;
    return true;
}

// ------------------------------------------------------------------------
// Internal function
// Removes a node from this node's list of parent nodes
// ------------------------------------------------------------------------
bool vsComponent::removeParent(vsNode *targetParent)
{
    // If the current parent doesn't match the target, return failure
    if (parentNode != targetParent)
	return false;

    // Otherwise, clear the parentNode pointer and return success
    parentNode = NULL;
    return true;
}
