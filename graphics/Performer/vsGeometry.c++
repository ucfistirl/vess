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
#include "vsBackfaceAttribute.h++"
#include "vsFogAttribute.h++"
#include "vsMaterialAttribute.h++"
#include "vsShadingAttribute.h++"
#include "vsTextureAttribute.h++"
#include "vsTransparencyAttribute.h++"
#include "vsWireframeAttribute.h++"
#include "vsGraphicsState.h++"

vsTreeMap *vsGeometry::binModeList = NULL;
bool vsGeometry::binModesChanged = false;

// ------------------------------------------------------------------------
// Default Constructor - Creates a Performer geode and geoset and connects
// them together, sets up empty geometry lists, and registers callbacks.
// ------------------------------------------------------------------------
vsGeometry::vsGeometry() : parentList(5, 5)
{
    unsigned int unit;
    int loop;

    // Start with no parents
    parentCount = 0;

    // Create a Performer geometry node to hold the geometry
    performerGeode = new pfGeode();
    performerGeode->ref();

    // Create a pfGeoSet and add it to the geode
    performerGeoset = new pfGeoSet();
    performerGeoset->ref();
    performerGeode->addGSet(performerGeoset);
    
    // Create a pfGeoState
    performerGeostate = new pfGeoState();
    performerGeostate->ref();
    performerGeoset->setGState(performerGeostate);
    
    // Initialize the attribute lists to NULL and size 0
    colorList = NULL;
    colorListSize = 0;
    normalList = NULL;
    normalListSize = 0;
    for (unit = 0; unit < VS_MAXIMUM_TEXTURE_UNITS; unit++)
    {
        texCoordList[unit] = NULL;
        texCoordListSize[unit] = 0;
    }
    vertexList = NULL;
    vertexListSize = 0;
    lengthsList = NULL;

    // Initialize the number of primitives and the size of the primitive
    // length list to zero
    setPrimitiveCount(0);
    
    // Set up our lights list
    lightsList = (pfLight **)
        (pfMemory::malloc(sizeof(pfLight *) * PF_MAX_LIGHTS));
    for (loop = 0; loop < PF_MAX_LIGHTS; loop++)
        lightsList[loop] = NULL;

    // Set our callback function as the geostate callback function,
    // and our array of pfLights as the callback data pointer
    performerGeostate->setFuncs(geostateCallback, NULL, lightsList);
    
    // Disable forced-flatshaded mode on the geoset
    performerGeoset->setDrawMode(PFGS_FLATSHADE, PF_OFF);
    
    // Enable lighting (by default)
    enableLighting();

    // Register the pfGeode with the vsObjectMap
    getMap()->registerLink(this, performerGeode);
}

