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
//    VESS Module:  vsGeometry.c++
//
//    Description:  vsNode subclass that is a leaf node in a VESS scene
//                  graph. Stores geometry data such as vertex and texture
//                  coordinates, colors, and face normals.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include <osg/MatrixTransform>
#include "vsGeometry.h++"
#include "vsComponent.h++"
#include "vsBackfaceAttribute.h++"
#include "vsFogAttribute.h++"
#include "vsMaterialAttribute.h++"
#include "vsShadingAttribute.h++"
#include "vsTextureAttribute.h++"
#include "vsTransparencyAttribute.h++"
#include "vsWireframeAttribute.h++"
#include "vsGraphicsState.h++"

vsTreeMap *vsGeometry::binModeList = NULL;
int vsGeometry::binModesChanged = VS_FALSE;

// ------------------------------------------------------------------------
// Default Constructor - Creates a Performer geode and geoset and connects
// them together, sets up empty geometry lists, and registers callbacks.
// ------------------------------------------------------------------------
vsGeometry::vsGeometry() : parentList(5, 5)
{
    // Initialize the number of parents to zero
    parentCount = 0;

    // Create an osg::Geode
    osgGeode = new osg::Geode();
    osgGeode->ref();
    
    // Create an osg::Geometry node to contain the Geode
    osgGeometry = new osg::Geometry();
    osgGeometry->ref();
    osgGeode->addDrawable(osgGeometry);

    // Create the color array
    colorList = new osg::Vec4Array();
    colorList->ref();
    colorListSize = 0;
    osgGeometry->setColorArray(colorList);

    // Create the normal array
    normalList = new osg::Vec3Array();
    normalList->ref();
    normalListSize = 0;
    osgGeometry->setNormalArray(normalList);

    // Create the texture coordinate array
    texCoordList = new osg::Vec2Array();
    texCoordList->ref();
    texCoordListSize = 0;
    osgGeometry->setTexCoordArray(0, NULL);
    textureBinding = VS_GEOMETRY_BIND_NONE;

    // Create the vertex array
    vertexList = new osg::Vec3Array();
    vertexList->ref();
    vertexListSize = 0;
    osgGeometry->setVertexArray(vertexList);

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
vsGeometry::~vsGeometry()
{
    vsAttribute *attr;
    vsNode *parent;
    int loop;

    // Remove all attached attributes; destroy those that aren't being
    // used by other nodes.
    while (getAttributeCount() > 0)
    {
        // Get the first attribute, remove it, and delete it if it's
	// unused
        attr = getAttribute(0);
        removeAttribute(attr);
        if (!(attr->isAttached()))
            delete attr;
    }
 
    // Remove this node from its parents
    while (getParentCount() > 0)
    {
        // Get the first parent, and remove this node from it
        parent = getParent(0);
        parent->removeChild(this);
    }

    // Unlink and destroy the OSG objects
    colorList->unref();
    normalList->unref();
    texCoordList->unref();
    vertexList->unref();
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
const char *vsGeometry::getClassName()
{
    return "vsGeometry";
}

// ------------------------------------------------------------------------
// Retrieves the type of this node
// ------------------------------------------------------------------------
int vsGeometry::getNodeType()
{
    return VS_NODE_TYPE_GEOMETRY;
}

// ------------------------------------------------------------------------
// Retrieves the number of parent nodes for this node
// ------------------------------------------------------------------------
int vsGeometry::getParentCount()
{
    return parentCount;
}

// ------------------------------------------------------------------------
// Retrieves one of the parent nodes of this node, specified by index.
// The index of the first parent is 0.
// ------------------------------------------------------------------------
vsNode *vsGeometry::getParent(int index)
{
    // Check the index to make sure it refers to a valid parent, complain 
    // and return NULL if not
    if ((index < 0) || (index >= parentCount))
    {
        printf("vsGeometry::getParent: Bad parent index\n");
        return NULL;
    }
    
    // Return the requested parent
    return (vsNode *)(parentList[index]);
}

// ------------------------------------------------------------------------
// Sets the type of geometric primitive that this object contains
// ------------------------------------------------------------------------
void vsGeometry::setPrimitiveType(int newType)
{
    // Make sure the type argument is a valid primitive type
    if ((newType < VS_GEOMETRY_TYPE_POINTS) ||
        (newType > VS_GEOMETRY_TYPE_POLYS))
    {
        printf("vsGeometry::setPrimitiveType: Unrecognized primitive type\n");
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
int vsGeometry::getPrimitiveType()
{
    return primitiveType;
}

// ------------------------------------------------------------------------
// Sets the number of geometric primitive that this object contains. Must
// be called before any calls to set the data for any specific primitive.
// ------------------------------------------------------------------------
void vsGeometry::setPrimitiveCount(int newCount)
{
    int loop;
    
    // Sanity check, primarily to avoid memory corruption
    if ((newCount < 0) || (newCount > VS_GEOMETRY_MAX_LIST_INDEX))
    {
        printf("vsGeometry::setPrimitiveCount: Invalid count value '%d'\n",
            newCount);
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
int vsGeometry::getPrimitiveCount()
{
    return primitiveCount;
}

// ------------------------------------------------------------------------
// Sets the number of verticies for the primitive with the specified index
// within the object. The index of the first primitive is 0.
// ------------------------------------------------------------------------
void vsGeometry::setPrimitiveLength(int index, int length)
{
    // Make sure the index is valid, given the current primitive count
    if ((index < 0) || (index >= getPrimitiveCount()))
    {
        printf("vsGeometry::setPrimitiveLength: Index out of bounds\n");
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
int vsGeometry::getPrimitiveLength(int index)
{
    // Make sure the index is valid, given the current primitive count
    if ((index < 0) || (index >= getPrimitiveCount()))
    {
        printf("vsGeometry::getPrimitiveLength: Index out of bounds\n");
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
void vsGeometry::setPrimitiveLengths(int *lengths)
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
void vsGeometry::getPrimitiveLengths(int *lengthsBuffer)
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
void vsGeometry::setBinding(int whichData, int binding)
{
    osg::Geometry::AttributeBinding osgBinding;
    
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
            printf("vsGeometry::setBinding: Unrecognized binding value\n");
            return;
    }

    // Figure out which data is being affected and apply the new binding
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            // Check the binding and make sure it is per-vertex (this is
            // the only valid setting for vertices)
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsGeometry::setBinding: Vertex coordinate binding must "
                    "always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }
            break;

        case VS_GEOMETRY_NORMALS:
            // Set the normal binding to the new value
            osgGeometry->setNormalBinding(osgBinding);
            break;

        case VS_GEOMETRY_COLORS:
            // Set the color binding to the new value
            osgGeometry->setColorBinding(osgBinding);
            break;

        case VS_GEOMETRY_TEXTURE_COORDS:
            // Make sure the binding is a valid value for this list
            // (only NONE and PER_VERTEX make sense for texture coordinates)
            if ((binding != VS_GEOMETRY_BIND_PER_VERTEX) &&
                (binding != VS_GEOMETRY_BIND_NONE))
            {
                printf("vsGeometry::setBinding: Texture coordinates binding "
                    "must be either VS_GEOMETRY_BIND_PER_VERTEX or "
                    "VS_GEOMETRY_BIND_NONE\n");
                return;
            }
            
            // Since OSG does not have a binding value for texture
            // coordinates, we instead have to set the texture coordinate
            // array pointer to NULL if textures are to be off.
            // (Yes, OSG detects NULL pointers and works around them.)
            if (binding == VS_GEOMETRY_BIND_NONE)
                osgGeometry->setTexCoordArray(0, NULL);
            else
                osgGeometry->setTexCoordArray(0, texCoordList);

            // Store the binding value in this object
            textureBinding = binding;
            break;

        default:
            printf("vsGeometry::setBinding: Unrecognized data value\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves the binding mode for the geometry object for the specified
// type of data
// ------------------------------------------------------------------------
int vsGeometry::getBinding(int whichData)
{
    int result;

    // Interpret the whichData parameter and fetch the appropriate binding
    // value.  Note that vertices are always PER_VERTEX, and the
    // texture coordinate binding is stored locally, since OSG doesn't
    // use a texture coordinate binding.  The other two data list bindings
    // are fetched from OSG and translated below.
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
        case VS_GEOMETRY_TEXTURE_COORDS:
            return textureBinding;
            break;
        default:
            printf("vsGeometry::getBinding: Unrecognized data value\n");
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
void vsGeometry::setData(int whichData, int dataIndex, vsVector data)
{
    int loop;

    // Make sure the index is not negative
    if (dataIndex < 0)
    {
        printf("vsGeometry::setData: Index out of bounds\n");
        return;
    }
    
    // Select the appropriate data list
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            // Make sure the index is valid, given the list size
            if (dataIndex >= vertexListSize)
            {
                printf("vsGeometry::setData: Index out of bounds\n");
                return;
            }

            // Make sure the vector size is correct
            if (data.getSize() < 3)
            {
                printf("vsGeometry::setData: Insufficient data (vertex "
                    "coordinates require 3 values)\n");
                return;
            }

            // Copy the vector data to the data list at the given index
            for (loop = 0; loop < 3; loop++)
                ((*vertexList)[dataIndex])[loop] = data[loop];

            // Set the OSG vertex array with the new data
            osgGeometry->setVertexArray(vertexList);
            break;

        case VS_GEOMETRY_NORMALS:
            // Make sure the index is valid, given the list size
            if (dataIndex >= normalListSize)
            {
                printf("vsGeometry::setData: Index out of bounds\n");
                return;
            }

            // Make sure the vector size is correct
            if (data.getSize() < 3)
            {
                printf("vsGeometry::setData: Insufficient data (vertex "
                    "normals require 3 values)\n");
                return;
            }

            // Copy the vector data to the data list at the given index
            for (loop = 0; loop < 3; loop++)
                ((*normalList)[dataIndex])[loop] = data[loop];

            // Set the OSG normal array with the new data
            osgGeometry->setNormalArray(normalList);
            break;

        case VS_GEOMETRY_COLORS:
            // Make sure the index is valid, given the list size
            if (dataIndex >= colorListSize)
            {
                printf("vsGeometry::setData: Index out of bounds\n");
                return;
            }

            // Make sure the vector size is correct
            if (data.getSize() < 4)
            {
                printf("vsGeometry::setData: Insufficient data (colors "
                    "require 4 values)\n");
                return;
            }

            // Copy the vector data to the data list at the given index
            for (loop = 0; loop < 4; loop++)
                ((*colorList)[dataIndex])[loop] = data[loop];

            // Set the OSG color array with the new data
            osgGeometry->setColorArray(colorList);
            break;

        case VS_GEOMETRY_TEXTURE_COORDS:
            // Make sure the index is valid, given the list size
            if (dataIndex >= texCoordListSize)
            {
                printf("vsGeometry::setData: Index out of bounds\n");
                return;
            }

            // Make sure the vector size is correct
            if (data.getSize() < 2)
            {
                printf("vsGeometry::setData: Insufficient data (texture "
                    "coordinates require 2 values)\n");
                return;
            }

            // Copy the vector data to the data list at the given index
            for (loop = 0; loop < 2; loop++)
                ((*texCoordList)[dataIndex])[loop] = data[loop];

            // Set the OSG texture coordinate array with the new data
            osgGeometry->setTexCoordArray(0, texCoordList);
            break;

        default:
            printf("vsGeometry::setData: Unrecognized data type\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves one data point from the geometry objects' lists of data. The
// whichData value indicates which list to pull from, and the index
// specifies which point is desired. The index of the first data point is
// 0.
// ------------------------------------------------------------------------
vsVector vsGeometry::getData(int whichData, int dataIndex)
{
    vsVector result;
    int loop;

    // Make sure the index is not negative
    if (dataIndex < 0)
    {
        printf("vsGeometry::getData: Index out of bounds (dataIndex = %d)\n",
            dataIndex);
        return result;
    }
    
    // Select the appropriate data list
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            // Make sure the index is valid, given the list size
            if (dataIndex >= vertexListSize)
            {
                printf("vsGeometry::getData: Index out of bounds "
                    "(list = VERTEX_COORDS, dataIndex = %d, listSize = %d)\n",
                    dataIndex, vertexListSize);
                return result;
            }

            // Set the result vector to the appropriate size, and copy
            // the requested data
            result.setSize(3);
            for (loop = 0; loop < 3; loop++)
                result[loop] = ((*vertexList)[dataIndex])[loop];
            break;

        case VS_GEOMETRY_NORMALS:
            // Make sure the index is valid, given the list size
            if (dataIndex >= normalListSize)
            {
                printf("vsGeometry::getData: Index out of bounds "
                    "(list = NORMALS, dataIndex = %d, listSize = %d)\n",
                    dataIndex, normalListSize);
                return result;
            }

            // Set the result vector to the appropriate size, and copy
            // the requested data
            result.setSize(3);
            for (loop = 0; loop < 3; loop++)
                result[loop] = ((*normalList)[dataIndex])[loop];
            break;

        case VS_GEOMETRY_COLORS:
            // Make sure the index is valid, given the list size
            if (dataIndex >= colorListSize)
            {
                printf("vsGeometry::getData: Index out of bounds "
                    "(list = COLORS, dataIndex = %d, listSize = %d)\n",
                    dataIndex, colorListSize);
                return result;
            }

            // Set the result vector to the appropriate size, and copy
            // the requested data
            result.setSize(4);
            for (loop = 0; loop < 4; loop++)
                result[loop] = ((*colorList)[dataIndex])[loop];
            break;

        case VS_GEOMETRY_TEXTURE_COORDS:
            // Make sure the index is valid, given the list size
            if (dataIndex >= texCoordListSize)
            {
                printf("vsGeometry::getData: Index out of bounds "
                    "(list = TEXTURE_COORDS, dataIndex = %d, listSize = %d)\n",
                    dataIndex, texCoordListSize);
                return result;
            }

            // Set the result vector to the appropriate size, and copy
            // the requested data
            result.setSize(2);
            for (loop = 0; loop < 2; loop++)
                result[loop] = ((*texCoordList)[dataIndex])[loop];
            break;

        default:
            printf("vsGeometry::getData: Unrecognized data type\n");
            return result;
    }
    
    // Return the result vector
    return result;
}

// ------------------------------------------------------------------------
// Sets all of the data points within one of the geometry objects' lists
// to the values in dataList. The dataList array must be at least as large
// as the size of particular list in question.
// ------------------------------------------------------------------------
void vsGeometry::setDataList(int whichData, vsVector *dataList)
{
    int loop, sloop;
    
    // Select the appropriate data list
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            // Copy the data list into the vertex array
            for (loop = 0; loop < vertexListSize; loop++)
                for (sloop = 0; sloop < 3; sloop++)
                    (*vertexList)[loop][sloop] = dataList[loop][sloop];

            // Set the OSG vertex array with the new data
            osgGeometry->setVertexArray(vertexList);
            break;

        case VS_GEOMETRY_NORMALS:
            // Copy the data list into the normal array
            for (loop = 0; loop < normalListSize; loop++)
                for (sloop = 0; sloop < 3; sloop++)
                    (*normalList)[loop][sloop] = dataList[loop][sloop];

            // Set the OSG normal array with the new data
            osgGeometry->setNormalArray(normalList);
            break;

        case VS_GEOMETRY_COLORS:
            // Copy the data list into the color array
            for (loop = 0; loop < colorListSize; loop++)
                for (sloop = 0; sloop < 4; sloop++)
                    (*colorList)[loop][sloop] = dataList[loop][sloop];

            // Set the OSG color array with the new data
            osgGeometry->setColorArray(colorList);
            break;

        case VS_GEOMETRY_TEXTURE_COORDS:
            // Copy the data list into the texture coordinate array
            for (loop = 0; loop < texCoordListSize; loop++)
                for (sloop = 0; sloop < 2; sloop++)
                    (*texCoordList)[loop][sloop] = dataList[loop][sloop];

            // Set the OSG texture coordinate array with the new data
            osgGeometry->setTexCoordArray(0, texCoordList);
            break;

        default:
            printf("vsGeometry::setDataList: Unrecognized data type\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves all of the data points within one of the geometry objects'
// lists, storing that data in the specified dataBuffer. The dataBuffer
// array must be at least as large as the size of particular list in
// question.
// ------------------------------------------------------------------------
void vsGeometry::getDataList(int whichData, vsVector *dataBuffer)
{
    int loop, sloop;
    
    // Copy the appropriate data list into the buffer provided
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            for (loop = 0; loop < vertexListSize; loop++)
            {
                // Copy the data to the vector buffer
                dataBuffer[loop].setSize(3);
                for (sloop = 0; sloop < 3; sloop++)
                    dataBuffer[loop][sloop] = (*vertexList)[loop][sloop];
            }
            break;

        case VS_GEOMETRY_NORMALS:
            for (loop = 0; loop < normalListSize; loop++)
            {
                // Copy the data to the vector buffer
                dataBuffer[loop].setSize(3);
                for (sloop = 0; sloop < 3; sloop++)
                    dataBuffer[loop][sloop] = (*normalList)[loop][sloop];
            }
            break;

        case VS_GEOMETRY_COLORS:
            for (loop = 0; loop < colorListSize; loop++)
            {
                // Copy the data to the vector buffer
                dataBuffer[loop].setSize(4);
                for (sloop = 0; sloop < 4; sloop++)
                    dataBuffer[loop][sloop] = (*colorList)[loop][sloop];
            }
            break;

        case VS_GEOMETRY_TEXTURE_COORDS:
            for (loop = 0; loop < texCoordListSize; loop++)
            {
                // Copy the data to the vector buffer
                dataBuffer[loop].setSize(2);
                for (sloop = 0; sloop < 2; sloop++)
                    dataBuffer[loop][sloop] = (*texCoordList)[loop][sloop];
            }
            break;

        default:
            printf("vsGeometry::getDataList: Unrecognized data type\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Sets the size of one of the object's data lists. Generally the data list
// sizes must be set on a new geometry object before data can be put into
// it.
// ------------------------------------------------------------------------
void vsGeometry::setDataListSize(int whichData, int newSize)
{
    // Sanity check, primarily to avoid memory corruption
    if ((newSize < 0) || (newSize > VS_GEOMETRY_MAX_LIST_INDEX))
    {
        printf("vsGeometry::setDataListSize: Invalid list size '%d'\n",
            newSize);
        return;
    }

    // Resize the appropriate data list 
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            vertexList->resize(newSize);
            osgGeometry->setVertexArray(vertexList);
            vertexListSize = newSize;

            // Rebuild the osg::PrimitiveSet with the new settings
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

        case VS_GEOMETRY_TEXTURE_COORDS:
            texCoordList->resize(newSize);
            texCoordListSize = newSize;
        
            // If the texture coordinate binding is OFF, then the pointer
            // to the coordinate list should be NULL so that OSG knows not
            // to use texture coordinates at all.
            if (textureBinding == VS_GEOMETRY_BIND_NONE)
                osgGeometry->setTexCoordArray(0, NULL);
            else
                osgGeometry->setTexCoordArray(0, texCoordList);
            break;

        default:
            printf("vsGeometry::setDataListSize: Unrecognized data value\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves the size of one of the object's data lists
// ------------------------------------------------------------------------
int vsGeometry::getDataListSize(int whichData)
{
    // Return the size of the appropriate data list
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            return vertexListSize;
        case VS_GEOMETRY_NORMALS:
            return normalListSize;
        case VS_GEOMETRY_COLORS:
            return colorListSize;
        case VS_GEOMETRY_TEXTURE_COORDS:
            return texCoordListSize;
        default:
            printf("vsGeometry::getDataListSize: Unrecognized data value\n");
    }
    
    // If the whichData constant is unrecognized, return an error value
    return -1;
}

// ------------------------------------------------------------------------
// Enables lit rendering for this geometry
// ------------------------------------------------------------------------
void vsGeometry::enableLighting()
{
    osg::StateSet *osgStateSet;
    
    // Enable the GL lighting mode on the Geode's StateSet
    osgStateSet = osgGeode->getOrCreateStateSet();
    osgStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);
    
    // Mark lighting as enabled
    lightingEnable = VS_TRUE;
}

// ------------------------------------------------------------------------
// Disables lit rendering for this geometry
// ------------------------------------------------------------------------
void vsGeometry::disableLighting()
{
    osg::StateSet *osgStateSet;
    
    // Disable the GL lighting mode on the Geode's StateSet
    osgStateSet = osgGeode->getOrCreateStateSet();
    osgStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    
    // Mark lighting as disabled
    lightingEnable = VS_FALSE;
}

// ------------------------------------------------------------------------
// Returns if lighting is enabled for this geometry
// ------------------------------------------------------------------------
int vsGeometry::isLightingEnabled()
{
    return lightingEnable;
}

// ------------------------------------------------------------------------
// Sets the rendering bin to place this object's geometry into
// ------------------------------------------------------------------------
void vsGeometry::setRenderBin(int binNum)
{
    renderBin = binNum;
}

// ------------------------------------------------------------------------
// Gets the rendering bin that this object's geometry is placed into
// ------------------------------------------------------------------------
int vsGeometry::getRenderBin()
{
    return renderBin;
}

// ------------------------------------------------------------------------
// Static function
// Sets the geometry sorting mode for the specified bin number. Note that
// this is a *global* change; this will change the sorting mode for all
// geometry objects that use the specified bin number.
// ------------------------------------------------------------------------
void vsGeometry::setBinSortMode(int binNum, int sortMode)
{
    // Create the list if it doesn't already exist
    if (!binModeList)
        binModeList = new vsTreeMap();

    // If the target bin is already specified, change its value; else,
    // add a new bin entry to the list.
    if (binModeList->containsKey((void *)binNum))
        binModeList->changeValue((void *)binNum, (void *)sortMode);
    else
        binModeList->addEntry((void *)binNum, (void *)sortMode);

    // Mark that the global bin list changed so that the system object
    // will notice it next drawFrame() and force an update of all geometry
    // objects' bin data.
    binModesChanged = VS_TRUE;
}

// ------------------------------------------------------------------------
// Static function
// Gets the geometry sorting mode for the specified bin number
// ------------------------------------------------------------------------
int vsGeometry::getBinSortMode(int binNum)
{
    // If there's no list, return a default value
    if (!binModeList)
        return VS_GEOMETRY_SORT_STATE;

    // If there's no specified mode for the desired bin, return a
    // default value
    if (!(binModeList->containsKey((void *)binNum)))
        return VS_GEOMETRY_SORT_STATE;

    // Otherwise, return the bin's mode according to its setting in the
    // bin mode list
    return (int)(binModeList->getValue((void *)binNum));
}

// ------------------------------------------------------------------------
// Static function
// Clears all of the specified render bin sorting modes from the list by
// deleting the list; all sort mode queries return 'state-sorted' by
// default if there is no list.
// ------------------------------------------------------------------------
void vsGeometry::clearBinSortModes()
{
    // Delete the bin mode list, this will cause all render bins to be
    // state sorted
    if (binModeList)
    {
        delete binModeList;
        binModeList = NULL;
        binModesChanged = VS_TRUE;
    }
}

// ------------------------------------------------------------------------
// Retrieves the center point and radius of a sphere that encompasses all
// of the geometry within this object.
// ------------------------------------------------------------------------
void vsGeometry::getBoundSphere(vsVector *centerPoint, double *radius)
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
vsMatrix vsGeometry::getGlobalXform()
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
void vsGeometry::setIntersectValue(unsigned int newValue)
{
    osgGeode->setNodeMask(newValue);
}

// ------------------------------------------------------------------------
// Retrieves the intersection value for this geometry.
// ------------------------------------------------------------------------
unsigned int vsGeometry::getIntersectValue()
{
    return (osgGeode->getNodeMask());
}

// ------------------------------------------------------------------------
// Adds the given attribute to the geometry object's list of child
// attributes. If successful, also notifies the attribute that it has been
// added to a list.
// ------------------------------------------------------------------------
void vsGeometry::addAttribute(vsAttribute *newAttribute)
{
    int attrCat, attrType;
    int loop;

    // Verify that the attribute is willing to be attached
    if (!(newAttribute->canAttach()))
    {
        printf("vsGeometry::addAttribute: Attribute is already in use\n");
        return;
    }
    
    // vsGeometries can only contain state attributes for now
    attrCat = newAttribute->getAttributeCategory();
    if (attrCat != VS_ATTRIBUTE_CATEGORY_STATE)
    {
        printf("vsGeometry::addAttribute: Geometry nodes may not contain "
            "attributes of that type\n");
        return;
    }
    
    // vsGeometries can only contain one of each type of state attribute
    attrType = newAttribute->getAttributeType();
    for (loop = 0; loop < getAttributeCount(); loop++)
        if ((getAttribute(loop))->getAttributeType() == attrType)
        {
            printf("vsGeometry::addAttribute: Geometry node already "
                "contains that type of attribute\n");
            return;
        }

    // If we made it this far, it must be okay to add the attribute in
    vsNode::addAttribute(newAttribute);
}

// ------------------------------------------------------------------------
// Returns the Performer object associated with this object
// ------------------------------------------------------------------------
osg::Geode *vsGeometry::getBaseLibraryObject()
{
    return osgGeode;
}

// ------------------------------------------------------------------------
// Private function
// Erases and reconstructs the OSG Primitive object(s) describing the
// current geometry
// ------------------------------------------------------------------------
void vsGeometry::rebuildPrimitives()
{
    osg::DrawArrays *osgDrawArrays;
    osg::DrawArrayLengths *osgDrawArrayLengths;

    // Erase the current list of PrimitiveSets
    // * I hope this deletes them as well... supposedly they're held on to
    // by OSG's ref_ptr objects, meaning that they should get deleted when
    // the references are destroyed, but I don't know how thorough
    // std::vector is when destroying objects.
    (osgGeometry->getPrimitiveSetList()).clear();
    
    // Create one or more new PrimitiveSet objects based on the type,
    // number, and length data of the primitives stored in this vsGeometry
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
// Internal function
// Adds a node to this node's list of parent nodes
// ------------------------------------------------------------------------
int vsGeometry::addParent(vsNode *newParent)
{
    // Add the parent to our parent list and reference it
    parentList[parentCount++] = newParent;
    newParent->ref();
    
    // Return success
    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Internal function
// Removes a node from this node's list of parent nodes
// ------------------------------------------------------------------------
int vsGeometry::removeParent(vsNode *targetParent)
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
            targetParent->unref();
            parentCount--;

            // Return that the remove succeeded
            return VS_TRUE;
        }
    }

    // Return failure if the specified parent isn't found
    return VS_FALSE;
}

// ------------------------------------------------------------------------
// Internal function
// Calls the apply function on all attached attributes, and then calls the
// scene library's graphics state object to affect the changes to the
// graphics library state. Also applies the geometry's current rendering
// bin to the state set if specified for this object.
// ------------------------------------------------------------------------
void vsGeometry::applyAttributes()
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
        sortMode = getBinSortMode(renderBin);

        // Set the sort order on the corresponding osg::RenderBin
        if (sortMode == VS_GEOMETRY_SORT_DEPTH)
            osgStateSet->setRenderBinDetails(renderBin, "DepthSortedBin");
        else
            osgStateSet->setRenderBinDetails(renderBin, "RenderBin");
    }
}
