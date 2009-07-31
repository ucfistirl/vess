
#include "vsCOLLADASubmesh.h++"
#include "vsSkeletonMeshGeometry.h++"

// ------------------------------------------------------------------------
// Creates a vsCOLLADASubmesh using the given XML subtree
// ------------------------------------------------------------------------
vsCOLLADASubmesh::vsCOLLADASubmesh(atXMLDocument *doc,
                                   atXMLDocumentNodePtr current,
                                   atMap *sources, atList *meshVertexInputs)
{
    int primitiveType;
    bool polylist;
    int primitiveCount;
    char *attr;
    vsCOLLADAInputEntry *inputEntry;
    atXMLDocumentNodePtr child;
    atString semantic;
    int maxOffset;
    int primsPerPList;

    // Create a new geometry for this set of triangles
    geometry = new vsGeometry();
    geometry->ref();
   
    // Keep track of our parent geometry's data sources
    dataSources = sources;
   
    // Create a list to store the input entry information
    inputList = new atList();

    // Initialize the submesh's index list to NULL, we keep track of the
    // indices used to generate the final vertex list in case we need to
    // add additional per-vertex information to the submesh later (such
    // as vertex weights and bone ID's in the case of a skin controller)
    indexList = NULL;
    indexListSize = 0;

    // Figure out the primitive type
    if (strcmp(doc->getNodeName(current), "lines") == 0)
        primitiveType = VS_GEOMETRY_TYPE_LINES;
    else if (strcmp(doc->getNodeName(current), "linestrips") == 0)
        primitiveType = VS_GEOMETRY_TYPE_LINE_STRIPS;
    else if (strcmp(doc->getNodeName(current), "polygons") == 0)
        primitiveType = VS_GEOMETRY_TYPE_POLYS;
    else if (strcmp(doc->getNodeName(current), "polylist") == 0)
        primitiveType = VS_GEOMETRY_TYPE_POLYS;
    else if (strcmp(doc->getNodeName(current), "triangles") == 0)
        primitiveType = VS_GEOMETRY_TYPE_TRIS;
    else if (strcmp(doc->getNodeName(current), "tristrips") == 0)
        primitiveType = VS_GEOMETRY_TYPE_TRI_STRIPS;
    else if (strcmp(doc->getNodeName(current), "trifans") == 0)
        primitiveType = VS_GEOMETRY_TYPE_TRI_FANS;

    // If this is a polylist, we need to remember that
    if (strcmp(doc->getNodeName(current), "polylist") == 0)
        polylist = true;
    else
        polylist = false;

    // Set the primitive type
    geometry->setPrimitiveType(primitiveType);

    // Get the material ID for this submesh
    attr = doc->getNodeAttribute(current, "material");
    if (attr)
        materialID = atString(attr);
    else
        materialID = atString("no_material");

    // Get the primitive count from the node's attributes
    attr = doc->getNodeAttribute(current, "count");
    if (attr)
        primitiveCount = atoi(attr);
    else
        primitiveCount = 0;

    // Set the primitive count on the geometry object
    geometry->setPrimitiveCount(primitiveCount);

    // Now, process the mesh inputs using the information
    // we've gathered so far
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See if this is an input node
        if (strcmp(doc->getNodeName(child), "input") == 0)
        {
            // See if this input has the "VERTEX" semantic
            attr = doc->getNodeAttribute(child, "semantic");
            semantic.setString(attr);
            if (strcmp(semantic.getString(), "VERTEX") == 0)
            {
                // Copy the vsCOLLADAInputEntrys from the meshVertexInputs
                // list to our input list (the meshVertexInputs list contains
                // the input entries that are shared across all submeshes)
                inputEntry = 
                    (vsCOLLADAInputEntry *)meshVertexInputs->getFirstEntry();
                while (inputEntry != NULL)
                {
                    // Clone the entry from the meshVertexInputs
                    // list and add it to our input list
                    inputList->addEntry(inputEntry->clone());

                    // Get the next input entry
                    inputEntry = (vsCOLLADAInputEntry *)
                        meshVertexInputs->getNextEntry();
                }
            }
            else
            {
                // Process the input normally and store it in the input
                // list we created above
                processInput(doc, child);
            }
        }
    
        // Try the next node
        child = doc->getNextSiblingNode(child);
    }
    
    // Traverse the final input list once to figure out the stride to use
    // on the primitive index list(s)
    maxOffset = 0;
    inputEntry = (vsCOLLADAInputEntry *)inputList->getFirstEntry();
    while (inputEntry != NULL)
    {
        // Check the offset of this entry and update the maximum offset
        // if necessary
        if (inputEntry->getOffset() > maxOffset)
            maxOffset = inputEntry->getOffset();

        // Move on to the next entry
        inputEntry = (vsCOLLADAInputEntry *)inputList->getNextEntry();
    }

    // Set the input stride to one greater than the maximum input offset
    inputStride = maxOffset + 1;

    // Compute the lengths of each primitive in the primitive set.  The
    // technique for this depends on the primitive type.  Lines, triangles,
    // and polylists are handled explicitly, while line strips, triangle
    // strips, triangle fans, and polygons are handled implicitly
    if ((primitiveType == VS_GEOMETRY_TYPE_LINES) ||
        (primitiveType == VS_GEOMETRY_TYPE_TRIS) ||
        ((primitiveType == VS_GEOMETRY_TYPE_POLYS) && (polylist == true)))
    {
        // Compute the lengths of the primitives using explicitly known
        // information
        computeLengthsExplicit(doc, current);

        // There will be only one <p> list specifying indices, and all of
        // the submesh's primitives will come from that list
        primsPerPList = primitiveCount;
    }
    else
    {
        // Compute the lengths of the primitives by counting the number
        // of indices in the <p> list(s)
        computeLengthsImplicit(doc, current);

        // Each <p> list in the subdocument will specify only one primitive
        primsPerPList = 1;
    }

    // Finally, process the primitive index list(s) to generate the geometry
    // data lists
    processPrimitiveIndices(doc, current, primsPerPList);
}

