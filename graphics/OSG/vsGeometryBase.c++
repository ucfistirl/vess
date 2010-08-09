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
//    VESS Module:  vsGeometryBase.c++
//
//    Description:  vsNode subclass that is a base class for all geometry
//                  nodes.  Geometries are leaf nodes in a VESS scene
//                  graph. They store geometry data such as vertex and
//                  texture coordinates, colors, and face normals.
//
//    Author(s):    Bryan Kline, Duvan Cope, Jason Daly
//
//------------------------------------------------------------------------

#include "vsGeometryBase.h++"
#include "vsGraphicsState.h++"
#include "vsTextureAttribute.h++"
#include "vsTextureCubeAttribute.h++"
#include "vsTextureRectangleAttribute.h++"
#include "vsTransformAttribute.h++"
#include <osg/MatrixTransform>
#include <stdio.h>

// ------------------------------------------------------------------------
// Default Constructor - does nothing in the base class
// ------------------------------------------------------------------------
vsGeometryBase::vsGeometryBase() : parentList(5, 5)
{
    int loop;

    // Initialize number of parents to zero
    parentCount = 0;

    // Create an osg::Geode
    osgGeode = new osg::Geode();
    osgGeode->ref();

    // Create an osg::Geometry node to contain the Geode
    osgGeometry = new osg::Geometry();
    osgGeometry->ref();
    osgGeode->addDrawable(osgGeometry);

    // Initialize texture bindings to NONE
    for (loop = 0; loop < VS_MAXIMUM_TEXTURE_UNITS; loop++)
        textureBinding[loop] = VS_GEOMETRY_BIND_NONE;

    // Initialize all lists to NULL
    memset(dataList, 0, sizeof(dataList));
    memset(dataListSize, 0, sizeof(dataListSize));
    indexList = NULL;
    indexListSize = 0;
    lengthsList = NULL;

    // Initialize generic data flags (indicating whether a list is using
    // the generic or traditional vertex attributes) to false
    memset(dataIsGeneric, 0, sizeof(dataIsGeneric));

    // Create the various data arrays
    for (loop = 0; loop < VS_GEOMETRY_LIST_COUNT; loop++)
        allocateDataArray(loop);

    // Set the primitive count to 0, and the type to POINTS
    primitiveCount = 0;
    primitiveType = VS_GEOMETRY_TYPE_POINTS;

    // Lighting is enabled by default
    lightingEnable = true;

    // Set the render bin to NULL (just use the default bin)
    renderBin = NULL;
}

// ------------------------------------------------------------------------
// Destructor - does nothing in the base class
// ------------------------------------------------------------------------
vsGeometryBase::~vsGeometryBase()
{
    int loop;

    // If we're using vertex indices, unreference the index list now
    if (indexList)
        free(indexList);

    // Destroy the data lists
    for (loop = 0; loop < VS_GEOMETRY_LIST_COUNT; loop++)
        dataList[loop]->unref();

    // If we've created a primitive lengths list, free this now
    if (lengthsList)
        free(lengthsList);

    // Unlink and destroy the OSG objects
    osgGeometry->unref();
    osgGeode->unref();
}

// ------------------------------------------------------------------------
// Retrieves the number of parent nodes for this node
// ------------------------------------------------------------------------
int vsGeometryBase::getParentCount()
{
    return parentCount;
}

// ------------------------------------------------------------------------
// Retrieves one of the parent nodes of this node, specified by index.
// The index of the first parent is 0.
// ------------------------------------------------------------------------
vsNode *vsGeometryBase::getParent(int index)
{
    // Check the index to make sure it refers to a valid parent, complain 
    // and return NULL if not
    if ((index < 0) || (index >= parentCount))
    {
        printf("vsGeometryBase::getParent: Bad parent index\n");
        return NULL;
    }
    
    // Return the requested parent
    return (vsNode *)(parentList[index]);
}

