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
//    VESS Module:  vsSkeletonMeshGeometry.c++
//
//    Description:  vsNode subclass that is a leaf node in a VESS scene
//                  graph. Stores geometry data such as vertex and texture
//                  coordinates, colors, and face normals.  This version
//                  is a simple subclass of vsDynamicGeometry for Open Scene
//                  Graph.  Since OSG only operates in a single process,
//                  this version does not need to do any extra work to
//                  support dynamic geometry.
//
//    Author(s):    Duvan Cope, Bryan Kline, Jason Daly
//
//------------------------------------------------------------------------

#include <stdio.h>
#include <osg/MatrixTransform>
#include "vsSkeletonMeshGeometry.h++"
#include "vsComponent.h++"
#include "vsBackfaceAttribute.h++"
#include "vsFogAttribute.h++"
#include "vsMaterialAttribute.h++"
#include "vsShadingAttribute.h++"
#include "vsTextureAttribute.h++"
#include "vsTextureCubeAttribute.h++"
#include "vsTextureRectangleAttribute.h++"
#include "vsTransformAttribute.h++"
#include "vsTransparencyAttribute.h++"
#include "vsWireframeAttribute.h++"
#include "vsGraphicsState.h++"

// ------------------------------------------------------------------------
// Default Constructor - Creates a Performer geode and geoset and connects
// them together, sets up empty geometry lists, and registers callbacks.
// ------------------------------------------------------------------------
vsSkeletonMeshGeometry::vsSkeletonMeshGeometry() : parentList(5, 5)
{
    int loop;

    // Initialize the number of parents to zero
    parentCount = 0;

    // Create an osg::Geode
    osgGeode = new osg::Geode();
    osgGeode->ref();
    
    // Create an osg::Geometry node to contain the Geode
    osgGeometry = new osg::Geometry();
    osgGeometry->ref();
    osgGeode->addDrawable(osgGeometry);

    // Since this geometry is dynamic (i.e.: it will change every frame),
    // disable display listing of the geometry data.
    osgGeometry->setUseDisplayList(false);

    // Create the various data arrays
    for (loop = 0; loop < VS_GEOMETRY_LIST_COUNT; loop++)
        allocateDataArray(loop);

    // Set the normal, weight and bone indices bindings to per vertex,
    // that is the only way it can be skinned.
    osgGeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
    osgGeometry->setVertexAttribBinding(VS_GEOMETRY_VERTEX_WEIGHTS,
        osg::Geometry::BIND_PER_VERTEX);
    osgGeometry->setVertexAttribBinding(VS_GEOMETRY_BONE_INDICES,
        osg::Geometry::BIND_PER_VERTEX);

    // Vertex array, a copy to keep in its original form unmodified by the
    // skeleton.
    originalVertexList = new osg::Vec3Array();
    originalVertexList->ref();
                                                                                
    // Normal array, a copy to keep in its original form unmodified by the
    // skeleton.
    originalNormalList = new osg::Vec3Array();
    originalNormalList->ref();

    // Initialize other values
    lengthsList = NULL;
    primitiveCount = 0;
    primitiveType = VS_GEOMETRY_TYPE_POINTS;

    // Enable lighting on this Geometry and set the render bin to default
    enableLighting();
    renderBin = -1;

    // Register this node and osg::Geode in the node map
    getMap()->registerLink(this, osgGeode);
}

