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
//    VESS Module:  vsCal3DAnimationLoader.c++
//
//    Description:  
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------
                                                                                
#include "vsCal3DAnimationLoader.h++"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "vsPathMotion.h++"

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
vsCal3DAnimationLoader::vsCal3DAnimationLoader()
{
   directoryList = NULL;
}
                                                                                
// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsCal3DAnimationLoader::~vsCal3DAnimationLoader()
{
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
char *vsCal3DAnimationLoader::findFile(char *filename)
{
   DirectoryNode *tempNode;
   char *absoluteFilename;
   char tempString[500];
   
   // Loop through the list of directories.
   tempNode = directoryList;
   while (tempNode != NULL)
   {
      // Create the tempString
      strcpy(tempString, tempNode->dirName);
      strcat(tempString, "/");
      strcat(tempString, filename);
      
      // See if this file can be read by this process. 
      if (access(tempString, R_OK) == 0)
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
void vsCal3DAnimationLoader::addFilePath(const char *dirName)
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
// This function performs the actual XML parsing of the animation file.
// It builds up vsPathMotions for each bone's motion and returns the list
// of these vsPathMotions in a vsPathMotionManager.
// ------------------------------------------------------------------------
vsPathMotionManager *vsCal3DAnimationLoader::parseXML(char *filename,
    vsSkeletonKinematics *skeletonKinematics)
{
    FILE                    *filePointer;
    char                    *fileBuffer;
    long                    fileSize;
    xmlValidCtxt            context;
    xmlDocPtr               document;
    xmlNodePtr              current;
    xmlNodePtr              currentTrackChild;
    xmlNodePtr              currentKeyframeChild;
    xmlAttrPtr              attribute;
    bool                    validVersion;
    int                     trackCount;
    int                     tracksProcessed;
    int                     currentBoneID;
    int                     keyframeCount;
    int                     keyframesProcessed;
    double                  animationDuration;
    double                  previousKeyframeTime;
    double                  keyframeTime;
    double                  x, y, z, w;
    char                    *tempString;
    vsPathMotionManager     *pathMotionManager;
    vsPathMotion            *bonePathMotion;
    vsKinematics            *boneKinematics;
    vsQuat                  rotation;
    vsVector                position;
    vsMatrix                relativeBoneTransform;
    vsQuat                  inverseRelativeBoneRotation;
    vsVector                relativeBonePosition;

    trackCount = 0;
    tracksProcessed = 0;
    animationDuration = 0.0;
    validVersion = false;
    
    // Append an appropriate directory name to the filename.
    filename = findFile(filename);
    
    // Attempt to open the file for reading.
    if ((filePointer = fopen(filename, "r")) == NULL)
    {
        fprintf(stderr, "vsCal3DAnimationLoader::parseXML: Error opening "
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
        strlen(VS_CAL3D_XML_ANIMATION_BEGIN_TAG) +
        strlen(VS_CAL3D_XML_ANIMATION_END_TAG) + 1];

    // Fill in the buffer with the initial begin tag, then read the file
    // content and then concatenate the end tag.
    strcpy(fileBuffer, VS_CAL3D_XML_ANIMATION_BEGIN_TAG);
    fileSize = fread(&(fileBuffer[strlen(VS_CAL3D_XML_ANIMATION_BEGIN_TAG)]), 1, fileSize,
        filePointer);
    // Need to insert a null because fread() does not.
    fileBuffer[strlen(VS_CAL3D_XML_ANIMATION_BEGIN_TAG)+fileSize] = 0;
    strcat(fileBuffer, VS_CAL3D_XML_ANIMATION_END_TAG);

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
        fprintf(stderr, "vsCal3DAnimationLoader::parseXML: Document not parsed "
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
        fprintf(stderr, "vsCal3DAnimationLoader::parseXML: Empty document.\n");
        xmlFreeDoc(document);
        delete [] fileBuffer;
        return NULL;
    }

    // Go to the child because we created the parent and don't
    // need to parse him (VS_CAL3D_XML_ANIMATION_BEGIN_TAG).
    current = current->children;

    // If the HEADER field is encountered, process its properties.
    if (xmlStrcmp(current->name, (const xmlChar *) "ANIMATION") == 0)
    {
        // Traverse the properties of this tag.
        attribute = current->properties;
        while (attribute != NULL)
        {
            // If it is a DURATION property, store the value.
            if (xmlStrcmp(attribute->name, (const xmlChar *) "DURATION") == 0)
            {
                animationDuration = atof((const char *)
                    XML_GET_CONTENT(attribute->children));
            }
            // If it is a DURATION property, store the value.
            else if (xmlStrcmp(attribute->name, (const xmlChar *) "NUMTRACKS")
                     == 0)
            {
                trackCount = atoi((const char *)
                    XML_GET_CONTENT(attribute->children));
            }
            // Else if the property is named VERSION, check to see if it is the
            // proper value "1000".
            else if (xmlStrcmp(attribute->name,
                     (const xmlChar *) "VERSION") == 0)
            {
                if (atoi((const char *)XML_GET_CONTENT(attribute->children))
                    >= 1000)
                {
                    validVersion = true;
                }
                else
                {
                    fprintf(stderr, "vsCal3DAnimationLoader::parseXML: File "
                        "older than version 1000!\n");
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
        fprintf(stderr, "vsCal3DAnimationLoader::parseXML: Document of wrong "
            "type.\n");
        xmlFreeDoc(document);
        delete [] fileBuffer;
        return NULL;
    }

    // If the duration is 0, then it is an error.
    if (animationDuration == 0.0)
    {
        fprintf(stderr, "vsCal3DAnimationLoader::parseXML: 0.0 duration!\n");
        xmlFreeDoc(document);
        delete [] fileBuffer;
        return NULL;
    }

    // If there are 0 tracks, then it is an error.
    if (trackCount == 0)
    {
        fprintf(stderr, "vsCal3DAnimationLoader::parseXML: No tracks found!\n");
        xmlFreeDoc(document);
        delete [] fileBuffer;
        return NULL;
    }

    // So far everything is proper, go ahead and make vsPathMotionManager to
    // store all the vsPathMotion objects.
    pathMotionManager = new vsPathMotionManager();

    // Process all if the ANIMATION's children, the TRACKs.
    current = current->children;
    while (current)
    {
        // Insure the child is a TRACK to perform further processing.
        if (xmlStrcmp(current->name, (const xmlChar *) "TRACK") == 0)
        {
            // Process all of the TRACK's properties.
            attribute = current->properties;
            while (attribute != NULL)
            {
                // Process the BONEID property.
                if (xmlStrcmp(attribute->name, (const xmlChar *) "BONEID") == 0)
                {
                    currentBoneID = atoi((const char *)
                        XML_GET_CONTENT(attribute->children));
                }
                // Process the NUMKEYFRAMES property.
                else if (xmlStrcmp(attribute->name,
                         (const xmlChar *) "NUMKEYFRAMES") == 0)
                {
                    keyframeCount = atoi((const char *)
                        XML_GET_CONTENT(attribute->children));
                }

                // Move to next TRACK property.
                attribute = attribute->next;
            }

            // Get the bone kinematics that this track affects.
            boneKinematics = skeletonKinematics->getBoneKinematics(
                currentBoneID);

            // Get the relative position of the current bone from its parent.
            relativeBoneTransform = ((vsTransformAttribute *)
                boneKinematics->getComponent()->getTypedAttribute(
                VS_ATTRIBUTE_TYPE_TRANSFORM, 0))->getPreTransform();

            // Get the inverse rotation information and place in a vsQuat.
            // The rotation is concidered the inverse of what the animation
            // is using because we end up having to invert it when we load
            // the bone anyways.
            // The whole idea here is to extract the rotation which converts
            // the default bone's rotation to the animated frame's...
            // A(-1) = Default Bone rotation (what is in PreTransform)
            // B = Current frame rotation
            // C = A * B(-1)
            // C = The rotation to convert A(-1) to B(-1), which is what we
            // want to give to the PathMotion.
            inverseRelativeBoneRotation.setMatrixRotation(
                relativeBoneTransform);
            inverseRelativeBoneRotation.invert(); 

            // Get the translation information to calculate the change from
            // default to the frames translation.  The frame data specifies it
            // as an absolute translation, like the rotation, instead of a
            // relative one based on the default translation.
            relativeBoneTransform.getTranslation(&x, &y, &z);
            relativeBonePosition.setSize(3);
            relativeBonePosition.set(x, y, z);

            // If the kinematics is valid, create and setup the path motion.
            if (boneKinematics)
            {
                // Create the path motion for the bone.
                bonePathMotion = new vsPathMotion(boneKinematics);

                // Set the number of frames.
                bonePathMotion->setPointListSize(keyframeCount);

                // Add the vsPathMotion to the vsPathMotionManager.
                pathMotionManager->addPathMotion(bonePathMotion);
            }

            // Initialize to no keyframes processed.
            keyframesProcessed = 0;
            previousKeyframeTime = 0.0;

            // If the kinematics is valid continue to process keyframes,
            // else skip all the data since there is no place to put it.
            if (boneKinematics)
                currentTrackChild = current->children;
            else
                currentTrackChild = NULL;

            // Process all of the TRACKS's KEYFRAME children now.
            while (currentTrackChild != NULL)
            {
                // Process the KEYFRAME child.
                if (xmlStrcmp(currentTrackChild->name,
                    (const xmlChar *) "KEYFRAME") == 0)
                {
                    // Process all if the KEYFRAME's properties.
                    attribute = currentTrackChild->properties;
                    while (attribute != NULL)
                    {
                        // Process the TIME property.
                        if (xmlStrcmp(attribute->name,
                            (const xmlChar *) "TIME") == 0)
                        {
                            keyframeTime = atof((const char *)
                                XML_GET_CONTENT(attribute->children));

                            // Calculate the relative time for this frame and
                            // set it.
                            bonePathMotion->setTime(keyframesProcessed,
                                keyframeTime - previousKeyframeTime);

                            // Store this keyframe's time, so we can later
                            // generate a relative time for the next frame.
                            previousKeyframeTime = keyframeTime;
                        }

                        // Move to next KEYFRAME property.
                        attribute = attribute->next;
                    }

                    // Process all of the KEYFRAME's children.
                    currentKeyframeChild = currentTrackChild->children;
                    while (currentKeyframeChild != NULL)
                    {
                        // Process the TRANSLATION child.
                        if (xmlStrcmp(currentKeyframeChild->name,
                            (const xmlChar *) "TRANSLATION") == 0)
                        {
                            tempString = (char *) xmlNodeGetContent(
                                currentKeyframeChild->children);
                            sscanf(tempString, "%lf %lf %lf", &x, &y, &z);

                            // Calculate the change from the default bone
                            // position this frame is at, and set that as the
                            // frame's position.
                            position.set(x, y, z);
                            position = position - relativeBonePosition;
                            bonePathMotion->setPosition(keyframesProcessed,
                                position);
                                                                                
                            // Free the XML string.
                            xmlFree(tempString);
                        }
                        // Process the ROTATION child.
                        else if (xmlStrcmp(currentKeyframeChild->name,
                            (const xmlChar *) "ROTATION") == 0)
                        {
                            tempString = (char *) xmlNodeGetContent(
                                currentKeyframeChild->children);
                            sscanf(tempString, "%lf %lf %lf %lf",
                                &x, &y, &z, &w);

                            // Set the vsQuat with the read rotation, and 
                            // pass that to the vsPathMotion.
                            rotation.set(x, y, z, w);

                            // Insure it is normalized.
                            rotation.normalize();
                            rotation.invert();

                            // Calculate the rotation needed to move the bone
                            // from default position to this frame's position.
                            rotation = inverseRelativeBoneRotation * rotation;

                            // Set it as this frame's rotational change.
                            bonePathMotion->setOrientation(keyframesProcessed,
                                rotation);

                            // Free the XML string.
                            xmlFree(tempString);
                        }

                        // Move to next KEYFRAME child.
                        currentKeyframeChild = currentKeyframeChild->next;
                    }

                    // Increment the processed keyframe count.
                    keyframesProcessed++;
                }

                // Move to next TRACK child.
                currentTrackChild = currentTrackChild->next;
            }

            // If the number of keyframes processed and the number specified
            // do not match, print an error.
            if (keyframeCount != keyframesProcessed)
            {
                fprintf(stderr, "vsCal3DAnimationLoader::parseXML: "
                    "Missmatched keyframe data\n\tExpected: %d  Got: %d\n",
                    keyframeCount, keyframesProcessed);
            }

            // Increment the processed track count.
            tracksProcessed++;
        }

        // Move to next ANIMATION child.
        current = current->next;
    }

    // If we did not process the same number of tracks as specified in the
    // ANIMATION property, then assume an error.
    if (tracksProcessed != trackCount)
    {
        fprintf(stderr, "vsCal3DAnimationLoader::parseXML: Animation "
            "processing error!\n");
        xmlFreeDoc(document);
        delete [] fileBuffer;
        delete pathMotionManager;

        return NULL;
    }

    // Free the document and the buffer.
    xmlFreeDoc(document);
    delete [] fileBuffer;

    return pathMotionManager;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsCal3DAnimationLoader::getClassName()
{
    return "vsCal3DAnimationLoader";
}

// ------------------------------------------------------------------------
// Call to attempt to load a Cal3D animation file.  Returns the
// vsPathMotionManager created with the set of vsPathMoitions.
// ------------------------------------------------------------------------
vsPathMotionManager *vsCal3DAnimationLoader::loadAnimation(char *filename,
    vsSkeletonKinematics *skeletonKinematics)
{
    int    nameLength;
    char   fileEnding[5];

    nameLength = strlen(filename);

    // If the name is only a file ending, return NULL.
    if (nameLength < 5)
    {
        fprintf(stderr,
            "vsCal3DAnimationLoader::loadAnimation: Load of '%s' failed\n",
            filename);
        return NULL;
    }

    // Convert the file ending to upper case, for the sake of checking.
    fileEnding[0] = filename[nameLength - 4];
    fileEnding[1] = toupper(filename[nameLength - 3]);
    fileEnding[2] = toupper(filename[nameLength - 2]);
    fileEnding[3] = toupper(filename[nameLength - 1]);
    fileEnding[4] = 0;

    // If it is an XML animation definition file, process it.
    if (strcmp(fileEnding, ".XAF") == 0)
    {
        return parseXML(filename, skeletonKinematics);
    }
    // If it is the binary version, print an error.
    else if (strcmp(fileEnding, ".CAF") == 0)
    {
        fprintf(stderr, "vsCal3DAnimationLoader::loadAnimation: Load of '%s' "
            "failed\n\tCan only load the .xaf variants.\n", filename);
    }
    // If it is an unknown type, print an error.
    else
    {
        fprintf(stderr,
            "vsCal3DAnimationLoader::loadAnimation: Load of '%s' failed"
            " file ending: \"%s\".\n",
            filename, fileEnding);
    }

    return NULL;
}

