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
//                  coordinates, colors, and face normals.
//
//    Author(s):    Bryan Kline, Jason Daly, Duvan Cope
//
//------------------------------------------------------------------------

#include "vsSkeletonMeshGeometry.h++"

#include <Performer/pf/pfSCS.h>
#include <Performer/pr/pfLight.h>
#include <Performer/pf/pfNode.h>
#include <Performer/pf/pfTraverser.h>
#include "vsComponent.h++"
#include "vsShadingAttribute.h++"
#include "vsGraphicsState.h++"

// ------------------------------------------------------------------------
// Default Constructor - Creates a Performer geode and geoset and connects
// them together, sets up empty geometry lists, and registers callbacks.
// ------------------------------------------------------------------------
vsSkeletonMeshGeometry::vsSkeletonMeshGeometry() : parentList(5, 5)
{
    int loop, list;
    vsDynamicDataList dynList;
    vsDynamicDataList *initData;

    // Start with no parents
    parentCount = 0;

    // Create the pfGeode
    performerGeode = new pfGeode();
    performerGeode->ref();

    // Create a pfFlux for the pfGeoArrays to manage the changes in the 
    // pfGeoArrays as they progress through the different processes in the 
    // Performer pipeline
    performerFlux = new pfFlux(initFluxedGeoArray, PFFLUX_DEFAULT_NUM_BUFFERS);
    performerFlux->ref();

    // Create a pfGeoState
    performerGeostate = new pfGeoState();
    performerGeostate->ref();

    // Extract the first pfGeoArray from the pfFlux and attach the pfGeoState
    // to it
    performerGeoarray = (pfGeoArray *)performerFlux->getCurData();
    performerGeode->addGSet(performerGeoarray);
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

    // Initialize the original (or "skin") vertex and normal lists as well
    originalVertexList = NULL;
    originalNormalList = NULL;

    // Set up a dynamic data list structure used to initialize the fluxed
    // lists below
    dynList.dataList = NULL;
    dynList.dataListSize = 0;
    dynList.dataIsGeneric = false;
    dynList.dataBinding = VS_GEOMETRY_BIND_NONE;

    // In order to preserve data coherency across frames, we'll create another
    // set of pfFluxes to hold the data and parameters so we can modify and
    // resize them.  We could almost do this by using fluxed memory for the
    // data itself, but pfFluxes have fixed size, so we couldn't resize the
    // lists on the fly.
    for (list = 0; list < VS_GEOMETRY_LIST_COUNT; list++)
    {
        // Create a new pfFlux for this list and its metadata
        dynamicData[list] = new pfFlux(sizeof(vsDynamicDataList), 
            PFFLUX_DEFAULT_NUM_BUFFERS);
        dynamicData[list]->ref();
        
        // Initialize the dynamic data list members for all flux buffers
        dynamicData[list]->initData(&dynList);

        // Set the fluxBufferID field on each flux buffer (this is only set 
        // once, and only really useful for debugging
        for (loop = 0;
             loop < dynamicData[list]->getNumBuffers(PFFLUX_BUFFERS_GENERATED);
             loop++)
        {
            // Get the loop'th buffer from the flux for this list, and
            // initialize the buffer ID to a unique number
            initData = 
                (vsDynamicDataList *)dynamicData[list]->getBufferData(loop);
            initData->fluxBufferID = loop;
        }
    }

    // Set the vertex, weight, normal, and bone index list bindings to 
    // per-vertex (since they can't be anything else)
    dataBinding[VS_GEOMETRY_VERTEX_COORDS] = VS_GEOMETRY_BIND_PER_VERTEX;
    dataBinding[VS_GEOMETRY_VERTEX_WEIGHTS] = VS_GEOMETRY_BIND_PER_VERTEX;
    dataBinding[VS_GEOMETRY_NORMALS] = VS_GEOMETRY_BIND_PER_VERTEX;
    dataBinding[VS_GEOMETRY_BONE_INDICES] = VS_GEOMETRY_BIND_PER_VERTEX;

    // Initialize the "public" color list sizes and bindings
    // Because pfGeoArrays are always per-vertex, we have to emulate the
    // overall and per-primitive modes of the color list
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
vsSkeletonMeshGeometry::~vsSkeletonMeshGeometry()
{
    int list;

    // Remove all parents
    detachFromParents();

    // Remove all attributes
    deleteAttributes();

    // Unlink and destroy the Performer objects.  pfDelete()'ing the pfFlux
    // will (should) take care of the pfGeoArrays attached to it.
    performerGeode->unref();
    pfDelete(performerGeode);
    performerFlux->unref();
    pfDelete(performerFlux);
    performerGeostate->unref();
    pfDelete(performerGeostate);

    // Delete the data lists
    for (list = 0; list < VS_MAXIMUM_TEXTURE_UNITS; list++)
    {
        if (dataList[list] && !(pfMemory::getRef(dataList[list])))
            pfMemory::free(dataList[list]);

        dynamicData[list]->unref();
        pfDelete(dynamicData[list]);
    }
    if (lengthsList && !(pfMemory::getRef(lengthsList)))
        pfMemory::free(lengthsList);
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
void vsSkeletonMeshGeometry::convertToPerVertex(int list)
{
    float *fakeList, *tempList;
    int fakeListSize, fakeBinding;
    int elementSize, copySize;
    int i, j, k;
    int baseIndex, realIndex, fakeIndex;
    int realListType;
    int newSize;

    // See which list we're converting
    if (list == VS_GEOMETRY_COLORS)
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
void vsSkeletonMeshGeometry::setOverallData(int list, vsVector data)
{
    int i, j;

    // Figure out which list we're manipulating. Note that we assume all
    // error checking on ranges, list sizes, and bounds has already been
    // done by the calling function
    if (list == VS_GEOMETRY_COLORS)
    {
        // Copy the data to every element in the list
        for (i = 0; i < dataListSize[list]; i++)
        {
            for (j = 0; j < 4; j++)
                dataList[list][i*4 + j] = data[j];
        }
    }

    // Update the pfGeoArrayData
    performerGeoarray->updateData();
}

// ------------------------------------------------------------------------
// Private function
// Emulates setting a per-primitive-bound data element on the normal or
// color list
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::setPerPrimitiveData(int list, int index, vsVector data)
{
    int baseIndex, i, j;
    int elementSize;

    // Figure out which list we're manipulating and determine the size of
    // each element of the list.
    if (list == VS_GEOMETRY_COLORS)
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
            printf("vsSkeletonMeshGeometry::setPerPrimitiveData:  Unrecognized "
                "primitive type\n");
    }

    // Update the pfGeoArrayData
    performerGeoarray->updateData();
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
// Retrieves the number of parents for this node
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
// Begins a new state/frame of the dynamic geometry.  Creates a new GeoArray
// and copies the current state into it.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::beginNewState()
{
    pfGeoArray *readGeoarray;
    vsDynamicDataList *readDynData, *writeDynData;
    unsigned int unit, list, lindex;
    int elementSize, copySize;
    bool listChanged[VS_GEOMETRY_LIST_COUNT];
    float *oldList[VS_GEOMETRY_LIST_COUNT];

    // Get the first writable flux buffer and cast it to a pfGeoArray
    performerGeoarray = (pfGeoArray *)performerFlux->getWritableData();

    // Get the geoarray that Performer is currently using for drawing
    // (this is for reading only)
    readGeoarray = (pfGeoArray *)performerFlux->getCurData();

    // Copy the primitive count, primitive type and geostate to the new
    // geoarray
    performerGeoarray->setNumPrims(readGeoarray->getNumPrims());
    performerGeoarray->setPrimType(readGeoarray->getPrimType());
    performerGeoarray->setGState(readGeoarray->getGState());

    // Assume that no list has changed at first
    memset(listChanged, 0, sizeof(listChanged));
    memset(oldList, 0, sizeof(oldList));

    // Copy each data list from the most recent update to the main set of
    // lists
    for (list = 0; list < VS_GEOMETRY_LIST_COUNT; list++)
    {
        // Get the set of dynamic data for this list from the most recently
        // committed state (this is for reading only)
        readDynData = (vsDynamicDataList *)dynamicData[list]->getCurData();

        // Get the new set of dynamic data that we'll be writing to
        writeDynData = 
            (vsDynamicDataList *)dynamicData[list]->getWritableData();

        // Compute the size of the list elements, based on which list
        // we're working on
        if (readDynData->dataIsGeneric)
            elementSize = 4;
        else if (list == VS_GEOMETRY_VERTEX_COORDS)
            elementSize = 3;
        else if (list == VS_GEOMETRY_NORMALS)
            elementSize = 3;
        else if ((list >= VS_GEOMETRY_TEXTURE0_COORDS) &&
                 (list <= VS_GEOMETRY_TEXTURE7_COORDS))
            elementSize = 2;
        else
            elementSize = 4;

        // Synchronize the previously committed state with the new state
        if (readDynData->dataListSize != writeDynData->dataListSize)
        {
            // The lists between last frame and this are different sizes.
            // Figure out how we need to adjust the list size
            if (readDynData->dataListSize == 0)
            {
                // The list was previously deleted, so we need to delete
                // our copy
                pfMemory::unrefDelete(writeDynData->dataList);
                writeDynData->dataList = NULL;
            }
            else if (writeDynData->dataListSize == 0)
            {
                // A new list was previously created, so we need to create
                // space for our copy
                writeDynData->dataList = (float *)
                    (pfMemory::malloc(readDynData->dataListSize *
                        elementSize * sizeof(float)));
                pfMemory::ref(writeDynData->dataList);
            }
            else
            {
                // Keep track of the old list, so we can properly unreference
                // and delete it later
                oldList[list] = writeDynData->dataList;

                // The list was resized, so we need to resize our copy to
                // match
                writeDynData->dataList = (float *)
                    (pfMemory::malloc(readDynData->dataListSize * elementSize * 
                        sizeof(float)));
                pfMemory::ref(writeDynData->dataList);
            }

            // Copy the new list size
            writeDynData->dataListSize = readDynData->dataListSize;

            // Lastly, mark that this list has changed, so we know to update
            // the local pfGeoArray with the new list pointer below
            listChanged[list] = true;
        }

        // Figure out how much data to copy from the old list
        if (writeDynData->dataListSize < readDynData->dataListSize)
            copySize = writeDynData->dataListSize * elementSize *
                sizeof(float);
        else
            copySize = readDynData->dataListSize * elementSize *
                sizeof(float);

        // Copy data from the previous frame's list to this frame's list, the
        // source of data depends on which list is being initialized
        if (list == VS_GEOMETRY_VERTEX_COORDS)
        {
            // The vertex list gets its initial data from the skin vertex list
            if (writeDynData->dataList != NULL)
                memcpy(writeDynData->dataList, originalVertexList,
                    copySize);
        }
        else if (list == VS_GEOMETRY_NORMALS)
        {
            // The normal list gets its initial data from the skin normal list
            if (writeDynData->dataList != NULL)
                memcpy(writeDynData->dataList, originalNormalList,
                    copySize);
        }
        else
        {
            // Copy the list data from the previous frame's list to the new
            // one
            if (writeDynData->dataList != NULL)
                memcpy(writeDynData->dataList, readDynData->dataList,
                    copySize);
        }

        // Copy the remaining metadata
        writeDynData->dataIsGeneric = readDynData->dataIsGeneric;
        writeDynData->dataBinding = readDynData->dataBinding;

        // Now copy the writable dynamic data into the main data members of
        // the object
        dataList[list] = writeDynData->dataList;
        dataListSize[list] = writeDynData->dataListSize;
        dataIsGeneric[list] = writeDynData->dataIsGeneric;
        dataBinding[list] = writeDynData->dataBinding;
    }

    // Query each vertex attribute from the currently active geoarray
    for (list = 0; list < VS_GEOMETRY_LIST_COUNT; list++)
    {
        // If the data is generic, we need to make sure to query a generic
        // attribute from the geoarray
        if (dataIsGeneric[list])
            lindex = list + VS_GEOMETRY_LIST_COUNT;
        else
            lindex = list;
            
        // Each list requires a different query type
        switch (lindex)
        {
            case VS_GEOMETRY_VERTEX_COORDS:
                dataAttr[list] = 
                    performerGeoarray->queryAttrType(PFGA_COORD_ARRAY);
                break;
            case VS_GEOMETRY_NORMALS:
                dataAttr[list] = 
                    performerGeoarray->queryAttrType(PFGA_NORMAL_ARRAY);
                break;
            case VS_GEOMETRY_COLORS:
                dataAttr[list] = 
                    performerGeoarray->queryAttrType(PFGA_COLOR_ARRAY);
                break;
            case VS_GEOMETRY_TEXTURE0_COORDS:
            case VS_GEOMETRY_TEXTURE1_COORDS:
            case VS_GEOMETRY_TEXTURE2_COORDS:
            case VS_GEOMETRY_TEXTURE3_COORDS:
            case VS_GEOMETRY_TEXTURE4_COORDS:
            case VS_GEOMETRY_TEXTURE5_COORDS:
            case VS_GEOMETRY_TEXTURE6_COORDS:
            case VS_GEOMETRY_TEXTURE7_COORDS:
                // Figure out the texture unit in question
                unit = list - VS_GEOMETRY_TEXTURE0_COORDS;

                // Query the unit
                dataAttr[list] = 
                    performerGeoarray->queryAttrType(PFGA_TEX_ARRAY, unit);
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
                dataAttr[list] = 
                    performerGeoarray->queryAttrType(PFGA_GENERIC_ARRAY, list);
                break;
        }

        // Now, fix up the discrepancies between the geoarray and the dynamic
        // data
        if ((dataAttr[list] != NULL) && (dataList[list] == NULL))
        {
            // We have a vertex attribute for this list, but there is no 
            // longer a list available to use
            performerGeoarray->removeAttr(dataAttr[list]);
            dataAttr[list] = NULL;
        }
        else if ((dataAttr[list] == NULL) && (dataList[list] != NULL))
        {
            // We don't have a vertex attribute for this list, but there is 
            // a new list available to use.  Set the new list on the geoarray,
            // and keep track of the new vertex attribute.  The details of
            // this operation depend on which list we're dealing with.
            switch (lindex)
            {
                case VS_GEOMETRY_VERTEX_COORDS:
                    dataAttr[list] = 
                        performerGeoarray->setAttr(PFGA_COORD_ARRAY, 3,
                            GL_FLOAT, 0, dataList[list]);
                    break;
                case VS_GEOMETRY_NORMALS:
                    dataAttr[list] = 
                        performerGeoarray->setAttr(PFGA_NORMAL_ARRAY, 3,
                            GL_FLOAT, 0, dataList[list]);
                    break;
                case VS_GEOMETRY_COLORS:
                    dataAttr[list] = 
                        performerGeoarray->setAttr(PFGA_COLOR_ARRAY, 4,
                            GL_FLOAT, 0, dataList[list]);
                    break;
                case VS_GEOMETRY_TEXTURE0_COORDS:
                case VS_GEOMETRY_TEXTURE1_COORDS:
                case VS_GEOMETRY_TEXTURE2_COORDS:
                case VS_GEOMETRY_TEXTURE3_COORDS:
                case VS_GEOMETRY_TEXTURE4_COORDS:
                case VS_GEOMETRY_TEXTURE5_COORDS:
                case VS_GEOMETRY_TEXTURE6_COORDS:
                case VS_GEOMETRY_TEXTURE7_COORDS:
                    // Figure out the texture unit in question
                    unit = list - VS_GEOMETRY_TEXTURE0_COORDS;
    
                    // Query the unit
                    dataAttr[list] = 
                        performerGeoarray->setMultiAttr(PFGA_TEX_ARRAY, unit, 
                            2, GL_FLOAT, 0, dataList[list]);
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
                    dataAttr[list] = 
                        performerGeoarray->setMultiAttr(PFGA_GENERIC_ARRAY, 
                            list, 4, GL_FLOAT, 0, dataList[list]);
                    break;
            }
        }
        else if (listChanged[list])
        {
            // The pfVertexAttr on the geoarray shouldn't have changed, but
            // we need to give the new list pointer to the geoarray
            dataAttr[list]->setPtr(dataList[list]);

            // Now, free up the old list
            pfMemory::unrefDelete(oldList[list]);
        }

        // Enable or disable the attribute, depending on the binding
        if (dataBinding[list] == VS_GEOMETRY_BIND_NONE)
            performerGeoarray->disableAttr(dataAttr[list]);
        else
            performerGeoarray->enableAttr(dataAttr[list]);
    }

    // Update the pfGeoArrayData
    performerGeoarray->updateData();
}

// ------------------------------------------------------------------------
// Finalizes the new dynamic geometry state.  This makes the state readable
// for rendering, and no longer writable.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::finishNewState()
{
    int list;
    vsDynamicDataList *dynData;

    // Copy the data from each list to the corresponding fluxed list
    for (list = 0; list < VS_GEOMETRY_LIST_COUNT; list++)
    {
        // Get the next dynamic data structure available for writing
        dynData = (vsDynamicDataList *)dynamicData[list]->getWritableData();

        // Copy the data from the master lists to the dynamic structure
        dynData->dataList = dataList[list];
        dynData->dataListSize = dataListSize[list];
        dynData->dataIsGeneric = dataIsGeneric[list];
        dynData->dataBinding = dataBinding[list];

        // Signal that we're done writing to this flux buffer
        dynamicData[list]->writeComplete();
    }

    // Signal the main pfFlux that all changes to the current pfGeoArray are 
    // complete
    performerFlux->writeComplete();
}

// ------------------------------------------------------------------------
// Sets the type of geometric primitive that this object contains
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::setPrimitiveType(int newType)
{
    // Translate the VESS primitive type to the performer counterpart
    // and set the Performer GeoArray to use it
    switch (newType)
    {
        case VS_GEOMETRY_TYPE_POINTS:
            performerGeoarray->setPrimType(PFGS_POINTS);
            primitiveType = PFGS_POINTS;
            break;
        case VS_GEOMETRY_TYPE_LINES:
            performerGeoarray->setPrimType(PFGS_LINES);
            primitiveType = PFGS_LINES;
            break;
        case VS_GEOMETRY_TYPE_LINE_STRIPS:
            performerGeoarray->setPrimType(PFGS_LINESTRIPS);
            primitiveType = PFGS_LINESTRIPS;
            break;
        case VS_GEOMETRY_TYPE_LINE_LOOPS:
            printf("vsSkeletonMeshGeometry::setPrimitiveType: "
                "VS_GEOMETRY_TYPE_LINE_LOOPS type not supported under "
                "Performer operation\n");
            performerGeoarray->setPrimType(PFGS_LINESTRIPS);
            primitiveType = PFGS_LINESTRIPS;
            break;
        case VS_GEOMETRY_TYPE_TRIS:
            performerGeoarray->setPrimType(PFGS_TRIS);
            primitiveType = PFGS_TRIS;
            break;
        case VS_GEOMETRY_TYPE_TRI_STRIPS:
            performerGeoarray->setPrimType(PFGS_TRISTRIPS);
            primitiveType = PFGS_TRISTRIPS;
            break;
        case VS_GEOMETRY_TYPE_TRI_FANS:
            performerGeoarray->setPrimType(PFGS_TRIFANS);
            primitiveType = PFGS_TRIFANS;
            break;
        case VS_GEOMETRY_TYPE_QUADS:
            performerGeoarray->setPrimType(PFGS_QUADS);
            primitiveType = PFGS_QUADS;
            break;
        case VS_GEOMETRY_TYPE_QUAD_STRIPS:
            printf("vsSkeletonMeshGeometry::setPrimitiveType: "
                "VS_GEOMETRY_TYPE_QUAD_STRIPS type not supported under"
                "Performer operation\n");
            performerGeoarray->setPrimType(PFGS_QUADS);
            primitiveType = PFGS_QUADS;
            break;
        case VS_GEOMETRY_TYPE_POLYS:
            performerGeoarray->setPrimType(PFGS_POLYS);
            primitiveType = PFGS_POLYS;
            break;
        default:
            printf("vsSkeletonMeshGeometry::setPrimitiveType: Unrecognized "
                "primitive type\n");
            break;
    }
}

// ------------------------------------------------------------------------
// Retrieves the type of geometric primitive that this object contains
// ------------------------------------------------------------------------
int vsSkeletonMeshGeometry::getPrimitiveType()
{
    // Translate the Performer primitive type to the VESS counterpart
    // and return it.  Return -1 if the current type is invalid.
    switch (primitiveType)
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
void vsSkeletonMeshGeometry::setPrimitiveCount(int newCount)
{
    // Set the primitive count on the pfGeoArray to the new value
    performerGeoarray->setNumPrims(newCount);

    // Remember the new count internally as well
    primitiveCount = newCount;
    
    // If the geometry's particular primitive type doesn't require a
    // primitive lengths array, don't bother updating it.
    if ((primitiveType == VS_GEOMETRY_TYPE_POINTS) ||
        (primitiveType == VS_GEOMETRY_TYPE_LINES) ||
        (primitiveType == VS_GEOMETRY_TYPE_TRIS) ||
        (primitiveType == VS_GEOMETRY_TYPE_QUADS))
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
    // Validate the index parameter
    if ((index < 0) || (index >= primitiveCount))
    {
        printf("vsSkeletonMeshGeometry::setPrimitiveLength: Index out of "
            "bounds\n");
        return;
    }
    
    // If the geometry's particular primitive type doesn't require a
    // primitive lengths array, don't set anything.
    if ((primitiveType == VS_GEOMETRY_TYPE_POINTS) ||
        (primitiveType == VS_GEOMETRY_TYPE_LINES) ||
        (primitiveType == VS_GEOMETRY_TYPE_TRIS) ||
        (primitiveType == VS_GEOMETRY_TYPE_QUADS))
        return;

    // Change the appropriate length
    lengthsList[index] = length;
}

// ------------------------------------------------------------------------
// Retrieves the number of verticies specified for the primitive with the
// indicated index in the object. The index of the first primitive is 0.
// ------------------------------------------------------------------------
int vsSkeletonMeshGeometry::getPrimitiveLength(int index)
{
    // Validate the index parameter
    if ((index < 0) || (index >= primitiveCount))
    {
        printf("vsSkeletonMeshGeometry::getPrimitiveLength: Index out of "
            "bounds\n");
        return -1;
    }
    
    // If the geometry's particular primitive type doesn't require a
    // primitive lengths array, return a pre-packaged value.
    if (primitiveType == VS_GEOMETRY_TYPE_POINTS)
        return 1;
    if (primitiveType == VS_GEOMETRY_TYPE_LINES)
        return 2;
    if (primitiveType == VS_GEOMETRY_TYPE_TRIS)
        return 3;
    if (primitiveType == VS_GEOMETRY_TYPE_QUADS)
        return 4;

    // Return the given primitive length
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
    
    // If the geometry's particular primitive type doesn't require a
    // primitive lengths array, don't set anything.
    if ((primitiveType == VS_GEOMETRY_TYPE_POINTS) ||
        (primitiveType == VS_GEOMETRY_TYPE_LINES) ||
        (primitiveType == VS_GEOMETRY_TYPE_TRIS) ||
        (primitiveType == VS_GEOMETRY_TYPE_QUADS))
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
void vsSkeletonMeshGeometry::getPrimitiveLengths(int *lengthsBuffer)
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
        switch (primitiveType)
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
void vsSkeletonMeshGeometry::setBinding(int whichData, int binding)
{
    unsigned int unit;
    int list;

    // Figure out which list is being modified.
    if ((whichData == VS_GEOMETRY_SKIN_VERTEX_COORDS) ||
        (whichData == VS_GEOMETRY_SKIN_NORMALS))
    {
        // Skin data can only be per vertex.
        if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
        {
            printf("vsSkeletonMeshGeometry::setBinding: Skin attribute "
                "binding must always be VS_GEOMETRY_BIND_PER_VERTEX\n");
        }

        // Nothing more to do
        return;
    }
    else if (whichData >= VS_GEOMETRY_LIST_COUNT)
    {  
        // This is a generic list, we need to translate the index
        list = whichData - VS_GEOMETRY_LIST_COUNT;

        // Check the corresponding conventional attribute list to make sure
        // it's not being used
        if ((dataListSize[list] > 0) && (!dataIsGeneric[list]))
        {
            printf("vsSkeletonMeshGeometry::setBinding:  Cannot modify binding"
                 " on generic attribute type %d when\n", list);
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
        case VS_GEOMETRY_VERTEX_WEIGHTS:

            // Generic attribute bindings can only be per vertex.
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsSkeletonMeshGeometry::setBinding: Vertex weight "
                    "binding must always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }
            break;

        case VS_GEOMETRY_BONE_INDICES:

            // Generic attribute bindings can only be per vertex.
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsSkeletonMeshGeometry::setBinding: Bone indices "
                    "binding must always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }
            break;

        case VS_GEOMETRY_VERTEX_COORDS:

            // Vertex coordinate binding must be per-vertex (no other
            // binding makes sense)
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsSkeletonMeshGeometry::setBinding: Vertex coordinate "
                    "binding must always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }

            // Set the vertex coordinate binding to the given value
            if ((dataBinding[list] != VS_GEOMETRY_BIND_PER_VERTEX) &&
                (dataList[list] != NULL))
            {
                performerGeoarray->enableAttr(dataAttr[list]);
                dataBinding[list] = VS_GEOMETRY_BIND_PER_VERTEX;
            }
            break;

        case VS_GEOMETRY_NORMALS:

            // Normal binding must be per-vertex
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsSkeletonMeshGeometry::setBinding: Normal "
                    "binding must always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }

            // Set the normal binding to the given value
            if ((dataBinding[list] != VS_GEOMETRY_BIND_PER_VERTEX) &&
                (dataList[list] != NULL))
            {
                performerGeoarray->enableAttr(dataAttr[list]);
                dataBinding[list] = VS_GEOMETRY_BIND_PER_VERTEX;
            }
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
                printf("vsSkeletonMeshGeometry::setBinding: Alternate color"
                    "binding must be either\n");
                printf("    VS_GEOMETRY_BIND_PER_VERTEX or "
                    "VS_GEOMETRY_BIND_NONE\n");
                return;
            }

            // Set alternate color binding on the geoarray
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
                printf("vsSkeletonMeshGeometry::setBinding: Fog coordinates"
                    " binding must be either\n");
                printf("    VS_GEOMETRY_BIND_PER_VERTEX or "
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
            // We pass user data down as a generic attribute.  Binding
            // should be either none or per-vertex
            if ((binding != VS_GEOMETRY_BIND_PER_VERTEX) &&
                (binding != VS_GEOMETRY_BIND_NONE))
            {
                printf("vsSkeletonMeshGeometry::setBinding: User data binding "
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
            // Calculate the texture unit we are working with.
            unit = whichData - VS_GEOMETRY_TEXTURE0_COORDS;

            // Texture coordinate binding must be none or per-vertex
            // (no other binding makes sense).
            if ((binding != VS_GEOMETRY_BIND_PER_VERTEX) &&
                (binding != VS_GEOMETRY_BIND_NONE))
            {
                printf("vsSkeletonMeshGeometry::setBinding: Texture "
                    "coordinates binding must be either "
                    "VS_GEOMETRY_BIND_PER_VERTEX or VS_GEOMETRY_BIND_NONE\n");
                return;
            }

            // Set the texture coordinate binding to the given value
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
                    printf("vsSkeletonMeshGeometry::setBinding: Generic "
                        "attribute binding must be either\n");
                    printf("    VS_GEOMETRY_BIND_PER_VERTEX "
                        "or VS_GEOMETRY_BIND_NONE\n");
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
    int list;

    // Figure out which list is required
    if (whichData == VS_GEOMETRY_COLORS)
    {
        // Return the emulated color binding, instead of the actual list
        // binding
        return colorBinding;
    }
    else if ((whichData == VS_GEOMETRY_SKIN_VERTEX_COORDS) ||
             (whichData == VS_GEOMETRY_SKIN_NORMALS))
    {
        // These lists are always specified per-vertex
        return VS_GEOMETRY_BIND_PER_VERTEX;
    }
    else if (whichData >= VS_GEOMETRY_LIST_COUNT)
    {
        // Translate the generic attribute to a working list index
        list = whichData - VS_GEOMETRY_LIST_COUNT;
    }
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
void vsSkeletonMeshGeometry::setData(int whichData, int dataIndex,
                                     vsVector data)
{
    int loop, list;
    int listSize;

    // Validate the data and parameters.  The procedure for this is different
    // based on the list that's being changed.
    if ((whichData == VS_GEOMETRY_SKIN_VERTEX_COORDS) ||
        (whichData == VS_GEOMETRY_SKIN_NORMALS))
    {
        // Remember the list index
        list = whichData;

        // For the skin attribute lists, check the index agains the vertex
        // list size
        if ((dataIndex < 0) || 
            (dataIndex >= dataListSize[VS_GEOMETRY_VERTEX_COORDS]))
        {
            printf("vsSkeletonMeshGeometry::setData: Index out of bounds\n");
            return;
        }
    }
    else
    {
        // Figure out which list we're changing
        if (whichData >= VS_GEOMETRY_LIST_COUNT)
            list = whichData - VS_GEOMETRY_LIST_COUNT;
        else
            list = whichData;

        // Bounds check.  First get the size of the list we're working with.
        // If we're working with colors we need to check against the
        // internal list size instead of the pfGeoArray list size.
        if (whichData == VS_GEOMETRY_COLORS)
            listSize = colorListSize;
        else
            listSize = dataListSize[list];

        // Now check the given index against the size of the list
        if ((dataIndex < 0) || (dataIndex >= listSize))
        {
            printf("vsSkeletonMeshGeometry::setData: Index out of bounds\n");
            printf("   list = %d, size = %d,  index = %d\n", list,
                listSize, dataIndex);
            return;
        }

        // Make sure we don't trample over a list that's in use by trying to
        // alter it's corresponding overlapping list
        if ((dataIsGeneric[list]) && (list == whichData))
        {
            printf("vsSkeletonMeshGeometry::setData:  Cannot modify data on "
                 "conventional attribute type %d when\n", list);
            printf("    corresponding generic attribute type is in use.\n");
            return;
        }
        else if ((!dataIsGeneric[list]) && (list != whichData))
        {
            printf("vsSkeletonMeshGeometry::setData:  Cannot modify data on"
                 " generic attribute type %d when\n", list);
            printf("    corresponding conventional attribute type is in "
                 "use.\n");
            return;
        }
    }

    // Different actions necessary depending on which data is being set
    switch (whichData)
    {
        case VS_GEOMETRY_SKIN_VERTEX_COORDS:
            // Vertex coordinates require a 3-component vector
            if (data.getSize() < 3)
            {
                printf("vsSkeletonMeshGeometry::setData: Insufficient data "
                    "(vertex coordinates require 3 values)\n");
                return;
            }

            // Copy the data from the vector into the vertex list
            for (loop = 0; loop < 3; loop++)
            {
                originalVertexList[dataIndex*3 + loop] = data[loop];
                dataList[VS_GEOMETRY_VERTEX_COORDS][dataIndex*3 + loop] =
                    data[loop];
            }
            break;

        case VS_GEOMETRY_SKIN_NORMALS:
            // Normals require a 3-component vector
            if (data.getSize() < 3)
            {
                printf("vsSkeletonMeshGeometry::setData: Insufficient data "
                    "(vertex normals require 3 values)\n");
                return;
            }

            // Copy the data from the vector into the normal list
            for (loop = 0; loop < 3; loop++)
            {
                originalNormalList[dataIndex*3 + loop] = data[loop];
                dataList[VS_GEOMETRY_NORMALS][dataIndex*3 + loop] = data[loop];
            }
            break;

        case VS_GEOMETRY_VERTEX_COORDS:
            printf("vsSkeletonMeshGeometry::setData: Cannot set vertex coords "
                   "  as they are generated based on bone positions.\n");
            printf("  Use VS_GEOMETRY_SKIN_VERTEX_COORDS instead.\n");
            break;

        case VS_GEOMETRY_NORMALS:
            printf("vsSkeletonMeshGeometry::setData: Cannot set normals "
                   "  as they are generated based on bone positions.\n");
            printf("  Use VS_GEOMETRY_SKIN_NORMALS instead.\n");
            break;

        case VS_GEOMETRY_COLORS:
            // Colors require a 4-component vector
            if (data.getSize() < 4)
            {
                printf("vsSkeletonMeshGeometry::setData: Insufficient data "
                    "(colors require 4 values)\n");
                return;
            }

            // Copy the data from the vector into the color list
            for (loop = 0; loop < 4; loop++)
                colorList[dataIndex*4 + loop] = data[loop];

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
            // Texture coordinates require a 2-component vector
            if (data.getSize() < 2)
            {
                printf("vsSkeletonMeshGeometry::setData: Insufficient data "
                    "(texture coordinates require 2 values)\n");
                return;
            }

            // Copy the data from the vector into the texture coordinate list
            for (loop = 0; loop < 2; loop++)
                dataList[list][dataIndex*2 + loop] = data[loop];
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
        case VS_GEOMETRY_ALT_COLORS:
        case VS_GEOMETRY_FOG_COORDS:
        case VS_GEOMETRY_USER_DATA0:
        case VS_GEOMETRY_BONE_INDICES:
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
            printf("vsSkeletonMeshGeometry::setData: Unrecognized data type\n");
            return;
    }

    // Update the pfGeoArrayData
    performerGeoarray->updateData();
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
    int list;
    int listSize;
    
    // Validate the data and parameters.  The procedure for this is different
    // based on the list that's being queried.
    if ((whichData == VS_GEOMETRY_SKIN_VERTEX_COORDS) ||
        (whichData == VS_GEOMETRY_SKIN_NORMALS))
    {
        // Remember the list index
        list = whichData;

        // For the skin attribute lists, check the index agains the vertex
        // list size
        if ((dataIndex < 0) || 
            (dataIndex >= dataListSize[VS_GEOMETRY_VERTEX_COORDS]))
        {
            printf("vsSkeletonMeshGeometry::getData: Index out of bounds\n");
            return result;
        }
    }
    else
    {
        // Figure out which list we're changing
        if (whichData >= VS_GEOMETRY_LIST_COUNT)
            list = whichData - VS_GEOMETRY_LIST_COUNT;
        else
            list = whichData;

        // Bounds check.  First get the size of the list we're working with.
        // If we're working with colors we need to check against the
        // internal list size instead of the pfGeoArray list size.
        if (whichData == VS_GEOMETRY_COLORS)
            listSize = colorListSize;
        else
            listSize = dataListSize[list];

        // Now check the given index against the size of the list
        if ((dataIndex < 0) || (dataIndex >= listSize))
        {
            printf("vsSkeletonMeshGeometry::getData: Index out of bounds\n");
            return result;
        }

        // Make sure we don't trample over a list that's in use by trying to
        // alter it's corresponding overlapping list
        if ((dataIsGeneric[list]) && (list == whichData))
        {
            printf("vsSkeletonMeshGeometry::getData:  Cannot query data on "
                 "conventional attribute type %d when\n", list);
            printf("    corresponding generic attribute type is in use.\n");
            return result;
        }
        else if ((!dataIsGeneric[list]) && (list != whichData))
        {
            printf("vsSkeletonMeshGeometry::getData:  Cannot query data on"
                 " generic attribute type %d when\n", list);
            printf("    corresponding conventional attribute type is in "
                 "use.\n");
            return result;
        }
    }

    // Determine which list we should obtain the data from, and return
    // the requested item from that list
    switch (whichData)
    {
        case VS_GEOMETRY_SKIN_VERTEX_COORDS:
            // Copy the data to the result vector
            result.setSize(3);
            for (loop = 0; loop < 3; loop++)
                result[loop] = originalVertexList[dataIndex*3 + loop];
            break;

        case VS_GEOMETRY_SKIN_NORMALS:
            // Copy the normal in question
            result.setSize(3);
            for (loop = 0; loop < 3; loop++)
                result[loop] = originalNormalList[dataIndex*3 + loop];
            break;

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
                result[loop] = dataList[list][dataIndex*3 + loop];
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
        case VS_GEOMETRY_BONE_INDICES:
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
            printf("vsSkeletonMeshGeometry::getData: Unrecognized data type "
                "(%d)\n", whichData);
            return result;
    }
    
    // Return the vector copied from the requested list and index
    return result;
}

// ------------------------------------------------------------------------
// Sets all of the data points within one of the geometry objects' lists
// to the values in dataList. The dataList array must be at least as large
// as the size of particular list in question.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::setDataList(int whichData, vsVector *newDataList)
{
    int loop, sloop;
    int list;

    // Validate the data and parameters.  The procedure for this is different
    // based on the list that's being changed.
    if ((whichData == VS_GEOMETRY_SKIN_VERTEX_COORDS) ||
        (whichData == VS_GEOMETRY_SKIN_NORMALS))
    {
        // Remember the list index
        list = whichData;
    }
    else
    {
        // Figure out which list we're changing
        if (whichData >= VS_GEOMETRY_LIST_COUNT)
            list = whichData - VS_GEOMETRY_LIST_COUNT;
        else
            list = whichData;

        // Make sure we don't trample over a list that's in use by trying to
        // alter it's corresponding overlapping list
        if ((dataIsGeneric[list]) && (list == whichData))
        {
            printf("vsSkeletonMeshGeometry::setDataList:  Cannot modify data "
                 "on conventional attribute type %d when\n", list);
            printf("    corresponding generic attribute type is in use.\n");
            return;
        }
        else if ((!dataIsGeneric[list]) && (list != whichData))
        {
            printf("vsSkeletonMeshGeometry::setDataList:  Cannot modify data "
                 "on generic attribute type %d when\n", list);
            printf("    corresponding conventional attribute type is in "
                 "use.\n");
            return;
        }
    }

    // Copy the entire data list given to the appropriate geometry data list
    switch (whichData)
    {
        case VS_GEOMETRY_SKIN_VERTEX_COORDS:
            for (loop = 0; 
                 loop < dataListSize[VS_GEOMETRY_VERTEX_COORDS]; 
                 loop++)
                for (sloop = 0; sloop < 3; sloop++)
                {
                    originalVertexList[loop*3 + sloop] =
                        newDataList[loop][sloop];
                    dataList[list][loop*3 + sloop] = newDataList[loop][sloop];
                }
            break;

        case VS_GEOMETRY_SKIN_NORMALS:
            for (loop = 0; loop < dataListSize[VS_GEOMETRY_NORMALS]; loop++)
                for (sloop = 0; sloop < 3; sloop++)
                {
                    originalNormalList[loop*3 + sloop] =
                        newDataList[loop][sloop];
                    dataList[list][loop*3 + sloop] = newDataList[loop][sloop];
                }
            break;

        case VS_GEOMETRY_VERTEX_COORDS:
            printf("vsSkeletonMeshGeometry::setData: Cannot set vertex "
                   "coords, as they are generated\n");
            printf("  based on bone positions.\n");
            printf("  Use VS_GEOMETRY_SKIN_VERTEX_COORDS instead.\n");
            break;

        case VS_GEOMETRY_NORMALS:
            printf("vsSkeletonMeshGeometry::setData: Cannot set normals, "
                   "as they are generated\n");
            printf("  based on bone positions.\n");
            printf("  Use VS_GEOMETRY_SKIN_NORMALS instead.\n");
            break;

        case VS_GEOMETRY_COLORS:
            for (loop = 0; loop < colorListSize; loop++)
                for (sloop = 0; sloop < 4; sloop++)
                    colorList[loop*3 + sloop] = newDataList[loop][sloop];

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
        case VS_GEOMETRY_BONE_INDICES:
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
            printf("vsSkeletonMeshGeometry::setDataList: Unrecognized data "
                "type\n");
            return;
    }

    // Update the pfGeoArrayData
    performerGeoarray->updateData();
}

// ------------------------------------------------------------------------
// Retrieves all of the data points within one of the geometry objects'
// lists, storing that data in the specified dataBuffer. The dataBuffer
// array must be at least as large as the size of particular list in
// question.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::getDataList(int whichData, vsVector *dataBuffer)
{
    int list;
    int loop, sloop;
    
    // Validate the data and parameters.  The procedure for this is different
    // based on the list that's being queried.
    if ((whichData == VS_GEOMETRY_SKIN_VERTEX_COORDS) ||
        (whichData == VS_GEOMETRY_SKIN_NORMALS))
    {
        // Remember the list index
        list = whichData;
    }
    else
    {
        // Figure out which list we're changing
        if (whichData >= VS_GEOMETRY_LIST_COUNT)
            list = whichData - VS_GEOMETRY_LIST_COUNT;
        else
            list = whichData;

        // Make sure we don't trample over a list that's in use by trying to
        // alter it's corresponding overlapping list
        if ((dataIsGeneric[list]) && (list == whichData))
        {
            printf("vsSkeletonMeshGeometry::getDataList:  Cannot query data "
                 "on conventional attribute type %d when\n", list);
            printf("    corresponding generic attribute type is in use.\n");
            return;
        }
        else if ((!dataIsGeneric[list]) && (list != whichData))
        {
            printf("vsSkeletonMeshGeometry::getDataList:  Cannot query data "
                 "on generic attribute type %d when\n", list);
            printf("    corresponding conventional attribute type is in "
                 "use.\n");
            return;
        }
    }

    // Interpret the whichData constant and copy the appropriate data list
    // to the given data buffer
    switch (whichData)
    {
        case VS_GEOMETRY_SKIN_VERTEX_COORDS:
            for (loop = 0;
                 loop < dataListSize[VS_GEOMETRY_VERTEX_COORDS];
                 loop++)
            {
                // Copy the data to the vector buffer
                dataBuffer[loop].setSize(3);
                for (sloop = 0; sloop < 3; sloop++)
                    dataBuffer[loop][sloop] =
                        originalVertexList[loop*3 + sloop];
            }
            break;

        case VS_GEOMETRY_SKIN_NORMALS:
            for (loop = 0; loop < dataListSize[VS_GEOMETRY_NORMALS]; loop++)
            {
                // Copy the data to the vector buffer
                dataBuffer[loop].setSize(3);
                for (sloop = 0; sloop < 3; sloop++)
                    dataBuffer[loop][sloop] =
                        originalNormalList[loop*3 + sloop];
            }
            break;

        case VS_GEOMETRY_VERTEX_COORDS:
        case VS_GEOMETRY_NORMALS:
            for (loop = 0; loop < dataListSize[list]; loop++)
            {
                // Copy the data to the vector buffer
                dataBuffer[loop].setSize(3);
                for (sloop = 0; sloop < 3; sloop++)
                    dataBuffer[loop][sloop] = dataList[list][loop*3 + sloop];
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
        case VS_GEOMETRY_BONE_INDICES:
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
    int list, unit;
    int copySize;
    float *tempList;

    // If we're changing a skin attribute, we need to do our general list
    // validations in a special way
    if ((whichData == VS_GEOMETRY_SKIN_VERTEX_COORDS) ||
        (whichData == VS_GEOMETRY_SKIN_NORMALS))
    {
        // If we're resizing the list to the same size, there's not much
        // to do
        if ((whichData == VS_GEOMETRY_SKIN_VERTEX_COORDS) &&
            (newSize == dataListSize[VS_GEOMETRY_VERTEX_COORDS]))
            return;
        else if ((whichData == VS_GEOMETRY_SKIN_NORMALS) &&
                 (newSize == dataListSize[VS_GEOMETRY_NORMALS]))
            return;
    }
    else
    {
        // Figure out which list we're changing
        if (whichData >= VS_GEOMETRY_LIST_COUNT)
            list = whichData - VS_GEOMETRY_LIST_COUNT;
        else
            list = whichData;

        // Certain lists (vertices, normals, weights, and bone indices)
        // cannot be made generic in a skeleton mesh geometry, or the
        // skinning process will fail.  Check for this problem here
        if ((whichData == VS_GEOMETRY_GENERIC_0) ||
            (whichData == VS_GEOMETRY_GENERIC_1) ||
            (whichData == VS_GEOMETRY_GENERIC_2) ||
            (whichData == VS_GEOMETRY_GENERIC_7))
        {
            printf("vsSkeletonMeshGeometry::setDataListSize:  Cannot enable"
                "generic attribute %d.\n", list);
            printf("     The corresponding conventional attribute is "
                "essential\n");
            printf("     for the skinning process.\n");
            return;
        }

        // Check to see if the overlapping generic or conventional list is
        // already in use before we change this one
        if ((list == whichData) && (dataIsGeneric[list]) && 
            (dataListSize[list] > 0))
        {
            printf("vsSkeletonMeshGeometry::setDataListSize:  Cannot resize "
                 "conventional attribute list %d when the\n", list);
            printf("    corresponding generic attribute list is in use.\n");
            printf("    Resize the corresponding list to 0 first.\n");
            return;
        }
        else if ((list != whichData) && (!dataIsGeneric[list]) &&
                 (dataListSize[list] > 0))
        {
            printf("vsSkeletonMeshGeometry::setDataListSize:  Cannot resize "
                "generic attribute list %d when the\n", list);
            printf("    corresponding conventional attribute list is in "
                "use.\n");
            printf("    Resize the corresponding list to 0 first.\n");
            return;
        }

        // If we're resizing the list to the same size, there's not much to
        // do.  Just make sure we compare against the internal list size
        // for colors.
        if (list == VS_GEOMETRY_COLORS) 
        {
            if (colorListSize == newSize)
                return;
        }
        else if (dataListSize[list] == newSize)
            return;
    }

    // If we get this far, we're correctly modifying the requested list.
    // First, set the "is generic" flag on the list to the correct value.
    if (list == whichData)
        dataIsGeneric[list] = false;
    else
        dataIsGeneric[list] = true;

    // Interpret the whichData constant
    switch (whichData)
    {
        case VS_GEOMETRY_SKIN_VERTEX_COORDS:
        case VS_GEOMETRY_VERTEX_COORDS:
            // Check the current vertex list size and the requested new 
            // size, and reallocate the list as appropriate
            if (newSize && !dataList[VS_GEOMETRY_VERTEX_COORDS])
            {
                // No list exists, create a new list
                dataList[VS_GEOMETRY_VERTEX_COORDS] =
                    (float *)(pfMemory::malloc(sizeof(float) * 3 * newSize));
                pfMemory::ref(dataList[VS_GEOMETRY_VERTEX_COORDS]);

                // Make sure the skin vertex list matches the rendering list
                if (originalVertexList == NULL)
                {
                    originalVertexList = (float *)(pfMemory::malloc(
                        sizeof(float) * 3 * newSize));
                    pfMemory::ref(originalVertexList);
                }
                else
                {
                    originalVertexList = (float *)(pfMemory::realloc(
                        originalVertexList, sizeof(float) * 3 * newSize));
                }
                                                                                
                // Set the newly-created vertex list on the pfGeoArray, if it
                // is currently bound
                dataAttr[VS_GEOMETRY_VERTEX_COORDS] =
                    performerGeoarray->setAttr(PFGA_COORD_ARRAY, 3, GL_FLOAT,
                        0, dataList[VS_GEOMETRY_VERTEX_COORDS]);

                // Automatically bind the list as per-vertex
                performerGeoarray->
                    enableAttr(dataAttr[VS_GEOMETRY_VERTEX_COORDS]);
                dataBinding[VS_GEOMETRY_VERTEX_COORDS] =
                    VS_GEOMETRY_BIND_PER_VERTEX;
            }
            else if (!newSize && dataList[VS_GEOMETRY_VERTEX_COORDS])
            {
                // List exists, but the requested new size is zero, so
                // delete the existing list
                if (dataAttr[VS_GEOMETRY_VERTEX_COORDS])
                {
                    performerGeoarray->
                        removeAttr(dataAttr[VS_GEOMETRY_VERTEX_COORDS]);
                    dataAttr[VS_GEOMETRY_VERTEX_COORDS] = NULL;
                }

                // Delete the existing list
                pfMemory::unrefDelete(dataList[VS_GEOMETRY_VERTEX_COORDS]);
                dataList[VS_GEOMETRY_VERTEX_COORDS] = NULL;

                // Make sure the skin vertex list matches the rendering list
                if (originalVertexList != NULL)
                {
                    pfMemory::unrefDelete(originalVertexList);
                    originalVertexList = NULL;
                }

                // To prevent confusion, unbind the list when it's deleted
                dataBinding[VS_GEOMETRY_VERTEX_COORDS] =
                    VS_GEOMETRY_BIND_NONE;
            }
            else
            {
                // Modify the length of the existing list
                tempList = dataList[VS_GEOMETRY_VERTEX_COORDS];
                dataList[VS_GEOMETRY_VERTEX_COORDS] =
                    (float *)(pfMemory::malloc(sizeof(float) * 3 * newSize));
                pfMemory::ref(dataList[VS_GEOMETRY_VERTEX_COORDS]);

                // Figure out how much data to copy from the old list
                if (newSize < dataListSize[VS_GEOMETRY_VERTEX_COORDS])
                    copySize = newSize * 3 * sizeof(float);
                else
                    copySize = dataListSize[VS_GEOMETRY_VERTEX_COORDS] * 3 *
                        sizeof(float);

                // Copy the data from the old list
                memcpy(dataList[VS_GEOMETRY_VERTEX_COORDS], tempList,
                    copySize);

                // Update the pfGeoArray data
                dataAttr[VS_GEOMETRY_VERTEX_COORDS]->
                    setPtr(dataList[VS_GEOMETRY_VERTEX_COORDS]);
                performerGeoarray->updateData();

                // Free the old list
                pfMemory::unrefDelete(tempList);

                // Also resize the skin vertex list
                if (originalVertexList == NULL)
                {
                    originalVertexList = (float *)(pfMemory::malloc(
                        sizeof(float) * 3 * newSize));
                    pfMemory::ref(originalVertexList);
                }
                else
                {
                    originalVertexList = (float *)(pfMemory::realloc(
                        originalVertexList, sizeof(float) * 3 * newSize));
                }
            }

            // Store the new list size
            dataListSize[VS_GEOMETRY_VERTEX_COORDS] = newSize;

            // Since we've changed the number of vertices in the geometry,
            // we may need to reconvert the color array (if its binding is
            // not per-vertex)
            convertToPerVertex(VS_GEOMETRY_COLORS);
            break;

        case VS_GEOMETRY_SKIN_NORMALS:
        case VS_GEOMETRY_NORMALS:
            // Check the current vertex list size and the requested new
            // size, and reallocate the list as appropriate
            if (newSize && !dataList[VS_GEOMETRY_NORMALS])
            {
                // No list exists, create a new list
                dataList[VS_GEOMETRY_NORMALS] = (float *)(pfMemory::malloc(
                    sizeof(float) * 3 * newSize));
                pfMemory::ref(dataList[VS_GEOMETRY_NORMALS]);

                // Make sure the skin vertex list matches the rendering list
                if (originalNormalList == NULL)
                {
                    originalNormalList = (float *)(pfMemory::malloc(
                        sizeof(float) * 3 * newSize));
                    pfMemory::ref(originalNormalList);
                }
                else
                {
                    originalNormalList = (float *)(pfMemory::realloc(
                        originalNormalList, sizeof(float) * 3 * newSize));
                }

                // Set the newly-created vertex list on the pfGeoArray, if it
                // is currently bound
                dataAttr[VS_GEOMETRY_NORMALS] =
                    performerGeoarray->setAttr(PFGA_NORMAL_ARRAY, 3, GL_FLOAT,
                        0, dataList[VS_GEOMETRY_NORMALS]);

                // Automatically bind the list as per-vertex
                performerGeoarray->enableAttr(dataAttr[VS_GEOMETRY_NORMALS]);
                dataBinding[VS_GEOMETRY_NORMALS] = VS_GEOMETRY_BIND_PER_VERTEX;
            }
            else if (!newSize && dataList[VS_GEOMETRY_NORMALS])
            {
                // List exists, but the requested new size is zero, so
                // remove the existing list
                if (dataAttr[VS_GEOMETRY_NORMALS])
                {
                    performerGeoarray->
                        removeAttr(dataAttr[VS_GEOMETRY_NORMALS]);
                    dataAttr[VS_GEOMETRY_NORMALS] = NULL;
                }

                // Delete the existing list
                pfMemory::unrefDelete(dataList[VS_GEOMETRY_NORMALS]);
                dataList[VS_GEOMETRY_NORMALS] = NULL;

                // Make sure the skin normal list matches the rendering list
                if (originalNormalList != NULL)
                {
                    pfMemory::unrefDelete(originalNormalList);
                    originalNormalList = NULL;
                }

                // To prevent confusion, unbind the list when it's deleted
                dataBinding[VS_GEOMETRY_NORMALS] = VS_GEOMETRY_BIND_NONE;
            }
            else
            {
                // Modify the length of the existing list
                tempList = dataList[VS_GEOMETRY_NORMALS];
                dataList[VS_GEOMETRY_NORMALS] =
                    (float *)(pfMemory::malloc(sizeof(float) * 3 * newSize));
                pfMemory::ref(dataList[VS_GEOMETRY_NORMALS]);

                // Figure out how much data to copy from the old list
                if (newSize < dataListSize[VS_GEOMETRY_NORMALS])
                    copySize = newSize * 3 * sizeof(float);
                else
                    copySize = dataListSize[VS_GEOMETRY_NORMALS] * 3 *
                        sizeof(float);

                // Copy the data from the old list
                memcpy(dataList[VS_GEOMETRY_NORMALS], tempList,
                    copySize);

                // Update the pfGeoArray data
                dataAttr[VS_GEOMETRY_NORMALS]->
                    setPtr(dataList[VS_GEOMETRY_NORMALS]);
                performerGeoarray->updateData();

                // Free the old list
                pfMemory::unrefDelete(tempList);

                // Also resize the skin vertex list
                if (originalNormalList == NULL)
                {
                    originalNormalList = (float *)(pfMemory::malloc(
                        sizeof(float) * 3 * newSize));
                    pfMemory::ref(originalNormalList);
                }
                else
                {
                    originalNormalList = (float *)(pfMemory::realloc(
                        originalNormalList, sizeof(float) * 3 * newSize));
                }
            }

            // Store the new list size
            dataListSize[VS_GEOMETRY_NORMALS] = newSize;
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
        case VS_GEOMETRY_BONE_INDICES:
            // Determine what we need to do with the data list
            // based on whether or not it currently exists, and
            // the desired new size of the list
            if (newSize && !dataList[list])
            {
                // No list exists, create a new generic list
                dataList[list] = (float *)(pfMemory::malloc(
                    sizeof(float) * 4 * newSize));
                pfMemory::ref(dataList[list]);
                                                                                
                // Set the newly-created attribute list on the pfGeoArray
                dataAttr[list] = performerGeoarray->
                    setMultiAttr(PFGA_GENERIC_ARRAY, list, 4, GL_FLOAT, 
                        0, dataList[list]);

                // Automatically bind the new list
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
                    dataBinding[list] = VS_GEOMETRY_BIND_NONE;
                }

                // Delete the existing generic list
                performerGeoarray->setMultiAttr(PFGA_GENERIC_ARRAY, list, 4, 
                    GL_FLOAT, 0, NULL);
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



        case VS_GEOMETRY_ALT_COLORS:
        case VS_GEOMETRY_FOG_COORDS:
        case VS_GEOMETRY_USER_DATA0:
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

            printf("vsSkeletonMeshGeometry::setDataListSize: Unrecognized "
                "data value\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves the size of one of the object's data lists
// ------------------------------------------------------------------------
int vsSkeletonMeshGeometry::getDataListSize(int whichData)
{
    unsigned int unit;
    int list;

    // Interpret the whichData constant and filter out the special cases first
    if (whichData == VS_GEOMETRY_SKIN_VERTEX_COORDS)
        return dataListSize[VS_GEOMETRY_VERTEX_COORDS];
    else if (whichData == VS_GEOMETRY_SKIN_NORMALS)
        return dataListSize[VS_GEOMETRY_NORMALS];
    else if (whichData == VS_GEOMETRY_COLORS)
        return colorListSize;
    else if (whichData >= VS_GEOMETRY_LIST_COUNT)
        list = whichData - VS_GEOMETRY_LIST_COUNT;
    else
        list = whichData;

    // Range check the list index
    if ((list < 0) || (list >= VS_GEOMETRY_LIST_COUNT))
    {
        printf("vsSkeletonMeshGeometry::getDataListSize:  Invalid data list "
            "index\n");
        return -1;
    }

    // Make sure we're not returning the size of a generic list when a
    // conventional list is active, and vice versa
    if (((list == whichData) && (!dataIsGeneric[list])) ||
        ((list != whichData) && (dataIsGeneric[list])))
        return dataListSize[list];
    else
        return 0;
}

// ------------------------------------------------------------------------
// Enables lit rendering for this geometry
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::enableLighting()
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
void vsSkeletonMeshGeometry::disableLighting()
{
    // Set the lighting state to OFF on the Performer GeoState
    performerGeostate->setMode(PFSTATE_ENLIGHTING, PF_OFF);
}

// ------------------------------------------------------------------------
// Returns if lighting is enabled for this geometry
// ------------------------------------------------------------------------
bool vsSkeletonMeshGeometry::isLightingEnabled()
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
void vsSkeletonMeshGeometry::setRenderBin(int binNum)
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
vsMatrix vsSkeletonMeshGeometry::getGlobalXform()
{
    pfNode *nodePtr;
    pfMatrix xform;
    const pfMatrix *scsMatPtr;
    vsMatrix result;
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
    
    // Copy the pfMatrix into a vsMatrix.  Recall that a pfMatrix is
    // transposed with respect to a vsMatrix (this is why the indices
    // below are reversed)
    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            result[loop][sloop] = xform[sloop][loop];

    // Return the vsMatrix
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
    // Set the mask of the Performer intersection traversal for this node
    // to the given value.
    performerGeode->setTravMask(PFTRAV_ISECT, newValue, PFTRAV_SELF, PF_SET);
}

// ------------------------------------------------------------------------
// Retrieves the intersection value for this geometry.
// ------------------------------------------------------------------------
unsigned int vsSkeletonMeshGeometry::getIntersectValue()
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
void vsSkeletonMeshGeometry::enableCull()
{
    performerGeode->setTravMask(PFTRAV_CULL, 0xFFFFFFFF,
        PFTRAV_SELF | PFTRAV_DESCEND, PF_SET);
}

// ------------------------------------------------------------------------
// Disables culling (view frustum and otherwise) on this node
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::disableCull()
{
    performerGeode->setTravMask(PFTRAV_CULL, 0x0, PFTRAV_SELF | PFTRAV_DESCEND,
        PF_SET);
}

// ------------------------------------------------------------------------
// Returns the Performer object associated with this object
// ------------------------------------------------------------------------
pfGeode *vsSkeletonMeshGeometry::getBaseLibraryObject()
{
    return performerGeode;
}

// ------------------------------------------------------------------------
// Private, static function
// Initializes a pfGeoArray that will occupy one of the data areas of the
// performerFlux object.
// ------------------------------------------------------------------------
int vsSkeletonMeshGeometry::initFluxedGeoArray(pfFluxMemory *fluxMem)
{
    // If the fluxMemory is NULL, return the size of a fluxed pfGeoArray.
    // This is standard procedure for Performer fluxes (see the man page
    // for pfFlux::pfFlux()).
    if (fluxMem == NULL)
        return sizeof(pfGeoArray);

    // Initialize the fluxMemory to a pfGeoArray
    new(fluxMem) pfGeoArray;

    // Return 0 indicating the pfFluxMemory is valid and we have initialized 
    // it
    return 0;
}

// ------------------------------------------------------------------------
// Internal function
// Adds a node to this node's list of parent nodes
// ------------------------------------------------------------------------
bool vsSkeletonMeshGeometry::addParent(vsNode *newParent)
{
    // Add the given node to the parent list and reference it
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

    // Look thru this node's parent list to see if the target parent is
    // there
    for (loop = 0; loop < parentCount; loop++)
    {
        // Check the current parent against the target parent
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
// scene utilities' graphics state object to affect the changes to the
// graphics library state.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::applyAttributes()
{
    // Call the parent class method
    vsNode::applyAttributes();
    
    // Apply the current vsGraphicsState settings to this object's
    // pfGeoState
    (vsGraphicsState::getInstance())->applyState(performerGeostate);
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
    float *vertexList, *normalList, *boneList, *weightList;
    int vertexListSize, normalListSize, boneListSize, weightListSize;

    // Begin a new dynamic geometry state
    beginNewState();

    // For readability, get the important lists and their sizes into 
    // temporary variables
    vertexList = dataList[VS_GEOMETRY_VERTEX_COORDS];
    vertexListSize = dataListSize[VS_GEOMETRY_VERTEX_COORDS];
    normalList = dataList[VS_GEOMETRY_NORMALS];
    normalListSize = dataListSize[VS_GEOMETRY_NORMALS];
    boneList = dataList[VS_GEOMETRY_BONE_INDICES];
    boneListSize = dataListSize[VS_GEOMETRY_BONE_INDICES];
    weightList = dataList[VS_GEOMETRY_VERTEX_WEIGHTS];
    weightListSize = dataListSize[VS_GEOMETRY_VERTEX_WEIGHTS];

    // If all the relevant lists are equal in size, continue to apply.
    if ((vertexListSize == boneListSize) && (boneListSize == weightListSize))
    {
        // For each vertex.
        for (vertexIndex = 0; vertexIndex < vertexListSize; vertexIndex++)
        {
            // Get the original vertex value.
            vertex.set(originalVertexList[vertexIndex*3],
                originalVertexList[vertexIndex*3 + 1],
                originalVertexList[vertexIndex*3 + 2]);

            // Get the original normal value.
            normal.set(originalNormalList[vertexIndex*3],
                originalNormalList[vertexIndex*3 + 1],
                originalNormalList[vertexIndex*3 + 2]);

            // Clear the final matrices.
            finalVertexMatrix.clear();
            finalNormalMatrix.clear();

            // For each data index, AKA possible influences.
            for (dataIndex = 0; dataIndex < 4; dataIndex++)
            {
                // Get the weight for this bone
                weight = weightList[vertexIndex*4 + dataIndex];

                // Get the bone index
                bone = (int)(boneList[vertexIndex*4 + dataIndex]);

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
            vertexList[vertexIndex*3 + 0] = vertex[0];
            vertexList[vertexIndex*3 + 1] = vertex[1];
            vertexList[vertexIndex*3 + 2] = vertex[2];

            // Transform the original normal by the average matrix,
            // re-normalize.
            normal = finalNormalMatrix.getVectorXform(normal);
            normal.normalize();

            // Set the final normal into the normal array list.
            normalList[vertexIndex*3 + 0] = normal[0];
            normalList[vertexIndex*3 + 1] = normal[1];
            normalList[vertexIndex*3 + 2] = normal[2];
        }
    }
    else
        printf("vsSkeletonMeshGeometry::applySkin:  Data list sizes don't "
            "match!\n");

    // Complete the skinning process
    finishNewState();
}

// ------------------------------------------------------------------------
// Static internal function - Performer callback
// "Pre" callback function for pfGeoState attached to the
// vsSkeletonMeshGeometry.  Required in order to activate 'local'
// vsLightAttributes that are affecting this geometry.
// ------------------------------------------------------------------------
int vsSkeletonMeshGeometry::geostateCallback(pfGeoState *gstate, void *userData)
{
    pfLight **lightList;
    int loop;
    
    // Get the light list from the userData parameter
    lightList = (pfLight **)userData;
    
    // Turn all the lights in the list on
    for (loop = 0; loop < PF_MAX_LIGHTS; loop++)
        if (lightList[loop] != NULL)
            (lightList[loop])->on();

    // Return zero (Performer callback requires a return value, even though
    // it is ignored)
    return 0;
}
