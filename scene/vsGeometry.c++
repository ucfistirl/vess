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

#include "vsGeometry.h++"

#include <Performer/pf/pfSCS.h>
#include "vsBackfaceAttribute.h++"
#include "vsFogAttribute.h++"
#include "vsMaterialAttribute.h++"
#include "vsShadingAttribute.h++"
#include "vsTextureAttribute.h++"
#include "vsTransparencyAttribute.h++"
#include "vsWireframeAttribute.h++"
#include "vsSystem.h++"

// ------------------------------------------------------------------------
// Default Constructor - Creates a Performer geode and geoset and connects
// them together, sets up empty geometry lists, and registers callbacks.
// ------------------------------------------------------------------------
vsGeometry::vsGeometry()
{
    int loop;

    // Create the Performer objects and chain them together
    performerGeode = new pfGeode();
    performerGeode->ref();

    performerGeoset = new pfGeoSet();
    performerGeoset->ref();
    performerGeode->addGSet(performerGeoset);
    
    performerGeostate = new pfGeoState();
    performerGeostate->ref();
    performerGeoset->setGState(performerGeostate);
    
    // Initialize our list variables
    colorList = NULL;
    colorListSize = 0;
    normalList = NULL;
    normalListSize = 0;
    texCoordList = NULL;
    texCoordListSize = 0;
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
    
    // Create a link between this object and the Performer geode object
    // in our object map
    ((vsSystem::systemObject)->getNodeMap())->registerLink(this,
        performerGeode);
}

