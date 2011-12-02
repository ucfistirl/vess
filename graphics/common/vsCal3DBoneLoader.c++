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
//    VESS Module:  vsCal3DBoneLoader.c++
//
//    Description:  Object for loading Cal3D skeleton files.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#include "vsCal3DBoneLoader.h++"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "vsTransformAttribute.h++"
#include "atArray.h++"

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
vsCal3DBoneLoader::vsCal3DBoneLoader()
{
   // Set the notification name
   setName("vsCal3DBoneLoader");

   boneSpaceMatrixList = NULL;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsCal3DBoneLoader::~vsCal3DBoneLoader()
{
   // Delete the bone space matrices (if any)
   if (boneSpaceMatrixList)
       delete boneSpaceMatrixList;
}

// ------------------------------------------------------------------------
// Given a filename (without prepended directory), this function will find
// return a filename that exists with a prepended directory that has been
// added to the DirectoryNode listing. If there is no file in the
// listed directories, this function will return NULL. This is a private
// helper function.
// ------------------------------------------------------------------------
atString vsCal3DBoneLoader::findFile(atString filename)
{
   atString *path;
   char *absoluteFilename;
   char tempString[500];

   // Loop through the list of directories.
   path = (atString *)directoryList.getFirstEntry();
   while (path)
   {
      // Create the tempString
      strcpy(tempString, path->getString());
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
      
      // Move on to the next path
      path = (atString *)directoryList.getNextEntry();
   }
   
   
   // We didn't find the file, so just return the original string.
   return filename;
}

// ------------------------------------------------------------------------
// Adds a directory listing to the list that we should search for files in.
// ------------------------------------------------------------------------
void vsCal3DBoneLoader::addFilePath(const char *dirName)
{
   atString *directory;

   // Create a new string and fill it with the specified directory 
   directory = new atString();
   directory->setString(dirName);

   // Add the new directory to our list
   directoryList.addEntry(directory);
}

// ------------------------------------------------------------------------
// Recursively traverse up the tree to find the root node/bone.
// ------------------------------------------------------------------------
vsComponent *vsCal3DBoneLoader::getRootBone(vsComponent *current)
{
    if (current->getParentCount() > 0)
    {
        return getRootBone((vsComponent *)current->getParent(0));
    }
    else
        return current;
}

// ------------------------------------------------------------------------
// This function performs the actual XML parsing of the skeleton file.
// It builds up the skeleton information and returns a vsSkeleton with
// all the needed information.
// ------------------------------------------------------------------------
vsSkeleton *vsCal3DBoneLoader::parseXML(char *filename)
{
    atString                filepath;
    FILE                    *filePointer;
    char                    *fileBuffer;
    long                    fileSize;
    xmlValidCtxt            context;
    xmlDocPtr               document;
    xmlNodePtr              current;
    xmlNodePtr              currentBoneChild;
    xmlAttrPtr              attribute;
    bool                    validVersion;
    bool                    validRotation;
    bool                    validTranslation;
    bool                    validLocalRotation;
    bool                    validLocalTranslation;
    int                     boneCount;
    int                     bonesProcessed;
    int                     boneID;
    int                     boneChildrenCount;
    int                     boneChildrenProcessed;
    int                     boneChildID;
    char                    *boneName;
    char                    *tempString;
    double                  translationX;
    double                  translationY;
    double                  translationZ;
    double                  localTranslationX;
    double                  localTranslationY;
    double                  localTranslationZ;
    double                  x, y, z, w;
    atQuat                  rotationQuat;
    atMatrix                transformMatrix;
    atMatrix                rotationMatrix;
    atMatrix                translationMatrix;
    atQuat                  localRotationQuat;
    atMatrix                localTransformMatrix;
    atMatrix                localRotationMatrix;
    atMatrix                localTranslationMatrix;
    vsComponent             *rootComponent;
    vsComponent             *currentComponent;
    vsComponent             *childComponent;
    atArray                 *boneList;
    vsTransformAttribute    *boneTransform;
    atMatrix                *currentBoneSpaceMatrix;

    boneCount = 0;
    boneChildrenCount = 0;
    bonesProcessed = 0;
    boneChildrenProcessed = 0;
    validVersion = false;
    rootComponent = NULL;
    
    // Prepend an appropriate directory name from the listing we have.
    filepath = findFile(filename);
    
    // Attempt to open the file for reading.
    if ((filePointer = fopen(filepath.getString(), "r")) == NULL)
    {
        notify(AT_ERROR, "parseXML: Error opening file!\n");
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
        strlen(VS_CAL3D_XML_SKELETON_BEGIN_TAG) +
        strlen(VS_CAL3D_XML_SKELETON_END_TAG) + 1];

    // Fill in the buffer with the initial begin tag, then read the file
    // content and then concatenate the end tag.
    strcpy(fileBuffer, VS_CAL3D_XML_SKELETON_BEGIN_TAG);
    fileSize = fread(&(fileBuffer[strlen(VS_CAL3D_XML_SKELETON_BEGIN_TAG)]), 1, fileSize,
        filePointer);
    // Need to insert a null because fread() does not.
    fileBuffer[strlen(VS_CAL3D_XML_SKELETON_BEGIN_TAG)+fileSize] = 0;
    strcat(fileBuffer, VS_CAL3D_XML_SKELETON_END_TAG);

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
        notify(AT_ERROR, "parseXML: Document not parsed successfully.\n");
        xmlFreeDoc(document);
        delete [] fileBuffer;
        return NULL;
    }

    // Get the root element of the file.
    current = xmlDocGetRootElement(document);

    // If the root element is NULL, then the file is empty.
    if (current == NULL)
    {
        notify(AT_ERROR, "parseXML: Empty document.\n");
        xmlFreeDoc(document);
        delete [] fileBuffer;
        return NULL;
    }

    // Go to the child because we created the parent and don't
    // need to parse him (VS_CAL3D_XML_SKELETON_BEGIN_TAG).
    current = current->children;

    // If the HEADER field is encountered, process its properties.
    if (xmlStrcmp(current->name, (const xmlChar *) "SKELETON") == 0)
    {
        // Traverse the properties of this tag.
        attribute = current->properties;
        while (attribute != NULL)
        {
            // Get the number of bones in this skeleton.
            if (xmlStrcmp(attribute->name, (const xmlChar *) "NUMBONES") == 0)
            {
                boneCount = atoi((const char *)
                    XML_GET_CONTENT(attribute->children));
            }
            // Else if the property is named VERSION, check to see if it is the
            // proper value "900".
            else if (xmlStrcmp(attribute->name,
                     (const xmlChar *) "VERSION") == 0)
            {
                if (atoi((const char *)XML_GET_CONTENT(attribute->children))
                    >= 900)
                {
                    validVersion = true;
                }
                else
                {
                    notify(AT_ERROR,
                           "parseXML: File older than version 900!\n");
                }
            }

            // Move to the next property.
            attribute = attribute->next;
        }
    }

    // If either the magic and version properties were invalid or not found,
    // print error, free resources and return.
    if (!validVersion)
    {
        notify(AT_ERROR, "parseXML: Document of wrong type.\n");
        xmlFreeDoc(document);
        delete [] fileBuffer;
        return NULL;
    }

    // If we have no bones, then it is an error.
    if (boneCount == 0)
    {
        notify(AT_ERROR, "parseXML: No bones found!\n");
        xmlFreeDoc(document);
        delete [] fileBuffer;
        return NULL;
    }

    // Create an array to hold the bone space matrices (delete the existing
    // array if there is one)
    if (boneSpaceMatrixList != NULL)
        delete boneSpaceMatrixList;
    boneSpaceMatrixList = new atArray();

    // Create the array and fill it with vsComponents to be used for each bone.
    // Done so in advance so when establishing the hierarchy all the expected
    // bones should be vsComponents already.
    boneList = new atArray();
    for (boneID = 0; boneID < boneCount; boneID++)
    {
        currentComponent = new vsComponent();
        boneList->setEntry(boneID, currentComponent);

        currentBoneSpaceMatrix = new atMatrix();
        boneSpaceMatrixList->setEntry(boneID, currentBoneSpaceMatrix);
    }

    // Move to the children of SKELETON, and process them all.  These
    // are the actual bones we need.
    current = current->children;
    while (current)
    {
        // Insure the child is a BONE to perform further processing.
        if (xmlStrcmp(current->name, (const xmlChar *) "BONE") == 0)
        {
            // Process all if the BONE's properties.
            attribute = current->properties;
            while (attribute)
            {
                // If it is the bone id, store it.
                if (xmlStrcmp(attribute->name, (const xmlChar *) "ID") == 0)
                {
                    boneID = atoi((const char *)
                        XML_GET_CONTENT(attribute->children));
                }
                // Else if it is the bone name, keep a reference to it.
                else if (xmlStrcmp(attribute->name, (const xmlChar *) "NAME")
                         == 0)
                {
                    boneName = (char*) XML_GET_CONTENT(attribute->children);
                }
                // Else if it is the number of children, store the value.
                else if (xmlStrcmp(attribute->name,
                         (const xmlChar *) "NUMCHILDS") == 0)
                {
                    boneChildrenCount = atoi((const char *)
                        XML_GET_CONTENT(attribute->children));
                }

                // Move to the next property.
                attribute = attribute->next;
            }

            // Get the component that should correspond to the current bone.
            // If it does not exist, create it.
            currentComponent = (vsComponent *) boneList->getEntry(boneID);
            if (currentComponent == NULL)
            {
                currentComponent = new vsComponent();
                boneList->setEntry(boneID, currentComponent);
            }

            // Get the Bone Space matrix pointer for this bone.
            currentBoneSpaceMatrix = (atMatrix *)
                boneSpaceMatrixList->getEntry(boneID);
            if (!currentBoneSpaceMatrix)
            {
                currentBoneSpaceMatrix = new atMatrix();
                boneSpaceMatrixList->setEntry(boneID, currentBoneSpaceMatrix);
            }

            // Set the assumed root to be the first bone we encounter.
            if (!rootComponent)
                rootComponent = currentComponent;

            // Set the component/bone's name to what we read.
            currentComponent->setName(boneName);

            // Reset the variables per BONE.
            boneChildrenProcessed = 0;
            validRotation = false;
            validTranslation = false;
            translationX = 0;
            translationY = 0;
            translationZ = 0;
            rotationQuat.clear();
            transformMatrix.setIdentity();
            rotationMatrix.setIdentity();
            translationMatrix.setIdentity();

            validLocalRotation = false;
            validLocalTranslation = false;
            localTranslationX = 0;
            localTranslationY = 0;
            localTranslationZ = 0;
            localRotationQuat.clear();
            localTransformMatrix.setIdentity();
            localRotationMatrix.setIdentity();
            localTranslationMatrix.setIdentity();

            // Process all of the BONE's children now.
            currentBoneChild = current->children;
            while (currentBoneChild)
            {
                // If this is a CHILDID node, process it and add the child to
                // the vsComponent for this bone.
                if (xmlStrcmp(currentBoneChild->name,
                    (const xmlChar *) "CHILDID") == 0)
                {
                    boneChildID = atoi((const char *)
                        XML_GET_CONTENT(currentBoneChild->children));

                    // Get the component that corresponds to the child bone.
                    // Create it if necessary.
                    childComponent = (vsComponent *)
                        boneList->getEntry(boneChildID);
                    if (childComponent == NULL)
                    {
                        childComponent = new vsComponent();
                        boneList->setEntry(boneChildID, childComponent);
                    }

                    // Make that a child of the current bone component.
                    currentComponent->addChild(childComponent);

                    // Increment the counf of children processed to test
                    // if we match the specified number when finished.
                    boneChildrenProcessed++;
                }
                // Else if it is a TRANSLATION node, read in the
                // translation information.
                else if (xmlStrcmp(currentBoneChild->name,
                         (const xmlChar *) "TRANSLATION") == 0)
                {
                    // Get the string for the TRANSLATION key and parse
                    // the 3 floating point numbers from it.
                    tempString = (char *) xmlNodeGetContent(currentBoneChild);
                    sscanf(tempString, "%lf %lf %lf", &translationX,
                        &translationY, &translationZ);
                    xmlFree(tempString);

                    // Mark the fact that we processed a translation.
                    validTranslation = true;
                }
                // Else if it is a ROTATION node, read in the quaternion
                // rotation information.
                else if (xmlStrcmp(currentBoneChild->name,
                         (const xmlChar *) "ROTATION") == 0)
                {
                    // Get the string for the ROTATION key and parse the
                    // 4 floating point numbers from it.
                    tempString = (char *) xmlNodeGetContent(currentBoneChild);
                    sscanf(tempString, "%lf %lf %lf %lf", &x, &y, &z, &w);

                    // Create a quaternion from the values.
                    rotationQuat.set(x, y, z, w);
                    rotationQuat.invert();

                    // Set a matrix to represent the quaternion rotation.
                    rotationMatrix.setQuatRotation(rotationQuat);
                    xmlFree(tempString);

                    // Mark the fact that we processed a rotation.
                    validRotation = true;
                }
                // Else if it is a LOCALTRANSLATION node, read in the
                // translation information.
                else if (xmlStrcmp(currentBoneChild->name,
                         (const xmlChar *) "LOCALTRANSLATION") == 0)
                {
                    // Get the string for the LOCALTRANSLATION key and parse
                    // the 3 floating point numbers from it.
                    tempString = (char *) xmlNodeGetContent(currentBoneChild);
                    sscanf(tempString, "%lf %lf %lf", &localTranslationX,
                        &localTranslationY, &localTranslationZ);
                    xmlFree(tempString);

                    // Mark the fact that we processed a translation.
                    validLocalTranslation = true;
                }
                // Else if it is a LOCALROTATION node, read in the quaternion
                // rotation information.
                else if (xmlStrcmp(currentBoneChild->name,
                         (const xmlChar *) "LOCALROTATION") == 0)
                {
                    // Get the string for the LOCALROTATION key and parse the
                    // 4 floating point numbers from it.
                    tempString = (char *) xmlNodeGetContent(currentBoneChild);
                    sscanf(tempString, "%lf %lf %lf %lf", &x, &y, &z, &w);

                    // Create a quaternion from the values.
                    localRotationQuat.set(x, y, z, w);
                    localRotationQuat.invert();

                    // Set a matrix to represent the quaternion rotation.
                    localRotationMatrix.setQuatRotation(localRotationQuat);
                    xmlFree(tempString);

                    // Mark the fact that we processed a rotation.
                    validLocalRotation = true;
                }

                // Move to the next child of the BONE tree.
                currentBoneChild = currentBoneChild->next;
            }

            // Report if we did not find the rotation or translation.
            if (!validTranslation)
            {
                notify(AT_ERROR,
                       "parseXML: Could not find translation information!\n");
            }
            if (!validRotation)
            {
                notify(AT_ERROR,
                       "parseXML: Could not find rotation information!\n");
            }
            // Report if we did not find the rotation or translation.
            if (!validLocalTranslation)
            {
                notify(AT_ERROR,
                       "parseXML: Could not find local translation "
                       "information!\n");
            }
            if (!validLocalRotation)
            {
                notify(AT_ERROR,
                       "parseXML: Could not find local rotation "
                       "information!\n");
            }

            // Check the children count and see if we found the right number.
            if (boneChildrenCount != boneChildrenProcessed)
            {
                notify(AT_ERROR,
                       "parseXML: Possible error in children specification.\n"
                       "\tExpected: %d \tFound:%d\n",
                       boneChildrenCount, boneChildrenProcessed);
            }

            // Combine the translation and rotation.
            translationMatrix.setTranslation(translationX, translationY,
                translationZ);
            transformMatrix = translationMatrix * rotationMatrix;

            // Combine the local translation and rotation.
            localTranslationMatrix.setTranslation(localTranslationX,
                localTranslationY, localTranslationZ);
            localTransformMatrix = localTranslationMatrix * localRotationMatrix;
            *currentBoneSpaceMatrix = localTransformMatrix;

            // Create, set, and attach the transform attribute for the bone.
            boneTransform = new vsTransformAttribute();
            boneTransform->setPreTransform(transformMatrix);
            currentComponent->addAttribute(boneTransform);

            // Increment the number of bones we have processed to account
            // for the current one.
            bonesProcessed++;
        }

        // Move to the next possible BONE.
        current = current->next;
    }

    // If we did not process the same number of bones as specified in the
    // SKELETON property, then assume an error.
    if (bonesProcessed != boneCount)
    {
        notify(AT_ERROR,
               "parseXML: Possible error in bone specification.\n"
               "\tExpected: %d \tFound:%d\n", boneCount, bonesProcessed);

        xmlFreeDoc(document);
        delete [] fileBuffer;
        return NULL;
    }

    // Free the document and the buffer.
    xmlFreeDoc(document);
    delete [] fileBuffer;

    // Insure our assumed root component is the actual root.
    rootComponent = getRootBone(rootComponent);

    // Return a new vsSkeleton pointer with the bone data just read.
    // We do not free the boneList or the components in it because that will
    // be the responsability of the vsSkeleton object now.
    return (new vsSkeleton(boneList, boneList->getNumEntries(), rootComponent));
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsCal3DBoneLoader::getClassName()
{
    return "vsCal3DBoneLoader";
}

// ------------------------------------------------------------------------
// Call to attempt to load a Cal3D bone file.  Returns the vsSkeleton
// created from it.
// ------------------------------------------------------------------------
vsSkeleton *vsCal3DBoneLoader::loadSkeleton(char *filename)
{
    int    nameLength;
    char   fileEnding[5];

    nameLength = strlen(filename);

    // If the name is only a file ending, return NULL.
    if (nameLength < 5)
    {
        notify(AT_ERROR, "loadSkeleton: Load of '%s' failed\n", filename);
        return NULL;
    }

    // Convert the file ending to upper case, for the sake of checking.
    fileEnding[0] = filename[nameLength - 4];
    fileEnding[1] = toupper(filename[nameLength - 3]);
    fileEnding[2] = toupper(filename[nameLength - 2]);
    fileEnding[3] = toupper(filename[nameLength - 1]);
    fileEnding[4] = 0; 

    // If it is an XML bone definition file, process it.
    if (strcmp(fileEnding, ".XSF") == 0)
    {
        return parseXML(filename);
    }
    // If it is the binary version, print an error.
    else if (strcmp(fileEnding, ".CSF") == 0)
    {
        notify(AT_ERROR, "loadSkeleton: Load of '%s' failed\n"
            "\tCan only load the .xsf variants.\n", filename);
    }
    // If it is an unknown type, print an error.
    else
    {
        notify(AT_ERROR, "loadSkeleton: Load of '%s' failed\n", filename);
    }

    return NULL;
}

// ------------------------------------------------------------------------
// Return the bone space matrix list for the skeleton most recently loaded
// (or NULL if no skeleton has been loaded yet)
// ------------------------------------------------------------------------
atArray *vsCal3DBoneLoader::getBoneSpaceMatrixList()
{
    return boneSpaceMatrixList;
}
