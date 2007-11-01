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
//    Author(s):    Bryan Kline, Duvan Cope
//
//------------------------------------------------------------------------

#include "vsGeometry.h++"

#include <Performer/pf/pfSCS.h>
#include <Performer/pr/pfLight.h>
#include "vsComponent.h++"
#include "vsShadingAttribute.h++"
#include "vsGraphicsState.h++"

vsTreeMap *vsGeometry::binModeList = NULL;
bool vsGeometry::binModesChanged = false;

// ------------------------------------------------------------------------
// Default Constructor - Creates a Performer geode and geoset and connects
// them together, sets up empty geometry lists, and registers callbacks.
// ------------------------------------------------------------------------
vsGeometry::vsGeometry() : parentList(5, 5)
{
    unsigned int list;
    int loop;

    // Start with no parents
    parentCount = 0;

    // Create a Performer geometry node to hold the geometry
    performerGeode = new pfGeode();
    performerGeode->ref();

    // Create a pfGeoArray and add it to the geode
    performerGeoarray = new pfGeoArray();
    performerGeoarray->ref();
    performerGeode->addGSet(performerGeoarray);
    
    // Create a pfGeoState
    performerGeostate = new pfGeoState();
    performerGeostate->ref();
    performerGeoarray->setGState(performerGeostate);

    // Initialize the attribute lists to NULL and size 0
    for (list = 0; list < VS_GEOMETRY_LIST_COUNT; list++)
    {
        dataAttr[list] = NULL;
        dataList[list] = NULL;
        dataListSize[list] = 0;
        dataIsGeneric[list] = false;
        dataBinding[list] = VS_GEOMETRY_BIND_NONE;
    }
    lengthsList = NULL;

    // Initialize the "public" normal and color list sizes and bindings
    // Because pfGeoArrays are always per-vertex, we have to emulate the
    // overall and per-primitive modes of these data lists
    normalList = NULL;
    normalBinding = VS_GEOMETRY_BIND_NONE;
    normalListSize = 0;
    colorList = NULL;
    colorBinding = VS_GEOMETRY_BIND_NONE;
    colorListSize = 0;

    // Initialize the number of primitives and the type of the primitive
    setPrimitiveCount(0);
    setPrimitiveType(VS_GEOMETRY_TYPE_POINTS);
    
    // Take care of lights and other graphics state initialization
    lightsList = (pfLight **)
        (pfMemory::malloc(sizeof(pfLight *) * PF_MAX_LIGHTS));
    for (loop = 0; loop < PF_MAX_LIGHTS; loop++)
        lightsList[loop] = NULL;

    // Set up a pre-callback for the Performer GeoState.  This allows
    // VESS to track state changes and set node attributes appropriately.
    performerGeostate->setFuncs(geostateCallback, NULL, lightsList);
    
    // Make sure the "force flat shading" draw mode is off since we don't
    // want all geometry to be drawn flat shaded.
    performerGeoarray->setDrawMode(PFGS_FLATSHADE, PF_OFF);

    // Enable lighting (by default)
    enableLighting();

    // Initialize the default render bin
    renderBin = performerGeoarray->getDrawBin();

    // Register the pfGeode with the vsObjectMap
    getMap()->registerLink(this, performerGeode);
}

// ------------------------------------------------------------------------
// Destructor - Disconnects this node from its Performer counterpart
// ------------------------------------------------------------------------
vsGeometry::~vsGeometry()
{
    unsigned int list;

    // Remove all parents
    detachFromParents();

    // Remove all attributes
    deleteAttributes();

    // Unlink and destroy the Performer objects
    performerGeode->unref();
    pfDelete(performerGeode);
    performerGeoarray->unref();
    pfDelete(performerGeoarray);
    performerGeostate->unref();
    pfDelete(performerGeostate);

    // Delete the data lists
    for (list = 0; list < VS_MAXIMUM_TEXTURE_UNITS; list++)
        if (dataList[list] && !(pfMemory::getRef(dataList[list])))
            pfMemory::free(dataList[list]);
    if (lengthsList && !(pfMemory::getRef(lengthsList)))
        pfMemory::free(lengthsList);
    if (normalList)
        free(normalList);
    if (colorList)
        free(colorList);

    getMap()->removeLink(this, VS_OBJMAP_FIRST_LIST);
}

// ------------------------------------------------------------------------
// Private function
// Copies the internal lists for colors and normals to the actual list used
// by the pfGeoArray. Along the way, we convert the list from per-primitive
// or overall binding to per-vertex, if necessary.
// ------------------------------------------------------------------------
void vsGeometry::convertToPerVertex(int list)
{
    float *fakeList, *tempList;
    int fakeListSize, fakeBinding;
    int elementSize, copySize;
    int primitiveCount, primitiveType;
    int i, j, k;
    int baseIndex, realIndex, fakeIndex;
    int realListType;
    int newSize;

    // See which list we're converting
    if (list == VS_GEOMETRY_NORMALS)
    {
        fakeList = normalList;
        fakeListSize = normalListSize;
        fakeBinding = normalBinding;
        elementSize = 3;
        realListType = PFGA_NORMAL_ARRAY;
    }
    else if (list == VS_GEOMETRY_COLORS)
    {
        fakeList = colorList;
        fakeListSize = colorListSize;
        fakeBinding = colorBinding;
        elementSize = 4;
        realListType = PFGA_COLOR_ARRAY;
    }
    else
    {
        // None of the other lists accept bindings other than NONE or
        // PER_VERTEX, so bail out here
        return;
    }

    // Check the list of vertex coordinates and make sure we have a
    // vertex list to mirror
    if (dataListSize[VS_GEOMETRY_VERTEX_COORDS] == 0)
    {
        // We have no vertices.  If the actual list still exists, clean it up
        // now.
        if (dataListSize[list] > 0)
        {
            // Unbind the list
            dataBinding[list] = VS_GEOMETRY_BIND_NONE;

            // Remove the list's attribute from the geoarray
            performerGeoarray->removeAttr(dataAttr[list]);
            dataAttr[list] = NULL;
             
            // Free up the list
            pfMemory::unrefDelete(dataList[list]);
            dataList[list] = NULL;
        }
    }

    // If the list to be converted doesn't exist, bail out
    if (fakeList == NULL)
        return;

    // Figure out how big to make the real list
    newSize = dataListSize[VS_GEOMETRY_VERTEX_COORDS];

    // Resize the list to match the number of vertices in the geometry.
    // Determine what we need to do with the data list
    // based on whether or not it currently exists, and
    // the desired new size of the list
    if (newSize && !dataList[list])
    {
        // No list exists, create new list
        dataList[list] = (float *)(pfMemory::malloc(
            sizeof(float) * elementSize * newSize));
        pfMemory::ref(dataList[list]);
    }
    else if (!newSize && dataList[list])
    {
        // List exists, but the requested new size is zero, so
        // delete the existing list
        if (dataAttr[list] != NULL)
        {
            performerGeoarray->removeAttr(dataAttr[list]);
            dataAttr[list] = NULL;
            dataBinding[list] = VS_GEOMETRY_BIND_NONE;
        }

        // Now, delete the list
        pfMemory::unrefDelete(dataList[list]);
        dataList[list] = NULL;
    }
    else if ((newSize && dataList[list]) &&
             (newSize != dataListSize[list]))
    {
        // Modify the length of the existing list
        tempList = dataList[list];
        dataList[list] = (float *)(pfMemory::malloc(sizeof(float) * 
            elementSize * newSize));
        pfMemory::ref(dataList[list]);

        // Figure out how much data to copy from the old list
        if (newSize < dataListSize[list])
            copySize = newSize * elementSize * sizeof(float);
        else
            copySize = dataListSize[list] * elementSize * sizeof(float);

        // Copy the old list data to the new list
        memcpy(dataList[list], tempList, copySize);

        // Update the pfGeoArray data
        dataAttr[list]->setPtr(dataList[list]);
        performerGeoarray->updateData();

        // Free the old list
        pfMemory::unrefDelete(tempList);
    }

    // Update the data list size
    dataListSize[list] = newSize;

    // Process the list to convert the old binding to per-vertex
    if (fakeBinding == VS_GEOMETRY_BIND_OVERALL)
    {
        // Copy the data from the first element of the internal "fake" list
        // to all elements of the real list
        for (i = 0; i < dataListSize[list]; i++)
            for (j = 0; j < elementSize; j++)
                dataList[list][i*elementSize + j] = fakeList[j];

        // Switch the actual binding of the list to per-vertex and attach
        // the list to the geometry if necessary
        if (dataAttr[list] == NULL)
            dataAttr[list] = performerGeoarray->setAttr(realListType, 
                elementSize, GL_FLOAT, 0, dataList[list]);
        performerGeoarray->enableAttr(dataAttr[list]);
        dataBinding[list] = VS_GEOMETRY_BIND_PER_VERTEX;
    }
    else if (fakeBinding == VS_GEOMETRY_BIND_PER_PRIMITIVE)
    {
        // Get the primitive type and count
        primitiveType = getPrimitiveType();
        primitiveCount = getPrimitiveCount();

        // Copy the data from the first element to all other elements
        // of the list.  The process will be different depending on the
        // primitive type.
        switch (primitiveType)
        {
            case VS_GEOMETRY_TYPE_POINTS:
                // A straight list-to-list copy works for points
                memcpy(dataList[list], fakeList,
                    fakeListSize * elementSize * sizeof(float));
                break;

            case VS_GEOMETRY_TYPE_LINES:
                // Copy the lines' attributes (two per primitive)
                for (i = 0; i < primitiveCount; i++)
                    for (j = 0; j < 2; j++)
                        for (k = 0; k < elementSize; k++)
                        {
                            // Compute the list indices, each primitive has
                            // two vertices with elementSize components each
                            realIndex = i*2*elementSize + j*elementSize + k;
                            fakeIndex = i*elementSize + k;
                            dataList[list][realIndex] = fakeList[fakeIndex];
                        }
                break;

            case VS_GEOMETRY_TYPE_TRIS:
                // Copy the triangles' attributes (three per primitive)
                for (i = 0; i < primitiveCount; i++)
                    for (j = 0; j < 3; j++)
                        for (k = 0; k < elementSize; k++)
                        {
                            // Compute the list indices, each primitive has
                            // three vertices with elementSize components each
                            realIndex = i*3*elementSize + j*elementSize + k;
                            fakeIndex = i*elementSize + k;
                            dataList[list][realIndex] = fakeList[fakeIndex];
                        }
                break;

            case VS_GEOMETRY_TYPE_QUADS:
                // Copy the quads' attributes (four per primitive)
                for (i = 0; i < primitiveCount; i++)
                    for (j = 0; j < 4; j++)
                        for (k = 0; k < elementSize; k++)
                        {
                            // Compute the list indices, each primitive has
                            // four vertices with elementSize components each
                            realIndex = i*4*elementSize + j*elementSize + k;
                            fakeIndex = i*elementSize + k;
                            dataList[list][realIndex] = fakeList[fakeIndex];
                        }
                break;

            case VS_GEOMETRY_TYPE_LINE_STRIPS:
            case VS_GEOMETRY_TYPE_TRI_STRIPS:
            case VS_GEOMETRY_TYPE_TRI_FANS:
            case VS_GEOMETRY_TYPE_POLYS:
                // Copy the strips' attributes, using the lengths array
                // to determine how many vertices per primitive.  Use
                // a baseIndex variable to keep track of where in the list
                // each primitive starts
                baseIndex = 0;
                for (i = 0; i < primitiveCount; i++)
                {
                    for (j = 0; j < lengthsList[i]; j++)
                    {
                        for (k = 0; k < elementSize; k++)
                        {
                            // Compute the list indices
                            realIndex =
                                baseIndex + j*elementSize + k;
                            fakeIndex = i*elementSize + k;
                            dataList[list][realIndex] = fakeList[fakeIndex];
                        }
                    }

                    // Update the baseIndex so we know where the next
                    // primitive starts
                    baseIndex += lengthsList[i]*elementSize;
                }
                break;
        }

        // Switch the actual binding of the list to per-vertex and attach
        // the list to the geometry if necessary
        if (dataAttr[list] == NULL)
            dataAttr[list] = performerGeoarray->setAttr(realListType, 
                elementSize, GL_FLOAT, 0, dataList[list]);
        performerGeoarray->enableAttr(dataAttr[list]);
        dataBinding[list] = VS_GEOMETRY_BIND_PER_VERTEX;
    }
    else if (fakeBinding == VS_GEOMETRY_BIND_PER_VERTEX)
    {
        // If the binding is already per-vertex, we just need to copy
        // the list data over
        pfMemory::copy(dataList[list], fakeList);

        // Set the actual binding of the list to per-vertex and attach
        // the list to the geometry if necessary
        if (dataAttr[list] == NULL)
            dataAttr[list] = performerGeoarray->setAttr(realListType, 
                elementSize, GL_FLOAT, 0, dataList[list]);
        performerGeoarray->enableAttr(dataAttr[list]);
        dataBinding[list] = VS_GEOMETRY_BIND_PER_VERTEX;
    }
    else if (fakeBinding == VS_GEOMETRY_BIND_NONE)
    {
        // Set the actual binding of the list to none and disable
        // the list on the geometry if necessary
        if (dataAttr[list] != NULL)
        {
            dataBinding[list] = VS_GEOMETRY_BIND_NONE;
            performerGeoarray->disableAttr(dataAttr[list]);
        }
    }
}

