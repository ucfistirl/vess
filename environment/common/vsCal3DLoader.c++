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
//    VESS Module:  vsCal3DLoader.c++
//
//    Description:  Loader for the .cfg files generated by the Cal3D
//                  exporter for 3DStudio.  A very simple file which
//                  just specifies all the other Cal3D files needed
//
//    Author(s):    Duvan Cope, Jason Daly
//
//------------------------------------------------------------------------

#include "vsCal3DLoader.h++"

#include <string.h>
#include <stdio.h>

//------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------
vsCal3DLoader::vsCal3DLoader()
{
    // Create the three sub-loaders for meshes, bones, and animations
    meshLoader = new vsCal3DMeshLoader();
    boneLoader = new vsCal3DBoneLoader();
    animationLoader = new vsCal3DAnimationLoader();

    // Create a list to hold the file search paths
    directoryList = new atList();
}

//------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------
vsCal3DLoader::~vsCal3DLoader()
{
    // Clear out the directory listing
    delete directoryList;

    // Delete the sub-loaders
    delete meshLoader;
    delete boneLoader;
    delete animationLoader;
}

// ------------------------------------------------------------------------
// Given a filename (without prepended directory), this function will find
// return a filename that exists with a prepended directory that has been
// added to the DirectoryNode listing. If there is no file in the
// listed directories, this function will return NULL. This is a private
// helper function
// ------------------------------------------------------------------------
char *vsCal3DLoader::findFile(char *filename)
{
   atString *path;
   char *absoluteFilename;
   char tempString[500];
   
   // Loop through the list of directories
   path = (atString *)directoryList->getFirstEntry();
   while(path != NULL)
   {
      // Create the tempString
      strcpy(tempString, path->getString());
      strcat(tempString, "/");
      strcat(tempString, filename);
      
      // See if this file can be read by this process. 
      if(access(tempString, R_OK) == 0)
      {
         // Make the absoluteFilename string
         absoluteFilename = 
            (char*)(calloc(strlen(tempString)+2, sizeof(char)));
            
         strcpy(absoluteFilename, tempString);
         
         // Return it
         return absoluteFilename;
      }
      
      // Try the next directory
      path = (atString *)directoryList->getNextEntry();
   }
   
   // We didn't find the file, so just return the original string
   return filename;
}
      

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsCal3DLoader::getClassName()
{
    return "vsCal3DLoader";
}

// ------------------------------------------------------------------------
// Adds a directory listing to the list that we should search for files in
// ------------------------------------------------------------------------
void vsCal3DLoader::addFilePath(const char *dirName)
{
   atString *newPath;
   
   // Create the node and copy the directory name
   newPath = new atString();
   newPath->setString(dirName);

   // Add the new path to the list
   directoryList->addEntry(newPath);
   
   // Tell the sub-loaders to add this directory to their listings
   meshLoader->addFilePath(dirName);
   boneLoader->addFilePath(dirName);
   animationLoader->addFilePath(dirName);
}

