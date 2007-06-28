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
//    VESS Module:  vsScene.c++
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
#include "vsSkeletonMeshGeometry.h++"
#include "vsTextureAttribute.h++"
#include "vsTextureCubeAttribute.h++"
#include "vsTextureRectangleAttribute.h++"
#include "vsTransformAttribute.h++"
#include "vsUnmanagedNode.h++"

// ------------------------------------------------------------------------
// Default Constructor - Set the light list and child pointer to NULL,
// meaning no lights and no child present.
// ------------------------------------------------------------------------
vsScene::vsScene()
{
    int loop;
    
    // Create a new OSG Group node and reference it
    osgGroup = new osg::Group();
    osgGroup->ref();

    // Initialize the list of global lights to empty.
    for (loop = 0; loop < VS_LIGHT_MAX; loop++)
    {       
        lightList[loop] = NULL;
    }

    // Set the child to NULL (none).
    child = NULL;

    // Set the child count to reflect that it has no children now.
    childCount = 0;

    // EarthSky is disabled by default.
    esEnabled = false;
}

// ------------------------------------------------------------------------
// Destructor - Informs any lights that may be in the light list that
// this scene is no longer valid.  Unref and remove the child.
// ------------------------------------------------------------------------
vsScene::~vsScene()
{
    int loop;
 
    // Remove any lights we may have in the lightList.
    for (loop = 0; loop < VS_LIGHT_MAX; loop++)
    {
        if (lightList[loop] != NULL)
        {
            lightList[loop]->setScene(NULL);
            lightList[loop] = NULL;
        }
    }

    // Remove all attributes
    deleteAttributes();

    // Remove all children
    deleteTree();

    // Unreference the OSG Group we created
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
// Add a node to this node's child list
// ------------------------------------------------------------------------
bool vsScene::addChild(vsNode *newChild)
{
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    vsDynamicGeometry *childDynamicGeometry;
    vsSkeletonMeshGeometry *childSkeletonMeshGeometry;
    vsUnmanagedNode *childUnmanagedNode;

    // Make sure we don't already have a child
    if (child)
    {
        printf("vsScene::addChild: Scene object already has a child\n");
        return false;
    }

    // Notify the newChild node that it is getting a new parent. This might
    // fail, as the child node is permitted to object to getting a parent.
    if (newChild->addParent(this) == false)
    {
        printf("vsScene::addChild: 'newChild' node may not have any "
            "more parent nodes\n");
        return false;
    }

    // Connect the OSG nodes together. This differs by type because the
    // getBaseLibaryObject() method is not virtual.  The type can't be 
    // a vsScene, because a scene node would never consent to getting 
    // a parent.
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
    else if (newChild->getNodeType() == VS_NODE_TYPE_SKELETON_MESH_GEOMETRY)
    {
        childSkeletonMeshGeometry = (vsSkeletonMeshGeometry *)newChild;
        osgGroup->addChild(childSkeletonMeshGeometry->getBaseLibraryObject());
    }
    else if (newChild->getNodeType() == VS_NODE_TYPE_UNMANAGED)
    {
        childUnmanagedNode = (vsUnmanagedNode *)newChild;
        osgGroup->addChild(childUnmanagedNode->getBaseLibraryObject());
    }

    // Set the newChild node as our child
    child = newChild;
    newChild->ref();

    // Mark the entire tree above and below this node as needing an update
    newChild->dirty();
    
    // Return success
    return true;
}

// ------------------------------------------------------------------------
// Insert a node into this node's child list at the specified index
// ------------------------------------------------------------------------
bool vsScene::insertChild(vsNode *newChild, int index)
{
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    vsDynamicGeometry *childDynamicGeometry;
    vsSkeletonMeshGeometry *childSkeletonMeshGeometry;
    vsUnmanagedNode *childUnmanagedNode;

    // Make sure we don't already have a child
    if (child)
    {
        printf("vsScene::insertChild: Scene object already has a child\n");
        return false;
    }
    
    // Make sure the index is valid (only 0 is allowed for vsScenes)
    if (index != 0)
    {
        printf("vsScene::insertChild: Invalid index\n");
        return false;
    }

    // Notify the newChild node that it is getting a new parent. This might
    // fail, as the child node is permitted to object to getting a parent.
    if (newChild->addParent(this) == false)
    {
        printf("vsScene::insertChild: 'newChild' node may not have any "
            "more parent nodes\n");
        return false;
    }

    // Connect the OSG nodes together. This differs by type because the
    // getBaseLibaryObject() method is not virtual.  The type can't be 
    // a vsScene, because a scene node would never consent to getting 
    // a parent.
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
    else if (newChild->getNodeType() == VS_NODE_TYPE_SKELETON_MESH_GEOMETRY)
    {
        childSkeletonMeshGeometry = (vsSkeletonMeshGeometry *)newChild;
        osgGroup->addChild(childSkeletonMeshGeometry->getBaseLibraryObject());
    }
    else if (newChild->getNodeType() == VS_NODE_TYPE_UNMANAGED)
    {
        childUnmanagedNode = (vsUnmanagedNode *)newChild;
        osgGroup->addChild(childUnmanagedNode->getBaseLibraryObject());
    }

    // Set the newChild node as our child
    child = newChild;
    newChild->ref();

    // Mark the entire tree above and below this node as needing an update
    newChild->dirty();
    
    return true;
}

// ------------------------------------------------------------------------
// Remove a node from this node's child list
// ------------------------------------------------------------------------
bool vsScene::removeChild(vsNode *targetChild)
{
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    vsDynamicGeometry *childDynamicGeometry;
    vsSkeletonMeshGeometry *childSkeletonMeshGeometry;
    vsUnmanagedNode *childUnmanagedNode;

    // Make sure the target child is actually our child
    if (child != targetChild)
    {
        printf("vsScene::removeChild: 'targetChild' is not a child of "
            "this node\n");
        return false;
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
    else if (targetChild->getNodeType() == 
                VS_NODE_TYPE_SKELETON_MESH_GEOMETRY)
    {
        childSkeletonMeshGeometry = (vsSkeletonMeshGeometry *)targetChild;
        osgGroup->removeChild(
            childSkeletonMeshGeometry->getBaseLibraryObject());
    }
    else if (targetChild->getNodeType() == VS_NODE_TYPE_UNMANAGED)
    {
        childUnmanagedNode = (vsUnmanagedNode *)targetChild;
        osgGroup->removeChild(childUnmanagedNode->getBaseLibraryObject());
    }

    // Finish the VESS detachment
    child = NULL;
    targetChild->unref();
    
    // Check for errors as we remove this component from the
    // child's parent list
    if (targetChild->removeParent(this) == false)
        printf("vsScene::removeChild: Scene graph inconsistency: "
            "child to be removed does not have this component as "
            "a parent\n");

    // Return success
    return true;
}

// ------------------------------------------------------------------------
// Replace a node in this node's child list with a new node
// ------------------------------------------------------------------------
bool vsScene::replaceChild(vsNode *targetChild, vsNode *newChild)
{
    vsComponent *childComponent;
    vsGeometry *childGeometry;
    vsDynamicGeometry *childDynamicGeometry;
    vsSkeletonMeshGeometry *childSkeletonMeshGeometry;
    vsUnmanagedNode *childUnmanagedNode;
    osg::Node *oldNode, *newNode;

    // Make sure the target child is actually our child
    if (child != targetChild)
    {
        printf("vsScene::replaceChild: 'targetChild' is not a child of "
            "this node\n");
        return false;
    }

    // Notify the newChild node that it is getting a new parent.
    // This might fail, as the child node is permitted to object to
    // getting a parent.
    if (newChild->addParent(this) == false)
    {
        printf("vsScene::replaceChild: 'newChild' node may not "
            "have any more parent nodes\n");
        return false;
    }

    // Mark the entire portion of the tree that has any connection
    // to the old node as needing of an update
    targetChild->dirty();
        
    // Replace the OSG nodes; checks for the type of the component because
    // the getBaseLibraryObject call is not virtual. The type can't be a
    // vsScene, because a scene node would never have a parent.
    // First, get the old child node.
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
    else if (targetChild->getNodeType() == VS_NODE_TYPE_SKELETON_MESH_GEOMETRY)
    {
        childSkeletonMeshGeometry = (vsSkeletonMeshGeometry *)targetChild;
        oldNode = childSkeletonMeshGeometry->getBaseLibraryObject();
    }
    else if (targetChild->getNodeType() == VS_NODE_TYPE_UNMANAGED)
    {
        childUnmanagedNode = (vsUnmanagedNode *)targetChild;
        oldNode = childUnmanagedNode->getBaseLibraryObject();
    }

    // Next, get the new child node.
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

    // Finally, replace the child on the osg::Group node
    osgGroup->replaceChild(oldNode, newNode);

    // Change the connection in the VESS nodes
    child = newChild;
            
    // Update reference counts
    targetChild->unref();
    newChild->ref();

    // Check for errors as we remove this component from the
    // child's parent list
    if (targetChild->removeParent(this) == false)
        printf("vsScene::replaceChild: Scene graph inconsistency: "
            "child to be removed does not have this component as "
            "a parent\n");

    // Mark the entire portion of the tree that has any connection
    // to the new node as needing of an update
    newChild->dirty();

    // Return success
    return true;
}

// ------------------------------------------------------------------------
// Retrieves the number of child nodes for this node
// ------------------------------------------------------------------------
int vsScene::getChildCount()
{
    // Return 1 if we have a child
    if (child)
        return 1;

    // Otherwise, return 0
    return 0;
}

// ------------------------------------------------------------------------
// Retrieves one of the child nodes of this node, specified by index.
// The index of the first child is 0.
// ------------------------------------------------------------------------
vsNode *vsScene::getChild(int index)
{
    // If the index is not zero, it can't be valid.  Just return NULL if
    // this is the case
    if (index != 0)
        return NULL;

    // Return the child pointer (NULL or otherwise)
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

    // Convert the center to a vsVector if the centerPoint pointer is
    // valid
    if (centerPoint)
    {
        center = boundSphere.center();
        centerPoint->set(center[0], center[1], center[2]);
    }

    // Fetch and return the radius if the pointer is valid
    if (radius)
        *radius = boundSphere.radius();
}

// ------------------------------------------------------------------------
// Returns the global transformation matrix of the scene
// ------------------------------------------------------------------------
vsMatrix vsScene::getGlobalXform()
{
    vsMatrix mat;

    // A scene's transform will always be identity, since it is the
    // root node of the scene graph
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
    int newAttrType, newAttrCat;
    int attrType;
    int loop;
    int textureUnit, newTextureUnit;
    vsAttribute *attribute;

    // See if the attribute will let us attach it
    if (!(newAttribute->canAttach()))
    {
        printf("vsScene::addAttribute: Attribute is already in use\n");
        return;
    }

    // Scenes may not receive grouping, transform, or container attributes
    // (primarily because these don't make sense at the root of a scene)
    newAttrCat = newAttribute->getAttributeCategory();
    if ((newAttrCat != VS_ATTRIBUTE_CATEGORY_STATE) &&
        (newAttrCat != VS_ATTRIBUTE_CATEGORY_OTHER))
    {
        printf("vsScene::addAttribute: Scene nodes may not contain "
            "attributes of that type\n");
        return;
    }

    // Make sure we're not attaching more than one of the same type of 
    // attribute
    newAttrType = newAttribute->getAttributeType();
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
                printf("vsScene::addAttribute: Scene node "
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
                printf("vsScene::addAttribute: Scene node already "
                    "contains that type of attribute\n");
                return;
            }

    // If we made it this far, it must be okay to add the attribute in
    vsNode::addAttribute(newAttribute);
}

// ------------------------------------------------------------------------
// Enables lighting on the scene. This is a recursive call, and will
// pass through all geometry and components in the scene graph.
// ------------------------------------------------------------------------
void vsScene::enableLighting()
{
    // If the child is valid, enable lighting.
    if(child != NULL)
    {
        child->enableLighting();
    }
}

// ------------------------------------------------------------------------
// Disables lighting on the scene. This is a recursive call, and will
// pass through all geometry and components in the scene graph.
// ------------------------------------------------------------------------
void vsScene::disableLighting()
{
    // If the child is valid, disable lighting.
    if(child != NULL)
    {
        child->disableLighting();
    } 
}

// ------------------------------------------------------------------------
// Enables drawing of the earth/sky background in this channel
// ------------------------------------------------------------------------
void vsScene::enableEarthSky()
{
    esEnabled = true;
}

// ------------------------------------------------------------------------
// Disables drawing of the earth/sky background in this channel
// ------------------------------------------------------------------------
void vsScene::disableEarthSky()
{
    esEnabled = false;
}

// ------------------------------------------------------------------------
// Returns whether the drawing pane should use the vsScene EarthSky colors.
// ------------------------------------------------------------------------
bool vsScene::isEarthSkyEnabled()
{
    return esEnabled;
}

// ------------------------------------------------------------------------
// Sets the altitude of the ground plane in the earth/sky background
// ------------------------------------------------------------------------
void vsScene::setESGroundHeight(double newHeight)
{
    // No earth/sky ground height support in OSG, do nothing.
}

// ------------------------------------------------------------------------
// Retrieves the altitude of the ground plane in the earth/sky background
// ------------------------------------------------------------------------
double vsScene::getESGroundHeight()
{
    // No earth/sky support in OSG, so just return 0.0 as the ground height.
    return 0.0;
}

// ------------------------------------------------------------------------
// Sets the aspect of the earth/sky background color specified by which to
// the specified color
// ------------------------------------------------------------------------
void vsScene::setESColor(vsSceneEarthSkyColor which, double r, double g,
    double b)
{
    // Only the UNIFORM color is supported in OSG.
    if (which == VS_SCENE_ESCOLOR_UNIFORM)
    {
        esUniformColor.set(r, g, b, 1.0);
    }
}

// ------------------------------------------------------------------------
// Retrieves the aspect of the earth/sky background color specified by
// which. NULL pointers may be passed in for unneeded return values.
// ------------------------------------------------------------------------
void vsScene::getESColor(vsSceneEarthSkyColor which, double *r, double *g,
    double *b)
{
    if (which == VS_SCENE_ESCOLOR_UNIFORM)
    {
        // Return each non-NULL parameter
        if (r != NULL)
            *r = esUniformColor[0];
        if (g != NULL)
            *g = esUniformColor[1];
        if (b != NULL)
            *b = esUniformColor[2];
    }
    else
    {
        // Return each non-NULL parameter
        if (r != NULL)
            *r = 0.0;
        if (g != NULL)
            *g = 0.0;
        if (b != NULL)
            *b = 0.0;
    }
}

// ------------------------------------------------------------------------
// Enables culling on this node and its children
// ------------------------------------------------------------------------
void vsScene::enableCull()
{
    // If the child is valid, enable culling
    if (child != NULL)
    {
        child->enableCull();
    }
}

// ------------------------------------------------------------------------
// Disables culling on this node and its children
// ------------------------------------------------------------------------
void vsScene::disableCull()
{
    // If the child is valid, disable culling
    if (child != NULL)
    {
        child->disableCull();
    }
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
        // Set the light at the index we found to the given light
        lightList[index] = light;

        // At the current node's StateSet, turn the corresponding OpenGL
        // light on
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

    // Search for the given light, keep track of the list index
    index = 0;
    while ((lightList[index] != light) && (index < VS_LIGHT_MAX))
    {
        index++;
    }

    // If it found it, set its slot to NULL.
    if (index < VS_LIGHT_MAX)
    {
        // Set the light at the index we found to NULL
        lightList[index] = NULL;

        // On the current node's StateSet, turn the OpenGL light at the 
        // index we found to OFF
        tempStateSet = osgGroup->getOrCreateStateSet();
        tempStateSet->setMode(GL_LIGHT0+index, osg::StateAttribute::OFF);
    }
}

// ------------------------------------------------------------------------
// Internal function
// Recursively finds the topLeft and bottomRight of the geometry that is
// represented by this scene (all the objects in the children list).
// ------------------------------------------------------------------------
void vsScene::getAxisAlignedBoxBounds(vsVector  *minValues,
    vsVector *maxValues)
{
    int childCount = getChildCount();
    int cntChild;
    int dataCount;
    int cntGData;
    int column, row;
    vsVector tempMinValues;
    vsVector tempMaxValues;
    vsVector passMinValues;
    vsVector passMaxValues;
    vsVector oldPoint;
    vsVector newPoint;
    vsTransformAttribute *transform = NULL;
    vsMatrix dynamicMatrix;
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

    // Set the min and max values for this scene
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