// ------------------------------------------------------------------------
// VESS internal function
// Constructor - Creates a geometry object using the data contained within
// the given Performer geometry node
// ------------------------------------------------------------------------
vsGeometry::vsGeometry(pfGeode *targetGeode)
{
    pfGeoSet *geoset;
    pfGeoState *geostate;
    pfFog *fog, *newFog;
    vsFogAttribute *fogAttrib;
    pfMaterial *frontMaterial, *backMaterial, *newBack;
    vsMaterialAttribute *materialAttrib;
    pfTexture *texture;
    pfTexEnv *texEnv, *newTexEnv;
    vsTextureAttribute *texAttrib;
    int transMode, cullMode, shadeMode, wireMode;
    vsTransparencyAttribute *transAttrib;
    vsBackfaceAttribute *backAttrib;
    vsShadingAttribute *shadeAttrib;
    vsWireframeAttribute *wireAttrib;
    ushort *ilist;
    pfVec2 *vec2List;
    pfVec3 *vec3List;
    pfVec4 *vec4List;
    int loop, tResult;
    int attrBind;

    // Store the geode pointer and obtain the geoset pointer
    performerGeode = targetGeode;
    performerGeode->ref();
    performerGeoset = targetGeode->getGSet(0);
    performerGeoset->ref();
    
    // * Retrieve the attribute lists
    
    // Vertex Coordinates
    // Get the vertex coordinate list and index list, if applicable
    performerGeoset->getAttrLists(PFGS_COORD3, (void **)(&vertexList), &ilist);
    if (ilist)
    {
        // Convert from indexed to non-indexed values by selectively
	// copying from the data list to a new list based on the
	// index list indices
        vertexListSize = (pfMemory::getSize(ilist) / sizeof(ushort));
        vec3List = (pfVec3 *)
            (pfMemory::malloc(sizeof(pfVec3) * vertexListSize));
        for (loop = 0; loop < vertexListSize; loop++)
            (vec3List[loop]).copy(vertexList[ilist[loop]]);
        
	// Attach our own list to the geoset
        attrBind = performerGeoset->getAttrBind(PFGS_COORD3);
        performerGeoset->setAttr(PFGS_COORD3, attrBind, (void *)vec3List, NULL);

        // Dispose of the old data list and the index list, if neither
	// is still being used
        if (!(pfMemory::getRef(vertexList)))
            pfMemory::free(vertexList);
        if (!(pfMemory::getRef(ilist)))
            pfMemory::free(ilist);

        // Keep the new data list
        vertexList = vec3List;
    }
    else
    {
        // Calculate the size of the list and store it
        vertexListSize = (pfMemory::getSize(vertexList) / sizeof(pfVec3));
    }

    // Colors
    // Get the color list and index list, if applicable
    performerGeoset->getAttrLists(PFGS_COLOR4, (void **)(&colorList), &ilist);
    if (ilist)
    {
        // Convert from indexed to non-indexed values by selectively
	// copying from the data list to a new list based on the
	// index list indices
        colorListSize = (pfMemory::getSize(ilist) / sizeof(ushort));
        vec4List = (pfVec4 *)
            (pfMemory::malloc(sizeof(pfVec4) * colorListSize));
        for (loop = 0; loop < colorListSize; loop++)
            (vec4List[loop]).copy(colorList[ilist[loop]]);
        
	// Attach our own list to the geoset
        attrBind = performerGeoset->getAttrBind(PFGS_COLOR4);
        performerGeoset->setAttr(PFGS_COLOR4, attrBind, (void *)vec4List, NULL);

        // Dispose of the old data list and the index list, if neither
	// is still being used
        if (!(pfMemory::getRef(colorList)))
            pfMemory::free(colorList);
        if (!(pfMemory::getRef(ilist)))
            pfMemory::free(ilist);

        // Keep the new data list
        colorList = vec4List;
    }
    else
    {
        // Calculate the size of the list and store it
        colorListSize = (pfMemory::getSize(colorList) / sizeof(pfVec4));
    }

    // Normals
    // Get the normal list and index list, if applicable
    performerGeoset->getAttrLists(PFGS_NORMAL3, (void **)(&normalList), &ilist);
    if (ilist)
    {
        // Convert from indexed to non-indexed values by selectively
	// copying from the data list to a new list based on the
	// index list indices
        normalListSize = (pfMemory::getSize(ilist) / sizeof(ushort));
        vec3List = (pfVec3 *)
            (pfMemory::malloc(sizeof(pfVec3) * normalListSize));
        for (loop = 0; loop < normalListSize; loop++)
            (vec3List[loop]).copy(normalList[ilist[loop]]);
        
	// Attach our own list to the geoset
        attrBind = performerGeoset->getAttrBind(PFGS_NORMAL3);
        performerGeoset->setAttr(PFGS_NORMAL3, attrBind, (void *)vec3List,
            NULL);

        // Dispose of the old data list and the index list, if neither
	// is still being used
        if (!(pfMemory::getRef(normalList)))
            pfMemory::free(normalList);
        if (!(pfMemory::getRef(ilist)))
            pfMemory::free(ilist);

        // Keep the new data list
        normalList = vec3List;
    }
    else
    {
        // Calculate the size of the list and store it
        normalListSize = (pfMemory::getSize(normalList) / sizeof(pfVec3));
    }

    // Texture Coordinates
    // Get the texture coordinate list and index list, if applicable
    performerGeoset->getAttrLists(PFGS_TEXCOORD2, (void **)(&texCoordList),
        &ilist); 
    if (ilist)
    {
        // Convert from indexed to non-indexed values by selectively
	// copying from the data list to a new list based on the
	// index list indices
        texCoordListSize = (pfMemory::getSize(ilist) / sizeof(ushort));
        vec2List = (pfVec2 *)
            (pfMemory::malloc(sizeof(pfVec2) * texCoordListSize));
        for (loop = 0; loop < texCoordListSize; loop++)
            (vec2List[loop]).copy(texCoordList[ilist[loop]]);
        
	// Attach our own list to the geoset
        attrBind = performerGeoset->getAttrBind(PFGS_TEXCOORD2);
        performerGeoset->setAttr(PFGS_TEXCOORD2, attrBind, (void *)vec2List,
            NULL);

        // Dispose of the old data list and the index list, if neither
	// is still being used
        if (!(pfMemory::getRef(texCoordList)))
            pfMemory::free(texCoordList);
        if (!(pfMemory::getRef(ilist)))
            pfMemory::free(ilist);

        // Keep the new data list
        texCoordList = vec2List;
    }
    else
    {
        // Calculate the size of the list and store it
        texCoordListSize = (pfMemory::getSize(texCoordList) / sizeof(pfVec2));
    }

    // Get the list of primitive lengths from the Performer geoset
    lengthsList = performerGeoset->getPrimLengths();
    
    // Replace Performer FLAT primitive types with more conventional ones
    inflateFlatGeometry();

    // Create a connection between the Performer geode and this vsGeometry
    // in the node map
    ((vsSystem::systemObject)->getNodeMap())->registerLink(this,
        performerGeode);

    // Obtain the geostate
    geoset = performerGeode->getGSet(0);
    geostate = geoset->getGState();

    // * Interpret the GeoState elements here
    if (geostate)
    {
        // Fog
        fog = (pfFog *)(geostate->getAttr(PFSTATE_FOG));
        if (fog)
        {
            // Create a new pfFog, copy the geostate's fog data into that,
	    // and used it to create a new vsFogAttribute on this geometry
            newFog = new pfFog();
            newFog->copy(fog);
            fogAttrib = new vsFogAttribute(newFog);
            addAttribute(fogAttrib);
        }

        // Material
        frontMaterial = (pfMaterial *)(geostate->getAttr(PFSTATE_FRONTMTL));
        backMaterial = (pfMaterial *)(geostate->getAttr(PFSTATE_BACKMTL));
        if (frontMaterial)
        {
            // Check the object map to see if this material already has
	    // a VESS attribute associated with it
            materialAttrib = (vsMaterialAttribute *)((vsSystem::systemObject)->
                getNodeMap()->mapSecondToFirst(frontMaterial));

            // If there's no associated material attribute, make one
            if (!materialAttrib)
            {
		// VESS materials must always have back sides; create the
		// data for the back from the front if it doesn't already
		// exist.
                if (backMaterial)
                    newBack = backMaterial;
                else
                {
                    newBack = new pfMaterial();
                    newBack->copy(frontMaterial);
                }

                // Create the material attribute from the Performer
		// materials
                materialAttrib = new vsMaterialAttribute(frontMaterial,
                    newBack);

                // Register the Performer material to VESS material
		// attribute connection in the object map
                (vsSystem::systemObject)->getNodeMap()->
                    registerLink(materialAttrib, frontMaterial);
            }

            // Add the material attribute to this geometry
            addAttribute(materialAttrib);
        }
        else
        {
            // Mark that there's no material on the geostate
            materialAttrib = NULL;
        }

        // Texture
        texture = (pfTexture *)(geostate->getAttr(PFSTATE_TEXTURE));
        texEnv = (pfTexEnv *)(geostate->getAttr(PFSTATE_TEXENV));
        if (texture)
        {
            // Check the object map to see if this texture already has
	    // a VESS attribute associated with it
            texAttrib = (vsTextureAttribute *)((vsSystem::systemObject)->
                getNodeMap()->mapSecondToFirst(texture));

            // If there's no associated texture attribute, make one
            if (!texAttrib)
            {
		// VESS textures always have an associated Performer
		// texture environment built in; create one if the
		// geostate doesn't alredy have one.
                if (texEnv)
                    newTexEnv = texEnv;
                else
                    newTexEnv = new pfTexEnv();

                // Create the texture attribute from the Performer
		// texture objects
                texAttrib = new vsTextureAttribute(texture, newTexEnv);

                // Register the Performer texture to VESS texture
		// attribute connection in the object map
                (vsSystem::systemObject)->getNodeMap()->registerLink(texAttrib,
                    texture);
            }

            // Add the texture attribute to this geometry
            addAttribute(texAttrib);
        }
        else
        {
            // Mark that there's no texture on the geostate
            texAttrib = NULL;
        }
    
        // Transparency
        if ((geostate->getInherit() & PFSTATE_TRANSPARENCY) == 0)
        {
            // Get the transparency mode from the Performer geostate,
	    // create a new transparency attribute, and set the attribute
	    // to match the mode from the geostate
            transMode = geostate->getMode(PFSTATE_TRANSPARENCY);
            transAttrib = new vsTransparencyAttribute();

            if (transMode == PFTR_OFF)
                transAttrib->disable();
            else
                transAttrib->enable();

            // Add the transparency attribute to this geometry
            addAttribute(transAttrib);
        }
        else
        {
            // Determine by hand if transparency is needed

            // Start with no transparency
            tResult = 0;

            // Check the material alpha for less-than-full opacity
            if (materialAttrib &&
                (materialAttrib->getAlpha(VS_MATERIAL_SIDE_FRONT) < 1.0))
                tResult = 1;

            // Check the vertex colors for non-opaque values
            if (!tResult)
                for (loop = 0; loop < colorListSize; loop++)
                    if (fabs(colorList[loop][3] - 1.0) > 1E-6)
                        tResult = 1;

            // Scan the texture pixels (if one exists) for transparency
            if (!tResult && texAttrib &&
                (texAttrib->getApplyMode() != VS_TEXTURE_APPLY_DECAL))
                {
                    long texLoop, pixelSize;
                    int xSize, ySize, dFormat;
                    unsigned char *imageData;
                
                    // Get the texture image data and parameters from
		    // the vsTextureAttribute
                    texAttrib->getImage(&imageData, &xSize, &ySize, &dFormat);

                    // Only scan the texture if the format is in RGB mode
		    // and includes alpha values
                    if (dFormat == VS_TEXTURE_DFORMAT_RGBA)
                    {
                        // Search each pixel of the texture for alpha < 1.0
                        pixelSize = ((long)xSize) * ((long)ySize);
                        for (texLoop = 0; texLoop < pixelSize; texLoop++)
                        {
                            // Check the individual alpha value; if it's
			    // less than opaque, set the needs-transparency
			    // flag.
                            if (imageData[(texLoop * 4) + 3] < 255)
                            {
                                tResult = 1;
                                break;
                            }
                        }
                    }
                }

            // If transparency has been determined to be required, create
	    // a new enabled transparency attribute on this geometry
            if (tResult)
            {
                transAttrib = new vsTransparencyAttribute();
                transAttrib->enable();
                addAttribute(transAttrib);
            }
        }

        // Backface (Cull Face)
        if ((geostate->getInherit() & PFSTATE_CULLFACE) == 0)
        {
            // Get the face culling mode from the geostate
            cullMode = geostate->getMode(PFSTATE_CULLFACE);

            // Create a new backface attribute and set its mode based on
	    // the face culling mode
            backAttrib = new vsBackfaceAttribute();
            if (cullMode == PFCF_OFF)
                backAttrib->enable();
            else
                backAttrib->disable();

            // Add the attribute to this geometry
            addAttribute(backAttrib);
        }

        // Shading
        if (((geostate->getInherit() & PFSTATE_SHADEMODEL) == 0) &&
            !(getTypedAttribute(VS_ATTRIBUTE_TYPE_SHADING, 0)))
        {
            // Get the shading mode from the geostate
            shadeMode = geostate->getMode(PFSTATE_SHADEMODEL);

            // Create a new shading attribute and set its mode based on
	    // the shading mode
            shadeAttrib = new vsShadingAttribute();
            if (shadeMode == PFSM_FLAT)
                shadeAttrib->setShading(VS_SHADING_FLAT);
            else
                shadeAttrib->setShading(VS_SHADING_GOURAUD);

            // Add the attribute to this geometry
            addAttribute(shadeAttrib);
        }

        // Wireframe
        if ((geostate->getInherit() & PFSTATE_ENWIREFRAME) == 0)
        {
            // Get the wireframe mode from the geostate
            wireMode = geostate->getMode(PFSTATE_ENWIREFRAME);

            // Create a new wireframe attribute and set its mode based on
	    // the wireframe mode
            wireAttrib = new vsWireframeAttribute();
            if (wireMode == PF_ON)
                wireAttrib->enable();
            else
                wireAttrib->disable();

            // Add the attribute to this geometry
            addAttribute(wireAttrib);
        }

        // At this point, we don't need the geostate anymore
        pfDelete(geostate);
    }

    // Disable forced-flatshaded mode on the geoset
    performerGeoset->setDrawMode(PFGS_FLATSHADE, PF_OFF);

    // Create a brand-new (empty) geostate on this geometry's geoset
    performerGeostate = new pfGeoState();
    performerGeostate->ref();
    performerGeoset->setGState(performerGeostate);
    
    // Set up our lights list
    lightsList = (pfLight **)
        (pfMemory::malloc(sizeof(pfLight *) * PF_MAX_LIGHTS));
    for (loop = 0; loop < PF_MAX_LIGHTS; loop++)
        lightsList[loop] = NULL;

    // Set our callback function as the geostate callback function,
    // and our array of pfLights as the callback data pointer
    performerGeostate->setFuncs(geostateCallback, NULL, lightsList);
}

