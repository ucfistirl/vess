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
//    VESS Module:  vsSkeleton.c++
//
//    Description:  This file manages the bone subgraph.  It maintains
//                  the boneIDs from Cal3D and generates the matrices
//                  that represent each bone.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#include "vsSkeleton.h++"

#include "vsTransformAttribute.h++"

//------------------------------------------------------------------------
// Constructor.
//------------------------------------------------------------------------
vsSkeleton::vsSkeleton(atArray *componentList, int listLength,
                       vsComponent *root)
{
    vsNode *parent;
    vsComponent *bone;
    int i;

    // Keep a reference to the component map.
    skeletonComponentMap = componentList;
    for (i = 0; i < skeletonComponentMap->getNumEntries(); i++)
    {
        // Get and reference the i'th bone (if it exists)
        bone = (vsComponent *)skeletonComponentMap->getEntry(i);
        bone->ref();
    }

    // Store the size of the kinematics map, which is the number of bones.
    boneCount = listLength;

    // Keep a reference to the root node of the bone subgraph.
    skeletonRoot = root;
    skeletonRoot->ref();

    // Initialize the last found index to 0, the beginning.
    // This variable is just used to slightly attempt to speed up lookups.
    // It is not a significant thing, but it is simple and easy to do.
    lastFoundIndex = 0;

    // Create the array to store the bone matrices.
    skeletonMatrices = new atArray();

    // Set the offset to identity (nothing).
    offsetMatrix.setIdentity();

    // Generate the list of bone matrices to be used for skinning.
    update();
}

//------------------------------------------------------------------------
// Copy constructor.
//------------------------------------------------------------------------
vsSkeleton::vsSkeleton(vsSkeleton *original)
{
    int index;
    atMatrix *skelMat;
    atMatrix *newSkelMat;
    
    // Copy primitive values.
    boneCount = original->boneCount;
    lastFoundIndex = original->lastFoundIndex;
    offsetMatrix = original->offsetMatrix;
    
    // Clone the subgraph nodes.
    skeletonRoot = (vsComponent*)(original->skeletonRoot->cloneTree());
    skeletonRoot->ref();
    
    // Copy the skeletonComponentMap atArray
    skeletonComponentMap = new atArray();
    copySkeletonTree(skeletonRoot, original->skeletonRoot,
        original->skeletonComponentMap); 
    
    // Copy the skeletonMatrices array
    skeletonMatrices = new atArray();
    for (index = 0; index < original->skeletonMatrices->getNumEntries();
         index++)
    {
        skelMat = (atMatrix*)(original->skeletonMatrices->getEntry(index));
        
        // Make sure it isn't NULL - if it is, we've reached the end of the
        // list, so break.
        if(skelMat == NULL)
            break;
            
        newSkelMat = new atMatrix();
        newSkelMat->copy(*skelMat);
        skeletonMatrices->setEntry(index, newSkelMat);
    }
    
    update();
}

//------------------------------------------------------------------------
// Destructor.
//------------------------------------------------------------------------
vsSkeleton::~vsSkeleton()
{
    int i;
    vsComponent *bone;

    // Delete the root node which will in turn delete all the children.
    vsObject::unrefDelete(skeletonRoot);

    // Unreference (and possibly delete) all bones in the component map
    for (i = 0; i < skeletonComponentMap->getNumEntries(); i++)
    {
        // Get and unreference/delete the i'th bone (if it exists)
        bone = (vsComponent *)skeletonComponentMap->getEntry(i);
        if (bone != NULL)
        {
            vsObject::unrefDelete(bone);
            skeletonComponentMap->setEntry(i, NULL);
        }
    }

    // Delete the component map, now that it's empty
    delete skeletonComponentMap;

    // Delete the matrix list as well
    delete skeletonMatrices;
}

