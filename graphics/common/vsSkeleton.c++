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
vsSkeleton::vsSkeleton(vsGrowableArray *componentList,
                       vsGrowableArray *boneSpaceMatrixList, int listLength,
                       vsComponent *root)
{
    int index;

    // Keep a reference to the component map.
    skeletonComponentMap = componentList;

    // Keep a reference to the list of bone space matrices.
    skeletonBoneSpaceMatrices = boneSpaceMatrixList;

    // Store the size of the kinematics map, which is the number of bones.
    boneCount = listLength;

    // Keep a reference to the root node of the bone subgraph.
    skeletonRootBone = root;

    // Make a root node to hold the skeleton and a transform to modify the
    // skeleton with the offsetMatrix.
    skeletonRoot = new vsComponent();
    skeletonRoot->ref();
    skeletonRoot->addChild(skeletonRootBone);
    skeletonTransform = new vsTransformAttribute();
    skeletonRoot->addAttribute(skeletonTransform);

    // Initialize the last found index to 0, the beginning.
    // This variable is just used to slightly attempt to speed up lookups.
    // It is not a significant thing, but it is simple and easy to do.
    lastFoundIndex = 0;

    // Create the array to store the bone matrices.
    skeletonMatrices = new vsGrowableArray(boneCount, 5);

    // Create the array to store the inverse transpose bone matrices.
    skeletonITMatrices = new vsGrowableArray(boneCount, 5);

    // Set the offset to identity (nothing).
    offsetMatrix.setIdentity();

    // Set the offset matrix to the skeletonTransform.
    skeletonTransform->setDynamicTransform(offsetMatrix);

    // Generate the list of bone matrices to be used for skinning.
    update();
}

//------------------------------------------------------------------------
// Destructor.
//------------------------------------------------------------------------
vsSkeleton::~vsSkeleton()
{
    int index;

    // Delete the root node which will in turn delete all the children.
    vsObject::unrefDelete(skeletonRoot);

    // Delete objects in the arrays with references counts of zero.
    for (index = 0; index < boneCount; index++)
    {
        delete ((vsMatrix *) skeletonMatrices->getData(index));
        delete ((vsMatrix *) skeletonITMatrices->getData(index));
        delete ((vsMatrix *) skeletonBoneSpaceMatrices->getData(index));
    }

    // Delete objects with references counts of zero.
    delete skeletonComponentMap;
    delete skeletonMatrices;
    delete skeletonITMatrices;
    delete skeletonBoneSpaceMatrices;
}

//------------------------------------------------------------------------
// Recursive function to update the bone matrices.  See the public call
// for more information.
//------------------------------------------------------------------------
void vsSkeleton::updateMatrices(vsNode *node, vsMatrix currentMatrix)
{
    vsComponent           *component;
    vsNode                *childNode;
    vsMatrix              *absoluteMatrix;
    vsMatrix              *ITAbsoluteMatrix;
    vsMatrix              *boneSpaceMatrix;
    vsMatrix              boneMatrix;
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
            boneMatrix = transform->getCombinedTransform();
            currentMatrix.postMultiply(boneMatrix);
        }

        // If this component has a valid bone ID, then update its
        // respective matrix in the array.
        if ((index = getBoneID(component)) > -1)
        {
            // Get the matrix pointer, if it is NULL create a matrix object.
            absoluteMatrix = (vsMatrix *) skeletonMatrices->getData(index);
            if (!absoluteMatrix)
            {
                absoluteMatrix = new vsMatrix();
                skeletonMatrices->setData(index, absoluteMatrix);
            }

            // Get the bone space matrix for this bone.
            boneSpaceMatrix = (vsMatrix *)
                skeletonBoneSpaceMatrices->getData(index);

            // Calculate the final absolute matrix for this bone and store
            // it in the matrix list.
            *absoluteMatrix = currentMatrix * (*boneSpaceMatrix);

            // Get the matrix pointer, if it is NULL create a matrix object.
            ITAbsoluteMatrix = (vsMatrix *) skeletonITMatrices->getData(index);
            if (!ITAbsoluteMatrix)
            {
                ITAbsoluteMatrix = new vsMatrix();
                skeletonITMatrices->setData(index, ITAbsoluteMatrix);
            }

            // Calculate the inverse transpose to the absolute and store it in
            // the matrix list.
            *ITAbsoluteMatrix = absoluteMatrix->getInverse();
            ITAbsoluteMatrix->transpose();
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
        returnValue = ((vsComponent *) skeletonComponentMap->getData(boneID));
    }

    return returnValue;
}

