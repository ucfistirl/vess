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
// DAC
    performerFlux->setMode(PFFLUX_COPY_LAST_DATA, PF_ON);

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
    originalNormalList = NULL;
    originalVertexList = NULL;
    boneList = NULL;
    boneListSize = 0;
    weightList = NULL;
    weightListSize = 0;

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
    normalBinding = VS_GEOMETRY_BIND_PER_VERTEX;
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
vsSkeletonMeshGeometry::~vsSkeletonMeshGeometry()
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

    if (originalVertexList)
        pfMemory::free(originalVertexList);
                                                                                
    if (originalNormalList)
        pfMemory::free(originalNormalList);
    
    getMap()->removeLink(this, VS_OBJMAP_FIRST_LIST);
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
// Retrieves the number of parents for this node
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
// Begins a new state/frame of the dynamic geometry.  Creates a new GeoSet
// and copies the current state for primitiveType, primitiveCount, and 
// attribute bindings.  The remaining attributes are extracted from the 
// performerFlux and may be in an initial state (set up by 
// pfFluxedGSetInit()) or may be a previous state (written one or more 
// frames ago).  These steps will allow fluxed memory to be reused.  For
// consistent results, attributes (colors, normals, vertex coordinates, and
// texture coordinates) must be updated every frame.  
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::beginNewState()
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
    colorListSize = performerGeoset->getAttrRange(PFGS_COLOR4, NULL, NULL);
    performerGeoset->getAttrLists(PFGS_NORMAL3, (void **)&normalList, &dummy);
    normalListSize = performerGeoset->getAttrRange(PFGS_NORMAL3, NULL, NULL);
    for (unit = 0; unit < VS_MAXIMUM_TEXTURE_UNITS; unit++)
    {
        performerGeoset->getMultiAttrLists(PFGS_TEXCOORD2, unit,
            (void **)&(texCoordList[unit]), &dummy);
        texCoordListSize[unit] =
            performerGeoset->getMultiAttrRange(PFGS_TEXCOORD2, unit,
                NULL, NULL);
    }
    performerGeoset->getAttrLists(PFGS_COORD3, (void **)&vertexList, &dummy);
    vertexListSize = performerGeoset->getAttrRange(PFGS_COORD3, NULL, NULL);

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
void vsSkeletonMeshGeometry::finishNewState()
{
    // Signal the pfFlux that all changes to the current pfGeoSet are 
    // complete
    performerFlux->writeComplete();
}

