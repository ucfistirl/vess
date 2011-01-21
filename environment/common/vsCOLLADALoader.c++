
#include "vsCOLLADALoader.h++"
#include "vsScenePrinter.h++"
#include "atString.h++"
#include "atStringBuffer.h++"
#include "atXMLDocument.h++"
#include "atXMLReader.h++"

// ------------------------------------------------------------------------
// Constructor for the COLLADA loader.  Creates all the object libraries
// and initializes the loader to the default state
// ------------------------------------------------------------------------
vsCOLLADALoader::vsCOLLADALoader()
{
    // Initialize a list that will hold paths to search when looking for
    // a file to load
    pathList = new atList();

    // We haven't parsed a document yet
    mainDocument = NULL;
}

// ------------------------------------------------------------------------
// Denstructor for the COLLADA loader.  Cleans up the object libraries and
// removes any data created during the loading process
// ------------------------------------------------------------------------
vsCOLLADALoader::~vsCOLLADALoader()
{
    // Delete the path list
    delete pathList;

    // Delete the main document
    vsObject::unrefDelete(mainDocument);
}

// ------------------------------------------------------------------------
// Returns the name of this class
// ------------------------------------------------------------------------
const char *vsCOLLADALoader::getClassName()
{
   return "vsCOLLADALoader";
}

// ------------------------------------------------------------------------
// Finds the specified file in our list of search paths
// ------------------------------------------------------------------------
atString vsCOLLADALoader::findFile(const char *filename)
{
    atString fileStr;
    atStringBuffer fullPath;
    atString *path;
    char baseFile[4096];
    const char *ptr;

    // If the filename starts with the URL file protocol ("file://"), skip
    // past that part
    if (strncmp(filename, "file://", 7) == 0)
        ptr = &filename[7];
    else
        ptr = filename;

    // Get the filename as an atString
    fileStr.setString(ptr);

    // Try the file by itself first
    if (access(fileStr.getString(), F_OK) == 0)
        return fileStr;
   
    // Iterate over the paths in the path list, and see if the file is
    // in the search path somewhere
    path = (atString *)pathList->getFirstEntry();
    while (path != NULL)
    {
        // Concatenate the path with the filename
        fullPath.setString(*path);
        fullPath.append("/");
        fullPath.append(fileStr);

        // If the file is there, return it as an atString
        if (access(fullPath.getString(), F_OK) == 0)
            return fullPath.getAsString();

        // Try the next path
        path = (atString *)pathList->getNextEntry();
    }

    // Still didn't find it, try stripping any leading path from the base
    // filename and finding that
    strcpy(baseFile, fileStr.getString());
    ptr = strrchr(baseFile, '/');
    if (ptr == NULL)
    {
        // Try looking for a backslash instead
        ptr = strrchr(baseFile, '\\');
    }

    // If we didn't find a path element separator, we've nothing left to do
    if (ptr == NULL)
        return atString();

    // Advance the pointer past the separator and move the base filename to
    // the beginning of the string to clobber the leading path elements
    ptr++;
    memmove(baseFile, ptr, strlen(ptr)+1);

    // Now try finding the file in the search path again
    return findFile(baseFile);
}

// ------------------------------------------------------------------------
// Adds a path to search when looking for the file to parse
// ------------------------------------------------------------------------
void vsCOLLADALoader::addPath(const char *path)
{
    atString *newPath;

    // Add the new path to the list
    newPath = new atString();
    newPath->setString(path);
    pathList->addEntry(newPath);
}

// ------------------------------------------------------------------------
// Clears the current list of search paths
// ------------------------------------------------------------------------
void vsCOLLADALoader::clearPath()
{
    // Delete and recreate the path list (this will delete any path strings
    // currently in the list)
    delete pathList;
    pathList = new atList();
}

// ------------------------------------------------------------------------
// Initiates parsing of the COLLADA document stored in the specified file
// ------------------------------------------------------------------------
void vsCOLLADALoader::parseFile(const char *filename)
{
    atString path;
    atXMLReader *reader;
    atXMLDocument *doc;
    atXMLDocumentNodePtr rootNode;
    atXMLDocumentNodePtr current;
    char basePath[256];
    char *ptr;
    vsCOLLADADocument *colladaDoc;

    // If we have a main document loaded already, delete it now
    vsObject::unrefDelete(mainDocument);
    mainDocument = NULL;

    // Find the full path to the requested file
    path = findFile(filename);

    // Make sure we found the file
    if (path.getLength() == 0)
    {
        printf("vsCOLLADALoader::parseFile:  Unable to find file %s\n",
            filename);

        return;
    }

    // Store the base path of the file (we'll use this as an extra path to
    // look up images and other supporting files)
    strcpy(basePath, filename);
    ptr = strrchr(basePath, '/');
    if (ptr == NULL)
    {
        // Try looking for a backslash instead
        ptr = strrchr(basePath, '\\');
    }

    // If we didn't find a path element separator, the document must be
    // in the current directory
    if (ptr == NULL)
        documentPath.setString(".");
    else
    {
        // Terminate the base path at the final separator
        *ptr = 0;
        documentPath.setString(basePath);

        // Add the document's own path to the list of paths that we'll
        // search
        addPath(documentPath.getString());
    }

    // Create an XML reader for the given file
    reader = new atXMLReader(path.getString());

    // Get the COLLADA subdocument from the reader
    doc = reader->getSubDocument("COLLADA");

    // If the document is valid, process it
    if (doc == NULL)
    {
        // Clean up the reader
        delete reader;

        // Print an error and bail
        printf("vsCOLLADALoader::parseFile:  File %s not found, or not a "
            "COLLADA file\n", filename);
        return;
    }
    else
    {
        // Create a COLLADA document from the XML document
        mainDocument = new vsCOLLADADocument(doc, pathList);
        mainDocument->ref();
    }

    // We're done with the XML reader and document now
    delete reader;
    delete doc;
}

// ------------------------------------------------------------------------
// Returns a clone of the scene that was created by parsing the <scene> tag
// in the document (if any), minus the scene elements corresponding to the
// character (if any)
// ------------------------------------------------------------------------
vsComponent *vsCOLLADALoader::getScene()
{
    // Return the scene created in the main document
    if (mainDocument != NULL)
        return (vsComponent *)mainDocument->getScene();
    else
        return NULL;
}

// ------------------------------------------------------------------------
// Returns a clone of the character we found in the scene (if any).  For
// now, we assume there is only one character instanced in the scene
// ------------------------------------------------------------------------
vsCharacter *vsCOLLADALoader::getCharacter()
{
    // Return a clone of the main document's character (if we found one)
    if (mainDocument != NULL)
        return mainDocument->getCharacter();
    else
        return NULL;
}

