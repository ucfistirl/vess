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
#include "vsUnmanagedNode.h++"
#include "vsOSGNode.h++"

// ------------------------------------------------------------------------
// Default Constructor - Sets up the OSG objects associated with
// this component
// ------------------------------------------------------------------------
vsComponent::vsComponent()
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

    // Create the child list
    childList = new vsArray();
    parentNode = NULL;

    // Add a node map entry that relates this component to its osg Groups
    getMap()->registerLink(this, new vsOSGNode(topGroup));
}

// ------------------------------------------------------------------------
// Destructor - Disconnects this component from its OSG counterpart.
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

    // Remove the node map entry that relates the component to its Groups
    getMap()->removeLink(this, VS_OBJMAP_FIRST_LIST);

    // Unlink and destroy the OSG objects
    topGroup->removeChild(lightHook);
    lightHook->removeChild(bottomGroup);
    topGroup->unref();
    lightHook->unref();
    bottomGroup->unref();

    // Delete the child list (need to do this last because some of the
    // other destruction procedures might need to call getChildCount() on
    // this node before it's gone)
    delete childList;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsComponent::getClassName()
{
    return "vsComponent";
}

// ------------------------------------------------------------------------
// Retrieves the type of this node
// ------------------------------------------------------------------------
int vsComponent::getNodeType()
{
    return VS_NODE_TYPE_COMPONENT;
}

// ------------------------------------------------------------------------
// Clones the subgraph rooted at this node
// ------------------------------------------------------------------------
vsNode *vsComponent::cloneTree()
{
    vsNode *cloneNode;

    // Call the recursive clone method on this component, and keep track of
    // the result
    cloneNode = cloneTreeRecursive();

    // Mark the cloned node and its children dirty
    cloneNode->dirty();

    // Return the clone
    return cloneNode;
}

// ------------------------------------------------------------------------
// Destroys the entire scene graph rooted at this node, up to but not
// including this node itself. Deletes any objects whose reference counts
// reach zero.
// ------------------------------------------------------------------------
void vsComponent::deleteTree()
{
    vsNode *node;
   
    // Delete all children of this node
    while (getChildCount() > 0)
    {
        // We can always get the first child, because removing a child
        // causes all of the other children to slide over to fill the
        // gap.
        node = getChild(0);

        // See if this node is a component
        if (node->getNodeType() == VS_NODE_TYPE_COMPONENT)
        {
            // Delete the subgraph below the selected child
            ((vsComponent *)node)->deleteTree();

            // Remove the child from this node, but don't mark it dirty
            // (components can only have one parent, so we won't need
            // to dirty this node in any case)
            removeChild(node, false);
        }
        else
        {
            // Remove the child from this node, but don't mark it dirty yet
            removeChild(node, false);

            // If the node is clean and it has any remaining parents, mark
            // it dirty manually
            if ((node->getParentCount() > 0) && (!node->isDirty()))
               node->dirty();
        }

        // Delete the child if it's now unowned
        vsObject::checkDelete(node);
    }
}

// ------------------------------------------------------------------------
// Add the given node as a child of this component.  The public version
// simply calls the protected version with the dirty flag set to true
// (most of the time, we want newly added nodes to be marked dirty, so
// state information and other data can be properly propagated to the
// children)
// ------------------------------------------------------------------------
bool vsComponent::addChild(vsNode *newChild)
{
    // Call the protected method with the dirty flag set to true
    return addChild(newChild, true);
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
    vsUnmanagedNode *childUnmanagedNode;
    osg::Node *newNode, *displacedNode;
    int loop;
    vsSwitchAttribute *switchAttr;

    // Bounds check
    if (index < 0)
    {
        printf("vsComponent::insertChild: Index out of bounds\n");
        return false;
    }

    // If the index is greater than the current number of children on this
    // component, simply add the new child on the end normally
    if (index >= childList->getNumEntries())
        return addChild(newChild);

    // Notify the newChild node that it is getting a new parent. This might
    // fail, as the child node is permitted to object to getting a parent.
    if (newChild->addParent(this) == false)
    {
        printf("vsComponent::insertChild: 'newChild' node may not have any "
            "more parent nodes\n");
        return false;
    }
    
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
    else if (newChild->getNodeType() == VS_NODE_TYPE_SKELETON_MESH_GEOMETRY)
    {
        childSkeletonMeshGeometry = (vsSkeletonMeshGeometry *)newChild;
        newNode = childSkeletonMeshGeometry->getBaseLibraryObject();
    }
    else if (newChild->getNodeType() == VS_NODE_TYPE_UNMANAGED)
    {
        childUnmanagedNode = (vsUnmanagedNode *)newChild;
        newNode = childUnmanagedNode->getBaseLibraryObject();
    }

    // Now, we replace the node at index and shove the rest of the nodes
    // over
    for (loop = index; loop < (int)bottomGroup->getNumChildren(); loop++)
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
    childList->insertEntry(index, newChild);

    // Special case:  If there is a switch attribute attached to this 
    // component, then we need to add a switch mask to the switch with 
    // the new child active.  This emulates Performer's pfSwitch behavior.
    switchAttr = (vsSwitchAttribute *)
        getTypedAttribute(VS_ATTRIBUTE_TYPE_SWITCH, 0);
    if (switchAttr != NULL)
        switchAttr->addMask(this, newChild);

    // Finally, mark the entire tree above and below this node as needing
    // of an update
    newChild->dirty();
    
    return true;
}

