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
#include <Performer/pf/pfNode.h>
#include <Performer/pf/pfTraverser.h>
#include "vsComponent.h++"
#include "vsShadingAttribute.h++"
#include "vsGraphicsState.h++"

// ------------------------------------------------------------------------
// Default Constructor - Creates a Performer geode and geoset and connects
// them together, sets up empty geometry lists, and registers callbacks.
// ------------------------------------------------------------------------
vsDynamicGeometry::vsDynamicGeometry() : parentList(5, 5)
{
    unsigned int unit;
    int loop;

    // Start with no parents
    parentCount = 0;

    // Create the pfGeode
    performerGeode = new pfGeode();
    performerGeode->ref();

    // Create a pfFlux for the pfGeoSets to manage the changes in the 
    // pfGeoSets as they progress through the different processes in the 
    // Performer pipeline
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
    vertexList = NULL;
    vertexListSize = 0;
    lengthsList = NULL;

    // Initialize the texture lists and bindings
    for (unit = 0; unit < VS_MAXIMUM_TEXTURE_UNITS; unit++)
    {
        texCoordList[unit] = NULL;
        texCoordListSize[unit] = 0;
        texCoordBinding[unit] = VS_GEOMETRY_BIND_NONE;
    }

    // Initialize the primitive count to zero
    setPrimitiveCount(0);
    
    // Initialize the attribute bindings
    colorBinding = VS_GEOMETRY_BIND_NONE;
    normalBinding = VS_GEOMETRY_BIND_NONE;
    vertexBinding = VS_GEOMETRY_BIND_PER_VERTEX;
    
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
    // Remove all parents
    detachFromParents();

    // Remove all attributes
    deleteAttributes();

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
    unsigned int   unit;
    int            min;
    int            *lengths;

    // Get the first writable flux buffer and cast it to a pfGeoSet
    performerGeoset = (pfGeoSet *)performerFlux->getWritableData();

    // Set the primitive type and number of primitives
    performerGeoset->setPrimType(primitiveType);
    performerGeoset->setNumPrims(primitiveCount);

    // Retrieve the existing attribute data lists 
    performerGeoset->getAttrLists(PFGS_COLOR4, (void **)&colorList, &dummy);
    performerGeoset->getAttrRange(PFGS_COLOR4, &min, &colorListSize);
    performerGeoset->getAttrLists(PFGS_NORMAL3, (void **)&normalList, &dummy);
    performerGeoset->getAttrRange(PFGS_NORMAL3, &min, &normalListSize);
    for (unit = 0; unit < VS_MAXIMUM_TEXTURE_UNITS; unit++)
    {
        performerGeoset->getMultiAttrLists(PFGS_TEXCOORD2, unit,
            (void **)&(texCoordList[unit]), &dummy);
        performerGeoset->getMultiAttrRange(PFGS_TEXCOORD2, unit, &min,
            &(texCoordListSize[unit]));
    }
    performerGeoset->getAttrLists(PFGS_COORD3, (void **)&vertexList, &dummy);
    performerGeoset->getAttrRange(PFGS_COORD3, &min, &vertexListSize);

    // Copy the lengths list from the pfGeoSet, if it has one
    lengths = performerGeoset->getPrimLengths();
    if (lengths == NULL)
        lengthsList = NULL;
    else
        memcpy(lengthsList, lengths, sizeof(int) * primitiveCount);

    // Set the attribute bindings
    setBinding(VS_GEOMETRY_COLORS, colorBinding);
    setBinding(VS_GEOMETRY_NORMALS, normalBinding);
    for (unit = 0; unit < VS_MAXIMUM_TEXTURE_UNITS; unit++)
        setBinding((VS_GEOMETRY_TEXTURE0_COORDS+unit), texCoordBinding[unit]);
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
    // Signal the pfFlux that all changes to the current pfGeoSet are 
    // complete
    performerFlux->writeComplete();
}

