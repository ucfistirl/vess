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
//    Description:  vsNode subclass that is a leaf node in a VESS scene.
//                  This version of geometry handles all the data for skins.
//
//    Author(s):    Bryan Kline, Duvan Cope
//
//------------------------------------------------------------------------

#include "vsSkeletonMeshGeometry.h++"

#include <stdio.h>
#include <osg/MatrixTransform>
#include <osg/Drawable>
#include "vsGraphicsState.h++"
#include "vsTextureAttribute.h++"
#include "vsTextureCubeAttribute.h++"
#include "vsTextureRectangleAttribute.h++"

// ------------------------------------------------------------------------
// Default Constructor - Creates a Performer geode and geoset and connects
// them together, sets up empty geometry lists, and registers callbacks.
// ------------------------------------------------------------------------
vsSkeletonMeshGeometry::vsSkeletonMeshGeometry() : parentList(5, 5)
{
    unsigned int unit;

    parentCount = 0;

    // Geode
    osgGeode = new osg::Geode();
    osgGeode->ref();
    
    // Geometry
    osgGeometry = new osg::Geometry();
    osgGeometry->ref();
    osgGeode->addDrawable(osgGeometry);
    
    // Since this geometry is dynamic (i.e.: it will change every frame), 
    // disable display listing of the geometry data. 
    osgGeometry->setUseDisplayList(false);

    // Color array
    colorList = new osg::Vec4Array();
    colorList->ref();
    colorListSize = 0;
    osgGeometry->setColorArray(colorList);

    // Normal array
    normalList = new osg::Vec3Array();
    normalList->ref();
    normalListSize = 0;
    osgGeometry->setNormalArray(normalList);
    osgGeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

    for (unit = 0; unit < VS_MAXIMUM_TEXTURE_UNITS; unit++)
    {
        // Texture coordinate array
        texCoordList[unit] = new osg::Vec2Array();
        texCoordList[unit]->ref();
        texCoordListSize[unit] = 0;
        osgGeometry->setTexCoordArray(unit, NULL);
        textureBinding[unit] = VS_GEOMETRY_BIND_NONE;
    }

    // Vertex array
    vertexList = new osg::Vec3Array();
    vertexList->ref();
    vertexListSize = 0;
    osgGeometry->setVertexArray(vertexList);

    // Vertex array, a copy to keep in its original form unmodified by the
    // skeleton.
    originalVertexList = new osg::Vec3Array();
    originalVertexList->ref();

    // Normal array, a copy to keep in its original form unmodified by the
    // skeleton.
    originalNormalList = new osg::Vec3Array();
    originalNormalList->ref();

    // Bone index array
    boneList = new osg::Vec4Array();
    boneList->ref();
    boneListSize = 0;
    osgGeometry->setVertexAttribArray(VS_BONE_ATTRIBUTE_INDEX, boneList);
    osgGeometry->setVertexAttribBinding(VS_BONE_ATTRIBUTE_INDEX,
        osg::Geometry::BIND_PER_VERTEX);

    // Vertex weight array
    weightList = new osg::Vec4Array();
    weightList->ref();
    weightListSize = 0;
    osgGeometry->setVertexAttribArray(VS_WEIGHT_ATTRIBUTE_INDEX, weightList);
    osgGeometry->setVertexAttribBinding(VS_WEIGHT_ATTRIBUTE_INDEX,
        osg::Geometry::BIND_PER_VERTEX);

    // Other values
    lengthsList = NULL;
    primitiveCount = 0;
    primitiveType = VS_GEOMETRY_TYPE_POINTS;
    enableLighting();
    renderBin = -1;

    // Register the connection
    getMap()->registerLink(this, osgGeode);
}