// ------------------------------------------------------------------------
// Private function
// Emulates setting an overall-bound data element on the normal or color
// list
// ------------------------------------------------------------------------
void vsGeometry::setOverallData(int list, atVector data)
{
    int i, j;

    // Figure out which list we're manipulating. Note that we assume all
    // error checking on ranges, list sizes, and bounds has already been
    // done by the calling function
    if (list == VS_GEOMETRY_NORMALS)
    {
        // Copy the data to every element in the list
        for (i = 0; i < dataListSize[list]; i++)
        {
            for (j = 0; j < 3; j++)
                dataList[list][i*3 + j] = data[j];
        }
    }
    else if (list == VS_GEOMETRY_COLORS)
    {
        // Copy the data to every element in the list
        for (i = 0; i < dataListSize[list]; i++)
        {
            for (j = 0; j < 4; j++)
                dataList[list][i*4 + j] = data[j];
        }
    }

    // Update the pfGeoArray data
    performerGeoarray->updateData();
}

// ------------------------------------------------------------------------
// Private function
// Emulates setting a per-primitive-bound data element on the normal or
// color list
// ------------------------------------------------------------------------
void vsGeometry::setPerPrimitiveData(int list, int index, atVector data)
{
    int baseIndex, i, j;
    int elementSize;

    // Figure out which list we're manipulating and determine the size of
    // each element of the list.
    if (list == VS_GEOMETRY_NORMALS)
        elementSize = 3;
    else if (list == VS_GEOMETRY_COLORS)
        elementSize = 4;
    else
        return;

    // Adjust the list data.  Note that we assume all error checking on
    // ranges, list sizes, and bounds has already been done by the calling
    // function
    switch (getPrimitiveType())
    {
        case VS_GEOMETRY_TYPE_POINTS:
            // Only one element to copy
            for (i = 0 ; i < elementSize; i++)
                dataList[list][index*elementSize + i] = data[i];
            break;

        case VS_GEOMETRY_TYPE_LINES:
            // Two elements to copy
            for (i = 0 ; i < 2; i++)
                for (j = 0; j < elementSize; j++)
                    dataList[list][index*elementSize*2 + i*elementSize + j] =
                        data[j];
            break;

        case VS_GEOMETRY_TYPE_TRIS:
            // Three elements to copy
            for (i = 0 ; i < 3; i++)
                for (j = 0; j < elementSize; j++)
                    dataList[list][index*elementSize*3 + i*elementSize + j] =
                        data[j];
            break;

        case VS_GEOMETRY_TYPE_QUADS:
            // Four elements to copy
            for (i = 0 ; i < 4; i++)
                for (j = 0; j < elementSize; j++)
                    dataList[list][index*elementSize*4 + i*elementSize + j] =
                        data[j];
            break;

        case VS_GEOMETRY_TYPE_LINE_STRIPS:
        case VS_GEOMETRY_TYPE_TRI_STRIPS:
        case VS_GEOMETRY_TYPE_TRI_FANS:
        case VS_GEOMETRY_TYPE_POLYS:
            // Compute the base index where we will start modifying data in the
            // list
            baseIndex = 0;
            for (i = 0; i < index; i++)
                baseIndex += lengthsList[i]*elementSize;

            // Modify the data for all vertices related to this primitive
            for (i = 0; i < lengthsList[index]; i++)
                for (j = 0; j < elementSize; j++)
                    dataList[list][baseIndex + i*elementSize + j] = data[j];
            break;

        default:
            printf("vsGeometry::setPerPrimitiveData:  Unrecognized primitive "
                "type\n");
    }

    // Update the pfGeoArray data
    performerGeoarray->updateData();
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
    if ((index < 0) || (index >= parentCount))
    {
        printf("vsGeometry::getParent: Bad parent index\n");
        return NULL;
    }

    return (vsNode *)(parentList[index]);
}

// ------------------------------------------------------------------------
// Sets the type of geometric primitive that this object contains
// ------------------------------------------------------------------------
void vsGeometry::setPrimitiveType(int newType)
{
    // Translate the primitive type constant
    switch (newType)
    {
        case VS_GEOMETRY_TYPE_POINTS:
            performerGeoarray->setPrimType(PFGS_POINTS);
            break;
        case VS_GEOMETRY_TYPE_LINES:
            performerGeoarray->setPrimType(PFGS_LINES);
            break;
        case VS_GEOMETRY_TYPE_LINE_STRIPS:
            performerGeoarray->setPrimType(PFGS_LINESTRIPS);
            break;
        case VS_GEOMETRY_TYPE_LINE_LOOPS:
            printf("vsGeometry::setPrimitiveType: VS_GEOMETRY_TYPE_LINE_LOOPS "
                "type not supported under Performer operation\n");
            performerGeoarray->setPrimType(PFGS_LINESTRIPS);
            break;
        case VS_GEOMETRY_TYPE_TRIS:
            performerGeoarray->setPrimType(PFGS_TRIS);
            break;
        case VS_GEOMETRY_TYPE_TRI_STRIPS:
            performerGeoarray->setPrimType(PFGS_TRISTRIPS);
            break;
        case VS_GEOMETRY_TYPE_TRI_FANS:
            performerGeoarray->setPrimType(PFGS_TRIFANS);
            break;
        case VS_GEOMETRY_TYPE_QUADS:
            performerGeoarray->setPrimType(PFGS_QUADS);
            break;
        case VS_GEOMETRY_TYPE_QUAD_STRIPS:
            printf("vsGeometry::setPrimitiveType: VS_GEOMETRY_TYPE_QUAD_STRIPS "
                "type not supported under Performer operation\n");
            performerGeoarray->setPrimType(PFGS_QUADS);
            break;
        case VS_GEOMETRY_TYPE_POLYS:
            performerGeoarray->setPrimType(PFGS_POLYS);
            break;
        default:
            printf("vsGeometry::setPrimitiveType: Unrecognized primitive "
                "type\n");
            break;
    }
}