// ------------------------------------------------------------------------
// Destructor - Disconnects this node from its Performer counterpart
// ------------------------------------------------------------------------
vsSkeletonMeshGeometry::~vsSkeletonMeshGeometry()
{
    int loop;

    // Remove all parents
    detachFromParents();

    // Remove all attributes
    deleteAttributes();

    // Unlink and destroy the OSG objects
    for (loop = 0; loop < VS_GEOMETRY_LIST_COUNT; loop++)
        dataList[loop]->unref();
    originalVertexList->unref();
    originalNormalList->unref();
    osgGeometry->unref();
    osgGeode->unref();
    
    // Remove the link to the osg node from the object map
    getMap()->removeLink(this, VS_OBJMAP_FIRST_LIST);

    // If we've created a primitive lengths list, free this now
    if (lengthsList)
        free(lengthsList);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsSkeletonMeshGeometry::getClassName()
{
    return "vsSkeletonMeshGeometry";
}

// ------------------------------------------------------------------------
// Retrieves the type of this node
// ------------------------------------------------------------------------
int vsSkeletonMeshGeometry::getNodeType()
{
    return VS_NODE_TYPE_SKELETON_MESH_GEOMETRY;
}

// ------------------------------------------------------------------------
// Retrieves the number of parent nodes for this node
// ------------------------------------------------------------------------
int vsSkeletonMeshGeometry::getParentCount()
{
    return parentCount;
}

// ------------------------------------------------------------------------
// Retrieves one of the parent nodes of this node, specified by index.
// The index of the first parent is 0.
// ------------------------------------------------------------------------
vsNode *vsSkeletonMeshGeometry::getParent(int index)
{
    // Check the index to make sure it refers to a valid parent, complain 
    // and return NULL if not
    if ((index < 0) || (index >= parentCount))
    {
        printf("vsSkeletonMeshGeometry::getParent: Bad parent index\n");
        return NULL;
    }
    
    // Return the requested parent
    return (vsNode *)(parentList[index]);
}

// ------------------------------------------------------------------------
// Begins a new state/frame of the dynamic geometry.  In OSG, this function
// does nothing.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::beginNewState()
{
}
                                                                                
// ------------------------------------------------------------------------
// Finalizes the new dynamic geometry state.  In OSG, this function
// does nothing.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::finishNewState()
{
}

// ------------------------------------------------------------------------
// Sets the type of geometric primitive that this object contains
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::setPrimitiveType(int newType)
{
    // Make sure the type argument is a valid primitive type
    if ((newType < VS_GEOMETRY_TYPE_POINTS) ||
        (newType > VS_GEOMETRY_TYPE_POLYS))
    {
        printf("vsSkeletonMeshGeometry::setPrimitiveType: Unrecognized "
            "primitive type\n");
        return;
    }

    // Set the primitive type
    primitiveType = newType;
    
    // Reconstruct the primitives with a new type
    rebuildPrimitives();
}

// ------------------------------------------------------------------------
// Retrieves the type of geometric primitive that this object contains
// ------------------------------------------------------------------------
int vsSkeletonMeshGeometry::getPrimitiveType()
{
    return primitiveType;
}

// ------------------------------------------------------------------------
// Sets the number of geometric primitive that this object contains. Must
// be called before any calls to set the data for any specific primitive.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::setPrimitiveCount(int newCount)
{
    int loop;
    
    // Sanity check, primarily to avoid memory corruption
    if ((newCount < 0) || (newCount > VS_GEOMETRY_MAX_LIST_INDEX))
    {
        printf("vsSkeletonMeshGeometry::setPrimitiveCount: Invalid count value "
            "'%d'\n", newCount);
        return;
    }

    // Change the length of the primitive lengths array
    if (newCount && !lengthsList)
    {
        // Create the lengths array
        lengthsList = (int *)(calloc(newCount, sizeof(int)));
    }
    else if (!newCount && lengthsList)
    {
        // Delete the lengths array
        free(lengthsList);
        lengthsList = NULL;
    }
    else
    {
        // Modify the size of the lengths array
        lengthsList = (int *)(realloc(lengthsList, sizeof(int) * newCount));
        if (newCount > primitiveCount)
            for (loop = primitiveCount; loop < newCount; loop++)
                lengthsList[loop] = 0;
    }
    
    // Set the new primitive count
    primitiveCount = newCount;
    
    // Reconstruct the osg::PrimitiveSet with the new settings
    rebuildPrimitives();
}

// ------------------------------------------------------------------------
// Retrieves the number of geometric primitives that this object contains
// ------------------------------------------------------------------------
int vsSkeletonMeshGeometry::getPrimitiveCount()
{
    return primitiveCount;
}

// ------------------------------------------------------------------------
// Sets the number of verticies for the primitive with the specified index
// within the object. The index of the first primitive is 0.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::setPrimitiveLength(int index, int length)
{
    // Make sure the index is valid, given the current primitive count
    if ((index < 0) || (index >= getPrimitiveCount()))
    {
        printf("vsSkeletonMeshGeometry::setPrimitiveLength: Index out of "
            "bounds\n");
        return;
    }
    
    // Set the new length in the primitive lengths list
    lengthsList[index] = length;
    
    // Reconstruct the osg::PrimitiveSet with the new setting
    rebuildPrimitives();
}

// ------------------------------------------------------------------------
// Retrieves the number of verticies specified for the primitive with the
// indicated index in the object. The index of the first primitive is 0.
// ------------------------------------------------------------------------
int vsSkeletonMeshGeometry::getPrimitiveLength(int index)
{
    // Make sure the index is valid, given the current primitive count
    if ((index < 0) || (index >= getPrimitiveCount()))
    {
        printf("vsSkeletonMeshGeometry::getPrimitiveLength: Index out of "
            "bounds\n");
        return -1;
    }
    
    // If the current primitiveType is one of the fixed-length types,
    // then return the fixed length instead of the actual array entry
    switch (primitiveType)
    {
        case VS_GEOMETRY_TYPE_POINTS:
            return 1;
        case VS_GEOMETRY_TYPE_LINES:
            return 2;
        case VS_GEOMETRY_TYPE_TRIS:
            return 3;
        case VS_GEOMETRY_TYPE_QUADS:
            return 4;
    }

    // Return the desired length value
    return (lengthsList[index]);
}

// ------------------------------------------------------------------------
// Sets the number of verticies for all of the primitives within the object
// at once. The number of entries in the lengths array must be equal to or
// greater than the number of primitives in the object.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::setPrimitiveLengths(int *lengths)
{
    int loop;
    
    // Copy the given integer array into the primitive lengths array
    for (loop = 0; loop < getPrimitiveCount(); loop++)
        lengthsList[loop] = lengths[loop];

    // Reconstruct the osg::PrimitiveSet with the new setting
    rebuildPrimitives();
}

// ------------------------------------------------------------------------
// Copies the number of verticies for all of the primitives within the
// object into the array specified by lengthsBuffer. The number of entries
// in the specified buffer must be equal to or greater than the number
// of primitives in the object.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::getPrimitiveLengths(int *lengthsBuffer)
{
    int loop;
    
    // Copy primitive length values from this object to the specified
    // array, assuming that the primitive count is set correctly
    for (loop = 0; loop < getPrimitiveCount(); loop++)
        lengthsBuffer[loop] = lengthsList[loop];
}

// ------------------------------------------------------------------------
// Sets the binding mode for the geometry object for the given type of
// data. The binding governs how many vertices within the geometry each
// data value affects. Vertex coordinates must always have per-vertex
// binding.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::setBinding(int whichData, int binding)
{
    osg::Geometry::AttributeBinding osgBinding;
    unsigned int unit;
    
    // Translate the binding constant
    switch (binding)
    {
        case VS_GEOMETRY_BIND_NONE:
            osgBinding = osg::Geometry::BIND_OFF;
            break;
        case VS_GEOMETRY_BIND_OVERALL:
            osgBinding = osg::Geometry::BIND_OVERALL;
            break;
        case VS_GEOMETRY_BIND_PER_PRIMITIVE:
            osgBinding = osg::Geometry::BIND_PER_PRIMITIVE;
            break;
        case VS_GEOMETRY_BIND_PER_VERTEX:
            osgBinding = osg::Geometry::BIND_PER_VERTEX;
            break;
        default:
            printf("vsSkeletonMeshGeometry::setBinding: Unrecognized binding "
                "value\n");
            return;
    }

    // Figure out which data is being affected and apply the new binding
    switch (whichData)
    {
        case VS_GEOMETRY_SKIN_VERTEX_COORDS:
            // Check the binding and make sure it is per-vertex (this is
            // the only valid setting for skin vertices)
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsSkeletonMeshGeometry::setBinding: Skin vertex "
                    "coordinate binding must always be "
                    "VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }
            break;
        case VS_GEOMETRY_SKIN_NORMALS:
            // Check the binding and make sure it is per-vertex (this is
            // the only valid setting for skin normals)
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsSkeletonMeshGeometry::setBinding: Skin normal "
                    "binding must always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }
            break;
        case VS_GEOMETRY_VERTEX_COORDS:
            // Check the binding and make sure it is per-vertex (this is
            // the only valid setting for vertices)
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsSkeletonMeshGeometry::setBinding: Vertex coordinate "
                    "binding must always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
            // Check the binding and make sure it is per-vertex (this is
            // the only valid setting for vertex weights)
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsSkeletonMeshGeometry::setBinding: Vertex weight "
                    "binding must always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }
            break;

        case VS_GEOMETRY_NORMALS:
            // Check the binding and make sure it is per-vertex (this is
            // the only valid setting for vertex normals)
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsSkeletonMeshGeometry::setBinding: Normal "
                    "binding must always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }
            break;

        case VS_GEOMETRY_COLORS:
            // Set the color binding to the new value
            osgGeometry->setColorBinding(osgBinding);
            break;

        case VS_GEOMETRY_ALT_COLORS:
            // Set the secondary color binding to the new value
            osgGeometry->setSecondaryColorBinding(osgBinding);
            break;

        case VS_GEOMETRY_FOG_COORDS:
            // Set the fog coordinate binding to the new value
            osgGeometry->setFogCoordBinding(osgBinding);
            break;

        case VS_GEOMETRY_USER_DATA0:
            // There is no 'standard' binding for this data; use the
            // generic attribute binding.
            osgGeometry->setVertexAttribBinding(whichData, osgBinding);
            break;

        case VS_GEOMETRY_USER_DATA1:
            // Check the binding and make sure it is per-vertex (this is
            // the only valid setting for bone indices)
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsSkeletonMeshGeometry::setBinding: Bone indices "
                    "binding must always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }
            break;

        case VS_GEOMETRY_TEXTURE0_COORDS:
        case VS_GEOMETRY_TEXTURE1_COORDS:
        case VS_GEOMETRY_TEXTURE2_COORDS:
        case VS_GEOMETRY_TEXTURE3_COORDS:
        case VS_GEOMETRY_TEXTURE4_COORDS:
        case VS_GEOMETRY_TEXTURE5_COORDS:
        case VS_GEOMETRY_TEXTURE6_COORDS:
        case VS_GEOMETRY_TEXTURE7_COORDS:
            // Calculate the texture unit we are working with.
            unit = whichData - VS_GEOMETRY_TEXTURE0_COORDS;

            // Make sure the binding is a valid value for this list
            // (only NONE and PER_VERTEX make sense for texture coordinates)
            if ((binding != VS_GEOMETRY_BIND_PER_VERTEX) &&
                (binding != VS_GEOMETRY_BIND_NONE))
            {
                printf("vsSkeletonMeshGeometry::setBinding: Texture "
                    "coordinates binding must be either "
                    "VS_GEOMETRY_BIND_PER_VERTEX or VS_GEOMETRY_BIND_NONE\n");
                return;
            }
            
            // Since OSG does not have a binding value for texture
            // coordinates, we instead have to set the texture coordinate
            // array pointer to NULL if textures are to be off.
            // (Yes, OSG detects NULL pointers and works around them.)
            if (binding == VS_GEOMETRY_BIND_NONE)
                osgGeometry->setTexCoordArray(unit, NULL);
            else
                osgGeometry->setTexCoordArray(unit, dataList[whichData]);

            // Store the binding value in this object
            textureBinding[unit] = binding;
            break;

        case VS_GEOMETRY_GENERIC_0:
        case VS_GEOMETRY_GENERIC_1:
        case VS_GEOMETRY_GENERIC_2:
        case VS_GEOMETRY_GENERIC_3:
        case VS_GEOMETRY_GENERIC_4:
        case VS_GEOMETRY_GENERIC_5:
        case VS_GEOMETRY_GENERIC_6:
        case VS_GEOMETRY_GENERIC_7:
        case VS_GEOMETRY_GENERIC_8:
        case VS_GEOMETRY_GENERIC_9:
        case VS_GEOMETRY_GENERIC_10:
        case VS_GEOMETRY_GENERIC_11:
        case VS_GEOMETRY_GENERIC_12:
        case VS_GEOMETRY_GENERIC_13:
        case VS_GEOMETRY_GENERIC_14:
        case VS_GEOMETRY_GENERIC_15:
            // Set the generic attribute binding to the new value
            osgGeometry->setVertexAttribBinding(
                whichData - VS_GEOMETRY_LIST_COUNT, osgBinding);
            break;

        default:
            printf("vsSkeletonMeshGeometry::setBinding: Unrecognized data "
                "value\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves the binding mode for the geometry object for the specified
// type of data
// ------------------------------------------------------------------------
int vsSkeletonMeshGeometry::getBinding(int whichData)
{
    unsigned int unit;
    int result;

    // Interpret the whichData parameter and fetch the appropriate binding
    // value.  Note that vertices are always PER_VERTEX, and the
    // texture coordinate binding is stored locally, since OSG doesn't
    // use a texture coordinate binding.  The other two data list bindings
    // are fetched from OSG and translated below.
    switch (whichData)
    {
        case VS_GEOMETRY_SKIN_VERTEX_COORDS:
        case VS_GEOMETRY_SKIN_NORMALS:
        case VS_GEOMETRY_VERTEX_WEIGHTS:
        case VS_GEOMETRY_USER_DATA1:
        case VS_GEOMETRY_VERTEX_COORDS:
        case VS_GEOMETRY_NORMALS:
            return VS_GEOMETRY_BIND_PER_VERTEX;
        case VS_GEOMETRY_COLORS:
            result = osgGeometry->getColorBinding();
            break;
        case VS_GEOMETRY_ALT_COLORS:
            result = osgGeometry->getSecondaryColorBinding();
            break;
        case VS_GEOMETRY_FOG_COORDS:
            result = osgGeometry->getFogCoordBinding();
            break;
        case VS_GEOMETRY_USER_DATA0:
            result = osgGeometry->getVertexAttribBinding(whichData);
            break;
        case VS_GEOMETRY_TEXTURE0_COORDS:
        case VS_GEOMETRY_TEXTURE1_COORDS:
        case VS_GEOMETRY_TEXTURE2_COORDS:
        case VS_GEOMETRY_TEXTURE3_COORDS:
        case VS_GEOMETRY_TEXTURE4_COORDS:
        case VS_GEOMETRY_TEXTURE5_COORDS:
        case VS_GEOMETRY_TEXTURE6_COORDS:
        case VS_GEOMETRY_TEXTURE7_COORDS:
            // Calculate the texture unit we are working with.
            unit = whichData - VS_GEOMETRY_TEXTURE0_COORDS;

            return textureBinding[unit];
            break;
        case VS_GEOMETRY_GENERIC_0:
        case VS_GEOMETRY_GENERIC_1:
        case VS_GEOMETRY_GENERIC_2:
        case VS_GEOMETRY_GENERIC_3:
        case VS_GEOMETRY_GENERIC_4:
        case VS_GEOMETRY_GENERIC_5:
        case VS_GEOMETRY_GENERIC_6:
        case VS_GEOMETRY_GENERIC_7:
        case VS_GEOMETRY_GENERIC_8:
        case VS_GEOMETRY_GENERIC_9:
        case VS_GEOMETRY_GENERIC_10:
        case VS_GEOMETRY_GENERIC_11:
        case VS_GEOMETRY_GENERIC_12:
        case VS_GEOMETRY_GENERIC_13:
        case VS_GEOMETRY_GENERIC_14:
        case VS_GEOMETRY_GENERIC_15:
            result = osgGeometry->getVertexAttribBinding(
                whichData - VS_GEOMETRY_LIST_COUNT);
            break;
        default:
            printf("vsSkeletonMeshGeometry::getBinding: Unrecognized data "
                "value\n");
            return -1;
    }
    
    // Translate the result to its VESS counterpart
    switch (result)
    {
        case osg::Geometry::BIND_OFF:
            return VS_GEOMETRY_BIND_NONE;
        case osg::Geometry::BIND_OVERALL:
            return VS_GEOMETRY_BIND_OVERALL;
        case osg::Geometry::BIND_PER_PRIMITIVE:
            return VS_GEOMETRY_BIND_PER_PRIMITIVE;
        case osg::Geometry::BIND_PER_VERTEX:
            return VS_GEOMETRY_BIND_PER_VERTEX;
    }
    
    // Unrecognized binding values return an error result
    return -1;
}

// ------------------------------------------------------------------------
// Sets one data point within the geometry objects' lists of data. The
// whichData value specifies which type of data is to be affected, and
// the index specifies which data point is to be altered. The index of
// the first data point is 0.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::setData(int whichData, int dataIndex, 
                                     vsVector data)
{
    int loop;
    int slotNum;
    int dataSize;

    // If trying to set VS_GEOMETRY_VERTEX_COORDS, print error.  Vertex coords
    // cannot be set directly.
    if (whichData == VS_GEOMETRY_VERTEX_COORDS)
    {
        printf("vsSkeletonMeshGeometry::setData: Cannot set vertex coords "
            "they are generated based on bone positions.\n");
        return;
    }
    // If trying to set VS_GEOMETRY_NORMALS, print error.  Normals cannot be
    // set directly.
    else if (whichData == VS_GEOMETRY_NORMALS)
    {
        printf("vsSkeletonMeshGeometry::setData: Cannot set normals "
            "they are generated based on bone positions.\n");
        return;
    }
    // If modifying the original vertex or normals data lists, also modify the
    // corresponding conventional data lists.
    else if (whichData == VS_GEOMETRY_SKIN_VERTEX_COORDS)
    {
        whichData = VS_GEOMETRY_VERTEX_COORDS;
    }
    else if (whichData == VS_GEOMETRY_SKIN_NORMALS)
    {
        whichData = VS_GEOMETRY_NORMALS;
    }

    // Determine the minimum required number of entries that should be in the
    // data parameter. A value of 0 here means that it doesn't matter. This
    // also doubles as a check to make sure that we recognize the specified
    // constant.
    dataSize = getDataElementCount(whichData);
    if (dataSize == -1)
    {
        printf("vsSkeletonMeshGeometry::setData: Unrecognized data type\n");
        return;
    }

    // Calculate which entry in the data arrays corresponds to the given
    // constant
    if (whichData < VS_GEOMETRY_LIST_COUNT)
        slotNum = whichData;
    else
        slotNum = whichData - VS_GEOMETRY_LIST_COUNT;

    // Bounds checking; make sure the index is valid, given the list size.
    if ((dataIndex < 0) || (dataIndex >= dataListSize[slotNum]))
    {
        printf("vsSkeletonMeshGeometry::setData: Index out of bounds\n");
        return;
    }

    // Make sure that the input vector has enough data
    if (data.getSize() < dataSize)
    {
        printf("vsSkeletonMeshGeometry::setData: Insufficient data (data of "
            "the given type requires at least %d values)\n", dataSize);
        return;
    }

    // If a conventional attribute is specified, then make sure we're not
    // already using the generic attribute, and vice versa.
    if (whichData < VS_GEOMETRY_LIST_COUNT)
    {
        // Conventional data specified
        if (dataIsGeneric[slotNum])
        {
            printf("vsSkeletonMeshGeometry::setData: Cannot use conventional "
                "data type when corresponding generic attribute is in use\n");
            return;
        }
    }
    else
    {
        // Generic attribute specified
        if (!(dataIsGeneric[slotNum]))
        {
            printf("vsSkeletonMeshGeometry::setData: Cannot use generic "
                "attribute type when corresponding conventional data is in "
                "use\n");
            return;
        }
    }
    
    // Copy the data from the vector to the data list at the given index
    switch (dataSize)
    {
        case 1:
            ((*((osg::FloatArray *)(dataList[slotNum])))[dataIndex]) =
                data[0];
            break;

        case 2:
            for (loop = 0; loop < 2; loop++)
                ((*((osg::Vec2Array *)(dataList[slotNum])))[dataIndex])[loop]
                    = data[loop];
            break;

        case 3:
            // If setting VERTEX_COORDS, then we must be trying to set the
            // originalVertexList (as well as the VERTEX_COORDS) so set both.
            if (whichData == VS_GEOMETRY_VERTEX_COORDS)
            {
                for (loop = 0; loop < 3; loop++)
                {
                    ((*((osg::Vec3Array *)(dataList[slotNum])))[dataIndex])
                        [loop] = data[loop];
                    ((*originalVertexList)[dataIndex])[loop] = data[loop];
                }
            }
            // If setting NORMALS, then we must be trying to set the
            // originalNormalList (as well as the NORMALS) so set both.
            else if (whichData == VS_GEOMETRY_NORMALS)
            {
                for (loop = 0; loop < 3; loop++)
                {
                    ((*((osg::Vec3Array *)(dataList[slotNum])))[dataIndex])
                        [loop] = data[loop];
                    ((*originalNormalList)[dataIndex])[loop] = data[loop];
                }
            }
            // Else set the specified list normally.
            else
            {
                for (loop = 0; loop < 3; loop++)
                    ((*((osg::Vec3Array *)(dataList[slotNum])))[dataIndex])
                        [loop] = data[loop];
            }
            break;

        case 4:
            for (loop = 0; loop < 4; loop++)
                ((*((osg::Vec4Array *)(dataList[slotNum])))[dataIndex])[loop]
                    = data[loop];
            break;

        case 0:
            for (loop = 0; loop < data.getSize(); loop++)
                ((*((osg::Vec4Array *)(dataList[slotNum])))[dataIndex])[loop]
                    = data[loop];
            break;
    }

    // Let the appropriate OSG data array know that it's data has changed
    notifyOSGDataChanged(whichData);
}

// ------------------------------------------------------------------------
// Retrieves one data point from the geometry objects' lists of data. The
// whichData value indicates which list to pull from, and the index
// specifies which point is desired. The index of the first data point is
// 0.
// ------------------------------------------------------------------------
vsVector vsSkeletonMeshGeometry::getData(int whichData, int dataIndex)
{
    vsVector result;
    int loop;
    int slotNum;
    int dataSize;
    bool originalData;

    originalData = false;

    // If the user wants originalVertexList data, mark it.
    if (whichData == VS_GEOMETRY_SKIN_VERTEX_COORDS)
    {
        originalData = true;
        whichData = VS_GEOMETRY_VERTEX_COORDS;
    }
    // If the user wants originalNormalList data, mark it.
    else if (whichData == VS_GEOMETRY_SKIN_NORMALS)
    {
        originalData = true;
        whichData = VS_GEOMETRY_NORMALS;
    }

    // Determine the minimum required number of entries that should be in the
    // data parameter. A value of 0 here means that it doesn't matter. This
    // also doubles as a check to make sure that we recognize the specified
    // constant.
    dataSize = getDataElementCount(whichData);
    if (dataSize == -1)
    {
        printf("vsSkeletonMeshGeometry::getData: Unrecognized data type\n");
        return result;
    }

    // Calculate which entry in the data arrays corresponds to the given
    // constant
    if (whichData < VS_GEOMETRY_LIST_COUNT)
        slotNum = whichData;
    else
        slotNum = whichData - VS_GEOMETRY_LIST_COUNT;

    // Bounds checking; make sure the index is valid, given the list size.
    if ((dataIndex < 0) || (dataIndex >= dataListSize[slotNum]))
    {
        printf("vsSkeletonMeshGeometry::getData: Index out of bounds\n");
        return result;
    }

    // If a conventional attribute is specified, then make sure we're not
    // already using the generic attribute, and vice versa.
    if (whichData < VS_GEOMETRY_LIST_COUNT)
    {
        // Conventional data specified
        if (dataIsGeneric[slotNum])
        {
            printf("vsSkeletonMeshGeometry::getData: Cannot use conventional "
                "data type when corresponding generic attribute is in use\n");
            return result;
        }
    }
    else
    {
        // Generic attribute specified
        if (!(dataIsGeneric[slotNum]))
        {
            printf("vsSkeletonMeshGeometry::getData: Cannot use generic "
                "attribute type when corresponding conventional data is in "
                "use\n");
            return result;
        }
    }

    // Set the result vector to the appropriate size, and copy
    // the requested data
    if (dataSize == 0)
        result.setSize(4);
    else
        result.setSize(dataSize);

    switch (dataSize)
    {
        case 1:
            result[0] =
                ((*((osg::FloatArray *)(dataList[slotNum])))[dataIndex]);
            break;

        case 2:
            for (loop = 0; loop < 2; loop++)
                result[loop] =
                    (( *((osg::Vec2Array *)(dataList[slotNum])) )
                        [dataIndex])[loop];
            break;

        case 3:
            // If the user wants the originalVertexList data, get it.
            if ((whichData == VS_GEOMETRY_VERTEX_COORDS) && (originalData))
            {
                for (loop = 0; loop < 3; loop++)
                    result[loop] = ((*originalVertexList)[dataIndex])[loop];
            }
            // If the user wants the originalNormalList data, get it.
            else if ((whichData == VS_GEOMETRY_NORMALS) && (originalData))
            {
                for (loop = 0; loop < 3; loop++)
                    result[loop] = ((*originalNormalList)[dataIndex])[loop];
            }
            // Else get the specified list data.
            else
            {
                for (loop = 0; loop < 3; loop++)
                    result[loop] =
                        (( *((osg::Vec3Array *)(dataList[slotNum])) )
                            [dataIndex])[loop];
            }
            break;

        case 0:
        case 4:
            for (loop = 0; loop < 4; loop++)
                result[loop] =
                    (( *((osg::Vec4Array *)(dataList[slotNum])) )
                        [dataIndex])[loop];
            break;
    }
    
    // Return the result vector
    return result;
}

// ------------------------------------------------------------------------
// Sets all of the data points within one of the geometry objects' lists
// to the values in dataList. The dataList array must be at least as large
// as the size of particular list in question.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::setDataList(int whichData, vsVector *dataBuffer)
{
    int loop, sloop;
    int slotNum;
    int dataSize;

    // If trying to set VS_GEOMETRY_VERTEX_COORDS, print error.  Vertex coords
    // cannot be set directly.
    if (whichData == VS_GEOMETRY_VERTEX_COORDS)
    {
        printf("vsSkeletonMeshGeometry::setDataList: Cannot set vertex coords "
            "they are generated based on bone positions.\n");
        return;
    }
    // If trying to set VS_GEOMETRY_NORMALS, print error.  Normals cannot be
    // set directly.
    else if (whichData == VS_GEOMETRY_NORMALS)
    {
        printf("vsSkeletonMeshGeometry::setDataList: Cannot set normals "
            "they are generated based on bone positions.\n");
        return;
    }
    // If modifying the original vertex or normals data lists, also modify the
    // corresponding conventional data lists 
    else if (whichData == VS_GEOMETRY_SKIN_VERTEX_COORDS)
    {
        whichData = VS_GEOMETRY_VERTEX_COORDS;
    }
    else if (whichData == VS_GEOMETRY_SKIN_NORMALS)
    {
        whichData = VS_GEOMETRY_NORMALS;
    }

    // Determine the minimum required number of entries that should be in the
    // data parameters. A value of 0 here means that it doesn't matter. This
    // also doubles as a check to make sure that we recognize the specified
    // constant.
    dataSize = getDataElementCount(whichData);
    if (dataSize == -1)
    {
        printf("vsSkeletonMeshGeometry::setDataList: Unrecognized data type\n");
        return;
    }

    // Calculate which entry in the data arrays corresponds to the given
    // constant
    if (whichData < VS_GEOMETRY_LIST_COUNT)
        slotNum = whichData;
    else
        slotNum = whichData - VS_GEOMETRY_LIST_COUNT;

    // If a conventional attribute is specified, then make sure we're not
    // already using the generic attribute, and vice versa.
    if (whichData < VS_GEOMETRY_LIST_COUNT)
    {
        // Conventional data specified
        if (dataIsGeneric[slotNum])
        {
            printf("vsSkeletonMeshGeometry::setDataList: Cannot use "
                "conventional data type when corresponding generic attribute "
                "is in use\n");
            return;
        }
    }
    else
    {
        // Generic attribute specified
        if (!(dataIsGeneric[slotNum]))
        {
            printf("vsSkeletonMeshGeometry::setDataList: Cannot use generic "
                "attribute type when corresponding conventional data is in "
                "use\n");
            return;
        }
    }
    
    // Copy the data from the vector to the data list at the given index
    switch (dataSize)
    {
        case 1:
            for (loop = 0; loop < dataListSize[slotNum]; loop++)
                ((*((osg::FloatArray *)(dataList[slotNum])))[loop])
                    = dataBuffer[loop][0];
            break;

        case 2:
            for (loop = 0; loop < dataListSize[slotNum]; loop++)
                for (sloop = 0; sloop < 2; sloop++)
                    ((*((osg::Vec2Array *)(dataList[slotNum])))[loop])[sloop]
                        = dataBuffer[loop][sloop];
            break;

        case 3:
            // If setting VERTEX_COORDS, then we must be trying to set the
            // originalVertexList (as well as the VERTEX_COORDS) so set both.
            if (whichData == VS_GEOMETRY_VERTEX_COORDS)
            {
                for (loop = 0; loop < dataListSize[slotNum]; loop++)
                    for (sloop = 0; sloop < 3; sloop++)
                    {
                        ((*((osg::Vec3Array *)(dataList[slotNum])))[loop])
                            [sloop] = dataBuffer[loop][sloop];
                        (*originalVertexList)[loop][sloop] =
                            dataBuffer[loop][sloop];
                    }
            }
            // If setting NORMALS, then we must be trying to set the
            // originalNormalList (as well as the NORMALS) so set both.
            else if (whichData == VS_GEOMETRY_NORMALS)
            {
                for (loop = 0; loop < dataListSize[slotNum]; loop++)
                    for (sloop = 0; sloop < 3; sloop++)
                    {
                        ((*((osg::Vec3Array *)(dataList[slotNum])))[loop])
                            [sloop] = dataBuffer[loop][sloop];
                        (*originalNormalList)[loop][sloop] =
                            dataBuffer[loop][sloop];
                    }
            }
            // Else set the specified list normally.
            else
            {
                for (loop = 0; loop < dataListSize[slotNum]; loop++)
                    for (sloop = 0; sloop < 3; sloop++)
                        ((*((osg::Vec3Array *)(dataList[slotNum])))[loop])
                            [sloop] = dataBuffer[loop][sloop];
            }
            break;

        case 4:
            for (loop = 0; loop < dataListSize[slotNum]; loop++)
                for (sloop = 0; sloop < 4; sloop++)
                    ((*((osg::Vec4Array *)(dataList[slotNum])))[loop])[sloop]
                        = dataBuffer[loop][sloop];
            break;

        case 0:
            for (loop = 0; loop < dataListSize[slotNum]; loop++)
                for (sloop = 0; sloop < dataBuffer[loop].getSize(); sloop++)
                    ((*((osg::Vec4Array *)(dataList[slotNum])))[loop])[sloop]
                        = dataBuffer[loop][sloop];
            break;
    }

    // Let the appropriate OSG data array know that it's data has changed
    notifyOSGDataChanged(whichData);
}

// ------------------------------------------------------------------------
// Retrieves all of the data points within one of the geometry objects'
// lists, storing that data in the specified dataBuffer. The dataBuffer
// array must be at least as large as the size of particular list in
// question.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::getDataList(int whichData, vsVector *dataBuffer)
{
    int loop, sloop;
    int slotNum;
    int dataSize;
    bool originalData;
                                                                                
    originalData = false;
                                                                                
    // If the user wants the originalVertexList, mark it.
    if (whichData == VS_GEOMETRY_SKIN_VERTEX_COORDS)
    {
        originalData = true;
        whichData = VS_GEOMETRY_VERTEX_COORDS;
    }
    // If the user wants the originalNormalList, mark it.
    else if (whichData == VS_GEOMETRY_SKIN_NORMALS)
    {
        originalData = true;
        whichData = VS_GEOMETRY_NORMALS;
    }

    // Determine the minimum required number of entries that should be in the
    // data parameter. A value of 0 here means that it doesn't matter. This
    // also doubles as a check to make sure that we recognize the specified
    // constant.
    dataSize = getDataElementCount(whichData);
    if (dataSize == -1)
    {
        printf("vsSkeletonMeshGeometry::getDataList: Unrecognized data type\n");
        return;
    }

    // Calculate which entry in the data arrays corresponds to the given
    // constant
    if (whichData < VS_GEOMETRY_LIST_COUNT)
        slotNum = whichData;
    else
        slotNum = whichData - VS_GEOMETRY_LIST_COUNT;

    // If a conventional attribute is specified, then make sure we're not
    // already using the generic attribute, and vice versa.
    if (whichData < VS_GEOMETRY_LIST_COUNT)
    {
        // Conventional data specified
        if (dataIsGeneric[slotNum])
        {
            printf("vsSkeletonMeshGeometry::getDataList: Cannot use "
                "conventional data type when corresponding generic attribute "
                "is in use\n");
            return;
        }
    }
    else
    {
        // Generic attribute specified
        if (!(dataIsGeneric[slotNum]))
        {
            printf("vsSkeletonMeshGeometry::getDataList: Cannot use generic "
                "attribute type when corresponding conventional data is in "
                "use\n");
            return;
        }
    }

    // Copy the requested data to the output buffer
    switch (dataSize)
    {
        case 1:
            for (loop = 0; loop < dataListSize[slotNum]; loop++)
            {
                dataBuffer[loop].setSize(1);
                dataBuffer[loop][0] =
                    ((*((osg::FloatArray *)(dataList[slotNum])))[loop]);
            }
            break;

        case 2:
            for (loop = 0; loop < dataListSize[slotNum]; loop++)
            {
                dataBuffer[loop].setSize(2);
                for (sloop = 0; sloop < 2; sloop++)
                    dataBuffer[loop][sloop] =
                        (( *((osg::Vec2Array *)(dataList[slotNum])) )
                            [loop])[sloop];
            }
            break;

        case 3:
            // If the user wants the originalVertexList, get it.
            if ((whichData == VS_GEOMETRY_VERTEX_COORDS) && (originalData))
            {
                for (loop = 0; loop < dataListSize[slotNum]; loop++)
                {
                    dataBuffer[loop].setSize(3);
                    for (sloop = 0; sloop < 3; sloop++)
                        dataBuffer[loop][sloop] =
                            ((*originalVertexList)[loop])[sloop];
                }
            }
            // If the user wants the originalNormalList, get it.
            else if ((whichData == VS_GEOMETRY_NORMALS) && (originalData))
            {
                for (loop = 0; loop < dataListSize[slotNum]; loop++)
                {
                    dataBuffer[loop].setSize(3);
                    for (sloop = 0; sloop < 3; sloop++)
                        dataBuffer[loop][sloop] =
                            ((*originalNormalList)[loop])[sloop];
                }
            }
            // Else get the specified list.
            else
            {
                for (loop = 0; loop < dataListSize[slotNum]; loop++)
                {
                    dataBuffer[loop].setSize(3);
                    for (sloop = 0; sloop < 3; sloop++)
                        dataBuffer[loop][sloop] =
                            (( *((osg::Vec3Array *)(dataList[slotNum])) )
                                [loop])[sloop];
                }
            }
            break;

        case 0:
        case 4:
            for (loop = 0; loop < dataListSize[slotNum]; loop++)
            {
                dataBuffer[loop].setSize(4);
                for (sloop = 0; sloop < 4; sloop++)
                    dataBuffer[loop][sloop] =
                        (( *((osg::Vec4Array *)(dataList[slotNum])) )
                            [loop])[sloop];
            }
            break;
    }
}

// ------------------------------------------------------------------------
// Sets the size of one of the object's data lists. Generally the data list
// sizes must be set on a new geometry object before data can be put into
// it.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::setDataListSize(int whichData, int newSize)
{
    int slotNum;
    int dataSize;

    // If modifying the original vertex or normals data lists, also modify the
    // corresponding conventional data lists.
    if (whichData == VS_GEOMETRY_SKIN_VERTEX_COORDS)
    {
        whichData = VS_GEOMETRY_VERTEX_COORDS;
    }
    else if (whichData == VS_GEOMETRY_SKIN_NORMALS)
    {
        whichData = VS_GEOMETRY_NORMALS;
    }

    // Determine the type of the data array associated with the specified
    // data parameter. A value of 0 here means that we are using Vec4s. This
    // also doubles as a check to make sure that we recognize the specified
    // constant.
    dataSize = getDataElementCount(whichData);
    if (dataSize == -1)
    {
        printf("vsSkeletonMeshGeometry::setDataListSize: Unrecognized data "
            "type\n");
        return;
    }

    // Sanity check, primarily to avoid memory corruption
    if ((newSize < 0) || (newSize > VS_GEOMETRY_MAX_LIST_INDEX))
    {
        printf("vsSkeletonMeshGeometry::setDataListSize: Invalid list size "
            "'%d'\n", newSize);
        return;
    }

    // Calculate which entry in the data arrays corresponds to the given
    // constant
    if (whichData < VS_GEOMETRY_LIST_COUNT)
        slotNum = whichData;
    else
        slotNum = whichData - VS_GEOMETRY_LIST_COUNT;

    // If a conventional attribute is specified, then make sure we're not
    // already using the generic attribute, and vice versa. The only exception
    // to this rule is if the existing list size is zero; that's the only way
    // to switch from one type to the other.
    if (whichData < VS_GEOMETRY_LIST_COUNT)
    {
        // Conventional data specified
        if (dataIsGeneric[slotNum] && (dataListSize[slotNum] > 0))
        {
            printf("vsSkeletonMeshGeometry::setDataListSize: Cannot use "
                "conventional data type when corresponding generic attribute "
                "is in use\n");
            return;
        }
    }
    else
    {
        // Generic attribute specified
        if ((!dataIsGeneric[slotNum]) && (dataListSize[slotNum] > 0))
        {
            printf("vsSkeletonMeshGeometry::setDataListSize: Cannot use "
                "generic attribute type when corresponding conventional data "
                "is in use\n");
            return;
        }
    }

    // If we are changing from one attribute type to the other, then we'll
    // need to reallocate the data array, as it's type could change.
    if ((whichData < VS_GEOMETRY_LIST_COUNT) && (dataIsGeneric[slotNum]))
    {
        // Switching from generic to conventional

        // Delete the old list
        dataList[slotNum]->unref();

        // Create the new list; we have a helper function to do this for us
        allocateDataArray(whichData);

        // Note that we're now using a conventional attribute
        dataIsGeneric[slotNum] = false;
    }
    else if ((whichData >= VS_GEOMETRY_LIST_COUNT) &&
        (!dataIsGeneric[slotNum]))
    {
        // Switching from conventional to generic

        // Delete the old list
        dataList[slotNum]->unref();

        // Create the new list; we have a helper function to do this for us
        allocateDataArray(whichData);

        // Note that we're now using a generic attribute
        dataIsGeneric[slotNum] = true;
    }

    // Resize the data list
    switch (dataSize)
    {
        case 1:
            ((osg::FloatArray *)(dataList[slotNum]))->resize(newSize);
            break;
        case 2:
            ((osg::Vec2Array *)(dataList[slotNum]))->resize(newSize);
            break;
        case 3:
            // If setting VERTEX_COORDS, then we must also set the
            // originalVertexList (as well as the VERTEX_COORDS) so set both.
            if (whichData == VS_GEOMETRY_VERTEX_COORDS)
                originalVertexList->resize(newSize);
            // If setting NORMALS, then we must also set the
            // originalNormalList (as well as the NORMALS) so set both.
            else if (whichData == VS_GEOMETRY_NORMALS)
                originalNormalList->resize(newSize); 
            ((osg::Vec3Array *)(dataList[slotNum]))->resize(newSize);
            break;
        case 0:
        case 4:
            ((osg::Vec4Array *)(dataList[slotNum]))->resize(newSize);
            break;
    }
    dataListSize[slotNum] = newSize;

    // Let the appropriate OSG data array know that it's data has changed
    notifyOSGDataChanged(whichData);

    // If we're dealing with vertex coordinates, then we have to reconstruct
    // OSG's primitive set as well. (We do this with generic attribute #0 as
    // well because generic 0 is always considered to contain vertex
    // coordinates.)
    if ((whichData == VS_GEOMETRY_VERTEX_COORDS) ||
        (whichData == VS_GEOMETRY_GENERIC_0))
        rebuildPrimitives();
}

// ------------------------------------------------------------------------
// Retrieves the size of one of the object's data lists
// ------------------------------------------------------------------------
int vsSkeletonMeshGeometry::getDataListSize(int whichData)
{
    int slotNum;

    // If we request the list size of the original lists, get
    // the list size of their conventional lists (they are always equal).
    if (whichData == VS_GEOMETRY_SKIN_VERTEX_COORDS)
    {
        whichData = VS_GEOMETRY_VERTEX_COORDS;
    }
    else if (whichData == VS_GEOMETRY_SKIN_NORMALS)
    {
        whichData = VS_GEOMETRY_NORMALS;
    }

    // Bounds checking
    if ((whichData < 0) || (whichData > (VS_GEOMETRY_LIST_COUNT * 2)))
    {
        printf("vsSkeletonMeshGeometry::getDataListSize: Unrecognized data "
            "value\n");
        return -1;
    }

    // Calculate which entry in the data arrays corresponds to the given
    // constant
    if (whichData < VS_GEOMETRY_LIST_COUNT)
        slotNum = whichData;
    else
        slotNum = whichData - VS_GEOMETRY_LIST_COUNT;

    // Determine if the type of the data (conventional or generic) is the same
    // as what is currently in the specified array. If the types don't match,
    // then the user is asking for an array which (virtually) doesn't exist;
    // return a zero size in this case.
    if (dataIsGeneric[slotNum] != (whichData >= VS_GEOMETRY_LIST_COUNT))
        return 0;

    // Return the size of the specified list
    return dataListSize[slotNum];
}

// ------------------------------------------------------------------------
// Enables lit rendering for this geometry
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::enableLighting()
{
    osg::StateSet *osgStateSet;
    
    // Enable the GL lighting mode on the Geode's StateSet
    osgStateSet = osgGeode->getOrCreateStateSet();
    osgStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);
    
    // Mark lighting as enabled
    lightingEnable = true;
}

// ------------------------------------------------------------------------
// Disables lit rendering for this geometry
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::disableLighting()
{
    osg::StateSet *osgStateSet;
    
    // Disable the GL lighting mode on the Geode's StateSet
    osgStateSet = osgGeode->getOrCreateStateSet();
    osgStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    
    // Mark lighting as disabled
    lightingEnable = false;
}

// ------------------------------------------------------------------------
// Returns if lighting is enabled for this geometry
// ------------------------------------------------------------------------
bool vsSkeletonMeshGeometry::isLightingEnabled()
{
    return lightingEnable;
}

// ------------------------------------------------------------------------
// Sets the rendering bin to place this object's geometry into
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::setRenderBin(int binNum)
{
    renderBin = binNum;
}

// ------------------------------------------------------------------------
// Gets the rendering bin that this object's geometry is placed into
// ------------------------------------------------------------------------
int vsSkeletonMeshGeometry::getRenderBin()
{
    return renderBin;
}

// ------------------------------------------------------------------------
// Retrieves the center point and radius of a sphere that encompasses all
// of the geometry within this object.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::getBoundSphere(vsVector *centerPoint,
                                            double *radius)
{
    osg::BoundingSphere boundSphere;
    
    boundSphere = osgGeode->getBound();

    // Copy the sphere center to the result point, if there is one
    if (centerPoint)
        centerPoint->set(boundSphere._center[0], boundSphere._center[1],
            boundSphere._center[2]);

    // Copy the sphere radius to the result value, if there is one
    if (radius)
        *radius = boundSphere._radius;
}

// ------------------------------------------------------------------------
// Computes the global coordinate transform at this geometry by multiplying
// together all of the transforms at nodes above this one.
// ------------------------------------------------------------------------
vsMatrix vsSkeletonMeshGeometry::getGlobalXform()
{
    osg::Node *nodePtr;
    osg::Matrix xform;
    osg::Matrix osgXformMat;
    vsMatrix result;
    int loop, sloop;

    // Start with an identity matrix
    xform.makeIdentity();

    // Start at the geometry's osg::Geode, and work our way up the OSG tree
    nodePtr = osgGeode;
    
    // Check the parent count to determine if we're at the top of the tree
    while (nodePtr->getNumParents() > 0)
    {
        // Check to see if the current node is a transform
        if (dynamic_cast<osg::MatrixTransform *>(nodePtr))
        {
            // Multiply this Transform's matrix into the accumulated
            // transform
            osgXformMat = ((osg::MatrixTransform *)nodePtr)->getMatrix();
            xform.postMult(osgXformMat);
        }
        
        // Move to the node's (first) parent
        nodePtr = nodePtr->getParent(0);
    }
    
    // Transpose the matrix when going from OSG to VESS
    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            result[loop][sloop] = xform(sloop, loop);

    // Return the resulting vsMatrix
    return result;
}

// ------------------------------------------------------------------------
// Sets the intersection value for this geometry. During an intersection
// run, at each geometry object a bitwise AND of the intersection's mask
// and the geometry's value is performed; if the result of the AND is zero,
// the intersection ignores the geometry.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::setIntersectValue(unsigned int newValue)
{
    osgGeode->setNodeMask(newValue);
}

// ------------------------------------------------------------------------
// Retrieves the intersection value for this geometry.
// ------------------------------------------------------------------------
unsigned int vsSkeletonMeshGeometry::getIntersectValue()
{
    return (osgGeode->getNodeMask());
}

// ------------------------------------------------------------------------
// Adds the given attribute to the geometry object's list of child
// attributes. If successful, also notifies the attribute that it has been
// added to a list.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::addAttribute(vsAttribute *newAttribute)
{
    int newAttrCat, newAttrType, attrType;
    int loop;
    vsAttribute *attribute;
    unsigned int textureUnit, newTextureUnit;

    // Verify that the attribute is willing to be attached
    if (!(newAttribute->canAttach()))
    {
        printf("vsSkeletonMeshGeometry::addAttribute: Attribute is already in "
            "use\n");
        return;
    }
    
    // vsGeometries can only contain state attributes for now
    newAttrCat = newAttribute->getAttributeCategory();
    if (newAttrCat != VS_ATTRIBUTE_CATEGORY_STATE)
    {
        printf("vsSkeletonMeshGeometry::addAttribute: Geometry nodes may not "
            "contain attributes of that type\n");
        return;
    }

    // Initialize the texture unit to invalid maximum.
    textureUnit = VS_MAXIMUM_TEXTURE_UNITS;
    newTextureUnit = VS_MAXIMUM_TEXTURE_UNITS+1;

    // Get the new attribute's type.
    newAttrType = newAttribute->getAttributeType();

    // Get the texture unit of the new attribute, if it is a texture
    // attribute.
    if (newAttrType == VS_ATTRIBUTE_TYPE_TEXTURE)
    {
        newTextureUnit =
            ((vsTextureAttribute *) newAttribute)->getTextureUnit();
    }
    else if (newAttrType == VS_ATTRIBUTE_TYPE_TEXTURE_CUBE)
    {
        newTextureUnit =
            ((vsTextureCubeAttribute *) newAttribute)->getTextureUnit();
    }
    else if (newAttrType == VS_ATTRIBUTE_TYPE_TEXTURE_RECTANGLE)
    {
        newTextureUnit =
            ((vsTextureRectangleAttribute *)
            newAttribute)->getTextureUnit();
    }

    // Check each attribute we have.
    for (loop = 0; loop < getAttributeCount(); loop++)
    {
        attribute = getAttribute(loop);
        attrType = attribute->getAttributeType();
                                                                                
        // Get the texture unit of the current attribute, if it is a texture
        // attribute.
        if (attrType == VS_ATTRIBUTE_TYPE_TEXTURE)
        {
            textureUnit = ((vsTextureAttribute *) attribute)->getTextureUnit();
        }
        else if (attrType == VS_ATTRIBUTE_TYPE_TEXTURE_CUBE)
        {
            textureUnit =
                ((vsTextureCubeAttribute *) attribute)->getTextureUnit();
        }
        else if (attrType == VS_ATTRIBUTE_TYPE_TEXTURE_RECTANGLE)
        {
            textureUnit =
                ((vsTextureRectangleAttribute *) attribute)->getTextureUnit();
        }
        // Else they were not texture type attributes so print error and
        // return if they are equal.
        else if (attrType == newAttrType)
        {
            printf("vsSkeletonMeshGeometry::addAttribute: Geometry node "
                "already contains that type of attribute\n");
            return;
        }
                                                                                
        // If the texture units are equal then they both must have been texture
        // type attributes and had the same unit.  We don't want that to be
        // allowed so print error and return.
        if (textureUnit == newTextureUnit)
        {
            printf("vsSkeletonMeshGeometry::addAttribute: Geometry node "
                "already contains a texture attribute on unit %d\n",
                textureUnit);
            return;
        }
    }

    // If we made it this far, it must be okay to add the attribute in
    vsNode::addAttribute(newAttribute);
}

// ------------------------------------------------------------------------
// Enables culling on this node and its children
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::enableCull()
{
    osgGeode->setCullingActive(true);
}

// ------------------------------------------------------------------------
// Disables culling on this node and its children
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::disableCull()
{
    osgGeode->setCullingActive(false);
}

// ------------------------------------------------------------------------
// Returns the Performer object associated with this object
// ------------------------------------------------------------------------
osg::Geode *vsSkeletonMeshGeometry::getBaseLibraryObject()
{
    return osgGeode;
}

// ------------------------------------------------------------------------
// Private function
// Erases and reconstructs the OSG Primitive object(s) describing the
// current geometry
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::rebuildPrimitives()
{
    int numSets;
    osg::DrawArrays *osgDrawArrays;
    osg::DrawArrayLengths *osgDrawArrayLengths;

    // Erase the current list of PrimitiveSets
    numSets = osgGeometry->getNumPrimitiveSets();
    if (numSets > 0)
        osgGeometry->removePrimitiveSet(0, numSets);
    
    // Create one or more new PrimitiveSet objects based on the type,
    // number, and length data of the primitives stored in this
    // vsSkeletonMeshGeometry
    if ((primitiveType == VS_GEOMETRY_TYPE_POINTS) ||
        (primitiveType == VS_GEOMETRY_TYPE_LINES) ||
        (primitiveType == VS_GEOMETRY_TYPE_TRIS) ||
        (primitiveType == VS_GEOMETRY_TYPE_QUADS))
    {
        // If the primitive type is one of the fixed-length types, then
        // we only need to make one DrawArrays object to represent all
        // of the geometry.
        osgDrawArrays = NULL;
        
        // Select the appropriate primitive type
        switch (primitiveType)
        {
            case VS_GEOMETRY_TYPE_POINTS:
                osgDrawArrays = new osg::DrawArrays(
                    osg::PrimitiveSet::POINTS, 0, primitiveCount);
                break;
            case VS_GEOMETRY_TYPE_LINES:
                osgDrawArrays = new osg::DrawArrays(
                    osg::PrimitiveSet::LINES, 0, primitiveCount * 2);
                break;
            case VS_GEOMETRY_TYPE_TRIS:
                osgDrawArrays = new osg::DrawArrays(
                    osg::PrimitiveSet::TRIANGLES, 0, primitiveCount * 3);
                break;
            case VS_GEOMETRY_TYPE_QUADS:
                osgDrawArrays = new osg::DrawArrays(
                    osg::PrimitiveSet::QUADS, 0, primitiveCount * 4);
                break;
        }
        
        // Make sure the DrawArrays is valid, then add it to the Geometry
        if (osgDrawArrays)
            osgGeometry->addPrimitiveSet(osgDrawArrays);
    }
    else
    {
        // The primitive type must be one of the variable-lengths types.
        // * An OSG DrawArrayLengths primitive set *should* work here,
        // provided that the different entries in the lengths array
        // are interpreted by OSG as different primitives.
        osgDrawArrayLengths = NULL;
        
        // Select the appropriate primitive type
        switch (primitiveType)
        {
            case VS_GEOMETRY_TYPE_LINE_STRIPS:
                osgDrawArrayLengths = new osg::DrawArrayLengths(
                    osg::PrimitiveSet::LINE_STRIP, 0, primitiveCount,
                    lengthsList);
                break;
            case VS_GEOMETRY_TYPE_LINE_LOOPS:
                osgDrawArrayLengths = new osg::DrawArrayLengths(
                    osg::PrimitiveSet::LINE_LOOP, 0, primitiveCount,
                    lengthsList);
                break;
            case VS_GEOMETRY_TYPE_TRI_STRIPS:
                osgDrawArrayLengths = new osg::DrawArrayLengths(
                    osg::PrimitiveSet::TRIANGLE_STRIP, 0, primitiveCount,
                    lengthsList);
                break;
            case VS_GEOMETRY_TYPE_TRI_FANS:
                osgDrawArrayLengths = new osg::DrawArrayLengths(
                    osg::PrimitiveSet::TRIANGLE_FAN, 0, primitiveCount,
                    lengthsList);
                break;
            case VS_GEOMETRY_TYPE_QUAD_STRIPS:
                osgDrawArrayLengths = new osg::DrawArrayLengths(
                    osg::PrimitiveSet::QUAD_STRIP, 0, primitiveCount,
                    lengthsList);
                break;
            case VS_GEOMETRY_TYPE_POLYS:
                osgDrawArrayLengths = new osg::DrawArrayLengths(
                    osg::PrimitiveSet::POLYGON, 0, primitiveCount,
                    lengthsList);
                break;
        }
        
        // Make sure the DrawArrayLengths is valid, then add it to the 
        // Geometry
        if (osgDrawArrayLengths)
            osgGeometry->addPrimitiveSet(osgDrawArrayLengths);
    }
}

// ------------------------------------------------------------------------
// Private function
// Gets the number of elements in the vectors for the specified data type.
// A return value of 0 is a 'don't care' value; although these types
// typically have 4-element vectors allocated to them, it is not required
// to complete fill all 4 values. A return value of -1 indicates an error.
// ------------------------------------------------------------------------
int vsSkeletonMeshGeometry::getDataElementCount(int whichData)
{
    // Simple switch on the data type
    switch (whichData)
    {
        case VS_GEOMETRY_FOG_COORDS:
            return 1;

        case VS_GEOMETRY_TEXTURE0_COORDS:
        case VS_GEOMETRY_TEXTURE1_COORDS:
        case VS_GEOMETRY_TEXTURE2_COORDS:
        case VS_GEOMETRY_TEXTURE3_COORDS:
        case VS_GEOMETRY_TEXTURE4_COORDS:
        case VS_GEOMETRY_TEXTURE5_COORDS:
        case VS_GEOMETRY_TEXTURE6_COORDS:
        case VS_GEOMETRY_TEXTURE7_COORDS:
            return 2;

        case VS_GEOMETRY_VERTEX_COORDS:
        case VS_GEOMETRY_NORMALS:
            return 3;

        case VS_GEOMETRY_COLORS:
        case VS_GEOMETRY_ALT_COLORS:
            return 4;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
        case VS_GEOMETRY_USER_DATA0:
        case VS_GEOMETRY_USER_DATA1:
        case VS_GEOMETRY_GENERIC_0:
        case VS_GEOMETRY_GENERIC_1:
        case VS_GEOMETRY_GENERIC_2:
        case VS_GEOMETRY_GENERIC_3:
        case VS_GEOMETRY_GENERIC_4:
        case VS_GEOMETRY_GENERIC_5:
        case VS_GEOMETRY_GENERIC_6:
        case VS_GEOMETRY_GENERIC_7:
        case VS_GEOMETRY_GENERIC_8:
        case VS_GEOMETRY_GENERIC_9:
        case VS_GEOMETRY_GENERIC_10:
        case VS_GEOMETRY_GENERIC_11:
        case VS_GEOMETRY_GENERIC_12:
        case VS_GEOMETRY_GENERIC_13:
        case VS_GEOMETRY_GENERIC_14:
        case VS_GEOMETRY_GENERIC_15:
            return 0;
    }

    // Unrecognized constant
    return -1;
}

// ------------------------------------------------------------------------
// Private function
// Allocates the correct OSG array associated with the specified data type,
// and places it in the proper slot in the data list array.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::allocateDataArray(int whichData)
{
    int slotNum;
    int unit;

    // The type of array to create is based on which attribute of the
    // geometry will be stored in it
    slotNum = whichData;
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            // Vertex coordinates are always 3 elements
            dataList[slotNum] = new osg::Vec3Array();
            osgGeometry->setVertexArray(dataList[slotNum]);
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
            // Vertex weights aren't generally used directly by the system;
            // they are there for a shader program to use. Hand them off as a
            // generic attribute.
            dataList[slotNum] = new osg::Vec4Array();
            osgGeometry->setVertexAttribArray(whichData, dataList[slotNum]);
            break;

        case VS_GEOMETRY_NORMALS:
            // Normals are always 3 elements
            dataList[slotNum] = new osg::Vec3Array();
            osgGeometry->
                setNormalArray((osg::Vec3Array *)(dataList[slotNum]));
            break;

        case VS_GEOMETRY_COLORS:
            // Colors are always 4 elements
            dataList[slotNum] = new osg::Vec4Array();
            osgGeometry->setColorArray(dataList[slotNum]);
            break;

        case VS_GEOMETRY_ALT_COLORS:
            // Secondary colors are always 4 elements
            dataList[slotNum] = new osg::Vec4Array();
            osgGeometry->setSecondaryColorArray(dataList[slotNum]);
            break;

        case VS_GEOMETRY_FOG_COORDS:
            // Fog coordinates are single elements
            dataList[slotNum] = new osg::FloatArray();
            osgGeometry->setFogCoordArray(dataList[slotNum]);
            break;

        case VS_GEOMETRY_USER_DATA0:
        case VS_GEOMETRY_USER_DATA1:
            // These attributes are never used directly by the system; they
            // only exist for use in shader programs. Always pass them through
            // as generic attributes.
            dataList[slotNum] = new osg::Vec4Array();
            osgGeometry->setVertexAttribArray(whichData, dataList[slotNum]);
            break;

        case VS_GEOMETRY_TEXTURE0_COORDS:
        case VS_GEOMETRY_TEXTURE1_COORDS:
        case VS_GEOMETRY_TEXTURE2_COORDS:
        case VS_GEOMETRY_TEXTURE3_COORDS:
        case VS_GEOMETRY_TEXTURE4_COORDS:
        case VS_GEOMETRY_TEXTURE5_COORDS:
        case VS_GEOMETRY_TEXTURE6_COORDS:
        case VS_GEOMETRY_TEXTURE7_COORDS:
            // Texture coordinates are always 2 elements
            dataList[slotNum] = new osg::Vec2Array();
            unit = whichData - VS_GEOMETRY_TEXTURE0_COORDS;
            osgGeometry->setTexCoordArray(unit, NULL);
            textureBinding[unit] = VS_GEOMETRY_BIND_NONE;
            break;

        case VS_GEOMETRY_GENERIC_0:
        case VS_GEOMETRY_GENERIC_1:
        case VS_GEOMETRY_GENERIC_2:
        case VS_GEOMETRY_GENERIC_3:
        case VS_GEOMETRY_GENERIC_4:
        case VS_GEOMETRY_GENERIC_5:
        case VS_GEOMETRY_GENERIC_6:
        case VS_GEOMETRY_GENERIC_7:
        case VS_GEOMETRY_GENERIC_8:
        case VS_GEOMETRY_GENERIC_9:
        case VS_GEOMETRY_GENERIC_10:
        case VS_GEOMETRY_GENERIC_11:
        case VS_GEOMETRY_GENERIC_12:
        case VS_GEOMETRY_GENERIC_13:
        case VS_GEOMETRY_GENERIC_14:
        case VS_GEOMETRY_GENERIC_15:
            // Since we can't know what sort of data is going into these
            // attributes, we always assume 4 elements. These always get
            // passed through the generic attribute mechanism.
            slotNum = whichData - VS_GEOMETRY_LIST_COUNT;
            dataList[slotNum] = new osg::Vec4Array();
            osgGeometry->setVertexAttribArray(slotNum, dataList[slotNum]);
    }

    // * Perform other initialization that is common to all of the lists

    // Make sure than OSG won't delete the array on us
    dataList[slotNum]->ref();

    // Mark that the list is currently empty
    dataListSize[slotNum] = 0;

    // Mark if the attribute is a conventional one or a generic one
    if (whichData == slotNum)
        dataIsGeneric[slotNum] = false;
    else
        dataIsGeneric[slotNum] = true;
}