// ------------------------------------------------------------------------
// Destructor - Disconnects this node from its Performer counterpart
// ------------------------------------------------------------------------
vsSkeletonMeshGeometry::~vsSkeletonMeshGeometry()
{
    unsigned int unit;

    // Remove all parents
    detachFromParents();

    // Remove all attributes
    deleteAttributes();

    // Unlink and destroy the OSG objects
    colorList->unref();
    normalList->unref();
    for (unit = 0; unit < VS_MAXIMUM_TEXTURE_UNITS; unit++)
        texCoordList[unit]->unref();
    vertexList->unref();
    originalVertexList->unref();
    boneList->unref();
    weightList->unref();
    osgGeometry->unref();
    osgGeode->unref();
    
    getMap()->removeLink(this, VS_OBJMAP_FIRST_LIST);

    // Other cleanup
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
    if ((index < 0) || (index >= parentCount))
    {
        printf("vsSkeletonMeshGeometry::getParent: Bad parent index\n");
        return NULL;
    }
    
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
    if ((newType < VS_GEOMETRY_TYPE_POINTS) ||
        (newType > VS_GEOMETRY_TYPE_POLYS))
    {
        printf("vsSkeletonMeshGeometry::setPrimitiveType: Unrecognized "
            "primitive type\n");
        return;
    }

    primitiveType = newType;
    
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
    
    // Sanity check
    if ((newCount < 0) || (newCount > 1000000))
    {
        printf("vsSkeletonMeshGeometry::setPrimitiveCount: Invalid count value "
            "'%d'\n", newCount);
        return;
    }

    // Change the length of the primitive lengths array
    if (newCount && !lengthsList)
    {
        // Create
        lengthsList = (int *)(calloc(newCount, sizeof(int)));
    }
    else if (!newCount && lengthsList)
    {
        // Delete
        free(lengthsList);
        lengthsList = NULL;
    }
    else
    {
        // Modify
        lengthsList = (int *)(realloc(lengthsList, sizeof(int) * newCount));
        if (newCount > primitiveCount)
            for (loop = primitiveCount; loop < newCount; loop++)
                lengthsList[loop] = 0;
    }
    
    primitiveCount = newCount;
    
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
    if ((index < 0) || (index >= getPrimitiveCount()))
    {
        printf("vsSkeletonMeshGeometry::setPrimitiveLength: Index out of "
            "bounds\n");
        return;
    }
    
    lengthsList[index] = length;
    
    rebuildPrimitives();
}

