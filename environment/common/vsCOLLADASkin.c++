
#include "atStringTokenizer.h++"
#include "vsCOLLADASkin.h++"
#include "vsCOLLADADataSource.h++"
#include "vsSkeletonMeshGeometry.h++"
#include "vsTransformAttribute.h++"


// ------------------------------------------------------------------------
// Construct a vsCOLLADASkin from the given XML subtree and geometry
// ------------------------------------------------------------------------
vsCOLLADASkin::vsCOLLADASkin(atXMLDocument *doc, atXMLDocumentNodePtr current,
                             vsCOLLADAGeometry *geom)
             : vsCOLLADAController(geom)
{
    atXMLDocumentNodePtr child;
    vsCOLLADADataSource *source;
    atString *id;

    // Initialize the bind shape matrix
    bindShapeMatrix.setIdentity();

    // Convert the vsGeometry nodes in the COLLADA geometry object to
    // vsSkeletonMeshGeometry nodes
    convertGeometry();

    // Process the elements of the skin
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See if this is an element we recognize
        if (strcmp(doc->getNodeName(child), "bind_shape_matrix") == 0)
        {
            // Parse the matrix and store it
            bindShapeMatrix = parseMatrix(doc, child);
        }
        else if (strcmp(doc->getNodeName(child), "source") == 0)
        {
            // Parse the data source
            source = new vsCOLLADADataSource(doc, child);

            // Get the source's ID
            id = new atString(source->getID());

            // Add the source to the skin
            dataSources->addEntry(id, source);
        }
        else if (strcmp(doc->getNodeName(child), "joints") == 0)
        {
            // Process the skeleton's joints
            processJoints(doc, child);
        }
        else if (strcmp(doc->getNodeName(child), "vertex_weights") == 0)
        {
            // Process the vertex weights
            processVertexWeights(doc, child);
        }

        // Try the next node
        child = doc->getNextSiblingNode(child);
    }
}

