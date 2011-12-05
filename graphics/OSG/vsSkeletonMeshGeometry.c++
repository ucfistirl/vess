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
//    Description:  vsGeometryBase subclass that handles geometry for
//                  skinned characters.  Skinning may be done in software
//                  using the additional applySkin() method of this class,
//                  or in hardware using an appropriate GPU program.
//
//    Author(s):    Duvan Cope, Bryan Kline, Jason Daly
//
//------------------------------------------------------------------------

#include "vsSkeletonMeshGeometry.h++"
#include "vsOSGNode.h++"

// ------------------------------------------------------------------------
// Default Constructor - Creates an OSG geode and geometry and connects
// them together, sets up empty geometry lists and configures for dynamic
// operation (no display lists)
// ------------------------------------------------------------------------
vsSkeletonMeshGeometry::vsSkeletonMeshGeometry() 
                      : vsGeometryBase()
{
    // Vertex array, a copy to keep in its original form unmodified by the
    // skeleton.
    originalVertexList = new osg::Vec3Array();
    originalVertexList->ref();
                                                                                
    // Normal array, a copy to keep in its original form unmodified by the
    // skeleton.
    originalNormalList = new osg::Vec3Array();
    originalNormalList->ref();

    // Since this geometry is dynamic (i.e.: it will change every frame),
    // disable display listing of the geometry data, and set its data
    // variance to dynamic
    osgGeometry->setUseDisplayList(false);
    osgGeometry->setDataVariance(osg::Object::DYNAMIC);

    // Register this node and osg::Geode in the node map
    getMap()->registerLink(this, new vsOSGNode(osgGeode));
}

// ------------------------------------------------------------------------
// Destructor - Disconnects this node from its OSG counterpart
// ------------------------------------------------------------------------
vsSkeletonMeshGeometry::~vsSkeletonMeshGeometry()
{
    vsObject *nodeRefObj;

    // Remove all parents
    detachFromParents();

    // Remove all attributes
    deleteAttributes();

    // Destroy the "original" vertex/normal lists
    originalVertexList->unref();
    originalNormalList->unref();

    // Unregister this node and get rid of its vsOSGNode wrapper
    nodeRefObj = getMap()->removeLink(this, VS_OBJMAP_FIRST_LIST);
    delete nodeRefObj;
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

            // Also set the regular normal list binding
            osgGeometry->setNormalBinding(osgBinding);
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
            osgGeometry->setVertexAttribBinding(whichData, osgBinding);
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
                                     atVector data)
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
            "the given type (%d) requires at least %d values)\n", whichData,
            dataSize);
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
atVector vsSkeletonMeshGeometry::getData(int whichData, int dataIndex)
{
    atVector result;
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
void vsSkeletonMeshGeometry::setDataList(int whichData, atVector *dataBuffer)
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
void vsSkeletonMeshGeometry::getDataList(int whichData, atVector *dataBuffer)
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
// Deindexes the geometry by expanding all active data lists to match
// what is currently represented by the index list.  Since the index list
// is being used, this method assumes all active lists are in PER_VERTEX
// or OVERALL mode.  This version calls the base class deindex method and
// uses the results to manipulate the "original" vertex and normal lists in
// the same way
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::deindexGeometry()
{
    osg::Vec3Array *vertexList;
    osg::Vec3Array *normalList;

    // Cast the vertex and normal arrays to the proper type
    vertexList = 
        dynamic_cast<osg::Vec3Array *>(dataList[VS_GEOMETRY_VERTEX_COORDS]);
    normalList = 
        dynamic_cast<osg::Vec3Array *>(dataList[VS_GEOMETRY_NORMALS]);

    // Copy the original vertices and normals to the corresponding regular
    // data lists.  This will let us use the base class optimize method
    // to also optimize the original vertices and normals.
    if (vertexList != NULL)
    {
        vertexList->
            assign(originalVertexList->begin(), originalVertexList->end());
    }
    if (normalList != NULL)
    {
        normalList->assign(
            originalNormalList->begin(), originalNormalList->end());
    }

    // Call the base class deindex method
    vsGeometryBase::deindexGeometry();

    // Copy the optimized vertices and normals back to the original
    // vertex and normal lists
    if (vertexList != NULL)
        originalVertexList->assign(vertexList->begin(), vertexList->end());
    if (normalList != NULL)
        originalNormalList->assign(normalList->begin(), normalList->end());
}

// ------------------------------------------------------------------------
// Optimizes the vertex data lists by searching for duplicate vertices
// (i.e.: vertices that have the same data in all lists that are in use),
// and re-indexing them so that all duplicates are indexed to a single
// instance of that vertex.  This version calls the base class version
// and uses the results to manipulate the "original" vertex and normal
// lists in the same way
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::optimizeVertices()
{
    osg::Vec3Array *vertexList;
    osg::Vec3Array *normalList;

    // Cast the vertex and normal arrays to the proper type
    vertexList = 
        dynamic_cast<osg::Vec3Array *>(dataList[VS_GEOMETRY_VERTEX_COORDS]);
    normalList = 
        dynamic_cast<osg::Vec3Array *>(dataList[VS_GEOMETRY_NORMALS]);

    // Copy the original vertices and normals to the corresponding regular
    // data lists.  This will let us use the base class optimize method
    // to also optimize the original vertices and normals.
    if (vertexList != NULL)
    {
        vertexList->
            assign(originalVertexList->begin(), originalVertexList->end());
    }
    if (normalList != NULL)
    {
        normalList->assign(
            originalNormalList->begin(), originalNormalList->end());
    }

    // Call the base class optimize method
    vsGeometryBase::optimizeVertices();

    // Copy the optimized vertices and normals back to the original
    // vertex and normal lists
    if (vertexList != NULL)
        originalVertexList->assign(vertexList->begin(), vertexList->end());
    if (normalList != NULL)
        originalNormalList->assign(normalList->begin(), normalList->end());
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
void vsSkeletonMeshGeometry::applySkin(atArray *boneMatrices,
                                       atArray *ITBoneMatrices)
{
    int vertexIndex;
    int dataIndex;
    double weight;
    int bone;
    atVector vertex;
    atVector normal;
    atMatrix *boneMatrix, *itBoneMatrix;
    atMatrix finalVertexMatrix;
    atMatrix finalNormalMatrix;
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
                boneMatrix = ((atMatrix *)boneMatrices->getEntry(bone));
                itBoneMatrix = ((atMatrix *)ITBoneMatrices->getEntry(bone));

                // If the weight is zero, or the matrices are NULL, don't
                // bother multiplying.
                if ((boneMatrix != NULL) && (itBoneMatrix != NULL) &&
                    (weight != 0.0))
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

        // Tell OSG it has new vertex and normal data
        notifyOSGDataChanged(VS_GEOMETRY_VERTEX_COORDS);
        notifyOSGDataChanged(VS_GEOMETRY_NORMALS);
    }
    else
    {
        printf("vsSkeletonMeshGeometry::applySkin:  List size mismatch!\n");
        printf("    vertices = %d\n", vertexListSize);
        printf("    normals  = %d\n", normalListSize);
        printf("    weights  = %d\n", weightListSize);
        printf("    bone idx = %d\n", boneListSize);
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

    // Tell OSG it has new vertex and normal data
    notifyOSGDataChanged(VS_GEOMETRY_VERTEX_COORDS);
    notifyOSGDataChanged(VS_GEOMETRY_NORMALS);
}