// ------------------------------------------------------------------------
// Private function
// Notifies OSG that the data in one of its data arrays has been changed;
// this allows OSG to preform housekeeping chores such as rebuilding GL
// display lists.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::notifyOSGDataChanged(int whichData)
{
    int slotNum;
    int unit;

    // Calculate which entry in the data arrays corresponds to the given
    // constant
    if (whichData < VS_GEOMETRY_LIST_COUNT)
        slotNum = whichData;
    else
        slotNum = whichData - VS_GEOMETRY_LIST_COUNT;

    // Let the appropriate OSG data array know that it's data has changed
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            osgGeometry->setVertexArray(dataList[slotNum]);
            break;
        case VS_GEOMETRY_VERTEX_WEIGHTS:
            osgGeometry->setVertexAttribArray(slotNum, dataList[slotNum]);
            break;
        case VS_GEOMETRY_NORMALS:
            osgGeometry->setNormalArray((osg::Vec3Array *)(dataList[slotNum]));
            break;
        case VS_GEOMETRY_COLORS:
            osgGeometry->setColorArray(dataList[slotNum]);
            break;
        case VS_GEOMETRY_ALT_COLORS:
            osgGeometry->setSecondaryColorArray(dataList[slotNum]);
            break;
        case VS_GEOMETRY_FOG_COORDS:
            osgGeometry->setFogCoordArray(dataList[slotNum]);
            break;
        case VS_GEOMETRY_USER_DATA0:
        case VS_GEOMETRY_USER_DATA1:
            osgGeometry->setVertexAttribArray(slotNum, dataList[slotNum]);
            break;
        case VS_GEOMETRY_TEXTURE0_COORDS:
        case VS_GEOMETRY_TEXTURE1_COORDS:
        case VS_GEOMETRY_TEXTURE2_COORDS:
        case VS_GEOMETRY_TEXTURE3_COORDS:
        case VS_GEOMETRY_TEXTURE4_COORDS:
        case VS_GEOMETRY_TEXTURE5_COORDS:
        case VS_GEOMETRY_TEXTURE6_COORDS:
        case VS_GEOMETRY_TEXTURE7_COORDS:
            // Calculate the texture unit we are working with.
            unit = whichData - VS_GEOMETRY_TEXTURE0_COORDS;

            // Set the OSG texture coordinate array with the new data
            if (textureBinding[unit] == VS_GEOMETRY_BIND_PER_VERTEX)
                osgGeometry->setTexCoordArray(unit, dataList[slotNum]);

            break;
        case VS_GEOMETRY_GENERIC_0:
        case VS_GEOMETRY_GENERIC_1:
        case VS_GEOMETRY_GENERIC_2:
        case VS_GEOMETRY_GENERIC_3:
        case VS_GEOMETRY_GENERIC_4:
        case VS_GEOMETRY_GENERIC_5:
        case VS_GEOMETRY_GENERIC_6:
        case VS_GEOMETRY_GENERIC_7:
        case VS_GEOMETRY_GENERIC_8:
        case VS_GEOMETRY_GENERIC_9:
        case VS_GEOMETRY_GENERIC_10:
        case VS_GEOMETRY_GENERIC_11:
        case VS_GEOMETRY_GENERIC_12:
        case VS_GEOMETRY_GENERIC_13:
        case VS_GEOMETRY_GENERIC_14:
        case VS_GEOMETRY_GENERIC_15:
            osgGeometry->setVertexAttribArray(slotNum, dataList[slotNum]);
            break;
    }
}

