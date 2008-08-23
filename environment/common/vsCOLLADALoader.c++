
#include "vsCOLLADALoader.h++"
#include "vsCOLLADAChannelGroup.h++"
#include "vsCOLLADADataSource.h++"
#include "vsCOLLADAAnimation.h++"
#include "vsCOLLADAGeometry.h++"
#include "vsCOLLADANode.h++"
#include "vsCOLLADASkin.h++"
#include "vsCOLLADATransform.h++"
#include "vsGeometry.h++"
#include "vsDynamicGeometry.h++"
#include "vsSkin.h++"
#include "vsSkeletonMeshGeometry.h++"
#include "vsLODAttribute.h++"
#include "vsLightAttribute.h++"
#include "vsTextureAttribute.h++"
#include "vsTransparencyAttribute.h++"
#include "atStringBuffer.h++"
#include "atXMLReader.h++"

#include "vsScenePrinter.h++"

// ------------------------------------------------------------------------
// Constructor for the COLLADA loader.  Creates all the object libraries
// and initializes the loader to the default state
// ------------------------------------------------------------------------
vsCOLLADALoader::vsCOLLADALoader()
{
    // Initialize a list that will hold paths to search when looking for
    // a file to load
    pathList = new atList();

    // Initialize scene pointer and character to NULL, since we haven't
    // loaded anything yet
    sceneRoot = NULL;
    sceneCharacter= NULL;

    // Create lists for the skeletons and skins
    skeletonList = new atList();
    skinList = new atList();

    // Create all library maps
    animationLibrary = new atMap();
    animationClipLibrary = NULL;
    cameraLibrary = NULL;
    controllerLibrary = new atMap();
    effectLibrary = new atMap();
    geometryLibrary = new atMap();
    imageLibrary = new atMap();
    lightLibrary = new atMap();
    materialLibrary = new atMap();
    nodeLibrary = new atMap();
    visualSceneLibrary = new atMap();

    // Create a map to store the root node of each skeleton in the scene
    skeletonRoots = new atMap();
    parsingSkeleton = false;

    // Create a map to store the vsSkeleton objects created
    skeletons = new atMap();

    // Create a map to store the vsPathMotionManager objects (animations)
    // created
    animations = new atMap();

    // Default the units scalar to 1.0
    unitScale = 1.0;

    // Initialize the counter for the unnamed ID generator
    unnamedCount = 0;
}

// ------------------------------------------------------------------------
// Denstructor for the COLLADA loader.  Cleans up the object libraries and
// removes any data created during the loading process
// ------------------------------------------------------------------------
vsCOLLADALoader::~vsCOLLADALoader()
{
    // Delete the scene's character (if any)
    if (sceneCharacter != NULL)
        vsObject::unrefDelete(sceneCharacter);

    // Unreference/delete the scene we created (if any)
    if (sceneRoot != NULL)
        vsObject::unrefDelete(sceneRoot);

    // Clean up the loader's lists
    unrefDeleteList(skinList);
    unrefDeleteList(skeletonList);

    // Clean up the auxiliary maps
    unrefDeleteMap(animations);
    unrefDeleteMap(skeletonRoots);
    unrefDeleteMap(skeletons);

    // Clean up the library maps
    unrefDeleteMap(animationLibrary);
    unrefDeleteMap(animationClipLibrary);
    unrefDeleteMap(visualSceneLibrary);
    unrefDeleteMap(nodeLibrary);
    unrefDeleteMap(controllerLibrary);
    unrefDeleteMap(geometryLibrary);
    unrefDeleteMap(materialLibrary);
    unrefDeleteMap(effectLibrary);
    unrefDeleteMap(imageLibrary);
    unrefDeleteMap(lightLibrary);
    unrefDeleteMap(cameraLibrary);

    // Delete the path list
    delete pathList;
}

// ------------------------------------------------------------------------
// Returns the name of this class
// ------------------------------------------------------------------------
const char *vsCOLLADALoader::getClassName()
{
   return "vsCOLLADALoader";
}

// ------------------------------------------------------------------------
// Often, there will be unnamed or unidentified objects in the file.  This
// method simply makes up a name/ID for them that is unique for this loader
// session, so the objects can be placed in the appropriate library map
// ------------------------------------------------------------------------
atString vsCOLLADALoader::getUnnamedName()
{
    char name[64];

    // Create a unique name
    sprintf(name, "unnamed-%lu", unnamedCount);

    // Increment the unnamed item count
    unnamedCount++;

    // Return a string with the name we created
    return atString(name);
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
// Returns the skeleton root node specified by the given identifier
// ------------------------------------------------------------------------
vsCOLLADANode *vsCOLLADALoader::getSkeletonRoot(atString id)
{
    char *idStr;
    atString newID;
    vsCOLLADANode *node;

    // Check the ID string to see what kind of URI this is
    idStr = id.getString();
    if (idStr[0] == '#')
    {
        // This is a URI fragment, meaning the root node is local to this
        // file.  We should already have the source in our data source map,
        // so we should only need to strip the leading '#' and look up the
        // ID
        newID.setString(&idStr[1]);

        // Look in the skeletonRoots map for the node and return it if
        // we find it
        node = (vsCOLLADANode *)skeletonRoots->getValue(&newID);
        if (node)
            return node;
        else
            return NULL;
    }
    else
    {
        // Other URI forms aren't currently supported
        return NULL;
    }
}

// ------------------------------------------------------------------------
// Returns whether or not the given component is the root of a skeleton
// ------------------------------------------------------------------------
bool vsCOLLADALoader::isSkeletonRoot(vsComponent *node)
{
    vsCOLLADANode *colladaNode;
    atString nodeID;
    char testID[256];
    vsCOLLADANode *skeletonRoot;

    // First, see if this component is a vsCOLLADANode
    colladaNode = dynamic_cast<vsCOLLADANode *>(node);

    // Now, check to see if this node is a skeleton root.  To do this, we
    // put a "#" in front of the node's ID and look it up in the skeleton
    // root's map
    nodeID = colladaNode->getID();
    sprintf(testID, "#%s", nodeID.getString());
    skeletonRoot = getSkeletonRoot(atString(testID));

    // If we get a valid skeleton root back, this node must be a skeleton
    // root
    if (skeletonRoot != NULL)
        return true;
    else
        return false;
}

// ------------------------------------------------------------------------
// Returns the vsSkeleton object associated with the given skeleton root
// identifier
// ------------------------------------------------------------------------
vsSkeleton *vsCOLLADALoader::getSkeleton(atString id)
{
    char *idStr;
    atString newID;

    // Check the ID string to see what kind of URI this is
    idStr = id.getString();
    if (idStr[0] == '#')
    {
        // This is a URI fragment, meaning the root node is local to this
        // file.  We should already have the source in our data source map,
        // so we should only need to strip the leading '#' and look up the
        // ID
        newID.setString(&idStr[1]);

        // Look in the skeletons map for the node and return it
        return (vsSkeleton *)skeletons->getValue(&newID);
    }
    else
    {
        // Other URI forms aren't currently supported
        return NULL;
    }
}

// ------------------------------------------------------------------------
// Returns a COLLADA effect object specified by the given identifier
// ------------------------------------------------------------------------
vsCOLLADAEffect *vsCOLLADALoader::getEffect(atString id)
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

        // Look in the geometry library for the geometry and return it
        return (vsCOLLADAEffect *)effectLibrary->getValue(&newID);
    }
    else
    {
        // Other URI forms aren't currently supported
        return NULL;
    }
}

// ------------------------------------------------------------------------
// Returns a COLLADA effect object specified by the given identifier
// ------------------------------------------------------------------------
vsCOLLADAEffect *vsCOLLADALoader::getMaterial(atString id)
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

        // Look in the geometry library for the geometry and return it
        return (vsCOLLADAEffect *)materialLibrary->getValue(&newID);
    }
    else
    {
        // Other URI forms aren't currently supported
        return NULL;
    }
}

// ------------------------------------------------------------------------
// Returns a controller object specified by the given identifier
// ------------------------------------------------------------------------
vsCOLLADAController *vsCOLLADALoader::getController(atString id)
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

        // Look in the geometry library for the geometry and return it
        return (vsCOLLADAController *)controllerLibrary->getValue(&newID);
    }
    else
    {
        // Other URI forms aren't currently supported
        return NULL;
    }
}

// ------------------------------------------------------------------------
// Returns a geometry object specified by the given identifier
// ------------------------------------------------------------------------
vsCOLLADAGeometry *vsCOLLADALoader::getGeometry(atString id)
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

        // Look in the geometry library for the geometry and return it
        return (vsCOLLADAGeometry *)geometryLibrary->getValue(&newID);
    }
    else
    {
        // Other URI forms aren't currently supported
        return NULL;
    }
}

// ------------------------------------------------------------------------
// Returns a visual scene object specified by the given identifier
// ------------------------------------------------------------------------
vsCOLLADANode *vsCOLLADALoader::getVisualScene(atString id)
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

        // Look in the visual scene library for the scene and return it
        return (vsCOLLADANode *)visualSceneLibrary->getValue(&newID);
    }
    else
    {
        // Other URI forms aren't currently supported
        return NULL;
    }
}

// ------------------------------------------------------------------------
// Simple convenience method.  Retrieves the next token from the given
// string tokenizer and converts it to an integer before returning it
// ------------------------------------------------------------------------
int vsCOLLADALoader::getIntToken(atStringTokenizer *tokens)
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
// Simple convenience method.  Retrieves the next token from the given
// string tokenizer and converts it to an floating point number before
// returning it
// ------------------------------------------------------------------------
double vsCOLLADALoader::getFloatToken(atStringTokenizer *tokens)
{
    atString *tempStr;
    double value;

    // Get the next token from the tokenizer
    tempStr = tokens->getToken(" \n\r\t");

    // Convert the token to a floating point value
    value = atof(tempStr->getString());

    // Get rid of the temporary string
    delete tempStr;

    // Return the floating point value
    return value;
}

// ------------------------------------------------------------------------
// Parses a atVector of the given size from the given XML text node
// ------------------------------------------------------------------------
atVector vsCOLLADALoader::parseVector(atXMLDocument *doc,
                                      atXMLDocumentNodePtr text, int size)
{
    atVector vec;
    atString textStr;
    atStringTokenizer *tokens;
    int i;

    // Set the vector size
    vec.setSize(size);

    // Create a string and tokenizer from the text
    textStr.setString(doc->getNodeText(text));
    tokens = new atStringTokenizer(textStr);

    // Get the components of the vector
    for (i = 0; i < size; i++)
    {
        vec.setValue(i, getFloatToken(tokens));
    }

    // Return the vector
    return vec;
}

// ------------------------------------------------------------------------
// Convenience method for cleaning up library lists that maintain pointers
// to reference-counted objects
// ------------------------------------------------------------------------
void vsCOLLADALoader::unrefDeleteList(atList *list)
{
    vsObject *object;

    // Make sure the list exists
    if (list == NULL)
        return;

    // Unreference and delete any objects in the map
    object = (vsObject *)list->getFirstEntry();
    while (object != NULL)
    {
        // Remove the current entry from the list
        list->removeCurrentEntry();

        // Unreference and delete the object
        vsObject::unrefDelete(object);

        // Get the next object from the list
        object = (vsObject *)list->getNextEntry();
    }

    // Delete the list
    delete list;
}

// ------------------------------------------------------------------------
// Convenience method for cleaning up library maps that maintain pointers
// to reference-counted values
// ------------------------------------------------------------------------
void vsCOLLADALoader::unrefDeleteMap(atMap *map)
{
    atList *keys;
    atList *values;
    atString *key;
    vsObject *value;

    // Make sure the map exists
    if (map == NULL)
        return;

    // Get lists of the keys and values in the map
    keys = new atList();
    values = new atList();
    map->getSortedList(keys, values);

    // Unreference and delete any objects in the map
    key = (atString *)keys->getFirstEntry();
    value = (vsObject *)values->getFirstEntry();
    while (key != NULL)
    {
        // Remove the current entry from both lists
        keys->removeCurrentEntry();
        values->removeCurrentEntry();

        // Remove the entry from the map
        map->removeEntry(key);

        // Unreference and delete the object
        vsObject::unrefDelete(value);

        // Delete the key
        delete key;

        // Get the next key and value from the list
        key = (atString *)keys->getNextEntry();
        value = (vsObject *)values->getNextEntry();
    }

    // Delete the two lists and the map
    delete keys;
    delete values;
    delete map;
}

