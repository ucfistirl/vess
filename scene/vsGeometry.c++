// File vsGeometry.c++

#include "vsGeometry.h++"

// ------------------------------------------------------------------------
// Default Constructor - Creates a Performer geode and geoset and connects
// them together, sets up empty geometry lists, and registers callbacks.
// ------------------------------------------------------------------------
vsGeometry::vsGeometry()
{
    performerGeode = new pfGeode();
    performerGeoset = new pfGeoSet();
    performerGeode->addGSet(performerGeoset);
    setPrimitiveCount(0);
    
    colorList = NULL;
    colorListSize = 0;
    normalList = NULL;
    normalListSize = 0;
    texCoordList = NULL;
    texCoordListSize = 0;
    vertexList = NULL;
    vertexListSize = 0;
    lengthsList = NULL;
    
    ((vsSystem::systemObject)->getNodeMap())->registerLink(this,
        performerGeode);
    
    performerGeode->setTravFuncs(PFTRAV_DRAW, geoDrawCallback,
        postDrawCallback);
    performerGeode->setTravData(PFTRAV_DRAW, this);
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
    pfMaterial *frontMaterial, *backMaterial, *newFront, *newBack;
    vsMaterialAttribute *materialAttrib;
    pfTexture *texture, *newTexture;
    pfTexEnv *texEnv, *newTexEnv;
    vsTextureAttribute *texAttrib;
    int transMode, cullMode;
    vsTransparencyAttribute *transAttrib;
    vsBackfaceAttribute *backAttrib;
    ushort *temp;

    performerGeode = targetGeode;
    performerGeoset = targetGeode->getGSet(0);
    
    performerGeoset->getAttrLists(PFGS_COLOR4, (void **)(&colorList), &temp);
    performerGeoset->getAttrLists(PFGS_NORMAL3, (void **)(&normalList), &temp);
    performerGeoset->getAttrLists(PFGS_TEXCOORD2, (void **)(&texCoordList),
        &temp); 
    performerGeoset->getAttrLists(PFGS_COORD3, (void **)(&vertexList), &temp);

    lengthsList = performerGeoset->getPrimLengths();

    if (colorList && pfMemory::getSize(colorList))
        colorListSize = (pfMemory::getSize(colorList) / sizeof(pfVec4));
    else
        colorListSize = -1;
    if (normalList && pfMemory::getSize(normalList))
        normalListSize = (pfMemory::getSize(normalList) / sizeof(pfVec3));
    else
        normalListSize = -1;
    if (texCoordList && pfMemory::getSize(texCoordList))
        texCoordListSize = (pfMemory::getSize(texCoordList) / sizeof(pfVec2));
    else
        texCoordListSize = -1;
    if (vertexList && pfMemory::getSize(vertexList))
        vertexListSize = (pfMemory::getSize(vertexList) / sizeof(pfVec3));
    else
        vertexListSize = -1;
    
    ((vsSystem::systemObject)->getNodeMap())->registerLink(this,
        performerGeode);
    
    performerGeode->setTravFuncs(PFTRAV_DRAW, geoDrawCallback,
        postDrawCallback);
    performerGeode->setTravData(PFTRAV_DRAW, this);

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
        vsAttributeList::addAttribute(fogAttrib);
    }
    
    // Material
    frontMaterial = (pfMaterial *)(geostate->getAttr(PFSTATE_FRONTMTL));
    backMaterial = (pfMaterial *)(geostate->getAttr(PFSTATE_BACKMTL));
    if (frontMaterial || backMaterial)
    {
        newFront = new pfMaterial();
        if (frontMaterial)
            newFront->copy(frontMaterial);

        newBack = new pfMaterial();
        if (backMaterial)
            newBack->copy(backMaterial);

        materialAttrib = new vsMaterialAttribute(newFront, newBack);
        vsAttributeList::addAttribute(materialAttrib);
    }
    
    // Texture
    texture = (pfTexture *)(geostate->getAttr(PFSTATE_TEXTURE));
    texEnv = (pfTexEnv *)(geostate->getAttr(PFSTATE_TEXENV));
    if (texture || texEnv)
    {
        newTexture = new pfTexture();
        if (texture)
            newTexture->copy(texture);

        newTexEnv = new pfTexEnv();
        if (texEnv)
            newTexEnv->copy(texEnv);

        texAttrib = new vsTextureAttribute(newTexture, newTexEnv);
        vsAttributeList::addAttribute(texAttrib);
    }
    
    // Transparency
    if ((geostate->getInherit() & PFSTATE_TRANSPARENCY) == 0)
    {
        transMode = geostate->getMode(PFSTATE_TRANSPARENCY);
        transAttrib = new vsTransparencyAttribute(transMode);
        vsAttributeList::addAttribute(transAttrib);
    }
    
    // Backface (Cull Face)
    if ((geostate->getInherit() & PFSTATE_CULLFACE) == 0)
    {
        cullMode = geostate->getMode(PFSTATE_CULLFACE);
        if (cullMode == PFCF_OFF)
            backAttrib = new vsBackfaceAttribute(VS_FALSE);
        else
            backAttrib = new vsBackfaceAttribute(VS_TRUE);
        vsAttributeList::addAttribute(backAttrib);
    }
    
    geostate->setInherit(PFSTATE_ALL);
}

