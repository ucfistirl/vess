
#include "vsCOLLADATextureSet.h++"

// ------------------------------------------------------------------------
// Creates a texture set, setting up the texture list
// ------------------------------------------------------------------------
vsCOLLADATextureSet::vsCOLLADATextureSet()
{
    // Initialize the texture list
    textureList = new atList();
}

// ------------------------------------------------------------------------
// Cleans up a texture set, unreferencing all textures added to the set
// ------------------------------------------------------------------------
vsCOLLADATextureSet::~vsCOLLADATextureSet()
{
    vsObject *tex;

    // Clean up the texture list
    tex = (vsObject *)textureList->getFirstEntry();
    while (tex != NULL)
    {
        // Remove the entry from the list
        textureList->removeCurrentEntry();

        // Unreference and delete the texture
        vsObject::unrefDelete(tex);

        // Move on to the next entry
        tex = (vsObject *)textureList->getNextEntry();
    }
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
        newTexture->ref();
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
atList *vsCOLLADATextureSet::getTextureList()
{
    // Return the texture list
    return textureList;
}