// ------------------------------------------------------------------------
// Retrieves the number of verticies specified for the primitive with the
// indicated index in the object. The index of the first primitive is 0.
// ------------------------------------------------------------------------
int vsSkeletonMeshGeometry::getPrimitiveLength(int index)
{
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
    
    for (loop = 0; loop < getPrimitiveCount(); loop++)
        lengthsList[loop] = lengths[loop];

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
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsSkeletonMeshGeometry::setBinding: Skin vertex "
                    "binding must always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }
            break;
        case VS_GEOMETRY_SKIN_NORMALS:
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsSkeletonMeshGeometry::setBinding: Skin normal "
                    "binding must always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }
            break;
        case VS_GEOMETRY_VERTEX_WEIGHTS:
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsSkeletonMeshGeometry::setBinding: Vertex weight "
                    "binding must always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }
            break;
        case VS_GEOMETRY_BONE_INDICES:
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsSkeletonMeshGeometry::setBinding: Bone indices "
                    "binding must always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }
            break;
        case VS_GEOMETRY_VERTEX_COORDS:
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsSkeletonMeshGeometry::setBinding: Vertex coordinate "
                    "binding must always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }
            break;
        case VS_GEOMETRY_NORMALS:
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsSkeletonMeshGeometry::setBinding: Normal "
                    "binding must always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }
            break;

        case VS_GEOMETRY_COLORS:
            osgGeometry->setColorBinding(osgBinding);
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
                osgGeometry->setTexCoordArray(unit, texCoordList[unit]);
            textureBinding[unit] = binding;
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

    switch (whichData)
    {
        case VS_GEOMETRY_SKIN_VERTEX_COORDS:
        case VS_GEOMETRY_SKIN_NORMALS:
        case VS_GEOMETRY_VERTEX_WEIGHTS:
        case VS_GEOMETRY_BONE_INDICES:
        case VS_GEOMETRY_VERTEX_COORDS:
        case VS_GEOMETRY_NORMALS:
            return VS_GEOMETRY_BIND_PER_VERTEX;
        case VS_GEOMETRY_COLORS:
            result = osgGeometry->getColorBinding();
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
        default:
            printf("vsSkeletonMeshGeometry::getBinding: Unrecognized data "
                "value\n");
            return -1;
    }
    
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
    unsigned int unit;
    int loop;

    if (dataIndex < 0)
    {
        printf("vsSkeletonMeshGeometry::setData: Index out of bounds\n");
        return;
    }
    
    switch (whichData)
    {
        case VS_GEOMETRY_SKIN_VERTEX_COORDS:
            if (dataIndex >= vertexListSize)
            {
                printf("vsSkeletonMeshGeometry::setData: Index out of"
                   " bounds\n");
                return;
            }
            if (data.getSize() < 3)
            {
                printf("vsSkeletonMeshGeometry::setData: Insufficient data"
                    " (vertex coordinates require 3 values)\n");
                return;
            }
            for (loop = 0; loop < 3; loop++)
            {
                ((*originalVertexList)[dataIndex])[loop] = data[loop];
                ((*vertexList)[dataIndex])[loop] = data[loop];
            }
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
            if (dataIndex >= weightListSize)
            {
                printf("vsSkeletonMeshGeometry::setData: Index out of"
                   " bounds\n");
                return;
            }
            if (data.getSize() < 4)
            {
                printf("vsSkeletonMeshGeometry::setData: Insufficient data"
                    " (vertex weight require 4 values)\n");
                return;
            }
            for (loop = 0; loop < 4; loop++)
                ((*weightList)[dataIndex])[loop] = data[loop];

            osgGeometry->setVertexAttribArray(VS_WEIGHT_ATTRIBUTE_INDEX,
                weightList);
            break;

        case VS_GEOMETRY_BONE_INDICES:
            if (dataIndex >= boneListSize)
            {
                printf("vsSkeletonMeshGeometry::setData: Index out of"
                   " bounds\n");
                return;
            }
            if (data.getSize() < 4)
            {
                printf("vsSkeletonMeshGeometry::setData: Insufficient data"
                    " (vertex bone indices require 4 values)\n");
                return;
            }
            for (loop = 0; loop < 4; loop++)
                ((*boneList)[dataIndex])[loop] = data[loop];

            osgGeometry->setVertexAttribArray(VS_BONE_ATTRIBUTE_INDEX,
                boneList);
            break;

        case VS_GEOMETRY_SKIN_NORMALS:
            if (dataIndex >= normalListSize)
            {
                printf("vsSkeletonMeshGeometry::setData: Index out of"
                   " bounds\n");
                return;
            }
            if (data.getSize() < 3)
            {
                printf("vsSkeletonMeshGeometry::setData: Insufficient data"
                    " (vertex normals require 3 values)\n");
                return;
            }
            for (loop = 0; loop < 3; loop++)
            {
                ((*originalNormalList)[dataIndex])[loop] = data[loop];
                ((*normalList)[dataIndex])[loop] = data[loop];
            }
            break;

        case VS_GEOMETRY_VERTEX_COORDS:
            printf("vsSkeletonMeshGeometry::setData: Cannot set vertex coords "
                   "they are generated based on bone positions.\n");
            break;

        case VS_GEOMETRY_NORMALS:
            printf("vsSkeletonMeshGeometry::setData: Cannot set normals "
                   "they are generated based on bone positions.\n");
            break;

        case VS_GEOMETRY_COLORS:
            if (dataIndex >= colorListSize)
            {
                printf("vsSkeletonMeshGeometry::setData: Index out of "
                    "bounds\n");
                return;
            }
            if (data.getSize() < 4)
            {
                printf("vsSkeletonMeshGeometry::setData: Insufficient data "
                    "(colors require 4 values)\n");
                return;
            }
            for (loop = 0; loop < 4; loop++)
                ((*colorList)[dataIndex])[loop] = data[loop];
            osgGeometry->setColorArray(colorList);
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
                                                                                
            if (dataIndex >= texCoordListSize[unit])
            {
                printf("vsSkeletonMeshGeometry::setData: Index out of "
                    "bounds\n");
                return;
            }
            if (data.getSize() < 2)
            {
                printf("vsSkeletonMeshGeometry::setData: Insufficient data "
                    "(texture coordinates require 2 values)\n");
                return;
            }
            for (loop = 0; loop < 2; loop++)
                ((*texCoordList[unit])[dataIndex])[loop] = data[loop];
            osgGeometry->setTexCoordArray(unit, texCoordList[unit]);
            break;

        default:
            printf("vsSkeletonMeshGeometry::setData: Unrecognized data type\n");
            return;
    }
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
    unsigned int unit;
    int loop;

    if (dataIndex < 0)
    {
        printf("vsSkeletonMeshGeometry::getData: Index out of bounds "
            "(dataIndex = %d)\n", dataIndex);
        return result;
    }

    // Determine which list we should obtain the data from, and return
    // the requested item from that list
    switch (whichData)
    {
        case VS_GEOMETRY_SKIN_VERTEX_COORDS:
            if (dataIndex >= vertexListSize)
            {
                printf("vsSkeletonMeshGeometry::getData: Index out of bounds "
                    "(list = SKIN_VERTEX_COORDS, dataIndex = %d,"
                    " listSize = %d)\n",
                    dataIndex, vertexListSize);
                return result;
            }
            result.setSize(3);
            for (loop = 0; loop < 3; loop++)
                result[loop] = ((*originalVertexList)[dataIndex])[loop];
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
            if (dataIndex >= weightListSize)
            {
                printf("vsSkeletonMeshGeometry::getData: Index out of bounds "
                    "(list = VERTEX_WEIGHTS, dataIndex = %d, listSize = %d)\n",
                    dataIndex, weightListSize);
                return result;
            }
            result.setSize(4);
            for (loop = 0; loop < 4; loop++)
                result[loop] = ((*weightList)[dataIndex])[loop];
            break;

        case VS_GEOMETRY_BONE_INDICES:
            if (dataIndex >= boneListSize)
            {
                printf("vsSkeletonMeshGeometry::getData: Index out of bounds "
                    "(list = BONE_INDICES, dataIndex = %d, listSize = %d)\n",
                    dataIndex, boneListSize);
                return result;
            }
            result.setSize(4);
            for (loop = 0; loop < 4; loop++)
                result[loop] = ((*boneList)[dataIndex])[loop];
            break;

        case VS_GEOMETRY_VERTEX_COORDS:
            if (dataIndex >= vertexListSize)
            {
                printf("vsSkeletonMeshGeometry::getData: Index out of bounds "
                    "(list = VERTEX_COORDS, dataIndex = %d, listSize = %d)\n",
                    dataIndex, vertexListSize);
                return result;
            }
            result.setSize(3);
            for (loop = 0; loop < 3; loop++)
                result[loop] = ((*vertexList)[dataIndex])[loop];
            break;

        case VS_GEOMETRY_SKIN_NORMALS:
            if (dataIndex >= normalListSize)
            {
                printf("vsSkeletonMeshGeometry::getData: Index out of bounds "
                    "(list = SKIN_NORMALS, dataIndex = %d, listSize = %d)\n",
                    dataIndex, normalListSize);
                return result;
            }
            result.setSize(3);
            for (loop = 0; loop < 3; loop++)
                result[loop] = ((*originalNormalList)[dataIndex])[loop];
            break;

        case VS_GEOMETRY_NORMALS:
            if (dataIndex >= normalListSize)
            {
                printf("vsSkeletonMeshGeometry::getData: Index out of bounds "
                    "(list = NORMALS, dataIndex = %d, listSize = %d)\n",
                    dataIndex, normalListSize);
                return result;
            }
            result.setSize(3);
            for (loop = 0; loop < 3; loop++)
                result[loop] = ((*normalList)[dataIndex])[loop];
            break;

        case VS_GEOMETRY_COLORS:
            if (dataIndex >= colorListSize)
            {
                printf("vsSkeletonMeshGeometry::getData: Index out of bounds "
                    "(list = COLORS, dataIndex = %d, listSize = %d)\n",
                    dataIndex, colorListSize);
                return result;
            }
            result.setSize(4);
            for (loop = 0; loop < 4; loop++)
                result[loop] = ((*colorList)[dataIndex])[loop];
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
                                                                                
            if (dataIndex >= texCoordListSize[unit])
            {
                printf("vsSkeletonMeshGeometry::getData: Index out of bounds "
                    "(list = TEXTURE%d_COORDS, dataIndex = %d, "
                    "listSize = %d)\n", unit, dataIndex,
                    texCoordListSize[unit]);
                return result;
            }
            result.setSize(2);
            for (loop = 0; loop < 2; loop++)
                result[loop] = ((*texCoordList[unit])[dataIndex])[loop];
            break;

        default:
            printf("vsSkeletonMeshGeometry::getData: Unrecognized data type\n");
            return result;
    }
    
    return result;
}