// ------------------------------------------------------------------------
// Destructor - Disconnects this node from its Performer counterpart
// ------------------------------------------------------------------------
vsGeometry::~vsGeometry()
{
    unsigned int unit;

    // Remove all parents
    detachFromParents();

    // Remove all attributes
    deleteAttributes();

    // Unlink and destroy the Performer objects
    performerGeode->unref();
    pfDelete(performerGeode);
    performerGeoset->unref();
    pfDelete(performerGeoset);
    performerGeostate->unref();
    pfDelete(performerGeostate);
    
    // Delete the geoetric data lists
    if (vertexList && !(pfMemory::getRef(vertexList)))
        pfMemory::free(vertexList);
    if (colorList && !(pfMemory::getRef(colorList)))
        pfMemory::free(colorList);
    if (normalList && !(pfMemory::getRef(normalList)))
        pfMemory::free(normalList);
    for (unit = 0; unit < VS_MAXIMUM_TEXTURE_UNITS; unit++)
        if (texCoordList[unit] && !(pfMemory::getRef(texCoordList[unit])))
            pfMemory::free(texCoordList[unit]);
    if (lengthsList && !(pfMemory::getRef(lengthsList)))
        pfMemory::free(lengthsList);

    getMap()->removeLink(this, VS_OBJMAP_FIRST_LIST);
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
            performerGeoset->setPrimType(PFGS_POINTS);
            break;
        case VS_GEOMETRY_TYPE_LINES:
            performerGeoset->setPrimType(PFGS_LINES);
            break;
        case VS_GEOMETRY_TYPE_LINE_STRIPS:
            performerGeoset->setPrimType(PFGS_LINESTRIPS);
            break;
        case VS_GEOMETRY_TYPE_LINE_LOOPS:
            printf("vsGeometry::setPrimitiveType: VS_GEOMETRY_TYPE_LINE_LOOPS "
                "type not supported under Performer operation\n");
            performerGeoset->setPrimType(PFGS_LINESTRIPS);
            break;
        case VS_GEOMETRY_TYPE_TRIS:
            performerGeoset->setPrimType(PFGS_TRIS);
            break;
        case VS_GEOMETRY_TYPE_TRI_STRIPS:
            performerGeoset->setPrimType(PFGS_TRISTRIPS);
            break;
        case VS_GEOMETRY_TYPE_TRI_FANS:
            performerGeoset->setPrimType(PFGS_TRIFANS);
            break;
        case VS_GEOMETRY_TYPE_QUADS:
            performerGeoset->setPrimType(PFGS_QUADS);
            break;
        case VS_GEOMETRY_TYPE_QUAD_STRIPS:
            printf("vsGeometry::setPrimitiveType: VS_GEOMETRY_TYPE_QUAD_STRIPS "
                "type not supported under Performer operation\n");
            performerGeoset->setPrimType(PFGS_QUADS);
            break;
        case VS_GEOMETRY_TYPE_POLYS:
            performerGeoset->setPrimType(PFGS_POLYS);
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
    switch (performerGeoset->getPrimType())
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
    
    // If the primitive type is unrecignized, return an error value
    return -1;
}

// ------------------------------------------------------------------------
// Sets the number of geometric primitive that this object contains. Must
// be called before any calls to set the data for any specific primitive.
// ------------------------------------------------------------------------
void vsGeometry::setPrimitiveCount(int newCount)
{
    // Set the number of primitives on the Performer geoset
    performerGeoset->setNumPrims(newCount);
    
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
    
    // Set the primitive lengths array on the Performer geoset
    performerGeoset->setPrimLengths(lengthsList);
}

