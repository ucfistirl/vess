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
//    VESS Module:  vsDynamicGeometry.c++
//
//    Description:  vsNode subclass that is a leaf node in a VESS scene
//                  graph. Stores geometry data such as vertex and texture
//                  coordinates, colors, and face normals.  This version
//                  is a simple subclass of vsDynamicGeometry for Open Scene
//                  Graph.  Since OSG only operates in a single process,
//                  this version does not need to do any extra work to
//                  support dynamic geometry.
//
//    Author(s):    Bryan Kline, Duvan Cope
//
//------------------------------------------------------------------------

#include <stdio.h>
#include <osg/MatrixTransform>
#include "vsDynamicGeometry.h++"
#include "vsComponent.h++"
#include "vsBackfaceAttribute.h++"
#include "vsFogAttribute.h++"
#include "vsMaterialAttribute.h++"
#include "vsShadingAttribute.h++"
#include "vsTextureAttribute.h++"
#include "vsTransparencyAttribute.h++"
#include "vsWireframeAttribute.h++"
#include "vsGraphicsState.h++"

// ------------------------------------------------------------------------
// Default Constructor - Creates a Performer geode and geoset and connects
// them together, sets up empty geometry lists, and registers callbacks.
// ------------------------------------------------------------------------
vsDynamicGeometry::vsDynamicGeometry() : parentList(5, 5)
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
vsDynamicGeometry::~vsDynamicGeometry()
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
const char *vsDynamicGeometry::getClassName()
{
    return "vsDynamicGeometry";
}

// ------------------------------------------------------------------------
// Retrieves the type of this node
// ------------------------------------------------------------------------
int vsDynamicGeometry::getNodeType()
{
    return VS_NODE_TYPE_DYNAMIC_GEOMETRY;
}

// ------------------------------------------------------------------------
// Retrieves the number of parent nodes for this node
// ------------------------------------------------------------------------
int vsDynamicGeometry::getParentCount()
{
    return parentCount;
}

// ------------------------------------------------------------------------
// Retrieves one of the parent nodes of this node, specified by index.
// The index of the first parent is 0.
// ------------------------------------------------------------------------
vsNode *vsDynamicGeometry::getParent(int index)
{
    if ((index < 0) || (index >= parentCount))
    {
        printf("vsDynamicGeometry::getParent: Bad parent index\n");
        return NULL;
    }
    
    return (vsNode *)(parentList[index]);
}

// ------------------------------------------------------------------------
// Begins a new state/frame of the dynamic geometry.  In OSG, this function
// does nothing.
// ------------------------------------------------------------------------
void vsDynamicGeometry::beginNewState()
{
}

// ------------------------------------------------------------------------
// Finalizes the new dynamic geometry state.  In OSG, this function
// does nothing.
// ------------------------------------------------------------------------
void vsDynamicGeometry::finishNewState()
{
}

// ------------------------------------------------------------------------
// Sets the type of geometric primitive that this object contains
// ------------------------------------------------------------------------
void vsDynamicGeometry::setPrimitiveType(int newType)
{
    if ((newType < VS_GEOMETRY_TYPE_POINTS) ||
        (newType > VS_GEOMETRY_TYPE_POLYS))
    {
        printf("vsDynamicGeometry::setPrimitiveType: Unrecognized primitive "
            "type\n");
        return;
    }

    primitiveType = newType;
    
    rebuildPrimitives();
}

// ------------------------------------------------------------------------
// Retrieves the type of geometric primitive that this object contains
// ------------------------------------------------------------------------
int vsDynamicGeometry::getPrimitiveType()
{
    return primitiveType;
}