// ------------------------------------------------------------------------
// Retrieves the type of geometric primitive that this object contains
// ------------------------------------------------------------------------
int vsGeometry::getPrimitiveType()
{
    // Obtain the Performer primitive type from the geoset, and translate
    // it to VESS
    switch (performerGeoarray->getPrimType())
    {
        case PFGS_POINTS:
            return VS_GEOMETRY_TYPE_POINTS;
        case PFGS_LINES:
            return VS_GEOMETRY_TYPE_LINES;
        case PFGS_LINESTRIPS:
        case PFGS_FLAT_LINESTRIPS:
            return VS_GEOMETRY_TYPE_LINE_STRIPS;
        case PFGS_TRIS:
            return VS_GEOMETRY_TYPE_TRIS;
        case PFGS_TRISTRIPS:
        case PFGS_FLAT_TRISTRIPS:
            return VS_GEOMETRY_TYPE_TRI_STRIPS;
        case PFGS_TRIFANS:
        case PFGS_FLAT_TRIFANS:
            return VS_GEOMETRY_TYPE_TRI_FANS;
        case PFGS_QUADS:
            return VS_GEOMETRY_TYPE_QUADS;
        case PFGS_POLYS:
            return VS_GEOMETRY_TYPE_POLYS;
    }
    
    // If the primitive type is unrecognized, return an error value
    return -1;
}

// ------------------------------------------------------------------------
// Sets the number of geometric primitive that this object contains. Must
// be called before any calls to set the data for any specific primitive.
// ------------------------------------------------------------------------
void vsGeometry::setPrimitiveCount(int newCount)
{
    // Set the number of primitives on the Performer geoset
    performerGeoarray->setNumPrims(newCount);
    
    // If the geometry's particular primitive type doesn't require a
    // primitive lengths array, don't bother updating it.
    if ((getPrimitiveType() == VS_GEOMETRY_TYPE_POINTS) ||
        (getPrimitiveType() == VS_GEOMETRY_TYPE_LINES) ||
        (getPrimitiveType() == VS_GEOMETRY_TYPE_TRIS) ||
        (getPrimitiveType() == VS_GEOMETRY_TYPE_QUADS))
        return;
    
    // Change the length of the primitive lengths array
    if (newCount && !lengthsList)
    {
        // No lengths array exists, but there are primitives to draw.
        // Create a new lengths array.
        lengthsList = (int *)(pfMemory::malloc(sizeof(int) * newCount));
    }
    else if (!newCount && lengthsList)
    {
        // Delete the existing lengths array.  It is no longer needed since
        // there are now no primitives to draw.
        pfMemory::free(lengthsList);
        lengthsList = NULL;
    }
    else
    {
        // Lengths array exists and there are primitives to draw.
        // Modify the current lengths array to match the number of
        // primitives just set.
        lengthsList = (int *)(pfMemory::realloc(lengthsList,
            sizeof(int) * newCount));
    }
    
    // Update the lengths array on the pfGeoArray
    performerGeoarray->setPrimLengths(lengthsList);
}

// ------------------------------------------------------------------------
// Retrieves the number of geometric primitives that this object contains
// ------------------------------------------------------------------------
int vsGeometry::getPrimitiveCount()
{
    return (performerGeoarray->getNumPrims());
}

// ------------------------------------------------------------------------
// Sets the number of verticies for the primitive with the specified index
// within the object. The index of the first primitive is 0.
// ------------------------------------------------------------------------
void vsGeometry::setPrimitiveLength(int index, int length)
{
    // Bounds check
    if ((index < 0) || (index >= getPrimitiveCount()))
    {
        printf("vsGeometry::setPrimitiveLength: Index out of bounds\n");
        return;
    }
    
    // If the geometry's particular primitive type doesn't require a
    // primitive lengths array, don't set anything.
    if ((getPrimitiveType() == VS_GEOMETRY_TYPE_POINTS) ||
        (getPrimitiveType() == VS_GEOMETRY_TYPE_LINES) ||
        (getPrimitiveType() == VS_GEOMETRY_TYPE_TRIS) ||
        (getPrimitiveType() == VS_GEOMETRY_TYPE_QUADS))
        return;

    // Set the desired length value
    lengthsList[index] = length;
}

// ------------------------------------------------------------------------
// Retrieves the number of verticies specified for the primitive with the
// indicated index in the object. The index of the first primitive is 0.
// ------------------------------------------------------------------------
int vsGeometry::getPrimitiveLength(int index)
{
    // Bounds check
    if ((index < 0) || (index >= getPrimitiveCount()))
    {
        printf("vsGeometry::getPrimitiveLength: Index out of bounds\n");
        return -1;
    }
    
    // If the geometry's particular primitive type doesn't require a
    // primitive lengths array, return a pre-packaged value.
    if (getPrimitiveType() == VS_GEOMETRY_TYPE_POINTS)
        return 1;
    if (getPrimitiveType() == VS_GEOMETRY_TYPE_LINES)
        return 2;
    if (getPrimitiveType() == VS_GEOMETRY_TYPE_TRIS)
        return 3;
    if (getPrimitiveType() == VS_GEOMETRY_TYPE_QUADS)
        return 4;

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
    
    // If the geometry's particular primitive type doesn't require a
    // primitive lengths array, don't set anything.
    if ((getPrimitiveType() == VS_GEOMETRY_TYPE_POINTS) ||
        (getPrimitiveType() == VS_GEOMETRY_TYPE_LINES) ||
        (getPrimitiveType() == VS_GEOMETRY_TYPE_TRIS) ||
        (getPrimitiveType() == VS_GEOMETRY_TYPE_QUADS))
        return;

    // Copy the list of primitive lengths from the specified array to
    // our internal array, assuming that the length of the list is
    // equal to the number of primitives in the geometry
    for (loop = 0; loop < getPrimitiveCount(); loop++)
        lengthsList[loop] = lengths[loop];
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
    {
        // If this geometry contains one of the fixed-length primitive
	// types, then copy that fixed length into the result array
	// positions; otherwise, copy the entry from our primitive
	// lengths array.
        switch (getPrimitiveType())
        {
            // The first four cases have fixed primitive lengths
            case VS_GEOMETRY_TYPE_POINTS:
                lengthsBuffer[loop] = 1;
                break;
            case VS_GEOMETRY_TYPE_LINES:
                lengthsBuffer[loop] = 2;
                break;
            case VS_GEOMETRY_TYPE_TRIS:
                lengthsBuffer[loop] = 3;
                break;
            case VS_GEOMETRY_TYPE_QUADS:
                lengthsBuffer[loop] = 4;
                break;

             // The remaining primitives are variable length, so
             // we can simply copy the lengths list we have stored
             // into the buffer provided
            default:
                lengthsBuffer[loop] = lengthsList[loop];
                break;
        }
    }
}