//------------------------------------------------------------------------
// Private helper recursive function to copy the skeleton component map.
// Currently called just from the copy constructor.
//------------------------------------------------------------------------
void vsSkeleton::copySkeletonTree(vsNode *newNode, vsNode *origNode, 
                                  atArray *origMap)
{
    int index;
    vsNode *trav;
    
    // Initial check: Make sure they have the same number of children.
    if(newNode->getChildCount() != origNode->getChildCount())
    {
        // Return, nothing we can do.
        return;
    }
    
    // Look through the original map to find the pointer to the original node.
    for (index = 0; index < origMap->getNumEntries(); index++)
    {
        trav = (vsNode*)(origMap->getEntry(index));
        
        // Make sure the array didn't return NULL; if it did, we're at the
        // end of the list, so break from this search loop, nothing we can do.
        if(trav == NULL)
            break;
        
        // Compare the two pointers for equality.
        if(origNode == trav)
        {
            // So, the origNode we're at is mapped into the array at this
            // index. So, put the node we have here into this object's
            // map.
            newNode->ref();
            skeletonComponentMap->setEntry(index, newNode);
        }
    }
    
    // Recursively call for the two nodes' children.
    for (index = 0; index < newNode->getChildCount(); index++)
    {
        // Make sure that both of the child nodes at this index are components.
        if(newNode->getChild(index)->getNodeType() == VS_NODE_TYPE_COMPONENT &&
           origNode->getChild(index)->getNodeType() == VS_NODE_TYPE_COMPONENT)
        {
            copySkeletonTree(newNode->getChild(index),
                             origNode->getChild(index),
                             origMap);
        }
    }
}
        
//------------------------------------------------------------------------
// Recursive function to update the bone matrices.  See the public call
// for more information.
//------------------------------------------------------------------------
void vsSkeleton::updateMatrices(vsNode *node, atMatrix currentMatrix)
{
    vsComponent           *component;
    vsNode                *childNode;
    atMatrix              *boneMatrix;
    atMatrix              localMatrix;
    vsTransformAttribute  *transform;
    int                   childCount;
    int                   index;

    // If this is a component, traverse its children and update the bone
    // matrices thus far.
    if (node->getNodeType() == VS_NODE_TYPE_COMPONENT)
    {
        component = (vsComponent *) node;

        // Multiply our passed in matrix by the current node's.
        transform = (vsTransformAttribute *)
            component->getTypedAttribute(VS_ATTRIBUTE_TYPE_TRANSFORM, 0);
        if (transform)
        {
            // Multiply this Transform's matrix into the accumulated
            // transform
            localMatrix = transform->getCombinedTransform();
            currentMatrix.postMultiply(localMatrix);
        }

        // If this component has a valid bone ID, then update its
        // respective matrix in the array.
        if ((index = getBoneID(component)) > -1)
        {
            // Get the matrix pointer, if it is NULL create a matrix object.
            boneMatrix = (atMatrix *) skeletonMatrices->getEntry(index);
            if (!boneMatrix)
            {
                boneMatrix = new atMatrix();
                skeletonMatrices->setEntry(index, boneMatrix);
            }

            // Store the currently accumulated transform for this bone in
            // the matrix list.
            *boneMatrix = currentMatrix;
        }

        // Get children and traverse them.
        childCount = component->getChildCount();
        for (index = 0; index < childCount; index++)
        {
            childNode = component->getChild(index);
            updateMatrices(childNode, currentMatrix);
        }
    }
}

//------------------------------------------------------------------------
// Gets a string representation of this object's class name.
//------------------------------------------------------------------------
const char *vsSkeleton::getClassName()
{
    return "vsSkeleton";
}

//------------------------------------------------------------------------
// Return the vsComponent that represents the given bone.
//------------------------------------------------------------------------
vsComponent *vsSkeleton::getBone(int boneID)
{
    vsComponent *returnValue;

    returnValue = NULL;

    // If given a valid boneID, return the vsComponent for that boneID.
    if ((boneID < boneCount) && (boneID >= 0))
    {
        returnValue = ((vsComponent *) skeletonComponentMap->getEntry(boneID));
    }

    return returnValue;
}

