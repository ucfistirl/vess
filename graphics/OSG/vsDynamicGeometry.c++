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
//    Description:  vsGeometryBase subclass that handles dynamic geometry.
//                  Under Open Scene Graph, the only real difference
//                  between this class and vsGeometry is that this class
//                  disables the use of display lists.
//
//    Author(s):    Bryan Kline, Jason Daly, Duvan Cope
//
//------------------------------------------------------------------------

#include "vsDynamicGeometry.h++"

// ------------------------------------------------------------------------
// Default Constructor - Creates an OSG geode and geometry and connects
// them together, sets up empty geometry lists, and configures for dynamic
// operation (no display lists)
// ------------------------------------------------------------------------
vsDynamicGeometry::vsDynamicGeometry() 
                 : vsGeometryBase()
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

    // Create the various data arrays
    for (loop = 0; loop < VS_GEOMETRY_LIST_COUNT; loop++)
        allocateDataArray(loop);

    // Initialize other values
    indexList = NULL;
    indexListSize = 0;
    lengthsList = NULL;
    primitiveCount = 0;
    primitiveType = VS_GEOMETRY_TYPE_POINTS;

    // Since this geometry is dynamic (i.e.: it will change every frame),
    // disable display listing of the geometry data, and set its data
    // variance to dynamic
    osgGeometry->setUseDisplayList(false);
    osgGeometry->setDataVariance(osg::Object::DYNAMIC);

    // Enable lighting on this Geometry and set the render bin to default
    enableLighting();
    renderBin = -1;

    // Register this node and osg::Geode in the node map
    getMap()->registerLink(this, osgGeode);
}