// ------------------------------------------------------------------------
// Clean up all the data we've collected
// ------------------------------------------------------------------------
vsCOLLADASkin::~vsCOLLADASkin()
{
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsCOLLADASkin::getClassName()
{
    return "vsCOLLADASkin";
}

// ------------------------------------------------------------------------
// Converts the vsGeometry nodes in our vsCOLLADAGeometry to
// vsSkeletonMeshGeometry for skinning
// ------------------------------------------------------------------------
void vsCOLLADASkin::convertGeometry()
{
    vsCOLLADASubmesh *submesh;
    vsGeometryBase *geom;
    vsSkeletonMeshGeometry *skelMesh;
    atVector *vecList;
    vsAttribute *attrib;
    int *intList;
    u_int *uintList;
    int listSize;
    int i;
    int whichData;

    // Iterate over the submeshes in the geometry
    submesh = sourceGeometry->getFirstSubmesh();
    while (submesh != NULL)
    {
        // Get the geometry object from the list
        geom = submesh->getGeometry();

        // Don't bother doing any conversion if the geometry is already
        // a skeleton mesh
        skelMesh = dynamic_cast<vsSkeletonMeshGeometry *>(geom);
        if (skelMesh == NULL)
        {
            // Create the skeleton mesh
            skelMesh = new vsSkeletonMeshGeometry();

            // Set the primitive count and type
            skelMesh->setPrimitiveCount(geom->getPrimitiveCount());
            skelMesh->setPrimitiveType(geom->getPrimitiveType());

            // Copy the primitive lengths
            intList = (int *)malloc(sizeof(int) * geom->getPrimitiveCount());
            geom->getPrimitiveLengths(intList);
            skelMesh->setPrimitiveLengths(intList);
            free(intList);

            // Copy the index array (if any)
            listSize = geom->getIndexListSize();
            if (listSize > 0)
            {
                uintList = (u_int *)malloc(sizeof(u_int) * listSize);
                geom->getIndexList(uintList);
                skelMesh->setIndexList(uintList);
                free(uintList);
            }

            // Copy each data list
            for (i = 0; i < VS_GEOMETRY_LIST_COUNT; i++)
            {
                // If we're copying vertices or normals, we need to
                // copy to the SKIN versions
                if (i == VS_GEOMETRY_VERTEX_COORDS)
                    whichData = VS_GEOMETRY_SKIN_VERTEX_COORDS;
                else if (i == VS_GEOMETRY_NORMALS)
                    whichData = VS_GEOMETRY_SKIN_NORMALS;
                else
                    whichData = i;

                // Don't copy this list if it isn't there
                listSize = geom->getDataListSize(i);
                if (listSize > 0)
                {
                    vecList = (atVector *)malloc(sizeof(atVector) * listSize);
                    geom->getDataList(i, vecList);
                    skelMesh->setDataListSize(whichData, listSize);
                    skelMesh->setDataList(whichData, vecList);
                    free(vecList);
                }

                // Set the list binding, if we're setting bindings on weights
                // or bone indices, force this to PER_VERTEX.  Otherwise,
                // just copy the original binding
                if (whichData = VS_GEOMETRY_VERTEX_WEIGHTS)
                    skelMesh->
                        setBinding(whichData, VS_GEOMETRY_BIND_PER_VERTEX);
                else if (whichData = VS_GEOMETRY_BONE_INDICES)
                    skelMesh->
                        setBinding(whichData, VS_GEOMETRY_BIND_PER_VERTEX);
                else
                    skelMesh->setBinding(whichData, geom->getBinding(i));
            }

             // Copy the lighting state
             if (geom->isLightingEnabled())
                 skelMesh->enableLighting();
             else
                 skelMesh->disableLighting();

             // Copy the attributes (if any)
             for (i = 0; i < geom->getAttributeCount(); i++)
             {
                 attrib = geom->getAttribute(i);
                 attrib->attachDuplicate(skelMesh);
             }

             // Replace the submesh's geometry with the new skeleton
             // mesh geometry
             submesh->setGeometry(skelMesh);
             delete geom;
        }

        // Move on to the next submesh
        submesh = sourceGeometry->getNextSubmesh();
    }
}

// ------------------------------------------------------------------------
// Parse an integer from the given string tokenizer
// ------------------------------------------------------------------------
int vsCOLLADASkin::getIntToken(atStringTokenizer *tokens)
{
    atString *tempStr;
    int value;

    // Get the next token from the tokenizer
    tempStr = tokens->getToken(" \n\r\t");

    // Convert the token to a integer value
    value = atoi(tempStr->getString());

    // Get rid of the temporary string
    delete tempStr;

    // Return the integer value
    return value;
}

// ------------------------------------------------------------------------
// Parses a matrix from the given XML subtree
// ------------------------------------------------------------------------
atMatrix vsCOLLADASkin::parseMatrix(atXMLDocument *doc,
                                    atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr child;
    atStringTokenizer *tokens;
    atString *token;
    atMatrix result;
    int i, j;

    // Create a string tokenizer using the text of the current node
    child = doc->getNextChildNode(current);
    tokens = new atStringTokenizer(doc->getNodeText(child));

    // We assume a 4x4 matrix
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            // Get the next token from the tokenizer
            token = tokens->getToken(" \t\n\r");

            // Make sure we got a token
            if (token != NULL)
            {
                // Convert to a float and set the matrix entry
                result[i][j] = atof(token->getString());

                // Get rid of the token string
                delete token;
            }
            else
            {
                // Set the element to zero
                result[i][j] = 0.0;
            }
        }
    }

    // Ditch the tokenizer
    delete tokens;

    // Return the matrix we parsed
    return result;
}

// ------------------------------------------------------------------------
// Returns the data source specified by the given identifier
// ------------------------------------------------------------------------
vsCOLLADADataSource *vsCOLLADASkin::getDataSource(atString id)
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
// Process the joints for this skin from the given XML subtree
// ------------------------------------------------------------------------
void vsCOLLADASkin::processJoints(atXMLDocument *doc,
                                  atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr child;
    char *attr;
    atMatrix invBindMatrix;

    // Get the identifiers for the joint and inverse bind matrix data
    // sources
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See if this is an input node
        if (strcmp(doc->getNodeName(child), "input") == 0)
        {
            // Get the input's semantic
            attr = doc->getNodeAttribute(child, "semantic");
            if (strcmp(attr, "JOINT") == 0)
            {
                // Get the data source's ID
                attr = doc->getNodeAttribute(child, "source");

                // Fetch the joint names from our data source map
                jointNames = getDataSource(atString(attr));
            }
            else if (strcmp(attr, "INV_BIND_MATRIX") == 0)
            {
                // Get the data source's ID
                attr = doc->getNodeAttribute(child, "source");

                // Fetch the matrices from our data source map
                inverseBindMatrices = getDataSource(atString(attr));
            }
        }
 
        // Try the next node
        child = doc->getNextSiblingNode(child);
    }
}