// ------------------------------------------------------------------------
// Sets the type of geometric primitive that this object contains
// ------------------------------------------------------------------------
void vsGeometryBase::setPrimitiveType(int newType)
{
    // Make sure the type argument is a valid primitive type
    if ((newType < VS_GEOMETRY_TYPE_POINTS) ||
        (newType > VS_GEOMETRY_TYPE_POLYS))
    {
        printf("vsGeometryBase::setPrimitiveType: Unrecognized primitive type\n");
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
int vsGeometryBase::getPrimitiveType()
{
    return primitiveType;
}

// ------------------------------------------------------------------------
// Sets the number of geometric primitive that this object contains. Must
// be called before any calls to set the data for any specific primitive.
// ------------------------------------------------------------------------
void vsGeometryBase::setPrimitiveCount(int newCount)
{
    int loop;
    
    // Sanity check, primarily to avoid memory corruption
    if ((newCount < 0) || (newCount > VS_GEOMETRY_MAX_LIST_INDEX))
    {
        printf("vsGeometryBase::setPrimitiveCount: Invalid count value '%d'\n",
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
int vsGeometryBase::getPrimitiveCount()
{
    return primitiveCount;
}

// ------------------------------------------------------------------------
// Sets the number of verticies for the primitive with the specified index
// within the object. The index of the first primitive is 0.
// ------------------------------------------------------------------------
void vsGeometryBase::setPrimitiveLength(int index, int length)
{
    // Make sure the index is valid, given the current primitive count
    if ((index < 0) || (index >= getPrimitiveCount()))
    {
        printf("vsGeometryBase::setPrimitiveLength: Index out of bounds\n");
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
int vsGeometryBase::getPrimitiveLength(int index)
{
    // Make sure the index is valid, given the current primitive count
    if ((index < 0) || (index >= getPrimitiveCount()))
    {
        printf("vsGeometryBase::getPrimitiveLength: Index out of bounds\n");
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
void vsGeometryBase::setPrimitiveLengths(int *lengths)
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
void vsGeometryBase::getPrimitiveLengths(int *lengthsBuffer)
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
void vsGeometryBase::setBinding(int whichData, int binding)
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
            printf("vsGeometryBase::setBinding: Unrecognized binding value\n");
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
                printf("vsGeometryBase::setBinding: Vertex coordinate binding must "
                    "always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
            // There is no 'standard' binding for vertex weights; use the
            // generic attribute binding.
            osgGeometry->setVertexAttribBinding(whichData, osgBinding);
            break;

        case VS_GEOMETRY_NORMALS:
            // Set the normal binding to the new value
            osgGeometry->setNormalBinding(osgBinding);
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
        case VS_GEOMETRY_USER_DATA1:
            // There is no 'standard' binding for this data; use the
            // generic attribute binding.
            osgGeometry->setVertexAttribBinding(whichData, osgBinding);
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
                printf("vsGeometryBase::setBinding: Texture coordinates binding "
                    "must be either VS_GEOMETRY_BIND_PER_VERTEX or "
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
            printf("vsGeometryBase::setBinding: Unrecognized data value\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves the binding mode for the geometry object for the specified
// type of data
// ------------------------------------------------------------------------
int vsGeometryBase::getBinding(int whichData)
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
        case VS_GEOMETRY_VERTEX_COORDS:
            return VS_GEOMETRY_BIND_PER_VERTEX;
        case VS_GEOMETRY_VERTEX_WEIGHTS:
            result = osgGeometry->getVertexAttribBinding(whichData);
            break;
        case VS_GEOMETRY_NORMALS:
            result = osgGeometry->getNormalBinding();
            break;
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
        case VS_GEOMETRY_USER_DATA1:
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
            printf("vsGeometryBase::getBinding: Unrecognized data value\n");
abort();
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
void vsGeometryBase::setData(int whichData, int dataIndex, atVector data)
{
    int loop;
    int slotNum;
    int dataSize;

    // Determine the minimum required number of entries that should be in the
    // data parameter. A value of 0 here means that it doesn't matter. This
    // also doubles as a check to make sure that we recognize the specified
    // constant.
    dataSize = getDataElementCount(whichData);
    if (dataSize == -1)
    {
        printf("vsGeometryBase::setData: Unrecognized data type\n");
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
        printf("vsGeometryBase::setData: Index out of bounds\n");
        return;
    }

    // Make sure that the input vector has enough data
    if (data.getSize() < dataSize)
    {
        printf("vsGeometryBase::setData: Insufficient data (data of the given "
            "type requires at least %d values)\n", dataSize);
        return;
    }

    // If a conventional attribute is specified, then make sure we're not
    // already using the generic attribute, and vice versa.
    if (whichData < VS_GEOMETRY_LIST_COUNT)
    {
        // Conventional data specified
        if (dataIsGeneric[slotNum])
        {
            printf("vsGeometryBase::setData: Cannot use conventional data type "
                "when corresponding generic attribute is in use\n");
            return;
        }
    }
    else
    {
        // Generic attribute specified
        if (!(dataIsGeneric[slotNum]))
        {
            printf("vsGeometryBase::setData: Cannot use generic attribute type "
                "when corresponding conventional data is in use\n");
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
            for (loop = 0; loop < 3; loop++)
                ((*((osg::Vec3Array *)(dataList[slotNum])))[dataIndex])[loop]
                    = data[loop];
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
atVector vsGeometryBase::getData(int whichData, int dataIndex)
{
    atVector result;
    int loop;
    int slotNum;
    int dataSize;

    // Determine the minimum required number of entries that should be in the
    // data parameter. A value of 0 here means that it doesn't matter. This
    // also doubles as a check to make sure that we recognize the specified
    // constant.
    dataSize = getDataElementCount(whichData);
    if (dataSize == -1)
    {
        printf("vsGeometryBase::getData: Unrecognized data type\n");
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
        printf("vsGeometryBase::getData: Index out of bounds\n");
        return result;
    }

    // If a conventional attribute is specified, then make sure we're not
    // already using the generic attribute, and vice versa.
    if (whichData < VS_GEOMETRY_LIST_COUNT)
    {
        // Conventional data specified
        if (dataIsGeneric[slotNum])
        {
            printf("vsGeometryBase::getData: Cannot use conventional data type "
                "when corresponding generic attribute is in use\n");
            return result;
        }
    }
    else
    {
        // Generic attribute specified
        if (!(dataIsGeneric[slotNum]))
        {
            printf("vsGeometryBase::getData: Cannot use generic attribute type "
                "when corresponding conventional data is in use\n");
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
            for (loop = 0; loop < 3; loop++)
                result[loop] =
                    (( *((osg::Vec3Array *)(dataList[slotNum])) )
                        [dataIndex])[loop];
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
void vsGeometryBase::setDataList(int whichData, atVector *dataBuffer)
{
    int loop, sloop;
    int slotNum;
    int dataSize;

    // Determine the minimum required number of entries that should be in the
    // data parameters. A value of 0 here means that it doesn't matter. This
    // also doubles as a check to make sure that we recognize the specified
    // constant.
    dataSize = getDataElementCount(whichData);
    if (dataSize == -1)
    {
        printf("vsGeometryBase::setDataList: Unrecognized data type\n");
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
            printf("vsGeometryBase::setDataList: Cannot use conventional data "
                "type when corresponding generic attribute is in use\n");
            return;
        }
    }
    else
    {
        // Generic attribute specified
        if (!(dataIsGeneric[slotNum]))
        {
            printf("vsGeometryBase::setDataList: Cannot use generic attribute "
                "type when corresponding conventional data is in use\n");
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
            for (loop = 0; loop < dataListSize[slotNum]; loop++)
                for (sloop = 0; sloop < 3; sloop++)
                    ((*((osg::Vec3Array *)(dataList[slotNum])))[loop])[sloop]
                        = dataBuffer[loop][sloop];
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
void vsGeometryBase::getDataList(int whichData, atVector *dataBuffer)
{
    int loop, sloop;
    int slotNum;
    int dataSize;

    // Determine the minimum required number of entries that should be in the
    // data parameter. A value of 0 here means that it doesn't matter. This
    // also doubles as a check to make sure that we recognize the specified
    // constant.
    dataSize = getDataElementCount(whichData);
    if (dataSize == -1)
    {
        printf("vsGeometryBase::getDataList: Unrecognized data type\n");
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
            printf("vsGeometryBase::getDataList: Cannot use conventional data "
                "type when corresponding generic attribute is in use\n");
            return;
        }
    }
    else
    {
        // Generic attribute specified
        if (!(dataIsGeneric[slotNum]))
        {
            printf("vsGeometryBase::getDataList: Cannot use generic attribute "
                "type when corresponding conventional data is in use\n");
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
            for (loop = 0; loop < dataListSize[slotNum]; loop++)
            {
                dataBuffer[loop].setSize(3);
                for (sloop = 0; sloop < 3; sloop++)
                    dataBuffer[loop][sloop] =
                        (( *((osg::Vec3Array *)(dataList[slotNum])) )
                            [loop])[sloop];
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
void vsGeometryBase::setDataListSize(int whichData, int newSize)
{
    int slotNum;
    int dataSize;

    // Determine the type of the data array associated with the specified
    // data parameter. A value of 0 here means that we are using Vec4s. This
    // also doubles as a check to make sure that we recognize the specified
    // constant.
    dataSize = getDataElementCount(whichData);
    if (dataSize == -1)
    {
        printf("vsGeometryBase::setDataListSize: Unrecognized data type\n");
        return;
    }

    // Sanity check, primarily to avoid memory corruption
    if ((newSize < 0) || (newSize > VS_GEOMETRY_MAX_LIST_INDEX))
    {
        printf("vsGeometryBase::setDataListSize: Invalid list size '%d'\n",
            newSize);
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
            printf("vsGeometryBase::setDataListSize: Cannot use conventional "
                "data type when corresponding generic attribute is in use\n");
            return;
        }
    }
    else
    {
        // Generic attribute specified
        if ((!dataIsGeneric[slotNum]) && (dataListSize[slotNum] > 0))
        {
            printf("vsGeometryBase::setDataListSize: Cannot use generic "
                "attribute type when corresponding conventional data is in "
                "use\n");
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
int vsGeometryBase::getDataListSize(int whichData)
{
    int slotNum;

    // Bounds checking
    if ((whichData < 0) || (whichData > (VS_GEOMETRY_LIST_COUNT * 2)))
    {
        printf("vsGeometryBase::getDataListSize: Unrecognized data value\n");
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
// Sets one of the indexes in the geometry's index list
// ------------------------------------------------------------------------
void vsGeometryBase::setIndex(int indexIndex, u_int newIndex)
{
    // Make sure the index's index in the list is valid
    if ((indexIndex < 0) || (indexIndex > indexListSize))
    {
        printf("vsGeometryBase::setIndex: Index is out of range\n");
        return;
    }

    // Set the index
    indexList[indexIndex] = newIndex;

    // A change to the index list means the primitives need rebuilding
    rebuildPrimitives();
}

// ------------------------------------------------------------------------
// Retrieves one of the indexes in the geometry's index list
// ------------------------------------------------------------------------
u_int vsGeometryBase::getIndex(int indexIndex)
{
    // Make sure the index's index in the list is valid
    if ((indexIndex < 0) || (indexIndex > indexListSize))
    {
        printf("vsGeometryBase::setIndex: Index is out of range\n");
        return 0;
    }

    // Return the desired index
    return indexList[indexIndex];
}

// ------------------------------------------------------------------------
// Sets all of the indexes in the geometry's index list (the list provided
// must contain enough indices to fill the current index list)
// ------------------------------------------------------------------------
void vsGeometryBase::setIndexList(u_int *indexBuffer)
{
    int i;

    // Don't try to set it if it isn't there
    if (indexList == NULL)
    {
        printf("vsGeometryBase::setIndexList: Tried to set NULL index list!\n");
        return;
    }

    // Set all indices
    memcpy(indexList, indexBuffer, indexListSize * sizeof(u_int));

    // A change to the index list means the primitives need rebuilding
    rebuildPrimitives();
}

// ------------------------------------------------------------------------
// Retrieves all of the indexes in the geometry's index list (the buffer
// provided must be large enough to hold all index values)
// ------------------------------------------------------------------------
void vsGeometryBase::getIndexList(u_int *indexBuffer)
{
    int i;

    // If we have no index list, don't try to retrieve anything
    if (indexList == NULL)
        return;

    // Copy the indices from the index list to the buffer provided
    memcpy(indexBuffer, indexList, indexListSize * sizeof(u_int));
}

// ------------------------------------------------------------------------
// Sets the size of the list for vertex indices.  When using vertex 
// indices, the vertices specified by the index list are used to render the
// primitives, instead of pulling vertices from the data lists directly.
// ------------------------------------------------------------------------
void vsGeometryBase::setIndexListSize(int newSize)
{
    // See if the size is valid
    if ((newSize < 0) || (newSize > VS_GEOMETRY_MAX_LIST_INDEX))
    {
        printf("vsGeometryBase::vsGeometryBase:  Index list size is invalid.\n");
        return;
    }

    // See if we already have an index list
    if ((newSize > 0) && (indexList == NULL))
    {
        // Create a new index list
        indexList = (u_int *)calloc(newSize, sizeof(u_int));
        indexListSize = newSize;
    }
    else if ((newSize == 0) && (indexList != NULL))
    {
        // Delete the existing index list
        free(indexList);
        indexList = NULL;
        indexListSize = 0;
    }
    else
    {
        // Modify the current index list
        indexList = (u_int *)realloc(indexList, sizeof(u_int) * newSize);
        indexListSize = newSize;
    }

    // A change to the index list will most likely mean the primitive set
    // needs rebuilding
    rebuildPrimitives();
}

// ------------------------------------------------------------------------
// Return the current size of the vertex index list
// ------------------------------------------------------------------------
int vsGeometryBase::getIndexListSize()
{
    return indexListSize;
}

// ------------------------------------------------------------------------
// Enables lit rendering for this geometry
// ------------------------------------------------------------------------
void vsGeometryBase::enableLighting()
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
void vsGeometryBase::disableLighting()
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
bool vsGeometryBase::isLightingEnabled()
{
    return lightingEnable;
}

// ------------------------------------------------------------------------
// Sets the rendering bin to place this object's geometry into
// ------------------------------------------------------------------------
void vsGeometryBase::setRenderBin(vsRenderBin *newBin)
{
    // Unreference the old bin, if necessary
    if (renderBin != NULL)
       renderBin->unref();

    // Store the new bin and reference it
    renderBin = newBin;
    renderBin->ref();
}

// ------------------------------------------------------------------------
// Gets the rendering bin that this object's geometry is placed into
// ------------------------------------------------------------------------
vsRenderBin *vsGeometryBase::getRenderBin()
{
    return renderBin;
}

// ------------------------------------------------------------------------
// Retrieves the center point and radius of a sphere that encompasses all
// of the geometry within this object.
// ------------------------------------------------------------------------
void vsGeometryBase::getBoundSphere(atVector *centerPoint, double *radius)
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
atMatrix vsGeometryBase::getGlobalXform()
{
    osg::Node *nodePtr;
    osg::Matrix xform;
    osg::Matrix osgXformMat;
    atMatrix result;
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

    // Return the resulting atMatrix
    return result;
}

// ------------------------------------------------------------------------
// Sets the intersection value for this geometry. During an intersection
// run, at each geometry object a bitwise AND of the intersection's mask
// and the geometry's value is performed; if the result of the AND is zero,
// the intersection ignores the geometry.
// ------------------------------------------------------------------------
void vsGeometryBase::setIntersectValue(unsigned int newValue)
{
    osgGeode->setNodeMask(newValue);
}

// ------------------------------------------------------------------------
// Retrieves the intersection value for this geometry.
// ------------------------------------------------------------------------
unsigned int vsGeometryBase::getIntersectValue()
{
    return (osgGeode->getNodeMask());
}

// ------------------------------------------------------------------------
// Adds the given attribute to the geometry object's list of child
// attributes. If successful, also notifies the attribute that it has been
// added to a list.
// ------------------------------------------------------------------------
void vsGeometryBase::addAttribute(vsAttribute *newAttribute)
{
    int newAttrCat, newAttrType, attrType;
    int loop;
    vsAttribute *attribute;
    unsigned int textureUnit, newTextureUnit;

    // Verify that the attribute is willing to be attached
    if (!(newAttribute->canAttach()))
    {
        printf("vsGeometryBase::addAttribute: Attribute is already in use\n");
        return;
    }
    
    // vsGeometries can only contain state attributes for now
    newAttrCat = newAttribute->getAttributeCategory();
    if (newAttrCat != VS_ATTRIBUTE_CATEGORY_STATE)
    {
        printf("vsGeometryBase::addAttribute: Geometry nodes may not contain "
            "attributes of that type\n");
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
            printf("vsGeometryBase::addAttribute: Geometry node "
                "already contains that type of attribute\n");
            return;
        }
                                                                                
        // If the texture units are equal then they both must have been texture
        // type attributes and had the same unit.  We don't want that to be
        // allowed so print error and return.
        if (textureUnit == newTextureUnit)
        {
            printf("vsGeometryBase::addAttribute: Geometry node "
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
void vsGeometryBase::enableCull()
{
    osgGeode->setCullingActive(true);
}

// ------------------------------------------------------------------------
// Disables culling on this node and its children
// ------------------------------------------------------------------------
void vsGeometryBase::disableCull()
{
    osgGeode->setCullingActive(false);
}

// ------------------------------------------------------------------------
// Deindexes the geometry by expanding all active data lists to match
// what is currently represented by the index list.  Since the index list
// is being used, this method assumes all active lists are in PER_VERTEX
// or OVERALL mode
// ------------------------------------------------------------------------
void vsGeometryBase::deindexGeometry()
{
    int i, j;
    atVector *newList;
    int whichData;
    int listSize;

    // Don't try to deindex if there is no index list
    if (indexListSize <= 0)
        return;

    // Sanity check.  Don't go any farther if any list is in PER_PRIMITIVE
    // mode
    for (i = 0; i < VS_GEOMETRY_LIST_COUNT; i++)
    {
        if (dataIsGeneric[i])
            whichData = i + VS_GEOMETRY_LIST_COUNT;
        else
            whichData = i;

        if (getBinding(whichData) == VS_GEOMETRY_BIND_PER_PRIMITIVE)
        {
            printf("vsGeometryBase::deindexGeometry:\n");
            printf("   ERROR: Geometry is using indexed rendering, but has "
                "PER_PRIMITIVE vertex data!\n");
            return;
        }
    }

    // Get the current index list size (this will be the size for each
    // PER_VERTEX list)
    listSize = indexListSize;

    // Create a new data list for each active list, and de-index the
    // current list into it
    for (i = 0; i < VS_GEOMETRY_LIST_COUNT; i++)
    {
        if (dataIsGeneric[i])
            whichData = i + VS_GEOMETRY_LIST_COUNT;
        else
            whichData = i;

        // Check the binding, and de-index any PER_VERTEX lists (OVERALL
        // lists can stay as they are)
        if (getBinding(whichData) == VS_GEOMETRY_BIND_PER_VERTEX)
        {
            // Create a new list
            newList = new atVector[listSize];

            // De-index the list data
            for (j = 0; j < listSize; j++)
                newList[j] = getData(whichData, indexList[j]);

            // Set the new data list
            setDataListSize(whichData, listSize);
            setDataList(whichData, newList);

            // OSG now has our new list, so clean up
            delete [] newList;
        }
    }

    // Remove the index list and rebuild the geometry
    setIndexListSize(0);
    rebuildPrimitives();
}

// ------------------------------------------------------------------------
// Expands a list that is currently bound as PER_PRIMITIVE to an equivalent
// PER_VERTEX binding
// ------------------------------------------------------------------------
void vsGeometryBase::expandToPerVertex(int whichData)
{
    atVector *newList;
    int i, j;
    int vertexCount;
    int currentVertex;
    atVector value;

    // Bail if this list isn't bound to PER_PRIMITIVE
    if (getBinding(whichData) != VS_GEOMETRY_BIND_PER_PRIMITIVE)
    {
        printf("vsGeometryBase::expandToPerVertex:  List isn't currently "
            "bound as PER_PRIMITIVE!\n");
        return;
    }

    // Get the number of vertices and create a new list
    vertexCount = getDataListSize(VS_GEOMETRY_VERTEX_COORDS);
    newList = new atVector[vertexCount];

    // Iterate over each primitive
    currentVertex = 0;
    for (i = 0; i < getPrimitiveCount(); i++)
    {
        // Iterate over the vertices of this primitive and copy the primitive's
        // data value to the new list, creating a copy of the data per
        // primitive vertex
        value = getData(whichData, i);
        for (j = 0; j < getPrimitiveLength(i); j++)
            newList[currentVertex++] = value;
    }

    // Now, set the new list data and binding
    setDataList(whichData, newList);
    setBinding(whichData, VS_GEOMETRY_BIND_PER_VERTEX);

    // The list data has been copied to OSG, so clean up our list
    delete [] newList;
}

// ------------------------------------------------------------------------
// Optimizes the vertex data lists by searching for duplicate vertices
// (i.e.: vertices that have the same data in all lists that are in use),
// and re-indexing them so that all duplicates are indexed to a single
// instance of that vertex
// ------------------------------------------------------------------------
void vsGeometryBase::optimizeVertices()
{
    int listSize;
    int i, j, k;
    u_int **candidateList;
    u_int *candidateCount;
    u_int *candidateSize;
    u_int *newList;
    u_int v1, v2;
    atVector vec1, vec2;
    u_int *adjustment;
    u_int currentAdjustment;
    int whichData;

    // Check for any PER_PRIMITIVE list bindings.  If there are any,
    // refactor them into PER_VERTEX lists (PER_PRIMITIVE data is incompatible
    // with indexed rendering, and it is slow to draw)
    if (getBinding(VS_GEOMETRY_NORMALS) == VS_GEOMETRY_BIND_PER_PRIMITIVE)
        expandToPerVertex(VS_GEOMETRY_NORMALS);
    if (getBinding(VS_GEOMETRY_COLORS) == VS_GEOMETRY_BIND_PER_PRIMITIVE)
        expandToPerVertex(VS_GEOMETRY_COLORS);
    if (getBinding(VS_GEOMETRY_ALT_COLORS) == VS_GEOMETRY_BIND_PER_PRIMITIVE)
        expandToPerVertex(VS_GEOMETRY_ALT_COLORS);
    if (getBinding(VS_GEOMETRY_FOG_COORDS) == VS_GEOMETRY_BIND_PER_PRIMITIVE)
        expandToPerVertex(VS_GEOMETRY_FOG_COORDS);

    // If the geometry is currently using an index list, de-index it first.
    // This makes the optimization process much simpler
    if (getIndexListSize() > 0)
        deindexGeometry();

    // Now, create a new index list for the geometry that is a simple 1:1
    // contiguous mapping from each vertex to its attribute index in each
    // data list
    listSize = getDataListSize(VS_GEOMETRY_VERTEX_COORDS);
    setIndexListSize(listSize);
    for (i = 0; i < listSize; i++)
        indexList[i] = i;

    // Create some temporary lists that we'll use to keep track of good
    // candidates for optimization.  A "good" candidate is simply a vertex
    // that shares the same position as a previous vertex in the list
    candidateList = (u_int **)calloc(listSize, sizeof(u_int *));
    candidateCount = (u_int *)calloc(listSize, sizeof(u_int));
    candidateSize = (u_int *)calloc(listSize, sizeof(u_int));
    for (i = 0; i < listSize; i++)
    {
        // Create the array for this vertex and set its size to 16
        candidateList[i] = (u_int *)calloc(16, sizeof(int));
        candidateSize[i] = 16;

        // Add this vertex to its own list of equivalents (i.e.: the first
        // candidate in any vertex's candidate list is the vertex itself).
        // If this vertex is found to be equivalent to another vertex, it
        // will be removed from its own list
        candidateList[i][0] = i;
        candidateCount[i] = 1;
    }

    // Now, iterate over the temporary list and find all candidates for
    // optimization.  We'll do this in such a way that the candidates for
    // optimization are associated with the first equivalent vertex in the
    // list.  This first pass will speed up the full optimization process
    // below, because we won't need to check the full data set for every
    // vertex against every other vertex.  We'll only do the full check on
    // the vertices that share the same position
    for (i = 0; i < listSize; i++)
    {
        // See if this vertex is already known to be a candidate for another
        // vertex or not
        if (candidateCount[i] > 0)
        {
            // Get the position of this vertex
            vec1 = getData(VS_GEOMETRY_VERTEX_COORDS, i);

            // Iterate over the remaining vertices
            for (j = i+1; j < listSize; j++)
            {
                // Get the position of this vertex and compare the two
                vec2 = getData(VS_GEOMETRY_VERTEX_COORDS, j);
                if (vec1.isEqual(vec2))
                {
                    // Add this vertex to the candidate list of the
                    // earlier equivalent vertex
                    candidateCount[i]++;

                    // Expand the candidate list, if necessary
                    if (candidateCount[i] >= candidateSize[i])
                    {
                        // Reallocate the list
                        newList = (u_int *)realloc(candidateList[i],
                            candidateSize[i] * 2 * sizeof(u_int));

                        // Update the list size
                        candidateList[i] = newList;
                        candidateSize[i] *= 2;
                    }

                    // Add vertex j to the existing list
                    candidateList[i][candidateCount[i]-1] = j;

                    // Remove vertex j from its own list
                    candidateCount[j] = 0;
                }
            }
        }
    }

    // Create a list that will store how many spaces we'll need to slide
    // each vertex and index after the optimization is complete
    adjustment = new u_int[listSize];
    currentAdjustment = 0;

    // Iterate over each vertex of the geometry and see if any vertices
    // have nearly the same data across all relevant data lists.  Use the
    // same temporary list we created above to keep track of equivalent
    // vertices
    for (i = 0; i < listSize; i++)
    {
        // This is for readability... v1 is the i'th vertex in the
        // geometry
        v1 = (u_int) i;

        // See if this vertex is unique, or if it's already deemed equivalent
        // to another vertex
        if (candidateCount[v1] > 0)
        {
            // This vertex is currently unique.  Check each candidate
            // vertex to see if they are totally equivalent to this
            // vertex (skip the first candidate, as we know the vertex
            // is equivalent to itself)
            for (j = 1; j < candidateCount[v1]; j++)
            {
                // This is for readability... v1 is the i'th vertex in the
                // geometry and v2 is the j'th candidate equivalent vertex
                // of v1
                v2 = candidateList[v1][j];

                // Check for equivalence
                if (areVerticesEquivalent(v1, v2))
                {
                    // The vertices are equivalent, so leave the candidates
                    // array as it is (this vertex will be optimized away).
                    // Update the index list to have v2 point to v1
                    indexList[v2] = v1;
                }
                else
                {
                    // These vertices are not equivalent.  Put this vertex
                    // back into its own place in the candidates array
                    candidateList[v2][0] = v2;

                    // Any remaining candidate vertices for v1 might also be
                    // equivalent to v2, so copy them as candidates of
                    // v2 as well
                    if (candidateSize[v2] < candidateCount[v1] - j)
                    {
                        // Make sure the list has enough space before copying
                        newList = (u_int *)realloc(candidateList[v2],
                            candidateCount[v1] * 2 * sizeof(u_int));

                        // Update the list size
                        candidateList[v2] = newList;
                        candidateSize[v2] = candidateCount[v1] * 2;
                    }

                    // Copy the remaining vertices from the candidate
                    // list for v1 to the candidate list for v2
                    candidateCount[v2] = candidateCount[v1] - j + 1;
                    for (k = 1; k < candidateCount[v2]; k++)
                        candidateList[v2][k] = candidateList[v1][j+k];

                    // Remove this vertex from the candidates for v1 (we
                    // now know that v1 and v2 are not equivalent)
                    candidateCount[v1]--;
                    if (j < candidateCount[v1])
                        memmove(&candidateList[v1][j], &candidateList[v1][j+1],
                            sizeof(u_int) * (candidateCount[v1] - j));
                }
            }

            // We didn't optimize away this vertex, so put the same adjustment
            // as for the last vertex in this vertex's slot
            adjustment[i] = currentAdjustment;
        }
        else
        {
            // This vertex is equivalent to another vertex, so it should
            // be optimized away.  Increment the current adjustment value,
            // and store it in the adjustment array (this indicates each
            // following vertex must be moved back one additional place
            // to account for the removal of this vertex)
            currentAdjustment++;
            adjustment[i] = currentAdjustment;
        }
    }

    // We're done with the candidate list and supporting data now (all we
    // need from now on is the adjustment list we just constructed)
    for (i = 0; i < listSize; i++)
        free(candidateList[i]);
    free(candidateList);
    free(candidateCount);
    free(candidateSize);

    // Now, use the adjustment list to compact the vertex attribute lists
    // by removing duplicate vertex data values, and sliding the remaining
    // data into place.  Also adjust the index list to account for the new
    // positions of the unique data values
    for (i = 0; i < listSize; i++)
    {
        // Adjust the index array according to the adjustment array that
        // we created above
        indexList[i] -= adjustment[indexList[i]];

        // See if this vertex is unique, or if it will be optimized away.
        // We can do this by comparing the adjustment value for this vertex
        // with the value for the previous vertex.  If they are different,
        // then this vertex is redundant, and we should ignore its data.
        // If they are equal, then this vertex's data is important, and
        // we should moved it into the correct position
        if ((i > 0) && (adjustment[i] == adjustment[i-1]))
        {
            // Iterate over all potential vertex attribute lists
            for (j = 0; j < VS_GEOMETRY_LIST_COUNT; j++)
            {
                // Account for the use of generic attributes
                if (dataIsGeneric[j])
                    whichData = j + VS_GEOMETRY_LIST_COUNT;
                else
                    whichData = j;

                // See if this list is PER_VERTEX, and adjust the i'th item
                // if so
                if ((getBinding(whichData) == VS_GEOMETRY_BIND_PER_VERTEX) &&
                    (dataListSize[whichData] > 0))
                {
                    // Move the data according to the adjustment array
                    vec1 = getData(whichData, i);
                    setData(whichData, i-adjustment[i], vec1);
                }
            }
        }
    }

    // Finally, resize all of the vertex attribute lists to the new
    // (hopefully smaller) size
    listSize -= adjustment[listSize-1];
    for (i = 0; i < VS_GEOMETRY_LIST_COUNT; i++)
    {
        // Account for the use of generic attributes
        if (dataIsGeneric[i])
            whichData = i + VS_GEOMETRY_LIST_COUNT;
        else
            whichData = i;

        // See if this list is PER_VERTEX, and resize it if so
        if ((getBinding(whichData) == VS_GEOMETRY_BIND_PER_VERTEX) &&
            (dataListSize[whichData] > 0))
            setDataListSize(whichData, listSize);
    }

    // Now we're done with the adjustment list as well
    delete [] adjustment;

    // Now that we have a new index list, rebuild the primitive sets
    rebuildPrimitives();
}

// ------------------------------------------------------------------------
// Returns the OSG object associated with this object
// ------------------------------------------------------------------------
osg::Geode *vsGeometryBase::getBaseLibraryObject()
{
    return osgGeode;
}

// ------------------------------------------------------------------------
// Private function
// Erases and reconstructs the OSG Primitive object(s) describing the
// current geometry
// ------------------------------------------------------------------------
void vsGeometryBase::rebuildPrimitives()
{
    int numSets;
    osg::DrawArrays *osgDrawArrays;
    osg::DrawArrayLengths *osgDrawArrayLengths;
    osg::DrawElementsUInt *osgDrawElements;
    int i, indexIndex;

    // Erase the current list of PrimitiveSets
    numSets = osgGeometry->getNumPrimitiveSets();
    if (numSets > 0)
        osgGeometry->removePrimitiveSet(0, numSets);

    // If we have an index list, we'll be using the DrawElementsUInt primitive
    // set type
    if (indexList != NULL)
    {
        // If we're using one of the fixed-length primitive types, we'll only
        // need one DrawElementsUInt
        if ((primitiveType == VS_GEOMETRY_TYPE_POINTS) ||
            (primitiveType == VS_GEOMETRY_TYPE_LINES) ||
            (primitiveType == VS_GEOMETRY_TYPE_TRIS) ||
            (primitiveType == VS_GEOMETRY_TYPE_QUADS))
        {
            // Initialize the DrawElements variable
            osgDrawElements = NULL;

            // A single DrawElementsUInt primitive set is enough to
            // handle all of our primitives in this case
            switch (primitiveType)
            {
                case VS_GEOMETRY_TYPE_POINTS:
                    osgDrawElements = 
                        new osg::DrawElementsUInt(
                           osg::PrimitiveSet::POINTS, indexListSize,
                           indexList);
                    break;
                case VS_GEOMETRY_TYPE_LINES:
                    osgDrawElements = 
                        new osg::DrawElementsUInt(
                           osg::PrimitiveSet::LINES, indexListSize,
                           indexList);
                    break;
                case VS_GEOMETRY_TYPE_TRIS:
                    osgDrawElements = 
                        new osg::DrawElementsUInt(
                           osg::PrimitiveSet::TRIANGLES, indexListSize,
                           indexList);
                    break;
                case VS_GEOMETRY_TYPE_QUADS:
                    osgDrawElements = 
                        new osg::DrawElementsUInt(
                           osg::PrimitiveSet::QUADS, indexListSize,
                           indexList);
                    break;
            }

            // Make sure the DrawElements is valid, and add the new
            // primitive set to the OSG Geometry if so
            if (osgDrawElements != NULL)
                osgGeometry->addPrimitiveSet(osgDrawElements);
        }
        else
        {
            // Start at the first index in the index list
            indexIndex = 0;

            // We'll need to add one DrawElementsUInt per primitive in this
            // case
            for (i = 0; i < primitiveCount; i++)
            {
                // Create a DrawElementsUInt primitive set for this
                // primitive
                switch (primitiveType)
                {
                    case VS_GEOMETRY_TYPE_LINE_STRIPS:
                        osgDrawElements =
                            new osg::DrawElementsUInt(
                               osg::PrimitiveSet::LINE_STRIP,
                               lengthsList[i], &indexList[indexIndex]);
                        break;
                    case VS_GEOMETRY_TYPE_LINE_LOOPS:
                        osgDrawElements =
                            new osg::DrawElementsUInt(
                               osg::PrimitiveSet::LINE_LOOP,
                               lengthsList[i], &indexList[indexIndex]);
                        break;
                    case VS_GEOMETRY_TYPE_TRI_STRIPS:
                        osgDrawElements =
                            new osg::DrawElementsUInt(
                               osg::PrimitiveSet::TRIANGLE_STRIP,
                               lengthsList[i], &indexList[indexIndex]);
                        break;
                    case VS_GEOMETRY_TYPE_TRI_FANS:
                        osgDrawElements =
                            new osg::DrawElementsUInt(
                               osg::PrimitiveSet::TRIANGLE_FAN,
                               lengthsList[i], &indexList[indexIndex]);
                        break;
                    case VS_GEOMETRY_TYPE_QUAD_STRIPS:
                        osgDrawElements =
                            new osg::DrawElementsUInt(
                               osg::PrimitiveSet::QUAD_STRIP,
                               lengthsList[i], &indexList[indexIndex]);
                        break;
                    case VS_GEOMETRY_TYPE_POLYS:
                        osgDrawElements =
                            new osg::DrawElementsUInt(
                               osg::PrimitiveSet::POLYGON,
                               lengthsList[i], &indexList[indexIndex]);
                        break;
                }

                // Make sure the DrawElements is valid, and add the new
                // primitive set to the OSG Geometry if so
                if (osgDrawElements != NULL)
                    osgGeometry->addPrimitiveSet(osgDrawElements);

                // Advance in the index list by the length of the current
                // primitive
                indexIndex += lengthsList[i];
            }
        }
    }
    else
    {
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
}

// ------------------------------------------------------------------------
// Private function
// Gets the number of elements in the vectors for the specified data type.
// A return value of 0 is a 'don't care' value; although these types
// typically have 4-element vectors allocated to them, it is not required
// to complete fill all 4 values. A return value of -1 indicates an error.
// ------------------------------------------------------------------------
int vsGeometryBase::getDataElementCount(int whichData)
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
void vsGeometryBase::allocateDataArray(int whichData)
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
void vsGeometryBase::notifyOSGDataChanged(int whichData)
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
bool vsGeometryBase::addParent(vsNode *newParent)
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
bool vsGeometryBase::removeParent(vsNode *targetParent)
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
// Figures out what the top left and bottom right coordinates of this
// particular geometry is
// ------------------------------------------------------------------------
void vsGeometryBase::getAxisAlignedBoxBounds(atVector *minValues,
    atVector *maxValues)
{
    int childCount = getChildCount();
    int dataCount;
    int cntGData;
    int column;
    atVector tempMinValues;
    atVector tempMaxValues;
    atVector passMinValues;
    atVector passMaxValues;
    atVector oldPoint;
    atVector newPoint;
    vsTransformAttribute *transform = NULL;
    atMatrix dynamicMatrix;
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
// Internal function
// Calls the apply function on all attached attributes, and then calls the
// scene library's graphics state object to affect the changes to the
// graphics library state. Also applies the geometry's current rendering
// bin to the state set if specified for this object.
// ------------------------------------------------------------------------
void vsGeometryBase::applyAttributes()
{
    osg::StateSet *osgStateSet;
    int sortMode;

    // Call the inherited applyAttributes function
    vsNode::applyAttributes();

    // Apply the attributes that we track during the traversal to this
    // Geometry's OSG StateSet.  Note that we use the OSG Geometry's StateSet
    // instead of the Geode's StateSet here.  This is because we have to clear
    // the StateSet before we apply anything, and this would wipe out any
    // other attributes that have already been applied by the parent class.
    osgStateSet = osgGeometry->getOrCreateStateSet();
    (vsGraphicsState::getInstance())->applyState(osgStateSet);
    
    // If the render bin is specified, set the bin details in the state
    // set. This overrides any bin set by attributes, notably transparency
    // attributes.
    if (renderBin != NULL)
    {
        // Get the bin's sort mode
        sortMode = renderBin->getSortMode();

        // Set the sort order on the corresponding osg::RenderBin
        if (sortMode == VS_RENDER_BIN_SORT_DEPTH)
            osgStateSet->
                setRenderBinDetails(renderBin->getNumber(), "DepthSortedBin");
        else
            osgStateSet->
                setRenderBinDetails(renderBin->getNumber(), "RenderBin");
    }
}

// ------------------------------------------------------------------------
// Compares all vertex attributes for the two vertices at the given indices
// and returns whether or not they are equivalent (i.e.:  all attribute
// values are within tolerance).  Since this method is intended to help
// optimize the number of vertices rendered by this geometry, the geometry
// must be using indexed rendering, and all attributes in use must be bound
// PER_VERTEX or OVERALL for this method to return true (otherwise, it's
// not possible to reduce the number of vertices and maintain the same
// geometry)
// ------------------------------------------------------------------------
bool vsGeometryBase::areVerticesEquivalent(int v1, int v2)
{
    atVector vec1, vec2;
    int whichData;
    int i;

    // The geometry must be using an index list.  If not, we bail
    if (indexListSize <= 0)
    {
        printf("vsGeometryBase::areVerticesEquivalent:  Geometry is not using"
            "indexed rendering.");
        return false;
    }

    // Make sure the two indices make sense
    if ((v1 < 0) || (v1 >= indexListSize) || (v2 < 0) || (v2 >= indexListSize))
    {
        printf("vsGeometryBase::areVerticesEquivalent:  Index out of range.");
        return false;
    }

    // Now, compare all vertex attributes for the two vertices
    for (i = 0; i < VS_GEOMETRY_LIST_COUNT; i++)
    {
        // If this list is using a generic attribute, adjust the index to
        // indicate this
        if (dataIsGeneric[i])
            whichData = i + VS_GEOMETRY_LIST_COUNT;
        else
            whichData = i;

        // Check the per-vertex attributes to see if they match (OVERALL
        // attributes will obviously always match, PER_PRIMITIVE values
        // aren't allowed)
        if (getBinding(whichData) == VS_GEOMETRY_BIND_PER_VERTEX)
        {
            // Get the two corresponding attribute values
            vec1 = getData(whichData, v1);
            vec2 = getData(whichData, v2);

            // If the values don't match, the vertex isn't equivalent
            if (!vec1.isEqual(vec2))
                return false;
        }
        else if (getBinding(whichData) == VS_GEOMETRY_BIND_PER_PRIMITIVE)
        {
            printf("vsGeometryBase::areVerticesEquivalent:  Geometry is using"
                "per-primitive attributes.");
            return false;
        }
    }

    // If we get this far, the vertices must be equivalent
    return true;
}

