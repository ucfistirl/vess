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
#include "vsMaterialAttribute.h++"

// Under Visual C++, we need to alias the access function and define the
// R_OK symbol for read access checks on files
#ifdef _MSC_VER
	#include <io.h>
	#define access _access
	#define R_OK 0x04
#endif

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsCal3DMeshLoader::vsCal3DMeshLoader()
{
    materialList = new vsGrowableArray(5, 1);
    materialCount = 0;
    
    directoryList = NULL;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsCal3DMeshLoader::~vsCal3DMeshLoader()
{
    clearMaterials();
    delete materialList;
    
    // Clear out the directory listing.
    DirectoryNode *tempNode;
    while (directoryList != NULL)
    {
        tempNode = directoryList;
        directoryList = directoryList->next;
        delete tempNode->dirName;
        delete tempNode;
    }
}

// ------------------------------------------------------------------------
// Given a filename (without prepended directory), this function will find
// return a filename that exists with a prepended directory that has been
// added to the DirectoryNode listing. If there is no file in the
// listed directories, this function will return NULL. This is a private
// helper function.
// ------------------------------------------------------------------------
char *vsCal3DMeshLoader::findFile(char *filename)
{
   DirectoryNode *tempNode;
   char *absoluteFilename;
   char tempString[500];
   
   // Loop through the list of directories.
   tempNode = directoryList;
   while(tempNode != NULL)
   {
      // Create the tempString
      strcpy(tempString, tempNode->dirName);
      strcat(tempString, "/");
      strcat(tempString, filename);
      
      // See if this file can be read by this process. 
      if(access(tempString, R_OK) == 0)
      {
         // Make the absoluteFilename string.
         absoluteFilename = 
            (char*)(calloc(strlen(tempString)+2, sizeof(char)));
            
         strcpy(absoluteFilename, tempString);
         
         // Return it.
         return absoluteFilename;
      }
      
      tempNode = tempNode->next;
   }
   
   // We didn't find the file, so just return the original string.
   return filename;
}

// ------------------------------------------------------------------------
// Adds a directory listing to the list that we should search for files in.
// ------------------------------------------------------------------------
void vsCal3DMeshLoader::addFilePath(const char *dirName)
{
   DirectoryNode *newNode;
   
   // Create the node and copy the directory name.
   newNode = (DirectoryNode*)(malloc(sizeof(DirectoryNode)));
   newNode->dirName = (char*)(calloc(strlen(dirName)+2, sizeof(char)));
   strcpy(newNode->dirName, dirName);
   
   // Put it at the beginning of the linked list.
   newNode->next = directoryList;
   directoryList = newNode;
}

// ------------------------------------------------------------------------
// Parse the XML material files that define Cal3D materials.
// ------------------------------------------------------------------------
void vsCal3DMeshLoader::parseXMLMaterial(char *filename)
{
    FILE                      *filePointer;
    char                      *fileBuffer;
    long                      fileSize;
    xmlValidCtxt              context;
    xmlDocPtr                 document;
    xmlNodePtr                current;
    xmlAttrPtr                attribute;
    bool                      validVersion;
    int                       currentTexture;
    int                       x, y, z, w;
    int                       index;
    char                      *tempString;
    vsSkinMaterialData        *materialData;

    currentTexture = 0;
    validVersion = false;
    
    // Prepend the directory information to the filename
    filename = findFile(filename);
    
    // If the file opening failed, print error and return.
    if ((filePointer = fopen(filename, "r")) == NULL)
    {
        fprintf(stderr, "vsCal3DMeshLoader::parseXMLMaterial: Error opening "
            "file!\n");
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
    fread(&(fileBuffer[strlen(VS_CAL3D_XML_MATERIAL_BEGIN_TAG)]), 1, fileSize,
        filePointer);
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
        fprintf(stderr, "vsCal3DMeshLoader::parseXMLMaterial: Document not "
            "parsed successfully.\n");
        xmlFreeDoc(document);
        delete [] fileBuffer;
        return;
    }

    // Get the root element of the file.
    current = xmlDocGetRootElement(document);

    // If the root element is NULL, then the file is empty.
    if (current == NULL)
    {
        fprintf(stderr, "vsCal3DMeshLoader::parseXMLMaterial: Empty "
            "document.\n");
        xmlFreeDoc(document);
        delete [] fileBuffer;
        return;
    }
    
    // Get an entry from the array which will be used for this material.
    materialData = (vsSkinMaterialData *) materialList->getData(materialCount);

    // If that entry has data in it, clear it out.
    if (materialData)
    {
        fprintf(stderr, "vsCal3DMeshLoader::parseXMLMaterial: Error: multiple "
            "definitions for material: %d\n", materialCount);
        for (index = 0; index < materialData->textureCount; index++)
            vsObject::unrefDelete(materialData->texture[index]);
        delete materialData;
    }

    // Create a new data structure for this material and place it in the array.
    materialData = new vsSkinMaterialData;
    memset(materialData, 0, sizeof(vsSkinMaterialData));
    materialList->setData(materialCount, materialData);
    
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
                materialData->textureCount = atoi((const char *)
                    XML_GET_CONTENT(attribute->children));

                // Cap it, can only support the defined maximum textures.
                if (materialData->textureCount > VS_MAXIMUM_TEXTURE_UNITS)
                    materialData->textureCount = VS_MAXIMUM_TEXTURE_UNITS;
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
        fprintf(stderr, "vsCal3DMeshLoader::parseXMLMaterial: Document of "
            "wrong type.\n");
        xmlFreeDoc(document);
        delete [] fileBuffer;
        return;
    }

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
            materialData->ambient[0] = x / 255.0;
            materialData->ambient[1] = y / 255.0;
            materialData->ambient[2] = z / 255.0;
            materialData->ambient[3] = w / 255.0;

            // Free the XML string.
            xmlFree(tempString);
        }
        // Process a DIFFUSE child.
        else if (xmlStrcmp(current->name, (const xmlChar *) "DIFFUSE") == 0)
        {
            tempString = (char *) xmlNodeGetContent(current->children);
            sscanf(tempString, "%d %d %d %d", &x, &y, &z, &w);

            // Normalize the values.
            materialData->diffuse[0] = x / 255.0;
            materialData->diffuse[1] = y / 255.0;
            materialData->diffuse[2] = z / 255.0;
            materialData->diffuse[3] = w / 255.0;

            // Free the XML string.
            xmlFree(tempString);
        }
        // Process a SPECULAR child.
        else if (xmlStrcmp(current->name, (const xmlChar *) "SPECULAR") == 0)
        {
            tempString = (char *) xmlNodeGetContent(current->children);
            sscanf(tempString, "%d %d %d %d", &x, &y, &z, &w);

            // Normalize the values.
            materialData->specular[0] = x / 255.0;
            materialData->specular[1] = y / 255.0;
            materialData->specular[2] = z / 255.0;
            materialData->specular[3] = w / 255.0;

            // Free the XML string.
            xmlFree(tempString);
        }
        // Process a SHININESS child.
        else if (xmlStrcmp(current->name, (const xmlChar *) "SHININESS") == 0)
        {
            materialData->shininess = atof((const char *)
                XML_GET_CONTENT(current->children));
        }
        // Process a MAP child.
        else if (xmlStrcmp(current->name, (const xmlChar *) "MAP") == 0)
        {
            if (currentTexture == materialData->textureCount)
            {
                fprintf(stderr, "vsCal3DMeshLoader::parseXMLMaterial: Too many "
                    "MAP children encountered!");
            }
            else
            {
                tempString = findFile(
                        (char *) XML_GET_CONTENT(current->children));
                        
                // Create a vsTextureAttribute for the texture, this attribute
                // will be referenced by any objects who use this texture.
                // Saves a significant amount of memory since textures
                // are often shared between meshes.
                materialData->texture[currentTexture] =
                    new vsTextureAttribute(currentTexture);
                materialData->texture[currentTexture]->ref();
                materialData->texture[currentTexture]->loadImageFromFile(
                    tempString);
                materialData->texture[currentTexture]->setBoundaryMode(
                    VS_TEXTURE_DIRECTION_ALL, VS_TEXTURE_BOUNDARY_CLAMP);
                materialData->texture[currentTexture]->setMagFilter(
                    VS_TEXTURE_MAGFILTER_LINEAR);
                materialData->texture[currentTexture]->setMinFilter(
                    VS_TEXTURE_MINFILTER_MIPMAP_LINEAR);
                if (currentTexture > 0)
                {
                    materialData->texture[currentTexture]->setApplyMode(
                        VS_TEXTURE_APPLY_MODULATE);
                }
                else
                {
                    materialData->texture[currentTexture]->setApplyMode(
                        VS_TEXTURE_APPLY_REPLACE);
                }

                // Increment the texture count.
                currentTexture++;
            }
        }

        // Move to next MATERIAL child.
        current = current->next;
    }

    // Finished creating material data, so increment the loaded material count.
    materialCount++;
}

