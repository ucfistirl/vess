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
//    Description:  vsNode subclass that acts as the root of a VESS scene
//                  graph.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#include "vsScene.h++"
#include "vsComponent.h++"
#include "vsGeometry.h++"
#include "vsDynamicGeometry.h++"

// ------------------------------------------------------------------------
// Default Constructor - Set the light list and child pointer to NULL,
// meaning no lights and no child present.
// ------------------------------------------------------------------------
vsScene::vsScene()
{
    int loop;
    
    osgGroup = new osg::Group();
    osgGroup->ref();

    // Initialize the light list to empty.
    for (loop = 0; loop < VS_LIGHT_MAX; loop++)
    {       
        lightList[loop] = NULL;
    }

    // Set the child to NULL (none).
    child = NULL;

    // Set the child count to reflect that it has no children now.
    childCount = 0;
}

// ------------------------------------------------------------------------
// Destructor - Informs any lights that may be in the light list that
// this scene is no longer valid.  Unref and remove the child.
// ------------------------------------------------------------------------
vsScene::~vsScene()
{
    int loop;

    // If we have a current child, remove and unreference it.
    if (child != NULL)
    {
        child->removeParent(this);
        child->unref();
        child = NULL;
    }

    // Remove any lights we may have in the lightList.
    for (loop = 0; loop < VS_LIGHT_MAX; loop++)
    {       
        if (lightList[loop] != NULL)
        {
            lightList[loop]->setScene(NULL);
            lightList[loop] = NULL;
        }
    }
    
    osgGroup->unref();
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsScene::getClassName()
{
    return "vsScene";
}

// ------------------------------------------------------------------------
// 'Clones' the tree rooted at this node, duplicating the portion of the
// scene graph rooted at this node, down to but not including leaf nodes.
// (Leaf nodes are instanced instead.)
// ------------------------------------------------------------------------
vsNode *vsScene::cloneTree()
{
    vsScene *result;
    vsNode *childClone;
    vsAttribute *attr;
    int loop;
    
    // Create a new scene
    result = new vsScene();
    
    // Copy the name and intersection value
    result->setName(getName());
    result->setIntersectValue(getIntersectValue());
   
    // Clone the child (if any) of this scene and add it to the new scene
    for (loop = 0; loop < getChildCount(); loop++)
    {
        childClone = getChild(loop)->cloneTree();
        result->addChild(childClone);
    }
    
    // Replicate the attributes on this scene and add them to the
    // new scene as well
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
void vsScene::deleteTree()
{
    vsNode *node;
 
    while (getChildCount() > 0)
    {
        // We can always get the first child, because removing a child
        // causes all of the other children to slide over to fill the gap.
        node = getChild(0);

        // Delete the subgraph below the selected child
        if (node->getNodeType() == VS_NODE_TYPE_COMPONENT)
            ((vsComponent *)node)->deleteTree();

        // Remove the child from the scene, and delete it if
        // it no longer being used
        removeChild(node);

        if (node->getParentCount() == 0)
            delete node;
    }
}

// ------------------------------------------------------------------------
// Add a node to this node's child list
// ------------------------------------------------------------------------
int vsScene::addChild(vsNode *newChild)
{
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    vsDynamicGeometry *childDynamicGeometry;

    // Make sure we don't already have a child
    if (child)
    {
        printf("vsScene::addChild: Scene object already has a child\n");
        return VS_FALSE;
    }

    // Notify the newChild node that it is getting a new parent. This might
    // fail, as the child node is permitted to object to getting a parent.
    if (newChild->addParent(this) == VS_FALSE)
    {
        printf("vsScene::addChild: 'newChild' node may not have any "
            "more parent nodes\n");
        return VS_FALSE;
    }

    // Connect the OSG nodes together. The type can't be a vsScene, because
    // a scene node would never consent to getting a parent.
    if (newChild->getNodeType() == VS_NODE_TYPE_COMPONENT)
    {
        childComponent = (vsComponent *)newChild;
        osgGroup->addChild(childComponent->getBaseLibraryObject());
    }
    else if (newChild->getNodeType() == VS_NODE_TYPE_GEOMETRY)
    {
        childGeometry = (vsGeometry *)newChild;
        osgGroup->addChild(childGeometry->getBaseLibraryObject());
    }
    else if (newChild->getNodeType() == VS_NODE_TYPE_DYNAMIC_GEOMETRY)
    {
        childDynamicGeometry = (vsDynamicGeometry *)newChild;
        osgGroup->addChild(childDynamicGeometry->getBaseLibraryObject());
    }

    // Set the newChild node as our child
    child = newChild;
    newChild->ref();

    // Mark the entire tree above and below this node as needing an update
    newChild->dirty();
    
    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Insert a node into this node's child list at the specified index
// ------------------------------------------------------------------------
int vsScene::insertChild(vsNode *newChild, int index)
{
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    vsDynamicGeometry *childDynamicGeometry;

    // Make sure we don't already have a child
    if (child)
    {
        printf("vsScene::insertChild: Scene object already has a child\n");
        return VS_FALSE;
    }
    
    if (index != 0)
    {
        printf("vsScene::insertChild: Invalid index\n");
        return VS_FALSE;
    }

    // Notify the newChild node that it is getting a new parent. This might
    // fail, as the child node is permitted to object to getting a parent.
    if (newChild->addParent(this) == VS_FALSE)
    {
        printf("vsScene::insertChild: 'newChild' node may not have any "
            "more parent nodes\n");
        return VS_FALSE;
    }

    // Connect the OSG nodes together. The type can't be a vsScene, because
    // a scene node would never consent to getting a parent.
    if (newChild->getNodeType() == VS_NODE_TYPE_COMPONENT)
    {
        childComponent = (vsComponent *)newChild;
        osgGroup->addChild(childComponent->getBaseLibraryObject());
    }
    else if (newChild->getNodeType() == VS_NODE_TYPE_GEOMETRY)
    {
        childGeometry = (vsGeometry *)newChild;
        osgGroup->addChild(childGeometry->getBaseLibraryObject());
    }
    else if (newChild->getNodeType() == VS_NODE_TYPE_DYNAMIC_GEOMETRY)
    {
        childDynamicGeometry = (vsDynamicGeometry *)newChild;
        osgGroup->addChild(childDynamicGeometry->getBaseLibraryObject());
    }

    // Set the newChild node as our child
    child = newChild;
    newChild->ref();

    // Mark the entire tree above and below this node as needing an update
    newChild->dirty();
    
    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Remove a node from this node's child list
// ------------------------------------------------------------------------
int vsScene::removeChild(vsNode *targetChild)
{
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    vsDynamicGeometry *childDynamicGeometry;

    if (child != targetChild)
    {
        printf("vsScene::removeChild: 'targetChild' is not a child of "
            "this node\n");
        return VS_FALSE;
    }

    // Mark the entire portion of the tree that has any connection
    // to this node as needing of an update
    targetChild->dirty();
        
    // Detach the OSG nodes; checks for the type of the component because
    // the getBaseLibraryObject call is not virtual. The type can't be a
    // vsScene, because a scene node would never have a parent.
    if (targetChild->getNodeType() == VS_NODE_TYPE_COMPONENT)
    {
        childComponent = (vsComponent *)targetChild;
        osgGroup->removeChild(childComponent->getBaseLibraryObject());
    }
    else if (targetChild->getNodeType() == VS_NODE_TYPE_GEOMETRY)
    {
        childGeometry = (vsGeometry *)targetChild;
        osgGroup->removeChild(childGeometry->getBaseLibraryObject());
    }
    else if (targetChild->getNodeType() == 
                VS_NODE_TYPE_DYNAMIC_GEOMETRY)
    {
        childDynamicGeometry = (vsDynamicGeometry *)targetChild;
        osgGroup->removeChild(childDynamicGeometry->getBaseLibraryObject());
    }

    // Finish the VESS detachment
    child = NULL;
    targetChild->unref();
    
    // Check for errors as we remove this component from the
    // child's parent list
    if (targetChild->removeParent(this) == VS_FALSE)
        printf("vsScene::removeChild: Scene graph inconsistency: "
            "child to be removed does not have this component as "
            "a parent\n");

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Replace a node in this node's child list with a new node
// ------------------------------------------------------------------------
int vsScene::replaceChild(vsNode *targetChild, vsNode *newChild)
{
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    vsDynamicGeometry *childDynamicGeometry;
    osg::Node *oldNode, *newNode;

    if (child != targetChild)
    {
        printf("vsScene::replaceChild: 'targetChild' is not a child of "
            "this node\n");
        return VS_FALSE;
    }

    // Notify the newChild node that it is getting a new parent.
    // This might fail, as the child node is permitted to object to
    // getting a parent.
    if (newChild->addParent(this) == VS_FALSE)
    {
        printf("vsScene::replaceChild: 'newChild' node may not "
            "have any more parent nodes\n");
        return VS_FALSE;
    }

    // Mark the entire portion of the tree that has any connection
    // to the old node as needing of an update
    targetChild->dirty();
        
    // Replace the OSG nodes; checks for the type of the component because
    // the getBaseLibraryObject call is not virtual. The type can't be a
    // vsScene, because a scene node would never have a parent.
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
    else if (targetChild->getNodeType() == VS_NODE_TYPE_DYNAMIC_GEOMETRY)
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

    osgGroup->replaceChild(oldNode, newNode);

    // Change the connection in the VESS nodes
    child = newChild;
            
    targetChild->unref();
    newChild->ref();

    // Check for errors as we remove this component from the
    // child's parent list
    if (targetChild->removeParent(this) == VS_FALSE)
        printf("vsScene::replaceChild: Scene graph inconsistency: "
            "child to be removed does not have this component as "
            "a parent\n");

    // Mark the entire portion of the tree that has any connection
    // to the new node as needing of an update
    newChild->dirty();

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Retrieves the number of child nodes for this node
// ------------------------------------------------------------------------
int vsScene::getChildCount()
{
    if (child)
        return 1;

    return 0;
}

// ------------------------------------------------------------------------
// Retrieves one of the child nodes of this node, specified by index.
// The index of the first child is 0.
// ------------------------------------------------------------------------
vsNode *vsScene::getChild(int index)
{
    if (index != 0)
        return NULL;

    return child;
}

// ------------------------------------------------------------------------
// Retrieves the type of this node
// ------------------------------------------------------------------------
int vsScene::getNodeType()
{
    return VS_NODE_TYPE_SCENE;
}


// ------------------------------------------------------------------------
// Retrieves the center point and radius of a sphere that encompasses all
// of the geometry within this object.
// ------------------------------------------------------------------------
void vsScene::getBoundSphere(vsVector *centerPoint, double *radius)
{
    osg::BoundingSphere boundSphere;
    osg::Vec3 center;

    // Get the bounding sphere from OSG
    boundSphere = osgGroup->getBound();

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
// Returns the global transformation matrix of the scene
// ------------------------------------------------------------------------
vsMatrix vsScene::getGlobalXform()
{
    vsMatrix mat;

    mat.setIdentity();
    return mat;
}

// ------------------------------------------------------------------------
// Set the intersect value of this node
// ------------------------------------------------------------------------
void vsScene::setIntersectValue(unsigned int newValue)
{
    // Pass the intersect value to the OSG group as it's node mask
    osgGroup->setNodeMask(newValue);
}

// ------------------------------------------------------------------------
// Get the intersection value of this node
// ------------------------------------------------------------------------
unsigned int vsScene::getIntersectValue()
{
    // Get the node mask from the OSG group and return it
    return osgGroup->getNodeMask();
}

// ------------------------------------------------------------------------
// Adds the given attribute to the geometry object's list of child
// attributes. If successful, also notifies the attribute that it has been
// added to a list.
// ------------------------------------------------------------------------
void vsScene::addAttribute(vsAttribute *newAttribute)
{
    int attrCat, attrType;
    int loop;

    if (!(newAttribute->canAttach()))
    {
        printf("vsScene::addAttribute: Attribute is already in use\n");
        return;
    }

    attrCat = newAttribute->getAttributeCategory();
    if ((attrCat != VS_ATTRIBUTE_CATEGORY_STATE) &&
        (attrCat != VS_ATTRIBUTE_CATEGORY_OTHER))
    {
        printf("vsScene::addAttribute: Scene nodes may not contain "
            "attributes of that type\n");
        return;
    }

    attrType = newAttribute->getAttributeType();
    for (loop = 0; loop < getAttributeCount(); loop++)
        if ((getAttribute(loop))->getAttributeType() == attrType)
        {
            printf("vsScene::addAttribute: Scene node already "
                "contains that type of attribute\n");
            return;
        }

    // If we made it this far, it must be okay to add the attribute in
    vsNode::addAttribute(newAttribute);
}

// ------------------------------------------------------------------------
// Returns the OSG object associated with this object
// ------------------------------------------------------------------------
osg::Group *vsScene::getBaseLibraryObject()
{
    return osgGroup;
}

// ------------------------------------------------------------------------
// Internal function
// Adds the light to the light list and returns the index it was placed in.
// This index is to be used as the light number.
// ------------------------------------------------------------------------
int vsScene::addLight(vsLightAttribute *light)
{
    osg::StateSet *tempStateSet;
    int index;

    // Search for an open slot in the array.
    index = 0;
    while ((lightList[index] != NULL) && (index < VS_LIGHT_MAX))
    {
        index++;
    }

    // If it did not find one, set index to -1 so it returns that.
    if (index >= VS_LIGHT_MAX)
    {
        index = -1;
    }
    // Else set the light to that open slot.
    else
    {
        lightList[index] = light;

        tempStateSet = osgGroup->getOrCreateStateSet();
        tempStateSet->setMode(GL_LIGHT0+index, osg::StateAttribute::ON);
    }

    // Return the index we have determined.
    return(index);
}

// ------------------------------------------------------------------------
// Internal function
// Removes given light from the light list.
// ------------------------------------------------------------------------
void vsScene::removeLight(vsLightAttribute *light)
{
    osg::StateSet *tempStateSet;
    int index;

    // Search for the given light.
    index = 0;
    while ((lightList[index] != light) && (index < VS_LIGHT_MAX))
    {
        index++;
    }

    // If it found it, set its slot to NULL.
    if (index < VS_LIGHT_MAX)
    {
        lightList[index] = NULL;

        tempStateSet = osgGroup->getOrCreateStateSet();
        tempStateSet->setMode(GL_LIGHT0+index, osg::StateAttribute::OFF);
    }
}