// ------------------------------------------------------------------------
// Sets the type of geometric primitive that this object contains
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::setPrimitiveType(int newType)
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
            printf("vsSkeletonMeshGeometry::setPrimitiveType: "
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
            printf("vsSkeletonMeshGeometry::setPrimitiveType: "
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
    
    return -1;
}

// ------------------------------------------------------------------------
// Sets the number of geometric primitive that this object contains. Must
// be called before any calls to set the data for any specific primitive.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::setPrimitiveCount(int newCount)
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
void vsSkeletonMeshGeometry::setPrimitiveLengths(int *lengths)
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
void vsSkeletonMeshGeometry::getPrimitiveLengths(int *lengthsBuffer)
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
void vsSkeletonMeshGeometry::setBinding(int whichData, int binding)
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
            printf("vsSkeletonMeshGeometry::setBinding: Unrecognized binding "
                "value\n");
            return;
    }

    // Translate the whichData parameter into its Performer counterpart
    // and alter the pfGeoSet's binding appropriately
    switch (whichData)
    {
        case VS_GEOMETRY_SKIN_VERTEX_COORDS:

            // Skin data can only be per vertex.
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsSkeletonMeshGeometry::setBinding: Skin vertex "
                    "binding must always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }
            break;

        case VS_GEOMETRY_SKIN_NORMALS:

            // Skin data can only be per vertex.
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsSkeletonMeshGeometry::setBinding: Skin normal "
                    "binding must always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }
            break;

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
            performerGeoset->setAttr(PFGS_COORD3, performerBinding,
                vertexList, NULL);
            vertexBinding = performerBinding;
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

            // Make sure we're not trying to affect an invalid texture unit
            if (unit >= VS_MAXIMUM_TEXTURE_UNITS)
            {
                printf("vsGeometry::getBinding:  Unsupported texture "
                    "unit %d\n", unit);
                return;
            }
                                                                                
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
            performerGeoset->setMultiAttr(PFGS_TEXCOORD2, unit, 
                performerBinding, texCoordList[unit], NULL);
            texCoordBinding[unit] = performerBinding;
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

    // Translate the whichData parameter to its VESS counterpart
    switch (whichData)
    {
        case VS_GEOMETRY_SKIN_VERTEX_COORDS:
        case VS_GEOMETRY_SKIN_NORMALS:
        case VS_GEOMETRY_VERTEX_WEIGHTS:
        case VS_GEOMETRY_BONE_INDICES:
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
                                                                                
            // Make sure we're not trying to affect an invalid texture unit
            if (unit >= VS_MAXIMUM_TEXTURE_UNITS)
            {
                printf("vsGeometry::getBinding:  Unsupported texture "
                    "unit %d\n", unit);
                return VS_GEOMETRY_BIND_NONE;
            }
                                                                                
            result = texCoordBinding[unit];
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
            printf("vsSkeletonMeshGeometry::getBinding: Unrecognized data "
                "value\n");
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
void vsSkeletonMeshGeometry::setData(int whichData, int dataIndex,
                                     vsVector data)
{
    unsigned int unit;
    int loop;

    // Make sure the data index is valid
    if (dataIndex < 0)
    {
        printf("vsSkeletonMeshGeometry::setData: Index out of bounds\n");
        return;
    }
    
    // Different actions necessary depending on which data is being set
    switch (whichData)
    {
        case VS_GEOMETRY_SKIN_VERTEX_COORDS:

            // Validate the index
            if (dataIndex >= vertexListSize)
            {
                printf("vsSkeletonMeshGeometry::setData: Index out of "
                    "bounds\n");
                return;
            }

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
                (originalVertexList[dataIndex])[loop] = data[loop];
                (vertexList[dataIndex])[loop] = data[loop];
            }
            break;

        case VS_GEOMETRY_SKIN_NORMALS:

            // Validate the index
            if (dataIndex >= normalListSize)
            {
                printf("vsSkeletonMeshGeometry::setData: Index out of "
                    "bounds\n");
                return;
            }

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
                (originalNormalList[dataIndex])[loop] = data[loop];
                (normalList[dataIndex])[loop] = data[loop];
            }
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:

            if (dataIndex >= weightListSize)
            {
                printf("vsSkeletonMeshGeometry::setData: Index out of"
                   " bounds\n");
                return;
            }
            if (data.getSize() < 4)
            {
                printf("vsSkeletonMeshGeometry::setData: Insufficient data"
                    " (vertex weight require 4 values)\n");
                return;
            }
            for (loop = 0; loop < 4; loop++)
                (weightList[dataIndex])[loop] = data[loop];

            break;

        case VS_GEOMETRY_BONE_INDICES:
            if (dataIndex >= boneListSize)
            {
                printf("vsSkeletonMeshGeometry::setData: Index out of"
                   " bounds\n");
                return;
            }
            if (data.getSize() < 4)
            {
                printf("vsSkeletonMeshGeometry::setData: Insufficient data"
                    " (vertex bone indices require 4 values)\n");
                return;
            }
            for (loop = 0; loop < 4; loop++)
                (boneList[dataIndex])[loop] = data[loop];

            break;

        case VS_GEOMETRY_VERTEX_COORDS:
            printf("vsSkeletonMeshGeometry::setData: Cannot set vertex coords "
                   "they are generated based on bone positions.\n");
            break;

        case VS_GEOMETRY_NORMALS:
            printf("vsSkeletonMeshGeometry::setData: Cannot set normals "
                   "they are generated based on bone positions.\n");
            break;

        case VS_GEOMETRY_COLORS:

            // Validate the index
            if (dataIndex >= colorListSize)
            {
                printf("vsSkeletonMeshGeometry::setData: Index out of "
                    "bounds\n");
                return;
            }

            // Colors require a 4-component vector
            if (data.getSize() < 4)
            {
                printf("vsSkeletonMeshGeometry::setData: Insufficient data "
                    "(colors require 4 values)\n");
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
                                                                                
            // Make sure we're not trying to affect an invalid texture unit
            if (unit >= VS_MAXIMUM_TEXTURE_UNITS)
            {
                printf("vsGeometry::getBinding:  Unsupported texture "
                    "unit %d\n", unit);
                return;
            }

            // Validate the index
            if (dataIndex >= texCoordListSize[unit])
            {
                printf("vsSkeletonMeshGeometry::setData: Index out of "
                    "bounds\n");
                return;
            }

            // Texture coordinates require a 2-component vector
            if (data.getSize() < 2)
            {
                printf("vsSkeletonMeshGeometry::setData: Insufficient data "
                    "(texture coordinates require 2 values)\n");
                return;
            }

            // Copy the data from the vector into the texture coordinate list
            for (loop = 0; loop < 2; loop++)
                (texCoordList[unit][dataIndex])[loop] = data[loop];
            break;

        case VS_GEOMETRY_ALT_COLORS:
            printf("vsGeometry::setData: Alternate colors not supported "
                "under Performer.\n");
            break;
                                                                                
        case VS_GEOMETRY_FOG_COORDS:
            printf("vsGeometry::setData: Fog coordinates not supported "
                "under Performer.\n");
            break;
                                                                                
        case VS_GEOMETRY_USER_DATA0:
            printf("vsGeometry::setData: User data attributes not "
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
            printf("vsGeometry::setData: Generic attributes not supported "
                "under Performer.\n");
            break;

        default:
            printf("vsSkeletonMeshGeometry::setData: Unrecognized data type\n");
            return;
    }
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
    unsigned int unit;
    int loop;

    // Make sure the data index is valid
    if (dataIndex < 0)
    {
        printf("vsSkeletonMeshGeometry::getData: Index out of bounds\n");
        return result;
    }
    
    // Determine which list we should obtain the data from, and return
    // the requested item from that list
    switch (whichData)
    {
        case VS_GEOMETRY_SKIN_VERTEX_COORDS:

            // Validate the index
            if (dataIndex >= vertexListSize)
            {
                printf("vsSkeletonMeshGeometry::getData: Index out of "
                    "bounds\n");
                return result;
            }

            // Set the result vector's size to 3
            result.setSize(3);

            // Copy the vertex in question
            for (loop = 0; loop < 3; loop++)
                result[loop] = (originalVertexList[dataIndex])[loop];
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:

            // Validate the index
            if (dataIndex >= weightListSize)
            {
                printf("vsSkeletonMeshGeometry::getData: Index out of "
                    "bounds\n");
                return result;
            }

            // Set the result vector's size to 4
            result.setSize(4);

            // Copy the vertex in question
            for (loop = 0; loop < 3; loop++)
                result[loop] = (weightList[dataIndex])[loop];
            break;

        case VS_GEOMETRY_BONE_INDICES:

            // Validate the index
            if (dataIndex >= boneListSize)
            {
                printf("vsSkeletonMeshGeometry::getData: Index out of "
                    "bounds\n");
                return result;
            }

            // Set the result vector's size to 4
            result.setSize(4);

            // Copy the vertex in question
            for (loop = 0; loop < 3; loop++)
                result[loop] = (boneList[dataIndex])[loop];
            break;

        case VS_GEOMETRY_SKIN_NORMALS:

            // Validate the index
            if (dataIndex >= normalListSize)
            {
                printf("vsSkeletonMeshGeometry::getData: Index out of "
                    "bounds\n");
                return result;
            }

            // Set the result vector's size to 3
            result.setSize(3);

            // Copy the vertex in question
            for (loop = 0; loop < 3; loop++)
                result[loop] = (originalNormalList[dataIndex])[loop];
            break;

        case VS_GEOMETRY_VERTEX_COORDS:

            // Validate the index
            if (dataIndex >= vertexListSize)
            {
                printf("vsSkeletonMeshGeometry::getData: Index out of "
                    "bounds\n");
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
                printf("vsSkeletonMeshGeometry::getData: Index out of "
                    "bounds\n");
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
                printf("vsSkeletonMeshGeometry::getData: Index out of "
                    "bounds\n");
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

            // Make sure we're not trying to affect an invalid texture unit
            if (unit >= VS_MAXIMUM_TEXTURE_UNITS)
            {
                printf("vsGeometry::getBinding:  Unsupported texture "
                    "unit %d\n", unit);
                return result;
            }
                                                                                
            // Validate the index
            if (dataIndex >= texCoordListSize[unit])
            {
                printf("vsSkeletonMeshGeometry::getData: Index out of "
                    "bounds\n");
                return result;
            }
            
            // Set the result vector's size to 2
            result.setSize(2);

            // Copy the texture coordinate in question 
            for (loop = 0; loop < 2; loop++)
                result[loop] = (texCoordList[unit][dataIndex])[loop];
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
            printf("vsSkeletonMeshGeometry::getData: Unrecognized data type\n");
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
void vsSkeletonMeshGeometry::setDataList(int whichData, vsVector *dataList)
{
    unsigned int unit;
    int loop, sloop;
    
    // Copy the entire data list given to the appropriate geometry data list
    switch (whichData)
    {
        case VS_GEOMETRY_SKIN_VERTEX_COORDS:
            for (loop = 0; loop < vertexListSize; loop++)
                for (sloop = 0; sloop < 3; sloop++)
                {
                    originalVertexList[loop][sloop] = dataList[loop][sloop];
                    vertexList[loop][sloop] = dataList[loop][sloop];
                }
            break;

        case VS_GEOMETRY_SKIN_NORMALS:
            for (loop = 0; loop < normalListSize; loop++)
                for (sloop = 0; sloop < 3; sloop++)
                {
                    originalNormalList[loop][sloop] = dataList[loop][sloop];
                    normalList[loop][sloop] = dataList[loop][sloop];
                }
            break;

        case VS_GEOMETRY_VERTEX_WEIGHTS:
            for (loop = 0; loop < weightListSize; loop++)
                for (sloop = 0; sloop < 4; sloop++)
                    weightList[loop][sloop] = dataList[loop][sloop];

            break;

        case VS_GEOMETRY_BONE_INDICES:
            for (loop = 0; loop < boneListSize; loop++)
                for (sloop = 0; sloop < 4; sloop++)
                    boneList[loop][sloop] = dataList[loop][sloop];

            break;

        case VS_GEOMETRY_VERTEX_COORDS:
            printf("vsSkeletonMeshGeometry::setData: Cannot set vertex coords, "                   "they are generated based on bone positions.\n");
            break;

        case VS_GEOMETRY_NORMALS:
            printf("vsSkeletonMeshGeometry::setData: Cannot set normals, "
                   "they are generated based on bone positions.\n");
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
                                                                                
            // Make sure we're not trying to affect an invalid texture unit
            if (unit >= VS_MAXIMUM_TEXTURE_UNITS)
            {
                printf("vsGeometry::getBinding:  Unsupported texture "
                    "unit %d\n", unit);
                return;
            }
                                                                                
            for (loop = 0; loop < texCoordListSize[unit]; loop++)
                for (sloop = 0; sloop < 2; sloop++)
                    texCoordList[unit][loop][sloop] = dataList[loop][sloop];
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
            printf("vsSkeletonMeshGeometry::setDataList: Unrecognized data "
                "type\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves all of the data points within one of the geometry objects'
// lists, storing that data in the specified dataBuffer. The dataBuffer
// array must be at least as large as the size of particular list in
// question.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::getDataList(int whichData, vsVector *dataBuffer)
{
    unsigned int unit;
    int loop, sloop;
    
    // Copy the entire geometry data list requested to the given buffer
    switch (whichData)
    {
        case VS_GEOMETRY_SKIN_VERTEX_COORDS:
            for (loop = 0; loop < vertexListSize; loop++)
            {
                dataBuffer[loop].setSize(3);
                for (sloop = 0; sloop < 3; sloop++)
                    dataBuffer[loop][sloop] = originalVertexList[loop][sloop];
            }
            break;
        case VS_GEOMETRY_SKIN_NORMALS:
            for (loop = 0; loop < normalListSize; loop++)
            {
                dataBuffer[loop].setSize(3);
                for (sloop = 0; sloop < 3; sloop++)
                    dataBuffer[loop][sloop] = originalNormalList[loop][sloop];
            }
            break;
        case VS_GEOMETRY_VERTEX_WEIGHTS:
            for (loop = 0; loop < weightListSize; loop++)
            {
                dataBuffer[loop].setSize(4);
                for (sloop = 0; sloop < 4; sloop++)
                    dataBuffer[loop][sloop] = weightList[loop][sloop];
            }
            break;
        case VS_GEOMETRY_BONE_INDICES:
            for (loop = 0; loop < boneListSize; loop++)
            {
                dataBuffer[loop].setSize(4);
                for (sloop = 0; sloop < 4; sloop++)
                    dataBuffer[loop][sloop] = boneList[loop][sloop];
            }
            break;
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
                                                                                
            // Make sure we're not trying to affect an invalid texture unit
            if (unit >= VS_MAXIMUM_TEXTURE_UNITS)
            {
                printf("vsGeometry::getBinding:  Unsupported texture "
                    "unit %d\n", unit);
                return;
            }
                                                                                
            for (loop = 0; loop < texCoordListSize[unit]; loop++)
            {
                dataBuffer[loop].setSize(2);
                for (sloop = 0; sloop < 2; sloop++)
                    dataBuffer[loop][sloop] = texCoordList[unit][loop][sloop];
            }
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
        case VS_GEOMETRY_VERTEX_WEIGHTS:

            // Check the current weight list size and the requested new
            // size, and reallocate the list as appropriate
            if (newSize && !weightList)
            {
                // No list exists, create a new list
                weightList = (pfVec4 *)(pfMemory::malloc(
                    sizeof(pfVec4) * newSize));
                pfMemory::ref(weightList);
            }
            else if (!newSize && weightList)
            {
                // List exists, but the requested new size is zero, so
                // delete the existing list
                pfMemory::unref(weightList);
                pfMemory::free(weightList);
                weightList = NULL;
            }
            else
            {
                // Either the list is NULL, and the requested size is
                // zero, or the list exists and the new size is non-zero.
                // Modify the length of the existing list using realloc.
                // If the list doesn't exist, the realloc call will do
                // nothing, since the requested size is also zero.
                weightList = (pfVec4 *)(pfMemory::realloc(weightList,
                    sizeof(pfVec4) * newSize));
            }

            // Store the new list size
            weightListSize = newSize;
            break;

        case VS_GEOMETRY_BONE_INDICES:

            // Check the current bone list size and the requested new
            // size, and reallocate the list as appropriate
            if (newSize && !boneList)
            {
                // No list exists, create a new list
                boneList = (pfVec4 *)(pfMemory::malloc(
                    sizeof(pfVec4) * newSize));
                pfMemory::ref(boneList);
            }
            else if (!newSize && boneList)
            {
                // List exists, but the requested new size is zero, so
                // delete the existing list
                pfMemory::unref(boneList);
                pfMemory::free(boneList);
                boneList = NULL;
            }
            else
            {
                // Either the list is NULL, and the requested size is
                // zero, or the list exists and the new size is non-zero.
                // Modify the length of the existing list using realloc.
                // If the list doesn't exist, the realloc call will do
                // nothing, since the requested size is also zero.
                boneList = (pfVec4 *)(pfMemory::realloc(boneList,
                    sizeof(pfVec4) * newSize));
            }

            // Store the new list size
            boneListSize = newSize;
            break;

        case VS_GEOMETRY_SKIN_VERTEX_COORDS:
        case VS_GEOMETRY_VERTEX_COORDS:

            // Check the current vertex list size and the requested new 
            // size, and reallocate the list as appropriate
            if (newSize && !vertexList)
            {
                // No list exists, create a new list
                vertexList = (pfVec3 *)(pfMemory::malloc(
                    sizeof(pfVec3) * newSize));
                pfMemory::ref(vertexList);

                if (originalVertexList == NULL)
                {
                    originalVertexList = (pfVec3 *)(pfMemory::malloc(
                        sizeof(pfVec3) * newSize));
                    pfMemory::ref(originalVertexList);
                }
                else
                {
                    originalVertexList = (pfVec3 *)(pfMemory::realloc(
                        originalVertexList, sizeof(pfVec3) * newSize));
                }
                                                                                
                // Set the newly-created vertex list on the pfGeoArray
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

                if (originalVertexList != NULL)
                {
                    if (pfMemory::unrefGetRef(originalVertexList) == 0)
                    {
                        pfMemory::free(originalVertexList);
                        originalVertexList = NULL;
                    }
                }
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

                if (originalVertexList == NULL)
                {
                    originalVertexList = (pfVec3 *)(pfMemory::malloc(
                        sizeof(pfVec3) * newSize));
                    pfMemory::ref(originalVertexList);
                }
                else
                    originalVertexList = (pfVec3 *)(pfMemory::realloc(
                        originalVertexList, sizeof(pfVec3) * newSize));

                // Set the newly-resized vertex list on the pfGeoArray
                performerGeoset->setAttr(PFGS_COORD3, performerBinding,
                    vertexList, NULL);
            }

            // Store the new list size
            vertexListSize = newSize;
            break;

        case VS_GEOMETRY_SKIN_NORMALS:
        case VS_GEOMETRY_NORMALS:

            // Check the current normal list size and the requested new 
            // size, and reallocate the list as appropriate
            if (newSize && !normalList)
            {
                // No list exists, create a new normal list
                normalList = (pfVec3 *)(pfMemory::malloc(
                    sizeof(pfVec3) * newSize));
                pfMemory::ref(normalList);

                if (originalNormalList == NULL)
                {
                     originalNormalList = (pfVec3 *)(pfMemory::malloc(
                         sizeof(pfVec3) * newSize));
                     pfMemory::ref(originalNormalList);
                }
                else
                {
                    originalNormalList = (pfVec3 *)(pfMemory::realloc(
                        originalNormalList, sizeof(pfVec3) * newSize));
                }

                // Set the newly-created normal list on the pfGeoArray
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

                if (originalNormalList != NULL)
                {
                    pfMemory::unref(originalNormalList);
                    pfMemory::free(originalNormalList);
                    originalNormalList = NULL;
                }
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
                                                                                
                if (originalNormalList == NULL)
                {
                    originalNormalList = (pfVec3 *)(pfMemory::malloc(
                        sizeof(pfVec3) * newSize));
                    pfMemory::ref(originalNormalList);
                }
                else
                    originalNormalList = (pfVec3 *)(pfMemory::realloc(
                        originalNormalList, sizeof(pfVec3) * newSize));
                                                                                
                // Set the newly-resized normal list on the pfGeoArray
                performerGeoset->setAttr(PFGS_NORMAL3, performerBinding,
                    normalList, NULL);
            }

            // Store the new list size
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
                                                                                
            // Make sure we're not trying to affect an invalid texture unit
            if (unit >= VS_MAXIMUM_TEXTURE_UNITS)
            {
                printf("vsGeometry::getBinding:  Unsupported texture "
                    "unit %d\n", unit);
                return;
            }
                                                                                
            // Check the current texture coordinate list size and the 
            // requested new size, and reallocate the list as appropriate
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

        case VS_GEOMETRY_ALT_COLORS:
            printf("vsGeometry::setDataListSize: Alternate colors not "
                "supported under Performer.\n");
            break;
                                                                                
        case VS_GEOMETRY_FOG_COORDS:
            printf("vsGeometry::setDataListSize: Fog coordinates not "
                "supported under Performer.\n");
            break;
                                                                                
        case VS_GEOMETRY_USER_DATA0:
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

            printf("vsSkeletonMeshGeometry::setDataListSize: Unrecognized data "
                "value\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves the size of one of the object's data lists
// ------------------------------------------------------------------------
int vsSkeletonMeshGeometry::getDataListSize(int whichData)
{
    unsigned int unit;

    // Return the size of the requested data list
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_WEIGHTS:
            return weightListSize;

        case VS_GEOMETRY_BONE_INDICES:
            return boneListSize;

        case VS_GEOMETRY_SKIN_VERTEX_COORDS:
        case VS_GEOMETRY_VERTEX_COORDS:
            return vertexListSize;

        case VS_GEOMETRY_SKIN_NORMALS:
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

            // Make sure we're not trying to affect an invalid texture unit
            if (unit >= VS_MAXIMUM_TEXTURE_UNITS)
            {
                printf("vsGeometry::getBinding:  Unsupported texture "
                    "unit %d\n", unit);
                return 0;
            }
                                                                                
            return texCoordListSize[unit];

        case VS_GEOMETRY_ALT_COLORS:
            printf("vsGeometry::setDataListSize: Alternate colors not "
                "supported under Performer.\n");
            break;
                                                                                
        case VS_GEOMETRY_FOG_COORDS:
            printf("vsGeometry::setDataListSize: Fog coordinates not "
                "supported under Performer.\n");
            break;
                                                                                
        case VS_GEOMETRY_USER_DATA0:
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
            printf("vsSkeletonMeshGeometry::getDataListSize: Unrecognized data "
                "value\n");
    }
    
    // Unknown data list specified, return -1 to indicate error
    return -1;
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
int vsSkeletonMeshGeometry::getRenderBin()
{
    return renderBin;
}

// ------------------------------------------------------------------------
// Retrieves the center point and radius of a sphere that encompasses all
// of the geometry within this object.
// ------------------------------------------------------------------------
void vsSkeletonMeshGeometry::getBoundSphere(vsVector *centerPoint, double *radius)
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
vsMatrix vsSkeletonMeshGeometry::getGlobalXform()
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
// Initializes a pfGeoSet that will occupy one of the data areas of the
// performerFlux object.
// ------------------------------------------------------------------------
int vsSkeletonMeshGeometry::initFluxedGeoSet(pfFluxMemory *fluxMem)
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

    // If all the relevant lists are equal in size, continue to apply.
    if ((vertexListSize == boneListSize) && (boneListSize == weightListSize))
    {
        // Begin a new dynamic geometry state
        beginNewState();

        // For each vertex.
        for (vertexIndex = 0; vertexIndex < vertexListSize; vertexIndex++)
        {
            // Get the original vertex value.
            vertex.set((originalVertexList[vertexIndex])[0],
                (originalVertexList[vertexIndex])[1],
                (originalVertexList[vertexIndex])[2]);

            // Get the original normal value.
            normal.set((originalNormalList[vertexIndex])[0],
                (originalNormalList[vertexIndex])[1],
                (originalNormalList[vertexIndex])[2]);

            // Clear the final matrices.
            finalVertexMatrix.clear();
            finalNormalMatrix.clear();

            // For each data index, AKA possible influences.
            for (dataIndex = 0; dataIndex < 4; dataIndex++)
            {
                // Get the weight for this bone
                weight = weightList[vertexIndex][dataIndex];

                // Get the bone index
                bone = (int)(boneList[vertexIndex][dataIndex]);

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
            (vertexList[vertexIndex])[0] = vertex[0];
            (vertexList[vertexIndex])[1] = vertex[1];
            (vertexList[vertexIndex])[2] = vertex[2];

            // Transform the original normal by the average matrix,
            // re-normalize.
            normal = finalNormalMatrix.getVectorXform(normal);
            normal.normalize();

            // Set the final normal into the normal array list.
            (normalList[vertexIndex])[0] = normal[0];
            (normalList[vertexIndex])[1] = normal[1];
            (normalList[vertexIndex])[2] = normal[2];
        }

        finishNewState();
    }
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
