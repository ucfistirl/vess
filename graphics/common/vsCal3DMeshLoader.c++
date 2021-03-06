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
//    VESS Module:  vsCal3DMeshLoader.c++
//
//    Description:  Object for loading Cal3D mesh and material files.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#include "vsCal3DMeshLoader.h++"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "vsSkeletonMeshGeometry.h++"
#include "vsTransformAttribute.h++"
#include "vsCal3DMaterial.h++"
#include "atString.h++"

// Under Visual C++, we need to alias the access function and define the
// R_OK symbol for read access checks on files
#ifdef _MSC_VER
    #include <io.h>
    #define access _access
    #define R_OK 0x04
#else
    #include <unistd.h>
#endif

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsCal3DMeshLoader::vsCal3DMeshLoader()
{
    setName("vsCal3DMeshLoader");

    materialList = new vsArray();
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsCal3DMeshLoader::~vsCal3DMeshLoader()
{
    clearMaterials();
    delete materialList;
}

// ------------------------------------------------------------------------
// Given a filename (without prepended directory), this function will find
// return a filename that exists with a prepended directory that has been
// added to the directory list. If there is no file in the listed
// directories, this function will return NULL. This is a private helper
// function.
// ------------------------------------------------------------------------
atString vsCal3DMeshLoader::findFile(atString filename)
{
   atString *tempPath;
   char *absoluteFilename;
   char tempString[500];
   
   // Loop through the list of directories.
   tempPath = (atString *) directoryList.getFirstEntry();
   while(tempPath != NULL)
   {
      // Create the tempString
      strcpy(tempString, tempPath->getString());
      strcat(tempString, "/");
      strcat(tempString, filename.getString());
      
      // See if this file can be read by this process. 
      if(access(tempString, R_OK) == 0)
      {
         // Make the absoluteFilename string.
         absoluteFilename = 
            (char*)(calloc(strlen(tempString)+2, sizeof(char)));
            
         strcpy(absoluteFilename, tempString);
         
         // Return it.
         return atString(absoluteFilename);
      }
      
      tempPath = (atString *) directoryList.getNextEntry();
   }

   // We didn't find the file, so just return the original string.
   return filename;
}

// ------------------------------------------------------------------------
// Adds a directory listing to the list that we should search for files in.
// ------------------------------------------------------------------------
void vsCal3DMeshLoader::addFilePath(const char *dirName)
{
   atString *filename;

   // Create a new string to hold our filename
   filename = new atString();
   filename->setString(dirName);

   // Add our string to the list of directories
   directoryList.addEntry(filename);
}

// ------------------------------------------------------------------------
// Parse the XML material files that define Cal3D materials.
// ------------------------------------------------------------------------
void vsCal3DMeshLoader::parseXMLMaterial(char *filename)
{
    atString               filepath;
    FILE                   *filePointer;
    char                   *fileBuffer;
    long                   fileSize;
    xmlValidCtxt           context;
    xmlDocPtr              document;
    xmlNodePtr             current;
    xmlAttrPtr             attribute;
    bool                   validVersion;
    int                    textureCount;
    int                    currentTexture;
    int                    x, y, z, w;
    char                   *tempString;
    atString               tempPath;
    vsMaterialAttribute    *tempMat;
    vsCal3DMaterial        *materialData;
    vsCal3DMaterial        *oldMaterial;

    currentTexture = 0;
    validVersion = false;

    // Prepend the directory information to the filename
    filepath = findFile(filename);

    // If the file opening failed, print error and return.
    if ((filePointer = fopen(filepath.getString(), "r")) == NULL)
    {
        notify(AT_ERROR, "parseXMLMaterial: Error opening file!\n");
        return;
    }

    // Seek to the end of the file so we can determine its size, and reset
    // to the beginning of the file.
    fseek(filePointer, 0L, SEEK_END);
    fileSize = ftell(filePointer);
    fseek(filePointer, 0L, SEEK_SET);

    // Allocate a buffer to hold the entire file content as well as our
    // begin and end tags to allow the xml library to parse the file
    // properly.
    fileBuffer = new char[fileSize +
        strlen(VS_CAL3D_XML_MATERIAL_BEGIN_TAG) +
        strlen(VS_CAL3D_XML_MATERIAL_END_TAG) + 1];

    // Fill in the buffer with the initial begin tag, then read the file
    // content and then concatenate the end tag.
    strcpy(fileBuffer, VS_CAL3D_XML_MATERIAL_BEGIN_TAG);
    fileSize = fread(&(fileBuffer[strlen(VS_CAL3D_XML_MATERIAL_BEGIN_TAG)]), 1,
                     fileSize, filePointer);
    // Need to insert a null because fread does not.
    fileBuffer[strlen(VS_CAL3D_XML_MATERIAL_BEGIN_TAG)+fileSize] = 0;
    strcat(fileBuffer, VS_CAL3D_XML_MATERIAL_END_TAG);

    // Close the file, we don't need it anymore.
    fclose(filePointer);

    // Setup the xml context.
    context.userData = stderr;
    context.error = (xmlValidityErrorFunc ) fprintf;
    context.warning = (xmlValidityWarningFunc ) fprintf;

    // Create an xml document by parsing the buffer.
    document = xmlParseMemory(fileBuffer, strlen(fileBuffer));

    // If our document is NULL, then we could not parse it properly.
    if (document == NULL)
    {
        notify(AT_ERROR,
               "parseXMLMaterial: Document not parsed successfully.\n");
        xmlFreeDoc(document);
        delete [] fileBuffer;
        return;
    }

    // Get the root element of the file.
    current = xmlDocGetRootElement(document);

    // If the root element is NULL, then the file is empty.
    if (current == NULL)
    {
        notify(AT_ERROR, "parseXMLMaterial: Empty document.\n");
        xmlFreeDoc(document);
        delete [] fileBuffer;
        return;
    }
    
    // Check the material array to see if there's already a material at
    // this slot
    oldMaterial = (vsCal3DMaterial *) materialList->getEntry(materialCount);
    if (oldMaterial)
    {
        // Print an error that a previously defined material will be replaced
        notify(AT_ERROR, "parseXMLMaterial: Error: multiple definitions for "
               "material: %d\n", materialCount);
    }

    // Add the material to the material array in the proper slot
    materialData = new vsCal3DMaterial();
    materialList->setEntry(materialCount, materialData);

    // Go to the child because we created the parent and don't
    // need to parse him "VS_CAL3D_XML_MATERIAL_BEGIN_TAG".
    current = current->children;

    // If the HEADER field is encountered, process its properties.
    if (xmlStrcmp(current->name, (const xmlChar *) "MATERIAL") == 0)
    {
        // Traverse the properties of this tag.
        for (attribute = current->properties; attribute != NULL;
             attribute = attribute->next)
        {
            // If the property is named MAGIC, check to see if it is the
            // proper value "XRF".
            if (xmlStrcmp(attribute->name, (const xmlChar *) "NUMMAPS") == 0)
            {
                // Read in the number of texture maps that this material has.
                textureCount = atoi((const char *)
                    XML_GET_CONTENT(attribute->children));

                // Cap it, can only support the defined maximum textures.
                if (textureCount > VS_MAXIMUM_TEXTURE_UNITS)
                    textureCount = VS_MAXIMUM_TEXTURE_UNITS;

                // Tell the material the texture count
                materialData->setTextureCount(textureCount);
            }
            // Else if the property is named VERSION, check to see if it is at
            // least version "1000".
            else if (xmlStrcmp(attribute->name,
                     (const xmlChar *) "VERSION") == 0)
            {
                if (atoi((const char *)XML_GET_CONTENT(attribute->children))
                    >= 1000)
                {
                    validVersion = true;
                }
            }
        }
    }

    // If either the magic and version properties were invalid or not found,
    // print error, free resources and return.
    if (!validVersion)
    {
        notify(AT_ERROR, "parseXMLMaterial: Document of wrong type.\n");
        xmlFreeDoc(document);
        delete [] fileBuffer;
        return;
    }

    // Create a material attribute to store the settings below
    tempMat = new vsMaterialAttribute();

    // Process all of the MATERIAL's children.
    current = current->children;
    while (current)
    {
        // Process a AMBIENT child.
        if (xmlStrcmp(current->name, (const xmlChar *) "AMBIENT") == 0)
        {
            tempString = (char *) xmlNodeGetContent(current->children);
            sscanf(tempString, "%d %d %d %d", &x, &y, &z, &w);

            // Normalize the values.
            tempMat->setColor(VS_MATERIAL_SIDE_BOTH, VS_MATERIAL_COLOR_AMBIENT,
                x/255.0, y/255.0, z/255.0);

            // Free the XML string.
            xmlFree(tempString);
        }
        // Process a DIFFUSE child.
        else if (xmlStrcmp(current->name, (const xmlChar *) "DIFFUSE") == 0)
        {
            tempString = (char *) xmlNodeGetContent(current->children);
            sscanf(tempString, "%d %d %d %d", &x, &y, &z, &w);

            // Normalize the values.
            tempMat->setColor(VS_MATERIAL_SIDE_BOTH, VS_MATERIAL_COLOR_DIFFUSE,
                x/255.0, y/255.0, z/255.0);

            // Free the XML string.
            xmlFree(tempString);
        }
        // Process a SPECULAR child.
        else if (xmlStrcmp(current->name, (const xmlChar *) "SPECULAR") == 0)
        {
            tempString = (char *) xmlNodeGetContent(current->children);
            sscanf(tempString, "%d %d %d %d", &x, &y, &z, &w);

            // Normalize the values.
            tempMat->setColor(VS_MATERIAL_SIDE_BOTH, VS_MATERIAL_COLOR_SPECULAR,
                x/255.0, y/255.0, z/255.0);

            // Free the XML string.
            xmlFree(tempString);
        }
        // Process a SHININESS child.
        else if (xmlStrcmp(current->name, (const xmlChar *) "SHININESS") == 0)
        {
            tempMat->setShininess(VS_MATERIAL_SIDE_BOTH, 
                atof((const char *) XML_GET_CONTENT(current->children)));
        }
        // Process a MAP child.
        else if (xmlStrcmp(current->name, (const xmlChar *) "MAP") == 0)
        {
            if (currentTexture == materialData->getTextureCount())
            {
                notify(AT_ERROR,
                       "parseXMLMaterial: Too many MAP children encountered!");
            }
            else
            {
                tempPath = findFile(
                        (char *) XML_GET_CONTENT(current->children));
                        
                // Create a vsTextureAttribute for the texture, this attribute
                // will be referenced by any objects who use this texture.
                // Saves a significant amount of memory since textures
                // are often shared between meshes.
                vsTextureAttribute *textureAttr = new vsTextureAttribute();
                textureAttr->loadImageFromFile(tempPath.getString());
                textureAttr->setBoundaryMode(VS_TEXTURE_DIRECTION_ALL,
                                             VS_TEXTURE_BOUNDARY_CLAMP);
                textureAttr->setMagFilter(VS_TEXTURE_MAGFILTER_LINEAR);
                textureAttr->setMinFilter(VS_TEXTURE_MINFILTER_MIPMAP_LINEAR);
                if (currentTexture > 0)
                    textureAttr->setApplyMode(VS_TEXTURE_APPLY_MODULATE);
                else
                    textureAttr->setApplyMode(VS_TEXTURE_APPLY_REPLACE);
                materialData->setTexture(currentTexture, textureAttr);

                // Increment the texture count.
                currentTexture++;
            }
        }

        // Move to next MATERIAL child.
        current = current->next;
    }

    // Set the VESS material that we created on the Cal3D material
    materialData->setMaterial(tempMat);

    // Finished creating material data, so increment the loaded material count.
    materialCount++;
}

// ------------------------------------------------------------------------
// Parse the XML mesh files that define Cal3D meshes.
// ------------------------------------------------------------------------
vsComponent *vsCal3DMeshLoader::parseXMLMesh(char *filename,
                                             vsComponent *rootNode)
{
    atString               filepath;
    FILE                   *filePointer;
    char                   *fileBuffer;
    long                   fileSize;
    xmlValidCtxt           context;
    xmlDocPtr              document;
    xmlNodePtr             current;
    xmlNodePtr             currentVertexChild;
    xmlNodePtr             currentSubMeshChild;
    xmlAttrPtr             attribute;
    bool                   validVersion;
    char                   geometryName[VS_NODE_NAME_MAX_LENGTH];
    int                    subMeshCount;
    int                    subMeshesProcessed;
    int                    meshTexCoordsProcessed;
    unsigned long          totalVertices;
    unsigned long          totalFaces;
    int                    meshVertices;
    int                    meshVerticesProcessed;
    int                    meshFaces;
    int                    meshTexCoords;
    int                    meshMaterial;
    int                    vertexInfluencesProcessed;
    int                    vertexID;
    int                    vertexInfluenceID;
    int                    vertexInfluences;
    double                 vertexInfluenceWeight;
    double                 weightSum;
    atVector               position;
    atVector               normal;
    atVector               textureCoords[VS_MAXIMUM_TEXTURE_UNITS];
    atVector               weights;
    atVector               boneIDs;
    vsCal3DMaterial        *materialData;
    int                    i; 
    int                    meshIndicesProcessed;
    char                   *tempString;
    int                    tx, ty, tz;
    int                    index;
    vsSkeletonMeshGeometry *resultMesh;
    vsMaterialAttribute    *materialAttribute;
    vsComponent            *resultComponent;

    totalVertices = 0;
    totalFaces = 0;
    resultComponent = rootNode;
    subMeshCount = 0;
    subMeshesProcessed = 0;
    validVersion = false;
    
    // Prepend the directory information to the filename
    filepath = findFile(filename);
    
    // If the file opening failed, print error and return.
    if ((filePointer = fopen(filepath.getString(), "r")) == NULL)
    {
        notify(AT_ERROR, "vsCal3DMeshLoader::parseXMLMesh: Error opening "
            "file!\n");
        return NULL;
    }

    // Seek to the end of the file so we can determine its size, and reset
    // to the beginning of the file.
    fseek(filePointer, 0L, SEEK_END);
    fileSize = ftell(filePointer);
    fseek(filePointer, 0L, SEEK_SET);

    // Allocate a buffer to hold the entire file content as well as our
    // begin and end tags to allow the xml library to parse the file
    // properly.
    fileBuffer = new char[fileSize +
        strlen(VS_CAL3D_XML_MESH_BEGIN_TAG) +
        strlen(VS_CAL3D_XML_MESH_END_TAG) + 1];

    // Fill in the buffer with the initial begin tag, then read the file
    // content and then concatenate the end tag.
    strcpy(fileBuffer, VS_CAL3D_XML_MESH_BEGIN_TAG);
    fileSize = fread(&(fileBuffer[strlen(VS_CAL3D_XML_MESH_BEGIN_TAG)]), 1, fileSize,
        filePointer);
    // Need to insert a null because fread does not.
    fileBuffer[strlen(VS_CAL3D_XML_MESH_BEGIN_TAG)+fileSize] = 0;
    strcat(fileBuffer, VS_CAL3D_XML_MESH_END_TAG);

    // Close the file, we don't need it anymore.
    fclose(filePointer);

    // Setup the xml context.
    context.userData = stderr;
    context.error = (xmlValidityErrorFunc ) fprintf;
    context.warning = (xmlValidityWarningFunc ) fprintf;

    // Create an xml document by parsing the buffer.
    document = xmlParseMemory(fileBuffer, strlen(fileBuffer));

    // If our document is NULL, then we could not parse it properly.
    if (document == NULL)
    {
        notify(AT_ERROR, "parseXMLMesh: Document not parsed successfully.\n");
        xmlFreeDoc(document);
        delete [] fileBuffer;
        return NULL;
    }

    // Get the root element of the file.
    current = xmlDocGetRootElement(document);

    // If the root element is NULL, then the file is empty.
    if (current == NULL)
    {
        notify(AT_ERROR, "parseXMLMesh: Empty document.\n");
        xmlFreeDoc(document);
        delete [] fileBuffer;
        return NULL;
    }

    // Go to the child because we created the parent and don't
    // need to parse him "VS_CAL3D_XML_MESH_BEGIN_TAG".
    current = current->children;

    // If the MESH field is encountered, process its properties.
    if (xmlStrcmp(current->name, (const xmlChar *) "MESH") == 0)
    {
        // Traverse the properties of this tag.
        for (attribute = current->properties; attribute != NULL;
             attribute = attribute->next)
        {
            // If we found something and it has a property, named 
            // NUMSUBMESH read it.
            if (((xmlStrcmp(attribute->name, 
                (const xmlChar *) "NUMSUBMESH")) == 0))
            {
                subMeshCount = atoi((const char *)
                  XML_GET_CONTENT(attribute->children));
            }
            // Else if the property is named VERSION, check to see if it is at
            // least version "1000".
            else if (xmlStrcmp(attribute->name,
                     (const xmlChar *) "VERSION") == 0)
            {
                if (atoi((const char *)XML_GET_CONTENT(attribute->children))
                    >= 1000)
                {
                    validVersion = true;
                }
            }
        }
    }
   
    // If we have no bones, then it is an error.
    if (subMeshCount == 0)
    {
        notify(AT_ERROR, "parseXMLMesh: No meshes found!\n");
        xmlFreeDoc(document);
        delete [] fileBuffer;
        return NULL;
    }
    
    // If either the magic and version properties were invalid or not found,
    // print error, free resources and return.
    if (!validVersion)
    {
        notify(AT_ERROR, "parseXMLMesh: Document of wrong type.\n");
        xmlFreeDoc(document);
        delete [] fileBuffer;
        return NULL;
    }
    
    // Move to the children of MESH, and process them all.  These
    // are the actual submeshes we need.
    current = current->children;
    while (current)
    {
        // Insure the child is a SUBMESH to perform further processing.
        if (xmlStrcmp(current->name, (const xmlChar *) "SUBMESH") == 0)
        {
            // Process all of the SUBMESH's properties.
            attribute = current->properties;
            while (attribute != NULL)
            {
                // Process the MATERIAL property.
                if (xmlStrcmp(attribute->name, (const xmlChar *) "MATERIAL")
                    == 0)
                {
                    meshMaterial = atoi((const char *)
                        XML_GET_CONTENT(attribute->children));
                }
                // Process the NUMVERTICES property.
                else if (xmlStrcmp(attribute->name,
                         (const xmlChar *) "NUMVERTICES") == 0)
                {
                    meshVertices = atoi((const char *)
                        XML_GET_CONTENT(attribute->children));
                }
                // Process the NUMFACES property.
                else if (xmlStrcmp(attribute->name,
                         (const xmlChar *) "NUMFACES") == 0)
                {
                    meshFaces = atoi((const char *)
                        XML_GET_CONTENT(attribute->children));
                }
                // Process the NUMTEXCOORDS property.
                else if (xmlStrcmp(attribute->name,
                         (const xmlChar *) "NUMTEXCOORDS") == 0)
                {
                    meshTexCoords = atoi((const char *)
                        XML_GET_CONTENT(attribute->children));
                }

                // Move to next node.
                attribute = attribute->next;
            }

            // Initialize to no vertices processed.
            meshVerticesProcessed = 0;

            // Create the mesh geometry object, setup to take in however
            // many faces (triangles) have been defined in the file.
            resultMesh = new vsSkeletonMeshGeometry();
            resultMesh->beginNewState();
            resultMesh->setPrimitiveType(VS_GEOMETRY_TYPE_TRIS);
            resultMesh->setPrimitiveCount(meshFaces);
            resultMesh->enableLighting();

            // Copy the filename and remove the file ending to make it the
            // geometry node name.
            strncpy(geometryName, filename, VS_NODE_NAME_MAX_LENGTH);
            geometryName[VS_NODE_NAME_MAX_LENGTH - 1] = 0;
            for (index = strlen(geometryName);
                 ((index > 0) && (geometryName[index] != '.')); index--);
            if (index > 0)
                geometryName[index] = 0;

            // Search for the last / or \.
            for (index = strlen(geometryName);
                 ((index > 0) && (geometryName[index] != '/') &&
                  (geometryName[index] != '\'')); index--);

            // Set the name string from the last / or \ onwards. (filename)
            resultMesh->setName(&geometryName[index+1]);

            // If no component is defined, make a new one.
            if (resultComponent == NULL)
                resultComponent = new vsComponent();

            // Add the new mesh to the component.
            resultComponent->addChild(resultMesh);

            // Set the list sizes and bindings.  All lists will be sized
            // to the number of vertices in the mesh (except the colors)
            resultMesh->setDataListSize(VS_GEOMETRY_SKIN_VERTEX_COORDS,
                meshVertices);
            resultMesh->setBinding(VS_GEOMETRY_SKIN_VERTEX_COORDS,
                VS_GEOMETRY_BIND_PER_VERTEX);
            resultMesh->setBinding(VS_GEOMETRY_VERTEX_COORDS,
                VS_GEOMETRY_BIND_PER_VERTEX);

            resultMesh->setDataListSize(VS_GEOMETRY_SKIN_NORMALS,
                meshVertices);
            resultMesh->setBinding(VS_GEOMETRY_SKIN_NORMALS,
                VS_GEOMETRY_BIND_PER_VERTEX);
            resultMesh->setBinding(VS_GEOMETRY_NORMALS,
                VS_GEOMETRY_BIND_PER_VERTEX);

            resultMesh->setDataListSize(VS_GEOMETRY_VERTEX_WEIGHTS,
                meshVertices);
            resultMesh->setBinding(VS_GEOMETRY_VERTEX_WEIGHTS,
                VS_GEOMETRY_BIND_PER_VERTEX);

            resultMesh->setDataListSize(VS_GEOMETRY_BONE_INDICES,
                meshVertices);
            resultMesh->setBinding(VS_GEOMETRY_BONE_INDICES,
                VS_GEOMETRY_BIND_PER_VERTEX);

            // Set color data binding and data information.
            resultMesh->setDataListSize(VS_GEOMETRY_COLORS, 1);
            resultMesh->setBinding(VS_GEOMETRY_COLORS,
                VS_GEOMETRY_BIND_OVERALL);

            // Set the mesh color to white.
            resultMesh->setData(VS_GEOMETRY_COLORS, 0,
                atVector(1.0, 1.0, 1.0, 1.0));

            // Get and apply the mesh material to this geometry, if it has one.
            materialData = (vsCal3DMaterial *)
                materialList->getEntry(meshMaterial);
            if (materialData)
            {
                for (index = 0;
                     index < materialData->getTextureCount();
                     index++)
                {
                    // Set the size and binding of current texture coordinate
                    // list.
                    resultMesh->setDataListSize(
                        VS_GEOMETRY_TEXTURE0_COORDS+index, meshVertices);
                    resultMesh->setBinding(VS_GEOMETRY_TEXTURE0_COORDS+index,
                        VS_GEOMETRY_BIND_PER_VERTEX);

                    // Hand the mesh the prepared vsTextureAttribute.
                    resultMesh->addAttribute( (vsTextureAttribute *)
                        materialData->getTexture(index));
                }

                // Add the material attribute as well
                materialAttribute = materialData->getMaterial();
                resultMesh->addAttribute(materialAttribute);
            }

            meshVerticesProcessed = 0;

            // Set the sizes of each data vector to the correct size
            position.setSize(3);
            normal.setSize(3);
            weights.setSize(4);
            boneIDs.setSize(4);
            for (i = 0; i < VS_MAXIMUM_TEXTURE_UNITS; i++)
                textureCoords[i].setSize(2);

            // Process all of the SUBMESH's VERTEX children now.
            currentSubMeshChild = current->children;
            while (currentSubMeshChild != NULL)
            {
                // Initialize to no texture coordinates and influences
                // processed.
                meshTexCoordsProcessed = 0;
                vertexInfluencesProcessed = 0;

                // Since we accumulate weight and bone ID data one influence
                // at a time, we need to clear these vectors for each new
                // vertex 
                weights.clear();
                boneIDs.clear();

                // Process the VERTEX child.
                if (xmlStrcmp(currentSubMeshChild->name,
                    (const xmlChar *) "VERTEX") == 0)
                {
                    // Process all of the VERTEX's properties.
                    attribute = currentSubMeshChild->properties;
                    while (attribute != NULL)
                    {
                        // Process the ID property.
                        if (xmlStrcmp(attribute->name,
                            (const xmlChar *) "ID") == 0)
                        {
                            vertexID = atoi((const char *)
                                XML_GET_CONTENT(attribute->children));
                        }
                        // Process the NUMINFLUENCES property.
                        else if (xmlStrcmp(attribute->name,
                            (const xmlChar *) "NUMINFLUENCES") == 0)
                        {
                            vertexInfluences = atoi((const char *)
                                XML_GET_CONTENT(attribute->children));

                            if (vertexInfluences >
                                VS_CAL3D_MESH_LOADER_MAX_INFLUENCES)
                            {
                                notify(AT_ERROR, 
                                       "parseXMLMesh: Bone influences greater "
                                       "than %d, truncating\n",
                                       VS_CAL3D_MESH_LOADER_MAX_INFLUENCES);
                            }
                        }

                        // Move to next node.
                        attribute = attribute->next;
                    }

                    // Process all of the VERTEX's children.
                    currentVertexChild = currentSubMeshChild->children;
                    while (currentVertexChild != NULL)
                    {
                        // Process the POS child.
                        if (xmlStrcmp(currentVertexChild->name,
                            (const xmlChar *) "POS") == 0)
                        {
                            tempString = (char *) xmlNodeGetContent(
                                currentVertexChild->children);
                            sscanf(tempString, "%lf %lf %lf", &(position[0]),
                                &(position[1]), &(position[2]));

                            // Free the XML string.
                            xmlFree(tempString);
                        }
                        // Process the NORM child.
                        else if (xmlStrcmp(currentVertexChild->name,
                            (const xmlChar *) "NORM") == 0)
                        {
                            tempString = (char *) xmlNodeGetContent(
                                currentVertexChild->children);
                            sscanf(tempString, "%lf %lf %lf", &(normal[0]),
                                &(normal[1]), &(normal[2]));

                            // Free the XML string.
                            xmlFree(tempString);
                        }
                        // Process the TEXCOORD child.
                        else if (xmlStrcmp(currentVertexChild->name,
                            (const xmlChar *) "TEXCOORD") == 0)
                        {
                            // If we have not reached the maximum texture count
                            // continue to process.
                            if (meshTexCoordsProcessed <
                                VS_MAXIMUM_TEXTURE_UNITS)
                            {
                                tempString = (char *) xmlNodeGetContent(
                                    currentVertexChild->children);

                                sscanf(tempString, "%lf %lf",
                                   &(textureCoords[meshTexCoordsProcessed][0]),
                                   &(textureCoords[meshTexCoordsProcessed][1]));

                                // Invert the T coordinate (Cal3D textures
                                // are flipped)
                                textureCoords[meshTexCoordsProcessed][1] =
                                    1.0 - 
                                    textureCoords[meshTexCoordsProcessed][1];

                                // Free the XML string.
                                xmlFree(tempString);

                                // Increment the processed texture coordinates.
                                meshTexCoordsProcessed++;
                            }
                            // Too many encountered, print error.
                            else
                            {
                                notify(AT_ERROR, "parseXMLMesh: Encountered "
                                       "more than %d texture coordinates, "
                                       "ignoring the rest\n",
                                       VS_MAXIMUM_TEXTURE_UNITS);
                            }
                        }
                        // Process the INFLUENCE child.
                        else if (xmlStrcmp(currentVertexChild->name,
                            (const xmlChar *) "INFLUENCE") == 0)
                        {
                            // If we have not reached the maximum influence
                            // count, process it.
                            if (vertexInfluencesProcessed <
                                VS_CAL3D_MESH_LOADER_MAX_INFLUENCES)
                            {
                                attribute = currentVertexChild->properties;
                                while (attribute != NULL)
                                {
                                    // Process the ID property.
                                    if (xmlStrcmp(attribute->name,
                                        (const xmlChar *) "ID") == 0)
                                    {
                                        vertexInfluenceID = atoi((const char *)
                                            XML_GET_CONTENT(
                                            attribute->children));
                                    }

                                    // Move to next node.
                                    attribute = attribute->next;
                                }

                                // Get the weight 
                                vertexInfluenceWeight = atof((const char *)
                                    XML_GET_CONTENT(
                                    currentVertexChild->children));

                                // Store the bone id and weight values.
                                boneIDs[vertexInfluencesProcessed] =
                                    (double) vertexInfluenceID;
                                weights[vertexInfluencesProcessed] =
                                    vertexInfluenceWeight;

                                // Increment the processed influences.
                                vertexInfluencesProcessed++;
                            }
                            // Too many encountered, print error.
                            else
                            {
                                notify(AT_ERROR, 
                                       "parseXMLMesh: Encountered more than "
                                       "%d influences, ignoring the rest\n",
                                       VS_CAL3D_MESH_LOADER_MAX_INFLUENCES);
                            }
                        }

                        // Move to next node.
                        currentVertexChild = currentVertexChild->next;
                    }

                    // If the number of texture coordinates processed and
                    // the number specified do not match, print an error.
                    if (meshTexCoords != meshTexCoordsProcessed)
                    {
                        notify(AT_ERROR,
                               "parseXMLMesh: Mismatched texcoordinate data\n"
                               "\tExpected: %d  Got: %d\n",
                               meshTexCoords, meshTexCoordsProcessed);
                    }

                    // If the number of influences processed and
                    // the number specified do not match, print an error.
                    if (vertexInfluences != vertexInfluencesProcessed)
                    {
                        notify(AT_ERROR,
                               "parseXMLMesh: Mismatched vertex influences\n"
                               "\tExpected: %d  Got: %d\n", vertexInfluences,
                               vertexInfluencesProcessed);
                    }

                    // Ensure weights are normalized [0, 1].
                    weightSum = 0.0;
                    for (index = 0; index < 4; index++)
                    {
                        weightSum += weights[index];
                    }
                    for (index = 0; index < 4; index++)
                    {
                        weights[index] /= weightSum;
                    }

                    // Apply all the attributes we just collected to the
                    // geometry
                    resultMesh->setData(VS_GEOMETRY_SKIN_VERTEX_COORDS,
                        vertexID, position);
                    resultMesh->setData(VS_GEOMETRY_SKIN_NORMALS,
                        vertexID, normal);
                    resultMesh->setData(VS_GEOMETRY_VERTEX_WEIGHTS,
                        vertexID, weights);
                    resultMesh->setData(VS_GEOMETRY_BONE_INDICES,
                        vertexID, boneIDs);
                    for (i = 0; i < meshTexCoordsProcessed; i++)
                    {
                       resultMesh->setData(VS_GEOMETRY_TEXTURE0_COORDS+i,
                          vertexID, textureCoords[i]);
                    }

                    // Increment the number of vertices processed.
                    meshVerticesProcessed++;
                }

                // Move to next node.
                currentSubMeshChild = currentSubMeshChild->next;
            }

            // If the number of vertices processed and the number specified
            // do not match, print an error.
            if (meshVerticesProcessed != meshVertices)
            {
                notify(AT_ERROR, "parseXMLMesh: Mismatched vertex data\n");
                notify(AT_ERROR, "\tExpected: %d  Got: %d\n", meshVertices,
                    meshVerticesProcessed);
            }

            // Set up the mesh's index list
            resultMesh->setIndexListSize(meshFaces * 3);
            meshIndicesProcessed = 0;

            // Process all of the SUBMESH's FACE children now.
            currentSubMeshChild = current->children;
            while (currentSubMeshChild != NULL)
            {
                if (xmlStrcmp(currentSubMeshChild->name,
                         (const xmlChar *) "FACE") == 0)
                {
                    attribute = currentSubMeshChild->properties;
                    while (attribute)
                    {
                        if (xmlStrcmp(attribute->name,
                            (const xmlChar *) "VERTEXID") == 0)
                        {
                            tempString = (char *) xmlNodeGetContent(
                                attribute->children);
                            sscanf(tempString, "%d %d %d", &tx, &ty, &tz);

                            // Copy the vertex indices into the mesh index
                            // list
                            resultMesh->setIndex(meshIndicesProcessed, tx);
                            meshIndicesProcessed++;
                            resultMesh->setIndex(meshIndicesProcessed, ty);
                            meshIndicesProcessed++;
                            resultMesh->setIndex(meshIndicesProcessed, tz);
                            meshIndicesProcessed++;

                            // Free the XML string.
                            xmlFree(tempString);
                        }

                        attribute = attribute->next;
                    }
                }

                // Move to next node.
                currentSubMeshChild = currentSubMeshChild->next;
            }

            // If the number of indices processed and the number specified
            // do not match, print an error.
            if (meshIndicesProcessed != (meshFaces * 3))
            {
                notify(AT_ERROR, "parseXMLMesh: Mismatched face/index data\n");
                notify(AT_ERROR, "\tExpected: %d  Got: %d\n", meshFaces*3,
                    meshIndicesProcessed);
            }

            // Finalize the changes to the mesh geometry.
            resultMesh->finishNewState();

            // Propagate the initial state down the pipeline
            resultMesh->beginNewState();
            resultMesh->finishNewState();

            // Increment the number of submeshes we have processed to account
            // for the current one.
            subMeshesProcessed++;
        }

        // Move to the next possible SUBMESH.
        current = current->next;
    }

    // Free the document and the buffer.
    xmlFreeDoc(document);
    delete [] fileBuffer;

    return resultComponent;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsCal3DMeshLoader::getClassName()
{
    return "vsCal3DMeshLoader";
}

// ------------------------------------------------------------------------
// Clear the currently built list of material data.
// ------------------------------------------------------------------------
void vsCal3DMeshLoader::clearMaterials()
{
    // Delete material array's elements.
    materialList->removeAllEntries();
    materialCount = 0;
}

// ------------------------------------------------------------------------
// Load a material file.  First checking if it is a valid filename.
// ------------------------------------------------------------------------
void vsCal3DMeshLoader::loadMaterial(char *filename)
{
    int    nameLength;
    char   fileEnding[5];

    nameLength = strlen(filename);

    // If the filename is not long enough to contain a name and ending,
    // print an error.
    if (nameLength < 5)
    {
        notify(AT_ERROR, "vsCal3DMeshLoader::loadMaterial: Load of '%s' "
            "failed\n", filename);
        return;
    }

    // Convert the file ending to all upper case, so we are able to ignore
    // case in the filename itself.
    fileEnding[0] = filename[nameLength - 4];
    fileEnding[1] = toupper(filename[nameLength - 3]);
    fileEnding[2] = toupper(filename[nameLength - 2]);
    fileEnding[3] = toupper(filename[nameLength - 1]);
    fileEnding[4] = 0;

    // If it ends in the proper file ending, further parse it.
    if (strcmp(fileEnding, ".XRF") == 0)
    {
        parseXMLMaterial(filename);
    }
    // If it ends in the binary type file ending, print an approriate error.
    else if (strcmp(fileEnding, ".CRF") == 0)
    {
        notify(AT_ERROR, "loadMaterial: Load of '%s' failed\n"
            "\tCan only load the .xrf variants.\n", filename);
    }
    // If it ends in anything else, print an approriate error.
    else
    {
        notify(AT_ERROR, "loadMaterial: Load of '%s' failed!\n"
            "\tUnknown file ending.\n", filename);
    }
}

// ------------------------------------------------------------------------
// Load a mesh file.  First checking if it is a valid filename.
// ------------------------------------------------------------------------
vsComponent *vsCal3DMeshLoader::loadMesh(char *filename)
{
    int    nameLength;
    char   fileEnding[5];

    nameLength = strlen(filename);

    // If the filename is not long enough to contain a name and ending,
    // print an error.
    if (nameLength < 5)
    {
        notify(AT_ERROR, "loadMesh: Load of '%s' failed\n", filename);
        return NULL;
    }

    // Convert the file ending to all upper case, so we are able to ignore
    // case in the filename itself.
    fileEnding[0] = filename[nameLength - 4];
    fileEnding[1] = toupper(filename[nameLength - 3]);
    fileEnding[2] = toupper(filename[nameLength - 2]);
    fileEnding[3] = toupper(filename[nameLength - 1]);
    fileEnding[4] = 0; 

    // If it ends in the proper file ending, further parse it.
    if (strcmp(fileEnding, ".XMF") == 0)
    {
        return parseXMLMesh(filename, NULL);
    }
    // If it ends in the binary type file ending, print an approriate error.
    else if (strcmp(fileEnding, ".CMF") == 0)
    {
        notify(AT_ERROR, "loadMesh: Load of '%s' failed\n"
            "\tCan only load the .xmf variants.\n", filename);
    }
    // If it ends in anything else, print an approriate error.
    else
    {
        notify(AT_ERROR, "loadMesh: Load of '%s' failed\n"
            "\tUnknown file ending.\n", filename);
    }

    return NULL;
}

// ------------------------------------------------------------------------
// Load a mesh file.  First checking if it is a valid filename.  Use the
// given vsComponent as the root for the mesh.
// ------------------------------------------------------------------------
vsComponent *vsCal3DMeshLoader::loadMesh(char *filename, vsComponent *rootNode)
{
    int    nameLength;
    char   fileEnding[5];

    nameLength = strlen(filename);

    // If the filename is not long enough to contain a name and ending,
    // print an error.
    if (nameLength < 5)
    {
        notify(AT_ERROR, "loadMesh: Load of '%s' failed\n", filename);
        return NULL;
    }

    // Convert the file ending to all upper case, so we are able to ignore
    // case in the filename itself.
    fileEnding[0] = filename[nameLength - 4];
    fileEnding[1] = toupper(filename[nameLength - 3]);
    fileEnding[2] = toupper(filename[nameLength - 2]);
    fileEnding[3] = toupper(filename[nameLength - 1]);
    fileEnding[4] = 0;

    // If it ends in the proper file ending, further parse it.
    if (strcmp(fileEnding, ".XMF") == 0)
    {
        return parseXMLMesh(filename, rootNode);
    }
    // If it ends in the binary type file ending, print an approriate error.
    else if (strcmp(fileEnding, ".CMF") == 0)
    {
        notify(AT_ERROR, "loadMesh: Load of '%s' failed\n"
            "\tCan only load the .xmf variants.\n", filename);
    }
    // If it ends in anything else, print an approriate error.
    else
    {
        notify(AT_ERROR, "loadMesh: Load of '%s' failed\n"
            "\tUnknown file ending.\n", filename);
    }

    return NULL;
}