// ------------------------------------------------------------------------
// Removes the given node from the list of children for this component
// The public version simply calls the protected version with the dirty
// flag set to true (most of the time, we want attached nodes to be marked
// dirty after a child is removed)
// ------------------------------------------------------------------------
bool vsComponent::removeChild(vsNode *targetChild)
{
    // Call the protected removeChild method with the dirty flag set to
    // true
    return removeChild(targetChild, true);
}

// ------------------------------------------------------------------------
// Replaces the target node with the new node in the list of children for
// this component. The new node occupies the same idex that the previous
// node did.
// ------------------------------------------------------------------------
bool vsComponent::replaceChild(vsNode *targetChild, vsNode *newChild)
{
    int index;
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    vsDynamicGeometry *childDynamicGeometry;
    vsSkeletonMeshGeometry *childSkeletonMeshGeometry;
    vsUnmanagedNode *childUnmanagedNode;
    osg::Node *oldNode, *newNode;
    
    // Search the child list for the target child
    index = childList->getIndexOf(targetChild);

    // If we didn't find it, return false to indicate failure
    if (index < 0)
       return false;

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
    else if (targetChild->getNodeType() ==
                VS_NODE_TYPE_SKELETON_MESH_GEOMETRY)
    {
        childSkeletonMeshGeometry =
            (vsSkeletonMeshGeometry *)targetChild;
        oldNode = childSkeletonMeshGeometry->getBaseLibraryObject();
    }
    else if (targetChild->getNodeType() == VS_NODE_TYPE_UNMANAGED)
    {
        childUnmanagedNode = (vsUnmanagedNode *)targetChild;
        oldNode = childUnmanagedNode->getBaseLibraryObject();
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
    else if (newChild->getNodeType() ==
                VS_NODE_TYPE_SKELETON_MESH_GEOMETRY)
    {
        childSkeletonMeshGeometry = (vsSkeletonMeshGeometry *)newChild;
        newNode = childSkeletonMeshGeometry->getBaseLibraryObject();
    }
    else if (newChild->getNodeType() == VS_NODE_TYPE_UNMANAGED)
    {
        childUnmanagedNode = (vsUnmanagedNode *)newChild;
        newNode = childUnmanagedNode->getBaseLibraryObject();
    }

    // Replace the old child with the new one on this component's
    // bottom group
    bottomGroup->replaceChild(oldNode, newNode);
    
    // Change the connection in the VESS nodes (use a temporary reference to
    // the old child to keep it from getting deleted)
    targetChild->ref();
    childList->setEntry(index, newChild);
    targetChild->unref();

    // Check for errors as we remove this component from the
    // child's parent list
    if (targetChild->removeParent(this) == false)
        printf("vsComponent::replaceChild: Scene graph inconsistency: "
            "child to be removed does not have this component as "
            "a parent\n");

    // Mark the entire portion of the tree that has any connection
    // to the new node as needing of an update
    newChild->dirty();

    return true;
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
    return childList->getNumEntries();
}

// ------------------------------------------------------------------------
// Retrieves the child with the given index from this component. The index
// of the first child is 0.
// ------------------------------------------------------------------------
vsNode *vsComponent::getChild(int index)
{
    // Bounds check
    if ((index < 0) || (index >= childList->getNumEntries()))
    {
        printf("vsComponent::getChild: Bad child index\n");
        return NULL;
    }
    
    // Return the requested child
    return (vsNode *)(childList->getEntry(index));
}

// ------------------------------------------------------------------------
// Retrieves the center point and radius of a sphere that encompasses all
// of the geometry within this object.
// ------------------------------------------------------------------------
void vsComponent::getBoundSphere(atVector *centerPoint, double *radius)
{
    osg::BoundingSphere boundSphere;
    osg::Vec3 center;
    
    // Get the bounding sphere from OSG
    boundSphere = topGroup->getBound();

    // Copy the sphere center point to the result vector, if there is one
    if (centerPoint)
    {
        center = boundSphere.center();
        centerPoint->set(center[0], center[1], center[2]);
    }

    // Copy the sphere radius to the result value, if there is one
    if (radius)
        *radius = boundSphere.radius();
}


// ------------------------------------------------------------------------
// Computes the global coordinate transform at this component by
// multiplying together all of the transforms at nodes at and above this
// one.
// ------------------------------------------------------------------------
atMatrix vsComponent::getGlobalXform()
{
    osg::Node *nodePtr;
    osg::Matrixd xform;
    osg::Matrixd matRef;
    atMatrix result;
    int loop, sloop;

    // Start at the group on the bottom of this component, and work
    // our way up the OSG tree

    xform.makeIdentity();
    nodePtr = bottomGroup;
    
    // Check the parent count to determine if we're at the top of the tree
    while (nodePtr->getNumParents() > 0)
    {
        if (dynamic_cast<osg::MatrixTransform *>(nodePtr))
        {
            // Multiply this Transform's matrix into the accumulated
            // transform
            matRef = ((osg::MatrixTransform *)nodePtr)->getMatrix();
            xform.postMult(matRef);
        }
        
        // Move to the node's (first) parent
        nodePtr = nodePtr->getParent(0);
    }

    // Transpose the matrix when going from OSG to VESS
    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            result[loop][sloop] = xform(sloop, loop);

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
        // Component may only contain one of each of these; if the new
        // attribute is one of these categories, make sure there's not
        // another one of the same type already
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
                    else
                        textureUnit = -1;

                    // If the texture units are equal then they both must
                    // have been texture type attributes and had the same
                    // unit.  We don't want that to be allowed so print
                    // error and return.
                    if (textureUnit == newTextureUnit)
                    {
                        printf("vsComponent::addAttribute: Component node "
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

        // Component may only contain one of any of these; if the new
        // attribute is this category, make sure there's not another one
        // of the same category already
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
// Enables lighting on this vsComponent, and all of its children and
// geometry.
// ------------------------------------------------------------------------
void vsComponent::enableLighting()
{
    int loop;
    
    // Loop through all children.
    for(loop = 0; loop < childList->getNumEntries(); loop++)
    {   
        // Enable lighting on this child.
        ((vsNode*)childList->getEntry(loop))->enableLighting();
    }
}

// ------------------------------------------------------------------------
// Disables lighting on this vsComponent, and all of its children and
// geometry.
// ------------------------------------------------------------------------
void vsComponent::disableLighting()
{  
   int loop;
   
   // Loop through all children.
   for(loop = 0; loop < childList->getNumEntries(); loop++)
   {
       // Disable lighting on this child
       ((vsNode*)childList->getEntry(loop))->disableLighting();
   }
}
        
// ------------------------------------------------------------------------
// Enables culling on this node and its children
// ------------------------------------------------------------------------
void vsComponent::enableCull()
{
    int loop;

    // Iterate over the component's children
    for (loop = 0; loop < childList->getNumEntries(); loop++)
    {
        // Enable culling on this child
        ((vsNode *)childList->getEntry(loop))->enableCull();
    }
}

// ------------------------------------------------------------------------
// Disables culling on this node and its children
// ------------------------------------------------------------------------
void vsComponent::disableCull()
{
    int loop;

    // Iterate over the component's children
    for (loop = 0; loop < childList->getNumEntries(); loop++)
    {
        // Disable culling on this child
        ((vsNode *)childList->getEntry(loop))->disableCull();
    }
}

// ------------------------------------------------------------------------
// Returns the OSG object associated with this object
// ------------------------------------------------------------------------
osg::Group *vsComponent::getBaseLibraryObject()
{
    return topGroup;
}

// ------------------------------------------------------------------------
// Protected function
// Recursively 'clones' the tree rooted at this node, duplicating the 
// portion of the scene graph rooted at this node, down to but not
// including leaf nodes. (Leaf nodes are instanced instead.)
// ------------------------------------------------------------------------
vsNode *vsComponent::cloneTreeRecursive()
{
    vsComponent *result;
    vsNode *childNode;
    vsComponent *childComp;
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
        // Get the next child
        childNode = getChild(loop);
        
        // Clone the tree below this node
        if (childNode->getNodeType() == VS_NODE_TYPE_COMPONENT)
        {
            // Since this is a component, we'll call the recursive clone
            // method
            childComp = (vsComponent *)childNode;
            childClone = childComp->cloneTreeRecursive();
        }
        else
        {
            // Call the regular cloneTree() method (in most cases, this
            // will simply return the node itself)
            childClone = childNode->cloneTree();
        }

        // Add the cloned child to this node without marking the child
        // dirty.  We'll make one dirty() call once we're done cloning the
        // subgraph
        result->addChild(childClone, false);
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
// Protected function
// Adds the given node as a child of this component, and either marks or
// doesn't mark the affected subgraph and its parents dirty (according
// to the dirty flag).  Most of the time, the dirty process needs to be
// done, but there are cases where we don't want this
// ------------------------------------------------------------------------
bool vsComponent::addChild(vsNode *newChild, bool dirtyFlag)
{
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    vsDynamicGeometry *childDynamicGeometry;
    vsSkeletonMeshGeometry *childSkeletonMeshGeometry;
    vsUnmanagedNode *childUnmanagedNode;
    vsSwitchAttribute *switchAttr;

    // Notify the newChild node that it is getting a new parent. This might
    // fail, as the child node is permitted to object to getting a parent.
    if (newChild->addParent(this) == false)
    {
        printf("vsComponent::addChild: 'newChild' node may not have any "
            "more parent nodes\n");
        return false;
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
    else if (newChild->getNodeType() == VS_NODE_TYPE_SKELETON_MESH_GEOMETRY)
    {
        childSkeletonMeshGeometry = (vsSkeletonMeshGeometry *)newChild;
        bottomGroup->addChild(
            childSkeletonMeshGeometry->getBaseLibraryObject());
    }
    else if (newChild->getNodeType() == VS_NODE_TYPE_UNMANAGED)
    {
        childUnmanagedNode = (vsUnmanagedNode *)newChild;
        bottomGroup->addChild(childUnmanagedNode->getBaseLibraryObject());
    }

    // Add the newChild node to our child node list
    childList->addEntry(newChild);

    // Special case:  If there is a switch attribute attached to this 
    // component, then we need to add a switch mask to the switch with 
    // the new child active.  This emulates Performer's pfSwitch behavior.
    switchAttr = (vsSwitchAttribute *)
        getTypedAttribute(VS_ATTRIBUTE_TYPE_SWITCH, 0);
    if (switchAttr != NULL)
        switchAttr->addMask(this, newChild);

    // See if we should mark the affected nodes dirty
    if (dirtyFlag)
    {
        // Mark the entire tree above and below this node as needing an
        // update
        newChild->dirty();
    }
    
    return true;
}

// ------------------------------------------------------------------------
// Removes the given node from the list of children for this component,
// and either marks or doesn't mark the affected subgraph dirty (according
// to the dirty flag).  Most of the time, the dirty process needs to be
// done, but there are cases where we don't want this
// ------------------------------------------------------------------------
bool vsComponent::removeChild(vsNode *targetChild, bool dirtyFlag)
{
    bool result; 
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    vsDynamicGeometry *childDynamicGeometry;
    vsSkeletonMeshGeometry *childSkeletonMeshGeometry;
    vsUnmanagedNode *childUnmanagedNode;
    vsSwitchAttribute *switchAttr;
    
    // Add a temporary reference to the target child to keep it from
    // getting deleted
    targetChild->ref();

    // Search the child list for the target child
    result = childList->removeEntry(targetChild);
    if (result == false)
    {
       // Remove our temporary reference and bail
       targetChild->unref();
       return false;
    }

    // Mark the entire portion of the tree that has any connection
    // to this node as needing of an update
    if (dirtyFlag)
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
    else if (targetChild->getNodeType() ==
                VS_NODE_TYPE_SKELETON_MESH_GEOMETRY)
    {
        childSkeletonMeshGeometry =
            (vsSkeletonMeshGeometry *)targetChild;
        bottomGroup->removeChild(
            childSkeletonMeshGeometry->getBaseLibraryObject());
    }
    else if (targetChild->getNodeType() == VS_NODE_TYPE_UNMANAGED)
    {
        childUnmanagedNode = (vsUnmanagedNode *)targetChild;
        bottomGroup->removeChild(
            childUnmanagedNode->getBaseLibraryObject());
    }

    // Check for errors as we remove this component from the
    // child's parent list
    if (targetChild->removeParent(this) == false)
        printf("vsComponent::removeChild: Scene graph inconsistency: "
            "child to be removed does not have this component as "
            "a parent\n");

    // Remove our temporary reference (don't delete it)
    targetChild->unref();
    
    // Special case:  If there is a switch attribute attached to this 
    // component, then we need to check the switch masks after we 
    // remove the child, and delete any masks that are now empty.  
    // This emulates Performer's pfSwitch behavior.
    switchAttr = (vsSwitchAttribute *)
       getTypedAttribute(VS_ATTRIBUTE_TYPE_SWITCH, 0);
    if (switchAttr != NULL)
       switchAttr->pruneMasks(this);

    return true;
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

    // Delete the old bottom group, and set the bottomGroup pointer to
    // point to the new one
    bottomGroup->unref();
    bottomGroup = newGroup;
    bottomGroup->ref();
}

// ------------------------------------------------------------------------
// Internal function
// Adds a node to this node's list of parent nodes
// ------------------------------------------------------------------------
bool vsComponent::addParent(vsNode *newParent)
{
    // We can only have one parent; resist any attempt to add more
    if (parentNode)
        return false;

    // Add the parent and return success
    parentNode = newParent;
    parentNode->ref();

    return true;
}

// ------------------------------------------------------------------------
// Internal function
// Removes a node from this node's list of parent nodes
// ------------------------------------------------------------------------
bool vsComponent::removeParent(vsNode *targetParent)
{
    // If the specified node isn't our parent, fail
    if (parentNode != targetParent)
        return false;

    // Unreference the parent node (don't delete it)
    parentNode->unref();

    // Remove the parent and return success
    parentNode = NULL;
    return true;
}

// ------------------------------------------------------------------------
// Internal function
// Recursively finds the topLeft and bottomRight of the geometry that is
// represented by this component (all the objects in the children list).
// ------------------------------------------------------------------------
void vsComponent::getAxisAlignedBoxBounds(atVector  *minValues,
    atVector *maxValues)
{
    int childCount = getChildCount();
    int cntChild;
    int column;
    atVector tempMinValues;
    atVector tempMaxValues;
    atVector passMinValues;
    atVector passMaxValues;
    atVector oldPoint;
    atVector newPoint;
    vsTransformAttribute *transform = NULL;
    atMatrix dynamicMatrix;
    bool minNotSet = true;
    bool maxNotSet = true;

    // Get the transformation attribute from myself.
    transform = (vsTransformAttribute *)
        getTypedAttribute(VS_ATTRIBUTE_TYPE_TRANSFORM, 0);

    // If I have no transform attribute set the dynamicMatrix (which stores the
    // transformation) to the identity matrix, otherwise take the one from the
    // transformAttribute.
    if (transform == NULL)
    {
        dynamicMatrix.setIdentity();
    }
    else
    {
        dynamicMatrix = transform->getCombinedTransform();
    }

    // If there are no children, then save time and leave.
    if (childCount == 0)
    {
        // Since there are no children associated with this node
        // this should just return from here since there is nothing to do
        return;
    }

    // Loop through all of the children and get their bounds.
    // While looping set the bounds for this function
    minNotSet = true;
    maxNotSet = true;
    for (cntChild = 0; cntChild < childCount; cntChild++)
    {
        // Grab this child's min/max values
        getChild(cntChild)->getAxisAlignedBoxBounds(
            &passMinValues, &passMaxValues);
        
        // Get the min and max of the children values
        for (column = 0; column < 3; column++)
        {
            // Check to see if this is the minimum point
            if (minNotSet || (passMinValues[column] < tempMinValues[column]))
            {
                tempMinValues[column] = passMinValues[column];
                minNotSet = false;
            }

            // Check to see if this is the maximum point
            if (maxNotSet || (passMaxValues[column] > tempMaxValues[column]))
            {
                tempMaxValues[column] = passMaxValues[column];
                maxNotSet = false;
            }
        }
    }

    // Transform the points 
    passMaxValues = dynamicMatrix.getPointXform(tempMaxValues);
    passMinValues = dynamicMatrix.getPointXform(tempMinValues);

    // Set the min and max values for this component
    for (column = 0; column < 3; column++)
    {
        if (passMinValues[column] < (*minValues)[column])
        {
            (*minValues)[column] = passMinValues[column];
        }

        if (passMaxValues[column] > (*maxValues)[column])
        {
            (*maxValues)[column] = passMaxValues[column];
        }    
    }

    // At this point (the exit of the function) the minValues and maxValues
    // vsMatricies have been set to the new bounds if that is at all
    // applicable.
}