// ------------------------------------------------------------------------
// Internal function
// Adds a node to this node's list of parent nodes
// ------------------------------------------------------------------------
bool vsSkeletonMeshGeometry::addParent(vsNode *newParent)
{
    // Add the parent to our parent list and reference it
    parentList[parentCount++] = newParent;
    
    // Return success
    return true;
}

// ------------------------------------------------------------------------
// Internal function
// Removes a node from this node's list of parent nodes
// ------------------------------------------------------------------------
bool vsSkeletonMeshGeometry::removeParent(vsNode *targetParent)
{
    int loop, sloop;

    // Look for the given "parent" in the parent list
    for (loop = 0; loop < parentCount; loop++)
    {
        // See if this is the parent we're looking for
        if (targetParent == parentList[loop])
        {
            // 'Slide' the parents down to cover up the removed one
            for (sloop = loop; sloop < parentCount-1; sloop++)
                parentList[sloop] = parentList[sloop+1];

            // Remove the given parent
            parentCount--;

            // Return that the remove succeeded
            return true;
        }
    }

    // Return failure if the specified parent isn't found
    return false;
}

// ------------------------------------------------------------------------
// Internal function
// Calls the apply function on all attached attributes, and then calls the
// scene library's graphics state object to affect the changes to the
// graphics library state. Also applies the geometry's current rendering
// bin to the state set if specified for this object.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::applyAttributes()
{
    osg::StateSet *osgStateSet;
    int sortMode;

    // Call the inherited applyAttributes function
    vsNode::applyAttributes();

    // Instruct the current active attributes to apply themselves to this
    // node's osg StateSet
    osgStateSet = osgGeometry->getOrCreateStateSet();
    (vsGraphicsState::getInstance())->applyState(osgStateSet);
    
    // If the render bin is specified, set the bin details in the state
    // set. This overrides any bin set by attributes, notably transparency
    // attributes.
    if (renderBin >= 0)
    {
        // Get the bin's sort mode
        sortMode = vsGeometry::getBinSortMode(renderBin);

        // Set the sort order on the corresponding osg::RenderBin
        if (sortMode == VS_GEOMETRY_SORT_DEPTH)
            osgStateSet->setRenderBinDetails(renderBin, "DepthSortedBin");
        else
            osgStateSet->setRenderBinDetails(renderBin, "RenderBin");
    }
}