// ------------------------------------------------------------------------
// Sets the binding mode for the geometry object for the given type of
// data. The binding governs how many vertices within the geometry each
// data value affects. Vertex coordinates must always have per-vertex
// binding.
// ------------------------------------------------------------------------
void vsGeometry::setBinding(int whichData, int binding)
{
    int list, unit;

    // Figure out which list is being modified.  If it's a generic list,
    // we need to translate the index.  We also need to make sure we're
    // not changing the binding of a list when the corresponding overlapping
    // list is in use.
    if (whichData >= VS_GEOMETRY_LIST_COUNT)
    {
        // Compute the list index
        list = whichData - VS_GEOMETRY_LIST_COUNT;

        // Check the corresponding conventional attribute list to make sure
        // it's not being used
        if ((dataListSize[list] > 0) && (!dataIsGeneric[list]))
        {
            printf("vsGeometry::setBinding:  Cannot modify binding on generic "
                 "attribute type %d when\n", list);
            printf("    corresponding conventional attribute type is in "
                 "use.\n");
            return;
        }
    }
    else
    {
        // Remember the list index
        list = whichData;

        // Check the corresponding generic attribute list to make sure
        // it's not being used
        if ((dataListSize[list] > 0) && (dataIsGeneric[list]))
        {
            printf("vsGeometry::setBinding:  Cannot modify binding on "
                 "conventional attribute type %d when\n", list);
            printf("    corresponding generic attribute type is in "
                 "use.\n");
            return;
        }
    }
    
    // Figure out which data is being affected and apply the new binding
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            // Vertex coordinates should always be per-vertex
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsGeometry::setBinding: Vertex coordinate binding must "
                    "always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }
            // Enable the attribute list on the geoarray, if necessary
            if ((dataBinding[list] != VS_GEOMETRY_BIND_PER_VERTEX) &&
                (dataList[list] != NULL))
            {
                performerGeoarray->enableAttr(dataAttr[list]);
                dataBinding[list] = VS_GEOMETRY_BIND_PER_VERTEX;
            }
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
            // Vertex weights are not supported directly by Performer, so
            // we pass them down as a generic attribute. Vertex weights 
            // binding should be either none or per-vertex
            if ((binding != VS_GEOMETRY_BIND_PER_VERTEX) &&
                (binding != VS_GEOMETRY_BIND_NONE))
            {
                printf("vsGeometry::setBinding: Vertex weights binding "
                    "must be either VS_GEOMETRY_BIND_PER_VERTEX or "
                    "VS_GEOMETRY_BIND_NONE\n");
                return;
            }

            // Set vertex weight binding on the geoset
            if (binding == VS_GEOMETRY_BIND_NONE)
            {
                // Remove the attribute list from the geoarray, if necessary
                if ((dataBinding[list] != VS_GEOMETRY_BIND_NONE) &&
                    (dataList[list] != NULL))
                    performerGeoarray->disableAttr(dataAttr[list]);
            }           
            else
            {
                // Add the attribute list to the geoarray, if necessary
                if ((dataBinding[list] == VS_GEOMETRY_BIND_NONE) &&
                    (dataList[list] != NULL))
                    performerGeoarray->enableAttr(dataAttr[list]);
            }
            dataBinding[list] = binding;
            break;

        case VS_GEOMETRY_NORMALS:
            // Update the binding on the fake normal list and recompute the
            // actual list, which will adjust the actual list's binding
            // appropriately
            normalBinding = binding;
            convertToPerVertex(VS_GEOMETRY_NORMALS);
            break;

        case VS_GEOMETRY_COLORS:
            // Update the binding on the fake color list and recompute the
            // actual list, which will adjust the actual list's binding
            // appropriately
            colorBinding = binding;
            convertToPerVertex(VS_GEOMETRY_COLORS);
            break;

        case VS_GEOMETRY_ALT_COLORS:
            // Alternate colors are not supported directly by Performer, so
            // we pass them down as a generic attribute.  Their binding 
            // should be either none or per-vertex
            if ((binding != VS_GEOMETRY_BIND_PER_VERTEX) &&
                (binding != VS_GEOMETRY_BIND_NONE))
            {
                printf("vsGeometry::setBinding: Alternate color binding "
                    "must be either VS_GEOMETRY_BIND_PER_VERTEX or "
                    "VS_GEOMETRY_BIND_NONE\n");
                return;
            }

            // Set vertex weight binding on the geoset
            if (binding == VS_GEOMETRY_BIND_NONE)
            {
                // Remove the attribute list from the geoarray, if necessary
                if ((dataBinding[list] != VS_GEOMETRY_BIND_NONE) &&
                    (dataList[list] != NULL))
                    performerGeoarray->disableAttr(dataAttr[list]);
            }           
            else
            {
                // Add the attribute list to the geoarray, if necessary
                if ((dataBinding[list] == VS_GEOMETRY_BIND_NONE) &&
                    (dataList[list] != NULL))
                    performerGeoarray->enableAttr(dataAttr[list]);
            }
            dataBinding[list] = binding;
            break;

        case VS_GEOMETRY_FOG_COORDS:
            // Fog coordinates are not supported directly by Performer, so
            // we pass them down as a generic attribute. Fog coordinates
            // binding should be either none or per-vertex
            if ((binding != VS_GEOMETRY_BIND_PER_VERTEX) &&
                (binding != VS_GEOMETRY_BIND_NONE))
            {
                printf("vsGeometry::setBinding: Fog coordinates binding "
                    "must be either VS_GEOMETRY_BIND_PER_VERTEX or "
                    "VS_GEOMETRY_BIND_NONE\n");
                return;
            }

            // Set fog coordinate binding on the geoset
            if (binding == VS_GEOMETRY_BIND_NONE)
            {
                // Remove the attribute list from the geoarray, if necessary
                if ((dataBinding[list] != VS_GEOMETRY_BIND_NONE) &&
                    (dataList[list] != NULL))
                    performerGeoarray->disableAttr(dataAttr[list]);
            }           
            else
            {
                // Add the attribute list to the geoarray, if necessary
                if ((dataBinding[list] == VS_GEOMETRY_BIND_NONE) &&
                    (dataList[list] != NULL))
                    performerGeoarray->enableAttr(dataAttr[list]);
            }
            dataBinding[list] = binding;
            break;

        case VS_GEOMETRY_USER_DATA0:
        case VS_GEOMETRY_USER_DATA1:
            // We pass user data down as a generic attribute.  Binding 
            // should be either none or per-vertex
            if ((binding != VS_GEOMETRY_BIND_PER_VERTEX) &&
                (binding != VS_GEOMETRY_BIND_NONE))
            {
                printf("vsGeometry::setBinding: User data binding "
                    "must be either VS_GEOMETRY_BIND_PER_VERTEX or "
                    "VS_GEOMETRY_BIND_NONE\n");
                return;
            }

            // Set user data binding on the geoarray
            if (binding == VS_GEOMETRY_BIND_NONE)
            {
                // Remove the attribute list from the geoarray, if necessary
                if ((dataBinding[list] != VS_GEOMETRY_BIND_NONE) &&
                    (dataList[list] != NULL))
                    performerGeoarray->disableAttr(dataAttr[list]);
            }           
            else
            {
                // Add the attribute list to the geoarray, if necessary
                if ((dataBinding[list] == VS_GEOMETRY_BIND_NONE) &&
                    (dataList[list] != NULL))
                    performerGeoarray->enableAttr(dataAttr[list]);
            }
            dataBinding[list] = binding;
            break;

        case VS_GEOMETRY_TEXTURE0_COORDS:
        case VS_GEOMETRY_TEXTURE1_COORDS:
        case VS_GEOMETRY_TEXTURE2_COORDS:
        case VS_GEOMETRY_TEXTURE3_COORDS:
        case VS_GEOMETRY_TEXTURE4_COORDS:
        case VS_GEOMETRY_TEXTURE5_COORDS:
        case VS_GEOMETRY_TEXTURE6_COORDS:
        case VS_GEOMETRY_TEXTURE7_COORDS:
            // Calculate the texture unit we're working with
            unit = list - VS_GEOMETRY_TEXTURE0_COORDS;

            // Texture coordinates should always be either per-vertex or off
            if ((binding != VS_GEOMETRY_BIND_PER_VERTEX) &&
                (binding != VS_GEOMETRY_BIND_NONE))
            {
                printf("vsGeometry::setBinding: Texture coordinates binding "
                    "must be either VS_GEOMETRY_BIND_PER_VERTEX or "
                    "VS_GEOMETRY_BIND_NONE\n");
                return;
            }

            // Set texture coordinate binding on the geoset
            if (binding == VS_GEOMETRY_BIND_NONE)
            {
                // Remove the attribute list from the geoarray, if necessary
                if ((dataBinding[list] != VS_GEOMETRY_BIND_NONE) &&
                    (dataList[list] != NULL))
                    performerGeoarray->disableAttr(dataAttr[list]);
            }           
            else
            {
                // Add the attribute list to the geoarray, if necessary
                if ((dataBinding[list] == VS_GEOMETRY_BIND_NONE) &&
                    (dataList[list] != NULL))
                    performerGeoarray->enableAttr(dataAttr[list]);
            }
            dataBinding[list] = binding;
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
            // Generic attributes should always be either per-vertex or off
            if ((binding != VS_GEOMETRY_BIND_PER_VERTEX) &&
                (binding != VS_GEOMETRY_BIND_NONE))
            {
                // Print out an appropriate error message
                if (whichData >= VS_GEOMETRY_LIST_COUNT)
                    printf("vsGeometry::setBinding: Generic attribute binding "
                        "must be either VS_GEOMETRY_BIND_PER_VERTEX or "
                        "VS_GEOMETRY_BIND_NONE\n");
                return;
            }

            // Set generic attribute binding on the geoarray
            if (binding == VS_GEOMETRY_BIND_NONE)
            {
                // Remove the attribute list from the geoarray, if necessary
                if ((dataBinding[list] != VS_GEOMETRY_BIND_NONE) &&
                    (dataList[list] != NULL))
                    performerGeoarray->disableAttr(dataAttr[list]);
            }           
            else
            {
                // Add the attribute list to the geoarray, if necessary
                if ((dataBinding[list] == VS_GEOMETRY_BIND_NONE) &&
                    (dataList[list] != NULL))
                    performerGeoarray->enableAttr(dataAttr[list]);
            }
            dataBinding[list] = binding;
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
    int list;

    // Figure out which list is required, return the "fake" binding (used
    // in the emulation of PER_PRIMITIVE and OVERALL data) if the normal
    // or color list is selected
    if (whichData == VS_GEOMETRY_NORMALS)
        return normalBinding;
    else if (whichData == VS_GEOMETRY_COLORS)
        return colorBinding;
    else if (whichData >= VS_GEOMETRY_LIST_COUNT)
        list = whichData - VS_GEOMETRY_LIST_COUNT;
    else
        list = whichData;

    // Make sure we're not returning the binding of a generic list when a
    // conventional list is active, and vice versa
    if (((list == whichData) && (!dataIsGeneric[list])) ||
        ((list != whichData) && (dataIsGeneric[list])))
        return dataBinding[list];
    else
        return VS_GEOMETRY_BIND_NONE;
}