// ------------------------------------------------------------------------
// Retrieves the number of geometric primitives that this object contains
// ------------------------------------------------------------------------
int vsGeometry::getPrimitiveCount()
{
    return (performerGeoset->getNumPrims());
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
    // Boudns check
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
void vsGeometry::setBinding(int whichData, int binding)
{
    int performerBinding;
    unsigned int unit;
    
    // Translate VESS constants to Performer constants
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
            printf("vsGeometry::setBinding: Unrecognized binding value\n");
            return;
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
            // Set vertex coordinate binding on the geoset
            performerGeoset->setAttr(PFGS_COORD3, performerBinding,
                vertexList, NULL);
            break;

        case VS_GEOMETRY_NORMALS:
            // Set color binding on the geoset
            performerGeoset->setAttr(PFGS_NORMAL3, performerBinding,
                normalList, NULL);
            break;

        case VS_GEOMETRY_COLORS:
            // Set normal binding on the geoset
            performerGeoset->setAttr(PFGS_COLOR4, performerBinding,
                colorList, NULL);
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

            // Make sure we don't try to set data we don't have a texture
            // unit for
            if (unit >= VS_MAXIMUM_TEXTURE_UNITS)
            {
                printf("vsGeometry::setBinding:  Unsupported texture "
                    "unit %d\n", unit);
                return;
            }
                                                                                
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
            performerGeoset->setMultiAttr(PFGS_TEXCOORD2, unit,
                performerBinding, texCoordList[unit], NULL);
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
            printf("vsGeometry::setBinding: Vertex weights not supported in "
                "vsGeometry.\n");
            printf("    (use vsSkeletonMeshGeometry for vertex skinning.\n");
            break;

        case VS_GEOMETRY_ALT_COLORS:
            printf("vsGeometry::setBinding: Alternate colors not supported "
                "under Performer.\n");
            break;

        case VS_GEOMETRY_FOG_COORDS:
            printf("vsGeometry::setBinding: Fog coordinates not supported "
                "under Performer.\n");
            break;

        case VS_GEOMETRY_USER_DATA0:
        case VS_GEOMETRY_USER_DATA1:
            printf("vsGeometry::setBinding: User data attributes not "
                "supported under Performer.\n");
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
            printf("vsGeometry::setBinding: Generic attributes not supported "
                "under Performer.\n");
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
    unsigned int unit;
    int result;

    // Fetch the binding value
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            return VS_GEOMETRY_BIND_PER_VERTEX;
        case VS_GEOMETRY_NORMALS:
            result = performerGeoset->getAttrBind(PFGS_NORMAL3);
            break;
        case VS_GEOMETRY_COLORS:
            result = performerGeoset->getAttrBind(PFGS_COLOR4);
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
                                                                                
            // Make sure we don't try to set data we don't have a texture
            // unit for
            if (unit >= VS_MAXIMUM_TEXTURE_UNITS)
            {
                printf("vsGeometry::getBinding:  Unsupported texture "
                    "unit %d\n", unit);
                return VS_GEOMETRY_BIND_NONE;
            }
                                                                                
            result = performerGeoset->getMultiAttrBind(PFGS_TEXCOORD2, unit);
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
            printf("vsGeometry::getBinding: Vertex weights not supported in "
                "vsGeometry.\n");
            printf("    (use vsSkeletonMeshGeometry for vertex skinning.\n");
            break;

        case VS_GEOMETRY_ALT_COLORS:
            printf("vsGeometry::getBinding: Alternate colors not supported "
                "under Performer.\n");
            break;

        case VS_GEOMETRY_FOG_COORDS:
            printf("vsGeometry::getBinding: Fog coordinates not supported "
                "under Performer.\n");
            break;

        case VS_GEOMETRY_USER_DATA0:
        case VS_GEOMETRY_USER_DATA1:
            printf("vsGeometry::getBinding: User data attributes not "
                "supported under Performer.\n");
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
            printf("vsGeometry::getBinding: Generic attributes not supported "
                "under Performer.\n");
            break;

        default:
            printf("vsGeometry::getBinding: Unrecognized data value\n");
            return -1;
    }
    
    // Translate Performer constants to VESS constants
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
    
    // If the Performer binding value is unrecognized, return an error value
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
    unsigned int unit;
    int loop;

    // Bounds check
    if (dataIndex < 0)
    {
        printf("vsGeometry::setData: Index out of bounds\n");
        return;
    }
    
    // Interpret the whichData constant
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            // Bounds check
            if (dataIndex >= vertexListSize)
            {
                printf("vsGeometry::setData: Index out of bounds\n");
                return;
            }
            // Input check
            if (data.getSize() < 3)
            {
                printf("vsGeometry::setData: Insufficient data (vertex "
                    "coordinates require 3 values)\n");
                return;
            }
            // Copy the data into our list
            for (loop = 0; loop < 3; loop++)
                (vertexList[dataIndex])[loop] = data[loop];
            break;

        case VS_GEOMETRY_NORMALS:
            // Bounds check
            if (dataIndex >= normalListSize)
            {
                printf("vsGeometry::setData: Index out of bounds\n");
                return;
            }
            // Input check
            if (data.getSize() < 3)
            {
                printf("vsGeometry::setData: Insufficient data (vertex "
                    "normals require 3 values)\n");
                return;
            }
            // Copy the data into our list
            for (loop = 0; loop < 3; loop++)
                (normalList[dataIndex])[loop] = data[loop];
            break;

        case VS_GEOMETRY_COLORS:
            // Bounds check
            if (dataIndex >= colorListSize)
            {
                printf("vsGeometry::setData: Index out of bounds\n");
                return;
            }
            // Input check
            if (data.getSize() < 4)
            {
                printf("vsGeometry::setData: Insufficient data (colors "
                    "require 4 values)\n");
                return;
            }
            // Copy the data into our list
            for (loop = 0; loop < 4; loop++)
                (colorList[dataIndex])[loop] = data[loop];
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
                                                                                
            // Make sure we don't try to set data we don't have a texture
            // unit for
            if (unit >= VS_MAXIMUM_TEXTURE_UNITS)
            {
                printf("vsGeometry::setData:  Unsupported texture "
                    "unit %d\n", unit);
                return;
            }
                                                                                
            // Bounds check
            if (dataIndex >= texCoordListSize[unit])
            {
                printf("vsGeometry::setData: Index out of bounds\n");
                return;
            }
            // Input check
            if (data.getSize() < 2)
            {
                printf("vsGeometry::setData: Insufficient data (texture "
                    "coordinates require 2 values)\n");
                return;
            }
            // Copy the data into our list
            for (loop = 0; loop < 2; loop++)
                (texCoordList[unit][dataIndex])[loop] = data[loop];
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
            printf("vsGeometry::getBinding: Vertex weights not supported in "
                "vsGeometry.\n");
            printf("    (use vsSkeletonMeshGeometry for vertex skinning.\n");
            break;

        case VS_GEOMETRY_ALT_COLORS:
            printf("vsGeometry::getBinding: Alternate colors not supported "
                "under Performer.\n");
            break;

        case VS_GEOMETRY_FOG_COORDS:
            printf("vsGeometry::getBinding: Fog coordinates not supported "
                "under Performer.\n");
            break;

        case VS_GEOMETRY_USER_DATA0:
        case VS_GEOMETRY_USER_DATA1:
            printf("vsGeometry::getBinding: User data attributes not "
                "supported under Performer.\n");
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
            printf("vsGeometry::getBinding: Generic attributes not supported "
                "under Performer.\n");
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
    unsigned int unit;
    int loop;

    // Bounds check
    if (dataIndex < 0)
    {
        printf("vsGeometry::getData: Index out of bounds (dataIndex = %d)\n",
            dataIndex);
        return result;
    }
    
    // Interpret the whichData constant
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            // Bounds check
            if (dataIndex >= vertexListSize)
            {
                printf("vsGeometry::getData: Index out of bounds "
                    "(list = VERTEX_COORDS, dataIndex = %d, listSize = %d)\n",
                    dataIndex, vertexListSize);
                return result;
            }
            // Copy the data to the result vector
            result.setSize(3);
            for (loop = 0; loop < 3; loop++)
                result[loop] = (vertexList[dataIndex])[loop];
            break;

        case VS_GEOMETRY_NORMALS:
            // Bounds check
            if (dataIndex >= normalListSize)
            {
                printf("vsGeometry::getData: Index out of bounds "
                    "(list = NORMALS, dataIndex = %d, listSize = %d)\n",
                    dataIndex, normalListSize);
                return result;
            }
            // Copy the data to the result vector
            result.setSize(3);
            for (loop = 0; loop < 3; loop++)
                result[loop] = (normalList[dataIndex])[loop];
            break;

        case VS_GEOMETRY_COLORS:
            // Bounds check
            if (dataIndex >= colorListSize)
            {
                printf("vsGeometry::getData: Index out of bounds "
                    "(list = COLORS, dataIndex = %d, listSize = %d)\n",
                    dataIndex, colorListSize);
                return result;
            }
            // Copy the data to the result vector
            result.setSize(4);
            for (loop = 0; loop < 4; loop++)
                result[loop] = (colorList[dataIndex])[loop];
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
                                                                                
            // Make sure we don't try to set data we don't have a texture
            // unit for
            if (unit >= VS_MAXIMUM_TEXTURE_UNITS)
            {
                printf("vsGeometry::getData:  Unsupported texture "
                    "unit %d\n", unit);
                return result;
            }
                                                                                
            // Bounds check
            if (dataIndex >= texCoordListSize[unit])
            {
                printf("vsGeometry::getData: Index out of bounds "
                    "(list = TEXTURE%d_COORDS, dataIndex = %d, "
                    "listSize = %d)\n", unit, dataIndex, texCoordListSize);
                return result;
            }
            // Copy the data to the result vector
            result.setSize(2);
            for (loop = 0; loop < 2; loop++)
                result[loop] = (texCoordList[unit][dataIndex])[loop];
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
            printf("vsGeometry::getData: Vertex weights not supported in "
                "vsGeometry.\n");
            printf("    (use vsSkeletonMeshGeometry for vertex skinning.\n");
            break;

        case VS_GEOMETRY_ALT_COLORS:
            printf("vsGeometry::getData: Alternate colors not supported "
                "under Performer.\n");
            break;

        case VS_GEOMETRY_FOG_COORDS:
            printf("vsGeometry::getData: Fog coordinates not supported "
                "under Performer.\n");
            break;

        case VS_GEOMETRY_USER_DATA0:
        case VS_GEOMETRY_USER_DATA1:
            printf("vsGeometry::getData: User data attributes not "
                "supported under Performer.\n");
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
            printf("vsGeometry::getData: Generic attributes not supported "
                "under Performer.\n");
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
void vsGeometry::setDataList(int whichData, vsVector *dataList)
{
    unsigned int unit;
    int loop, sloop;
    
    // Interpret the whichData constant
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
                                                                                
            // Make sure we don't try to set data we don't have a texture
            // unit for
            if (unit >= VS_MAXIMUM_TEXTURE_UNITS)
            {
                printf("vsGeometry::setDataList:  Unsupported texture "
                    "unit %d\n", unit);
                return;
            }
                                                                                
            for (loop = 0; loop < texCoordListSize[unit]; loop++)
                for (sloop = 0; sloop < 2; sloop++)
                    texCoordList[unit][loop][sloop] = dataList[loop][sloop];
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
            printf("vsGeometry::setDataList: Vertex weights not supported in "
                "vsGeometry.\n");
            printf("    (use vsSkeletonMeshGeometry for vertex skinning.\n");
            break;

        case VS_GEOMETRY_ALT_COLORS:
            printf("vsGeometry::setDataList: Alternate colors not supported "
                "under Performer.\n");
            break;

        case VS_GEOMETRY_FOG_COORDS:
            printf("vsGeometry::setDataList: Fog coordinates not supported "
                "under Performer.\n");
            break;

        case VS_GEOMETRY_USER_DATA0:
        case VS_GEOMETRY_USER_DATA1:
            printf("vsGeometry::setDataList: User data attributes not "
                "supported under Performer.\n");
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
            printf("vsGeometry::setDataList: Generic attributes not supported "
                "under Performer.\n");
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
    unsigned int unit;
    int loop, sloop;
    
    // Interpret the whichData constant
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            for (loop = 0; loop < vertexListSize; loop++)
            {
                // Copy the data to the vector buffer
                dataBuffer[loop].setSize(3);
                for (sloop = 0; sloop < 3; sloop++)
                    dataBuffer[loop][sloop] = vertexList[loop][sloop];
            }
            break;

        case VS_GEOMETRY_NORMALS:
            for (loop = 0; loop < normalListSize; loop++)
            {
                // Copy the data to the vector buffer
                dataBuffer[loop].setSize(3);
                for (sloop = 0; sloop < 3; sloop++)
                    dataBuffer[loop][sloop] = normalList[loop][sloop];
            }
            break;

        case VS_GEOMETRY_COLORS:
            for (loop = 0; loop < colorListSize; loop++)
            {
                // Copy the data to the vector buffer
                dataBuffer[loop].setSize(4);
                for (sloop = 0; sloop < 4; sloop++)
                    dataBuffer[loop][sloop] = colorList[loop][sloop];
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
                                                                                
            // Make sure we don't try to set data we don't have a texture
            // unit for
            if (unit >= VS_MAXIMUM_TEXTURE_UNITS)
            {
                printf("vsGeometry::getDataList:  Unsupported texture "
                    "unit %d\n", unit);
                return;
            }
                                                                                
            for (loop = 0; loop < texCoordListSize[unit]; loop++)
            {
                // Copy the data to the vector buffer
                dataBuffer[loop].setSize(2);
                for (sloop = 0; sloop < 2; sloop++)
                    dataBuffer[loop][sloop] = texCoordList[unit][loop][sloop];
            }
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
            printf("vsGeometry::getDataList: Vertex weights not supported in "
                "vsGeometry.\n");
            printf("    (use vsSkeletonMeshGeometry for vertex skinning.\n");
            break;

        case VS_GEOMETRY_ALT_COLORS:
            printf("vsGeometry::getDataList: Alternate colors not supported "
                "under Performer.\n");
            break;

        case VS_GEOMETRY_FOG_COORDS:
            printf("vsGeometry::getDataList: Fog coordinates not supported "
                "under Performer.\n");
            break;

        case VS_GEOMETRY_USER_DATA0:
        case VS_GEOMETRY_USER_DATA1:
            printf("vsGeometry::getDataList: User data attributes not "
                "supported under Performer.\n");
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
            printf("vsGeometry::getDataList: Generic attributes not supported "
                "under Performer.\n");
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
    unsigned int unit;
    int binding, performerBinding;
    
    // Get the VESS binding for this geometry
    binding = getBinding(whichData);

    // Translate VESS to Performer
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
            if (newSize && !vertexList)
            {
                // No list exists, create a new list
                vertexList = (pfVec3 *)(pfMemory::malloc(
                    sizeof(pfVec3) * newSize));
                pfMemory::ref(vertexList);

                // Set the newly-created vertex list on the pfGeoSet
                performerGeoset->setAttr(PFGS_COORD3, performerBinding,
                    vertexList, NULL);
            }
            else if (!newSize && vertexList)
            {
                // List exists, but the requested new size is zero, so
                // delete the existing list
                performerGeoset->setAttr(PFGS_COORD3, performerBinding,
                    NULL, NULL);
                pfMemory::unref(vertexList);
                pfMemory::free(vertexList);
                vertexList = NULL;
            }
            else
            {
                // Either the list is NULL, and the requested size is
                // zero, or the list exists and the new size is non-zero.
                // Modify the length of the existing list using realloc.
                // If the list doesn't exist, the realloc call will do
                // nothing, since the requested size is also zero.
                pfMemory::ref(vertexList);
                performerGeoset->setAttr(PFGS_COORD3, performerBinding,
                    NULL, NULL);
                vertexList = (pfVec3 *)(pfMemory::realloc(vertexList,
                    sizeof(pfVec3) * newSize));
                pfMemory::unref(vertexList);

                // Set the newly-resized vertex list on the pfGeoSet
                performerGeoset->setAttr(PFGS_COORD3, performerBinding,
                    vertexList, NULL);
            }

            // Store the new list size
            vertexListSize = newSize;
            break;

        case VS_GEOMETRY_NORMALS:
            // Determine what we need to do with the data list
            // based on whether or not it currently exists, and
            // the desired new size of the list
            if (newSize && !normalList)
            {
                // No list exists, create a new normal list
                normalList = (pfVec3 *)(pfMemory::malloc(
                    sizeof(pfVec3) * newSize));
                pfMemory::ref(normalList);

                // Set the newly-created normal list on the pfGeoSet
                performerGeoset->setAttr(PFGS_NORMAL3, performerBinding,
                    normalList, NULL);
            }
            else if (!newSize && normalList)
            {
                // List exists, but the requested new size is zero, so
                // delete the existing normal list
                performerGeoset->setAttr(PFGS_NORMAL3, performerBinding,
                    NULL, NULL);
                pfMemory::unref(normalList);
                pfMemory::free(normalList);
                normalList = NULL;
            }
            else
            {
                // Either the list is NULL, and the requested size is
                // zero, or the list exists and the new size is non-zero.
                // Modify the length of the existing list using realloc.
                // If the list doesn't exist, the realloc call will do
                // nothing, since the requested size is also zero.
                pfMemory::ref(normalList);
                performerGeoset->setAttr(PFGS_NORMAL3, performerBinding,
                    NULL, NULL);
                normalList = (pfVec3 *)(pfMemory::realloc(normalList,
                    sizeof(pfVec3) * newSize));
                pfMemory::unref(normalList);

                // Set the newly-resized normal list on the pfGeoSet
                performerGeoset->setAttr(PFGS_NORMAL3, performerBinding,
                    normalList, NULL);
            }

            // Store the new list size
            normalListSize = newSize;
            break;

        case VS_GEOMETRY_COLORS:
            // Determine what we need to do with the data list
            // based on whether or not it currently exists, and
            // the desired new size of the list
            if (newSize && !colorList)
            {
                // No list exists, create a new color list
                colorList = (pfVec4 *)(pfMemory::malloc(
                    sizeof(pfVec4) * newSize));
                pfMemory::ref(colorList);
                                                                                
                // Set the newly-created color list on the pfGeoSet
                performerGeoset->setAttr(PFGS_COLOR4, performerBinding,
                    colorList, NULL);
            }
            else if (!newSize && colorList)
            {
                // List exists, but the requested new size is zero, so
                // delete the existing color list
                performerGeoset->setAttr(PFGS_COLOR4, performerBinding,
                    NULL, NULL);
                pfMemory::unref(colorList);
                pfMemory::free(colorList);
                colorList = NULL;
            }
            else
            {
                // Either the list is NULL, and the requested size is
                // zero, or the list exists and the new size is non-zero.
                // Modify the length of the existing list using realloc.
                // If the list doesn't exist, the realloc call will do
                // nothing, since the requested size is also zero.
                pfMemory::ref(colorList);
                performerGeoset->setAttr(PFGS_COLOR4, performerBinding,
                    NULL, NULL);
                colorList = (pfVec4 *)(pfMemory::realloc(colorList,
                    sizeof(pfVec4) * newSize));
                pfMemory::unref(colorList);
                                                                                
                // Set the newly-resized color list on the pfGeoSet
                performerGeoset->setAttr(PFGS_COLOR4, performerBinding,
                    colorList, NULL);
            }

            // Store the new list size
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
                                                                                
            // Make sure we don't try to set data we don't have a texture
            // unit for
            if (unit >= VS_MAXIMUM_TEXTURE_UNITS)
            {
                printf("vsGeometry::setDataListSize:  Unsupported texture "
                    "unit %d\n", unit);
                return;
            }
                                                                                
            // Determine what we need to do with the data list
            // based on whether or not it currently exists, and
            // the desired new size of the list
            if (newSize && !texCoordList[unit])
            {
                // No list exists, create a new texture coordinate list
                texCoordList[unit] = (pfVec2 *)(pfMemory::malloc(
                    sizeof(pfVec2) * newSize));
                pfMemory::ref(texCoordList[unit]);
                                                                                
                // Set the newly-created texture coordinate list on the
                // pfGeoSet
                performerGeoset->setMultiAttr(PFGS_TEXCOORD2, unit,
                    performerBinding, texCoordList[unit], NULL);
            }
            else if (!newSize && texCoordList[unit])
            {
                // List exists, but the requested new size is zero, so
                // delete the existing texture coordinate list
                performerGeoset->setMultiAttr(PFGS_TEXCOORD2, unit,
                    performerBinding, NULL, NULL);
                pfMemory::unref(texCoordList[unit]);
                pfMemory::free(texCoordList[unit]);
                texCoordList[unit] = NULL;
            }
            else
            {
                // Either the list is NULL, and the requested size is
                // zero, or the list exists and the new size is non-zero.
                // Modify the length of the existing list using realloc.
                // If the list doesn't exist, the realloc call will do
                // nothing, since the requested size is also zero.
                pfMemory::ref(texCoordList[unit]);
                performerGeoset->setMultiAttr(PFGS_TEXCOORD2, unit,
                    performerBinding, NULL, NULL);
                texCoordList[unit] = (pfVec2 *)(pfMemory::realloc(
                    texCoordList[unit], sizeof(pfVec2) * newSize));
                pfMemory::unref(texCoordList[unit]);
                                                                                
                // Set the newly-resized texture coordinate list on the
                // pfGeoSet
                performerGeoset->setMultiAttr(PFGS_TEXCOORD2, unit,
                    performerBinding, texCoordList[unit], NULL);
            }

            // Store the new list size
            texCoordListSize[unit] = newSize;
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
            printf("vsGeometry::setDataListSize: Vertex weights not supported "
                "in vsGeometry.\n");
            printf("    (use vsSkeletonMeshGeometry for vertex skinning.\n");
            break;

        case VS_GEOMETRY_ALT_COLORS:
            printf("vsGeometry::setDataListSize: Alternate colors not "
                "supported under Performer.\n");
            break;

        case VS_GEOMETRY_FOG_COORDS:
            printf("vsGeometry::setDataListSize: Fog coordinates not "
                "supported under Performer.\n");
            break;

        case VS_GEOMETRY_USER_DATA0:
        case VS_GEOMETRY_USER_DATA1:
            printf("vsGeometry::setDataListSize: User data attributes not "
                "supported under Performer.\n");
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
            printf("vsGeometry::setDataListSize: Generic attributes not "
                "supported under Performer.\n");
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
    unsigned int unit;

    // Interpret the whichData constant
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
                                                                                
            // Make sure we don't try to set data we don't have a texture
            // unit for
            if (unit >= VS_MAXIMUM_TEXTURE_UNITS)
            {
                printf("vsGeometry::getDataListSize:  Unsupported texture "
                    "unit %d\n", unit);
                return 0;
            }
                                                                                
            return texCoordListSize[unit];

        case VS_GEOMETRY_VERTEX_WEIGHTS:
            printf("vsGeometry::getDataListSize: Vertex weights not supported "
                "in vsGeometry.\n");
            printf("    (use vsSkeletonMeshGeometry for vertex skinning.\n");
            break;

        case VS_GEOMETRY_ALT_COLORS:
            printf("vsGeometry::getDataListSize: Alternate colors not "
                "supported under Performer.\n");
            break;

        case VS_GEOMETRY_FOG_COORDS:
            printf("vsGeometry::getDataListSize: Fog coordinates not "
                "supported under Performer.\n");
            break;

        case VS_GEOMETRY_USER_DATA0:
        case VS_GEOMETRY_USER_DATA1:
            printf("vsGeometry::getDataListSize: User data attributes not "
                "supported under Performer.\n");
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
            printf("vsGeometry::getDataListSize: Generic attributes not "
                "supported under Performer.\n");
            break;

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

    // Set the pfGeoSet to use the given bin
    performerGeoset->setDrawBin((short)binNum);

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
void vsGeometry::getBoundSphere(vsVector *centerPoint, double *radius)
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
vsMatrix vsGeometry::getGlobalXform()
{
    pfNode *nodePtr;
    pfMatrix xform;
    const pfMatrix *scsMatPtr;
    vsMatrix result;
    int loop, sloop;

    // Start at this geometry's geode with an identity matrix
    xform.makeIdent();
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
            xform.postMult(*scsMatPtr);
        }
        
        // Move to the node's (first) parent
        nodePtr = nodePtr->getParent(0);
    }
    
    // Copy the resulting Performer matrix to a VESS one, transposing as
    // we go
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
    // Set the intersection mask on the Performer node
    performerGeode->setTravMask(PFTRAV_ISECT, newValue, PFTRAV_SELF, PF_SET);
}

// ------------------------------------------------------------------------
// Retrieves the intersection value for this geometry.
// ------------------------------------------------------------------------
unsigned int vsGeometry::getIntersectValue()
{
    // Get the intersection mask from the Performer node
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

    // Look for the given parent in the parent list
    for (loop = 0; loop < parentCount; loop++)
    {
        // See if the current parent is the one we're looking for
        if (targetParent == parentList[loop])
        {
            // Slide the remaining parents in the list down by one
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

    // Done (Performer ignores this function's return value)
    return 0;
}
