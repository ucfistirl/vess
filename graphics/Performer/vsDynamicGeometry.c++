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
//                  coordinates, colors, and face normals.
//
//    Author(s):    Bryan Kline, Jason Daly
//
//------------------------------------------------------------------------

#include "vsDynamicGeometry.h++"

#include <Performer/pf/pfSCS.h>
#include <Performer/pr/pfLight.h>
#include "vsComponent.h++"
#include "vsShadingAttribute.h++"
#include "vsGraphicsState.h++"

// ------------------------------------------------------------------------
// Default Constructor - Creates a Performer geode and geoset and connects
// them together, sets up empty geometry lists, and registers callbacks.
// ------------------------------------------------------------------------
vsDynamicGeometry::vsDynamicGeometry() : parentList(5, 5)
{
    int loop;

    // Start with no parents
    parentCount = 0;

    // Create the pfGeode
    performerGeode = new pfGeode();
    performerGeode->ref();

    // Create a pfFlux for the pfGeoSets
    performerFlux = new pfFlux(initFluxedGeoSet, PFFLUX_DEFAULT_NUM_BUFFERS);
    performerFlux->ref();

    // Create a pfGeoState
    performerGeostate = new pfGeoState();
    performerGeostate->ref();

    // Extract the first pfGeoSet from the pfFlux and attach the pfGeoState
    // to it
    performerGeoset = (pfGeoSet *)performerFlux->getCurData();
    performerGeode->addGSet(performerGeoset);

    // Initialize the attribute lists to NULL and size 0
    colorList = NULL;
    colorListSize = 0;
    normalList = NULL;
    normalListSize = 0;
    texCoordList = NULL;
    texCoordListSize = 0;
    vertexList = NULL;
    vertexListSize = 0;
    lengthsList = NULL;

    // Initialize the primitive count to zero
    setPrimitiveCount(0);
    
    // Initialize the attribute bindings
    colorBinding = VS_GEOMETRY_BIND_NONE;
    normalBinding = VS_GEOMETRY_BIND_NONE;
    texCoordBinding = VS_GEOMETRY_BIND_NONE;
    vertexBinding = VS_GEOMETRY_BIND_PER_VERTEX;
    
    // Take care of lights and other graphics state initialization
    lightsList = (pfLight **)
        (pfMemory::malloc(sizeof(pfLight *) * PF_MAX_LIGHTS));
    for (loop = 0; loop < PF_MAX_LIGHTS; loop++)
        lightsList[loop] = NULL;
    performerGeostate->setFuncs(geostateCallback, NULL, lightsList);
    
    // Turn off GeoSet flat shading
    performerGeoset->setDrawMode(PFGS_FLATSHADE, PF_OFF);

    // Enable lighting (by default)
    enableLighting();
    
    // Register the pfGeode with the vsObjectMap
    getMap()->registerLink(this, performerGeode);
}