// ------------------------------------------------------------------------
// Sets the number of geometric primitive that this object contains. Must
// be called before any calls to set the data for any specific primitive.
// ------------------------------------------------------------------------
void vsDynamicGeometry::setPrimitiveCount(int newCount)
{
    int loop;
    
    // Sanity check
    if ((newCount < 0) || (newCount > 1000000))
    {
        printf("vsDynamicGeometry::setPrimitiveCount: Invalid count value "
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
int vsDynamicGeometry::getPrimitiveCount()
{
    return primitiveCount;
}

// ------------------------------------------------------------------------
// Sets the number of verticies for the primitive with the specified index
// within the object. The index of the first primitive is 0.
// ------------------------------------------------------------------------
void vsDynamicGeometry::setPrimitiveLength(int index, int length)
{
    if ((index < 0) || (index >= getPrimitiveCount()))
    {
        printf("vsDynamicGeometry::setPrimitiveLength: Index out of bounds\n");
        return;
    }
    
    lengthsList[index] = length;
    
    rebuildPrimitives();
}

// ------------------------------------------------------------------------
// Retrieves the number of verticies specified for the primitive with the
// indicated index in the object. The index of the first primitive is 0.
// ------------------------------------------------------------------------
int vsDynamicGeometry::getPrimitiveLength(int index)
{
    if ((index < 0) || (index >= getPrimitiveCount()))
    {
        printf("vsDynamicGeometry::getPrimitiveLength: Index out of bounds\n");
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
void vsDynamicGeometry::setPrimitiveLengths(int *lengths)
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
void vsDynamicGeometry::getPrimitiveLengths(int *lengthsBuffer)
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
void vsDynamicGeometry::setBinding(int whichData, int binding)
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
            printf("vsDynamicGeometry::setBinding: Unrecognized binding "
                "value\n");
            return;
    }

    // Figure out which data is being affected and apply the new binding
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsDynamicGeometry::setBinding: Vertex coordinate "
                    "binding must always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }
            break;

        case VS_GEOMETRY_NORMALS:
            osgGeometry->setNormalBinding(osgBinding);
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
                printf("vsDynamicGeometry::setBinding: Texture coordinates "
                    "binding must be either VS_GEOMETRY_BIND_PER_VERTEX or "
                    "VS_GEOMETRY_BIND_NONE\n");
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
            printf("vsDynamicGeometry::setBinding: Unrecognized data value\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves the binding mode for the geometry object for the specified
// type of data
// ------------------------------------------------------------------------
int vsDynamicGeometry::getBinding(int whichData)
{
    unsigned int unit;
    int result;

    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            return VS_GEOMETRY_BIND_PER_VERTEX;
        case VS_GEOMETRY_NORMALS:
            result = osgGeometry->getNormalBinding();
            break;
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
            printf("vsDynamicGeometry::getBinding: Unrecognized data value\n");
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
void vsDynamicGeometry::setData(int whichData, int dataIndex, vsVector data)
{
    unsigned int unit;
    int loop;

    if (dataIndex < 0)
    {
        printf("vsDynamicGeometry::setData: Index out of bounds\n");
        return;
    }
    
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            if (dataIndex >= vertexListSize)
            {
                printf("vsDynamicGeometry::setData: Index out of bounds\n");
                return;
            }
            if (data.getSize() < 3)
            {
                printf("vsDynamicGeometry::setData: Insufficient data (vertex "
                    "coordinates require 3 values)\n");
                return;
            }
            for (loop = 0; loop < 3; loop++)
                ((*vertexList)[dataIndex])[loop] = data[loop];
            osgGeometry->setVertexArray(vertexList);
            break;

        case VS_GEOMETRY_NORMALS:
            if (dataIndex >= normalListSize)
            {
                printf("vsDynamicGeometry::setData: Index out of bounds\n");
                return;
            }
            if (data.getSize() < 3)
            {
                printf("vsDynamicGeometry::setData: Insufficient data (vertex "
                    "normals require 3 values)\n");
                return;
            }
            for (loop = 0; loop < 3; loop++)
                ((*normalList)[dataIndex])[loop] = data[loop];
            osgGeometry->setNormalArray(normalList);
            break;

        case VS_GEOMETRY_COLORS:
            if (dataIndex >= colorListSize)
            {
                printf("vsDynamicGeometry::setData: Index out of bounds\n");
                return;
            }
            if (data.getSize() < 4)
            {
                printf("vsDynamicGeometry::setData: Insufficient data (colors "
                    "require 4 values)\n");
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
                printf("vsDynamicGeometry::setData: Index out of bounds\n");
                return;
            }
            if (data.getSize() < 2)
            {
                printf("vsDynamicGeometry::setData: Insufficient data "
                    "(texture coordinates require 2 values)\n");
                return;
            }
            for (loop = 0; loop < 2; loop++)
                ((*texCoordList[unit])[dataIndex])[loop] = data[loop];
            osgGeometry->setTexCoordArray(unit, texCoordList[unit]);
            break;

        default:
            printf("vsDynamicGeometry::setData: Unrecognized data type\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves one data point from the geometry objects' lists of data. The
// whichData value indicates which list to pull from, and the index
// specifies which point is desired. The index of the first data point is
// 0.
// ------------------------------------------------------------------------
vsVector vsDynamicGeometry::getData(int whichData, int dataIndex)
{
    vsVector result;
    unsigned int unit;
    int loop;

    if (dataIndex < 0)
    {
        printf("vsDynamicGeometry::getData: Index out of bounds (dataIndex "
            "= %d)\n", dataIndex);
        return result;
    }
    
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            if (dataIndex >= vertexListSize)
            {
                printf("vsDynamicGeometry::getData: Index out of bounds "
                    "(list = VERTEX_COORDS, dataIndex = %d, listSize = %d)\n",
                    dataIndex, vertexListSize);
                return result;
            }
            result.setSize(3);
            for (loop = 0; loop < 3; loop++)
                result[loop] = ((*vertexList)[dataIndex])[loop];
            break;

        case VS_GEOMETRY_NORMALS:
            if (dataIndex >= normalListSize)
            {
                printf("vsDynamicGeometry::getData: Index out of bounds "
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
                printf("vsDynamicGeometry::getData: Index out of bounds "
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
                printf("vsDynamicGeometry::getData: Index out of bounds "
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
            printf("vsDynamicGeometry::getData: Unrecognized data type\n");
            return result;
    }
    
    return result;
}

// ------------------------------------------------------------------------
// Sets all of the data points within one of the geometry objects' lists
// to the values in dataList. The dataList array must be at least as large
// as the size of particular list in question.
// ------------------------------------------------------------------------
void vsDynamicGeometry::setDataList(int whichData, vsVector *dataList)
{
    unsigned int unit;
    int loop, sloop;
    
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            for (loop = 0; loop < vertexListSize; loop++)
                for (sloop = 0; sloop < 3; sloop++)
                    (*vertexList)[loop][sloop] = dataList[loop][sloop];
            osgGeometry->setVertexArray(vertexList);
            break;

        case VS_GEOMETRY_NORMALS:
            for (loop = 0; loop < normalListSize; loop++)
                for (sloop = 0; sloop < 3; sloop++)
                    (*normalList)[loop][sloop] = dataList[loop][sloop];
            osgGeometry->setNormalArray(normalList);
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
            printf("vsDynamicGeometry::setDataList: Unrecognized data type\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves all of the data points within one of the geometry objects'
// lists, storing that data in the specified dataBuffer. The dataBuffer
// array must be at least as large as the size of particular list in
// question.
// ------------------------------------------------------------------------
void vsDynamicGeometry::getDataList(int whichData, vsVector *dataBuffer)
{
    unsigned int unit;
    int loop, sloop;
    
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            for (loop = 0; loop < vertexListSize; loop++)
            {
                dataBuffer[loop].setSize(3);
                for (sloop = 0; sloop < 3; sloop++)
                    dataBuffer[loop][sloop] = (*vertexList)[loop][sloop];
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
            printf("vsDynamicGeometry::getDataList: Unrecognized data type\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Sets the size of one of the object's data lists. Generally the data list
// sizes must be set on a new geometry object before data can be put into
// it.
// ------------------------------------------------------------------------
void vsDynamicGeometry::setDataListSize(int whichData, int newSize)
{
    unsigned int unit;

    // Sanity check
    if ((newSize < 0) || (newSize > 1000000))
    {
        printf("vsDynamicGeometry::setDataListSize: Invalid list size '%d'\n",
            newSize);
        return;
    }

    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            vertexList->resize(newSize);
            osgGeometry->setVertexArray(vertexList);
            vertexListSize = newSize;
            rebuildPrimitives();
            break;

        case VS_GEOMETRY_NORMALS:
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
            printf("vsDynamicGeometry::setDataListSize: Unrecognized data "
                "value\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves the size of one of the object's data lists
// ------------------------------------------------------------------------
int vsDynamicGeometry::getDataListSize(int whichData)
{
    unsigned int unit;

    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            return vertexListSize;
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
            printf("vsDynamicGeometry::getDataListSize: Unrecognized data "
                "value\n");
    }
    
    return -1;
}

// ------------------------------------------------------------------------
// Enables lit rendering for this geometry
// ------------------------------------------------------------------------
void vsDynamicGeometry::enableLighting()
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
void vsDynamicGeometry::disableLighting()
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
bool vsDynamicGeometry::isLightingEnabled()
{
    return lightingEnable;
}

// ------------------------------------------------------------------------
// Sets the rendering bin to place this object's geometry into
// ------------------------------------------------------------------------
void vsDynamicGeometry::setRenderBin(int binNum)
{
    renderBin = binNum;
}

// ------------------------------------------------------------------------
// Gets the rendering bin that this object's geometry is placed into
// ------------------------------------------------------------------------
int vsDynamicGeometry::getRenderBin()
{
    return renderBin;
}

// ------------------------------------------------------------------------
// Retrieves the center point and radius of a sphere that encompasses all
// of the geometry within this object.
// ------------------------------------------------------------------------
void vsDynamicGeometry::getBoundSphere(vsVector *centerPoint, double *radius)
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
vsMatrix vsDynamicGeometry::getGlobalXform()
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
void vsDynamicGeometry::setIntersectValue(unsigned int newValue)
{
    osgGeode->setNodeMask(newValue);
}

// ------------------------------------------------------------------------
// Retrieves the intersection value for this geometry.
// ------------------------------------------------------------------------
unsigned int vsDynamicGeometry::getIntersectValue()
{
    return (osgGeode->getNodeMask());
}

// ------------------------------------------------------------------------
// Adds the given attribute to the geometry object's list of child
// attributes. If successful, also notifies the attribute that it has been
// added to a list.
// ------------------------------------------------------------------------
void vsDynamicGeometry::addAttribute(vsAttribute *newAttribute)
{
    int attrCat, attrType;
    int loop;

    if (!(newAttribute->canAttach()))
    {
        printf("vsDynamicGeometry::addAttribute: Attribute is already in "
            "use\n");
        return;
    }
    
    attrCat = newAttribute->getAttributeCategory();
    if (attrCat != VS_ATTRIBUTE_CATEGORY_STATE)
    {
        printf("vsDynamicGeometry::addAttribute: Geometry nodes may not "
            "contain attributes of that type\n");
        return;
    }
    
    attrType = newAttribute->getAttributeType();
    for (loop = 0; loop < getAttributeCount(); loop++)
        if ((getAttribute(loop))->getAttributeType() == attrType)
        {
            printf("vsDynamicGeometry::addAttribute: Geometry node already "
                "contains that type of attribute\n");
            return;
        }

    // If we made it this far, it must be okay to add the attribute in
    vsNode::addAttribute(newAttribute);
}

// ------------------------------------------------------------------------
// Enables culling on this node and its children
// ------------------------------------------------------------------------
void vsDynamicGeometry::enableCull()
{
    osgGeode->setCullingActive(true);
}

// ------------------------------------------------------------------------
// Disables culling on this node and its children
// ------------------------------------------------------------------------
void vsDynamicGeometry::disableCull()
{
    osgGeode->setCullingActive(false);
}

// ------------------------------------------------------------------------
// Returns the Performer object associated with this object
// ------------------------------------------------------------------------
osg::Geode *vsDynamicGeometry::getBaseLibraryObject()
{
    return osgGeode;
}

// ------------------------------------------------------------------------
// Private function
// Erases and reconstructs the OSG Primitive object(s) describing the
// current geometry
// ------------------------------------------------------------------------
void vsDynamicGeometry::rebuildPrimitives()
{
    int numSets;
    osg::DrawArrays *osgDrawArrays;
    osg::DrawArrayLengths *osgDrawArrayLengths;

    // Erase the current list of PrimitiveSets
    numSets = osgGeometry->getNumPrimitiveSets();
    if (numSets > 0)
        osgGeometry->removePrimitiveSet(0, numSets);

    // Create one or more new PrimitiveSet objects based on the type,
    // number, and length data of the primitives stored in this vsDynamicGeometry
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
bool vsDynamicGeometry::addParent(vsNode *newParent)
{
    parentList[parentCount++] = newParent;
    
    return true;
}

// ------------------------------------------------------------------------
// Internal function
// Removes a node from this node's list of parent nodes
// ------------------------------------------------------------------------
bool vsDynamicGeometry::removeParent(vsNode *targetParent)
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
void vsDynamicGeometry::applyAttributes()
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