// ------------------------------------------------------------------------
// Sets one data point within the geometry objects' lists of data. The
// whichData value specifies which type of data is to be affected, and
// the index specifies which data point is to be altered. The index of
// the first data point is 0.
// ------------------------------------------------------------------------
void vsGeometry::setData(int whichData, int dataIndex, atVector data)
{
    int list;
    int loop;
    int listSize;

    // Figure out which list we're changing
    if (whichData >= VS_GEOMETRY_LIST_COUNT)
        list = whichData - VS_GEOMETRY_LIST_COUNT;
    else
        list = whichData;

    // Bounds check.  First get the size of the list we're working with.
    // If we're working with normals or colors we need to check against the
    // internal list sizes instead of the pfGeoArray list sizes.
    if (whichData == VS_GEOMETRY_NORMALS)
        listSize = normalListSize;
    else if (whichData == VS_GEOMETRY_COLORS)
        listSize = colorListSize;
    else
        listSize = dataListSize[list];
    
    // Now check the given index against the size of the list
    if ((dataIndex < 0) || (dataIndex >= listSize))
    {
        printf("vsGeometry::setData: Index out of bounds\n");
        printf("   list = %d, size = %d,  index = %d\n", list, 
            listSize, dataIndex);
        return;
    }

    // Make sure we don't trample over a list that's in use by trying to
    // alter it's corresponding overlapping list
    if ((dataIsGeneric[list]) && (list == whichData))
    {
        printf("vsGeometry::setData:  Cannot modify data on conventional "
             "attribute type %d when\n", list);
        printf("    corresponding generic attribute type is in use.\n");
        return;
    }
    else if ((!dataIsGeneric[list]) && (list != whichData))
    {
        printf("vsGeometry::setData:  Cannot modify data on generic "
             "attribute type %d when\n", list);
        printf("    corresponding conventional attribute type is in "
             "use.\n");
        return;
    }
    
    // Different actions necessary depending on which data is being set
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            // Input check
            if (data.getSize() < 3)
            {
                printf("vsGeometry::setData: Insufficient data (vertex "
                    "coordinates require 3 values)\n");
                return;
            }
            // Copy the data into our list
            for (loop = 0; loop < 3; loop++)
                dataList[list][dataIndex*3 + loop] = data[loop];
            break;

        case VS_GEOMETRY_NORMALS:
            // Input check
            if (data.getSize() < 3)
            {
                printf("vsGeometry::setData: Insufficient data (vertex "
                    "normals require 3 values)\n");
                return;
            }
            // Copy the data into the emulated normal list, and also the
            // real data list if the binding is per-vertex
            for (loop = 0; loop < 3; loop++)
            {
                normalList[dataIndex*3 + loop] = data[loop];
            }

            // Use the appropriate helper function to alter the real data 
            // list
            if (normalBinding == VS_GEOMETRY_BIND_OVERALL)
                setOverallData(VS_GEOMETRY_NORMALS, data);
            else if (normalBinding == VS_GEOMETRY_BIND_PER_PRIMITIVE)
                setPerPrimitiveData(VS_GEOMETRY_NORMALS, dataIndex, data);
            else if (normalBinding == VS_GEOMETRY_BIND_PER_VERTEX)
            {
                // Use the same copy operation as above
                for (loop = 0; loop < 3; loop++)
                {
                    dataList[list][dataIndex*3 + loop] = data[loop];
                }
            }
            break;

        case VS_GEOMETRY_COLORS:
            if (data.getSize() < 4)
            {
                printf("vsGeometry::setData: Insufficient data (colors "
                    "require 4 values)\n");
                return;
            }
            // Copy the data into our list (both new and old-style color lists)
            for (loop = 0; loop < 4; loop++)
            {
                colorList[dataIndex*4 + loop] = data[loop];
            }

            // Use the appropriate helper function to alter the real data 
            // list
            if (colorBinding == VS_GEOMETRY_BIND_OVERALL)
                setOverallData(VS_GEOMETRY_COLORS, data);
            else if (colorBinding == VS_GEOMETRY_BIND_PER_PRIMITIVE)
                setPerPrimitiveData(VS_GEOMETRY_COLORS, dataIndex, data);
            else if (colorBinding == VS_GEOMETRY_BIND_PER_VERTEX)
            {
                // Use the same copy operation as above
                for (loop = 0; loop < 4; loop++)
                {
                    dataList[list][dataIndex*4 + loop] = data[loop];
                }
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
            // Input check
            if (data.getSize() < 2)
            {
                printf("vsGeometry::setData: Insufficient data (texture "
                    "coordinates require 2 values)\n");
                return;
            }
            // Copy the data into our list
            for (loop = 0; loop < 2; loop++)
                dataList[list][dataIndex*2 + loop] = data[loop];
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
        case VS_GEOMETRY_ALT_COLORS:
        case VS_GEOMETRY_FOG_COORDS:
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
            // Copy the data into our list
            for (loop = 0; loop < data.getSize(); loop++)
                dataList[list][dataIndex*4 + loop] = data[loop];
            break;

        default:
            printf("vsGeometry::setData: Unrecognized data type\n");
            return;
    }

    // Update the pfGeoArray's data
    performerGeoarray->updateData();
}

// ------------------------------------------------------------------------
// Retrieves one data point from the geometry objects' lists of data. The
// whichData value indicates which list to pull from, and the index
// specifies which point is desired. The index of the first data point is
// 0.
// ------------------------------------------------------------------------
atVector vsGeometry::getData(int whichData, int dataIndex)
{
    atVector result;
    int loop;
    int list;
    int listSize;
    
    // Figure out which list we're changing
    if (whichData >= VS_GEOMETRY_LIST_COUNT)
        list = whichData - VS_GEOMETRY_LIST_COUNT;
    else
        list = whichData;

    // Bounds check.  First get the size of the list we're working with.
    // If we're working with normals or colors we need to check against the
    // internal list sizes instead of the pfGeoArray list sizes.
    if (whichData == VS_GEOMETRY_NORMALS)
        listSize = normalListSize;
    else if (whichData == VS_GEOMETRY_COLORS)
        listSize = colorListSize;
    else
        listSize = dataListSize[list];
    
    // Now check the given index against the size of the list
    if ((dataIndex < 0) || (dataIndex >= listSize))
    {
        printf("vsGeometry::getData: Index out of bounds (dataIndex = %d)\n",
            dataIndex);
        return result;
    }

    // Make sure we don't return information about a generic list when
    // a conventional list is specified or vice versa
    if ((list == whichData) && (dataIsGeneric[list]))
    {
        printf("vsGeometry::getData:  Cannot query data from a conventional "
            "attribute list\n");
        printf("    when the corresponding generic list is active\n");
        return result;
    }
    else if ((list != whichData) && (!dataIsGeneric[list]))
    {
        printf("vsGeometry::getData:  Cannot query data from a generic "
            "attribute list\n");
        printf("    when the corresponding conventional list is active\n");
        return result;
    }
    
    // Determine which list we should obtain the data from, and return
    // the requested item from that list
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            // Copy the data to the result vector
            result.setSize(3);
            for (loop = 0; loop < 3; loop++)
                result[loop] = dataList[list][dataIndex*3 + loop];
            break;

        case VS_GEOMETRY_NORMALS:
            // Copy the data to the result vector
            result.setSize(3);
            for (loop = 0; loop < 3; loop++)
                result[loop] = normalList[dataIndex*3 + loop];
            break;

        case VS_GEOMETRY_COLORS:
            // Copy the data to the result vector
            result.setSize(4);
            for (loop = 0; loop < 4; loop++)
                result[loop] = colorList[dataIndex*4 + loop];
            break;

        case VS_GEOMETRY_TEXTURE0_COORDS:
        case VS_GEOMETRY_TEXTURE1_COORDS:
        case VS_GEOMETRY_TEXTURE2_COORDS:
        case VS_GEOMETRY_TEXTURE3_COORDS:
        case VS_GEOMETRY_TEXTURE4_COORDS:
        case VS_GEOMETRY_TEXTURE5_COORDS:
        case VS_GEOMETRY_TEXTURE6_COORDS:
        case VS_GEOMETRY_TEXTURE7_COORDS:
            // Copy the data to the result vector
            result.setSize(2);
            for (loop = 0; loop < 2; loop++)
                result[loop] = dataList[list][dataIndex*2 + loop];
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
        case VS_GEOMETRY_ALT_COLORS:
        case VS_GEOMETRY_FOG_COORDS:
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
            // Copy the data to the result vector
            result.setSize(4);
            for (loop = 0; loop < 4; loop++)
                result[loop] = dataList[list][dataIndex*4 + loop];
            break;

        default:
            printf("vsGeometry::getData: Unrecognized data type\n");
            return result;
    }
    
    // Return the copied data vector
    return result;
}