// ------------------------------------------------------------------------
// Destructor - Disconnects this node from its Performer counterpart
// ------------------------------------------------------------------------
vsDynamicGeometry::~vsDynamicGeometry()
{
    vsAttribute *attr;
    vsNode *parent;

    // Remove all attached attributes; destroy those that aren't being
    // used by other nodes.
    while (getAttributeCount() > 0)
    {
        attr = getAttribute(0);
        removeAttribute(attr);
        if (!(attr->isAttached()))
            delete attr;
    }
 
    // Remove this node from its parents
    while (getParentCount() > 0)
    {
        parent = getParent(0);
        parent->removeChild(this);
    }

    // Unlink and destroy the Performer objects.  pfDelete()'ing the pfFlux
    // will (should) take care of the pfGeoSets attached to it.
    performerGeode->unref();
    pfDelete(performerGeode);
    performerFlux->unref();
    pfDelete(performerFlux);
    performerGeostate->unref();
    pfDelete(performerGeostate);
    
    getMap()->removeLink(this, VS_OBJMAP_FIRST_LIST);
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
// Retrieves the number of parents for this node
// ------------------------------------------------------------------------
int vsDynamicGeometry::getParentCount()
{
    return parentCount;
}

// ------------------------------------------------------------------------
// Retrieves the number of parents for this node
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
// Begins a new state/frame of the dynamic geometry.  Creates a new GeoSet
// and copies the current state for primitiveType, primitiveCount, and 
// attribute bindings.  The remaining attributes are extracted from the 
// performerFlux and may be in an initial state (set up by 
// pfFluxedGSetInit()) or may be a previous state (written one or more 
// frames ago).  These steps will allow fluxed memory to be reused.  For
// consistent results, attributes (colors, normals, vertex coordinates, and
// texture coordinates) must be updated every frame.  
// ------------------------------------------------------------------------
void vsDynamicGeometry::beginNewState()
{
    unsigned short *dummy;
    int            min;
    int            *lengths;

    performerGeoset = (pfGeoSet *)performerFlux->getWritableData();
    performerGeoset->setPrimType(primitiveType);
    performerGeoset->setNumPrims(primitiveCount);

    // Retrieve the data lists 
    performerGeoset->getAttrLists(PFGS_COLOR4, (void **)&colorList, &dummy);
    performerGeoset->getAttrRange(PFGS_COLOR4, &min, &colorListSize);
    performerGeoset->getAttrLists(PFGS_NORMAL3, (void **)&normalList, &dummy);
    performerGeoset->getAttrRange(PFGS_NORMAL3, &min, &normalListSize);
    performerGeoset->getAttrLists(PFGS_TEXCOORD2, (void **)&texCoordList, 
        &dummy);
    performerGeoset->getAttrRange(PFGS_TEXCOORD2, &min, &texCoordListSize);
    performerGeoset->getAttrLists(PFGS_COORD3, (void **)&vertexList, &dummy);
    performerGeoset->getAttrRange(PFGS_COORD3, &min, &vertexListSize);

    // Copy the lengths list from the pfGeoSet, if it has one
    lengths = performerGeoset->getPrimLengths();
    if (lengths == NULL)
        lengthsList = NULL;
    else
        memcpy(lengthsList, lengths, sizeof(int) * primitiveCount);

    // Set the bindings
    setBinding(VS_GEOMETRY_COLORS, colorBinding);
    setBinding(VS_GEOMETRY_NORMALS, normalBinding);
    setBinding(VS_GEOMETRY_TEXTURE_COORDS, texCoordBinding);
    setBinding(VS_GEOMETRY_VERTEX_COORDS, vertexBinding);

    // Attach the current GeoState
    performerGeoset->setGState(performerGeostate);
}

// ------------------------------------------------------------------------
// Finalizes the new dynamic geometry state.  This makes the state readable
// for rendering, and no longer writable.
// ------------------------------------------------------------------------
void vsDynamicGeometry::finishNewState()
{
    performerFlux->writeComplete();
}

// ------------------------------------------------------------------------
// Sets the type of geometric primitive that this object contains
// ------------------------------------------------------------------------
void vsDynamicGeometry::setPrimitiveType(int newType)
{
    switch (newType)
    {
        case VS_GEOMETRY_TYPE_POINTS:
            performerGeoset->setPrimType(PFGS_POINTS);
            primitiveType = PFGS_POINTS;
            break;
        case VS_GEOMETRY_TYPE_LINES:
            performerGeoset->setPrimType(PFGS_LINES);
            primitiveType = PFGS_LINES;
            break;
        case VS_GEOMETRY_TYPE_LINE_STRIPS:
            performerGeoset->setPrimType(PFGS_LINESTRIPS);
            primitiveType = PFGS_LINESTRIPS;
            break;
        case VS_GEOMETRY_TYPE_LINE_LOOPS:
            printf("vsDynamicGeometry::setPrimitiveType: "
                "VS_GEOMETRY_TYPE_LINE_LOOPS type not supported under "
                "Performer operation\n");
            performerGeoset->setPrimType(PFGS_LINESTRIPS);
            primitiveType = PFGS_LINESTRIPS;
            break;
        case VS_GEOMETRY_TYPE_TRIS:
            performerGeoset->setPrimType(PFGS_TRIS);
            primitiveType = PFGS_TRIS;
            break;
        case VS_GEOMETRY_TYPE_TRI_STRIPS:
            performerGeoset->setPrimType(PFGS_TRISTRIPS);
            primitiveType = PFGS_TRISTRIPS;
            break;
        case VS_GEOMETRY_TYPE_TRI_FANS:
            performerGeoset->setPrimType(PFGS_TRIFANS);
            primitiveType = PFGS_TRIFANS;
            break;
        case VS_GEOMETRY_TYPE_QUADS:
            performerGeoset->setPrimType(PFGS_QUADS);
            primitiveType = PFGS_QUADS;
            break;
        case VS_GEOMETRY_TYPE_QUAD_STRIPS:
            printf("vsDynamicGeometry::setPrimitiveType: "
                "VS_GEOMETRY_TYPE_QUAD_STRIPS type not supported under"
                "Performer operation\n");
            performerGeoset->setPrimType(PFGS_QUADS);
            primitiveType = PFGS_QUADS;
            break;
        case VS_GEOMETRY_TYPE_POLYS:
            performerGeoset->setPrimType(PFGS_POLYS);
            primitiveType = PFGS_POLYS;
            break;
        default:
            printf("vsDynamicGeometry::setPrimitiveType: Unrecognized "
                "primitive type\n");
            break;
    }
}

// ------------------------------------------------------------------------
// Retrieves the type of geometric primitive that this object contains
// ------------------------------------------------------------------------
int vsDynamicGeometry::getPrimitiveType()
{
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
    
    return -1;
}

// ------------------------------------------------------------------------
// Sets the number of geometric primitive that this object contains. Must
// be called before any calls to set the data for any specific primitive.
// ------------------------------------------------------------------------
void vsDynamicGeometry::setPrimitiveCount(int newCount)
{
    performerGeoset->setNumPrims(newCount);
    primitiveCount = newCount;
    
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
        // Create
        lengthsList = (int *)(pfMemory::malloc(sizeof(int) * newCount));
    }
    else if (!newCount && lengthsList)
    {
        // Delete
        pfMemory::free(lengthsList);
        lengthsList = NULL;
    }
    else
    {
        // Modify
        lengthsList = (int *)(pfMemory::realloc(lengthsList,
            sizeof(int) * newCount));
    }
    
    performerGeoset->setPrimLengths(lengthsList);
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
    if ((index < 0) || (index >= primitiveCount))
    {
        printf("vsDynamicGeometry::setPrimitiveLength: Index out of bounds\n");
        return;
    }
    
    // If the geometry's particular primitive type doesn't require a
    // primitive lengths array, don't set anything.
    if ((getPrimitiveType() == VS_GEOMETRY_TYPE_POINTS) ||
        (getPrimitiveType() == VS_GEOMETRY_TYPE_LINES) ||
        (getPrimitiveType() == VS_GEOMETRY_TYPE_TRIS) ||
        (getPrimitiveType() == VS_GEOMETRY_TYPE_QUADS))
        return;

    lengthsList[index] = length;
}