//------------------------------------------------------------------------
// Return the absolute matrix for the given bone.
//------------------------------------------------------------------------
atMatrix *vsSkeleton::getBoneMatrix(int boneID)
{
    atMatrix *returnValue;

    returnValue = NULL;

    // If given a valid boneID, return the atMatrix for that boneID.
    if ((boneID < boneCount) && (boneID >= 0))
    {
        returnValue = (atMatrix *) skeletonMatrices->getEntry(boneID);
    }

    return returnValue;
}

//------------------------------------------------------------------------
// Return the list of absolute bone matrices.  This is used to transfrom
// the vertices of the skin.
//------------------------------------------------------------------------
atArray *vsSkeleton::getBoneMatrixList()
{
    return skeletonMatrices;
}

//------------------------------------------------------------------------
// Return the bone ID for the bone represented by the given vsComponent.
// The bone ID is the index into the matrix array for the bone.
//------------------------------------------------------------------------
int vsSkeleton::getBoneID(vsComponent *component)
{
    int      index;
    bool     found;

    found = false;

    // Search for the given vsComponent in the map, starting at where we last
    // found something.
    for (index = lastFoundIndex; ((!found) && (index < boneCount)); index++)
    {
        if (component == 
            ((vsComponent *) skeletonComponentMap->getEntry(index)))
        {
            found = true;

            // Update our last found index to the found one, in case our next
            // query will be the next entry in the array.
            lastFoundIndex = index;
        }
    }

    // If we did not find anything, then search the parts we skipped.
    for (index = 0; ((!found) && (index < lastFoundIndex)); index++)
    {
        if (component == 
            ((vsComponent *) skeletonComponentMap->getEntry(index)))
        {
            found = true;

            // Update our last found index to the found one, in case our next
            // query will be the next entry in the array.
            lastFoundIndex = index;
        }
    }

    // If we have not found anything, set the return index to -1.
    if (!found)
    {
        index = -1;
    }
    else
    {
        index = lastFoundIndex;
    }

    return index;
}

//------------------------------------------------------------------------
// Return the bone ID for the named bone.  The bone ID is the index into
// the matrix array for the bone.
//------------------------------------------------------------------------
int vsSkeleton::getBoneID(char *boneName)
{
    int      index;
    bool     found;

    found = false;

    // Search for the given component name in the map, starting at where we
    // last found something.
    for (index = lastFoundIndex; ((!found) && (index < boneCount)); index++)
    {
        if (!strcmp(boneName, ((vsComponent *)
            skeletonComponentMap->getEntry(index))->getName()))
        {
            found = true;

            // Update our last found index to the found one, in case our next
            // query will be the next entry in the array.
            lastFoundIndex = index;
        }
    }

    // If we did not find anything, then search the parts we skipped.
    for (index = 0; ((!found) && (index < lastFoundIndex)); index++)
    {
        if (!strcmp(boneName, ((vsComponent *)
            skeletonComponentMap->getEntry(index))->getName()))
        {
            found = true;

            // Update our last found index to the found one, in case our next
            // query will be the next entry in the array.
            lastFoundIndex = index;
        }
    }

    // If we have not found anything, set the return index to -1.
    if (!found)
    {
        index = -1;
    }
    else
    {
        index = lastFoundIndex;
    }

    return index;
}

//------------------------------------------------------------------------
// Return the root bone component, the subgraph.  This must be attached
// to the scenegraph if trying to visualize bone lines.  To perform normal
// skinning however, it can be ignored.
//------------------------------------------------------------------------
vsComponent *vsSkeleton::getRoot()
{
    return skeletonRoot;
}

//------------------------------------------------------------------------
// Return the number of bones this object manages.
//------------------------------------------------------------------------
int vsSkeleton::getBoneCount()
{
    return boneCount;
}

