
#include "vsCOLLADATextureSet.h++"

// ------------------------------------------------------------------------
// Creates a texture set, setting up the texture list
// ------------------------------------------------------------------------
vsCOLLADATextureSet::vsCOLLADATextureSet()
{
    // Initialize the texture list
    textureList = new vsList();
}

// ------------------------------------------------------------------------
// Cleans up a texture set, unreferencing all textures added to the set
// ------------------------------------------------------------------------
vsCOLLADATextureSet::~vsCOLLADATextureSet()
{
    delete textureList;
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsCOLLADATextureSet::getClassName()
{
    return "vsCOLLADATextureSet";
}

// ------------------------------------------------------------------------
// Clone this texture set
// ------------------------------------------------------------------------
vsCOLLADATextureSet *vsCOLLADATextureSet::clone()
{
    vsCOLLADATextureSet *newSet;
    vsTextureAttribute *tex;

    // Create a new texture set
    newSet = new vsCOLLADATextureSet();

    // Clone all of the textures in this set
    tex = (vsTextureAttribute *)textureList->getFirstEntry();
    while (tex != NULL)
    {
        // Add a clone of this texture to the new set
        newSet->textureList->addEntry(tex->clone());

        // Next texture
        tex = (vsTextureAttribute *)textureList->getNextEntry();
    }

    // Return the cloned set
    return newSet;
}

// ------------------------------------------------------------------------
// Adds a texture to the texture set (checking for duplicates)
// ------------------------------------------------------------------------
void vsCOLLADATextureSet::addTexture(vsTextureAttribute *newTexture)
{
    vsTextureAttribute *tex;

    // Check the list of textures to make sure we don't already have this
    // one
    tex = (vsTextureAttribute *)textureList->getFirstEntry();
    while ((tex != NULL) && (tex != newTexture))   
        tex = (vsTextureAttribute *)textureList->getNextEntry();

    // See if we already have this texture
    if (tex == NULL)
    {
        // We didn't find the texture, so add it to our list
        textureList->addEntry(newTexture);
    }
    else
    {
        // Print a warning
        printf("vsCOLLADATextureSet::addTexture:  Set already contains this "
            "texture!\n");
    }
}

// ------------------------------------------------------------------------
// Returns the list of textures in this set
// ------------------------------------------------------------------------
vsList *vsCOLLADATextureSet::getTextureList()
{
    // Return the texture list
    return textureList;
}