//------------------------------------------------------------------------
// Parse the .cfg file generated by the Cal3D exporter for 3ds MAX.
// It conveniently specifies the many material files in order as well
// as a skeleton, meshes, and a scale value.  Return the resulting data
// in a vsCharacter object
//------------------------------------------------------------------------
vsCharacter *vsCal3DLoader::loadCharacter(char *filename)
{
    FILE *filePointer;
    int fileSize;
    int fileLineLength;
    char *fileLine;
    char *fields;
    char *fieldValue;
    char *delimiter = " =\r\n";
    double scaleValue;
    vsSkeleton *skeleton;
    atArray *boneMatrices;
    vsSkeletonKinematics *skeletonKin;
    atString *subMeshFile;
    atList *subMeshes;
    vsArray *animations;
    atArray *animationNames;
    int loop;
    vsPathMotionManager *animation;
    char tempName[512];
    atString *animationName;
    vsComponent *mesh;
    vsComponent *subMesh;
    vsSkin *skin;
    vsCharacter *character;
    
    // Change the filename to a string with the directory appended
    filename = findFile(filename);
    
    // Open the file, print error and return if error
    if ((filePointer = fopen(filename, "r")) == NULL)
    {
        printf("vsCal3DLoader::parseFile: Error opening file!\n");
        return NULL;
    }

    // Clear the loaded materials, each call should get its own list
    meshLoader->clearMaterials();

    // Create the mesh list
    subMeshes = new atList();
    
    // Create the animation arrays
    animations = new vsArray();
    animationNames = new atArray();

    // Reinitialize the scale matrix to identity, in case no scale is found
    scaleMatrix.setIdentity();

    // Seek to the end of the file so we can determine its size, and reset
    // to the beginning of the file
    fseek(filePointer, 0L, SEEK_END);
    fileSize = ftell(filePointer);
    fseek(filePointer, 0L, SEEK_SET);

    // Allocate a token and line buffer, to the maximum possible line size
    fileLine = new char[fileSize];

    // While there are still lines to read, process them.
    while (fgets(fileLine, fileSize, filePointer))
    {
        // Get the length of the line
        fileLineLength = strlen(fileLine);

        // Reset pointers to point to the beginning of their memory space
        fields = fileLine;

        // Read the first possible token
        fieldValue = strtok(fields, delimiter);

        // If the line begins with #, it is a comments, so ignore.
        // If the size is less than 3, then it is impossible to be a useful
        // line, so ignore
        if ((fields[0] != '#') && (fileLineLength > 2))
        {
            // If it is a scale field, parse it
            if (strcmp(fieldValue, "scale") == 0)
            {
                if (fieldValue = strtok(NULL, delimiter))
                {
                    // Convert to a double and make a scale matrix
                    scaleValue = atof(fieldValue);
                    scaleMatrix.setScale(scaleValue, scaleValue, scaleValue);
                }
            }
            // If it is a skeleton field, parse it
            else if (strcmp(fieldValue, "skeleton") == 0)
            {
                if (fieldValue = strtok(NULL, delimiter))
                {
                    // Try to load the skeleton
                    skeleton = boneLoader->loadSkeleton(fieldValue);

                    // See if we got a valid skeleton
                    if (skeleton != NULL)
                    {
                        // Grab the bone space matrices from the bone loader
                        boneMatrices = boneLoader->getBoneSpaceMatrixList();

                        // Create a skeleton kinematics
                        skeletonKin = new vsSkeletonKinematics(skeleton);
                    }
                    else
                    {
                        // Set the matrix list and kinematics to NULL
                        boneMatrices = NULL;
                        skeletonKin = NULL;
                    }
                }
            }
            // If it is a material field, parse it
            else if (strcmp(fieldValue, "material") == 0)
            {
                if (fieldValue = strtok(NULL, delimiter))
                {
                    // Load the material, the mesh loader will use it when
                    // needed
                    meshLoader->loadMaterial(fieldValue);
                }
            }
            // If it is a mesh field, parse it
            else if (strcmp(fieldValue, "mesh") == 0)
            {
                if (fieldValue = strtok(NULL, delimiter))
                {
                    // Grab the mesh filename and put it in a list
                    subMeshFile = new atString(fieldValue);
                    subMeshes->addEntry(subMeshFile);
                }
            }
            // If it is a animation field, parse it
            else if (strcmp(fieldValue, "animation") == 0)
            {
                if (fieldValue = strtok(NULL, delimiter))
                {
                    // Try to load the animation
                    animation = animationLoader->
                        loadAnimation(fieldValue, skeletonKin);

                    // If the animation loaded successfully, create a name
                    // for it, based on the filename
                    if (animation != NULL)
                    {
                        // Add the animation to our animation array
                        animations->addEntry(animation);

                        // Copy over the animation name for comparison
                        strncpy(tempName, fieldValue, 512);
        
                        // Get the name without any of the path information,
                        // so we can compare it to the given animation name
                        // Cal3D does not specify an animation name in the XML,
                        // so we make use of the filename, without extension
                        // and path
                        loop = (strlen(tempName) - 1);
                        while ((loop != -1) && (tempName[loop] != '/'))
                            loop--;
              
                        // Cut the last four letters off of the animationName
                        // (the file extension, and the period before it)
                        tempName[strlen(tempName)-4] = '\0';

                        // Create the name string and add it to the array
                        animationName = new atString(&(tempName[loop+1]));
                        animationNames->addEntry(animationName);
                    }
                }
            }
        }
    }

    // Close the file and free the buffers
    fclose(filePointer);
    delete [] fileLine;

    // Set the offset matrix on the skeleton to take the scale factor into
    // account
    skeleton->setOffsetMatrix(scaleMatrix);

    // Load the various submeshes we found under a single root
    // component
    mesh = new vsComponent();
    subMeshFile = (atString *)subMeshes->getFirstEntry();
    while (subMeshFile != NULL)
    {
        // Add the submesh to the root mesh
        subMesh = meshLoader->loadMesh(subMeshFile->getString());
        mesh->addChild(subMesh);

        // Get the next submesh
        subMeshFile = (atString *)subMeshes->getNextEntry();
    }

    // Using the skeleton, bone space matrices, and mesh, create a skin for
    // the character (if the skeleton and/or bone matrices are NULL, the
    // skin will handle this)
    skin = new vsSkin(mesh, skeleton, boneMatrices);

    // Now, create the character using the skeleton, skin, and animations
    character = new vsCharacter(skeleton, skeletonKin, skin,
        animationNames, animations);

    // Return the character that we created
    return character;
}

