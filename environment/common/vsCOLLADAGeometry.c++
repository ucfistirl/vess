
#include "vsCOLLADAGeometry.h++"
#include "vsSkeletonMeshGeometry.h++"
#include "vsCOLLADAInputEntry.h++"

// ------------------------------------------------------------------------
// Creates a vsCOLLADAGeometry object
// ------------------------------------------------------------------------
vsCOLLADAGeometry::vsCOLLADAGeometry(atString id, atXMLDocument *doc,
                                     atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr child;
    char *attr;
    char tempStr[32];
    atString name;
    atString *geomID;

    // Set the geometry's ID
    geometryID.setString(id);

    // Create the list that will hold our submeshes
    submeshList = new atList();

    // Create the map that will hold our data sources
    dataSources = new atMap();

    // Get the geometry name (use the ID if there isn't one)
    attr = doc->getNodeAttribute(current, "name");
    if (attr)
        name.setString(attr);
    else
        name.setString(id);

    // Look for a recognized kind of geometry under the geometry node
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // Process this node if it's a recognized kind of geometry
        if (strcmp(doc->getNodeName(child), "mesh") == 0)
            processMesh(doc, child, id, name);

        // Move on to the next node
        child = doc->getNextSiblingNode(child);
    }
}

// ------------------------------------------------------------------------
// Destroys this COLLADA geometry object
// ------------------------------------------------------------------------
vsCOLLADAGeometry::~vsCOLLADAGeometry()
{
    // Destroy the submesh list
    delete submeshList;

    // Destroy the data sources
    delete dataSources;
}

// ------------------------------------------------------------------------
// Returns the data source specified by the given identifier
// ------------------------------------------------------------------------
vsCOLLADADataSource *vsCOLLADAGeometry::getDataSource(atString id)
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
int vsCOLLADAGeometry::getGeometryDataList(atString semantic, int set)
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
// Processes a <source> XML subtree and creates a COLLADA data source for
// it
// ------------------------------------------------------------------------
void vsCOLLADAGeometry::processSource(atXMLDocument *doc,
                                      atXMLDocumentNodePtr current)
{
    vsCOLLADADataSource *source;
    atString *sourceID;

    // Create the source
    source = new vsCOLLADADataSource(doc, current);

    // If the source has useful data, add it to our data source map
    if (source->getDataCount() > 0)
    {
        sourceID = new atString(source->getID());
        dataSources->addEntry(sourceID, source);
    }
else
printf("Source %s is invalid (%d data items)\n", source->getID().getString(),
    source->getDataCount());
}

// ------------------------------------------------------------------------
// Processes a <mesh> XML subtree, which is a type of COLLADA geometry
// ------------------------------------------------------------------------
void vsCOLLADAGeometry::processMesh(atXMLDocument *doc,  
                                  atXMLDocumentNodePtr current,
                                  atString id, atString name)
{   
    vsCOLLADAGeometry *geom; 
    atXMLDocumentNodePtr child;
    atList *meshVertexInputs;
    atXMLDocumentNodePtr input;
    char *attr;
    vsCOLLADASubmesh *submesh;
    atString *geomID; 

    // Process the sources first.  Iterate over the children of the mesh
    // and process all those that are "source" nodes
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See if this is a source node
        if (strcmp(doc->getNodeName(child), "source") == 0)
        {
            // Process the source node and create the data source
            processSource(doc, child);
        }

        // Try the next node
        child = doc->getNextSiblingNode(child);
    }

    // Next, we need to process the one and only "vertices" node.  This
    // node contains information about the mesh-vertices (the vertex
    // properties of the mesh that don't depend on tesselation order or
    // polygon winding).  For example, a cube has only 8 mesh-vertices, but
    // may have 24 primitive vertices (four vertices for each of the six
    // faces), meaning that each mesh-vertex is shared among three primitives.
    // The normal for each of the three primitive vertices is different, even
    // though the position is the same.
    //
    // Most of the time, the mesh-vertices will only have the position
    // of the vertex as an attribute, but other attributes can be placed here
    // as well. We don't really care about the mesh-vertex vs. regular
    // primitive vertex distinction (the distinction is much more important
    // for content creation tools), but we do need to keep track of all the
    // attributes tied to the mesh-vertices, so we can incorporate them into
    // the primitives later

    // Create the list of inputs for the mesh-vertices
    meshVertexInputs = new atList();

    // Look for the "vertices" node under the mesh
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See if this is the vertices node
        if (strcmp(doc->getNodeName(child), "vertices") == 0)
        {
            // Look for "input" nodes, which specify vertex attribute
            // data coming from the data sources we processed above
            input = doc->getNextChildNode(child);
            while (input != NULL)
            {
                // Keep track of all the mesh-vertex inputs
                if (strcmp(doc->getNodeName(input), "input") == 0)
                {
                    // Process the input and store it in the meshVertexInputs
                    // list
                    processInput(doc, input, meshVertexInputs);
                }

                // Try the next node
                input = doc->getNextSiblingNode(input);
            }
        }
    
        // Try the next node
        child = doc->getNextSiblingNode(child); 
    }
    
    // Finally, we process the actual primitives
    child = doc->getNextChildNode(current);
    while (child != NULL) 
    {
        // See if this is a primitive type we recognize
        if ((strcmp(doc->getNodeName(child), "lines") == 0) ||
            (strcmp(doc->getNodeName(child), "linestrips") == 0) ||
            (strcmp(doc->getNodeName(child), "polygons") == 0) ||
            (strcmp(doc->getNodeName(child), "polylist") == 0) ||
            (strcmp(doc->getNodeName(child), "triangles") == 0) ||
            (strcmp(doc->getNodeName(child), "tristrips") == 0) ||
            (strcmp(doc->getNodeName(child), "trifans") == 0))
        {
            // Process the primitive set to create a submesh, passing the
            // data source objects and the mesh-vertex input entries, so the
            // submesh has access to them
            submesh = new vsCOLLADASubmesh(doc, child, dataSources,
                                           meshVertexInputs);

            // Add the submesh to the submesh list
            if (submesh != NULL)
                submeshList->addEntry(submesh);
        }

        // Try the next node
        child = doc->getNextSiblingNode(child);
    }

    // Clean up the list of mesh-vertex inputs
    delete meshVertexInputs;
}