//------------------------------------------------------------------------
// Return the absolute matrix for the given bone.
//------------------------------------------------------------------------
vsMatrix *vsSkeleton::getBoneMatrix(int boneID)
{
    vsMatrix *returnValue;

    returnValue = NULL;

    // If given a valid boneID, return the vsMatrix for that boneID.
    if ((boneID < boneCount) && (boneID >= 0))
    {
        returnValue = (vsMatrix *) skeletonMatrices->getData(boneID);
    }

    return returnValue;
}

//------------------------------------------------------------------------
// Return the inverse transposed absolute bone matrix for the given bone.
//------------------------------------------------------------------------
vsMatrix *vsSkeleton::getITBoneMatrix(int boneID)
{
    vsMatrix *returnValue;

    returnValue = NULL;

    // If given a valid boneID, return the vsMatrix for that boneID.
    if ((boneID < boneCount) && (boneID >= 0))
    {
        returnValue = (vsMatrix *) skeletonITMatrices->getData(boneID);
    }

    return returnValue;
}

//------------------------------------------------------------------------
// Return the Cal3D bone space matrix for the given bone.
//------------------------------------------------------------------------
vsMatrix *vsSkeleton::getBoneSpaceMatrix(int boneID)
{
    vsMatrix *returnValue;

    returnValue = NULL;

    // If given a valid boneID, return the vsMatrix for that boneID.
    if ((boneID < boneCount) && (boneID >= 0))
    {
        returnValue = (vsMatrix *) skeletonBoneSpaceMatrices->getData(boneID);
    }

    return returnValue;
}

//------------------------------------------------------------------------
// Return the list of absolute bone matrices.  This is used to transfrom
// the vertices of the skin.
//------------------------------------------------------------------------
vsGrowableArray *vsSkeleton::getBoneMatrixList()
{
    return skeletonMatrices;
}

//------------------------------------------------------------------------
// Return the list of inverse transposed absolute bone matrices.
// This is used to transfrom the normals for each vertex of the skin.
//------------------------------------------------------------------------
vsGrowableArray *vsSkeleton::getITBoneMatrixList()
{
    return skeletonITMatrices;
}

//------------------------------------------------------------------------
// Return the list of bone space matrices.
//------------------------------------------------------------------------
vsGrowableArray *vsSkeleton::getBoneSpaceMatrixList()
{
    return skeletonBoneSpaceMatrices;
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
        if (component == ((vsComponent *) skeletonComponentMap->getData(index)))
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
        if (component == ((vsComponent *) skeletonComponentMap->getData(index)))
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
            skeletonComponentMap->getData(index))->getName()))
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
            skeletonComponentMap->getData(index))->getName()))
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
        currentBoneLine->setData(VS_GEOMETRY_VERTEX_COORDS, 1,
            boneTrans->getCombinedTransform().getPointXform(
            vsVector(0.0, 0.0, 0.0)));
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
            newBoneLine->setData(VS_GEOMETRY_COLORS, 0, vsVector(
                (double) (((boneID) % 3) == 1),
                (double) (((boneID + 1) % 3) == 1),
                (double) (((boneID + 2) % 3) == 1),
                1.0));

            // Set the start point of the line.
            newBoneLine->setData(VS_GEOMETRY_VERTEX_COORDS, 0,
                vsVector(0.0, 0.0, 0.0));

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
void vsSkeleton::setOffsetMatrix(vsMatrix newOffsetMatrix)
{
    offsetMatrix = newOffsetMatrix;

    // Set the offset matrix to the skeletonTransform.
    skeletonTransform->setDynamicTransform(offsetMatrix);
}

//------------------------------------------------------------------------
// Return the currently set offset matrix.
//------------------------------------------------------------------------
vsMatrix vsSkeleton::getOffsetMatrix()
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
    updateMatrices(skeletonRootBone, skeletonTransform->getCombinedTransform());
}