// ------------------------------------------------------------------------
// Processes an <animation> XML subtree and creates an animation library
// object
// ------------------------------------------------------------------------
void vsCOLLADALoader::processAnimation(atXMLDocument *doc,
                                       atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr child;
    char *attr;
    atString *id;
    vsCOLLADAAnimation *anim;

    // Get the animation's ID
    attr = doc->getNodeAttribute(current, "id");
    if (attr == NULL)
        id = new atString(getUnnamedName());
    else
        id = new atString(attr);

    // Process the animation
    anim = new vsCOLLADAAnimation(*id, doc, current);

    // Reference the animation, and add it to the library
    anim->ref();
    animationLibrary->addEntry(id, anim);
}

// ------------------------------------------------------------------------
// Processes the <library_animations> XML subtree and creates the
// animation library and all the objects it contains
// ------------------------------------------------------------------------
void vsCOLLADALoader::processLibraryAnimations(atXMLDocument *doc,
                                               atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr child;

    // Iterate over the controller nodes in the subdocument
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // If this is an animation, process it
        if (strcmp(doc->getNodeName(child), "animation") == 0)
            processAnimation(doc, child);

        // Move on to the next node
        child = doc->getNextSiblingNode(child);
    }
}

// ------------------------------------------------------------------------
// Processes an <animation_clip> XML subtree and creates an animation clip
// ------------------------------------------------------------------------
void vsCOLLADALoader::processLibraryAnimationClips(atXMLDocument *doc,
                                                   atXMLDocumentNodePtr current)
{
}

// ------------------------------------------------------------------------
// Processes the <library_animation_clips> XML subtree and creates the
// animation clip library and all the objects it contains
// ------------------------------------------------------------------------
void vsCOLLADALoader::processLibraryCameras(atXMLDocument *doc,
                                            atXMLDocumentNodePtr current)
{
}

// ------------------------------------------------------------------------
// Processes a <controller> XML subtree and creates a controller library
// object.  Controllers are objects that can manipulate vertices and
// include skinning and morphing objects
// ------------------------------------------------------------------------
void vsCOLLADALoader::processController(atXMLDocument *doc,
                                        atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr child;
    char *attr;
    atString *id;
    vsCOLLADAGeometry *geom;
    vsCOLLADASkin *skin;

    // Get the controller's ID
    attr = doc->getNodeAttribute(current, "id");
    if (attr == NULL)
        id = new atString(getUnnamedName());
    else
        id = new atString(attr);

    // Look for a controller under this node
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See if we recognize this type of controller
        if (strcmp(doc->getNodeName(child), "skin") == 0)
        {
            // Fetch the source geometry from the geometry library
            attr = doc->getNodeAttribute(child, "source");
            if (attr != NULL)
                geom = getGeometry(atString(attr));

            // Make sure we got a source geometry (otherwise we have
            // nothing to skin)
            if (geom != NULL)
            {
                // Process the skin
                skin = new vsCOLLADASkin(doc, child, geom);

                // Add the skin to the controller library
                skin->ref();
                controllerLibrary->addEntry(id, skin);
            }
        }

        // Try the next node
        child = doc->getNextSiblingNode(child);
    }
}

// ------------------------------------------------------------------------
// Processes the <library_controllers> XML subtree and creates the
// controller library and all the objects it contains
// ------------------------------------------------------------------------
void vsCOLLADALoader::processLibraryControllers(atXMLDocument *doc,
                                                atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr child;

    // Iterate over the controller nodes in the subdocument
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // If this is a controller, process it
        if (strcmp(doc->getNodeName(child), "controller") == 0)
            processController(doc, child);

        // Move on to the next node
        child = doc->getNextSiblingNode(child);
    }
}

// ------------------------------------------------------------------------
// Creates a new effect parameter that will be used upon instantiation of
// the effect in a material
// ------------------------------------------------------------------------
vsCOLLADAEffectParameter *vsCOLLADALoader::processNewParam(
                                                 atXMLDocument *doc,
                                                 atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr child;
    atXMLDocumentNodePtr surfaceNode;
    atXMLDocumentNodePtr tempNode;
    atXMLDocumentNodePtr samplerNode;
    char sid[256];
    char *text;
    vsCOLLADAEffectParameter *param;
    atStringTokenizer *tokens;
    atVector values;
    vsTextureAttribute *libTexture;
    vsTextureAttribute *paramTexture;
    atString imageID;
    char name[256];

    // Start with no parameter
    param = NULL;

    // Get the parameter's name (sid stands for Scoped IDentifier, meaning
    // this identifier is only valid within this effect)
    strcpy(sid, doc->getNodeAttribute(current, "sid"));

    // Iterate over and process the parameter child elements
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        if (strcmp(doc->getNodeName(child), "float") == 0)
        {
           // Create the parameter
           param = new vsCOLLADAEffectParameter(atString(sid),
                                                VS_COLLADA_FLOAT);

           // Set the value
           text = doc->getNodeText(doc->getNextChildNode(child));
           if (text != NULL)
               param->set(atof(text));
           else
               param->set(0.0);
        }
        else if (strcmp(doc->getNodeName(child), "float2") == 0)
        {
           // Create the parameter
           param = new vsCOLLADAEffectParameter(atString(sid),
                                                VS_COLLADA_FLOAT2);

           // Set the values
           text = doc->getNodeText(doc->getNextChildNode(child));
           if (text != NULL)
           {
               // Parse the float values into a vector
               values = parseVector(doc, child, 2);

               // Set the values on the parameter
               param->set(values[0], values[1]);
           }
           else
               param->set(0.0, 0.0);
        }
        else if (strcmp(doc->getNodeName(child), "float3") == 0)
        {
           // Create the parameter
           param = new vsCOLLADAEffectParameter(atString(sid),
                                                VS_COLLADA_FLOAT3);

           // Set the values
           text = doc->getNodeText(doc->getNextChildNode(child));
           if (text != NULL)
           {
               // Parse the float values into a vector
               values = parseVector(doc, child, 3);

               // Set the values on the parameter
               param->set(values[0], values[1], values[2]);
           }
           else
               param->set(0.0, 0.0, 0.0);
        }
        else if (strcmp(doc->getNodeName(child), "surface") == 0)
        {
            // Create a surface parameter
            param = new vsCOLLADAEffectParameter(atString(sid),
                VS_COLLADA_SURFACE);

            // Process the surface settings
            param->set(doc, child);

            // Look up the surface's image in the image library and
            // set the parameter's texture value (if any)
            imageID = param->getSourceImageID();
            libTexture = (vsTextureAttribute *)imageLibrary->getValue(&imageID);
            if (libTexture != NULL)
            {
                paramTexture = (vsTextureAttribute *)libTexture->clone();
                param->set(paramTexture);
            }
         }
         else if (strcmp(doc->getNodeName(child), "sampler2D") == 0)
         {
            // Create a sampler parameter
            param = new vsCOLLADAEffectParameter(sid, VS_COLLADA_TEXTURE_2D);
 
            // Process the sampler settings
            param->set(doc, child);
        }

        // Try the next node
        child = doc->getNextSiblingNode(child);
    }

    // Return the parameter we created (if any)
    return param;
}

