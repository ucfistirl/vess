// File vsGeometry.c++

#include "vsGeometry.h++"

#include "vsBackfaceAttribute.h++"
#include "vsFogAttribute.h++"
#include "vsMaterialAttribute.h++"
#include "vsShadingAttribute.h++"
#include "vsTextureAttribute.h++"
#include "vsTransparencyAttribute.h++"
#include "vsSystem.h++"

// ------------------------------------------------------------------------
// Default Constructor - Creates a Performer geode and geoset and connects
// them together, sets up empty geometry lists, and registers callbacks.
// ------------------------------------------------------------------------
vsGeometry::vsGeometry()
{
    int loop;

    performerGeode = new pfGeode();
    performerGeode->ref();

    performerGeoset = new pfGeoSet();
    performerGeoset->ref();
    performerGeode->addGSet(performerGeoset);
    setPrimitiveCount(0);
    
    performerGeostate = new pfGeoState();
    performerGeostate->ref();
    performerGeoset->setGState(performerGeostate);
    
    colorList = NULL;
    colorListSize = 0;
    normalList = NULL;
    normalListSize = 0;
    texCoordList = NULL;
    texCoordListSize = 0;
    vertexList = NULL;
    vertexListSize = 0;
    lengthsList = NULL;
    
    lightsList = (pfLight **)
	(pfMemory::malloc(sizeof(pfLight *) * PF_MAX_LIGHTS));
    for (loop = 0; loop < PF_MAX_LIGHTS; loop++)
	lightsList[loop] = NULL;
    performerGeostate->setFuncs(geostateCallback, NULL, lightsList);
//    performerGeostate->setAttr(PFSTATE_LIGHTS, lightsList);
    
    performerGeoset->setDrawMode(PFGS_FLATSHADE, PF_OFF);
    
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
    int transMode, cullMode, shadeMode;
    vsTransparencyAttribute *transAttrib;
    vsBackfaceAttribute *backAttrib;
    vsShadingAttribute *shadeAttrib;
    ushort *ilist;
    pfVec2 *vec2List;
    pfVec3 *vec3List;
    pfVec4 *vec4List;
    int loop, tResult;
    int attrBind;

    performerGeode = targetGeode;
    performerGeode->ref();
    performerGeoset = targetGeode->getGSet(0);
    performerGeoset->ref();
    
    // * Retrieve the attribute lists
    
    // Vertex Coordinates
    performerGeoset->getAttrLists(PFGS_COORD3, (void **)(&vertexList), &ilist);
    if (ilist)
    {
        // Convert indexed to non-indexed
        vertexListSize = (pfMemory::getSize(ilist) / sizeof(ushort));
        vec3List = (pfVec3 *)
            (pfMemory::malloc(sizeof(pfVec3) * vertexListSize));
        for (loop = 0; loop < vertexListSize; loop++)
            (vec3List[loop]).copy(vertexList[ilist[loop]]);
        
        attrBind = performerGeoset->getAttrBind(PFGS_COORD3);
        performerGeoset->setAttr(PFGS_COORD3, attrBind, (void *)vec3List, NULL);
        if (!(pfMemory::getRef(vertexList)))
            pfMemory::free(vertexList);
        if (!(pfMemory::getRef(ilist)))
            pfMemory::free(ilist);
        vertexList = vec3List;
    }
    else
        vertexListSize = (pfMemory::getSize(vertexList) / sizeof(pfVec3));

    // Colors
    performerGeoset->getAttrLists(PFGS_COLOR4, (void **)(&colorList), &ilist);
    if (ilist)
    {
        // Convert indexed to non-indexed
        colorListSize = (pfMemory::getSize(ilist) / sizeof(ushort));
        vec4List = (pfVec4 *)
            (pfMemory::malloc(sizeof(pfVec4) * colorListSize));
        for (loop = 0; loop < colorListSize; loop++)
            (vec4List[loop]).copy(colorList[ilist[loop]]);
        
        attrBind = performerGeoset->getAttrBind(PFGS_COLOR4);
        performerGeoset->setAttr(PFGS_COLOR4, attrBind, (void *)vec4List, NULL);
        if (!(pfMemory::getRef(colorList)))
            pfMemory::free(colorList);
        if (!(pfMemory::getRef(ilist)))
            pfMemory::free(ilist);
        colorList = vec4List;
    }
    else
        colorListSize = (pfMemory::getSize(colorList) / sizeof(pfVec4));

    // Normals
    performerGeoset->getAttrLists(PFGS_NORMAL3, (void **)(&normalList), &ilist);
    if (ilist)
    {
        // Convert indexed to non-indexed
        normalListSize = (pfMemory::getSize(ilist) / sizeof(ushort));
        vec3List = (pfVec3 *)
            (pfMemory::malloc(sizeof(pfVec3) * normalListSize));
        for (loop = 0; loop < normalListSize; loop++)
            (vec3List[loop]).copy(normalList[ilist[loop]]);
        
        attrBind = performerGeoset->getAttrBind(PFGS_NORMAL3);
        performerGeoset->setAttr(PFGS_NORMAL3, attrBind, (void *)vec3List,
            NULL);
        if (!(pfMemory::getRef(normalList)))
            pfMemory::free(normalList);
        if (!(pfMemory::getRef(ilist)))
            pfMemory::free(ilist);
        normalList = vec3List;
    }
    else
        normalListSize = (pfMemory::getSize(normalList) / sizeof(pfVec3));

    // Texture Coordinates
    performerGeoset->getAttrLists(PFGS_TEXCOORD2, (void **)(&texCoordList),
        &ilist); 
    if (ilist)
    {
        // Convert indexed to non-indexed
        texCoordListSize = (pfMemory::getSize(ilist) / sizeof(ushort));
        vec2List = (pfVec2 *)
            (pfMemory::malloc(sizeof(pfVec2) * texCoordListSize));
        for (loop = 0; loop < texCoordListSize; loop++)
            (vec2List[loop]).copy(texCoordList[ilist[loop]]);
        
        attrBind = performerGeoset->getAttrBind(PFGS_TEXCOORD2);
        performerGeoset->setAttr(PFGS_TEXCOORD2, attrBind, (void *)vec2List,
            NULL);
        if (!(pfMemory::getRef(texCoordList)))
            pfMemory::free(texCoordList);
        if (!(pfMemory::getRef(ilist)))
            pfMemory::free(ilist);
        texCoordList = vec2List;
    }
    else
        texCoordListSize = (pfMemory::getSize(texCoordList) / sizeof(pfVec2));

    lengthsList = performerGeoset->getPrimLengths();
    
    inflateFlatGeometry();

    ((vsSystem::systemObject)->getNodeMap())->registerLink(this,
        performerGeode);

    geoset = performerGeode->getGSet(0);
    geostate = geoset->getGState();

    // * Interpret the GeoState elements here
    
    // Fog
    fog = (pfFog *)(geostate->getAttr(PFSTATE_FOG));
    if (fog)
    {
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
        materialAttrib = (vsMaterialAttribute *)((vsSystem::systemObject)->
            getNodeMap()->mapSecondToFirst(frontMaterial));
        
        if (!materialAttrib)
        {
            if (backMaterial)
                newBack = backMaterial;
            else
            {
                newBack = new pfMaterial();
                newBack->copy(frontMaterial);
            }

            materialAttrib = new vsMaterialAttribute(frontMaterial, newBack);
            
            (vsSystem::systemObject)->getNodeMap()->registerLink(materialAttrib,
                frontMaterial);
        }

        addAttribute(materialAttrib);
    }
    else
        materialAttrib = NULL;
    
    // Texture
    texture = (pfTexture *)(geostate->getAttr(PFSTATE_TEXTURE));
    texEnv = (pfTexEnv *)(geostate->getAttr(PFSTATE_TEXENV));
    if (texture)
    {
        texAttrib = (vsTextureAttribute *)((vsSystem::systemObject)->
            getNodeMap()->mapSecondToFirst(texture));
        
        if (!texAttrib)
        {
            if (texEnv)
                newTexEnv = texEnv;
            else
                newTexEnv = new pfTexEnv();

            texAttrib = new vsTextureAttribute(texture, newTexEnv);
            
            (vsSystem::systemObject)->getNodeMap()->registerLink(texAttrib,
                texture);
        }

        addAttribute(texAttrib);
    }
    else
        texAttrib = NULL;
    
    // Transparency
    if ((geostate->getInherit() & PFSTATE_TRANSPARENCY) == 0)
    {
        transMode = geostate->getMode(PFSTATE_TRANSPARENCY);
        transAttrib = new vsTransparencyAttribute();
	if (transMode == PFTR_OFF)
	    transAttrib->disable();
	else
	    transAttrib->enable();
        addAttribute(transAttrib);
    }
    else
    {
        // Determine by hand if transparency is needed
        tResult = 0;

        // Check the material alpha
        if (materialAttrib &&
            (materialAttrib->getAlpha(VS_MATERIAL_SIDE_FRONT) < 1.0))
            tResult = 1;

        // Check the vertex colors
        if (!tResult)
            for (loop = 0; loop < colorListSize; loop++)
                if (fabs(colorList[loop][3] - 1.0) > 1E-6)
                    tResult = 1;

        // Scan the texture (if it exists) for transparency
        if (!tResult && texAttrib &&
            (texAttrib->getApplyMode() != VS_TEXTURE_APPLY_DECAL))
            {
                long texLoop, pixelSize;
                int xSize, ySize, dFormat;
                unsigned char *imageData;
                
                texAttrib->getImage(&imageData, &xSize, &ySize, &dFormat);
                if (dFormat == VS_TEXTURE_DFORMAT_RGBA)
                {
                    // Search each pixel of the texture for alpha < 1.0
                    pixelSize = ((long)xSize) * ((long)ySize);
                    for (texLoop = 0; texLoop < pixelSize; texLoop++)
                        if (imageData[(texLoop * 4) + 3] < 255)
                        {
                            tResult = 1;
                            break;
                        }
                }
            }

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
        cullMode = geostate->getMode(PFSTATE_CULLFACE);
	backAttrib = new vsBackfaceAttribute();
	
        if (cullMode == PFCF_OFF)
            backAttrib->enable();
        else
            backAttrib->disable();

        addAttribute(backAttrib);
    }

    // Shading
    if (((geostate->getInherit() & PFSTATE_SHADEMODEL) == 0) &&
        !(getTypedAttribute(VS_ATTRIBUTE_TYPE_SHADING, 0)))
    {
        shadeMode = geostate->getMode(PFSTATE_SHADEMODEL);
        shadeAttrib = new vsShadingAttribute();
	if (shadeMode == PFSM_FLAT)
	    shadeAttrib->setShading(VS_SHADING_FLAT);
	else
	    shadeAttrib->setShading(VS_SHADING_GOURAUD);
        addAttribute(shadeAttrib);
    }
    performerGeoset->setDrawMode(PFGS_FLATSHADE, PF_OFF);

    performerGeostate = new pfGeoState();
    performerGeostate->ref();
    performerGeoset->setGState(performerGeostate);
    
    lightsList = (pfLight **)
	(pfMemory::malloc(sizeof(pfLight *) * PF_MAX_LIGHTS));
    for (loop = 0; loop < PF_MAX_LIGHTS; loop++)
	lightsList[loop] = NULL;
    performerGeostate->setFuncs(geostateCallback, NULL, lightsList);

    pfDelete(geostate);
}