// ------------------------------------------------------------------------
// Destructor - Disconnects this node from its Performer counterpart
// ------------------------------------------------------------------------
vsGeometry::~vsGeometry()
{
    vsAttribute *attr;
    vsComponent *parent;

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
    if (texCoordList && !(pfMemory::getRef(texCoordList)))
        pfMemory::free(texCoordList);
    if (lengthsList && !(pfMemory::getRef(lengthsList)))
        pfMemory::free(lengthsList);

    // Remove the association between this object and the Performer
    // objects from the object map
    ((vsSystem::systemObject)->getNodeMap())->removeLink(this,
        VS_OBJMAP_FIRST_LIST);
}

// ------------------------------------------------------------------------
// Retrieves the type of this node
// ------------------------------------------------------------------------
int vsGeometry::getNodeType()
{
    return VS_NODE_TYPE_GEOMETRY;
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

    // Set the binding value
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
        case VS_GEOMETRY_TEXTURE_COORDS:
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
            performerGeoset->setAttr(PFGS_TEXCOORD2, performerBinding,
                texCoordList, NULL);
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
        case VS_GEOMETRY_TEXTURE_COORDS:
            result = performerGeoset->getAttrBind(PFGS_TEXCOORD2);
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
        case VS_GEOMETRY_TEXTURE_COORDS:
            // Bounds check
            if (dataIndex >= texCoordListSize)
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
                (texCoordList[dataIndex])[loop] = data[loop];
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

    // Bounds check
    if (dataIndex < 0)
    {
        printf("vsGeometry::getData: Index out of bounds\n");
        return result;
    }
    
    // Interpret the whichData constant
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            // Bounds check
            if (dataIndex >= vertexListSize)
            {
                printf("vsGeometry::getData: Index out of bounds\n");
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
                printf("vsGeometry::getData: Index out of bounds\n");
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
                printf("vsGeometry::getData: Index out of bounds\n");
                return result;
            }
            // Copy the data to the result vector
            result.setSize(4);
            for (loop = 0; loop < 4; loop++)
                result[loop] = (colorList[dataIndex])[loop];
            break;
        case VS_GEOMETRY_TEXTURE_COORDS:
            // Bounds check
            if (dataIndex >= texCoordListSize)
            {
                printf("vsGeometry::getData: Index out of bounds\n");
                return result;
            }
            // Copy the data to the result vector
            result.setSize(2);
            for (loop = 0; loop < 2; loop++)
                result[loop] = (texCoordList[dataIndex])[loop];
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
        case VS_GEOMETRY_TEXTURE_COORDS:
            for (loop = 0; loop < texCoordListSize; loop++)
                for (sloop = 0; sloop < 2; sloop++)
                    texCoordList[loop][sloop] = dataList[loop][sloop];
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
        case VS_GEOMETRY_TEXTURE_COORDS:
            for (loop = 0; loop < texCoordListSize; loop++)
            {
                // Copy the data to the vector buffer
                dataBuffer[loop].setSize(2);
                for (sloop = 0; sloop < 2; sloop++)
                    dataBuffer[loop][sloop] = texCoordList[loop][sloop];
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
            // Set the new list size on the Performer geoset
            performerGeoset->setAttr(PFGS_COORD3, performerBinding,
                vertexList, NULL);
            vertexListSize = newSize;
            break;
        case VS_GEOMETRY_NORMALS:
            // Determine what we need to do with the data list
	    // based on whether or not it currently exists, and
	    // the desired new size of the list
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
            // Set the new list size on the Performer geoset
            performerGeoset->setAttr(PFGS_NORMAL3, performerBinding,
                normalList, NULL);
            normalListSize = newSize;
            break;
        case VS_GEOMETRY_COLORS:
            // Determine what we need to do with the data list
	    // based on whether or not it currently exists, and
	    // the desired new size of the list
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
            // Set the new list size on the Performer geoset
            performerGeoset->setAttr(PFGS_COLOR4, performerBinding,
                colorList, NULL);
            colorListSize = newSize;
            break;
        case VS_GEOMETRY_TEXTURE_COORDS:
            // Determine what we need to do with the data list
	    // based on whether or not it currently exists, and
	    // the desired new size of the list
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
            // Set the new list size on the Performer geoset
            performerGeoset->setAttr(PFGS_TEXCOORD2, performerBinding,
                texCoordList, NULL);
            texCoordListSize = newSize;
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
    // Interpret the whichData constant
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
// Sets the visibility value for this geometry. During the culling portion
// of a frame drawing cycle, a bitwise AND of the pane's visibility mask
// and the node's visibility value is performed; if the result of the AND
// is zero, the node (and all other nodes under it) are culled, not to be
// drawn.
// ------------------------------------------------------------------------
void vsGeometry::setVisibilityValue(unsigned int newValue)
{
    // Set the visibility mask on the Performer node
    performerGeode->setTravMask(PFTRAV_DRAW, newValue, PFTRAV_SELF, PF_SET);
}

// ------------------------------------------------------------------------
// Retrieves the visibility value for this geometry.
// ------------------------------------------------------------------------
unsigned int vsGeometry::getVisibilityValue()
{
    // Get the visibility mask from the Performer node
    return (performerGeode->getTravMask(PFTRAV_DRAW));
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
pfGeode *vsGeometry::getBaseLibraryObject()
{
    return performerGeode;
}

// ------------------------------------------------------------------------
// Private function
// Manipulates pfGeoSets that contain FLAT primitive types into a format
// that's easier for VESS to handle
// ------------------------------------------------------------------------
void vsGeometry::inflateFlatGeometry()
{
    int loop, primLoop;
    int jumpCount;
    int oldPosition, newPosition;
    int binding;
    int listSum;
    pfVec4 *newColors;
    pfVec3 *newNormals;
    vsShadingAttribute *shading;

    // Determine how much data needs to be replicated based on the
    // flat primitive type
    switch (performerGeoset->getPrimType())
    {
        case PFGS_FLAT_LINESTRIPS:
            jumpCount = 1;
            break;
        case PFGS_FLAT_TRISTRIPS:
        case PFGS_FLAT_TRIFANS:
            jumpCount = 2;
            break;
        default:
            return;
    }
    
    // Calculate the number of vertices in the geometry, based on the
    // lengths of each primitive as stored in the primitive lengths list
    listSum = 0;
    for (loop = 0; loop < getPrimitiveCount(); loop++)
        listSum += lengthsList[loop];
    
    // Inflate the color data, if present and bound per-vertex
    binding = performerGeoset->getAttrBind(PFGS_COLOR4);
    if (binding == PFGS_PER_VERTEX)
    {
	// Run through the list of colors, adding extra values in to
	// compensate for what the flat primitive type leaves out; this
	// is done once per primitive.

	// Start at the beginning for both lists
        oldPosition = 0;
        newPosition = 0;

	// Create a new list
        newColors = (pfVec4 *)(pfMemory::malloc(sizeof(pfVec4) * listSum));

	// Copy values from the old list to the new one
        for (primLoop = 0; primLoop < getPrimitiveCount(); primLoop++)
        {
	    // Replicate a number of data values equal to the number
	    // of values the flat primitive type normally omits
            for (loop = 0; loop < jumpCount; loop++)
                (newColors[newPosition++]).copy(colorList[oldPosition]);

	    // Then copy the rest of the data values of the primitive
            for (loop = 0; loop < (lengthsList[primLoop] - jumpCount); loop++)
                (newColors[newPosition++]).copy(colorList[oldPosition++]);
        }

	// Bind the new color list to the geoset
        performerGeoset->setAttr(PFGS_COLOR4, binding, (void *)newColors, NULL);
        pfMemory::free(colorList);

        // Store the new color list and list size
        colorList = newColors;
        colorListSize = listSum;
    }
    
    // Inflate the normal data, if present and bound per-vertex
    binding = performerGeoset->getAttrBind(PFGS_NORMAL3);
    if (binding == PFGS_PER_VERTEX)
    {
	// Run through the list of normals, adding extra values in to
	// compensate for what the flat primitive type leaves out; this
	// is done once per primitive.

	// Start at the beginning for both lists
        oldPosition = 0;
        newPosition = 0;

	// Create a new list
        newNormals = (pfVec3 *)(pfMemory::malloc(sizeof(pfVec3) * listSum));

	// Copy values from the old list to the new one
        for (primLoop = 0; primLoop < getPrimitiveCount(); primLoop++)
        {
	    // Replicate a number of data values equal to the number
	    // of values the flat primitive type normally omits
            for (loop = 0; loop < jumpCount; loop++)
                (newNormals[newPosition++]).copy(normalList[oldPosition]);

	    // Then copy the rest of the data values of the primitive
            for (loop = 0; loop < (lengthsList[primLoop] - jumpCount); loop++)
                (newNormals[newPosition++]).copy(normalList[oldPosition++]);
        }

	// Bind the new normal list to the geoset
        performerGeoset->setAttr(PFGS_NORMAL3, binding, (void *)newNormals,
            NULL);
        pfMemory::free(normalList);

        // Store the new color list and list size
        normalList = newNormals;
        normalListSize = listSum;
    }
    
    // Create and add a shading attribute to this geometry to compensate for
    // the loss of the 'FLAT' primitive type
    shading = new vsShadingAttribute();
    shading->setShading(VS_SHADING_FLAT);
    addAttribute(shading);

    // Correct the primitive type
    switch (performerGeoset->getPrimType())
    {
        case PFGS_FLAT_LINESTRIPS:
            performerGeoset->setPrimType(PFGS_LINESTRIPS);
            break;
        case PFGS_FLAT_TRISTRIPS:
            performerGeoset->setPrimType(PFGS_TRISTRIPS);
            break;
        case PFGS_FLAT_TRIFANS:
            performerGeoset->setPrimType(PFGS_TRIFANS);
            break;
    }
}

// ------------------------------------------------------------------------
// VESS internal function
// Searches this node for the idx'th occurrence of a node with the given
// name. The idx value is decremented after each match; success only occurs
// once idx reaches zero. Returns a pointer to this node if a match is
// found and idx is zero, NULL otherwise.
// ------------------------------------------------------------------------
vsNode *vsGeometry::nodeSearch(const char *name, int *idx)
{
    // Check if this is the node we're looking for
    if (!strcmp(name, getName()))
    {
	// Check if this is the desired instance of the nodes with the
	// target name by examining the idx value
        if ((*idx) > 0)
        {
	    // The name is right but this is still the wrong one; decrement
	    // the idx value to mark that we've found one, and keep
	    // searching
            (*idx)--;
            return NULL;
        }
        else
            return this;
    }

    // NULL result if the node was not found
    return NULL;
}

// ------------------------------------------------------------------------
// VESS internal function
// Calls the apply function on all attached attributes, and then calls the
// system's graphics state object to affect the changes to the graphics
// library state.
// ------------------------------------------------------------------------
void vsGeometry::applyAttributes()
{
    // Call the inherited version of this function
    vsNode::applyAttributes();
    
    // Call the system's vsGraphicsState object to configure the
    // Performer geostate on this geometry
    (vsSystem::systemObject)->getGraphicsState()->applyState(
        performerGeostate);
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