// ------------------------------------------------------------------------
// Retrieves the number of verticies specified for the primitive with the
// indicated index in the object. The index of the first primitive is 0.
// ------------------------------------------------------------------------
int vsDynamicGeometry::getPrimitiveLength(int index)
{
    if ((index < 0) || (index >= primitiveCount))
    {
        printf("vsDynamicGeometry::getPrimitiveLength: Index out of bounds\n");
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
    
    // If the geometry's particular primitive type doesn't require a
    // primitive lengths array, don't set anything.
    if ((getPrimitiveType() == VS_GEOMETRY_TYPE_POINTS) ||
        (getPrimitiveType() == VS_GEOMETRY_TYPE_LINES) ||
        (getPrimitiveType() == VS_GEOMETRY_TYPE_TRIS) ||
        (getPrimitiveType() == VS_GEOMETRY_TYPE_QUADS))
        return;

    for (loop = 0; loop < primitiveCount; loop++)
        lengthsList[loop] = lengths[loop];
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
    
    for (loop = 0; loop < primitiveCount; loop++)
    {
        switch (getPrimitiveType())
        {
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
void vsDynamicGeometry::setBinding(int whichData, int binding)
{
    int performerBinding;
    
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
        default:
            printf("vsDynamicGeometry::setBinding: Unrecognized binding value\n");
            return;
    }

    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsDynamicGeometry::setBinding: Vertex coordinate "
                    "binding must always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }
            performerGeoset->setAttr(PFGS_COORD3, performerBinding,
                vertexList, NULL);
            vertexBinding = performerBinding;
            break;
        case VS_GEOMETRY_NORMALS:
            performerGeoset->setAttr(PFGS_NORMAL3, performerBinding,
                normalList, NULL);
            normalBinding = performerBinding;
            break;
        case VS_GEOMETRY_COLORS:
            performerGeoset->setAttr(PFGS_COLOR4, performerBinding,
                colorList, NULL);
            colorBinding = performerBinding;
            break;
        case VS_GEOMETRY_TEXTURE_COORDS:
            if ((binding != VS_GEOMETRY_BIND_PER_VERTEX) &&
                (binding != VS_GEOMETRY_BIND_NONE))
            {
                printf("vsDynamicGeometry::setBinding: Texture coordinates "
                    "binding must be either VS_GEOMETRY_BIND_PER_VERTEX or "
                    "VS_GEOMETRY_BIND_NONE\n");
                return;
            }
            performerGeoset->setAttr(PFGS_TEXCOORD2, performerBinding,
                texCoordList, NULL);
            texCoordBinding = performerBinding;
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
    int result;

    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            return VS_GEOMETRY_BIND_PER_VERTEX;
        case VS_GEOMETRY_NORMALS:
            result = normalBinding;
            break;
        case VS_GEOMETRY_COLORS:
            result = colorBinding;
            break;
        case VS_GEOMETRY_TEXTURE_COORDS:
            result = texCoordBinding;
            break;
        default:
            printf("vsDynamicGeometry::getBinding: Unrecognized data value\n");
            return -1;
    }
    
    switch (result)
    {
        case PFGS_OFF:
            return VS_GEOMETRY_BIND_NONE;
        case PFGS_OVERALL:
            return VS_GEOMETRY_BIND_OVERALL;
        case PFGS_PER_PRIM:
            return VS_GEOMETRY_BIND_PER_PRIMITIVE;
        case PFGS_PER_VERTEX:
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
                (vertexList[dataIndex])[loop] = data[loop];
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
                (normalList[dataIndex])[loop] = data[loop];
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
                (colorList[dataIndex])[loop] = data[loop];
            break;
        case VS_GEOMETRY_TEXTURE_COORDS:
            if (dataIndex >= texCoordListSize)
            {
                printf("vsDynamicGeometry::setData: Index out of bounds\n");
                return;
            }
            if (data.getSize() < 2)
            {
                printf("vsDynamicGeometry::setData: Insufficient data (texture "
                    "coordinates require 2 values)\n");
                return;
            }
            for (loop = 0; loop < 2; loop++)
                (texCoordList[dataIndex])[loop] = data[loop];
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
    int loop;

    if (dataIndex < 0)
    {
        printf("vsDynamicGeometry::getData: Index out of bounds\n");
        return result;
    }
    
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            if (dataIndex >= vertexListSize)
            {
                printf("vsDynamicGeometry::getData: Index out of bounds\n");
                return result;
            }
            result.setSize(3);
            for (loop = 0; loop < 3; loop++)
                result[loop] = (vertexList[dataIndex])[loop];
            break;
        case VS_GEOMETRY_NORMALS:
            if (dataIndex >= normalListSize)
            {
                printf("vsDynamicGeometry::getData: Index out of bounds\n");
                return result;
            }
            result.setSize(3);
            for (loop = 0; loop < 3; loop++)
                result[loop] = (normalList[dataIndex])[loop];
            break;
        case VS_GEOMETRY_COLORS:
            if (dataIndex >= colorListSize)
            {
                printf("vsDynamicGeometry::getData: Index out of bounds\n");
                return result;
            }
            result.setSize(4);
            for (loop = 0; loop < 4; loop++)
                result[loop] = (colorList[dataIndex])[loop];
            break;
        case VS_GEOMETRY_TEXTURE_COORDS:
            if (dataIndex >= texCoordListSize)
            {
                printf("vsDynamicGeometry::getData: Index out of bounds\n");
                return result;
            }
            result.setSize(2);
            for (loop = 0; loop < 2; loop++)
                result[loop] = (texCoordList[dataIndex])[loop];
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
    int loop, sloop;
    
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            for (loop = 0; loop < vertexListSize; loop++)
                for (sloop = 0; sloop < 3; sloop++)
                    vertexList[loop][sloop] = dataList[loop][sloop];
            break;
        case VS_GEOMETRY_NORMALS:
            for (loop = 0; loop < normalListSize; loop++)
                for (sloop = 0; sloop < 3; sloop++)
                    normalList[loop][sloop] = dataList[loop][sloop];
            break;
        case VS_GEOMETRY_COLORS:
            for (loop = 0; loop < colorListSize; loop++)
                for (sloop = 0; sloop < 4; sloop++)
                    colorList[loop][sloop] = dataList[loop][sloop];
            break;
        case VS_GEOMETRY_TEXTURE_COORDS:
            for (loop = 0; loop < texCoordListSize; loop++)
                for (sloop = 0; sloop < 2; sloop++)
                    texCoordList[loop][sloop] = dataList[loop][sloop];
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
    int loop, sloop;
    
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            for (loop = 0; loop < vertexListSize; loop++)
            {
                dataBuffer[loop].setSize(3);
                for (sloop = 0; sloop < 3; sloop++)
                    dataBuffer[loop][sloop] = vertexList[loop][sloop];
            }
            break;
        case VS_GEOMETRY_NORMALS:
            for (loop = 0; loop < normalListSize; loop++)
            {
                dataBuffer[loop].setSize(3);
                for (sloop = 0; sloop < 3; sloop++)
                    dataBuffer[loop][sloop] = normalList[loop][sloop];
            }
            break;
        case VS_GEOMETRY_COLORS:
            for (loop = 0; loop < colorListSize; loop++)
            {
                dataBuffer[loop].setSize(4);
                for (sloop = 0; sloop < 4; sloop++)
                    dataBuffer[loop][sloop] = colorList[loop][sloop];
            }
            break;
        case VS_GEOMETRY_TEXTURE_COORDS:
            for (loop = 0; loop < texCoordListSize; loop++)
            {
                dataBuffer[loop].setSize(2);
                for (sloop = 0; sloop < 2; sloop++)
                    dataBuffer[loop][sloop] = texCoordList[loop][sloop];
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
    int binding, performerBinding;
    
    binding = getBinding(whichData);
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

    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            if (newSize && !vertexList)
            {
                // Create
                vertexList = (pfVec3 *)(pfMemory::malloc(
                    sizeof(pfVec3) * newSize));
            }
            else if (!newSize && vertexList)
            {
                // Delete
                pfMemory::free(vertexList);
                vertexList = NULL;
            }
            else
            {
                // Modify
                vertexList = (pfVec3 *)(pfMemory::realloc(vertexList,
                    sizeof(pfVec3) * newSize));
            }
            performerGeoset->setAttr(PFGS_COORD3, performerBinding,
                vertexList, NULL);
            vertexListSize = newSize;
            break;
        case VS_GEOMETRY_NORMALS:
            if (newSize && !normalList)
            {
                // Create
                normalList = (pfVec3 *)(pfMemory::malloc(
                    sizeof(pfVec3) * newSize));
            }
            else if (!newSize && normalList)
            {
                // Delete
                pfMemory::free(normalList);
                normalList = NULL;
            }
            else
            {
                // Modify
                normalList = (pfVec3 *)(pfMemory::realloc(normalList,
                    sizeof(pfVec3) * newSize));
            }
            performerGeoset->setAttr(PFGS_NORMAL3, performerBinding,
                normalList, NULL);
            normalListSize = newSize;
            break;
        case VS_GEOMETRY_COLORS:
            if (newSize && !colorList)
            {
                // Create
                colorList = (pfVec4 *)(pfMemory::malloc(
                    sizeof(pfVec4) * newSize));
            }
            else if (!newSize && colorList)
            {
                // Delete
                pfMemory::free(colorList);
                colorList = NULL;
            }
            else
            {
                // Modify
                colorList = (pfVec4 *)(pfMemory::realloc(colorList,
                    sizeof(pfVec4) * newSize));
            }
            performerGeoset->setAttr(PFGS_COLOR4, performerBinding,
                colorList, NULL);
            colorListSize = newSize;
            break;
        case VS_GEOMETRY_TEXTURE_COORDS:
            if (newSize && !texCoordList)
            {
                // Create
                texCoordList = (pfVec2 *)(pfMemory::malloc(
                    sizeof(pfVec2) * newSize));
            }
            else if (!newSize && texCoordList)
            {
                // Delete
                pfMemory::free(texCoordList);
                texCoordList = NULL;
            }
            else
            {
                // Modify
                texCoordList = (pfVec2 *)(pfMemory::realloc(texCoordList,
                    sizeof(pfVec2) * newSize));
            }
            performerGeoset->setAttr(PFGS_TEXCOORD2, performerBinding,
                texCoordList, NULL);
            texCoordListSize = newSize;
            break;
        default:
            printf("vsDynamicGeometry::setDataListSize: Unrecognized data value\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves the size of one of the object's data lists
// ------------------------------------------------------------------------
int vsDynamicGeometry::getDataListSize(int whichData)
{
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
            printf("vsDynamicGeometry::getDataListSize: Unrecognized data value\n");
    }
    
    return -1;
}

// ------------------------------------------------------------------------
// Enables lit rendering for this geometry
// ------------------------------------------------------------------------
void vsDynamicGeometry::enableLighting()
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
void vsDynamicGeometry::disableLighting()
{
    // Set the lighting state to OFF on the Performer GeoState
    performerGeostate->setMode(PFSTATE_ENLIGHTING, PF_OFF);
}

// ------------------------------------------------------------------------
// Returns if lighting is enabled for this geometry
// ------------------------------------------------------------------------
int vsDynamicGeometry::isLightingEnabled()
{
    // Check the local GeoState to see if the lighting state is inherited.
    // If not, it is locally disabled.
    if ((performerGeostate->getInherit() & PFSTATE_ENLIGHTING))
        return VS_TRUE;
    else
        return VS_FALSE;
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
    pfSphere boundSphere;
    
    performerGeode->getBound(&boundSphere);
    
    if (centerPoint)
        centerPoint->set(boundSphere.center[PF_X], boundSphere.center[PF_Y],
            boundSphere.center[PF_Z]);

    if (radius)
        *radius = boundSphere.radius;
}

// ------------------------------------------------------------------------
// Computes the global coordinate transform at this geometry by multiplying
// together all of the transforms at nodes above this one.
// ------------------------------------------------------------------------
vsMatrix vsDynamicGeometry::getGlobalXform()
{
    pfNode *nodePtr;
    pfMatrix xform;
    const pfMatrix *scsMatPtr;
    vsMatrix result;
    int loop, sloop;

    xform.makeIdent();
    nodePtr = performerGeode;
    
    while (nodePtr->getNumParents() > 0)
    {
        if (nodePtr->isOfType(pfSCS::getClassType()))
        {
            scsMatPtr = ((pfSCS *)nodePtr)->getMatPtr();
            xform.postMult(*scsMatPtr);
        }
        
        nodePtr = nodePtr->getParent(0);
    }
    
    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            result[loop][sloop] = xform[sloop][loop];

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
    performerGeode->setTravMask(PFTRAV_ISECT, newValue, PFTRAV_SELF, PF_SET);
}

// ------------------------------------------------------------------------
// Retrieves the intersection value for this geometry.
// ------------------------------------------------------------------------
unsigned int vsDynamicGeometry::getIntersectValue()
{
    return (performerGeode->getTravMask(PFTRAV_ISECT));
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
        printf("vsDynamicGeometry::addAttribute: Attribute is already in use\n");
        return;
    }
    
    attrCat = newAttribute->getAttributeCategory();
    if (attrCat != VS_ATTRIBUTE_CATEGORY_STATE)
    {
        printf("vsDynamicGeometry::addAttribute: Geometry nodes may not contain "
            "attributes of that type\n");
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
// Returns the Performer object associated with this object
// ------------------------------------------------------------------------
pfGeode *vsDynamicGeometry::getBaseLibraryObject()
{
    return performerGeode;
}

// ------------------------------------------------------------------------
// Private, static function
// Initializes a pfGeoSet that will occupy one of the data areas of the
// performerFlux object.
// ------------------------------------------------------------------------
int vsDynamicGeometry::initFluxedGeoSet(pfFluxMemory *fluxMem)
{
    if (fluxMem == NULL)
        return pfFluxedGSetInit(fluxMem);

    pfFluxedGSetInit(fluxMem);

    return 0;
}

// ------------------------------------------------------------------------
// Internal function
// Adds a node to this node's list of parent nodes
// ------------------------------------------------------------------------
int vsDynamicGeometry::addParent(vsNode *newParent)
{
    parentList[parentCount++] = newParent;
    newParent->ref();

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Internal function
// Removes a node from this node's list of parent nodes
// ------------------------------------------------------------------------
int vsDynamicGeometry::removeParent(vsNode *targetParent)
{
    int loop, sloop;

    for (loop = 0; loop < parentCount; loop++)
        if (targetParent == parentList[loop])
        {
            for (sloop = loop; sloop < parentCount-1; sloop++)
                parentList[sloop] = parentList[sloop+1];
            targetParent->unref();
            parentCount--;
            return VS_TRUE;
        }

    return VS_FALSE;
}

// ------------------------------------------------------------------------
// Internal function
// Calls the apply function on all attached attributes, and then calls the
// scene utilities' graphics state object to affect the changes to the
// graphics library state.
// ------------------------------------------------------------------------
void vsDynamicGeometry::applyAttributes()
{
    vsNode::applyAttributes();
    
    (vsGraphicsState::getInstance())->applyState(performerGeostate);
}

// ------------------------------------------------------------------------
// Static internal function - Performer callback
// "Pre" callback function for pfGeoState attached to the vsDynamicGeometry.
// Required in order to activate 'local' vsLightAttributes that are
// affecting this geometry.
// ------------------------------------------------------------------------
int vsDynamicGeometry::geostateCallback(pfGeoState *gstate, void *userData)
{
    pfLight **lightList;
    int loop;
    
    lightList = (pfLight **)userData;
    
    for (loop = 0; loop < PF_MAX_LIGHTS; loop++)
        if (lightList[loop] != NULL)
            (lightList[loop])->on();

    return 0;
}