// ------------------------------------------------------------------------
// Parse the XML mesh files that define Cal3D meshes.
// ------------------------------------------------------------------------
vsComponent *vsCal3DMeshLoader::parseXMLMesh(char *filename,
                                             vsComponent *rootNode)
{
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
    vsGrowableArray        *vertexDataArray;
    vsSkinVertexData       *vertexData;
    vsSkinMaterialData     *materialData;
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
    filename = findFile(filename);
    
    // If the file opening failed, print error and return.
    if ((filePointer = fopen(filename, "r")) == NULL)
    {
        fprintf(stderr, "vsCal3DMeshLoader::parseXMLMesh: Error opening "
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
    fread(&(fileBuffer[strlen(VS_CAL3D_XML_MESH_BEGIN_TAG)]), 1, fileSize,
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
        fprintf(stderr, "vsCal3DMeshLoader::parseXMLMesh: Document not parsed "
            "successfully.\n");
        xmlFreeDoc(document);
        delete [] fileBuffer;
        return NULL;
    }

    // Get the root element of the file.
    current = xmlDocGetRootElement(document);

    // If the root element is NULL, then the file is empty.
    if (current == NULL)
    {
        fprintf(stderr, "vsCal3DMeshLoader::parseXMLMesh: Empty document.\n");
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
        fprintf(stderr, "vsCal3DMeshLoader::parseXMLMesh: No meshes found!\n");
        xmlFreeDoc(document);
        delete [] fileBuffer;
        return NULL;
    }
    
    // If either the magic and version properties were invalid or not found,
    // print error, free resources and return.
    if (!validVersion)
    {
        fprintf(stderr, "vsCal3DMeshLoader::parseXMLMesh: Document of wrong "
            "type.\n");
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

            // Create array with enough space to store the specified set of
            // vertices.
            vertexDataArray = new vsGrowableArray(meshVertices, 1);

            // Initialize to no vertices processed.
            meshVerticesProcessed = 0;

            // Process all of the SUBMESH's VERTEX children now.
            currentSubMeshChild = current->children;
            while (currentSubMeshChild != NULL)
            {
                // Initialize to no texture coordinates and influences
                // processed.
                meshTexCoordsProcessed = 0;
                vertexInfluencesProcessed = 0;

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
                                fprintf(stderr,
                                    "vsCal3DMeshLoader::parseXMLMesh: Bone "
                                    "influences greater than %d, truncating\n",
                                    VS_CAL3D_MESH_LOADER_MAX_INFLUENCES);
                            }
                        }

                        // Move to next node.
                        attribute = attribute->next;
                    }

                    // Get an entry from the array with vertex information.
                    vertexData = (vsSkinVertexData *)
                        vertexDataArray->getData(vertexID);

                    // If that entry has data in it, print an error and
                    // clear it out.  Vertices should not be defined more than
                    // once.
                    if (vertexData)
                    {
                        fprintf(stderr, "vsCal3DMeshLoader::parseXMLMesh: "
                            "Error: multiple definitions for vertex: %d\n",
                            vertexID);
                        delete vertexData;
                    }

                    // Create a new data structure for this vertex and place
                    // it in the array.
                    vertexData = new vsSkinVertexData;
                    memset(vertexData, 0, sizeof(vsSkinVertexData));
                    vertexDataArray->setData(vertexID, vertexData);

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
                            sscanf(tempString, "%lf %lf %lf",
                                &(vertexData->position[0]),
                                &(vertexData->position[1]),
                                &(vertexData->position[2]));

                            // Free the XML string.
                            xmlFree(tempString);
                        }
                        // Process the NORM child.
                        else if (xmlStrcmp(currentVertexChild->name,
                            (const xmlChar *) "NORM") == 0)
                        {
                            tempString = (char *) xmlNodeGetContent(
                                currentVertexChild->children);
                            sscanf(tempString, "%lf %lf %lf",
                                &(vertexData->normal[0]),
                                &(vertexData->normal[1]),
                                &(vertexData->normal[2]));

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
                                    &(vertexData->textureCoords[
                                    meshTexCoordsProcessed][0]),
                                    &(vertexData->textureCoords[
                                    meshTexCoordsProcessed][1]));

                                // Free the XML string.
                                xmlFree(tempString);

                                // Increment the processed texture coordinates.
                                meshTexCoordsProcessed++;
                            }
                            // Too many encountered, print error.
                            else
                            {
                                fprintf(stderr,
                                    "vsCal3DMeshLoader::parseXMLMesh: "
                                    "Encountered more than %d texture "
                                    "coordinates, ignoring the rest\n",
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
                                vertexData->boneIDs[vertexInfluencesProcessed] =
                                    (double) vertexInfluenceID;
                                vertexData->weights[vertexInfluencesProcessed] =
                                    vertexInfluenceWeight;

                                // Increment the processed influences.
                                vertexInfluencesProcessed++;
                            }
                            // Too many encountered, print error.
                            else
                            {
                                fprintf(stderr,
                                    "vsCal3DMeshLoader::parseXMLMesh: "
                                    "Encountered more than %d influences, "
                                    "ignoring the rest\n",
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
                        fprintf(stderr, "vsCal3DMeshLoader::parseXMLMesh: "
                            "Missmatched texcoordinate data\n"
                            "\tExpected: %d  Got: %d\n", meshTexCoords,
                            meshTexCoordsProcessed);
                    }

                    // If the number of influences processed and
                    // the number specified do not match, print an error.
                    if (vertexInfluences != vertexInfluencesProcessed)
                    {
                        fprintf(stderr, "vsCal3DMeshLoader::parseXMLMesh: "
                            "Missmatched vertex influences data\n"
                            "\tExpected: %d  Got: %d\n", vertexInfluences,
                            vertexInfluencesProcessed);
                    }

                    // Insure weights are normalized [0, 1].
                    weightSum = 0.0;
                    for (index = 0; index < 4; index++)
                    {
                        weightSum += vertexData->weights[index];
                    }
                    for (index = 0; index < 4; index++)
                    {
                        vertexData->weights[index] =
                            vertexData->weights[index] / weightSum;
                    }

                    // Store the number of textures used by this vertex.
                    vertexData->textures = meshTexCoordsProcessed;

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
                fprintf(stderr, "vsCal3DMeshLoader::parseXMLMesh: Missmatched "
                    "vertex data\n\tExpected: %d  Got: %d\n", meshVertices,
                    meshVerticesProcessed);
            }

/*
printf("Submesh Vertices: %d\n", meshVertices);
totalVertices += meshVertices;
*/

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

//printf("Mesh Geometry Loaded: %s\n", geometryName);

            // If no component is defined, make a new one.
            if (!resultComponent)
            {
                resultComponent = new vsComponent();
            }

            // Add the new mesh to the component.
            resultComponent->addChild(resultMesh);

            // Number of vertices are 3 times the faces, 3 vertices per face.
            // Many vertices are shared but this is the fast easy way for now.
            // Set the list sizes and bindings.
            resultMesh->setDataListSize(VS_GEOMETRY_SKIN_VERTEX_COORDS,
                (meshFaces * 3));
            resultMesh->setBinding(VS_GEOMETRY_SKIN_VERTEX_COORDS,
                VS_GEOMETRY_BIND_PER_VERTEX);

            resultMesh->setDataListSize(VS_GEOMETRY_SKIN_NORMALS,
                (meshFaces * 3));
            resultMesh->setBinding(VS_GEOMETRY_SKIN_NORMALS,
                VS_GEOMETRY_BIND_PER_VERTEX);

            resultMesh->setDataListSize(VS_GEOMETRY_VERTEX_WEIGHTS,
                (meshFaces * 3));
            resultMesh->setBinding(VS_GEOMETRY_VERTEX_WEIGHTS,
                VS_GEOMETRY_BIND_PER_VERTEX);

            resultMesh->setDataListSize(VS_GEOMETRY_BONE_INDICES,
                (meshFaces * 3));
            resultMesh->setBinding(VS_GEOMETRY_BONE_INDICES,
                VS_GEOMETRY_BIND_PER_VERTEX);

            // Set color data binding and data information.
/*
// For coloring vertices according to influences.
resultMesh->setDataListSize(VS_GEOMETRY_COLORS, (meshFaces * 3));
resultMesh->setBinding(VS_GEOMETRY_COLORS, VS_GEOMETRY_BIND_PER_VERTEX);
*/
            resultMesh->setDataListSize(VS_GEOMETRY_COLORS, 1);
            resultMesh->setBinding(VS_GEOMETRY_COLORS,
                VS_GEOMETRY_BIND_OVERALL);

            // Set the mesh color to white.
            resultMesh->setData(VS_GEOMETRY_COLORS, 0,
                vsVector(1.0, 1.0, 1.0, 1.0));

            // Get and apply the mesh material to this geometry, if it has one.
            materialData = (vsSkinMaterialData *)
                materialList->getData(meshMaterial);
            if (materialData)
            {
                for (index = 0; index < materialData->textureCount; index++)
                {
                    // Set the size and binding of current texture coordinate
                    // list.
                    resultMesh->setDataListSize(
                        VS_GEOMETRY_TEXTURE0_COORDS+index, (meshFaces * 3));
                    resultMesh->setBinding(VS_GEOMETRY_TEXTURE0_COORDS+index,
                        VS_GEOMETRY_BIND_PER_VERTEX);

                    // Hand the mesh the prepared vsTextureAttribute.
                    resultMesh->addAttribute(materialData->texture[index]);
                }

                materialAttribute = new vsMaterialAttribute();
                materialAttribute->setColor(VS_MATERIAL_SIDE_BOTH,
                    VS_MATERIAL_COLOR_AMBIENT, materialData->ambient[0],
                    materialData->ambient[1], materialData->ambient[2]);
                materialAttribute->setColor(VS_MATERIAL_SIDE_BOTH,
                    VS_MATERIAL_COLOR_DIFFUSE, materialData->diffuse[0],
                    materialData->diffuse[1], materialData->diffuse[2]);
                materialAttribute->setColor(VS_MATERIAL_SIDE_BOTH,
                    VS_MATERIAL_COLOR_SPECULAR, materialData->specular[0],
                    materialData->specular[1], materialData->specular[2]);
                materialAttribute->setShininess(VS_MATERIAL_SIDE_BOTH,
                    materialData->shininess);
                materialAttribute->setColorMode(VS_MATERIAL_SIDE_BOTH,
                    VS_MATERIAL_CMODE_NONE);

                resultMesh->addAttribute(materialAttribute);
            }

            meshVerticesProcessed = 0;

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

                            // Setup the first vertex.
                            vertexData = (vsSkinVertexData *)
                                vertexDataArray->getData(tx);

                            resultMesh->setData(VS_GEOMETRY_SKIN_VERTEX_COORDS,
                                meshVerticesProcessed,
                                vsVector(3, vertexData->position));

                            resultMesh->setData(VS_GEOMETRY_SKIN_NORMALS,
                                meshVerticesProcessed,
                                vsVector(3,vertexData->normal).getNormalized());

                            resultMesh->setData(VS_GEOMETRY_VERTEX_WEIGHTS,
                                meshVerticesProcessed,
                                vsVector(4, vertexData->weights));

                            // Set all the texture coordinates defined for
                            // this vertex, if we found a material above.
                            if (materialData)
                            {
                                for (meshTexCoordsProcessed = 0;
                                     meshTexCoordsProcessed <
                                       vertexData->textures;
                                     meshTexCoordsProcessed++)
                                {
                                    resultMesh->setData(
                                        (VS_GEOMETRY_TEXTURE0_COORDS+
                                        meshTexCoordsProcessed),
                                        meshVerticesProcessed,
                                        vsVector(2, vertexData->textureCoords[
                                        meshTexCoordsProcessed]));
                                }
                            }

/*
// Color vertex according to influences.
double sum;
int count;
int debugIndex;

sum = 0.0;
count = 0;
for (debugIndex = 0; debugIndex < 4; debugIndex++)
{
    sum += vertexData->weights[debugIndex];
    if (vertexData->weights[debugIndex] > 0.0)
        count++;
}
switch (count)
{
    case 1:
        resultMesh->setData(VS_GEOMETRY_COLORS, meshVerticesProcessed,
          vsVector(1.0, 0.0, 0.0, 1.0));
        break;
    case 2:
        resultMesh->setData(VS_GEOMETRY_COLORS, meshVerticesProcessed,
          vsVector(0.0, 1.0, 0.0, 1.0));
        break;
    case 3:
        resultMesh->setData(VS_GEOMETRY_COLORS, meshVerticesProcessed,
          vsVector(0.0, 0.0, 1.0, 1.0));
        break;
    case 4:
        resultMesh->setData(VS_GEOMETRY_COLORS, meshVerticesProcessed,
          vsVector(1.0, 1.0, 0.0, 1.0));
        break;
    default:
        resultMesh->setData(VS_GEOMETRY_COLORS, meshVerticesProcessed,
          vsVector(1.0, 1.0, 1.0, 1.0));
}
*/

                            resultMesh->setData(VS_GEOMETRY_BONE_INDICES,
                                meshVerticesProcessed,
                                vsVector(4, vertexData->boneIDs));

                            meshVerticesProcessed++;
                            // Finished with first vertex.

                            // Setup the second vertex.
                            vertexData = (vsSkinVertexData *)
                                vertexDataArray->getData(ty);

                            resultMesh->setData(VS_GEOMETRY_SKIN_VERTEX_COORDS,
                                meshVerticesProcessed,
                                vsVector(3, vertexData->position));

                            resultMesh->setData(VS_GEOMETRY_SKIN_NORMALS,
                                meshVerticesProcessed,
                                vsVector(3,vertexData->normal).getNormalized());

                            resultMesh->setData(VS_GEOMETRY_VERTEX_WEIGHTS,
                                meshVerticesProcessed,
                                vsVector(4, vertexData->weights));

                            // Set all the texture coordinates defined for
                            // this vertex, if we found a material above.
                            if (materialData)
                            {
                                for (meshTexCoordsProcessed = 0;
                                     meshTexCoordsProcessed <
                                       vertexData->textures;
                                     meshTexCoordsProcessed++)
                                {
                                    resultMesh->setData(
                                        (VS_GEOMETRY_TEXTURE0_COORDS+
                                        meshTexCoordsProcessed),
                                        meshVerticesProcessed,
                                        vsVector(2, vertexData->textureCoords[
                                        meshTexCoordsProcessed]));
                                }
                            }

/*
// Color vertex according to influences.
sum = 0.0;
count = 0;
for (debugIndex = 0; debugIndex < 4; debugIndex++)
{
    sum += vertexData->weights[debugIndex];
    if (vertexData->weights[debugIndex] > 0.0)
        count++;
}
switch (count)
{
    case 1:
        resultMesh->setData(VS_GEOMETRY_COLORS, meshVerticesProcessed,
          vsVector(1.0, 0.0, 0.0, 1.0));
        break;
    case 2:
        resultMesh->setData(VS_GEOMETRY_COLORS, meshVerticesProcessed,
          vsVector(0.0, 1.0, 0.0, 1.0));
        break;
    case 3:
        resultMesh->setData(VS_GEOMETRY_COLORS, meshVerticesProcessed,
          vsVector(0.0, 0.0, 1.0, 1.0));
        break;
    case 4:
        resultMesh->setData(VS_GEOMETRY_COLORS, meshVerticesProcessed,
          vsVector(1.0, 1.0, 0.0, 1.0));
        break;
    default:
        resultMesh->setData(VS_GEOMETRY_COLORS, meshVerticesProcessed,
          vsVector(1.0, 1.0, 1.0, 1.0));
}
*/

                            resultMesh->setData(VS_GEOMETRY_BONE_INDICES,
                                meshVerticesProcessed,
                                vsVector(4, vertexData->boneIDs));
                            
                            meshVerticesProcessed++;
                            // Finished with second vertex.

                            // Setup the third vertex.
                            vertexData = (vsSkinVertexData *)
                                vertexDataArray->getData(tz);

                            resultMesh->setData(VS_GEOMETRY_SKIN_VERTEX_COORDS,
                                meshVerticesProcessed,
                                vsVector(3, vertexData->position));

                            resultMesh->setData(VS_GEOMETRY_SKIN_NORMALS,
                                meshVerticesProcessed,
                                vsVector(3,vertexData->normal).getNormalized());

                            resultMesh->setData(VS_GEOMETRY_VERTEX_WEIGHTS,
                                meshVerticesProcessed,
                                vsVector(4, vertexData->weights));

                            // Set all the texture coordinates defined for
                            // this vertex, if we found a material above.
                            if (materialData)
                            {
                                for (meshTexCoordsProcessed = 0;
                                     meshTexCoordsProcessed <
                                       vertexData->textures;
                                     meshTexCoordsProcessed++)
                                {
                                    resultMesh->setData(
                                        (VS_GEOMETRY_TEXTURE0_COORDS+
                                        meshTexCoordsProcessed),
                                        meshVerticesProcessed,
                                        vsVector(2, vertexData->textureCoords[
                                        meshTexCoordsProcessed]));
                                }
                            }

/*
// Color vertex accoring to influences.
sum = 0.0;
count = 0;
for (debugIndex = 0; debugIndex < 4; debugIndex++)
{
    sum += vertexData->weights[debugIndex];
    if (vertexData->weights[debugIndex] > 0.0)
        count++;
}
switch (count)
{
    case 1:
        resultMesh->setData(VS_GEOMETRY_COLORS, meshVerticesProcessed,
          vsVector(1.0, 0.0, 0.0, 1.0));
        break;
    case 2:
        resultMesh->setData(VS_GEOMETRY_COLORS, meshVerticesProcessed,
          vsVector(0.0, 1.0, 0.0, 1.0));
        break;
    case 3:
        resultMesh->setData(VS_GEOMETRY_COLORS, meshVerticesProcessed,
          vsVector(0.0, 0.0, 1.0, 1.0));
        break;
    case 4:
        resultMesh->setData(VS_GEOMETRY_COLORS, meshVerticesProcessed,
          vsVector(1.0, 1.0, 0.0, 1.0));
        break;
    default:
        resultMesh->setData(VS_GEOMETRY_COLORS, meshVerticesProcessed,
          vsVector(1.0, 1.0, 1.0, 1.0));
}
*/

                            resultMesh->setData(VS_GEOMETRY_BONE_INDICES,
                                meshVerticesProcessed,
                                vsVector(4, vertexData->boneIDs));

                            meshVerticesProcessed++;
                            // Finished with third vertex.

                            // Free the XML string.
                            xmlFree(tempString);
                        }

                        attribute = attribute->next;
                    }