// ------------------------------------------------------------------------
// Sets all of the data points within one of the geometry objects' lists
// to the values in dataList. The dataList array must be at least as large
// as the size of particular list in question.
// ------------------------------------------------------------------------
void vsGeometry::setDataList(int whichData, atVector *newDataList)
{
    int loop, sloop;
    int list;

    // Figure out which list we're changing
    if (whichData >= VS_GEOMETRY_LIST_COUNT)
        list = whichData - VS_GEOMETRY_LIST_COUNT;
    else
        list = whichData;
    
    // Make sure we don't trample over a list that's in use by trying to
    // alter it's corresponding overlapping list
    if ((dataIsGeneric[list]) && (list == whichData))
    {
        printf("vsGeometry::setDataList:  Cannot modify data on conventional "
             "attribute type %d when\n", list);
        printf("    corresponding generic attribute type is in use.\n");
        return;
    }
    else if ((!dataIsGeneric[list]) && (list != whichData))
    {
        printf("vsGeometry::setDataList:  Cannot modify data on generic "
             "attribute type %d when\n", list);
        printf("    corresponding conventional attribute type is in "
             "use.\n");
        return;
    }
    
    // Interpret the whichData constant and copy the data from the new
    // data list provided to the appropriate data list
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            for (loop = 0; loop < dataListSize[list]; loop++)
                for (sloop = 0; sloop < 3; sloop++)
                    dataList[list][loop*3 + sloop] = newDataList[loop][sloop];
            break;

        case VS_GEOMETRY_NORMALS:
            // Copy the input list to our internal normal list
            for (loop = 0; loop < dataListSize[list]; loop++)
                for (sloop = 0; sloop < 3; sloop++)
                {
                    normalList[loop*3 + sloop] = newDataList[loop][sloop];
                }

            // Now that the internal normal list is set, convert the list to
            // a per-vertex list for rendering
            convertToPerVertex(list);
            break;

        case VS_GEOMETRY_COLORS:
            // Copy the input list to our internal color list
            for (loop = 0; loop < dataListSize[list]; loop++)
                for (sloop = 0; sloop < 4; sloop++)
                {
                    colorList[loop*4 + sloop] = newDataList[loop][sloop];
                }

            // Now that the internal color list is set, convert the list to
            // a per-vertex list for rendering
            convertToPerVertex(list);
            break;

        case VS_GEOMETRY_TEXTURE0_COORDS:
        case VS_GEOMETRY_TEXTURE1_COORDS:
        case VS_GEOMETRY_TEXTURE2_COORDS:
        case VS_GEOMETRY_TEXTURE3_COORDS:
        case VS_GEOMETRY_TEXTURE4_COORDS:
        case VS_GEOMETRY_TEXTURE5_COORDS:
        case VS_GEOMETRY_TEXTURE6_COORDS:
        case VS_GEOMETRY_TEXTURE7_COORDS:
            for (loop = 0; loop < dataListSize[list]; loop++)
                for (sloop = 0; sloop < 2; sloop++)
                    dataList[list][loop*2 + sloop] = newDataList[loop][sloop];
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
        case VS_GEOMETRY_ALT_COLORS:
        case VS_GEOMETRY_FOG_COORDS:
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
            for (loop = 0; loop < dataListSize[list]; loop++)
                for (sloop = 0; sloop < 4; sloop++)
                    dataList[list][loop*4 + sloop] = newDataList[loop][sloop];

        default:
            printf("vsGeometry::setDataList: Unrecognized data type\n");
            return;
    }

    // Update the pfGeoArray's data
    performerGeoarray->updateData();
}