// ------------------------------------------------------------------------
// Process the vertex weights for this skin from the given XML subtree
// ------------------------------------------------------------------------
void vsCOLLADASkin::processVertexWeights(atXMLDocument *doc,
                                         atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr child;
    char *attr;
    u_long influenceCount;
    int jointOffset;
    int weightOffset;
    char *text;
    atStringTokenizer *counts;
    atStringTokenizer *values;
    atVector *weights;
    atVector *boneIndices;
    double tempWeights[64];
    double tWeight;
    double max;
    int maxIdx;
    int tempBones[64];
    int tBone;
    int i, j, k;
    int boneCount;
    int index;
    double weightSum;
    vsCOLLADASubmesh *submesh;
    vsSkeletonMeshGeometry *skelMesh;
    int vertexCount;
    int indexCount;
    
    // Get the number of weight vectors available (should match the
    // vertex count in the source geometry)
    attr = doc->getNodeAttribute(current, "count");
    if (attr != NULL)
        influenceCount = atoi(attr);
    else
        influenceCount = 0;

    // If we don't have a valid count, bail out now
    if (influenceCount <= 0)
        return;

    // Create vector arrays for the weights and bone indices
    weights = (atVector *)malloc(sizeof(atVector) * influenceCount);
    boneIndices = (atVector *)malloc(sizeof(atVector) * influenceCount);

    // Get the two input lists
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See if this is an input
        if (strcmp(doc->getNodeName(child), "input") == 0)
        {
            // Get the input's semantic
            attr = doc->getNodeAttribute(child, "semantic");
            if (strcmp(attr, "JOINT") == 0)
            {
                // Get the data source's ID
                attr = doc->getNodeAttribute(child, "source");

                // Fetch the joint names from our data source map
                jointNames = getDataSource(atString(attr));

                // Get the offset for the joint index as well
                attr = doc->getNodeAttribute(child, "offset");
                if (attr != NULL)
                    jointOffset = atoi(attr);
                else
                    jointOffset = 0;
            }
            else if (strcmp(attr, "WEIGHT") == 0)
            {
                // Get the data source's ID
                attr = doc->getNodeAttribute(child, "source");

                // Fetch the weights from our data source map
                vertexWeights = getDataSource(atString(attr));

                // Get the offset for the weight index as well
                attr = doc->getNodeAttribute(child, "offset");
                if (attr != NULL)
                    weightOffset = atoi(attr);
                else
                    weightOffset = 0;
            }
        }

        // Try the next node
        child = doc->getNextSiblingNode(child);
    }

    // Look for the "vcount" list
    counts = NULL;
    child = doc->getNextChildNode(current);
    while ((child != NULL) && (strcmp(doc->getNodeName(child), "vcount") != 0))
        child = doc->getNextSiblingNode(child);

    // If we found it, create a string tokenizer on it
    if (child != NULL)
    {
        text = doc->getNodeText(doc->getNextChildNode(child));
        counts = new atStringTokenizer(atString(text));
    }

    // Look for the "v" list
    values = NULL;
    child = doc->getNextChildNode(current);
    while ((child != NULL) && (strcmp(doc->getNodeName(child), "v") != 0))
        child = doc->getNextSiblingNode(child);

    // If we found it, create a string tokenizer on it
    if (child != NULL)
    {
        text = doc->getNodeText(doc->getNextChildNode(child));
        values = new atStringTokenizer(atString(text));
    }

    // If we couldn't find either list, there's a problem, so bail
    // out now
    if ((counts == NULL) || (values == NULL))
        return;

    // Iterate over the vcount list to read how many influences affect
    // each vertex
    for (i = 0; i < influenceCount; i++)
    {
        // Get the number of bones affecting this vertex
        boneCount = getIntToken(counts);

        // Initialize the two vectors
        weights[i].setSize(4);
        weights[i].clear();
        boneIndices[i].setSize(4);
        boneIndices[i].clear();

        // Initialize the temporary arrays that will hold the weights
        // and bones at first
        memset(tempBones, 0, sizeof(tempBones));
        memset(tempWeights, 0, sizeof(tempWeights));

        // Iterate over the joint/weight pairs and fill the associated
        // lists
        for (j = 0; j < boneCount; j++)
        {
            // Copy the index of the bone directly
            tempBones[j] = getIntToken(values);

            // Look up the weight by index in the vertex weights
            // data source
            index = getIntToken(values);
            tempWeights[j] = vertexWeights->getFloat(index);
        }

        // We're limited to four influences per vertex, so sort the
        // temporary weight and bone lists by weight and keep the
        // top four
        if (boneCount > 4)
        {
            // Adjust the bone count to a maximum of 4
            boneCount = 4;

            // Since we really only need the top four weights, we'll just
            // use a four-iteration selection sort
            for (j = 0; j < 4; j++)
            {
                // Start with the j'th element
                max = tempWeights[j];
                maxIdx = j;

                // Find the largest weight in the remainder of the list
                for (k = j+1; k < boneCount; k++)
                {
                    // If this weight is greater than the current maximum,
                    // remember where it is
                    if (tempWeights[k] > max)
                    {
                        max = tempWeights[k];
                        maxIdx = k;
                    }
                }

                // If we found a greater value than the j'th element, swap
                // the two
                if (maxIdx != j)
                {
                    // Swap the weights
                    tWeight = tempWeights[j];
                    tempWeights[j] = tempWeights[maxIdx];
                    tempWeights[maxIdx] = tWeight;

                    // Swap the bones
                    tBone = tempBones[j];
                    tempBones[j] = tempBones[maxIdx];
                    tempBones[maxIdx] = tBone;
                }
            }
        }

        // Copy the temporary weights and bone ID's to the final vectors
        for (j = 0; j < boneCount; j++)
        {
            boneIndices[i][j] = tempBones[j];
            weights[i][j] = tempWeights[j];
        }

        // Normalize the vertex weights (we need an arithmetic normalization,
        // not a geometric one, so we can't just call "normalize" on the
        // vector)
        weightSum = 0.0;
        for (j = 0; j < boneCount; j++)
           weightSum += weights[i][j];
        for (j = 0; j < boneCount; j++)
           weights[i][j] /= weightSum;
    }

    // Now that we have all the data, iterate over the geometry's submeshes
    // and set the appropriate lists
    submesh = sourceGeometry->getFirstSubmesh();
    while (submesh != NULL)
    {
        // Get the geometry object itself (we know for sure that it's a
        // skeleton mesh geometry, so we don't need to check)
        skelMesh = (vsSkeletonMeshGeometry *)(submesh->getGeometry());

        // Get the number of vertices in the geometry
        vertexCount = skelMesh->getDataListSize(VS_GEOMETRY_VERTEX_COORDS);

        // Set the size of the weight and bone index lists based on the
        // size of the geometry's vertex list
        skelMesh->setDataListSize(VS_GEOMETRY_VERTEX_WEIGHTS, vertexCount);
        skelMesh->setDataListSize(VS_GEOMETRY_BONE_INDICES, vertexCount);

        // Update the bindings to make sure they're PER_VERTEX
        skelMesh->setBinding(VS_GEOMETRY_VERTEX_WEIGHTS,
            VS_GEOMETRY_BIND_PER_VERTEX);
        skelMesh->setBinding(VS_GEOMETRY_BONE_INDICES, 
            VS_GEOMETRY_BIND_PER_VERTEX);

        // Iterate over the submesh's index list, and expand the indexed
        // weight and bone index arrays to a per-vertex list.  This lets
        // the weight and bone index arrays match the vertex array one-for-
        // one (which is necessary for efficient hardware rendering)
        indexCount = submesh->getIndexListSize();
        for (i = 0; i < indexCount; i++)
        {
           // Get the i'th index from the submesh
           index = submesh->getIndex(i);

           // Make sure the index is valid
           if ((index >= 0) && (index < influenceCount))
           {
               // Set the corresponding weight and bone index vector on the
               // geometry
               skelMesh->setData(VS_GEOMETRY_VERTEX_WEIGHTS, i,
                   weights[index]);
               skelMesh->setData(VS_GEOMETRY_BONE_INDICES, i,
                   boneIndices[index]);
           }
           else
           {
               // Use zero vectors for the weights and indices
               skelMesh->setData(VS_GEOMETRY_VERTEX_WEIGHTS, i,
                   atVector(0.0, 0.0, 0.0, 0.0));
               skelMesh->setData(VS_GEOMETRY_BONE_INDICES, i,
                   atVector(0.0, 0.0, 0.0, 0.0));
           }
        }

        // Move on to the next submesh
        submesh = sourceGeometry->getNextSubmesh();
    }

    // Free the temporary lists
    free(weights);
    free(boneIndices);
}