// ------------------------------------------------------------------------
// Processes the material attributes for a profile_COMMON effect
// ------------------------------------------------------------------------
void vsCOLLADALoader::processMaterialAttributes(atXMLDocument *doc,
                                                atXMLDocumentNodePtr current,
                                                vsCOLLADAFixedEffect *effect)
{
    atXMLDocumentNodePtr child;
    atXMLDocumentNodePtr colorNode;
    char *text;
    atString params;
    atVector color;
    double value;
    bool rgbZeroOpaque;
    vsMaterialAttribute *mat;
    vsTextureAttribute *tex;
    atString texcoord;

    // Create the material attribute (we'll always need one of these)
    mat = new vsMaterialAttribute();

    // The implied color mode is NONE, that is, no material colors are to
    // be overridden by the colors on the geometry
    mat->setColorMode(VS_MATERIAL_SIDE_BOTH, VS_MATERIAL_CMODE_NONE);

    // Set the effect's material
    effect->setMaterial(mat);

    // Iterate over and process the material parameters
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // Process this attribute
        if (strcmp(doc->getNodeName(child), "emission") == 0)
        {
            // Get the color settings
            colorNode = doc->getNextChildNode(child);
            while (colorNode != NULL)
            {
                if (strcmp(doc->getNodeName(colorNode), "color") == 0)
                {
                    // Parse the color
                    color = 
                        parseVector(doc, doc->getNextChildNode(colorNode), 4);

                    // Set the constant emissive color of the material
                    mat->setColor(VS_MATERIAL_SIDE_BOTH, 
                        VS_MATERIAL_COLOR_EMISSIVE,
                        color[0], color[1], color[2]);
                }
                else if (strcmp(doc->getNodeName(colorNode), "param") == 0)
                {
                    // JPD:  Parametric material attributes not yet
                    //       implemented

                    // Get the parameter name attribute

                    // Look up the parameter

                    // Tell the effect that the emissive color comes from the
                    // given parameter
                }
                else if (strcmp(doc->getNodeName(colorNode), "texture") == 0)
                {
                    // Look up the texture attribute
                    text = doc->getNodeAttribute(colorNode, "texture");
                    tex = effect->getTextureFromParam(atString(text));

                    // Create an atString for the texture coordinate list id
                    text = doc->getNodeAttribute(colorNode, "texcoord");
                    texcoord = atString(text);

                    // Make sure we found the texture
                    if (tex != NULL)
                    {
                        // Set the apply mode to "replace", so the texture
                        // color shows independent of lighting 
                        tex->setApplyMode(VS_TEXTURE_APPLY_REPLACE);

                        // Add the texture coordinate list id and texture
                        // attribute to the effect.  The list id will be used
                        // to decide on which set of texture coordinates to
                        // use when the effect is instantiated in a material
                        effect->addTexture(texcoord, tex);
                    }
                }
 
                // Try the next node
                colorNode = doc->getNextSiblingNode(colorNode);
            }
        }
        else if (strcmp(doc->getNodeName(child), "ambient") == 0)
        {
            // Get the color settings
            colorNode = doc->getNextChildNode(child);
            while (colorNode != NULL)
            {
                if (strcmp(doc->getNodeName(colorNode), "color") == 0)
                {
                    // Parse the color
                    color =
                        parseVector(doc, doc->getNextChildNode(colorNode), 4);

                    // Set the ambient color of the material
                    mat->setColor(VS_MATERIAL_SIDE_BOTH, 
                        VS_MATERIAL_COLOR_AMBIENT,
                        color[0], color[1], color[2]);
                }
                else if (strcmp(doc->getNodeName(colorNode), "param") == 0)
                {
                    // JPD:  Parametric material attributes not yet
                    //       implemented

                    // Get the parameter name attribute

                    // Look up the parameter

                    // Tell the effect that the emissive color comes from the
                    // given parameter
                }
                else if (strcmp(doc->getNodeName(colorNode), "texture") == 0)
                {
                    // Set the ambient material color to fully white
                    mat->setColor(VS_MATERIAL_SIDE_BOTH, 
                        VS_MATERIAL_COLOR_AMBIENT, 1.0, 1.0, 1.0);

                    // Look up the texture attribute
                    text = doc->getNodeAttribute(colorNode, "texture");
                    tex = effect->getTextureFromParam(atString(text));

                    // Create an atString for the texture coordinate list id
                    text = doc->getNodeAttribute(colorNode, "texcoord");
                    texcoord = atString(text);

                    // Make sure we found the texture
                    if (tex != NULL)
                    {
                        // Set the apply mode to "modulate", so ambient
                        // lighting affects the final color (in fixed-function,
                        // we can't really distinguish between ambient,
                        // diffuse, and specular for texture mapping)
                        tex->setApplyMode(VS_TEXTURE_APPLY_MODULATE);

                        // Add the texture coordinate list id and texture
                        // attribute to the effect.  The list id will be used
                        // to decide on which set of texture coordinates to
                        // use when the effect is instantiated in a material
                        effect->addTexture(texcoord, tex);
                    }
                }
     
                // Try the next node
                colorNode = doc->getNextSiblingNode(colorNode);
            }
        }
        else if (strcmp(doc->getNodeName(child), "diffuse") == 0)
        {
            // Get the color settings
            colorNode = doc->getNextChildNode(child);
            while (colorNode != NULL)
            {
                if (strcmp(doc->getNodeName(colorNode), "color") == 0)
                {
                    // Parse the color
                    color =
                        parseVector(doc, doc->getNextChildNode(colorNode), 4);

                    // Set the diffuse color of the material
                    mat->setColor(VS_MATERIAL_SIDE_BOTH, 
                        VS_MATERIAL_COLOR_DIFFUSE,
                        color[0], color[1], color[2]);
                }
                else if (strcmp(doc->getNodeName(colorNode), "param") == 0)
                {
                    // JPD:  Parametric material attributes not yet
                    //       implemented

                    // Get the parameter name attribute

                    // Look up the parameter

                    // Tell the effect that the diffuse color comes from the
                    // given parameter
                }
                else if (strcmp(doc->getNodeName(colorNode), "texture") == 0)
                {
                    // Set the diffuse material color to fully white
                    mat->setColor(VS_MATERIAL_SIDE_BOTH, 
                        VS_MATERIAL_COLOR_DIFFUSE, 1.0, 1.0, 1.0);

                    // Look up the texture attribute
                    text = doc->getNodeAttribute(colorNode, "texture");
                    tex = effect->getTextureFromParam(atString(text));

                    // Create an atString for the texture coordinate list id
                    text = doc->getNodeAttribute(colorNode, "texcoord");
                    texcoord = atString(text);

                    // Make sure we found the texture
                    if (tex != NULL)
                    {
                        // Set the apply mode to "modulate", so diffuse
                        // lighting affects the final color (in fixed-function,
                        // we can't really distinguish between ambient,
                        // diffuse, and specular for texture mapping)
                        tex->setApplyMode(VS_TEXTURE_APPLY_MODULATE);

                        // Add the texture coordinate list id and texture
                        // attribute to the effect.  The list id will be used
                        // to decide on which set of texture coordinates to
                        // use when the effect is instantiated in a material
                        effect->addTexture(texcoord, tex);
                    }
                }

                // Try the next node
                colorNode = doc->getNextSiblingNode(colorNode);
            }
        }
        else if (strcmp(doc->getNodeName(child), "specular") == 0)
        {
            // Get the color settings
            colorNode = doc->getNextChildNode(child);
            while (colorNode != NULL)
            {
                if (strcmp(doc->getNodeName(colorNode), "color") == 0)
                {
                    // Parse the color
                    color =
                        parseVector(doc, doc->getNextChildNode(colorNode), 4);

                    // Set the specular color of the material
                    mat->setColor(VS_MATERIAL_SIDE_BOTH, 
                        VS_MATERIAL_COLOR_SPECULAR,
                        color[0], color[1], color[2]);
                }
                else if (strcmp(doc->getNodeName(colorNode), "param") == 0)
                {
                    // JPD:  Parametric material attributes not yet
                    //       implemented

                    // Get the parameter name attribute

                    // Look up the parameter

                    // Tell the effect that the emissive color comes from the
                    // given parameter
                }
                else if (strcmp(doc->getNodeName(colorNode), "texture") == 0)
                {
                    // Set the specular material color to fully white
                    mat->setColor(VS_MATERIAL_SIDE_BOTH, 
                        VS_MATERIAL_COLOR_SPECULAR, 1.0, 1.0, 1.0);

                    // Look up the texture attribute
                    text = doc->getNodeAttribute(colorNode, "texture");
                    tex = effect->getTextureFromParam(atString(text));

                    // Create an atString for the texture coordinate list id
                    text = doc->getNodeAttribute(colorNode, "texcoord");
                    texcoord = atString(text);

                    // Make sure we found the texture
                    if (tex != NULL)
                    {
                        // Set the apply mode to "modulate", so ambient
                        // lighting affects the final color (in fixed-function,
                        // we can't really distinguish between ambient,
                        // diffuse, and specular for texture mapping)
                        tex->setApplyMode(VS_TEXTURE_APPLY_MODULATE);

                        // Add the texture coordinate list id and texture
                        // attribute to the effect.  The list id will be used
                        // to decide on which set of texture coordinates to
                        // use when the effect is instantiated in a material
                        effect->addTexture(texcoord, tex);
                    }
                }

                // Try the next node
                colorNode = doc->getNextSiblingNode(colorNode);
            }
        }
        else if (strcmp(doc->getNodeName(child), "shininess") == 0)
        {
            // Get the shininess settings
            colorNode = doc->getNextChildNode(child);
            while (colorNode != NULL)
            {
                if (strcmp(doc->getNodeName(colorNode), "value") == 0)
                {
                    // Parse the exponent
                    value = atof(doc->getNodeText(
                        doc->getNextChildNode(colorNode)));

                    // Set the constant shininess value
                    mat->setShininess(VS_MATERIAL_SIDE_BOTH, value);
                }
                else if (strcmp(doc->getNodeName(colorNode), "param") == 0)
                {
                    // JPD:  Parametric material attributes not yet
                    //       implemented

                    // Get the parameter name attribute

                    // Look up the parameter

                    // Tell the effect that the shininess exponent comes
                    // from the given parameter
                }

                // Try the next node
                colorNode = doc->getNextSiblingNode(colorNode);
            }
        }
        else if (strcmp(doc->getNodeName(child), "reflective") == 0)
        {
            // Not yet supported
            // We'll need to use this to support environment maps
        }
        else if (strcmp(doc->getNodeName(child), "reflectivity") == 0)
        {
            // Not yet supported
            // May need to use this to determine how to modulate the
            // environment map with the regular material
        }
        else if (strcmp(doc->getNodeName(child), "transparent") == 0)
        {
            // Get the transparency mode
            text = doc->getNodeAttribute(child, "opaque");
            if (strcmp(text, "RGB_ZERO") == 0)
                rgbZeroOpaque = true;
            else
                rgbZeroOpaque = false;

            // If transparency is set to RGB_ZERO for opaque, we have to
            // translate the transparency color into an alpha value
            colorNode = doc->getNextChildNode(child);
            while ((rgbZeroOpaque) && (colorNode != NULL))
            {
                if (strcmp(doc->getNodeName(colorNode), "color") == 0)
                {
                    // Parse the color
                    color = parseVector(doc,
                        doc->getNextChildNode(colorNode), 4);

                    // Average the RGB values and set the material's alpha
                    // (zero is opaque in this mode, so invert the value)
                    value = (color[0] + color[1] + color[2]) / 3.0;
                    mat->setAlpha(VS_MATERIAL_SIDE_BOTH, 1.0 - value);
                }
                else if (strcmp(doc->getNodeName(colorNode), "param") == 0)
                {
                    // JPD:  Parametric material attributes not yet
                    //       implemented

                    // Get the parameter name attribute

                    // Look up the parameter

                    // Tell the effect that the transparent color comes
                    // from the given parameter
                }
                else if (strcmp(doc->getNodeName(colorNode), "texture") == 0)
                {
                    // This is an alpha map...
                    // We can't support this in fixed-function directly,
                    // we'd have to convert the RGB channels of the texture
                    // to an alpha channel, and then apply it to the geometry
                    // in "modulate" mode
                }

                // Try the next node
                colorNode = doc->getNextSiblingNode(colorNode);
            }
        }
        else if (strcmp(doc->getNodeName(child), "transparency") == 0)
        {
            // Get the transparency settings (but only if the mode has been
            // set to A_ONE by the "transparent" settings)
            colorNode = doc->getNextChildNode(child);
            while ((!rgbZeroOpaque) && (colorNode != NULL))
            {
                if (strcmp(doc->getNodeName(colorNode), "float") == 0)
                {
                    // Parse the transparency value
                    value = atof(doc->getNodeText(
                        doc->getNextChildNode(colorNode)));

                    // If we're using A_ONE for opaque, set the constant
                    // transparency value, otherwise (in RGB_ZERO mode) ignore
                    // this field as the transparency value comes from the
                    // transparent color
                    mat->setAlpha(VS_MATERIAL_SIDE_BOTH, value);
                }
                else if (strcmp(doc->getNodeName(colorNode), "param") == 0)
                {
                    // JPD:  Parametric material attributes not yet
                    //       implemented

                    // Get the parameter name attribute

                    // Look up the parameter

                    // Tell the effect that the transparency comes from the
                    // given parameter
                }

                // Try the next node
                colorNode = doc->getNextSiblingNode(colorNode);
            }
        }
        else if (strcmp(doc->getNodeName(child), "index_of_refraction") == 0)
        {
            // Not supported with fixed function materials
        }

        // Advance to the next node
        child = doc->getNextSiblingNode(child);
    }
}

// ------------------------------------------------------------------------
// Processes an effect using the profile_COMMON method.  This is
// essentially a fixed-function shader, which will be represented as
// a vsMaterialAttribute, and possibly one or more vsTextureAttributes
// ------------------------------------------------------------------------
void vsCOLLADALoader::processCommonEffect(atXMLDocument *doc,
                                          atXMLDocumentNodePtr current,
                                          atString *id)
{
    atXMLDocumentNodePtr child;
    atXMLDocumentNodePtr technique;
    vsCOLLADAFixedEffect *effect;
    vsCOLLADAEffectParameter *param;

    // Create the effect object, which acts as a container for a material
    // attribute and zero or more texture attributes
    effect = new vsCOLLADAFixedEffect(*id);

    // Iterate over the profile's children
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // Process this child node
        if (strcmp(doc->getNodeName(child), "image") == 0)
        {
            // Process and create a new image
            processImage(doc, child);
        }
        else if (strcmp(doc->getNodeName(child), "newparam") == 0)
        {
            // Process and create a new parameter for this effect profile
            param = processNewParam(doc, child);

            // Add the parameter to the effect (if a parameter was created)
            effect->addParameter(param);
        }
        else if (strcmp(doc->getNodeName(child), "technique") == 0)
        {
            // Generate the various attributes required based on the
            // rendering technique
            technique = doc->getNextChildNode(child);
            while (technique != NULL)
            {
                // Get the type of technique
                if ((strcmp(doc->getNodeName(technique), "constant") == 0) ||
                    (strcmp(doc->getNodeName(technique), "lambert") == 0) ||
                    (strcmp(doc->getNodeName(technique), "phong") == 0) ||
                    (strcmp(doc->getNodeName(technique), "blinn") == 0))
                {
                    // The primary difference between these four techniques
                    // is what material parameters are allowed.  We treat
                    // them as the same and process whatever parameters are
                    // encountered.  There is also a subtle difference
                    // between "phong" and "blinn" when it comes to specular
                    // lighting, but fixed-function OpenGL only supports
                    // the "blinn" model, so we treat them as identical here
                    processMaterialAttributes(doc, technique, effect);
                }

                // Try the next node
                technique = doc->getNextSiblingNode(technique);
            }
        }

        // Try the next node
        child = doc->getNextSiblingNode(child);
    }

    // Add the effect to the library
    effect->ref();
    effectLibrary->addEntry(id, effect);
}

// ------------------------------------------------------------------------
// Processes an effect using the profile_GLSL method.  This profile can
// make use of GLSL programs and shaders to generate its material effects.
// ------------------------------------------------------------------------
void vsCOLLADALoader::processGLSLEffect(atXMLDocument *doc,
                                        atXMLDocumentNodePtr current)
{
}

// ------------------------------------------------------------------------
// Processes an <effect> XML subtree and creates an effect library object.
// Effects are used as the basis for materials, and can be comprised of
// simple lighting parameters and/or shaders and shader parameters
// ------------------------------------------------------------------------
void vsCOLLADALoader::processEffect(atXMLDocument *doc,
                                    atXMLDocumentNodePtr current)
{
    char *attr;
    atXMLDocumentNodePtr child;
    atString *effectID;

    // Get the effect ID
    attr = doc->getNodeAttribute(current, "id");
    if (attr == NULL)
        effectID = new atString(getUnnamedName());
    else
        effectID = new atString(attr);

    // Process this effect's children
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See if this is a child we recognize
        if (strcmp(doc->getNodeName(child), "image") == 0)
        {
            // Process and create a new image
            processImage(doc, child);
        }
        else if (strcmp(doc->getNodeName(child), "newparam") == 0)
        {
            // Process and create a new parameter for this effect
            processNewParam(doc, child);
        }
        else if (strcmp(doc->getNodeName(child), "profile_COMMON") == 0)
        {
            // Process the common profile version of this effect
            processCommonEffect(doc, child, effectID);
        }
        else if (strcmp(doc->getNodeName(child), "profile_GLSL") == 0)
        {
            // Process the GLSL profile version of this effect
            processGLSLEffect(doc, child);
        }

        // Try the next child
        child = doc->getNextSiblingNode(child);
    }
}

// ------------------------------------------------------------------------
// Processes the <library_effects> XML subtree and creates the effect
// library and all the objects it contains
// ------------------------------------------------------------------------
void vsCOLLADALoader::processLibraryEffects(atXMLDocument *doc,
                                            atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr child;

    // Iterate over the image nodes in the subdocument
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // If this is an effect, process it
        if (strcmp(doc->getNodeName(child), "effect") == 0)
            processEffect(doc, child);

        // Move on to the next node
        child = doc->getNextSiblingNode(child);
    }
}