// ------------------------------------------------------------------------
// Retrieves all of the data points within one of the geometry objects'
// lists, storing that data in the specified dataBuffer. The dataBuffer
// array must be at least as large as the size of particular list in
// question.
// ------------------------------------------------------------------------
void vsGeometry::getDataList(int whichData, atVector *dataBuffer)
{
    int list;
    int loop, sloop;
    
    // Figure out which list we're changing
    if (whichData >= VS_GEOMETRY_LIST_COUNT)
        list = whichData - VS_GEOMETRY_LIST_COUNT;
    else
        list = whichData;
    
    // Make sure we don't trample over a list that's in use by trying to
    // alter it's corresponding overlapping list
    if ((dataIsGeneric[list]) && (list == whichData))
    {
        printf("vsGeometry::getDataList:  Cannot modify data on conventional "
             "attribute type %d when the\n", list);
        printf("    corresponding generic attribute type is in use.\n");
        return;
    }
    else if ((!dataIsGeneric[list]) && (list != whichData))
    {
        printf("vsGeometry::getDataList:  Cannot modify data on generic "
             "attribute type %d when the\n", list);
        printf("    corresponding conventional attribute type is in "
             "use.\n");
        return;
    }
    
    // Interpret the whichData constant and copy the appropriate data list
    // to the given data buffer
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            for (loop = 0; loop < dataListSize[list]; loop++)
            {
                // Copy the data to the vector buffer
                dataBuffer[loop].setSize(3);
                for (sloop = 0; sloop < 3; sloop++)
                    dataBuffer[loop][sloop] = dataList[list][loop*3 + sloop];
            }
            break;

        case VS_GEOMETRY_NORMALS:
            // Return the data from the internal normal list
            for (loop = 0; loop < dataListSize[list]; loop++)
            {
                // Copy the data to the vector buffer
                dataBuffer[loop].setSize(3);
                for (sloop = 0; sloop < 3; sloop++)
                    dataBuffer[loop][sloop] = normalList[loop*3 + sloop];
            }
            break;

        case VS_GEOMETRY_COLORS:
            // Return the data from the internal color list
            for (loop = 0; loop < dataListSize[list]; loop++)
            {
                // Copy the data to the vector buffer
                dataBuffer[loop].setSize(4);
                for (sloop = 0; sloop < 4; sloop++)
                    dataBuffer[loop][sloop] = colorList[loop*4 + sloop];
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
            for (loop = 0; loop < dataListSize[list]; loop++)
            {
                // Copy the data to the vector buffer
                dataBuffer[loop].setSize(2);
                for (sloop = 0; sloop < 2; sloop++)
                    dataBuffer[loop][sloop] = dataList[list][loop*2 + sloop];
            }
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
        case VS_GEOMETRY_ALT_COLORS:
        case VS_GEOMETRY_FOG_COORDS:
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
            for (loop = 0; loop < dataListSize[list]; loop++)
            {
                // Copy the data to the vector buffer
                dataBuffer[loop].setSize(4);
                for (sloop = 0; sloop < 4; sloop++)
                    dataBuffer[loop][sloop] = dataList[list][loop*4 + sloop];
            }

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
    unsigned int unit;
    int binding, performerBinding;
    int list;
    int copySize;
    float *tempList;

    // Figure out which list we're changing
    if (whichData >= VS_GEOMETRY_LIST_COUNT)
        list = whichData - VS_GEOMETRY_LIST_COUNT;
    else
        list = whichData;

    // Check to see if the overlapping generic or conventional list is
    // already in use before we change this one
    if ((list == whichData) && (dataIsGeneric[list]) && 
        (dataListSize[list] > 0))
    {
        printf("vsGeometry::setDataListSize:  Cannot resize conventional "
             "attribute list %d when the\n", list);
        printf("    corresponding generic attribute list is in use.\n");
        printf("    Resize the corresponding list to 0 first.\n");
        return;
    }
    else if ((list != whichData) && (!dataIsGeneric[list]) &&
             (dataListSize[list] > 0))
    {
        printf("vsGeometry::setDataListSize:  Cannot resize generic "
             "attribute list %d when the\n", list);
        printf("    corresponding conventional attribute list is in use.\n");
        printf("    Resize the corresponding list to 0 first.\n");
        return;
    }

    // If we're resizing the list to the same size, there's not much to do.
    // Just make sure we compare against the internal list sizes for normals
    // and colors.
    if (list == VS_GEOMETRY_NORMALS) 
    {
        if (normalListSize == newSize)
            return;
    }
    else if (list == VS_GEOMETRY_COLORS) 
    {
        if (colorListSize == newSize)
            return;
    }
    else if (dataListSize[list] == newSize)
        return;

    // If we get this far, we're correctly modifying the requested list.
    // First, set the "is generic" flag on the list to the correct value.
    if (list == whichData)
        dataIsGeneric[list] = false;
    else
        dataIsGeneric[list] = true;

    // Get the VESS binding for this geometry
    binding = getBinding(whichData);

    // Translate the VESS binding to its Performer counterpart
    switch (binding)
    {
        case VS_GEOMETRY_BIND_NONE:
            performerBinding = PFGS_OFF;
            break;
        case VS_GEOMETRY_BIND_OVERALL:
            performerBinding = PFGS_OVERALL;
            break;
        case VS_GEOMETRY_BIND_PER_PRIMITIVE:
            performerBinding = PFGS_PER_PRIM;
            break;
        case VS_GEOMETRY_BIND_PER_VERTEX:
            performerBinding = PFGS_PER_VERTEX;
            break;
    }

    // Interpret the whichData constant
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            // Determine what we need to do with the data list
            // based on whether or not it currently exists, and
            // the desired new size of the list
            if (newSize && !dataList[list])
            {
                // No list exists, create a new list
                dataList[list] = (float *)(pfMemory::malloc(
                    sizeof(float) * 3 * newSize));
                pfMemory::ref(dataList[list]);

                // Set the newly-created vertex list on the pfGeoArray, if it
                // is currently bound
                dataAttr[list] = performerGeoarray->setAttr(PFGA_COORD_ARRAY, 
                    3, GL_FLOAT, 0, dataList[list]);

                // Automatically bind the list as per-vertex
                performerGeoarray->enableAttr(dataAttr[list]);
                dataBinding[list] = VS_GEOMETRY_BIND_PER_VERTEX;
            }
            else if (!newSize && dataList[list])
            {
                // List exists, but the requested new size is zero, so
                // first remove the list from the geoarray
                if (dataAttr[list])
                {
                    performerGeoarray->removeAttr(dataAttr[list]);
                    dataAttr[list] = NULL;
                }

                // Delete the existing list
                pfMemory::unrefDelete(dataList[list]);
                dataList[list] = NULL;

                // To prevent confusion, unbind the list when it's deleted
                dataBinding[list] = VS_GEOMETRY_BIND_NONE;
            }
            else if (newSize && dataList[list])
            {
                // Modify the length of the existing list
                tempList = dataList[list];
                dataList[list] = (float *)(pfMemory::malloc(sizeof(float) * 
                   3 * newSize));
                pfMemory::ref(dataList[list]);

                // Figure out how much data to copy from the old list
                if (newSize < dataListSize[list])
                    copySize = newSize * 3 * sizeof(float);
                else
                    copySize = dataListSize[list] * 3 * sizeof(float);

                // Copy the data from the old list
                memcpy(dataList[list], tempList, copySize);

                // Update the pfGeoArray data
                dataAttr[list]->setPtr(dataList[list]);
                performerGeoarray->updateData();

                // Free the old list
                pfMemory::unrefDelete(tempList);
            }

            // Store the new list size
            dataListSize[list] = newSize;

            // Since we've changed the number of vertices in the geometry,
            // we may need to reconvert the normal and color arrays (if their
            // bindings are not per-vertex)
            convertToPerVertex(VS_GEOMETRY_NORMALS);
            convertToPerVertex(VS_GEOMETRY_COLORS);
            break;

        case VS_GEOMETRY_NORMALS:
            // Determine what we need to do with the data list
            // based on whether or not it currently exists, and
            // the desired new size of the list
            if (newSize && !normalList)
            {
                // No list exists, create new normal list
                normalList = (float *)(malloc(
                    sizeof(float) * 3 * newSize));
            }
            else if (!newSize && normalList)
            {
                // List exists, but the requested new size is zero, so
                // delete the existing normal list
                free(normalList);
                normalList = NULL;
            }
            else if (newSize && normalList)
            {
                // Modify the length of the existing list using realloc.
                normalList = (float *)(realloc(normalList,
                    sizeof(float) * 3 * newSize));
            }

            // Store the new list size, and convert the internal list to
            // a per-vertex sized list
            normalListSize = newSize;
            convertToPerVertex(VS_GEOMETRY_NORMALS);
            break;

        case VS_GEOMETRY_COLORS:
            // Determine what we need to do with the data list
            // based on whether or not it currently exists, and
            // the desired new size of the list
            if (newSize && !colorList)
            {
                // No list exists, create new color list
                colorList = (float *)(malloc(
                    sizeof(float) * 4 * newSize));
            }
            else if (!newSize && colorList)
            {
                // List exists, but the requested new size is zero, so
                // delete the existing color list
                free(colorList);
                colorList = NULL;
            }
            else if (newSize && colorList)
            {
                // Modify the length of the existing list using realloc.
                colorList = (float *)(realloc(colorList,
                    sizeof(float) * 4 * newSize));
            }

            // Store the new list size, and convert the internal list to
            // a per-vertex sized list
            colorListSize = newSize;
            convertToPerVertex(VS_GEOMETRY_COLORS);
            break;

        case VS_GEOMETRY_TEXTURE0_COORDS:
        case VS_GEOMETRY_TEXTURE1_COORDS:
        case VS_GEOMETRY_TEXTURE2_COORDS:
        case VS_GEOMETRY_TEXTURE3_COORDS:
        case VS_GEOMETRY_TEXTURE4_COORDS:
        case VS_GEOMETRY_TEXTURE5_COORDS:
        case VS_GEOMETRY_TEXTURE6_COORDS:
        case VS_GEOMETRY_TEXTURE7_COORDS:
            // Calculate the texture unit we're working with
            unit = list - VS_GEOMETRY_TEXTURE0_COORDS;

            // Determine what we need to do with the data list
            // based on whether or not it currently exists, and
            // the desired new size of the list
            if (newSize && !dataList[list])
            {
                // No list exists, create a new texture coordinate list
                dataList[list] = (float *)(pfMemory::malloc(
                    sizeof(float) * 2 * newSize));
                pfMemory::ref(dataList[list]);
                                                                                
                // Set the newly-created texture coordinate list on the
                // pfGeoArray
                if (dataBinding[list] == VS_GEOMETRY_BIND_PER_VERTEX)
                    dataAttr[list] = performerGeoarray->
                        setMultiAttr(PFGA_TEX_ARRAY, unit, 2, GL_FLOAT, 0, 
                            dataList[list]);
            }
            else if (!newSize && dataList[list])
            {
                // List exists, but the requested new size is zero, so
                // first remove the list from the geoarray
                if (dataAttr[list])
                {
                    performerGeoarray->removeAttr(dataAttr[list]);
                    dataAttr[list] = NULL;
                }

                // Delete the existing texture coordinate list
                pfMemory::unrefDelete(dataList[list]);
                dataList[list] = NULL;

                // To prevent confusion, unbind the list when it's deleted
                dataBinding[list] = VS_GEOMETRY_BIND_NONE;
            }
            else if (newSize && dataList[list])
            {
                // Modify the length of the existing list
                tempList = dataList[list];
                dataList[list] = (float *)(pfMemory::malloc(sizeof(float) *
                    2 * newSize));
                pfMemory::ref(dataList[list]);
                                                                                
                // Figure out how much data to copy from the old list
                if (newSize < dataListSize[list])
                    copySize = newSize * 2 * sizeof(float);
                else
                    copySize = dataListSize[list] * 2 * sizeof(float);

                // Copy the data from the old list
                memcpy(dataList[list], tempList, copySize);

                // Update the pfGeoArray data
                dataAttr[list]->setPtr(dataList[list]);
                performerGeoarray->updateData();

                // Free the old list
                pfMemory::unrefDelete(tempList);
            }

            // Store the new list size
            dataListSize[list] = newSize;
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
        case VS_GEOMETRY_ALT_COLORS:
        case VS_GEOMETRY_FOG_COORDS:
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
            // Determine what we need to do with the data list
            // based on whether or not it currently exists, and
            // the desired new size of the list
            if (newSize && !dataList[list])
            {
                // No list exists, create a new generic list
                dataList[list] = (float *)(pfMemory::malloc(
                    sizeof(float) * 4 * newSize));
                pfMemory::ref(dataList[list]);
                                                                                
                // Set the newly-created generic list on the
                // pfGeoArray
                if (dataBinding[list] == VS_GEOMETRY_BIND_PER_VERTEX)
                    dataAttr[list] = performerGeoarray->
                        setMultiAttr(PFGA_GENERIC_ARRAY, list, 4, GL_FLOAT, 
                            0, dataList[list]);
            }
            else if (!newSize && dataList[list])
            {
                // List exists, but the requested new size is zero, so
                // first remove the list from the geoarray
                if (dataAttr[list])
                {
                    performerGeoarray->removeAttr(dataAttr[list]);
                    dataAttr[list] = NULL;
                    dataBinding[list] = VS_GEOMETRY_BIND_NONE;
                }

                // Delete the existing generic list
                performerGeoarray->setMultiAttr(PFGA_GENERIC_ARRAY, list, 4, 
                    GL_FLOAT, 0, NULL);
                pfMemory::unrefDelete(dataList[list]);
                dataList[list] = NULL;
            }
            else if (newSize && dataList[list])
            {
                // Modify the length of the existing list
                tempList = dataList[list];
                dataList[list] = (float *)(pfMemory::malloc(sizeof(float) *
                    4 * newSize));
                pfMemory::ref(dataList[list]);
                                                                                
                // Figure out how much data to copy from the old list
                if (newSize < dataListSize[list])
                    copySize = newSize * 4 * sizeof(float);
                else
                    copySize = dataListSize[list] * 4 * sizeof(float);

                // Copy the data from the old list
                memcpy(dataList[list], tempList, copySize);

                // Update the pfGeoArray data
                dataAttr[list]->setPtr(dataList[list]);
                performerGeoarray->updateData();

                // Free the old list
                pfMemory::unrefDelete(tempList);
            }

            // Store the new list size
            dataListSize[list] = newSize;
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
    int list;

    // Interpret the whichData constant
    if (whichData >= VS_GEOMETRY_LIST_COUNT)
        list = whichData - VS_GEOMETRY_LIST_COUNT; 
    else
        list = whichData;

    // Range check the list index
    if ((list < 0) || (list >= VS_GEOMETRY_LIST_COUNT))
    {
        printf("vsGeometry::getDataListSize:  Invalid data list index\n");
        return -1;
    }

    // Make sure we're not returning the size of a generic list when a
    // conventional list is active, and vice versa
    if (((list == whichData) && (!dataIsGeneric[list])) ||
        ((list != whichData) && (dataIsGeneric[list])))
    {
        // Return the internal list size if normals or colors are requested
        if (list == VS_GEOMETRY_NORMALS)
            return normalListSize;
        else if (list == VS_GEOMETRY_COLORS)
            return colorListSize;
        else
            return dataListSize[list];
    }
    else
        return 0;
}

// ------------------------------------------------------------------------
// Enables lit rendering for this geometry
// ------------------------------------------------------------------------
void vsGeometry::enableLighting()
{
    uint64_t inheritMask;

    // Set the lighting state to inherit from the global state (which
    // always has lighting enabled)
    inheritMask = performerGeostate->getInherit();
    inheritMask |= PFSTATE_ENLIGHTING;
    performerGeostate->setInherit(inheritMask);
}

// ------------------------------------------------------------------------
// Disables lit rendering for this geometry
// ------------------------------------------------------------------------
void vsGeometry::disableLighting()
{
    // Set the lighting state to OFF on the Performer GeoState
    performerGeostate->setMode(PFSTATE_ENLIGHTING, PF_OFF);
}

// ------------------------------------------------------------------------
// Returns if lighting is enabled for this geometry
// ------------------------------------------------------------------------
bool vsGeometry::isLightingEnabled()
{
    // Check the local GeoState to see if the lighting state is inherited.
    // If not, it is locally disabled.
    if ((performerGeostate->getInherit() & PFSTATE_ENLIGHTING))
        return true;
    else
        return false;
}

// ------------------------------------------------------------------------
// Sets the rendering bin to place this object's geometry into
// ------------------------------------------------------------------------
void vsGeometry::setRenderBin(int binNum)
{
    // Store the bin number
    renderBin = binNum;

    // Set the pfGeoArray to use the given bin
    performerGeoarray->setDrawBin((short)binNum);

    // Set the sort order on the draw bin to a default value to force
    // a bin mode update.  This is necessary because Performer will not
    // recognize any bin unless it has been given a bin order for it.
    vsGeometry::setBinSortMode(binNum, VS_GEOMETRY_SORT_STATE);
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
    // Create the list of render bin modes if necessary
    if (!binModeList)
        binModeList = new vsTreeMap();

    // Look for this bin number in the list
    if (binModeList->containsKey((void *)binNum))
        // Change the given bin to use the given sorting mode
        binModeList->changeValue((void *)binNum, (void *)sortMode);
    else
        // Create an entry for the given bin and set the sorting mode
        binModeList->addEntry((void *)binNum, (void *)sortMode);

    // Signal that the bin modes have changed and need to be redistributed
    // to all pfChannels
    binModesChanged = true;
}

// ------------------------------------------------------------------------
// Static function
// Gets the geometry sorting mode for the specified bin number
// ------------------------------------------------------------------------
int vsGeometry::getBinSortMode(int binNum)
{
    // If no list exists, return the default setting (state-sorted)
    if (!binModeList)
        return VS_GEOMETRY_SORT_STATE;

    // If there is no entry for the given bin in the list, return
    // the default setting (state-sorted)
    if (!(binModeList->containsKey((void *)binNum)))
        return VS_GEOMETRY_SORT_STATE;

    // Otherwise, return the bin's setting according to its value in the list
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
    // If the bin mode list exists, clean it up now
    if (binModeList)
    {
        delete binModeList;
        binModeList = NULL;
        binModesChanged = true;
    }
}

// ------------------------------------------------------------------------
// Retrieves the center point and radius of a sphere that encompasses all
// of the geometry within this object.
// ------------------------------------------------------------------------
void vsGeometry::getBoundSphere(atVector *centerPoint, double *radius)
{
    pfSphere boundSphere;
    
    // Get the geometry bounding sphere from the Performer geode
    performerGeode->getBound(&boundSphere);
    
    // Copy the sphere center point to the result vector, if there is one
    if (centerPoint)
        centerPoint->set(boundSphere.center[PF_X], boundSphere.center[PF_Y],
            boundSphere.center[PF_Z]);

    // Copy the sphere radius to the result value, if there is one
    if (radius)
        *radius = boundSphere.radius;
}

// ------------------------------------------------------------------------
// Computes the global coordinate transform at this geometry by multiplying
// together all of the transforms at nodes above this one.
// ------------------------------------------------------------------------
atMatrix vsGeometry::getGlobalXform()
{
    pfNode *nodePtr;
    pfMatrix xform;
    const pfMatrix *scsMatPtr;
    atMatrix result;
    int loop, sloop;

    // Start at this geometry's geode with an identity matrix
    xform.makeIdent();

    // Start the node pointer at the pfGeode
    nodePtr = performerGeode;
    
    // Starting at this geometry's pfGeode, run through all of the
    // nodes in the Performer scene graph and accumulate transforms
    // from every pfSCS (or pfDCS, which is derived from pfSCS) along
    // the way. The assumption here is that each node will only have
    // one parent. (Not always the case, but if there is more then we
    // wouldn't know which one to use anyway.)
    while (nodePtr->getNumParents() > 0)
    {
        // Check if the node is a pfSCS (or subclass of one)
        if (nodePtr->isOfType(pfSCS::getClassType()))
        {
            // Multiply the pfSCS's matrix into our matrix
            scsMatPtr = ((pfSCS *)nodePtr)->getMatPtr();

            // Multiply it by the accumulated matrix
            xform.postMult(*scsMatPtr);
        }
        
        // Move to the node's (first) parent
        nodePtr = nodePtr->getParent(0);
    }
    
    // Copy the pfMatrix into a atMatrix.  Recall that a pfMatrix is
    // transposed with respect to a atMatrix (this is why the indices
    // below are reversed)
    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            result[loop][sloop] = xform[sloop][loop];

    // Return the resulting matrix
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
    // Set the mask of the Performer intersection traversal for this node
    // to the given value.
    performerGeode->setTravMask(PFTRAV_ISECT, newValue, PFTRAV_SELF, PF_SET);
}

// ------------------------------------------------------------------------
// Retrieves the intersection value for this geometry.
// ------------------------------------------------------------------------
unsigned int vsGeometry::getIntersectValue()
{
    // Get the current intersection traversal mask for this node from 
    // Performer and return it
    return (performerGeode->getTravMask(PFTRAV_ISECT));
}

// ------------------------------------------------------------------------
// Adds the given attribute to the geometry object's list of child
// attributes. If successful, also notifies the attribute that it has been
// added to a list.
// ------------------------------------------------------------------------
void vsGeometry::addAttribute(vsAttribute *newAttribute)
{
    int newAttrCat, newAttrType, attrType;
    int loop;
    vsAttribute *attribute;
    unsigned int textureUnit, newTextureUnit;

    // Verify that the attribute is willing to be attached
    if (!(newAttribute->canAttach()))
    {
        printf("vsGeometry::addAttribute: Attribute is already in use\n");
        return;
    }

    // vsGeometries can only contain state attributes for now
    newAttrCat = newAttribute->getAttributeCategory();
    if (newAttrCat != VS_ATTRIBUTE_CATEGORY_STATE)
    {
        printf("vsGeometry::addAttribute: Geometry nodes may not contain "
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
        // Else they were not texture type attributes so print error and
        // return if they are equal.
        else if (attrType == newAttrType)
        {
            printf("vsGeometry::addAttribute: Geometry node "
                "already contains that type of attribute\n");
            return;
        }

        // If the texture units are equal then they both must have been texture
        // type attributes and had the same unit.  We don't want that to be
        // allowed so print error and return.
        if (textureUnit == newTextureUnit)
        {
            printf("vsGeometry::addAttribute: Geometry node "
                "already contains a texture attribute on unit %d\n",
                textureUnit);
            return;
        }
    }

    // If we made it this far, it must be okay to add the attribute in
    vsNode::addAttribute(newAttribute);
}

// ------------------------------------------------------------------------
// Enables culling (view frustum and otherwise) on this node
// ------------------------------------------------------------------------
void vsGeometry::enableCull()
{
    performerGeode->setTravMask(PFTRAV_CULL, 0xFFFFFFFF,
        PFTRAV_SELF | PFTRAV_DESCEND, PF_SET);
}

// ------------------------------------------------------------------------
// Disables culling (view frustum and otherwise) on this node
// ------------------------------------------------------------------------
void vsGeometry::disableCull()
{
    performerGeode->setTravMask(PFTRAV_CULL, 0x0, PFTRAV_SELF | PFTRAV_DESCEND,
        PF_SET);
}

// ------------------------------------------------------------------------
// Returns the Performer object associated with this object
// ------------------------------------------------------------------------
pfGeode *vsGeometry::getBaseLibraryObject()
{
    return performerGeode;
}

// ------------------------------------------------------------------------
// Static internal function
// Returns the tree map of render bins and their modes
// ------------------------------------------------------------------------
vsTreeMap *vsGeometry::getBinModeList()
{
    return binModeList;
}

// ------------------------------------------------------------------------
// Internal function
// Adds a node to this node's list of parent nodes
// ------------------------------------------------------------------------
bool vsGeometry::addParent(vsNode *newParent)
{
    // Add the parent to the list and increment the parent count
    parentList[parentCount++] = newParent;

    // Return success
    return true;
}

// ------------------------------------------------------------------------
// Internal function
// Removes a node from this node's list of parent nodes
// ------------------------------------------------------------------------
bool vsGeometry::removeParent(vsNode *targetParent)
{
    int loop, sloop;

    // Look thru this node's parent list to see if the target parent is
    // there
    for (loop = 0; loop < parentCount; loop++)
    {
        // See if the current parent is the one we're looking for
        if (targetParent == parentList[loop])
        {
            // Found it!  Slide the remaining nodes in the list
            // down by one
            for (sloop = loop; sloop < parentCount-1; sloop++)
                parentList[sloop] = parentList[sloop+1];

            // Decrement the parent count
            parentCount--;

            // Return success
            return true;
        }
    }

    // Couldn't find the target parent, return failure
    return false;
}

// ------------------------------------------------------------------------
// Internal function
// Calls the apply function on all attached attributes, and then calls the
// scene library's graphics state object to affect the changes to the
// graphics library state.
// ------------------------------------------------------------------------
void vsGeometry::applyAttributes()
{
    // Call the inherited version of this function
    vsNode::applyAttributes();
    
    // Call the vsGraphicsState object to configure the Performer geostate 
    // on this geometry
    (vsGraphicsState::getInstance())->applyState(performerGeostate);
}

// ------------------------------------------------------------------------
// VESS static internal function - Performer callback
// "Pre" callback function for pfGeoState attached to the vsGeometry.
// Required in order to activate 'local' vsLightAttributes that are
// affecting this geometry.
// ------------------------------------------------------------------------
int vsGeometry::geostateCallback(pfGeoState *gstate, void *userData)
{
    pfLight **lightList;
    int loop;
    
    // Obtain the list of Performer light objects
    lightList = (pfLight **)userData;
    
    // Turn on local lights
    for (loop = 0; loop < PF_MAX_LIGHTS; loop++)
        if (lightList[loop] != NULL)
            (lightList[loop])->on();

    // Return zero (Performer callback requires a return value, even though
    // it is ignored)
    return 0;
}