//------------------------------------------------------------------------
// Recursive function to make the actual bone lines.  See the public call
// for more information.
//------------------------------------------------------------------------
void vsSkeleton::makeBoneGeometry(vsComponent *currentBone,
                                  vsGeometry *currentBoneLine)
{
    atMatrix boneMatrix;
    atVector point;
    vsGeometry *newBoneLine;
    vsTransformAttribute *boneTrans;
    vsNode *currentBoneChild;
    int childCount;
    int boneID;

    // If the given bone line is defined, finish constructing it.
    if (currentBoneLine)
    {
        // Get the transform attribute to this bone.
        boneTrans = (vsTransformAttribute *) currentBone->getTypedAttribute(
            VS_ATTRIBUTE_TYPE_TRANSFORM, 0);

        // Transform the origin to where the bone is defined, this
        // will be the end point of the line.
        boneMatrix = boneTrans->getCombinedTransform();
        point = boneMatrix.getPointXform(atVector(0.0, 0.0, 0.0));
        currentBoneLine->setData(VS_GEOMETRY_VERTEX_COORDS, 1, point);
    }

    // Process each of the children.
    childCount = currentBone->getChildCount();
    for (childCount--; childCount >= 0; childCount--)
    {
        // Only process if they are vsComponents.
        currentBoneChild = currentBone->getChild(childCount);
        if (currentBoneChild->getNodeType() == VS_NODE_TYPE_COMPONENT)
        {
            // Create the new line.
            newBoneLine = new vsGeometry();
                                                                                
            // Set up the lines.
            newBoneLine->setPrimitiveType(VS_GEOMETRY_TYPE_LINES);
            newBoneLine->setPrimitiveCount(1);

            // Tell it there are two vertices.
            newBoneLine->setDataListSize(VS_GEOMETRY_VERTEX_COORDS, 2);
                                                                                
            // Setup color data information.
            newBoneLine->setBinding(VS_GEOMETRY_COLORS,
                VS_GEOMETRY_BIND_OVERALL);
            newBoneLine->setDataListSize(VS_GEOMETRY_COLORS, 1);

            // Attempt to color the bone with a fairly unique color
            // compared to its neighbours.
            boneID = getBoneID(currentBone);
            newBoneLine->setData(VS_GEOMETRY_COLORS, 0, atVector(
                (double) (((boneID) % 3) == 1),
                (double) (((boneID + 1) % 3) == 1),
                (double) (((boneID + 2) % 3) == 1),
                1.0));

            // Set the start point of the line.
            point = atVector(0.0, 0.0, 0.0);
            newBoneLine->setData(VS_GEOMETRY_VERTEX_COORDS, 0, point);

            // Add the geometry.
            currentBone->addChild(newBoneLine);

            // Recursively continue to other bones.
            makeBoneGeometry((vsComponent *)currentBoneChild, newBoneLine);
        }
    }
}

//------------------------------------------------------------------------
// Makes a line to represent each bone and adds the geometry to the
// bone subgraph.  If it is to be displayed then the subgraph needs to
// be attached to the scenegraph.
//------------------------------------------------------------------------
void vsSkeleton::makeBoneGeometry()
{
    // Forward to the recursive function.
    makeBoneGeometry(skeletonRoot, NULL);
}

//------------------------------------------------------------------------
// Set an offset matrix which will be applied to the whole skeleton.
// Can be used to transform the skeleton and in turn the mesh applied to it.
//------------------------------------------------------------------------
void vsSkeleton::setOffsetMatrix(atMatrix newOffsetMatrix)
{
    offsetMatrix = newOffsetMatrix;
}

//------------------------------------------------------------------------
// Return the currently set offset matrix.
//------------------------------------------------------------------------
atMatrix vsSkeleton::getOffsetMatrix()
{
    return offsetMatrix;
}

//------------------------------------------------------------------------
// Update call that regenerates the matrices that represent each bone.
// These are used for the final skinning process.  Must be called when
// ever the bones are moved in order to reflect the change.
//------------------------------------------------------------------------
void vsSkeleton::update()
{
    // Forward to the recursive update call.  The stack can make the update
    // more efficient by storing previously calculated matrices instead
    // of recalculating them for each bone.
    updateMatrices(skeletonRoot, offsetMatrix);
}