// ------------------------------------------------------------------------
// Processes an <input> XML subtree, and creates a vsCOLLADAInputEntry
// object for it
// ------------------------------------------------------------------------
void vsCOLLADAGeometry::processInput(atXMLDocument *doc,
                                   atXMLDocumentNodePtr current,
                                   atList *inputList)
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
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsCOLLADAGeometry::getClassName()
{
    return "vsCOLLADAGeometry";
}

// ------------------------------------------------------------------------
// Return the identifier for this geometry object
// ------------------------------------------------------------------------
atString vsCOLLADAGeometry::getID()
{
    return geometryID.clone();
}

// ------------------------------------------------------------------------
// Retrieve the number of submeshes in our list (simple atList wrapper)
// ------------------------------------------------------------------------
u_long vsCOLLADAGeometry::getNumSubmeshes()
{
    return submeshList->getNumEntries();
}

// ------------------------------------------------------------------------
// Retrieve the first submesh in our list (simple atList wrapper)
// ------------------------------------------------------------------------
vsCOLLADASubmesh *vsCOLLADAGeometry::getFirstSubmesh()
{
    return (vsCOLLADASubmesh *)submeshList->getFirstEntry();
}

// ------------------------------------------------------------------------
// Retrieve the next submesh in our list since the last call (simple
// atList wrapper)
// ------------------------------------------------------------------------
vsCOLLADASubmesh *vsCOLLADAGeometry::getNextSubmesh()
{
    return (vsCOLLADASubmesh *)submeshList->getNextEntry();
}

// ------------------------------------------------------------------------
// Instances this geometry object by returning a new vsComponent with
// all of our submeshes attached as children
// ------------------------------------------------------------------------
vsComponent *vsCOLLADAGeometry::instance()
{
    vsComponent *instanceComp;
    vsComponent *mtlComp;
    vsCOLLADASubmesh *submesh;

    // Create the top-level component
    instanceComp = new vsComponent();
    instanceComp->setName(geometryID.getString());

    // Iterate over our list and attach each submesh to the instance component
    submesh = getFirstSubmesh();
    while (submesh != NULL)
    {
        // Create another component that will hold the instanced geometry's
        // materials and textures.  Give this component a name corresponding
        // to the material's ID, so the loader knows which materials to
        // apply
        mtlComp = new vsComponent();
        mtlComp->setName(submesh->getMaterialID().getString());
        instanceComp->addChild(mtlComp);

        // Add this submesh's geometry to the material component
        mtlComp->addChild(submesh->getGeometry());

        // Move on to the next submesh
        submesh = getNextSubmesh();
    }

    // Return the instance
    return instanceComp;
}