// ------------------------------------------------------------------------
// Internal function
// Figures out what the top left and bottom right coordinates of this
// particular geometry is
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::getAxisAlignedBoxBounds(vsVector *minValues,
    vsVector *maxValues)
{
    int childCount = getChildCount();
    int dataCount;
    int cntGData;
    int column;
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

    // Grab the geometry's transform attribute if it has one 
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

    // Loop through all of the verticies in the geometry and set the
    // max and min bounds.
    dataCount = getDataListSize(VS_GEOMETRY_VERTEX_COORDS);
    for (cntGData = 0; cntGData < dataCount; cntGData++)
    {
        // Get the current data point
        oldPoint = getData(VS_GEOMETRY_VERTEX_COORDS, cntGData);

        // Apply the dynamic transform in order to get the new 
        // location of the point in 3d space.
        newPoint = dynamicMatrix.getPointXform(oldPoint);

        // Check the new point to see if it's points are 
        // minimum or maximum.
        for (column = 0; column < 3; column++)
        {
            // Check to see if this point is minimum
            if (minNotSet || (newPoint[column] < (*minValues)[column]))
            {
                (*minValues)[column] = newPoint[column];
                minNotSet = false;
            }

            // Check to see if this point is maximum
            if (maxNotSet || (newPoint[column] > (*maxValues)[column]))
            {
                (*maxValues)[column] = newPoint[column];
                maxNotSet = false;
            }
        }
    }
}