// ------------------------------------------------------------------------
// Sets the type of geometric primitive that this object contains
// ------------------------------------------------------------------------
void vsDynamicGeometry::setPrimitiveType(int newType)
{
    // Translate the VESS primitive type to the performer counterpart
    // and set the Performer GeoSet to use it
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
    
    return -1;
}

// ------------------------------------------------------------------------
// Sets the number of geometric primitive that this object contains. Must
// be called before any calls to set the data for any specific primitive.
// ------------------------------------------------------------------------
void vsDynamicGeometry::setPrimitiveCount(int newCount)
{
    // Set the primitive count on the pfGeoSet to the new value
    performerGeoset->setNumPrims(newCount);

    // Store the new count
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
    
    // Update the lengths array on the pfGeoSet
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
    // Validate the index parameter
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

    // Change the appropriate length
    lengthsList[index] = length;
}

// ------------------------------------------------------------------------
// Retrieves the number of verticies specified for the primitive with the
// indicated index in the object. The index of the first primitive is 0.
// ------------------------------------------------------------------------
int vsDynamicGeometry::getPrimitiveLength(int index)
{
    // Validate the index parameter
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

    // Return the given primitive length
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

    // Set all the primitive lengths
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
    
    // Get all the primitive lengths in the Geometry object and
    // return them in the parameter
    for (loop = 0; loop < primitiveCount; loop++)
    {
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
void vsDynamicGeometry::setBinding(int whichData, int binding)
{
    unsigned int unit;
    int performerBinding;
    
    // Translate the binding type parameters into its Performer counterpart
    // act accordingly
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
            printf("vsDynamicGeometry::setBinding: Unrecognized binding "
                "value\n");
            return;
    }

    // Translate the whichData parameter into its Performer counterpart
    // and alter the pfGeoSet's binding appropriately
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:

            // Vertex coordinate binding must be per-vertex (no other
            // binding makes sense)
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsDynamicGeometry::setBinding: Vertex coordinate "
                    "binding must always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }

            // Set the vertex coordinate binding to the given value
            performerGeoset->setAttr(PFGS_COORD3, performerBinding,
                vertexList, NULL);
            vertexBinding = performerBinding;
            break;

        case VS_GEOMETRY_NORMALS:

            // Set the normal binding to the given value
            performerGeoset->setAttr(PFGS_NORMAL3, performerBinding,
                normalList, NULL);
            normalBinding = performerBinding;
            break;

        case VS_GEOMETRY_COLORS:

            // Set the color binding to the given value
            performerGeoset->setAttr(PFGS_COLOR4, performerBinding,
                colorList, NULL);
            colorBinding = performerBinding;
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
                printf("vsDynamicGeometry::setBinding: Texture coordinates "
                    "binding must be either VS_GEOMETRY_BIND_PER_VERTEX or "
                    "VS_GEOMETRY_BIND_NONE\n");
                return;
            }

            // Set the texture coordinate binding to the given value
            performerGeoset->setMultiAttr(PFGS_TEXCOORD2, unit, 
                performerBinding, texCoordList[unit], NULL);
            texCoordBinding[unit] = performerBinding;
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

    // Translate the whichData parameter to its VESS counterpart
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
                                                                                
            result = texCoordBinding[unit];
            break;
        default:
            printf("vsDynamicGeometry::getBinding: Unrecognized data value\n");
            return -1;
    }
    
    // Return the appropriate VESS binding value for the given data list
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
    
    // Return -1, indicating there was a problem
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

    // Make sure the data index is valid
    if (dataIndex < 0)
    {
        printf("vsDynamicGeometry::setData: Index out of bounds\n");
        return;
    }
    
    // Different actions necessary depending on which data is being set
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:

            // Validate the index
            if (dataIndex >= vertexListSize)
            {
                printf("vsDynamicGeometry::setData: Index out of bounds\n");
                return;
            }

            // Vertex coordinates require a 3-component vector
            if (data.getSize() < 3)
            {
                printf("vsDynamicGeometry::setData: Insufficient data (vertex "
                    "coordinates require 3 values)\n");
                return;
            }

            // Copy the data from the vector into the vertex list
            for (loop = 0; loop < 3; loop++)
                (vertexList[dataIndex])[loop] = data[loop];
            break;

        case VS_GEOMETRY_NORMALS:

            // Validate the index
            if (dataIndex >= normalListSize)
            {
                printf("vsDynamicGeometry::setData: Index out of bounds\n");
                return;
            }

            // Normals require a 3-component vector
            if (data.getSize() < 3)
            {
                printf("vsDynamicGeometry::setData: Insufficient data (vertex "
                    "normals require 3 values)\n");
                return;
            }

            // Copy the data from the vector into the normal list
            for (loop = 0; loop < 3; loop++)
                (normalList[dataIndex])[loop] = data[loop];
            break;

        case VS_GEOMETRY_COLORS:

            // Validate the index
            if (dataIndex >= colorListSize)
            {
                printf("vsDynamicGeometry::setData: Index out of bounds\n");
                return;
            }

            // Colors require a 4-component vector
            if (data.getSize() < 4)
            {
                printf("vsDynamicGeometry::setData: Insufficient data (colors "
                    "require 4 values)\n");
                return;
            }

            // Copy the data from the vector into the color list
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
                                                                                
            // Validate the index
            if (dataIndex >= texCoordListSize[unit])
            {
                printf("vsDynamicGeometry::setData: Index out of bounds\n");
                return;
            }

            // Texture coordinates require a 2-component vector
            if (data.getSize() < 2)
            {
                printf("vsDynamicGeometry::setData: Insufficient data (texture "
                    "coordinates require 2 values)\n");
                return;
            }

            // Copy the data from the vector into the texture coordinate list
            for (loop = 0; loop < 2; loop++)
                (texCoordList[unit][dataIndex])[loop] = data[loop];
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

    // Make sure the data index is valid
    if (dataIndex < 0)
    {
        printf("vsDynamicGeometry::getData: Index out of bounds\n");
        return result;
    }
    
    // Determine which list we should obtain the data from, and return
    // the requested item from that list
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:

            // Validate the index
            if (dataIndex >= vertexListSize)
            {
                printf("vsDynamicGeometry::getData: Index out of bounds\n");
                return result;
            }
            
            // Set the result vector's size to 3
            result.setSize(3);

            // Copy the vertex in question 
            for (loop = 0; loop < 3; loop++)
                result[loop] = (vertexList[dataIndex])[loop];
            break;

        case VS_GEOMETRY_NORMALS:

            // Validate the index
            if (dataIndex >= normalListSize)
            {
                printf("vsDynamicGeometry::getData: Index out of bounds\n");
                return result;
            }
            
            // Set the result vector's size to 3
            result.setSize(3);

            // Copy the normal in question 
            for (loop = 0; loop < 3; loop++)
                result[loop] = (normalList[dataIndex])[loop];
            break;

        case VS_GEOMETRY_COLORS:

            // Validate the index
            if (dataIndex >= colorListSize)
            {
                printf("vsDynamicGeometry::getData: Index out of bounds\n");
                return result;
            }
            
            // Set the result vector's size to 3
            result.setSize(4);

            // Copy the color in question 
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

            // Validate the index
            if (dataIndex >= texCoordListSize[unit])
            {
                printf("vsDynamicGeometry::getData: Index out of bounds\n");
                return result;
            }
            
            // Set the result vector's size to 2
            result.setSize(2);

            // Copy the texture coordinate in question 
            for (loop = 0; loop < 2; loop++)
                result[loop] = (texCoordList[unit][dataIndex])[loop];
            break;

        default:
            printf("vsDynamicGeometry::getData: Unrecognized data type\n");
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
void vsDynamicGeometry::setDataList(int whichData, vsVector *dataList)
{
    unsigned int unit;
    int loop, sloop;
    
    // Copy the entire data list given to the appropriate geometry data list
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
                                                                                
            for (loop = 0; loop < texCoordListSize[unit]; loop++)
                for (sloop = 0; sloop < 2; sloop++)
                    texCoordList[unit][loop][sloop] = dataList[loop][sloop];
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
    
    // Copy the entire geometry data list requested to the given buffer
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
                    dataBuffer[loop][sloop] = texCoordList[unit][loop][sloop];
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
    int binding, performerBinding;
    
    // Get the requested list's data binding (required by Performer's
    // setAttr() method for its pfGeoSet data lists)
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

    // Resize the performer attribute lists to the requested size
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:

            // Check the current vertex list size and the requested new 
            // size, and reallocate the list as appropriate
            if (newSize && !vertexList)
            {
                // No list exists, create a new list
                vertexList = (pfVec3 *)(pfMemory::malloc(
                    sizeof(pfVec3) * newSize));
            }
            else if (!newSize && vertexList)
            {
                // List exists, but the requested new size is zero, so
                // delete the existing list
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
                vertexList = (pfVec3 *)(pfMemory::realloc(vertexList,
                    sizeof(pfVec3) * newSize));
            }

            // Set the newly-resized vertex list on the pfGeoSet
            performerGeoset->setAttr(PFGS_COORD3, performerBinding,
                vertexList, NULL);
            vertexListSize = newSize;
            break;

        case VS_GEOMETRY_NORMALS:

            // Check the current normal list size and the requested new 
            // size, and reallocate the list as appropriate
            if (newSize && !normalList)
            {
                // No list exists, create a new normal list
                normalList = (pfVec3 *)(pfMemory::malloc(
                    sizeof(pfVec3) * newSize));
            }
            else if (!newSize && normalList)
            {
                // List exists, but the requested new size is zero, so
                // delete the existing normal list
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
                normalList = (pfVec3 *)(pfMemory::realloc(normalList,
                    sizeof(pfVec3) * newSize));
            }

            // Set the newly-resized normal list on the pfGeoSet
            performerGeoset->setAttr(PFGS_NORMAL3, performerBinding,
                normalList, NULL);
            normalListSize = newSize;
            break;

        case VS_GEOMETRY_COLORS:

            // Check the current color list size and the requested new 
            // size, and reallocate the list as appropriate
            if (newSize && !colorList)
            {
                // No list exists, create a new color list
                colorList = (pfVec4 *)(pfMemory::malloc(
                    sizeof(pfVec4) * newSize));
            }
            else if (!newSize && colorList)
            {
                // List exists, but the requested new size is zero, so
                // delete the existing color list
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
                colorList = (pfVec4 *)(pfMemory::realloc(colorList,
                    sizeof(pfVec4) * newSize));
            }

            // Set the newly-resized color list on the pfGeoSet
            performerGeoset->setAttr(PFGS_COLOR4, performerBinding,
                colorList, NULL);
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
                                                                                
            // Check the current texture coordinate list size and the 
            // requested new size, and reallocate the list as appropriate
            if (newSize && !texCoordList[unit])
            {
                // No list exists, create a new texture coordinate list
                texCoordList[unit] = (pfVec2 *)(pfMemory::malloc(
                    sizeof(pfVec2) * newSize));
            }
            else if (!newSize && texCoordList[unit])
            {
                // List exists, but the requested new size is zero, so
                // delete the existing texture coordinate list
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
                texCoordList[unit] = (pfVec2 *)(pfMemory::realloc(
                    texCoordList[unit], sizeof(pfVec2) * newSize));
            }

            // Set the newly-resized texture coordinate list on the pfGeoSet
            performerGeoset->setMultiAttr(PFGS_TEXCOORD2, unit,
                performerBinding, texCoordList[unit], NULL);
            texCoordListSize[unit] = newSize;
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

    // Return the size of the requested data list
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
    
    // Unknown data list specified, return -1 to indicate error
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
bool vsDynamicGeometry::isLightingEnabled()
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
void vsDynamicGeometry::setRenderBin(int binNum)
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
    
    // Get the geode's pfSphere
    performerGeode->getBound(&boundSphere);
    
    // Set the center point (if requested)
    if (centerPoint)
        centerPoint->set(boundSphere.center[PF_X], boundSphere.center[PF_Y],
            boundSphere.center[PF_Z]);

    // Set the radius (if requested)
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

    // Create an identity matrix
    xform.makeIdent();

    // Start the node pointer at the pfGeode
    nodePtr = performerGeode;
    
    // Loop until we reach the top of the scene graph
    while (nodePtr->getNumParents() > 0)
    {
        // Accumulate all transformations along the way
        if (nodePtr->isOfType(pfSCS::getClassType()))
        {
            // Get the matrix from this transform node
            scsMatPtr = ((pfSCS *)nodePtr)->getMatPtr();

            // Multiply it by the accumulated matrix
            xform.postMult(*scsMatPtr);
        }
        
        // Move to the (first) parent of this node
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
void vsDynamicGeometry::setIntersectValue(unsigned int newValue)
{
    // Set the mask of the Performer intersection traversal for this node
    // to the given value.
    performerGeode->setTravMask(PFTRAV_ISECT, newValue, PFTRAV_SELF, PF_SET);
}

// ------------------------------------------------------------------------
// Retrieves the intersection value for this geometry.
// ------------------------------------------------------------------------
unsigned int vsDynamicGeometry::getIntersectValue()
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
void vsDynamicGeometry::addAttribute(vsAttribute *newAttribute)
{
    int attrCat, attrType;
    int loop;

    // Make sure we can attach this attribute
    if (!(newAttribute->canAttach()))
    {
        printf("vsDynamicGeometry::addAttribute: Attribute is already in "
            "use\n");
        return;
    }
    
    // Make sure this attribute isn't a state attribute (these don't belong
    // on geometry nodes)
    attrCat = newAttribute->getAttributeCategory();
    if (attrCat != VS_ATTRIBUTE_CATEGORY_STATE)
    {
        printf("vsDynamicGeometry::addAttribute: Geometry nodes may not "
            "contain attributes of that type\n");
        return;
    }
    
    // Make sure we don't already have an attribute of that type
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
// Enables culling (view frustum and otherwise) on this node
// ------------------------------------------------------------------------
void vsDynamicGeometry::enableCull()
{
    performerGeode->setTravMask(PFTRAV_CULL, 0xFFFFFFFF, 
        PFTRAV_SELF | PFTRAV_DESCEND, PF_SET);
}

// ------------------------------------------------------------------------
// Disables culling (view frustum and otherwise) on this node
// ------------------------------------------------------------------------
void vsDynamicGeometry::disableCull()
{
    performerGeode->setTravMask(PFTRAV_CULL, 0x0, PFTRAV_SELF | PFTRAV_DESCEND,
        PF_SET);
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
    // If the fluxMemory is NULL, return the size of a fluxed pfGeoSet.
    // This is standard procedure for Performer fluxes (see the man page
    // for pfFlux::pfFlux()).
    if (fluxMem == NULL)
        return pfFluxedGSetInit(fluxMem);

    // Initialize the fluxMemory to a pfGeoSet
    pfFluxedGSetInit(fluxMem);

    // Return 0 if the pfFluxMemory is valid and we have initialized it
    return 0;
}

// ------------------------------------------------------------------------
// Internal function
// Adds a node to this node's list of parent nodes
// ------------------------------------------------------------------------
bool vsDynamicGeometry::addParent(vsNode *newParent)
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
bool vsDynamicGeometry::removeParent(vsNode *targetParent)
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
void vsDynamicGeometry::applyAttributes()
{
    // Call the parent class method
    vsNode::applyAttributes();
    
    // Apply the current vsGraphicsState settings to this object's
    // pfGeoState
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