// ------------------------------------------------------------------------
// Destructor - Disconnects this node from its Performer counterpart
// ------------------------------------------------------------------------
vsGeometry::~vsGeometry()
{
    performerGeode->unref();
    pfDelete(performerGeode);
    performerGeoset->unref();
    pfDelete(performerGeoset);
    performerGeostate->unref();
    pfDelete(performerGeostate);
    
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
// Check this node to see if its name matches the given name, and returns
// a pointer to this node if so.
// ------------------------------------------------------------------------
vsNode *vsGeometry::findNodeByName(const char *targetName)
{
    if (!strcmp(targetName, getName()))
        return this;
    else
        return NULL;
}

// ------------------------------------------------------------------------
// Sets the type of geometric primitive that this object contains
// ------------------------------------------------------------------------
void vsGeometry::setPrimitiveType(int newType)
{
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
    
    return -1;
}

// ------------------------------------------------------------------------
// Sets the number of geometric primitive that this object contains. Must
// be called before any calls to set the data for any specific primitive.
// ------------------------------------------------------------------------
void vsGeometry::setPrimitiveCount(int newCount)
{
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

    lengthsList[index] = length;
}

// ------------------------------------------------------------------------
// Retrieves the number of verticies specified for the primitive with the
// indicated index in the object. The index of the first primitive is 0.
// ------------------------------------------------------------------------
int vsGeometry::getPrimitiveLength(int index)
{
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
    
    for (loop = 0; loop < getPrimitiveCount(); loop++)
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
void vsGeometry::setBinding(int whichData, int binding)
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
            printf("vsGeometry::setBinding: Unrecognized binding value\n");
            return;
    }

    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            if (binding != VS_GEOMETRY_BIND_PER_VERTEX)
            {
                printf("vsGeometry::setBinding: Vertex coordinate binding must "
                    "always be VS_GEOMETRY_BIND_PER_VERTEX\n");
                return;
            }
            performerGeoset->setAttr(PFGS_COORD3, performerBinding,
                vertexList, NULL);
            break;
        case VS_GEOMETRY_NORMALS:
            performerGeoset->setAttr(PFGS_NORMAL3, performerBinding,
                normalList, NULL);
            break;
        case VS_GEOMETRY_COLORS:
            performerGeoset->setAttr(PFGS_COLOR4, performerBinding,
                colorList, NULL);
            break;
        case VS_GEOMETRY_TEXTURE_COORDS:
            if ((binding != VS_GEOMETRY_BIND_PER_VERTEX) &&
                (binding != VS_GEOMETRY_BIND_NONE))
            {
                printf("vsGeometry::setBinding: Texture coordinates binding "
                    "must be either VS_GEOMETRY_BIND_PER_VERTEX or "
                    "VS_GEOMETRY_BIND_NONE\n");
                return;
            }
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
void vsGeometry::setData(int whichData, int dataIndex, vsVector data)
{
    int loop;

    if (dataIndex < 0)
    {
        printf("vsGeometry::setData: Index out of bounds\n");
        return;
    }
    
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            if ((vertexListSize != -1) && (dataIndex >= vertexListSize))
            {
                printf("vsGeometry::setData: Index out of bounds\n");
                return;
            }
            for (loop = 0; loop < 3; loop++)
                (vertexList[dataIndex])[loop] = data[loop];
            break;
        case VS_GEOMETRY_NORMALS:
            if ((normalListSize != -1) && (dataIndex >= normalListSize))
            {
                printf("vsGeometry::setData: Index out of bounds\n");
                return;
            }
            for (loop = 0; loop < 3; loop++)
                (normalList[dataIndex])[loop] = data[loop];
            break;
        case VS_GEOMETRY_COLORS:
            if ((colorListSize != -1) && (dataIndex >= colorListSize))
            {
                printf("vsGeometry::setData: Index out of bounds\n");
                return;
            }
            for (loop = 0; loop < 4; loop++)
                (colorList[dataIndex])[loop] = data[loop];
            break;
        case VS_GEOMETRY_TEXTURE_COORDS:
            if ((texCoordListSize != -1) && (dataIndex >= texCoordListSize))
            {
                printf("vsGeometry::setData: Index out of bounds\n");
                return;
            }
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

    if (dataIndex < 0)
    {
        printf("vsGeometry::getData: Index out of bounds\n");
        return result;
    }
    
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            if ((vertexListSize != -1) && (dataIndex >= vertexListSize))
            {
                printf("vsGeometry::getData: Index out of bounds\n");
                return result;
            }
            result.setSize(3);
            for (loop = 0; loop < 3; loop++)
                result[loop] = (vertexList[dataIndex])[loop];
            break;
        case VS_GEOMETRY_NORMALS:
            if ((normalListSize != -1) && (dataIndex >= normalListSize))
            {
                printf("vsGeometry::getData: Index out of bounds (%d,%d)\n",
                    dataIndex, normalListSize);
                return result;
            }
            result.setSize(3);
            for (loop = 0; loop < 3; loop++)
                result[loop] = (normalList[dataIndex])[loop];
            break;
        case VS_GEOMETRY_COLORS:
            if ((colorListSize != -1) && (dataIndex >= colorListSize))
            {
                printf("vsGeometry::getData: Index out of bounds\n");
                return result;
            }
            result.setSize(4);
            for (loop = 0; loop < 4; loop++)
                result[loop] = (colorList[dataIndex])[loop];
            break;
        case VS_GEOMETRY_TEXTURE_COORDS:
            if ((texCoordListSize != -1) && (dataIndex >= texCoordListSize))
            {
                printf("vsGeometry::getData: Index out of bounds\n");
                return result;
            }
            result.setSize(2);
            for (loop = 0; loop < 2; loop++)
                result[loop] = (texCoordList[dataIndex])[loop];
            break;
        default:
            printf("vsGeometry::getData: Unrecognized data type\n");
            return result;
    }
    
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
    
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            if (vertexListSize != -1)
            {
                printf("vsGeometry::setDataList: List size not defined\n");
                return;
            }
            for (loop = 0; loop < vertexListSize; loop++)
                for (sloop = 0; sloop < 3; sloop++)
                    vertexList[loop][sloop] = dataList[loop][sloop];
            break;
        case VS_GEOMETRY_NORMALS:
            if (normalListSize != -1)
            {
                printf("vsGeometry::setDataList: List size not defined\n");
                return;
            }
            for (loop = 0; loop < normalListSize; loop++)
                for (sloop = 0; sloop < 3; sloop++)
                    normalList[loop][sloop] = dataList[loop][sloop];
            break;
        case VS_GEOMETRY_COLORS:
            if (colorListSize != -1)
            {
                printf("vsGeometry::setDataList: List size not defined\n");
                return;
            }
            for (loop = 0; loop < colorListSize; loop++)
                for (sloop = 0; sloop < 4; sloop++)
                    colorList[loop][sloop] = dataList[loop][sloop];
            break;
        case VS_GEOMETRY_TEXTURE_COORDS:
            if (texCoordListSize != -1)
            {
                printf("vsGeometry::setDataList: List size not defined\n");
                return;
            }
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
    
    switch (whichData)
    {
        case VS_GEOMETRY_VERTEX_COORDS:
            if (vertexListSize != -1)
            {
                printf("vsGeometry::getDataList: List size not defined\n");
                return;
            }
            for (loop = 0; loop < vertexListSize; loop++)
            {
                dataBuffer[loop].setSize(3);
                for (sloop = 0; sloop < 3; sloop++)
                    dataBuffer[loop][sloop] = vertexList[loop][sloop];
            }
            break;
        case VS_GEOMETRY_NORMALS:
            if (normalListSize != -1)
            {
                printf("vsGeometry::getDataList: List size not defined\n");
                return;
            }
            for (loop = 0; loop < normalListSize; loop++)
            {
                dataBuffer[loop].setSize(3);
                for (sloop = 0; sloop < 3; sloop++)
                    dataBuffer[loop][sloop] = normalList[loop][sloop];
            }
            break;
        case VS_GEOMETRY_COLORS:
            if (colorListSize != -1)
            {
                printf("vsGeometry::getDataList: List size not defined\n");
                return;
            }
            for (loop = 0; loop < colorListSize; loop++)
            {
                dataBuffer[loop].setSize(4);
                for (sloop = 0; sloop < 4; sloop++)
                    dataBuffer[loop][sloop] = colorList[loop][sloop];
            }
            break;
        case VS_GEOMETRY_TEXTURE_COORDS:
            if (texCoordListSize != -1)
            {
                printf("vsGeometry::getDataList: List size not defined\n");
                return;
            }
            for (loop = 0; loop < texCoordListSize; loop++)
            {
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
            printf("vsGeometry::setDataListSize: Unrecognized data value\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves the size of one of the object's data lists
// ------------------------------------------------------------------------
int vsGeometry::getDataListSize(int whichData)
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
            printf("vsGeometry::getDataListSize: Unrecognized data value\n");
    }
    
    return -1;
}

// ------------------------------------------------------------------------
// Retrieves the center point and radius of a sphere that encompasses all
// of the geometry within this object.
// ------------------------------------------------------------------------
void vsGeometry::getBoundSphere(vsVector *centerPoint, double *radius)
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
// Adds the given attribute to the geometry object's list of child
// attributes. If successful, also notifies the attribute that it has been
// added to a list.
// ------------------------------------------------------------------------
void vsGeometry::addAttribute(vsAttribute *newAttribute)
{
    int attrCat, attrType;
    int loop;

    if (!(newAttribute->canAttach()))
    {
        printf("vsGeometry::addAttribute: Attribute is already in use\n");
        return;
    }
    
    attrCat = newAttribute->getAttributeCategory();
    if (attrCat != VS_ATTRIBUTE_CATEGORY_STATE)
    {
        printf("vsGeometry::addAttribute: Geometry nodes may not contain "
            "attributes of that type\n");
        return;
    }
    
    attrType = newAttribute->getAttributeType();
    for (loop = 0; loop < getAttributeCount(); loop++)
        if ((getAttribute(loop))->getAttributeType() == attrType)
        {
            printf("vsGeometry::addAttribute: Geometry node already "
                "contains that type of attribute\n");
            return;
        }

    // If we made it this far, it must be okay to add the attribute in
    vsAttributeList::addAttribute(newAttribute);
    newAttribute->attach(this);
}

// ------------------------------------------------------------------------
// Removes the given attribute from the object's list of child attributes.
// Also notifies the attribute that it has been removed from a list.
// ------------------------------------------------------------------------
void vsGeometry::removeAttribute(vsAttribute *targetAttribute)
{
    targetAttribute->detach(this);
    vsAttributeList::removeAttribute(targetAttribute);
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
    
    listSum = 0;
    for (loop = 0; loop < getPrimitiveCount(); loop++)
        listSum += lengthsList[loop];
    
    // Inflate colors
    binding = performerGeoset->getAttrBind(PFGS_COLOR4);
    if (binding == PFGS_PER_VERTEX)
    {
        oldPosition = 0;
        newPosition = 0;
        newColors = (pfVec4 *)(pfMemory::malloc(sizeof(pfVec4) * listSum));
        for (primLoop = 0; primLoop < getPrimitiveCount(); primLoop++)
        {
            for (loop = 0; loop < jumpCount; loop++)
                (newColors[newPosition++]).copy(colorList[oldPosition]);
            for (loop = 0; loop < (lengthsList[primLoop] - jumpCount); loop++)
                (newColors[newPosition++]).copy(colorList[oldPosition++]);
        }
        performerGeoset->setAttr(PFGS_COLOR4, binding, (void *)newColors, NULL);
        pfMemory::free(colorList);
        colorList = newColors;
        colorListSize = listSum;
    }
    
    // Inflate normals
    binding = performerGeoset->getAttrBind(PFGS_NORMAL3);
    if (binding == PFGS_PER_VERTEX)
    {
        oldPosition = 0;
        newPosition = 0;
        newNormals = (pfVec3 *)(pfMemory::malloc(sizeof(pfVec3) * listSum));
        for (primLoop = 0; primLoop < getPrimitiveCount(); primLoop++)
        {
            for (loop = 0; loop < jumpCount; loop++)
                (newNormals[newPosition++]).copy(normalList[oldPosition]);
            for (loop = 0; loop < (lengthsList[primLoop] - jumpCount); loop++)
                (newNormals[newPosition++]).copy(normalList[oldPosition++]);
        }
        performerGeoset->setAttr(PFGS_NORMAL3, binding, (void *)newNormals,
            NULL);
        pfMemory::free(normalList);
        normalList = newNormals;
        normalListSize = listSum;
    }
    
    // Add a shading attribute to compensate for the loss of the
    // 'FLAT' primitive type
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
// Calls the apply function on all attached attributes, and then calls the
// system's graphics state object to affect the changes to the graphics
// library state.
// ------------------------------------------------------------------------
void vsGeometry::applyAttributes()
{
    vsNode::applyAttributes();
    
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
    
    lightList = (pfLight **)userData;
    
    for (loop = 0; loop < PF_MAX_LIGHTS; loop++)
	if (lightList[loop] != NULL)
	    (lightList[loop])->on();

    return 0;
}