// ------------------------------------------------------------------------
// Processes the <library_geometries> XML subtree and creates the geometry
// library and all the objects it contains
// ------------------------------------------------------------------------
void vsCOLLADALoader::processLibraryGeometries(atXMLDocument *doc,
                                               atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr child;
    vsCOLLADAGeometry *geom;
    atString *id;
    char *attr;

    // Get the first child in the library, and iterate over the geometries
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // Process this node if it's a geometry
        if (strcmp(doc->getNodeName(child), "geometry") == 0)
        {
            // Get the controller's ID (we need to do this here, so we can
            // create a unique ID for the geometry if it's missing)
            attr = doc->getNodeAttribute(child, "id");
            if (attr == NULL)
                id = new atString(getUnnamedName());
            else
                id = new atString(attr);

            // Create a new COLLADA geometry object
            geom = new vsCOLLADAGeometry(*id, doc, child);

            // Add the new geometry object to the geometry library
            geom->ref();
            geometryLibrary->addEntry(id, geom);
        }

        // Move on to the next node
        child = doc->getNextSiblingNode(child);
    }
}

// ------------------------------------------------------------------------
// Processes an <image> XML subtree and creates an image library object
// ------------------------------------------------------------------------
void vsCOLLADALoader::processImage(atXMLDocument *doc,
                                   atXMLDocumentNodePtr current)
{
    char *attr;
    atString *id;
    atXMLDocumentNodePtr child;
    vsTextureAttribute *texture;
    char *filename;
    char name[256];
    atString texFile;

    // Get the ID of the image
    id = new atString(doc->getNodeAttribute(current, "id"));

    // Start with a NULL texture
    texture = NULL;

    // Look for the image initializer
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See if this is a recognized image initializer
        if (strcmp(doc->getNodeName(child), "init_from") == 0)
        {
            // Copy the filename
            filename = doc->getNodeText(doc->getNextChildNode(child));

            // Find the full path to the texture file
            texFile = findFile(filename);

            // See if we found it
            if (texFile.getLength() > 0)
            {
                // Create the texture attribute using the given image filename
                texture = new vsTextureAttribute();
                texture->loadImageFromFile(texFile.getString());
            }
            else
            {
                // No image found, so no texture
                texture = NULL;
            }
        }
        else if (strcmp(doc->getNodeName(child), "data") == 0)
        {
            // This isn't supported now, because I can't find a concrete
            // spec on how it's formatted, or how the image metadata
            // (size, color depth, etc) should be specified.  It also seems
            // like nobody will be using this feature anyway (external
            // image files seem to be preferred by far)

            // No texture
            texture = NULL;
        }

        // Try the next child
        child = doc->getNextSiblingNode(child);
    }

    // If we managed to create a texture, add it to the image library
    if (texture != NULL)
    {
        texture->ref();
        imageLibrary->addEntry(id, texture);
    }
}

// ------------------------------------------------------------------------
// Processes the <library_images> XML subtree and creates the image
// library and all the objects it contains
// ------------------------------------------------------------------------
void vsCOLLADALoader::processLibraryImages(atXMLDocument *doc,
                                           atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr child;

    // Iterate over the image nodes in the subdocument
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // If this is an image, process it
        if (strcmp(doc->getNodeName(child), "image") == 0)
            processImage(doc, child);

        // Move on to the next node
        child = doc->getNextSiblingNode(child);
    }
}

// ------------------------------------------------------------------------
// Processes a <light> XML subtree and creates a light library object
// ------------------------------------------------------------------------
void vsCOLLADALoader::processLight(atXMLDocument *doc,
                                   atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr child;
    vsLightAttribute *light;
    char *attr;
    atString *lightID;
    atString lightName;
    atXMLDocumentNodePtr lightNode;
    atVector color;
    double constantAtten;
    double linearAtten;
    double quadraticAtten;
    double spotExponent;
    double spotAngle;

    // Get the light ID
    attr = doc->getNodeAttribute(current, "id");
    if (attr == NULL)
        lightID = new atString(getUnnamedName());
    else
        lightID = new atString(attr);

    // Create a vsLightAttribute for the light
    light = new vsLightAttribute();

    // Name the light
    attr = doc->getNodeAttribute(current, "name");
    if (attr == NULL)
        light->setName(lightID->getString());
    else
        light->setName(attr);

    // Set up the light
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See if this is a kind of light we recognize
        if (strcmp(doc->getNodeName(child), "ambient") == 0)
        {
            // Get the ambient light color
            lightNode = doc->getNextChildNode(child);
            while (lightNode != NULL)
            {
                if (strcmp(doc->getNodeName(lightNode), "color") == 0)
                {
                    // Parse the color
                    color = 
                        parseVector(doc, doc->getNextChildNode(lightNode), 3);
                }

                // Try the next node
                lightNode = doc->getNextSiblingNode(lightNode);
            }

            // Set up the ambient light
            light->setAmbientColor(color[0], color[1], color[2]);
            light->setDiffuseColor(0.0, 0.0, 0.0);
            light->setSpecularColor(0.0, 0.0, 0.0);
        }
        else if (strcmp(doc->getNodeName(child), "directional") == 0)
        {
            // Get the directional light color
            lightNode = doc->getNextChildNode(child);
            while (lightNode != NULL)
            {
                if (strcmp(doc->getNodeName(lightNode), "color") == 0)
                {
                    // Parse the color
                    color = 
                        parseVector(doc, doc->getNextChildNode(lightNode), 3);
                }

                // Try the next node
                lightNode = doc->getNextSiblingNode(lightNode);
            }

            // Set up the directional light
            light->setAmbientColor(0.0, 0.0, 0.0);
            light->setDiffuseColor(color[0], color[1], color[2]);
            light->setSpecularColor(color[0], color[1], color[2]);
        }
        else if (strcmp(doc->getNodeName(child), "point") == 0)
        {
            // Initialize the attenuation values
            constantAtten = 0.0;
            linearAtten = 0.0;
            quadraticAtten = 0.0;

            // Get the point light settings
            lightNode = doc->getNextChildNode(child);
            while (lightNode != NULL)
            {
                if (strcmp(doc->getNodeName(lightNode), "color") == 0)
                {
                    // Parse the color
                    color = 
                        parseVector(doc, doc->getNextChildNode(lightNode), 3);
                }

                // Try the next node
                lightNode = doc->getNextSiblingNode(lightNode);
            }

            // Set up the point light
            light->setAmbientColor(0.0, 0.0, 0.0);
            light->setDiffuseColor(color[0], color[1], color[2]);
            light->setSpecularColor(color[0], color[1], color[2]);
        }
        else if (strcmp(doc->getNodeName(child), "spot") == 0)
        {
            // Get the spotlight settings
            lightNode = doc->getNextChildNode(child);
            while (lightNode != NULL)
            {
                if (strcmp(doc->getNodeName(lightNode), "color") == 0)
                {
                    // Parse the color
                    color = 
                        parseVector(doc, doc->getNextChildNode(lightNode), 3);
                }

                // Try the next node
                lightNode = doc->getNextSiblingNode(lightNode);
            }

            // Set up the spotlight
            light->setAmbientColor(0.0, 0.0, 0.0);
            light->setDiffuseColor(color[0], color[1], color[2]);
            light->setSpecularColor(color[0], color[1], color[2]);
        }

        // Try the next node
        child = doc->getNextChildNode(child);
    }

    // Add the light to the library
    light->ref();
    lightLibrary->addEntry(lightID, light);
}

// ------------------------------------------------------------------------
// Processes the <library_lights> XML subtree and creates the light
// library and all the objects it contains
// ------------------------------------------------------------------------
void vsCOLLADALoader::processLibraryLights(atXMLDocument *doc,
                                           atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr child;

    // Iterate over the image nodes in the subdocument
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // If this is a controller, process it
        if (strcmp(doc->getNodeName(child), "light") == 0)
            processLight(doc, child);

        // Move on to the next node
        child = doc->getNextSiblingNode(child);
    }
}

// ------------------------------------------------------------------------
// Processes a <setparam> XML subtree as part of a material instantiation
// ------------------------------------------------------------------------
void vsCOLLADALoader::processSetParam(atXMLDocument *doc,
                                      atXMLDocumentNodePtr current,
                                      vsCOLLADAEffect *effect)
{
    atString paramName;
    vsCOLLADAEffectParameter *param;
    atXMLDocumentNodePtr valueNode;

    // Look up the parameter in the effect
    paramName = atString(doc->getNodeAttribute(current, "ref"));
    param = effect->getParameter(paramName);

    // If we found the parameter, set it
    if (param != NULL)
    {
        // Process the parameter value(s)
        param->set(doc, current);
    }
    else
    {
        notify(AT_WARN, "vsCOLLADALoader::processSetParam:"
            " Parameter %s not found in effect %s\n", paramName.getString(),
            effect->getID().getString());
    }
}

// ------------------------------------------------------------------------
// Processes a <material> XML subtree and creates a material library object
// ------------------------------------------------------------------------
void vsCOLLADALoader::processMaterial(atXMLDocument *doc,
                                      atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr child;
    atXMLDocumentNodePtr paramNode;
    char *text;
    atString *id;
    atString effectID;
    vsCOLLADAEffect *effect;
    vsCOLLADAEffect *materialEffect;
    vsCOLLADAEffectParameter *param;

    // Get the id of the material
    id = new atString(doc->getNodeAttribute(current, "id"));

    // Iterate over the children of this material node
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // If this is an effect instance, process it
        if (strcmp(doc->getNodeName(child), "instance_effect") == 0)
        {
            // Get the effect to instantiate
            effect = NULL;
            text = doc->getNodeAttribute(child, "url");
            if (text != NULL)
            {
                // Look up the effect in the effect library
                effectID = atString(text);
                effect = getEffect(effectID);
            }

            // If we have the effect, finish instantiating it
            if (effect != NULL)
            {
                // Clone the effect
                materialEffect = effect->clone(*id);

                // Process any parameter settings
                paramNode = doc->getNextChildNode(child);
                while (paramNode != NULL)
                {
                    // If this is a setparam node, process it
                    if (strcmp(doc->getNodeName(paramNode), "setparam") == 0)
                    {
                        // Process the setparam
                        processSetParam(doc, paramNode, materialEffect);
                    }
                }

                // Store the new material in the material library
                materialEffect->ref();
                materialLibrary->addEntry(id, materialEffect);
            }
        }

        // Move on to the next node
        child = doc->getNextSiblingNode(child);
    }
}

// ------------------------------------------------------------------------
// Processes the <library_materials> XML subtree and creates the material
// library and all the objects it contains
// ------------------------------------------------------------------------
void vsCOLLADALoader::processLibraryMaterials(atXMLDocument *doc,
                                              atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr child;

    // Iterate over the material nodes in the subdocument
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // If this is a material, process it
        if (strcmp(doc->getNodeName(child), "material") == 0)
            processMaterial(doc, child);

        // Move on to the next node
        child = doc->getNextSiblingNode(child);
    }
}

