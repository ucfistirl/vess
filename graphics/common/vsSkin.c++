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
//    VESS Module:  vsSkin.c++
//
//    Description:  Object to manage a set of meshes that are to be updated
//                  using the same skeleton.
//
//    Author(s):    Duvan Cope, Jason Daly
//
//------------------------------------------------------------------------

#include "vsSkin.h++"

//------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------
vsSkin::vsSkin(vsComponent *newRoot, vsSkeleton *newSkeleton,
               atArray *boneSpaceMatrices)
{
    int index;
    int childCount;
    vsNode *childNode;
    atMatrix *sourceMatrix;
    atMatrix *matrix;

    // Store a reference to the root component, increase the reference count.
    rootComponent = newRoot;
    rootComponent->ref();

    // Clone the array of bone space matrices
    boneSpaceMatrixList = new atArray();
    for (index = 0; index < boneSpaceMatrices->getNumEntries(); index++)
    {
        // Get the next matrix from the source array
        sourceMatrix = (atMatrix *)boneSpaceMatrices->getEntry(index);

        // Skip this entry if there is no corresponding entry in the source
        // array
        if (sourceMatrix)
        {
            matrix = new atMatrix();
            matrix->copy(*sourceMatrix);
            boneSpaceMatrixList->setEntry(index, matrix);
        }
    }

    // Create an array to hold the submeshes of the overall skin
    // (there may be multiple geometry objects that comprise the skin as
    // a whole) 
    subMeshCount = 0;
    meshList = new vsArray();

    // Traverse the children of the root mesh component, and add all
    // skeleton mesh geometries to our mesh array
    findSubmeshes(rootComponent);

    // Initialize the skeleton data information to nothing.
    skeleton = NULL;
    boneMatrixList = NULL;

    // Initialize the lists of final matrices
    skinMatrixList = NULL;
    skinITMatrixList = NULL;

    // Initialize the array that flags whether or not a given bone is in use
    boneUsed = NULL;

    // If we have a skeleton to work with, use it.
    if (newSkeleton)
        setSkeleton(newSkeleton);
}

//------------------------------------------------------------------------
// Copy constructor
//------------------------------------------------------------------------
vsSkin::vsSkin(vsSkin *original)
{
    int index;
    int childCount;
    vsNode *childNode;
    atMatrix *sourceMatrix;
    atMatrix *matrix;

    // Clone the root component of the skin and reference the clone 
    rootComponent = (vsComponent *)original->rootComponent->cloneTree();
    rootComponent->ref();

    // Clone the array of bone space matrices
    boneSpaceMatrixList = new atArray();
    for (index = 0; index < original->boneSpaceMatrixList->getNumEntries();
         index++)
    {
        // Get the next matrix from the source array
        sourceMatrix = (atMatrix *)original->boneSpaceMatrixList->
            getEntry(index);

        // Skip this entry if there is no corresponding entry in the source
        // array
        if (sourceMatrix)
        {
            matrix = new atMatrix();
            matrix->copy(*sourceMatrix);
            boneSpaceMatrixList->setEntry(index, matrix);
        }
    }

    // Create an array to hold the submeshes of the overall skin
    // (there may be multiple geometry objects that comprise the skin as
    // a whole) 
    subMeshCount = 0;
    meshList = new vsArray();

    // Traverse the children of the root mesh component, and add all
    // skeleton mesh geometries to our mesh array
    findSubmeshes(rootComponent);

    // Initialize the skeleton data information to nothing.
    skeleton = NULL;
    boneMatrixList = NULL;

    // Initialize the lists of final matrices
    skinMatrixList = NULL;
    skinITMatrixList = NULL;

    // If the original skin referenced a skeleton, reference the same
    // skeleton with the new skin.   This allows both skins to be controlled
    // by the same skeleton.  If the desire is to use a clone of the
    // original skeleton, then the original skeleton should be manually
    // cloned and then applied to this skin using setSkeleton()
    if (original->skeleton)
        setSkeleton(original->skeleton);
}