// ------------------------------------------------------------------------
// Recursively find the skeleton mesh geometry nodes in a give subgraph and
// transforms each geometry using the given bind shape matrices
// ------------------------------------------------------------------------
void vsCOLLADASkin::applyBindShape(vsNode *node, atMatrix bindShape,
                                   atMatrix bindShapeIT)
{
    vsSkeletonMeshGeometry *submesh;
    int listSize;
    atVector *listData;
    int i;
    atVector newVec;

    // See what kind of node this is
    if (node->getNodeType() == VS_NODE_TYPE_SKELETON_MESH_GEOMETRY)
    {
        // Cast the node to a skeleton mesh geometry
        submesh = (vsSkeletonMeshGeometry *)node;

        // Get the skin vertex and list and apply the bind shape matrix to
        // each vertex.  First, fetch the skin vertex list into a temporary
        // buffer
        listSize = submesh->getDataListSize(VS_GEOMETRY_SKIN_VERTEX_COORDS);
        listData = (atVector *)malloc(sizeof(atVector) * listSize);
        submesh->getDataList(VS_GEOMETRY_SKIN_VERTEX_COORDS, listData);

        // Transform the vertices using the bind shape matrix
        for (i = 0; i < listSize; i++)
        {
            newVec = bindShape.getPointXform(listData[i]);
            listData[i] = newVec;
        }

        // Set the new skin vertex list and free the buffer
        submesh->setDataList(VS_GEOMETRY_SKIN_VERTEX_COORDS, listData);
        free(listData);

        // Now, repeat the process for the skin normals
        listSize = submesh->getDataListSize(VS_GEOMETRY_SKIN_NORMALS);
        listData = (atVector *)malloc(sizeof(atVector) * listSize);
        submesh->getDataList(VS_GEOMETRY_SKIN_NORMALS, listData);

        // Transform the normals using the inverse transpose bind shape
        // matrix
        for (i = 0; i < listSize; i++)
        {
            newVec = bindShapeIT.getVectorXform(listData[i]);
            listData[i] = newVec;
        }

        // Set the new skin normal list and free the buffer
        submesh->setDataList(VS_GEOMETRY_SKIN_NORMALS, listData);
        free(listData);
    }
    else if (node->getNodeType() == VS_NODE_TYPE_COMPONENT)
    {
        // Recurse on this component's children
        for (i = 0; i < node->getChildCount(); i++)
            applyBindShape(node->getChild(i), bindShape, bindShapeIT);
    }
}