// ------------------------------------------------------------------------
// Process a <instance_material> XML subtree (under an <instance_geometry>)
// ------------------------------------------------------------------------
void vsCOLLADALoader::processInstanceMaterial(atXMLDocument *doc,
                                              atXMLDocumentNodePtr current,
                                              vsComponent *geomComponent)
{
    char *attr;
    atXMLDocumentNodePtr child;
    vsNode *subMesh;
    vsCOLLADAEffect *materialEffect;
    vsTextureAttribute *tex;
    vsMaterialAttribute *matAttr;
    vsTransparencyAttribute *transpAttr;
    atString semantic;
    atString inputSemantic;
    int set;
    atList *texList;
    int unit;
    bool unitsUsed[VS_MAXIMUM_TEXTURE_UNITS];
    vsGeometryBase *geom;
    int listSize;
    atVector *texcoords;
    bool alphaFromTexture;
    vsTransparencyAttribute *transp;

    // Get the material's ID
    attr = doc->getNodeAttribute(current, "target");

    // Look up the material effect in the library
    materialEffect = getMaterial(atString(attr));

    // Find the target material component under the main component (this
    // component's name will be the same as the target material symbol)
    attr = doc->getNodeAttribute(current, "symbol");
    subMesh = geomComponent->findNodeByName(attr);

    // Assume no alpha at first
    alphaFromTexture = false;

    // Initialize the array of texture units used
    memset(unitsUsed, 0, sizeof(unitsUsed));

    // If we found the effect and target, handle any bindings now
    if ((materialEffect != NULL) && (subMesh != NULL))
    {
        // Handle any parameter and attribute bindings
        child = doc->getNextChildNode(current);
        while (child != NULL)
        {
            // Look for bindings
            if (strcmp(doc->getNodeName(child), "bind_vertex_input") == 0)
            {
                // Get the bind attributes
                semantic = 
                    atString(doc->getNodeAttribute(child, "semantic"));
                inputSemantic = 
                    atString(doc->getNodeAttribute(child, "input_semantic"));
                set = atoi(doc->getNodeAttribute(child, "input_set"));

                // Check the input semantic for this vertex input
                if (strcmp(inputSemantic.getString(), "TEXCOORD") == 0)
                {
                    // The semantic in this case specifies the texture
                    // coordinate set to bind.  Get the textures assigned to
                    // these texture coordinates from the material
                    // JPD: This isn't necessarily a fixed effect here, we need
                    //      to re-evaluate this once we start adding shader
                    //      support
                    texList = ((vsCOLLADAFixedEffect *)
                        materialEffect)->getTextures(semantic);

                    // Make sure we got a valid texture list
                    if (texList != NULL)
                        tex = (vsTextureAttribute *)texList->getFirstEntry();
                    else
                        tex = NULL;

                    // Iterate over all textures in the list
                    while (tex != NULL)
                    {
                        // Find a viable texture unit for this texture,
                        // start with the input data set
                        unit = set;
                        // start with the first texture unit (unit 0)
                        //unit = 0;

                        // Keep searching until we find an unused texture unit
                        while ((unitsUsed[unit]) && 
                               (unit < VS_MAXIMUM_TEXTURE_UNITS))
                            unit++;

                        // See if we found a viable texture unit
                        if (unit < VS_MAXIMUM_TEXTURE_UNITS)
                        {
                            // See if the texture unit is the same as the 
                            // texture coordinate set
                            if (unit != set)
                            {
                                // We need to copy the texture coordinates
                                // from the given set to the set that
                                // matches the new texture unit.  First
                                // get the geometry object
                                geom = (vsGeometryBase *)subMesh->getChild(0);

                                // Get a copy of the source texture
                                // coordinates
                                listSize = geom->getDataListSize(
                                    VS_GEOMETRY_TEXTURE0_COORDS + set);
                                texcoords = new atVector[listSize];
                                geom->getDataList(
                                    VS_GEOMETRY_TEXTURE0_COORDS + set,
                                    texcoords);

                                // Set up the target texture coordinates
                                geom->setDataListSize(
                                    VS_GEOMETRY_TEXTURE0_COORDS + unit,
                                    listSize);
                                geom->setDataList(
                                    VS_GEOMETRY_TEXTURE0_COORDS + unit,
                                    texcoords);
                                geom->setBinding(
                                    VS_GEOMETRY_TEXTURE0_COORDS + unit,
                                    VS_GEOMETRY_BIND_PER_VERTEX);

                                // Clean up our temporary list
                                delete [] texcoords;
                            }
                            else
                            {
                                // Get the geometry object
                                geom = (vsGeometryBase *)subMesh->getChild(0);

                                // Make sure the binding is set to per-vertex
                                geom->setBinding(
                                    VS_GEOMETRY_TEXTURE0_COORDS + unit,
                                    VS_GEOMETRY_BIND_PER_VERTEX);
                            }

                            // Mark this texture unit used
                            unitsUsed[unit] = true;

                            // Set the texture unit, using the set number
                            tex->setTextureUnit(unit);

                            // Add the texture to the target
                            subMesh->addAttribute(tex->clone());
                        }

                        // Check this texture for transparency (we may need
                        // to add a transparency attribute if it is
                        // transparent)
                        if (tex->isTransparent())
                            alphaFromTexture = true;

                        // If there's another texture to bind, do it next
                        tex = (vsTextureAttribute *)texList->getNextEntry();
                    }
                }
            }
            else if (strcmp(doc->getNodeName(child), "bind") == 0)
            {
                // JPD:  Not yet supported (this is primarily for shaders)
            }

            // Try the next node
            child = doc->getNextSiblingNode(child);
        }

        // Add the effect's material as well
        matAttr = ((vsCOLLADAFixedEffect *)materialEffect)->getMaterial();
        if (matAttr != NULL)
        {
            subMesh->addAttribute(matAttr->clone());

            // If the material has a non-one alpha, also add a transparency
            // attribute
            if (matAttr->getAlpha(VS_MATERIAL_SIDE_FRONT) < 1.0)
            {
                transpAttr = new vsTransparencyAttribute();
                transpAttr->enable();
                subMesh->addAttribute(transpAttr);
            }
        }

        // Check the submesh to see if it has a transparency attribute
        transp = (vsTransparencyAttribute *)
            subMesh->getTypedAttribute(VS_ATTRIBUTE_TYPE_TRANSPARENCY, 0);

        // If there is no transparency attribute, and the submesh needs one
        // because of a transparent texture, create it and add it now
        if ((transp == NULL) && (alphaFromTexture == true))
        {
            transp = new vsTransparencyAttribute();
            transp->enable();
            subMesh->addAttribute(transp);
        }
    }
}

// ------------------------------------------------------------------------
// Process a <bind_material> XML subtree (under an <instance_geometry>)
// ------------------------------------------------------------------------
void vsCOLLADALoader::processBindMaterial(atXMLDocument *doc,
                                          atXMLDocumentNodePtr current,
                                          vsComponent *geomComponent)
{
    atXMLDocumentNodePtr child;
    atXMLDocumentNodePtr instance;

    // Get the first child of the bind_material node
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See what kind of node this is
        if (strcmp(doc->getNodeName(child), "param") == 0)
        {
            // VESS currently doesn't support animating material
            // parameters, so nothing happens here
        }
        else if (strcmp(doc->getNodeName(child), "technique_common") == 0)
        {
            // Only children here are "instance_material" nodes, so process
            // them
            instance = doc->getNextChildNode(child);
            while (instance != NULL)
            {
                // If this is a material instance, process it
                if (strcmp(doc->getNodeName(instance),
                    "instance_material") == 0)
                    processInstanceMaterial(doc, instance, geomComponent);

                // Try the next node
                instance = doc->getNextSiblingNode(instance);
            }
        }

        // Get the next child under the bind_material node
        child = doc->getNextSiblingNode(child);
    }
}

// ------------------------------------------------------------------------
// Takes a vsCOLLADANode that is the root of the skeleton node hierarchy,
// and a vsCOLLADASkin object representing the skin to be applied to the
// skeleton and creates a vsSkeleton object
// ------------------------------------------------------------------------
vsSkeleton *vsCOLLADALoader::createSkeleton(vsCOLLADANode *root,
                                            vsCOLLADASkin *skin)
{
    atArray *bones;
    int i;
    vsCOLLADADataSource *source;
    vsCOLLADANode *joint;
    vsCOLLADANode *skeletonRoot;
    vsSkeleton *newSkeleton;
    vsNode *parent;

    // Create an array to hold the bone nodes for the skeleton
    bones = new atArray();

    // Get the skeleton joint names from the skin, and find the corresponding
    // node in the skeleton hierarchy.  Be sure to use SID's for a Name_array
    // source and regular ID's for an IDREF_array source
    source = skin->getJointNames();
    if (source->getDataArray()->getDataType() == vsCOLLADADataArray::IDREF)
    {
        // Iterate over the joint names
        for (i = 0; i < source->getDataCount(); i++)
        {
            // Find the joint with the given ID in the skeleton hierarchy
            // and add it to the array
            joint = root->findNodeByID(source->getString(i));
            if (joint != NULL)
            {
                bones->setEntry(i, joint);
            }
            else
            {
                // Print an error and bail
                printf("vsCOLLADALoader::createSkeleton: ID %s not found!\n",
                    source->getString(i).getString());
            }
        }
    }
    else
    {
        // Iterate over the joint names
        for (i = 0; i < source->getDataCount(); i++)
        {
            // Find the joint with the given scoped ID in the skeleton
            // hierarchy and add it to the array
            joint = root->findNodeBySID(source->getString(i));
            if (joint != NULL)
                bones->setEntry(i, joint);
/*
            else
            {
                // Notify the user that we couldn't find one of the joints
                // in the skeleton
                printf("vsCOLLADALoader::createSkeleton: SID %s not found "
                    "under node %s!\n", source->getString(i).getString(),
                    root->getID().getString());
            }
*/
        }
    }

    // Create the skeleton
    newSkeleton = new vsSkeleton(bones, bones->getNumEntries(), root);

    // Return it
    return newSkeleton;
}

// ------------------------------------------------------------------------
// Process an <instance_controller> XML subtree and creates a new instance
// of the given controller.  This involves attaching the controller to a
// skeleton (if it's a skin controller), and binding materials
// ------------------------------------------------------------------------
void vsCOLLADALoader::processInstanceController(atXMLDocument *doc,
                                                atXMLDocumentNodePtr current,
                                                vsCOLLADANode *parent)
{
    char *attr;
    char *text;
    atString skeletonRootURI;
    char *idStr;
    atString skeletonID;
    atString *id;
    vsCOLLADAController *libraryCtrl;
    vsComponent *geomComponent;
    atXMLDocumentNodePtr child;
    vsCOLLADAEffect *materialEffect;
    vsCOLLADANode *skeletonRoot;
    vsCOLLADASkin *skinCtrl;
    vsSkeleton *thisSkeleton;
    vsCOLLADADataSource *source;
    atArray *inverseBindMatrices;
    atMatrix *matrix;
    int i;
    vsSkin *skin;

    // First, look up the geometry in the library
    attr = doc->getNodeAttribute(current, "url");
    libraryCtrl = getController(atString(attr));

    // See if we found the geometry in the library, bail if not
    if (libraryCtrl)
    {
        // Instance the controller's geometry and add it to the parent
        geomComponent = libraryCtrl->instance();
        parent->addChild(geomComponent);

        // Now, process the remaining controller parameters
        child = doc->getNextChildNode(current);
        while (child != NULL)
        {
            // If this is a material instance, process it
            if (strcmp(doc->getNodeName(child), "bind_material") == 0)
            {
                // Process the material bindings
                processBindMaterial(doc, child, geomComponent);
            }
            if (strcmp(doc->getNodeName(child), "skeleton") == 0)
            {
                // This must be a skin controller (only skins use skeletons),
                // so cast the controller as such
                skinCtrl = (vsCOLLADASkin *)libraryCtrl;

                // Get the name of the skeleton that this controller will
                // use
                text = doc->getNodeText(doc->getNextChildNode(child));
                skeletonRootURI = atString(text);
                skeletonID = NULL;

                // See if we've already created this skeleton
                thisSkeleton = getSkeleton(skeletonRootURI);
                if (thisSkeleton == NULL)
                {
                    // Check the ID string to see what kind of URI this is
                    idStr = skeletonRootURI.getString();
                    if (idStr[0] == '#')
                    {
                        // This is a URI fragment, meaning the root node
                        // is local to this file.  We should only need to
                        // strip the leading '#' and look up the ID
                        skeletonID = atString(&idStr[1]);
                    }
                    else
                    {
                        // Other forms of URI are not supported
                        skeletonID = skeletonRootURI;
                    }

                    // Try to fetch the root node of the skeleton from the
                    // library
                    skeletonRoot = getSkeletonRoot(skeletonRootURI);
                    if (skeletonRoot == NULL)
                    {
                        // Walk up to the root of the scene
                        skeletonRoot = parent;
                        while (skeletonRoot->getParentCount() > 0)
                            skeletonRoot = (vsCOLLADANode *)
                                skeletonRoot->getParent(0);

                        // Search for the root of the skeleton from the
                        // root of the scene
                        skeletonRoot = (vsCOLLADANode *)skeletonRoot->
                            findNodeByID(skeletonID);
                    }

                    // See if we found the root node of the skeleton
                    if (skeletonRoot != NULL)
                    {
                        // Now that we have the node hierarchy for the
                        // skeleton, and the skin controller prototype, we
                        // can create the vsSkeleton itself
                        thisSkeleton = createSkeleton(skeletonRoot, skinCtrl);

                        // Add the skeleton to the loader's skeleton list
                        thisSkeleton->ref();
                        skeletonList->addEntry(thisSkeleton);

                        // Also add the skeleton to our skeleton map, so
                        // we can look it up again by root node
                        id = new atString(skeletonID);
                        thisSkeleton->ref();
                        skeletons->addEntry(id, thisSkeleton);
                    }
                    else
                    {
                        // Print an error about the missing skeleton
                        printf("vsCOLLADALoader::createSkeleton:  Unable to "
                            "find skeleton at node '%s'\n",
                             skeletonRootURI.getString());
                    }
                }

                // Create an array to hold the inverse bind matrices (aka
                // the "bone space" matrices)
                inverseBindMatrices = new atArray(); 

                // Get the inverse bind matrices from the skin and store
                // them in the array
                source = skinCtrl->getInverseBindMatrices();
                for (i = 0; i < source->getDataCount(); i++)
                {
                    // Create a new matrix object and copy the matrix data
                    // from the data source
                    matrix = new atMatrix();
                    matrix->copy(source->getMatrix(i));

                    // Store the matrix in the array
                    inverseBindMatrices->setEntry(i, matrix);
                }

                // Create the VESS skin (whether we have a skeleton or not)
                skin = new vsSkin(geomComponent, thisSkeleton,
                    inverseBindMatrices);

                // Add the skin to the skin list
                skin->ref();
                skinList->addEntry(skin);
            }

            // Move on to the next node
            child = doc->getNextSiblingNode(child);
        }
    }
    else
        notify(AT_ERROR, "Controller %s (instanced by %s) not found!\n",
            attr, parent->getName());
}

