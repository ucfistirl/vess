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
//    VESS Module:  vsSkeletonMesh.c++
//
//    Description:  Object to manage a set of meshes that are to be updated
//                  using the same skeleton.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#include "vsSkeletonMesh.h++"

//------------------------------------------------------------------------
// Consntructor
//------------------------------------------------------------------------
vsSkeletonMesh::vsSkeletonMesh(vsComponent *newRoot, vsSkeleton *newSkeleton)
{
    int index;
    int childCount;
    vsNode *childNode;

    // Store a reference to the root component, increase the reference count.
    rootComponent = newRoot;
    rootComponent->ref();

    // Get the number of children that are under the root node.  This is
    // used as the mesh count, because it is expected that this component
    // only have meshes as immediate children.
    childCount = rootComponent->getChildCount();
    subMeshCount = 0;
    meshList = new vsGrowableArray(childCount, 1);

    // For all the children, test if they are meshes and if so add them to
    // the growable array.  If not the entry remains NULL, and will likely
    // cause a crash.  An error is printed.
    for (index = 0; index < childCount; index++)
    {
        childNode = rootComponent->getChild(index);
        if (childNode->getNodeType() == VS_NODE_TYPE_SKELETON_MESH_GEOMETRY)
        {
            childNode->ref();
            meshList->setData(subMeshCount, childNode);
            subMeshCount++;
        }
        else
        {
            fprintf(stderr, "vsSkeletonMesh::vsSkeletonMesh:\n"
                "Invalid vsSkeletonMeshGeometry child!\n");
        }
    }

    // If the child count and mesh count do not match, a likely invalid
    // object was given to the contructor, therefore inform.
    if (childCount != subMeshCount)
    {
        fprintf(stderr, "vsSkeletonMesh::vsSkeletonMesh:\n"
            "Possible invalid mesh root node!\n");
    }

    // Initialize the skeleton data information to nothing.
    skeleton = NULL;
    boneMatrixList = NULL;
    ITBoneMatrixList = NULL;

    // If we have a skeleton to work with, use it.
    if (newSkeleton)
        setSkeleton(newSkeleton);
}

//------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------
vsSkeletonMesh::~vsSkeletonMesh()
{
    int index;

    // Unreference the skeleton object.
    skeleton->unref();

    // Unreference all the mesh geometry nodes.
    for (index = 0; index < subMeshCount; index++)
    {
        ((vsSkeletonMeshGeometry *) meshList->getData(index))->unref();
    }

    // Unreference and attempt to delete the root bone.
    vsObject::unrefDelete(rootComponent);

    // Belete the vsGrowableArray.
    delete meshList;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsSkeletonMesh::getClassName()
{
    return "vsSkeletonMesh";
}

// ------------------------------------------------------------------------
// Returns the number of sub-meshes in this mesh
// ------------------------------------------------------------------------
int vsSkeletonMesh::getNumSubMeshes()
{
    return subMeshCount;
}

// ------------------------------------------------------------------------
// Return the geometry for the indexed mesh in this object.
// ------------------------------------------------------------------------
vsSkeletonMeshGeometry *vsSkeletonMesh::getSubMesh(int index)
{
    return (vsSkeletonMeshGeometry *) meshList->getData(index);
}

// ------------------------------------------------------------------------
// Return the root component of all the sub meshes.
// ------------------------------------------------------------------------
vsComponent *vsSkeletonMesh::getRootComponent()
{
    return rootComponent;
}

// ------------------------------------------------------------------------
// Set the skeleton that will influence all the meshes in this object.
// ------------------------------------------------------------------------
void vsSkeletonMesh::setSkeleton(vsSkeleton *newSkeleton)
{
    // Unreference the previous skeleton may have have been using.
    if (skeleton)
        skeleton->unref();

    // Store a reference to the skeleton, and make it count.
    skeleton = newSkeleton;
    skeleton->ref();

    // Store a reference to the matrix lists, so we do not need to keep asking.
    boneMatrixList = skeleton->getBoneMatrixList();
    ITBoneMatrixList = skeleton->getITBoneMatrixList();
}

// ------------------------------------------------------------------------
// Return the vsSkeleton object which the meshes under this object use
// to apply their skin.
// ------------------------------------------------------------------------
vsSkeleton *vsSkeletonMesh::getSkeleton()
{
    return skeleton;
}

// ------------------------------------------------------------------------
// Apply the skin to all the mesh objects this object controls.
// ------------------------------------------------------------------------
void vsSkeletonMesh::update()
{
    int index;

    // Cycle through the meshes and apply the skin on them.
    for (index = 0; index < subMeshCount; index++)
    {
        ((vsSkeletonMeshGeometry *) meshList->getData(index))->applySkin(
            boneMatrixList, ITBoneMatrixList);
    }
}