//------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------
vsSkin::~vsSkin()
{
    int index;

    // Unreference the skeleton object.
    if (skeleton != NULL)
        vsObject::unrefDelete(skeleton);

    // Delete the submesh list, (this will unreference all the mesh geometry
    // nodes and delete them if they're no longer in use)
    delete meshList;

    // Unreference and attempt to delete the root component of the skin
    vsObject::unrefDelete(rootComponent);

    // Delete the list of bone space matrices
    delete boneSpaceMatrixList;

    // Also, delete the final matrix lists (if they exist)
    if (skinMatrixList != NULL)
        delete skinMatrixList;
    if (skinITMatrixList != NULL)
        delete skinITMatrixList;

    // Clean up the list of bones in use
    if (boneUsed != NULL)
        delete [] boneUsed;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsSkin::getClassName()
{
    return "vsSkin";
}

// ------------------------------------------------------------------------
// Recursively searches the given node and locates all skeleton mesh
// geometry nodes among its descendants
// ------------------------------------------------------------------------
void vsSkin::findSubmeshes(vsNode *node)
{
    int i;

    // Make sure the node is valid before doing anything
    if (node == NULL)
        return;

    // If this node is a skeleton mesh geometry, add it to our list
    if (node->getNodeType() == VS_NODE_TYPE_SKELETON_MESH_GEOMETRY)
    {
        // Add the node to our list
        meshList->setEntry(subMeshCount, node);

        // Increment the submesh count
        subMeshCount++;
    }

    // Recurse on this node's children (if it has any)
    if (node->getChildCount() > 0)
    {
        // Iterate over the children and search for submeshes on them
        for (i = 0; i < node->getChildCount(); i++)
            findSubmeshes(node->getChild(i));
    }
}

// ------------------------------------------------------------------------
// Scans the mesh list to figure out which of the skeleton's bones are used
// by this skin.  This can improve update performance by skipping over
// unused bones.
// ------------------------------------------------------------------------
void vsSkin::findUsedBones()
{
    vsSkeletonMeshGeometry *mesh;
    int i, j;
    atVector bones, weights;

    // See if we already have a "bone used" array, and clean it up if so
    if (boneUsed != NULL)
       delete [] boneUsed;

    // Create the "bone used" array and clear it to false
    boneUsed = new bool[skeleton->getBoneCount()];
    memset(boneUsed, 0, sizeof(bool) * skeleton->getBoneCount());

    // Iterate over the meshes
    for (i = 0; i < meshList->getNumEntries(); i++)
    {
        // Get the next mesh
        mesh = (vsSkeletonMeshGeometry *)meshList->getEntry(i);

        // Iterate over the mesh's vertices
        for (j = 0; j < mesh->getDataListSize(VS_GEOMETRY_VERTEX_COORDS); j++)
        {
            // Get the weights and bone indices
            bones = mesh->getData(VS_GEOMETRY_BONE_INDICES, j);
            weights = mesh->getData(VS_GEOMETRY_VERTEX_WEIGHTS, j);

            // Mark the bones that are in use
            if (weights[0] > 1.0e-4)
                boneUsed[(int)bones[0]] = true;
            if (weights[1] > 1.0e-4)
                boneUsed[(int)bones[1]] = true;
            if (weights[2] > 1.0e-4)
                boneUsed[(int)bones[2]] = true;
            if (weights[3] > 1.0e-4)
                boneUsed[(int)bones[3]] = true;
        }
    }
}

// ------------------------------------------------------------------------
// Returns the number of sub-meshes in this mesh
// ------------------------------------------------------------------------
int vsSkin::getNumSubMeshes()
{
    return subMeshCount;
}

// ------------------------------------------------------------------------
// Return the geometry for the indexed mesh in this object.
// ------------------------------------------------------------------------
vsSkeletonMeshGeometry *vsSkin::getSubMesh(int index)
{
    return (vsSkeletonMeshGeometry *) meshList->getEntry(index);
}

// ------------------------------------------------------------------------
// Return the root component of all the sub meshes.
// ------------------------------------------------------------------------
vsComponent *vsSkin::getRootComponent()
{
    return rootComponent;
}

// ------------------------------------------------------------------------
// Set the skeleton that will influence all the meshes in this object.
// ------------------------------------------------------------------------
void vsSkin::setSkeleton(vsSkeleton *newSkeleton)
{
    atMatrix *newMatrix;
    int i;

    // Unreference the previous skeleton we may have been using
    if (skeleton)
        skeleton->unref();

    // Store a reference to the skeleton, and make it count
    skeleton = newSkeleton;
    skeleton->ref();

    // Store a reference to the matrix lists, so we do not need to keep asking
    boneMatrixList = skeleton->getBoneMatrixList();

    // Delete and re-create the final skin matrix list and the list of
    // corresponding inverse transpose matrices
    if (skinMatrixList != NULL)
        delete skinMatrixList;
    skinMatrixList = new atArray();
    if (skinITMatrixList != NULL)
        delete skinITMatrixList;
    skinITMatrixList = new atArray();

    // Make sure there is a matrix in the two final matrix lists for each
    // bone in the skeleton
    for (i = 0; i < skeleton->getBoneCount(); i++)
    {
        newMatrix = new atMatrix();
        skinMatrixList->setEntry(i, newMatrix);
        newMatrix = new atMatrix();
        skinITMatrixList->setEntry(i, newMatrix);
    }

    // Finally, figure out which bones are used by this skin
    findUsedBones();
}

// ------------------------------------------------------------------------
// Return the vsSkeleton object which the meshes under this object use
// to apply their skin.
// ------------------------------------------------------------------------
vsSkeleton *vsSkin::getSkeleton()
{
    return skeleton;
}

// ------------------------------------------------------------------------
// Return whether or not this skin uses the given bone
// ------------------------------------------------------------------------
bool vsSkin::usesBone(int boneIndex)
{
    // If we haven't yet constructed the boneUsed array, assume that the
    // given bone is in use, otherwise return whether or not the bone is
    // in use by the skin
    if (boneUsed == NULL)
        return true;
    else
        return boneUsed[boneIndex];
}

// ------------------------------------------------------------------------
// Get the final skinning matrix associated with the given bone index.
// Make sure update() is called before this method to be sure the matrix
// is as up to date as possible.
// ------------------------------------------------------------------------
atMatrix vsSkin::getSkinMatrix(int boneIndex)
{
    atMatrix *skinMatrix;
    atMatrix ident;

    // Get the requested matrix
    skinMatrix = (atMatrix *)skinMatrixList->getEntry(boneIndex);

    // See if the matrix is valid
    if (skinMatrix != NULL)
    {
        // Dereference and return the matrix by value
        return *skinMatrix;
    }
    else
    {
        // Return an identity matrix (there is no skin matrix associated with
        // the given bone)
        ident.setIdentity();
        return ident;
    }
}

// ------------------------------------------------------------------------
// Update the skin matrices by combining the skeleton's bone matrices with
// the skin's bone space matrices.  Also create the inverse transpose
// matrices.  We assume the skeleton has already been updated to generate
// a fresh set of bone matrices.
// ------------------------------------------------------------------------
void vsSkin::update()
{
    int i;
    atMatrix *finalMatrix;
    atMatrix *finalITMatrix;
    atMatrix *boneMatrix;
    atMatrix *boneSpaceMatrix;

    for (i = 0; i < skeleton->getBoneCount(); i++)
    {
        // See if this bone is used by the skin
        if ((boneUsed == NULL) || (boneUsed[i]))
        {
            // Get the final matrix for this bone
            finalMatrix = (atMatrix *)skinMatrixList->getEntry(i);

            // Get the bone matrix
            if (boneMatrixList != NULL)
                boneMatrix = (atMatrix *)boneMatrixList->getEntry(i);
            else
                boneMatrix = NULL;

            // Get our bone space transform
            boneSpaceMatrix = (atMatrix *)boneSpaceMatrixList->getEntry(i);

            // Make sure the two matrices are valid
            if ((boneMatrix != NULL) && (boneSpaceMatrix != NULL))
            {
                // Combine the bone matrix with the bone space transform to
                // create the final matrix
                *finalMatrix = (*boneMatrix) * (*boneSpaceMatrix);
            }
            else
            {
                // Just set the final matrix to identity
                finalMatrix->setIdentity();
            }

            // Get the corresponding inverse transpose matrix
            finalITMatrix = (atMatrix *)skinITMatrixList->getEntry(i);

            // Update the final inverse transpose matrix (use the quicker
            // inverse operation that assumes only rotations and translations
            // in the matrix)
            *finalITMatrix = finalMatrix->getInverseRigid();
            finalITMatrix->transpose();
        }
    }
}

// ------------------------------------------------------------------------
// Apply the skin transforms to all the geometry objects that comprise the
// skin
// ------------------------------------------------------------------------
void vsSkin::applySkin()
{
    int index;

    // Cycle through the submeshes and apply the skin on them.
    for (index = 0; index < subMeshCount; index++)
    {
        ((vsSkeletonMeshGeometry *) meshList->getEntry(index))->applySkin(
            skinMatrixList, skinITMatrixList);
    }
}

// ------------------------------------------------------------------------
// Reset the skin to its default pose (as if all bones were set to identity
// matrices)
// ------------------------------------------------------------------------
void vsSkin::reset()
{
    int index;

    // Cycle through the submeshes and reset the skin on them.
    for (index = 0; index < subMeshCount; index++)
    {
        ((vsSkeletonMeshGeometry *) meshList->getEntry(index))->resetSkin();
    }
}