// ------------------------------------------------------------------------
// Destructor - Disconnects this node from its OSG counterpart and destroys
// both this node and the underlying OSG nodes
// ------------------------------------------------------------------------
vsDynamicGeometry::~vsDynamicGeometry()
{
    int loop;

    // Remove all parents
    detachFromParents();

    // Remove all attributes
    deleteAttributes();

    // If we're using vertex indices, unreference the index list now
    if (indexList)
        free(indexList);

    // Destroy the data lists
    for (loop = 0; loop < VS_GEOMETRY_LIST_COUNT; loop++)
        dataList[loop]->unref();

    // Remove the link to the osg node from the object map
    getMap()->removeLink(this, VS_OBJMAP_FIRST_LIST);

    // Unlink and destroy the OSG objects
    osgGeometry->unref();
    osgGeode->unref();

    // If we've created a primitive lengths list, free this now
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
// Begins a new state/frame of the dynamic geometry
// ------------------------------------------------------------------------
void vsDynamicGeometry::beginNewState()
{
    // Initialize the "data chaged" flags to false
    memset(dataChanged, 0, sizeof(dataChanged));

    // Initialize another flag that indicates if there were any changes
    // to the geometry's primitive structure
    primitivesChanged = false;
}
                                                                                
// ------------------------------------------------------------------------
// Finalizes the new dynamic geometry state
// ------------------------------------------------------------------------
void vsDynamicGeometry::finishNewState()
{
    int i;

    // Look at each data list
    for (i = 0; i < VS_GEOMETRY_LIST_COUNT; i++)
    {
        // See if this list has changed
        if (dataChanged[i])
        {
            // Notify OSG of the change
            notifyOSGDataChanged(i);
        }
    }

    // Rebuild the primitive sets if necessary
    if (primitivesChanged)
        rebuildPrimitives();
}

// ------------------------------------------------------------------------
// Sets the type of geometric primitive that this object contains
// ------------------------------------------------------------------------
void vsDynamicGeometry::setPrimitiveType(int newType)
{
    // Make sure the type argument is a valid primitive type
    if ((newType < VS_GEOMETRY_TYPE_POINTS) ||
        (newType > VS_GEOMETRY_TYPE_POLYS))
    {
        printf("vsDynamicGeometry::setPrimitiveType: Unrecognized primitive "
            "type\n");
        return;
    }

    // Set the primitive type
    primitiveType = newType;
    
    // Indicate that the primitives need rebuilding
    primitivesChanged = true;
}

// ------------------------------------------------------------------------
// Sets the number of geometric primitive that this object contains. Must
// be called before any calls to set the data for any specific primitive.
// ------------------------------------------------------------------------
void vsDynamicGeometry::setPrimitiveCount(int newCount)
{
    int loop;

    // Sanity check, primarily to avoid memory corruption
    if ((newCount < 0) || (newCount > VS_GEOMETRY_MAX_LIST_INDEX))
    {
        printf("vsDynamicGeometry::setPrimitiveCount: Invalid count value "
            "'%d'\n",
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
    
    // Indicate that the primitives need rebuilding
    primitivesChanged = true;
}

// ------------------------------------------------------------------------
// Sets the number of verticies for the primitive with the specified index
// within the object. The index of the first primitive is 0.
// ------------------------------------------------------------------------
void vsDynamicGeometry::setPrimitiveLength(int index, int length)
{
    // Make sure the index is valid, given the current primitive count
    if ((index < 0) || (index >= getPrimitiveCount()))
    {
        printf("vsDynamicGeometry::setPrimitiveLength: Index out of bounds\n");
        return;
    }
    
    // Set the new length in the primitive lengths list
    lengthsList[index] = length;

    // Indicate that the primitives need rebuilding
    primitivesChanged = true;
}

// ------------------------------------------------------------------------
// Sets the number of verticies for all of the primitives within the object
// at once. The number of entries in the lengths array must be equal to or
// greater than the number of primitives in the object.
// ------------------------------------------------------------------------
void vsDynamicGeometry::setPrimitiveLengths(int *lengths)
{
    int loop;
    
    // Copy the given integer array into the primitive lengths array
    for (loop = 0; loop < getPrimitiveCount(); loop++)
        lengthsList[loop] = lengths[loop];
    
    // Indicate that the primitives need rebuilding
    primitivesChanged = true;
}

// ------------------------------------------------------------------------
// Sets one data point within the geometry objects' lists of data. The
// whichData value specifies which type of data is to be affected, and
// the index specifies which data point is to be altered. The index of
// the first data point is 0.
// ------------------------------------------------------------------------
void vsDynamicGeometry::setData(int whichData, int dataIndex, atVector data)
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
        printf("vsDynamicGeometry::setData: Unrecognized data type\n");
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
        printf("vsDynamicGeometry::setData: Index out of bounds\n");
        return;
    }

    // Make sure that the input vector has enough data
    if (data.getSize() < dataSize)
    {
        printf("vsDynamicGeometry::setData: Insufficient data (data of the "
            "given type requires at least %d values)\n", dataSize);
        return;
    }

    // If a conventional attribute is specified, then make sure we're not
    // already using the generic attribute, and vice versa.
    if (whichData < VS_GEOMETRY_LIST_COUNT)
    {
        // Conventional data specified
        if (dataIsGeneric[slotNum])
        {
            printf("vsDynamicGeometry::setData: Cannot use conventional data "
                "type when corresponding generic attribute is in use\n");
            return;
        }
    }
    else
    {
        // Generic attribute specified
        if (!(dataIsGeneric[slotNum]))
        {
            printf("vsDynamicGeometry::setData: Cannot use generic attribute "
                "type when corresponding conventional data is in use\n");
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

    // Indicate that a data list has changed
    dataChanged[whichData] = true;
}

// ------------------------------------------------------------------------
// Sets all of the data points within one of the geometry objects' lists
// to the values in dataList. The dataList array must be at least as large
// as the size of particular list in question.
// ------------------------------------------------------------------------
void vsDynamicGeometry::setDataList(int whichData, atVector *dataBuffer)
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
        printf("vsDynamicGeometry::setDataList: Unrecognized data type\n");
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
            printf("vsDynamicGeometry::setDataList: Cannot use conventional "
                "data type when corresponding generic attribute is in use\n");
            return;
        }
    }
    else
    {
        // Generic attribute specified
        if (!(dataIsGeneric[slotNum]))
        {
            printf("vsDynamicGeometry::setDataList: Cannot use generic "
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

    // Mark the appropriate data list as changed
    dataChanged[whichData] = true;
}

// ------------------------------------------------------------------------
// Sets the size of one of the object's data lists. Generally the data list
// sizes must be set on a new geometry object before data can be put into
// it.
// ------------------------------------------------------------------------
void vsDynamicGeometry::setDataListSize(int whichData, int newSize)
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
        printf("vsDynamicGeometry::setDataListSize: Unrecognized data type\n");
        return;
    }

    // Sanity check, primarily to avoid memory corruption
    if ((newSize < 0) || (newSize > VS_GEOMETRY_MAX_LIST_INDEX))
    {
        printf("vsDynamicGeometry::setDataListSize: Invalid list size '%d'\n",
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
            printf("vsDynamicGeometry::setDataListSize: Cannot use "
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
            printf("vsDynamicGeometry::setDataListSize: Cannot use generic "
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

    // Mark the appropriate data list as changed
    dataChanged[whichData] = true;

    // If we're dealing with vertex coordinates, then we have to reconstruct
    // OSG's primitive set as well. (We do this with generic attribute #0 as
    // well because generic 0 is always considered to contain vertex
    // coordinates.)
    if ((whichData == VS_GEOMETRY_VERTEX_COORDS) ||
        (whichData == VS_GEOMETRY_GENERIC_0))
        primitivesChanged = true;
}

// ------------------------------------------------------------------------
// Sets one of the indexes in the geometry's index list
// ------------------------------------------------------------------------
void vsDynamicGeometry::setIndex(int indexIndex, u_int newIndex)
{
    // Make sure the index's index in the list is valid
    if ((indexIndex < 0) || (indexIndex > indexListSize))
    {
        printf("vsDynamicGeometry::setIndex: Index is out of range\n");
        return;
    }

    // Set the index
    indexList[indexIndex] = newIndex; 
    
    // A change to the index list means the primitives need rebuilding
    primitivesChanged = true;
}   

// ------------------------------------------------------------------------
// Sets all of the indexes in the geometry's index list (the list provided
// must contain enough indices to fill the current index list)
// ------------------------------------------------------------------------
void vsDynamicGeometry::setIndexList(u_int *indexBuffer)
{
    int i;

    // Don't try to set it if it isn't there
    if (indexList == NULL)
    {
        printf("vsDynamicGeometry::setIndexList: Tried to set NULL index "
            "list!\n");
        return;
    }

    // Set all indices
    memcpy(indexList, indexBuffer, indexListSize * sizeof(u_int));

    // A change to the index list means the primitives need rebuilding
    primitivesChanged = true;
}

// ------------------------------------------------------------------------
// Sets the size of the list for vertex indices.  When using vertex
// indices, the vertices specified by the index list are used to render the
// primitives, instead of pulling vertices from the data lists directly.
// ------------------------------------------------------------------------
void vsDynamicGeometry::setIndexListSize(int newSize)
{
    // See if the size is valid
    if ((newSize < 0) || (newSize > VS_GEOMETRY_MAX_LIST_INDEX))
    {
        printf("vsDynamicGeometry::vsDynamicGeometry:  Index list size is "
            "invalid.\n");
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
    primitivesChanged = true;
}