// ------------------------------------------------------------------------
// Sets all of the data points within one of the geometry objects' lists
// to the values in dataList. The dataList array must be at least as large
// as the size of particular list in question.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::setDataList(int whichData, vsVector *dataList)
{
    unsigned int unit;
    int loop, sloop;
    
    switch (whichData)
    {
        case VS_GEOMETRY_SKIN_VERTEX_COORDS:
            for (loop = 0; loop < vertexListSize; loop++)
                for (sloop = 0; sloop < 3; sloop++)
                {
                    (*originalVertexList)[loop][sloop] = dataList[loop][sloop];
                    (*vertexList)[loop][sloop] = dataList[loop][sloop];
                }
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
            for (loop = 0; loop < weightListSize; loop++)
                for (sloop = 0; sloop < 4; sloop++)
                    (*weightList)[loop][sloop] = dataList[loop][sloop];
            osgGeometry->setVertexAttribArray(VS_WEIGHT_ATTRIBUTE_INDEX,
                weightList);
            break;

        case VS_GEOMETRY_BONE_INDICES:
            for (loop = 0; loop < boneListSize; loop++)
                for (sloop = 0; sloop < 4; sloop++)
                    (*boneList)[loop][sloop] = dataList[loop][sloop];
            osgGeometry->setVertexAttribArray(VS_BONE_ATTRIBUTE_INDEX,
                boneList);
            break;

        case VS_GEOMETRY_SKIN_NORMALS:
            for (loop = 0; loop < normalListSize; loop++)
                for (sloop = 0; sloop < 3; sloop++)
                {
                    (*originalNormalList)[loop][sloop] = dataList[loop][sloop];
                    (*normalList)[loop][sloop] = dataList[loop][sloop];
                }
            break;

        case VS_GEOMETRY_VERTEX_COORDS:
            printf("vsSkeletonMeshGeometry::setData: Cannot set vertex coords, "
                   "they are generated based on bone positions.\n");
            break;

        case VS_GEOMETRY_NORMALS:
            printf("vsSkeletonMeshGeometry::setData: Cannot set normals, "
                   "they are generated based on bone positions.\n");
            break;

        case VS_GEOMETRY_COLORS:
            for (loop = 0; loop < colorListSize; loop++)
                for (sloop = 0; sloop < 4; sloop++)
                    (*colorList)[loop][sloop] = dataList[loop][sloop];
            osgGeometry->setColorArray(colorList);
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
                                                                                
            for (loop = 0; loop < texCoordListSize[unit]; loop++)
                for (sloop = 0; sloop < 2; sloop++)
                    (*texCoordList[unit])[loop][sloop] = dataList[loop][sloop];
            osgGeometry->setTexCoordArray(unit, texCoordList[unit]);
            break;

        default:
            printf("vsSkeletonMeshGeometry::setDataList: Unrecognized data "
                "type\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves all of the data points within one of the geometry objects'
// lists, storing that data in the specified dataBuffer. The dataBuffer
// array must be at least as large as the size of particular list in
// question.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::getDataList(int whichData, vsVector *dataBuffer)
{
    unsigned int unit;
    int loop, sloop;
    
    switch (whichData)
    {
        case VS_GEOMETRY_SKIN_VERTEX_COORDS:
            for (loop = 0; loop < vertexListSize; loop++)
            {
                dataBuffer[loop].setSize(3);
                for (sloop = 0; sloop < 3; sloop++)
                    dataBuffer[loop][sloop] =
                        (*originalVertexList)[loop][sloop];
            }
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
            for (loop = 0; loop < weightListSize; loop++)
            {
                dataBuffer[loop].setSize(4);
                for (sloop = 0; sloop < 4; sloop++)
                    dataBuffer[loop][sloop] = (*weightList)[loop][sloop];
            }
            break;

        case VS_GEOMETRY_BONE_INDICES:
            for (loop = 0; loop < boneListSize; loop++)
            {
                dataBuffer[loop].setSize(4);
                for (sloop = 0; sloop < 4; sloop++)
                    dataBuffer[loop][sloop] = (*boneList)[loop][sloop];
            }
            break;

        case VS_GEOMETRY_VERTEX_COORDS:
            for (loop = 0; loop < vertexListSize; loop++)
            {
                dataBuffer[loop].setSize(3);
                for (sloop = 0; sloop < 3; sloop++)
                    dataBuffer[loop][sloop] = (*vertexList)[loop][sloop];
            }
            break;

        case VS_GEOMETRY_SKIN_NORMALS:
            for (loop = 0; loop < normalListSize; loop++)
            {
                dataBuffer[loop].setSize(3);
                for (sloop = 0; sloop < 3; sloop++)
                    dataBuffer[loop][sloop] =
                        (*originalNormalList)[loop][sloop];
            }
            break;

        case VS_GEOMETRY_NORMALS:
            for (loop = 0; loop < normalListSize; loop++)
            {
                dataBuffer[loop].setSize(3);
                for (sloop = 0; sloop < 3; sloop++)
                    dataBuffer[loop][sloop] = (*normalList)[loop][sloop];
            }
            break;

        case VS_GEOMETRY_COLORS:
            for (loop = 0; loop < colorListSize; loop++)
            {
                dataBuffer[loop].setSize(4);
                for (sloop = 0; sloop < 4; sloop++)
                    dataBuffer[loop][sloop] = (*colorList)[loop][sloop];
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
                                                                                
            for (loop = 0; loop < texCoordListSize[unit]; loop++)
            {
                dataBuffer[loop].setSize(2);
                for (sloop = 0; sloop < 2; sloop++)
                    dataBuffer[loop][sloop] =
                        (*texCoordList[unit])[loop][sloop];
            }
            break;

        default:
            printf("vsSkeletonMeshGeometry::getDataList: Unrecognized data "
                "type\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Sets the size of one of the object's data lists. Generally the data list
// sizes must be set on a new geometry object before data can be put into
// it.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::setDataListSize(int whichData, int newSize)
{
    unsigned int unit;

    // Sanity check
    if ((newSize < 0) || (newSize > 1000000))
    {
        printf("vsSkeletonMeshGeometry::setDataListSize: Invalid list size "
            "'%d'\n", newSize);
        return;
    }

    switch (whichData)
    {
        case VS_GEOMETRY_SKIN_VERTEX_COORDS:
        case VS_GEOMETRY_VERTEX_COORDS:
            originalVertexList->resize(newSize);
            vertexList->resize(newSize);
            osgGeometry->setVertexArray(vertexList);
            vertexListSize = newSize;
            rebuildPrimitives();
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
            weightList->resize(newSize);
            osgGeometry->setVertexAttribArray(VS_WEIGHT_ATTRIBUTE_INDEX,
                weightList);
            weightListSize = newSize;
            break;

        case VS_GEOMETRY_BONE_INDICES:
            boneList->resize(newSize);
            osgGeometry->setVertexAttribArray(VS_BONE_ATTRIBUTE_INDEX,
                boneList);
            boneListSize = newSize;
            break;

        case VS_GEOMETRY_SKIN_NORMALS:
        case VS_GEOMETRY_NORMALS:
            originalNormalList->resize(newSize);
            normalList->resize(newSize);
            osgGeometry->setNormalArray(normalList);
            normalListSize = newSize;
            break;

        case VS_GEOMETRY_COLORS:
            colorList->resize(newSize);
            osgGeometry->setColorArray(colorList);
            colorListSize = newSize;
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
                                                                                
            texCoordList[unit]->resize(newSize);
            texCoordListSize[unit] = newSize;
        
            // If the texture coordinate binding is OFF, then the pointer
            // to the coordinate list should be NULL so that OSG knows not
            // to use texture coordinates at all.
            if (textureBinding[unit] == VS_GEOMETRY_BIND_NONE)
                osgGeometry->setTexCoordArray(unit, NULL);
            else
                osgGeometry->setTexCoordArray(unit, texCoordList[unit]);
            break;

        default:
            printf("vsSkeletonMeshGeometry::setDataListSize: Unrecognized data "
                "value\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves the size of one of the object's data lists
// ------------------------------------------------------------------------
int vsSkeletonMeshGeometry::getDataListSize(int whichData)
{
    unsigned int unit;

    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_WEIGHTS:
            return weightListSize;
        case VS_GEOMETRY_BONE_INDICES:
            return boneListSize;
        case VS_GEOMETRY_SKIN_VERTEX_COORDS:
        case VS_GEOMETRY_VERTEX_COORDS:
            return vertexListSize;
        case VS_GEOMETRY_SKIN_NORMALS:
        case VS_GEOMETRY_NORMALS:
            return normalListSize;
        case VS_GEOMETRY_COLORS:
            return colorListSize;
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
                                                                                
            return texCoordListSize[unit];
        default:
            printf("vsSkeletonMeshGeometry::getDataListSize: Unrecognized data "
                "value\n");
    }
    
    return -1;
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

    if (centerPoint)
        centerPoint->set(boundSphere._center[0], boundSphere._center[1],
            boundSphere._center[2]);

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

    // Start at the geometry's Geode, and work our way up the OSG tree

    xform.makeIdentity();
    nodePtr = osgGeode;
    
    while (nodePtr->getNumParents() > 0)
    {
        if (dynamic_cast<osg::MatrixTransform *>(nodePtr))
        {
            // Multiply this Transform's matrix into the accumulated
            // transform
            osgXformMat = ((osg::MatrixTransform *)nodePtr)->getMatrix();
            xform.postMult(osgXformMat);
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
        
        switch (primitiveType)
        {
            case VS_GEOMETRY_TYPE_POINTS:
                osgDrawArrays = new osg::DrawArrays(
                    osg::PrimitiveSet::POINTS, 0, vertexListSize);
                break;
            case VS_GEOMETRY_TYPE_LINES:
                osgDrawArrays = new osg::DrawArrays(
                    osg::PrimitiveSet::LINES, 0, vertexListSize);
                break;
            case VS_GEOMETRY_TYPE_TRIS:
                osgDrawArrays = new osg::DrawArrays(
                    osg::PrimitiveSet::TRIANGLES, 0, vertexListSize);
                break;
            case VS_GEOMETRY_TYPE_QUADS:
                osgDrawArrays = new osg::DrawArrays(
                    osg::PrimitiveSet::QUADS, 0, vertexListSize);
                break;
        }
        
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
        
        if (osgDrawArrayLengths)
            osgGeometry->addPrimitiveSet(osgDrawArrayLengths);
    }
}

// ------------------------------------------------------------------------
// Internal function
// Adds a node to this node's list of parent nodes
// ------------------------------------------------------------------------
bool vsSkeletonMeshGeometry::addParent(vsNode *newParent)
{
    parentList[parentCount++] = newParent;
    
    return true;
}

// ------------------------------------------------------------------------
// Internal function
// Removes a node from this node's list of parent nodes
// ------------------------------------------------------------------------
bool vsSkeletonMeshGeometry::removeParent(vsNode *targetParent)
{
    int loop, sloop;

    for (loop = 0; loop < parentCount; loop++)
        if (targetParent == parentList[loop])
        {
            for (sloop = loop; sloop < parentCount-1; sloop++)
                parentList[sloop] = parentList[sloop+1];
            parentCount--;
            return true;
        }

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

    vsNode::applyAttributes();
    
    osgStateSet = osgGeometry->getOrCreateStateSet();
    (vsGraphicsState::getInstance())->applyState(osgStateSet);
    
    // If the render bin is specified, set the bin details in the state
    // set. This overrides any bin set by attributes, notably transparency
    // attributes.
    if (renderBin >= 0)
    {
        sortMode = vsGeometry::getBinSortMode(renderBin);
        if (sortMode == VS_GEOMETRY_SORT_DEPTH)
            osgStateSet->setRenderBinDetails(renderBin, "DepthSortedBin");
        else
            osgStateSet->setRenderBinDetails(renderBin, "RenderBin");
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

    // If all the relevant lists are equal in size, continue to apply.
    if ((vertexListSize == boneListSize) && (boneListSize == weightListSize))
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