// ------------------------------------------------------------------------
// Process an <instance_geometry> XML subtree and creates a new instance
// of the given geometry, binding any materials that are required
// ------------------------------------------------------------------------
void vsCOLLADALoader::processInstanceGeometry(atXMLDocument *doc,
                                              atXMLDocumentNodePtr current,
                                              vsCOLLADANode *parent)
{
    char *attr;
    vsCOLLADAGeometry *libraryGeom;
    vsComponent *geomComponent;
    atXMLDocumentNodePtr child;
    vsCOLLADAEffect *materialEffect;

    // First, look up the geometry in the library
    attr = doc->getNodeAttribute(current, "url");
    libraryGeom = getGeometry(atString(attr));

    // See if we found the geometry in the library, bail if not
    if (libraryGeom)
    {
        // Instance the geometry and add it to the parent
        geomComponent = libraryGeom->instance();
        parent->addChild(geomComponent);

        // Now, process any material bindings on this geometry
        child = doc->getNextChildNode(current);
        while (child != NULL)
        {
            // If this is a material instance, process it
            if (strcmp(doc->getNodeName(child), "bind_material") == 0)
            {
                // Process the material bindings
                processBindMaterial(doc, child, geomComponent);
            }

            // Move on to the next node
            child = doc->getNextSiblingNode(child);
        }
    }
    else
        notify(AT_ERROR, "Geometry %s (instanced by %s) not found!\n",
            attr, parent->getName());
}

// ------------------------------------------------------------------------
// Processes a <node> XML subtree and creates a node in a COLLADA scene.
// This can be part of a COLLADA visual scene, or just a part of the
// node library that may be instantiated elsewhere.  In either case, a node
// can have child nodes and may contain transformations.  This first pass
// ignores any object instantiations within the scene (some objects like
// controllers depend on the node hierarchy being in place first)
// ------------------------------------------------------------------------
void vsCOLLADALoader::processNodePass1(atXMLDocument *doc,
                                       atXMLDocumentNodePtr current,
                                       vsCOLLADANode *parent)
{
    char *attr;
    atString name;
    atString id;
    atString sid;
    vsCOLLADANodeType nodeType;
    vsCOLLADANode *newNode;
    atXMLDocumentNodePtr child;
    vsTransformAttribute *xformAttr;
    vsCOLLADATransform *xform;
    atString *rootID;
    bool isSkeletonRoot;

    // Get the id for this node
    attr = doc->getNodeAttribute(current, "id");
    if (attr != NULL)
        id.setString(attr);
    else
        id.setString(getUnnamedName());

    // Get the name for this node (if any)
    attr = doc->getNodeAttribute(current, "name");
    if (attr != NULL)
        name.setString(attr);

    // Get the scoped ID for this node (if any)
    attr = doc->getNodeAttribute(current, "sid");
    if (attr != NULL)
        sid.setString(attr);

    // At first, assume that this node is not the root of a skeleton
    isSkeletonRoot = false;

    // Get the node's type (joint or regular node)
    attr = doc->getNodeAttribute(current, "type");
    if ((attr != NULL) && (strcmp(attr, "JOINT") == 0))
        nodeType = VS_CNODE_TYPE_JOINT;
    else
        nodeType = VS_CNODE_TYPE_NODE;

    // Create the node, and add it to the parent node
    newNode = new vsCOLLADANode(id, name, sid, nodeType);
    parent->addChild(newNode);

    // See if this node is a joint
    if (nodeType == VS_CNODE_TYPE_JOINT)
    {
        // Check to see if we're in the middle of parsing a skeleton
        if (!parsingSkeleton)
        {
            // Remember that this is a skeleton root
            isSkeletonRoot = true;

            // Add this node to the skeletonRoots map
            rootID = new atString(id);
            newNode->ref();
            skeletonRoots->addEntry(rootID, newNode);

            // Flag that we're now parsing a skeleton
            parsingSkeleton = true;
        }
    }

    // Traverse the node's children
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See if this is a recognized type
        if (strcmp(doc->getNodeName(child), "node") == 0)
        {
            // Recursively traverse this node
            processNodePass1(doc, child, newNode);
        }
        else if ((strcmp(doc->getNodeName(child), "lookat") == 0) ||
                 (strcmp(doc->getNodeName(child), "matrix") == 0) ||
                 (strcmp(doc->getNodeName(child), "scale") == 0) ||
                 (strcmp(doc->getNodeName(child), "skew") == 0) ||
                 (strcmp(doc->getNodeName(child), "rotate") == 0) ||
                 (strcmp(doc->getNodeName(child), "translate") == 0))
        {
            // Create the transform node
            xform = new vsCOLLADATransform(doc, child);

            // Add the transform to the node
            newNode->addTransform(xform);
        }

        // Move on to the next child
        child = doc->getNextSiblingNode(child);
    }

    // If there are any COLLADA transforms on this node, create a
    // vsTransformAttribute to render them properly
    if (newNode->getFirstTransform() != NULL)
    {
        // Create a transform attribute using the final transform matrix
        xformAttr = new vsTransformAttribute();
        xformAttr->setDynamicTransform(newNode->getCombinedTransform());

        // Attach it to the component we created
        newNode->addAttribute(xformAttr);
    }

    // If this node is a skeleton root, flag that we're no
    // longer parsing a skeleton
    if (isSkeletonRoot)
        parsingSkeleton = false;
}

// ------------------------------------------------------------------------
// Parses a user properties string to create an LOD attribute for the given
// node
// ------------------------------------------------------------------------
void vsCOLLADALoader::processLOD(char *properties, vsCOLLADANode *node)
{
    char *ptr;
    int rangeCount;
    int lodCount;
    int i;
    char nodeName[64];
    int level[16];
    double range[16];
    vsNode *lodNode[16];
    vsLODAttribute *lodAttr;

    // Initialize the LOD node array and counters
    memset(lodNode, 0, sizeof(lodNode));
    rangeCount = 0;

    // Look for "LOD" substrings and parse the LOD ranges
    ptr = strstr(properties, "LOD");
    while (ptr != NULL)
    {
        // Grab the next LOD level and range
        sscanf(ptr, "LOD%d = %lf", &level[rangeCount], &range[rangeCount]);

        // Get the name of the node we're expecting
        sprintf(nodeName, "LOD%d", rangeCount);

        // Find the child with the correct name and put it in the node array
        for (i = 0; i < node->getChildCount(); i++)
        {
            // See if this is the child we're looking for
            if (strncasecmp(node->getChild(i)->getName(), nodeName,
                    strlen(nodeName)) == 0)
            {
                // Remove the LOD child from its parent and keep track of it
                lodNode[rangeCount] = node->getChild(i);
                node->removeChild(lodNode[rangeCount]);
            }
        }

        // Advance the character pointer 3 letters, then look for the
        // next LOD level
        ptr += 3;
        ptr = strstr(ptr, "LOD");

        // Increment the range counter
        rangeCount++;
    }

    // Now, put the children back in order.  Insert them in reverse range order
    // at the beginning of the child list (in case there are leftover children)
    for (i = rangeCount - 1; i >= 0; i--)
    {
        // If there's no node here, create an empty one
        if (lodNode[i] == NULL)
            lodNode[i] = new vsComponent();
        
        // Insert the node at the beginning of the parent's child list
        node->insertChild(lodNode[i], 0);
    }

    // If we got at least one range, create an LOD attribute for this node
    if (rangeCount > 0)
    {
        lodAttr = new vsLODAttribute();

        // Attach the attribute to the node
        node->addAttribute(lodAttr);

        // Set the LOD ranges
        for (i = 0; i < rangeCount; i++)
           lodAttr->setRangeEnd(level[i], range[i]);
    }
}

// ------------------------------------------------------------------------
// Processes an FCOLLADA technique under a node's extra tag
// ------------------------------------------------------------------------
void vsCOLLADALoader::processNodeFCTechnique(atXMLDocument *doc,
                                             atXMLDocumentNodePtr current,
                                             vsCOLLADANode *node)
{
    atXMLDocumentNodePtr child;
    char *text;
    char *start;
    char *end;
    char *props;

    // Get the first child node of the technique
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See if this is a node we recognize
        if (strcmp(doc->getNodeName(child), "user_properties") == 0)
        {
            // Get the text below this node
            text = doc->getNodeText(doc->getNextChildNode(child));

            if (text != NULL)
            {
                // Trim leading and trailing whitespace from the text
                start = text;
                while (((*start == ' ') || (*start == '\t') ||
                        (*start == '\n') || (*start == '\r')) &&
                       (start < end))
                   start++;
                end = text + strlen(text) - 1;
                while (((*end == ' ') || (*end == '\t') ||
                        (*end == '\n') || (*end == '\r')) &&
                       (end > start))
                   end--;

                // See if there's any string left after trimming
                if (start < end)
                {
                    // Copy the trimmed properties string and process it
                    props = (char *)malloc(end - start + 1);
                    strncpy(props, start, end - start);

                    // Forterra's OLIVE system uses user properties for LOD
                    if (strncmp(props, "LOD", 3) == 0)
                        processLOD(props, node);

                    // Get rid of the properties
                    free(props);
                }
            }
        }

        // Get the next XML node
        child = doc->getNextSiblingNode(child);
    }
}

// ------------------------------------------------------------------------
// Processes an <extra> tag for a node
// ------------------------------------------------------------------------
void vsCOLLADALoader::processNodeExtra(atXMLDocument *doc,
                                       atXMLDocumentNodePtr current,
                                       vsCOLLADANode *node)
{
    atXMLDocumentNodePtr technique;
    atXMLDocumentNodePtr child;
    char *attr;

    // Traverse the children of the extra tag
    technique = doc->getNextChildNode(current);
    while (technique != NULL)
    {
        // This should be a "technique" node
        if (strcmp(doc->getNodeName(technique), "technique") == 0)
        {
            // Check the profile
            attr = doc->getNodeAttribute(technique, "profile");
            if (strcmp(attr, "FCOLLADA") == 0)
            {
                // Process the FCOLLADA technique
                processNodeFCTechnique(doc, technique, node);
            }
            else if (strcmp(attr, "MAYA") == 0)
            {
                // Not supported
            }
            else if (strcmp(attr, "MAX3D") == 0)
            {
                // Not supported
            }
        }

        // Get the next XML node
        technique = doc->getNextSiblingNode(technique);
    }
}

// ------------------------------------------------------------------------
// Processes a <node> XML subtree and instantiates any objects that occur
// within it at the proper place in the scene
// ------------------------------------------------------------------------
void vsCOLLADALoader::processNodePass2(atXMLDocument *doc,
                                       atXMLDocumentNodePtr current,
                                       vsCOLLADANode *parent,
                                       int thisIndex)
{
    char *attr;
    atString id;
    vsCOLLADANode *thisNode;
    atXMLDocumentNodePtr child;
    vsTransformAttribute *xformAttr;
    vsCOLLADATransform *xform;
    atString *rootID;
    bool isSkeletonRoot;
    int childIndex;

    // Get the id for this node
    attr = doc->getNodeAttribute(current, "id");
    if (attr != NULL)
        id.setString(attr);

    // At first, assume that this node is not the root of a skeleton
    isSkeletonRoot = false;

    // Find the vsCOLLADANode in the existing hierarchy (created in pass 1)
    // that corresponds to this node in the COLLADA document
    thisNode = (vsCOLLADANode *)parent->getChild(thisIndex);

    // Initialize our child counter
    childIndex = 0;

    // Traverse the node's children
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See if this is a recognized type
        if (strcmp(doc->getNodeName(child), "node") == 0)
        {
            // Recursively traverse this child node, pass the child's index
            // from this node, so we can keep track of which child we're
            // traversing.  This could be done with node ID's, but nodes
            // are not explicitly required to have ID's
            processNodePass2(doc, child, thisNode, childIndex);

            // Update the child node counter
            childIndex++;
        }
        else if (strcmp(doc->getNodeName(child), "instance_camera") == 0)
        {
            // Cameras not yet supported
        }
        else if (strcmp(doc->getNodeName(child), "instance_controller") == 0)
        {
            // Process the controller instance (and all tasks that go along
            // with it)
            processInstanceController(doc, child, thisNode);
        }
        else if (strcmp(doc->getNodeName(child), "instance_geometry") == 0)
        {
            // Process the geometry instance (and all tasks that go along
            // with it)
            processInstanceGeometry(doc, child, thisNode);
        }
        else if (strcmp(doc->getNodeName(child), "instance_light") == 0)
        {
            // Lights not yet supported
        }
        else if (strcmp(doc->getNodeName(child), "instance_node") == 0)
        {
            // Library nodes not yet supported
            // (I haven't found a document that uses them yet)
        }
        else if (strcmp(doc->getNodeName(child), "extra") == 0)
        {
            // See if there is any information in the <extra> tag that
            // is relevant for us
            processNodeExtra(doc, child, thisNode);
        }

        // Move on to the next child
        child = doc->getNextSiblingNode(child);
    }
}