// ------------------------------------------------------------------------
// Destructor - Disconnects this node from its Performer counterpart
// ------------------------------------------------------------------------
vsGeometry::~vsGeometry()
{
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
            return VS_GEOMETRY_TYPE_LINE_STRIPS;
        case PFGS_TRIS:
            return VS_GEOMETRY_TYPE_TRIS;
        case PFGS_TRISTRIPS:
            return VS_GEOMETRY_TYPE_TRI_STRIPS;
        case PFGS_TRIFANS:
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
        lengthsBuffer[loop] = lengthsList[loop];
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
                printf("vsGeometry::getData: Index out of bounds\n");
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
    int binding;
    
    binding = getBinding(whichData);

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
            performerGeoset->setAttr(PFGS_COORD3, binding, vertexList, NULL);
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
            performerGeoset->setAttr(PFGS_NORMAL3, binding, normalList, NULL);
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
            performerGeoset->setAttr(PFGS_COLOR4, binding, colorList, NULL);
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
            performerGeoset->setAttr(PFGS_TEXCOORD2, binding, texCoordList,
                NULL);
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
// Adds the given attribute to the geometry object's list of child
// attributes. If successful, also notifies the attribute that it has been
// added to a list.
// ------------------------------------------------------------------------
void vsGeometry::addAttribute(vsAttribute *newAttribute)
{
    int attributeType;
    int loop;

    if (newAttribute->isAttached())
    {
        printf("vsGeometry::addAttribute: Attribute is already in use\n");
        return;
    }
    
    attributeType = newAttribute->getAttributeType();
    switch (attributeType)
    {
        // Geometry may only contain one of each of these
        case VS_ATTRIBUTE_TYPE_FOG:
        case VS_ATTRIBUTE_TYPE_MATERIAL:
        case VS_ATTRIBUTE_TYPE_TEXTURE:
        case VS_ATTRIBUTE_TYPE_TRANSPARENCY:
        case VS_ATTRIBUTE_TYPE_BACKFACE:
            for (loop = 0; loop < getAttributeCount(); loop++)
                if ((getAttribute(loop))->getAttributeType() == attributeType)
                {
                    printf("vsGeometry::addAttribute: Geometry nodes may "
                        "only contain one attribute of the type of the "
                        "given attribute\n");
                    return;
                }
            break;
        // Geometry may not contain any of these
        case VS_ATTRIBUTE_TYPE_LIGHT:
        case VS_ATTRIBUTE_TYPE_TRANSFORM:
        case VS_ATTRIBUTE_TYPE_SWITCH:
        case VS_ATTRIBUTE_TYPE_SEQUENCE:
        case VS_ATTRIBUTE_TYPE_LOD:
        case VS_ATTRIBUTE_TYPE_BILLBOARD:
            printf("vsGeometry::addAttribute: Geometry nodes may not contain "
                "attributes of the given type\n");
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
// static VESS internal function - Passed to Performer as a callback
// During Performer's DRAW traversal, aids in the sorting of geometry into
// transparent and non-transparent bins.
// ------------------------------------------------------------------------
int vsGeometry::geoDrawCallback(pfTraverser *_trav, void *_userData)
{
    int result;

    result = vsNode::preDrawCallback(_trav, _userData);
    
    if (pfGetTransparency())
    {
        ((vsGeometry *)_userData)->performerGeoset->setDrawBin(
            PFSORT_TRANSP_BIN);
    }
    else
    {
        ((vsGeometry *)_userData)->performerGeoset->setDrawBin(
            PFSORT_OPAQUE_BIN);
    }

    return result;
}