// ------------------------------------------------------------------------
// Destroys this vsCOLLADASubmesh
// ------------------------------------------------------------------------
vsCOLLADASubmesh::~vsCOLLADASubmesh()
{
    // Unreference our geometry node.  This will also delete it unless it's
    // been instanced into a scene somewhere
    vsObject::unrefDelete(geometry);

    // Delete the input list and data sources
    delete inputList;
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsCOLLADASubmesh::getClassName()
{
    return "vsCOLLADASubmesh";
}

// ------------------------------------------------------------------------
// Returns the data source specified by the given identifier
// ------------------------------------------------------------------------
vsCOLLADADataSource *vsCOLLADASubmesh::getDataSource(atString id)
{  
    char *idStr;
    atString newID;

    // Check the ID string to see what kind of URI this is
    idStr = id.getString();
    if (idStr[0] == '#')
    {
        // This is a URI fragment, meaning the source is local to this
        // file.  We should already have the source in our data source map,
        // so we should only need to strip the leading '#' and look up the
        // ID
        newID.setString(&idStr[1]);

        // Look in the dataSources map for the source and return it
        return (vsCOLLADADataSource *)dataSources->getValue(&newID);
    }
    else
    {
        // Other URI forms aren't currently supported
        return NULL;
    }
}

// ------------------------------------------------------------------------
// Converts the semantic name (and set number) for a given data source into
// the corresponding vsGeometry data list index
// ------------------------------------------------------------------------
int vsCOLLADASubmesh::getGeometryDataList(atString semantic, int set)
{
    // Translate the semantic name into a vsGeometry data list index.
    // For the multiple-set semantics (like
    if (strcmp(semantic.getString(), "POSITION") == 0)
    {
        return VS_GEOMETRY_VERTEX_COORDS;
    }
    else if (strcmp(semantic.getString(), "NORMAL") == 0)
    {
        return VS_GEOMETRY_NORMALS;
    }
    else if (strcmp(semantic.getString(), "COLOR") == 0)
    {
        // If the zero-based set number is 1, we'll assume they mean
        // secondary colors
        if (set == 1)
            return VS_GEOMETRY_ALT_COLORS;

        // Otherwise, just return the regular color index
        return VS_GEOMETRY_COLORS;
    }
    else if (strcmp(semantic.getString(), "WEIGHT") == 0)
    {
        return VS_GEOMETRY_VERTEX_WEIGHTS;
    }
    else if (strcmp(semantic.getString(), "JOINT") == 0)
    {
        return VS_GEOMETRY_BONE_INDICES;
    }
    else if (strcmp(semantic.getString(), "TEXCOORD") == 0)
    {
        // Validate the set number (return the first texture
        // coordinate index if its invalid)
        if ((set < 0) || (set >= VS_MAXIMUM_TEXTURE_UNITS))
            return VS_GEOMETRY_TEXTURE0_COORDS;
        else
            return VS_GEOMETRY_TEXTURE0_COORDS + set;
    }
    else if (strcmp(semantic.getString(), "UV") == 0)
    {
        // Validate the set number (zero is invalid because vertex
        // coordinates must always be specified)
        if ((set <= 0) || (set >= 16))
        {
            // Default to generic 6 because it doesn't have a
            // corresponding standard vertex attribute
            return VS_GEOMETRY_GENERIC_6;
        }
        else
        {
            return VS_GEOMETRY_GENERIC_0 + set;
        }
    }
    else if (strcmp(semantic.getString(), "TEXTANGENT") == 0)
    {
        // JPD:  Don't really have a good place for tangents,
        // Put them on GENERIC 12 for now (we'll assume only 1 set)
        return VS_GEOMETRY_GENERIC_12;
    }
    else if (strcmp(semantic.getString(), "TEXBINORMAL") == 0)
    {
        // JPD:  Don't really have a good place for binormals,
        // Put them on GENERIC 13 for now (we'll assume only 1 set)
        return VS_GEOMETRY_GENERIC_13;
    }
    else
    {
        // Print an error message
        printf("vsCOLLADGeometry::getGeometryDataList:\n");
        printf("    Unknown semantic '%s'\n", semantic.getString());

        // Return -1 to indicate an error
        return -1;
    }
}   

// ------------------------------------------------------------------------
// Parse an integer token from the given string at the given index and
// return it.  Update the index to point to the next token in the string
// ------------------------------------------------------------------------
int vsCOLLADASubmesh::getIntToken(char *tokenString, int *idx)
{
    char *src;
    char *delim;
    char token[32];
    int advance;
    int value;

    // Check for invalid index
    if (*idx < 0)
       return 0;

    // Get a pointer to the token string at the correct index
    src = &tokenString[*idx];

    // Find the next delimiter in the token string (or the end of the string)
    delim = strpbrk(src, " \n\r\t");
    if (delim == NULL)
        delim = src + strlen(src);

    // Copy the token
    strncpy(token, src, delim - src);
    token[delim - src] = 0;

    // Convert the token to a integer value
    value = atoi(token);

    // Advance the index past the delimiters
    advance = strspn(delim, " \n\r\t");
    *idx += strlen(token) + advance;

    // Check for end of string (we indicate this by setting idx to -1)
    if (tokenString[*idx] == 0)
       *idx = -1;

    // Return the integer value
    return value;
}

// ------------------------------------------------------------------------
// Determines the lengths of each primitive in the given primitive set
// using explicit knowledge of the primitives
// ------------------------------------------------------------------------
void vsCOLLADASubmesh::computeLengthsExplicit(atXMLDocument *doc,
                                             atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr vcount;
    char *text;
    int idx;
    int i;

    // Figure out the primitive length(s).  This is handled explicitly in
    // this case, because the length of each primitive is either fixed
    // (lines and triangles) or specified explicitly in the file using a
    // <vcount> node (polylists)
    if (geometry->getPrimitiveType() == VS_GEOMETRY_TYPE_POLYS)
    {
        // Look for the vcount node
        vcount = doc->getNextChildNode(current);
        while (vcount != NULL)
        {
            if (strcmp(doc->getNodeName(vcount), "vcount") == 0)
            {
                // Get the list of vertex counts
                text = doc->getNodeText(doc->getNextChildNode(vcount));
                idx = 0;

                // Parse the vertex counts
                for (i = 0; i < geometry->getPrimitiveCount(); i++)
                    geometry->setPrimitiveLength(i, getIntToken(text, &idx));
            }

            // Try the next node
            vcount = doc->getNextSiblingNode(vcount);
        }
    }
}

// ------------------------------------------------------------------------
// Determines the lengths of each primitive in the given primitive set
// implicitly by reading the number of primitives present in the index
// list
// ------------------------------------------------------------------------
void vsCOLLADASubmesh::computeLengthsImplicit(atXMLDocument *doc,
                                             atXMLDocumentNodePtr current)
{
    int primitive;
    atXMLDocumentNodePtr prim;
    char *text;
    int idx;
    int vertexCount;
    int value;

    // Figure out the primitive lengths.  This is handled implicitly in this
    // case by counting the indices in each <p> (primitive) node, and
    // dividing this value by the number of data sources (the input stride)
    // Line strips, triangle strips, triangle fans, and polygons are handled
    // in this way
    primitive = 0;
    prim = doc->getNextChildNode(current);
    while (prim != NULL)
    {
        if (strcmp(doc->getNodeName(prim), "p") == 0)
        {
            // Get the list of vertex counts
            text = doc->getNodeText(doc->getNextChildNode(prim));
            idx = 0;
    
            // Initialize the vertex count
            vertexCount = 0;

            // Parse and count the vertex indices
            while (idx >= 0)
            {
                // Increment the vertex count
                vertexCount++;

                // Try to get the next token
                value = getIntToken(text, &idx);
            }
    
            // Divide the count by the input stride to get the final
            // vertex count for this primitive
            vertexCount /= inputStride;
    
            // Set the primitive length
            geometry->setPrimitiveLength(primitive, vertexCount);

            // Increment the primitive index
            primitive++;
        }
    
        // Move on to the next node
        prim = doc->getNextSiblingNode(prim);
    }
}

// ------------------------------------------------------------------------
// Processes the primitive indices that are used to create a set of
// geometric primitives
// ------------------------------------------------------------------------
void vsCOLLADASubmesh::processPrimitiveIndices(atXMLDocument *doc,
                                              atXMLDocumentNodePtr current,
                                              int primitivesPerPList)
{
    int primitiveCount;
    int vertexCount;
    vsCOLLADAInputEntry *inputEntry;
    int primitiveLength;
    int dataList;
    int primitive;
    int vertex;
    atXMLDocumentNodePtr prim;
    int i, j, k;
    char *text;
    int idx;
    int indexes[64];
    int index;
    atVector data;

    // Count the total number of vertices in this primitive set
    primitiveCount = geometry->getPrimitiveCount();
    vertexCount = 0;
    for (i = 0; i < primitiveCount; i++)
        vertexCount += geometry->getPrimitiveLength(i);

    // Set the index list size on the submesh
    setIndexListSize(vertexCount);

    // Traverse the input list and set the data list size on each item
    inputEntry = (vsCOLLADAInputEntry *)inputList->getFirstEntry();
    while (inputEntry != NULL)
    {
        // Get the data list for this input
        dataList = inputEntry->getDataList();

        // Make sure the data list is valid
        if (dataList >= 0)
        {
            // Set the size of the list on the geometry object, and make sure
            // the binding is set to PER_VERTEX
            geometry->setDataListSize(dataList, vertexCount);
            geometry->setBinding(dataList, VS_GEOMETRY_BIND_PER_VERTEX);
        }
    
        // Move on to the next entry
        inputEntry = (vsCOLLADAInputEntry *)inputList->getNextEntry();
    }

    // Keep running primitive and vertex counts while traversing the primitive
    // index lists, so we know where to place each bit of data in the geometry
    // object
    primitive = 0;
    vertex = 0;

    // Now, process the index list to generate primitives
    prim = doc->getNextChildNode(current);
    while (prim != NULL)
    {
        // If this is a "p" (primitive) node, traverse the list
        // of indices under it and generate primitives
        if (strcmp(doc->getNodeName(prim), "p") == 0)
        {
            // Get the text under this node
            text = doc->getNodeText(doc->getNextChildNode(prim));
            idx = 0;

            // Tokenize the index string and use the indices to
            // generate data for the geometry lists
            for (i = 0; i < primitivesPerPList; i++)
            {
                // Get the length of the next primitive
                primitiveLength = geometry->getPrimitiveLength(primitive);

                // Process the vertices for this primitive
                for (j = 0; j < primitiveLength; j++)
                {
                    // Read the next set of indices from the primitive
                    // list
                    for (k = 0; k < inputStride; k++)
                    {
                        // Parse the next index from the list
                        indexes[k] = getIntToken(text, &idx);
                    }

                    // Iterate over the list of inputs and translate
                    // the primitive indexes into data for the
                    // geometry data lists
                    inputEntry =
                        (vsCOLLADAInputEntry *)inputList->getFirstEntry();
                    while (inputEntry != NULL)
                    {
                        // Get the index for this input entry
                        index = indexes[inputEntry->getOffset()];

                        // Make sure this input entry is valid
                        if ((inputEntry->getSource() != NULL) &&
                            (inputEntry->getDataList() >= 0))
                        {
                            // Get the data corresponding to this index
                            // from the data source
                            data = inputEntry->getSource()->getVector(index);

                            // Get the data list for this source
                            dataList = inputEntry->getDataList();

                            // Sometimes, only RGB color is specified, so
                            // check for this and add a 1.0 alpha if needed
                            if ((dataList == VS_GEOMETRY_COLORS) &&
                                (data.getSize() == 3))
                            {
                                data.setSize(4); 
                                data[3] = 1.0;
                            }

                            // If this is a vertex coordinate, store the
                            // appropriate index in the submesh's index
                            // list
                            if (dataList == VS_GEOMETRY_VERTEX_COORDS)
                                setIndex(vertex, index);

                            // Set the data on the correct geometry data
                            // list at the correct vertex index
                            geometry->setData(dataList, vertex, data);
                        }

                        // Process the next entry
                        inputEntry =
                            (vsCOLLADAInputEntry *)inputList->getNextEntry();
                    }

                    // Increment the vertex counter
                    vertex++;
                }

                // Increment the primitive counter
                primitive++;
            }
        }

        // Move on to the next node 
        prim = doc->getNextSiblingNode(prim);
    }
}

// ------------------------------------------------------------------------
// Processes an <input> XML subtree, and creates a vsCOLLADAInputEntry
// object for it
// ------------------------------------------------------------------------
void vsCOLLADASubmesh::processInput(atXMLDocument *doc,
                                    atXMLDocumentNodePtr current)
{
    char *attr;
    atString sourceID;
    vsCOLLADADataSource *dataSource;
    atString semantic;
    int offset;
    int set;
    int dataList;
    vsCOLLADAInputEntry *inputEntry;

    // Get the source ID
    attr = doc->getNodeAttribute(current, "source");
    sourceID.setString(attr);

    // Get the data source referenced by this ID
    dataSource = getDataSource(sourceID);

    // Get the semantic
    attr = doc->getNodeAttribute(current, "semantic");
    semantic.setString(attr);

if (dataSource == NULL)
    printf("Can't find %s data source (id = %s)\n",
         semantic.getString(), sourceID.getString());

    // Get the input offset
    attr = doc->getNodeAttribute(current, "offset");
    if (attr != NULL)
        offset = atoi(attr);
    else
        offset = 0;

    // If there is a set number, get that too
    attr = doc->getNodeAttribute(current, "set");
    if (attr != NULL)
        set = atoi(attr);
    else
        set = 0;
    
    // Based on the semantic and set, figure out which vsGeometry data list
    // should get this input
    dataList = getGeometryDataList(semantic, set);
    
    // Create a vsCOLLADAInputEntry for this data
    inputEntry = new vsCOLLADAInputEntry(dataSource, dataList, offset);
    
    // Add it to the input list
    inputList->addEntry(inputEntry);
}   

// ------------------------------------------------------------------------
// Return the geometry node associated with this submesh
// ------------------------------------------------------------------------
vsGeometryBase *vsCOLLADASubmesh::getGeometry()
{
    return geometry;
}

// ------------------------------------------------------------------------
// Replace the geometry node associated with this submesh
// ------------------------------------------------------------------------
void vsCOLLADASubmesh::setGeometry(vsGeometryBase *newGeom)
{
    // Check the new geometry for validity
    if (newGeom != NULL)
    {
        // Unreference the old geometry
        if (geometry)
            vsObject::unrefDelete(geometry);

        // Store and reference the new geometry
        geometry = newGeom;
        geometry->ref();
    }
    else
        printf("vsCOLLADASubmesh::setGeometry: NULL geometry specified\n");
}

// ------------------------------------------------------------------------
// Return the material ID associated with this submesh
// ------------------------------------------------------------------------
atString vsCOLLADASubmesh::getMaterialID()
{
    return materialID;
}

// ------------------------------------------------------------------------
// Return the first data input entry that comprises this submesh
// ------------------------------------------------------------------------
vsCOLLADAInputEntry *vsCOLLADASubmesh::getFirstInputEntry()
{
    return (vsCOLLADAInputEntry*)inputList->getFirstEntry();
}

// ------------------------------------------------------------------------
// Return the next data input entry that comprises this submesh
// ------------------------------------------------------------------------
vsCOLLADAInputEntry *vsCOLLADASubmesh::getNextInputEntry()
{
    return (vsCOLLADAInputEntry*)inputList->getNextEntry();
}

// ------------------------------------------------------------------------
// Return the data input entry that corresponds with the given ID
// ------------------------------------------------------------------------
vsCOLLADAInputEntry *vsCOLLADASubmesh::getInputEntryByID(atString id)
{
    vsCOLLADAInputEntry *entry;

    // Search the input list for the entry that uses the data source with
    // the given ID
    entry = getFirstInputEntry();
    while ((entry != NULL) && (!entry->getSource()->getID().equals(&id)))
        entry = getNextInputEntry();

    // Return the input entry (or NULL if we didn't find it)
    return NULL;
}

// ------------------------------------------------------------------------
// Returns the current size of the vertex index list
// ------------------------------------------------------------------------
int vsCOLLADASubmesh::getIndexListSize()
{
    return indexListSize;
}

// ------------------------------------------------------------------------
// Set the number of indices in the vertex index list.  Although we expand
// the indexed vertex lists into flat lists when loading them, we need to
// keep track of the original index order in case any other per-vertex
// data is loaded later (such as vertex weights and bones in a skin
// controller)
// ------------------------------------------------------------------------
void vsCOLLADASubmesh::setIndexListSize(int newSize)
{
    // See if we already have an index list
    if ((newSize > 0) && (indexList == NULL))
    {
        // Create a new index list
        indexList = (int *)calloc(newSize, sizeof(int));
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
        // Modify the current index list size
        indexList = (int *)realloc(indexList, sizeof(int) * newSize);
        indexListSize = newSize;
    }
}

// ------------------------------------------------------------------------
// Returns the requested index value from the vertex index list
// ------------------------------------------------------------------------
int vsCOLLADASubmesh::getIndex(int indexIndex)
{
    // Validate the index's index in the list
    if ((indexIndex < 0) || (indexIndex >= indexListSize))
    {
        printf("vsCOLLADASubmesh::getIndex:  Invalid list index (%d)\n",
            indexIndex);
        return -1;
    }

    // Return the requested index
    return indexList[indexIndex];
}

// ------------------------------------------------------------------------
// Sets an index value on the vertex index list
// ------------------------------------------------------------------------
void vsCOLLADASubmesh::setIndex(int indexIndex, int indexValue)
{
    // Validate the index's index in the list
    if ((indexIndex < 0) || (indexIndex >= indexListSize))
    {
        printf("vsCOLLADASubmesh::setIndex:  Invalid list index (%d)\n",
            indexIndex);
        return;
    }

    // Set the new index value in the list
    indexList[indexIndex] = indexValue;
}