// ------------------------------------------------------------------------
// Processes the <library_nodes> XML subtree and creates the node library
// and all the node objects it contains
// ------------------------------------------------------------------------
void vsCOLLADALoader::processLibraryNodes(atXMLDocument *doc,
                                          atXMLDocumentNodePtr current)
{
}

// ------------------------------------------------------------------------
// Processes a <visual_scene> XML subtree and creates a visual_scene
// object.  Visual scenes use a node hierarchy to specify how geometry,
// controllers, and materials are organized
// ------------------------------------------------------------------------
void vsCOLLADALoader::processVisualScene(atXMLDocument *doc,
                                         atXMLDocumentNodePtr current)
{
    char *attr;
    atString *sceneID;
    atString sceneName;
    vsCOLLADANode *visualSceneRoot;
    atXMLDocumentNodePtr child;
    int childNum;

    // Get the ID for this scene (make up an ID if there isn't one)
    attr = doc->getNodeAttribute(current, "id");
    if (attr != NULL)
        sceneID = new atString(attr);
    else
        sceneID = new atString(getUnnamedName());

    // Get the name for this scene and name the node (use the ID
    // if there isn't a name)
    attr = doc->getNodeAttribute(current, "name");
    if (attr != NULL)
        sceneName = atString(attr);
    else
        sceneName = atString(*sceneID);

    // Create the root node of the visual scene
    visualSceneRoot = 
        new vsCOLLADANode(*sceneID, sceneName, atString(), VS_CNODE_TYPE_NODE);

    // Add root node of the scene to the visual scene library
    visualSceneRoot->ref();
    visualSceneLibrary->addEntry(sceneID, visualSceneRoot);

    // Traverse the node structure attached to this visual scene and
    // convert it to VESS nodes and attributes
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See if this child is a "node"
        if (strcmp(doc->getNodeName(child), "node") == 0)
        {
            // Pass 1: Traverse and create the node hierarchy
            processNodePass1(doc, child, visualSceneRoot);
        }

        // Try the next node in the visual scene subdocument
        child = doc->getNextSiblingNode(child);
    }

    // Traverse the node structure a second time and handle any
    // object instantiations within the node hierarchy
    child = doc->getNextChildNode(current);
    childNum = 0;
    while (child != NULL)
    {
        // See if this child is a "node"
        if (strcmp(doc->getNodeName(child), "node") == 0)
        {
            // Pass 2: Traverse the node hierarchy and instantiate any
            //         objects
            processNodePass2(doc, child, visualSceneRoot, childNum);
            childNum++;
        }

        // Try the next node in the visual scene subdocument
        child = doc->getNextSiblingNode(child);
    }
}

// ------------------------------------------------------------------------
// Processes the <library_visual_scenes> XML subtree and creates the
// visual scene library and all the objects it contains
// ------------------------------------------------------------------------
void vsCOLLADALoader::processLibraryVisualScenes(atXMLDocument *doc,
                                                 atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr child;

    // Get the first child in the library, and iterate over the geometries
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // Process this node if it's a visual scene
        if (strcmp(doc->getNodeName(child), "visual_scene") == 0)
            processVisualScene(doc, child);

        // Move on to the next node
        child = doc->getNextSiblingNode(child);
    }
}

// ------------------------------------------------------------------------
// Extends the given list with the channels from the given animation
// ------------------------------------------------------------------------
void vsCOLLADALoader::addChannelsFromAnimation(atList *list,
                                               vsCOLLADAAnimation *anim)
{
    int i;
    vsCOLLADAChannel *channel;

    // Iterate over the animation's own channels
    for (i = 0; i < anim->getNumChannels(); i++)
    {
        // Get the i'th channel and add it to the list
        channel = anim->getChannel(i);
        list->addEntry(channel);
    }

    // Iterate over the animation's children and add their channels as well
    for (i = 0; i < anim->getNumChildren(); i++)
        addChannelsFromAnimation(list, anim->getChild(i));
}

// ------------------------------------------------------------------------
// Translates the COLLADA animation objects into equivalent VESS objects
// (vsPathMotion and/or vsPathMotionManager)
// ------------------------------------------------------------------------
void vsCOLLADALoader::buildAnimations(atList *skeletonList,
                                      atList *skelKinList)
{
    atList *animationNameList;
    atList *animationList;
    atString *animationName;
    vsCOLLADAAnimation *animation;
    vsPathMotionManager *pathMotionManager;
    atList *channelList;
    atMap *channelGroupMap;
    vsCOLLADAChannel *channel;
    vsCOLLADAChannelGroup *channelGroup;
    atString targetNodeID;
    vsCOLLADANode *targetNode;
    atList *nodeList;
    atList *groupList;
    vsSkeleton *skeleton;
    vsCOLLADANode *skeletonRoot;
    vsKinematics *kin;
    vsSkeletonKinematics *skelKin;
    vsPathMotion *pathMotion;

    // Get the list of top-level animations in the animation library
    animationNameList = new atList();
    animationList = new atList();
    animationLibrary->getSortedList(animationNameList, animationList);

    // Iterate over the top-level animations
    animationName = (atString *)animationNameList->getFirstEntry();
    animation = (vsCOLLADAAnimation *)animationList->getFirstEntry();
    while (animation != NULL)
    {
        // Collect the list of channels in this animation
        channelList = new atList();
        addChannelsFromAnimation(channelList, animation);

        // Create a mapping from target node to channel group
        channelGroupMap = new atMap();

        // Create a path motion manager to store the set of vsPathMotions
        // that will make up the overall animation
        pathMotionManager = new vsPathMotionManager();
        
        // Iterate over the channels in the list
        channel = (vsCOLLADAChannel *)channelList->getFirstEntry();
        while (channel != NULL)
        {
            // Get the ID of the node that is the target for this channel's
            // animation
            targetNodeID = channel->getTargetNodeID();

            // Look for the target node itself among the skeletons
            targetNode = NULL;
            skeleton = (vsSkeleton *)skeletonList->getFirstEntry();
            while ((targetNode == NULL) && (skeleton != NULL))
            { 
                // Look for the target node in this skeleton
                skeletonRoot = (vsCOLLADANode *)skeleton->getRoot();
                targetNode = skeletonRoot->findNodeByID(targetNodeID);

                // If we didn't find it, try the next skeleton
                if (targetNode == NULL)
                    skeleton = (vsSkeleton *)skeletonList->getNextEntry();
            }

            // Make sure we found the target node
            if (targetNode != NULL)
            {
                // Look up the target node's channel group in the map
                channelGroup = (vsCOLLADAChannelGroup *)
                    channelGroupMap->getValue(targetNode);

                // If there isn't one, create it
                if (channelGroup == NULL)
                {
                    // Create the new channel group
                    channelGroup = new vsCOLLADAChannelGroup(targetNode);

                    // Ref-count the node and group
                    targetNode->ref();
                    channelGroup->ref();

                    // Add the target node/channel group pair to the map
                    channelGroupMap->addEntry(targetNode, channelGroup);
                }

                // Add the channel to the channel group
                channelGroup->addChannel(channel);
            }
            
            // Next channel
            channel = (vsCOLLADAChannel *)channelList->getNextEntry();
        }

        // Now, iterate over the target nodes in the map and instance
        // the channel groups (creating vsPathMotion objects) for each one
        nodeList = new atList();
        groupList = new atList();
        channelGroupMap->getSortedList(nodeList, groupList);
        targetNode = (vsCOLLADANode *)nodeList->getFirstEntry();
        channelGroup = (vsCOLLADAChannelGroup *)groupList->getFirstEntry();
        while (targetNode != NULL)
        {
            // Find the kinematics corresponding to this target node
            kin = NULL;
            skeleton = (vsSkeleton *)skeletonList->getFirstEntry();
            skelKin = (vsSkeletonKinematics *)skelKinList->getFirstEntry();
            while ((kin == NULL) && (skelKin != NULL))
            {
                // Look for the kinematics on this skeleton
                kin = skelKin->getBoneKinematics(targetNode);

                // If we didn't find it, try the next skeleton
                if (kin == NULL)
                {
                    skeleton = (vsSkeleton *)skeletonList->getNextEntry();
                    skelKin = (vsSkeletonKinematics *)
                        skelKinList->getNextEntry();
                }
            }

            // See if we found a kinematics
            if (kin != NULL)
            {
                // Instance the channel group as a vsPathMotion and add it
                // to the overall path motion manager
                pathMotion = channelGroup->instance(kin);
                pathMotionManager->addPathMotion(pathMotion);
            }
else
{
   printf("No kinematics for target node %s\n", targetNode->getID().getString());
}

            // Remove the target node and channel group from the map
            // and lists
            nodeList->removeCurrentEntry();
            groupList->removeCurrentEntry();
            channelGroupMap->removeEntry(targetNode);

            // Unref-delete the node and group (the group should get deleted,
            // but the node should stick around as part of the scene)
            vsObject::unrefDelete(targetNode);
            vsObject::unrefDelete(channelGroup);

            // Next node and channel group
            targetNode = (vsCOLLADANode *)nodeList->getNextEntry();
            channelGroup = (vsCOLLADAChannelGroup *)groupList->getNextEntry();
        }

        // Ref-count the new path motion manager and store it in the animation
        // map (using the animation's name as a key).  We'll make use of these
        // animations when we create the character (if any)
        pathMotionManager->ref();
        animations->addEntry(new atString(*animationName), pathMotionManager);

        // Clean up the lists and map (they should be empty now)
        delete nodeList;
        delete groupList;
        delete channelGroupMap;

        // Move on to the next animation in the library
        animationName = (atString *)animationNameList->getNextEntry();
        animation = (vsCOLLADAAnimation *)animationList->getNextEntry();
    }

    // Flush and delete the lists
    animationNameList->removeAllEntries();
    animationList->removeAllEntries();
    delete animationNameList;
    delete animationList;
}

// ------------------------------------------------------------------------
// Creates a vsCharacter from the skeletons, skins, and animations that
// we found in the COLLADA document
// ------------------------------------------------------------------------
void vsCOLLADALoader::buildCharacter(vsCOLLADANode *sceneRootNode,
                                     atMatrix sceneMat)
{
    vsSkeleton *skeleton;
    atList *skelKinList;
    atMatrix skelXform;
    atMatrix offset;
    vsComponent *parent;
    vsSkin *skin;
    vsComponent *skinRoot;
    vsComponent *skinParent;
    atMatrix skinXform;
    atMatrix identMatrix;
    vsTransformAttribute *skinXformAttr;
    atArray *animationNamesArray;
    atArray *animationsArray;
    atList *animationNamesList;
    atList *animationsList;
    atString *animationName;
    vsPathMotionManager *animation;

    // Try and remove the character's elements from the scene.  This allows
    // the character to be handled separately from the remainder of the
    // scene, which can be useful for some applications.  It also makes
    // things cleaner if multiple copies of the character will be used
    if ((skeletonList->getNumEntries() > 0) &&
        (skinList->getNumEntries() > 0))
    {
        // Create kinematics for the skeletons
        skeleton = (vsSkeleton *)skeletonList->getFirstEntry();
        skelKinList = new atList();
        while (skeleton != NULL)
        {
           // Remove the skeleton from the scene, unless this skeleton is
           // a child of a larger skeleton.  First, traverse from the
           // skeleton root to the top of the scene
           parent = (vsComponent *)skeleton->getRoot()->getParent(0);
           while ((parent != NULL) && (!isSkeletonRoot(parent))) 
               parent = (vsComponent *)parent->getParent(0);

           // If the parent ends up NULL, there are no skeletons rooted
           // above this one, so we can go ahead and remove it
           if (parent == NULL)
           {
               // Get the node's immediate parent
               parent = (vsComponent *)skeleton->getRoot()->getParent(0);

               // This is a separate issue, if the node has no immediate
               // parent, it must be the root of the entire scene, so we
               // can't remove it (most scenes won't be arranged this way)
               if (parent != NULL)
               {
                   // Get the skeleton's global scene transform and apply
                   // it to the skeleton's offset matrix. Also apply the
                   // additional scene matrix (unit scale and up-axis)
                   offset = parent->getGlobalXform();
                   skeleton->setOffsetMatrix(sceneMat * offset);

                   // Now, go ahead and remove the skeleton
                   parent->removeChild(skeleton->getRoot());
               }
           }

           // Create a kinematics for the skeleton, and add it to a list
           skelKinList->addEntry(new vsSkeletonKinematics(skeleton));

           // Next skeleton
           skeleton = (vsSkeleton *)skeletonList->getNextEntry();
        }

        // Remove the skins from the scene before creating the character
        skin = (vsSkin *)skinList->getFirstEntry();
        while (skin != NULL)
        {
            // Get the root component of the skin
            skinRoot = skin->getRootComponent();

            // Remove the skin from the scene
            skinParent = (vsComponent *)skin->getRootComponent()->getParent(0);
            if (skinParent != NULL)
            {
                // Remove the skin from the scene
                skinParent->removeChild(skinRoot);
            }

            // Next skin
            skin = (vsSkin *)skinList->getNextEntry();
        }

        // Build the character's animations
        buildAnimations(skeletonList, skelKinList);

        // Create arrays for the animation names and animations
        animationNamesArray = new atArray();
        animationsArray = new atArray();
        
        // Fill the arrays with the entries in the animations map
        animationNamesList = new atList();
        animationsList = new atList();
        animations->getSortedList(animationNamesList, animationsList);
        animationName = (atString *)animationNamesList->getFirstEntry();
        animation = (vsPathMotionManager *)animationsList->getFirstEntry();
        while (animation != NULL)
        {
            // Transfer the animation from the list to the respective
            // array
            animationsArray->addEntry(animation);
            animationsList->removeCurrentEntry();

            // Clone the name and add it to the other array
            animationNamesArray->addEntry(new atString(*animationName));
            animationNamesList->removeCurrentEntry();

            // Next animation
            animationName = (atString *)animationNamesList->getNextEntry();
            animation = (vsPathMotionManager *)animationsList->getNextEntry();
        }

        // Delete the lists
        delete animationNamesList;
        delete animationsList;

        // Create a character from the skeletons, kinematics, and skins
        sceneCharacter = new vsCharacter(skeletonList, skelKinList,
            skinList, animationNamesArray, animationsArray);
        sceneCharacter->ref();
        sceneCharacter->update();
    }
    else
    {
        // This document doesn't contain a complete character
        sceneCharacter = NULL;
    }
}

