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
//                  just specifies all the other Cal3D files needed.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#include "vsCal3DLoader.h++"

#include <string.h>
#include <stdio.h>

//------------------------------------------------------------------------
// Constructor.
//------------------------------------------------------------------------
vsCal3DLoader::vsCal3DLoader()
{
    // Create the loaders.
    meshLoader = new vsCal3DMeshLoader();
    boneLoader = new vsCal3DBoneLoader();

    // Create and initialize the mesh vsGrowableArray to empty.
    meshList = new vsGrowableArray(10, 1);
    meshCount = 0;

    // Create and initialize the animation vsGrowableArray to empty.
    animationList = new vsGrowableArray(10, 1);
    animationCount = 0;

    // Initialize the skeletonFilename to none.
    skeletonFilename = NULL;
}

//------------------------------------------------------------------------
// Destructor.
//------------------------------------------------------------------------
vsCal3DLoader::~vsCal3DLoader()
{
    int index;

    // Delete the loaders.
    delete meshLoader;
    delete boneLoader;

    // Delete all the entires in the mesh list.
    for (index = 0; index < meshCount; index++)
    {
        delete [] ((char *) meshList->getData(index));
    }

    // Delete the list.
    delete meshList;

    // Delete all the entires in the animation list.
    for (index = 0; index < animationCount; index++)
    {
        delete [] ((char *) animationList->getData(index));
    }

    // Delete the list.
    delete animationList;

    // If we have a name allocated, delete it.
    if (skeletonFilename)
        delete [] skeletonFilename;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsCal3DLoader::getClassName()
{
    return "vsCal3DLoader";
}


//------------------------------------------------------------------------
// Parse the .cfg file generated by the Cal3D exporter for 3DStudio.
// It conveniently specifies the many material files in order as well
// as a skeleton, meshes, and a scale value.
//------------------------------------------------------------------------
void vsCal3DLoader::parseFile(char *filename)
{
    FILE *filePointer;
    int fileSize;
    int maxRead;
    int fileLineLength;
    char *fileLine;
    char *fields;
    char *fieldValue;
    char *delimiter = " =\r";
    char *meshFilename;
    char *animationFilename;
    double scaleValue;
    int index;

    // Open the file, print error and return if error.
    if ((filePointer = fopen(filename, "r")) == NULL)
    {
        printf("vsCal3DLoader::parseFile: Error opening file!\n");
        return;
    }

    // Clear the loaded materials, each call should get its own list.
    meshLoader->clearMaterials();

    // Clear the mesh filename list.
    for (index = 0; index < meshCount; index++)
    {
        delete [] ((char *) meshList->getData(index));
    }
    meshCount = 0;

    // Reinitialize the scale matrix to identity, in case no scale is found.
    scaleMatrix.setIdentity();

    // Seek to the end of the file so we can determine its size, and reset
    // to the beginning of the file.
    fseek(filePointer, 0L, SEEK_END);
    fileSize = ftell(filePointer);
    fseek(filePointer, 0L, SEEK_SET);

    // Allocate a token and line buffer, to the maximum possible line size.
    fileLine = new char[fileSize];
    maxRead = fileSize;

    // While there are still lines to read, process them.
    // This pass looks for everything but meshes.
    while (fgets(fileLine, maxRead, filePointer))
    {
        // Get the length of the line.
        fileLineLength = strlen(fileLine);

        // Reset pointers to point to the beginning of their memory space.
        fields = fileLine;

        // Read the first possible token.
        fieldValue = strtok(fields, delimiter);

        // If the line begins with #, it is a comments, so ignore.
        // If the size is less than 3, then it is impossible to be a useful
        // line, so ignore.
        if ((fields[0] != '#') && (fileLineLength > 2))
        {
            // If it is a scale field, parse it.
            if (strcmp(fieldValue, "scale") == 0)
            {
                if (fieldValue = strtok(NULL, delimiter))
                {
                    // Convert to a double and make a scale matrix.
                    scaleValue = atof(fieldValue);
                    scaleMatrix.setScale(scaleValue, scaleValue, scaleValue);
                }
            }
            // If it is a skeleton field, parse it.
            else if (strcmp(fieldValue, "skeleton") == 0)
            {
                if (fieldValue = strtok(NULL, delimiter))
                {
                    // Delete any previous filenames we may have.
                    if (skeletonFilename)
                        delete [] skeletonFilename;

                    // Create new data space, and copy the filename into it.
                    skeletonFilename = new char[strlen(fieldValue) + 1];
                    strcpy(skeletonFilename, fieldValue);
                }
            }
            // If it is a material field, parse it.
            else if (strcmp(fieldValue, "material") == 0)
            {
                if (fieldValue = strtok(NULL, delimiter))
                {
                    // Load the material.
                    meshLoader->loadMaterial(fieldValue);
                }
            }
            // If it is a mesh field, parse it.
            else if (strcmp(fieldValue, "mesh") == 0)
            {
                if (fieldValue = strtok(NULL, delimiter))
                {
                    // Create new data space and copy the mesh name into it.
                    meshFilename = new char[strlen(fieldValue) + 1];
                    strcpy(meshFilename, fieldValue);

                    // Add the new string into the mesh list.
                    meshList->setData(meshCount, meshFilename);
                    meshCount++;
                }
            }
            // If it is a animation field, parse it.
            else if (strcmp(fieldValue, "animation") == 0)
            {
                if (fieldValue = strtok(NULL, delimiter))
                {
                    // Create new data space and copy the animation name to it.
                    animationFilename = new char[strlen(fieldValue) + 1];
                    strcpy(animationFilename, fieldValue);

                    // Add the new string into the animation list.
                    animationList->setData(animationCount, animationFilename);
                    animationCount++;
                }
            }
        }

        // Keep track of how much we have read in relation to the file size
        // so we do not attempt to read more than possible.
        maxRead -= fileLineLength;
    }

    // Close the file and free the buffers.
    fclose(filePointer);
    delete [] fileLine;
}

//------------------------------------------------------------------------
// Return a new vsComponent which is a root to the meshes that were loaded
// based on what we have currently read from the file.
//------------------------------------------------------------------------
vsComponent *vsCal3DLoader::getNewMesh()
{
    vsComponent *rootMeshComponent;
    int index;

    rootMeshComponent = NULL;

    // If we have any meshes to load, create the root component.
    if (meshCount > 0)
    {
        rootMeshComponent = new vsComponent();

        // Load each of the meshes we have in the list.
        for (index = 0; index < meshCount; index++)
        {
            // Load the mesh and make it a child of rootMeshComponent.
            meshLoader->loadMesh(((char *) meshList->getData(index)),
                rootMeshComponent);
        }
    }

    return rootMeshComponent;
}

//------------------------------------------------------------------------
// Return a new vsSkeleton based on what we have currently read from the file.
//------------------------------------------------------------------------
vsSkeleton *vsCal3DLoader::getNewSkeleton()
{
    vsSkeleton *skeleton;

    skeleton = NULL;

    // If we have a skeleton file defined, load it.
    if (skeletonFilename)
    {
        skeleton = boneLoader->loadSkeleton(skeletonFilename);

        // Apply whatever scale matrix we have to it.
        skeleton->setOffsetMatrix(scaleMatrix);
    }

    return skeleton;
}

//------------------------------------------------------------------------
// Return a new vsPathMotionManager with all vsPathMotions for the specified
// animation file.
//------------------------------------------------------------------------
vsPathMotionManager *vsCal3DLoader::getNewAnimation(char *name,
                                                    vsSkeletonKinematics
                                                    *skeletonKinematics)
{
    vsPathMotionManager *animation;
    char *filename;
    char *animationName;
    int index;
    int loop;
    bool found;

    animation = NULL;

    // Search for the specified animation.
    found = false;
    for (index = 0; (index < animationCount) && (!found); index++)
    {
        filename = (char *) animationList->getData(index);

        // Get the name without any of the path information, so we can
        // compare it to the given animation name.
        // Cal3D does not specify an animation name in the XML, so making
        // use of the filename, without extension and path.
        loop = (strlen(filename) - 1);
        while ((loop != -1) && (filename[loop] != '/'))
            loop--;

        animationName = &filename[loop+1];

        // If found load and set as return value.
        if (strncmp(animationName, name, sizeof(name)) == 0)
        {
            found = true;
            animation = animationLoader->loadAnimation(filename,
                skeletonKinematics);
        }
    }

    return animation;
}