// ------------------------------------------------------------------------
// Apply the skin based on the bone matrix lists provided as arguments.
// Also modifies the vertex normals using the inverse transpose of the bone
// matrices.  The process is basically just a weighted sum of vertices.
//
// V' = V*M[0]*w[0] + V*M[1]*w[1] + ... + V*M[i]*w[i];
// Equivalent to:
// V' = V*(M[0]*w[0] + M[1]*w[1] + ... + M[i]*w[i]);
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::applySkin(vsGrowableArray *boneMatrices,
                                       vsGrowableArray *ITBoneMatrices)
{
    int vertexIndex;
    int dataIndex;
    double weight;
    int bone;
    vsVector vertex;
    vsVector normal;
    vsMatrix *boneMatrix, *itBoneMatrix;
    vsMatrix finalVertexMatrix;
    vsMatrix finalNormalMatrix;
    osg::Vec3Array *vertexList;
    osg::Vec3Array *normalList;
    osg::Vec4Array *weightList;
    osg::Vec4Array *boneList;
    int vertexListSize;
    int normalListSize;
    int weightListSize;
    int boneListSize;

    // Get references and cast the needed arrays, better to do once than
    // several hundred times per frame.  Makes it easier to read as well.
    vertexList = (osg::Vec3Array *) dataList[VS_GEOMETRY_VERTEX_COORDS];
    normalList = (osg::Vec3Array *) dataList[VS_GEOMETRY_NORMALS];
    weightList = (osg::Vec4Array *) dataList[VS_GEOMETRY_VERTEX_WEIGHTS];
    boneList = (osg::Vec4Array *) dataList[VS_GEOMETRY_BONE_INDICES];

    vertexListSize = dataListSize[VS_GEOMETRY_VERTEX_COORDS];
    normalListSize = dataListSize[VS_GEOMETRY_NORMALS];
    weightListSize = dataListSize[VS_GEOMETRY_VERTEX_WEIGHTS];
    boneListSize = dataListSize[VS_GEOMETRY_BONE_INDICES];

    // If all the relevant lists are equal in size, continue to apply.
    if ((vertexListSize == normalListSize) &&
        (normalListSize == weightListSize) &&
        (weightListSize ==  boneListSize))
    {
        // For each vertex.
        for (vertexIndex = 0; vertexIndex < vertexListSize; vertexIndex++)
        {
            // Get the original vertex value.
            vertex.set(((*originalVertexList)[vertexIndex])[0],
                ((*originalVertexList)[vertexIndex])[1],
                ((*originalVertexList)[vertexIndex])[2]);

            // Get the original normal value.
            normal.set(((*originalNormalList)[vertexIndex])[0],
                ((*originalNormalList)[vertexIndex])[1],
                ((*originalNormalList)[vertexIndex])[2]);

            // Clear the final matrices.
            finalVertexMatrix.clear();
            finalNormalMatrix.clear();

            // For each data index, AKA possible influences.
            for (dataIndex = 0; dataIndex < 4; dataIndex++)
            {
                // Get the weight for this bone
                weight = (*weightList)[vertexIndex][dataIndex];

                // Get the bone index
                bone = (int)((*boneList)[vertexIndex][dataIndex]);

                // Get the bone matrix and the inverse transpose for this
                // data index
                boneMatrix = ((vsMatrix *)boneMatrices->getData(bone));
                itBoneMatrix = ((vsMatrix *)ITBoneMatrices->getData(bone));

                // If the weight is zero, don't bother multiplying.
                if (weight != 0.0)
                {
                    // Sum up and scale each of the matrices to finally
                    // multiply with the vertex to get its final position.
                    finalVertexMatrix += boneMatrix->getScaled(weight);

                    // Sum up and scale each of the matrices to finally
                    // multiply with the normal to get its final normal.
                    finalNormalMatrix += itBoneMatrix->getScaled(weight);
                }
            }

            // Transform the original vertex by the average matrix.
            vertex = finalVertexMatrix.getPointXform(vertex);

            // Set the final vertex into the vertex array list.
            ((*vertexList)[vertexIndex])[0] = vertex[0];
            ((*vertexList)[vertexIndex])[1] = vertex[1];
            ((*vertexList)[vertexIndex])[2] = vertex[2];

            // Transform the original normal by the average matrix,
            // re-normalize.
            normal = finalNormalMatrix.getVectorXform(normal);
            normal.normalize();

            // Set the final normal into the normal array list.
            ((*normalList)[vertexIndex])[0] = normal[0];
            ((*normalList)[vertexIndex])[1] = normal[1];
            ((*normalList)[vertexIndex])[2] = normal[2];
        }

        rebuildPrimitives();
    }
}