// ------------------------------------------------------------------------
// Processes the one and only <scene> XML subtree.  This specifies which
// visual scene in the visual scene library will be instantiated when
// this COLLADA document is loaded.  Typically, there is only one visual
// scene in the visual scene library, but it is possible to have multiple
// visual scenes in a single COLLADA document
// ------------------------------------------------------------------------
void vsCOLLADALoader::processScene(atXMLDocument *doc,
                                   atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr child;
    char *attr;
    vsCOLLADANode *sceneRootNode;
    vsTransformAttribute *xform;
    atMatrix scaleMat;
    atMatrix sceneMat;

    // Traverse the scene node's children
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See if this is a visual scene instance
        if (strcmp(doc->getNodeName(child), "instance_visual_scene") == 0)
        {
            // Only one visual scene instance is allowed per document
            if (sceneRoot == NULL)
            {
                // Look up the visual scene in the library, clone it, and
                // use the cloned instance as the document's scene
                attr = doc->getNodeAttribute(child, "url");
                sceneRootNode = getVisualScene(atString(attr));
            }
        }

        // Try the next node
        child = doc->getNextSiblingNode(child);
    }

    // Scale and up-axis transforms will be needed by several instanced
    // components, so compute an overal scene matrix that will handle these
    // now
    scaleMat.setScale(unitScale, unitScale, unitScale);
    sceneMat = scaleMat * upAxisTransform;

    // If the necessary components for an animated character exist, go ahead
    // and construct the character now (pass the scene matrix, as it applies
    // to the character as well)
    buildCharacter(sceneRootNode, sceneMat);

    // Finally, clone the COLLADA scene to produce the final VESS scene
    if (sceneRootNode != NULL)
    {
        sceneRoot = (vsComponent *)sceneRootNode->cloneTree();

        // Apply a transform to scale and orient the scene properly
        xform = new vsTransformAttribute();
        xform->setPreTransform(sceneMat);
        sceneRoot->addAttribute(xform);

        // Reference count the scene for now (we'll release the
        // reference and clean up later)
        sceneRoot->ref();
    }
    else
        sceneRoot = NULL;
}
 
// ------------------------------------------------------------------------
// Processes an <asset> tag that can be associated with many types of
// objects in a COLLADA document.  The primary asset tag occurs at the
// top level of the document and specifies several important items, such
// as the units used by the geometry in the document.
// ------------------------------------------------------------------------
void vsCOLLADALoader::processAsset(atXMLDocument *doc,
                                   atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr child;
    atXMLDocumentNodePtr contrib;
    char *text;
    atQuat tempQuat;

    // Initialize the up-axis transform matrix to a -Z forward, Y-up
    // orientation (this is the default COLLADA orientation)
    tempQuat.setVecsRotation(
        atVector(0.0, 0.0, -1.0), atVector(0.0, 1.0, 0.0),
        atVector(0.0, 1.0, 0.0), atVector(0.0, 0.0, 1.0));
    upAxisTransform.setQuatRotation(tempQuat);

    // Descend into the asset information
    child = doc->getNextChildNode(current);

    // Parse the desired asset information
    while (child != NULL)
    {
        // See if this is a node we care about
        if (strcmp(doc->getNodeName(child), "contributor") == 0)
        {
            // Parse the contributor information
            contrib = doc->getNextChildNode(child);
            while (contrib != NULL)
            {
                // Look for contributor information we recognize
                if (strcmp(doc->getNodeName(contrib), "author") == 0)
                { 
                    // This is the author's name
                } 

                // Try the next node
                contrib = doc->getNextSiblingNode(contrib);
            }
        }
        else if (strcmp(doc->getNodeName(child), "created") == 0)
        {
            // Print the date/time this file was created
        }
        else if (strcmp(doc->getNodeName(child), "modified") == 0)
        {
            // Print the date/time this file was last modified
        }
        else if (strcmp(doc->getNodeName(child), "unit") == 0)
        {
            // Get the scale factor to convert this file's units to meters
            unitScale = atof(doc->getNodeAttribute(child, "meter"));

            // Print the units information
        }
        else if (strcmp(doc->getNodeName(child), "up_axis") == 0)
        {
            // Print the up axis
            text = doc->getNodeText(doc->getNextChildNode(child));
            
            // Get the transform that will orient the scene properly
            if (strcmp(text, "Z_UP") == 0)
            {
                // This is the native orientation, so set an indentity
                // matrix
                upAxisTransform.setIdentity();
            }
            else if (strcmp(text, "Y_UP") == 0)
            {
                // Set up a rotation to transform a -Z-forward, Y-up scene
                // to Y-forward, Z-up
                tempQuat.setVecsRotation(
                    atVector(0.0, 0.0, -1.0), atVector(0.0, 1.0, 0.0),
                    atVector(0.0, 1.0, 0.0), atVector(0.0, 0.0, 1.0));
                upAxisTransform.setQuatRotation(tempQuat);
            }
            else if (strcmp(text, "X_UP") == 0)
            {
                // Set up a rotation to transform a -Z-forward, X-up scene
                // to Y-forward, Z-up
                tempQuat.setVecsRotation(
                    atVector(0.0, 0.0, -1.0), atVector(1.0, 0.0, 0.0),
                    atVector(0.0, 1.0, 0.0), atVector(0.0, 0.0, 1.0));
                upAxisTransform.setQuatRotation(tempQuat);
            }
        }

        // Move on to the next node
        child = doc->getNextSiblingNode(child);
    }
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
        // Print an error and bail
        printf("vsCOLLADALoader::parseFile:  File %s not found, or not a "
            "COLLADA file\n", filename);
        return;
    }
    else
    {
        // Process the document's data
        // Pass 1:  Lights, Cameras, Geometry, and Images
        rootNode = doc->getRootNode();
        current = doc->getNextChildNode(rootNode);
        while (current != NULL)
        {
            // Figure out which chunk of data this is
            if (strcmp(doc->getNodeName(current), "asset") == 0)
            {
                // Process the asset metadata
                processAsset(doc, current);
            }
            else if (strcmp(doc->getNodeName(current), "library_cameras") == 0)
            {
                // Process the cameras
                processLibraryCameras(doc, current);
            }
            else if (strcmp(doc->getNodeName(current),
                            "library_geometries") == 0)
            {
                // Process the geometry data
                processLibraryGeometries(doc, current);
            }
            else if (strcmp(doc->getNodeName(current), "library_images") == 0)
            {
                // Process the images
                processLibraryImages(doc, current);
            }
            else if (strcmp(doc->getNodeName(current), "library_lights") == 0)
            {
                // Process the lights
                processLibraryLights(doc, current);
            }

            // Move on to the next node
            current = doc->getNextSiblingNode(current);
        }

        // Pass 2:  Effects and Controllers
        rootNode = doc->getRootNode();
        current = doc->getNextChildNode(rootNode);
        while (current != NULL)
        {
            // Figure out which chunk of data this is
            if (strcmp(doc->getNodeName(current), "library_controllers") == 0)
            {
                // Process the animation controllers
                processLibraryControllers(doc, current);
            }
            else if (strcmp(doc->getNodeName(current), "library_effects") == 0)
            {
                // Process the effects
                processLibraryEffects(doc, current);
            }

            // Move on to the next node
            current = doc->getNextSiblingNode(current);
        }

        // Pass 3:  Materials
        rootNode = doc->getRootNode();
        current = doc->getNextChildNode(rootNode);
        while (current != NULL)
        {
            // Figure out which chunk of data this is
            if (strcmp(doc->getNodeName(current), "library_materials") == 0)
            {
                // Process the materials
                processLibraryMaterials(doc, current);
            }

            // Move on to the next node
            current = doc->getNextSiblingNode(current);
        }

        // Pass 4:  Nodes
        rootNode = doc->getRootNode();
        current = doc->getNextChildNode(rootNode);
        while (current != NULL)
        {
            // Figure out which chunk of data this is
            if (strcmp(doc->getNodeName(current), "library_nodes") == 0)
            {
                // Process the nodes (subgraphs that are instanced in visual
                // scenes)
                processLibraryNodes(doc, current);
            }

            // Move on to the next node
            current = doc->getNextSiblingNode(current);
        }

        // Pass 5:  Visual Scenes
        rootNode = doc->getRootNode();
        current = doc->getNextChildNode(rootNode);
        while (current != NULL)
        {
            // Figure out which chunk of data this is
            if (strcmp(doc->getNodeName(current), "library_visual_scenes") == 0)
            {
                // Assemble the visual scenes (usually only one, but it's
                // possible to have multiple visual scenes)
                processLibraryVisualScenes(doc, current);
            }

            // Move on to the next node
            current = doc->getNextSiblingNode(current);
        }

        // Pass 6:  Animations
        rootNode = doc->getRootNode();
        current = doc->getNextChildNode(rootNode);
        while (current != NULL)
        {
            // Figure out which chunk of data this is
            if (strcmp(doc->getNodeName(current), "library_animations") == 0)
            {
                // Process the animation data
                processLibraryAnimations(doc, current);
            }

            // Move on to the next node
            current = doc->getNextSiblingNode(current);
        }

        // Pass 7:  Animation clips
        rootNode = doc->getRootNode();
        current = doc->getNextChildNode(rootNode);
        while (current != NULL)
        {
            // Figure out which chunk of data this is
            if (strcmp(doc->getNodeName(current),
                            "library_animation_clips") == 0)
            {
                // Process the animation clips
                processLibraryAnimationClips(doc, current);
            }

            // Move on to the next node
            current = doc->getNextSiblingNode(current);
        }

        // Pass 8:  The final scene
        rootNode = doc->getRootNode();
        current = doc->getNextChildNode(rootNode);
        while (current != NULL)
        {
            // Figure out which chunk of data this is
            if (strcmp(doc->getNodeName(current), "scene") == 0)
            {
                // Process the actual scene that we're loading and assembling
                processScene(doc, current);
            }

            // Move on to the next node
            current = doc->getNextSiblingNode(current);
        }
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
    // Return the scene created in this document
    if (sceneRoot != NULL)
        return (vsComponent *)sceneRoot->cloneTree();
    else
        return NULL;
}

// ------------------------------------------------------------------------
// Returns a clone of the character we found in the scene (if any).  For
// now, we assume there is only one character instanced in the scene
// ------------------------------------------------------------------------
vsCharacter *vsCOLLADALoader::getCharacter()
{
    // Return a clone of the scene's character (if we found one)
    if (sceneCharacter)
        return sceneCharacter->clone();
    else
        return NULL;
}