//totalFaces++;
                }

                // Move to next node.
                currentSubMeshChild = currentSubMeshChild->next;
            }

            // Finalize the changes to the mesh geometry.
            resultMesh->finishNewState();

            // Increment the number of submeshes we have processed to account
            // for the current one.
            subMeshesProcessed++;
        }

        // Move to the next possible SUBMESH.
        current = current->next;
    }

    // If we did not process the same number of bones as specified in the
    // SKELETON property, then assume an error.
    if (subMeshesProcessed != subMeshCount)
    {
        fprintf(stderr, "vsCal3DMeshLoader::parseXMLMesh: Mesh processing "
            "error!\n");
        xmlFreeDoc(document);
        delete [] fileBuffer;
        for (index = 0; index < vertexDataArray->getSize(); index++)
        {
            delete (vsSkinVertexData *) vertexDataArray->getData(index);
        }
        delete vertexDataArray;
        return NULL;
    }

/*
printf("subMeshCount: %d\n", subMeshCount);
printf("subMeshes Processed: %d\n", subMeshesProcessed);
printf("Total Vertices Processed: %ld\n", totalVertices);
printf("Total Faces Processed: %ld\n", totalFaces);
*/

    // Free the document and the buffer.
    xmlFreeDoc(document);
    delete [] fileBuffer;
    for (index = 0; index < vertexDataArray->getSize(); index++)
    {
        delete (vsSkinVertexData *) vertexDataArray->getData(index);
    }
    delete vertexDataArray;

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
    vsSkinMaterialData *tempData;
    int index;

    // Delete material array's elements.
    for (materialCount--; materialCount >= 0; materialCount--)
    {
        tempData = (vsSkinMaterialData *) materialList->getData(materialCount);
        for (index = 0; index < tempData->textureCount; index++)
            vsObject::unrefDelete(tempData->texture[index]);
        delete tempData;
        materialList->setData(materialCount, NULL);
    }
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
        fprintf(stderr, "vsCal3DMeshLoader::loadMaterial: Load of '%s' "
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
        fprintf(stderr, "vsCal3DMeshLoader::loadMaterial: Load of '%s' failed\n"
            "\tCan only load the .xrf variants.\n", filename);
    }
    // If it ends in anything else, print an approriate error.
    else
    {
        fprintf(stderr, "vsCal3DMeshLoader::loadMaterial: Load of '%s' "
            "failed!\n\tUnknown file ending.\n", filename);
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
        fprintf(stderr, "vsCal3DMeshLoader::loadMesh: Load of '%s' failed\n",
            filename);
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
        fprintf(stderr, "vsCal3DMeshLoader::loadMesh: Load of '%s' failed\n"
            "\tCan only load the .xmf variants.\n", filename);
    }
    // If it ends in anything else, print an approriate error.
    else
    {
        fprintf(stderr, "vsCal3DMeshLoader::loadMesh: Load of '%s' failed\n"
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
        fprintf(stderr, "vsCal3DMeshLoader::loadMesh: Load of '%s' failed\n",
            filename);
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
        fprintf(stderr, "vsCal3DMeshLoader::loadMesh: Load of '%s' failed\n"
            "\tCan only load the .xmf variants.\n", filename);
    }
    // If it ends in anything else, print an approriate error.
    else
    {
        fprintf(stderr, "vsCal3DMeshLoader::loadMesh: Load of '%s' failed\n"
            "\tUnknown file ending.\n", filename);
    }

    return NULL;
}