// ------------------------------------------------------------------------
// This method resets the mesh to the original vertex and normal
// coordinates.  That is, it resets the mesh to its default pose, as if
// all bones in the skeleton were set to identity.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::resetSkin()
{
    osg::Vec3Array *vertexList;
    osg::Vec3Array *normalList;
    int vertexListSize;
    int i;

    // Get references and cast the needed arrays, better to do once than
    // several hundred times per frame.  Makes it easier to read as well.
    vertexList = (osg::Vec3Array *) dataList[VS_GEOMETRY_VERTEX_COORDS];
    normalList = (osg::Vec3Array *) dataList[VS_GEOMETRY_NORMALS];
    vertexListSize = dataListSize[VS_GEOMETRY_VERTEX_COORDS];

    // For each vertex in the mesh...
    for (i = 0; i < vertexListSize; i++)
    {
        // Reset the vertex to the original vertex value.
        ((*vertexList)[i])[0] = ((*originalVertexList)[i])[0];
        ((*vertexList)[i])[1] = ((*originalVertexList)[i])[1];
        ((*vertexList)[i])[2] = ((*originalVertexList)[i])[2];

        // Reset the normal to the original normal value.
        ((*normalList)[i])[0] = ((*originalNormalList)[i])[0];
        ((*normalList)[i])[1] = ((*originalNormalList)[i])[1];
        ((*normalList)[i])[2] = ((*originalNormalList)[i])[2];
    }
}