// ------------------------------------------------------------------------
// Return the data source containing the joint names
// ------------------------------------------------------------------------
vsCOLLADADataSource *vsCOLLADASkin::getJointNames()
{
    return jointNames;
}

// ------------------------------------------------------------------------
// Return the data source containing the inverse bind matrices
// ------------------------------------------------------------------------
vsCOLLADADataSource *vsCOLLADASkin::getInverseBindMatrices()
{
    return inverseBindMatrices;
}

// ------------------------------------------------------------------------
// Instances the skin controller
// ------------------------------------------------------------------------
vsComponent *vsCOLLADASkin::instance()
{
    atMatrix ident;
    vsComponent *topComp;
    atMatrix bindInvTrans;

    // Instance the source geometry (we need a separate copy in case another
    // controller is using this same mesh)
    topComp = sourceGeometry->instance();

    // See if our bind shape matrix is identity
    ident.setIdentity();
    if (!bindShapeMatrix.isEqual(ident))
    {
        // "Bake" the bind shape matrix into the geometry's submeshes by
        // using it to transform each vertex and normal

        // First, create the inverse transpose bind shape matrix, so we can
        // properly transform the normals as well as the vertices
        bindInvTrans = bindShapeMatrix.getInverse().getTranspose();

        // Recursively find and transform the submeshes of the geometry
        applyBindShape(topComp, bindShapeMatrix, bindInvTrans);
    }

    // Return the top component
    return topComp;
}
