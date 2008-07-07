
#include "vsCOLLADAFixedEffect.h++"
#include "vsCOLLADATextureSet.h++"

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsCOLLADAFixedEffect::vsCOLLADAFixedEffect(atString id)
                    : vsCOLLADAEffect(id)
{
    // Initialize the material attribute and texture array
    material = NULL;
    textures = new atMap();
}

// ------------------------------------------------------------------------
// Destructor.  Cleans up all material and texture objects and containers
// ------------------------------------------------------------------------
vsCOLLADAFixedEffect::~vsCOLLADAFixedEffect()
{
    atList *keys;
    atString *key;
    atList *values;
    vsTextureAttribute *tex;

    // Delete the material attribute (if any)
    if (material != NULL)
        vsObject::unrefDelete(material);

    // Delete the map of textures
    delete textures;
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsCOLLADAFixedEffect::getClassName()
{
    return "vsCOLLADAFixedEffect";
}

// ------------------------------------------------------------------------
// Clone the effect
// ------------------------------------------------------------------------
vsCOLLADAFixedEffect *vsCOLLADAFixedEffect::clone(atString cloneID)
{
    vsCOLLADAFixedEffect *newEffect;
    vsCOLLADAEffectParameter *param;
    vsMaterialAttribute *mat;
    atList *texCoords;
    atList *textureSets;
    vsCOLLADATextureSet *texSet;
    atString *texCoordID;

    // Create a new effect
    newEffect = new vsCOLLADAFixedEffect(cloneID);

    // Copy the parameters
    param = (vsCOLLADAEffectParameter *)effectParameters->getFirstEntry();
    while (param != NULL)
    {
        // Add a clone of the parameter to the new effect
        newEffect->addParameter(param->clone());

        // Get the next parameter
        param = (vsCOLLADAEffectParameter *)effectParameters->getNextEntry();
    }

    // Clone the material attribute
    if (material != NULL)
    {
        // Create a new material attribute for the new effect
        mat = (vsMaterialAttribute *)material->clone();

        // Set the new effect's material
        newEffect->setMaterial(mat);
    }
    else
    {
        // Just set the new effect's material to NULL
        newEffect->setMaterial(NULL);
    }

    // Get the texture attributes and texture coordinate list ids from the
    // textures map
    texCoords = new atList();
    textureSets = new atList();
    textures->getSortedList(texCoords, textureSets);

    // Create new lists for the textures
    texCoordID = (atString *)texCoords->getFirstEntry();
    texSet = (vsCOLLADATextureSet *)textureSets->getFirstEntry();
    while ((texCoordID != NULL) && (texSet != NULL))
    {
        // Add a clone of this id/texture set pair to the new effect's textures
        // map
        newEffect->textures->
            addEntry(new atString(*texCoordID), texSet->clone());

        // Move on
        texCoordID = (atString *)texCoords->getNextEntry();
        texSet = (vsCOLLADATextureSet *)textureSets->getNextEntry();
    }

    // Get rid of the lists
    texCoords->removeAllEntries();
    delete texCoords;
    textureSets->removeAllEntries();
    delete textureSets;

    // Finally, return the new effect
    return newEffect;
}

// ------------------------------------------------------------------------
// Return the type of effect (fixed-function)
// ------------------------------------------------------------------------
vsCOLLADAEffectType vsCOLLADAFixedEffect::getType()
{
    return VS_COLLADA_EFFECT_FIXED;
}

// ------------------------------------------------------------------------
// Sets the given material attribute as the material attribute to be used
// for this effect (any geometry that instantiates this effect will get
// a clone of this material attribute)
// ------------------------------------------------------------------------
void vsCOLLADAFixedEffect::setMaterial(vsMaterialAttribute *mat)
{
    // If we already have a material attribute, delete the existing one
    // in favor of the new one
    if (material != NULL)
        vsObject::unrefDelete(material);

    // Store the new material
    material = mat;
    if (material != NULL)
        material->ref();
}

// ------------------------------------------------------------------------
// Returns the material attribute used by this effect
// ------------------------------------------------------------------------
vsMaterialAttribute *vsCOLLADAFixedEffect::getMaterial()
{
    return material;
}

// ------------------------------------------------------------------------
// Adds the given texture attribute as one of the textures used by this
// effect.  The position in the textures array will be determined by the
// texture attribute's texture unit setting
// ------------------------------------------------------------------------
void vsCOLLADAFixedEffect::addTexture(atString texCoordName,
                                      vsTextureAttribute *tex)
{
    atString *myTexCoord;
    vsCOLLADATextureSet *texSet;

    // If we already have a texture attribute using these texture coordinates,
    // delete the existing one in favor of the new one
    if (textures->containsKey(&texCoordName))
    {
        // Get the texture set
        texSet = (vsCOLLADATextureSet *)textures->getValue(&texCoordName);

        // Add the new texture to the texture set
        texSet->addTexture(tex);
    }
    else
    {
        // Create a new atString with the given texture coordinate name
        // so we can store it in the map
        myTexCoord = new atString(texCoordName);

        // Create a new texture set
        texSet = new vsCOLLADATextureSet();

        // Add the texture to the texture set
        texSet->addTexture(tex);

        // Add the texture coordinate key and the texture set to the 
        // textures map
        textures->addEntry(myTexCoord, texSet);
    }
}

// ------------------------------------------------------------------------
// Returns the texture attribute corresponding to the given texture
// coordinate list ID
// ------------------------------------------------------------------------
atList *vsCOLLADAFixedEffect::getTextures(atString texCoordID)
{
    vsCOLLADATextureSet *texSet;

    // Get the texture set from the textures map, and return the list
    // of textures
    texSet = (vsCOLLADATextureSet *)textures->getValue(&texCoordID);
    if (texSet == NULL)
        return NULL;
    else
        return texSet->getTextureList();
}

// ------------------------------------------------------------------------
// Returns the texture attribute corresponding to the given sampler
// parameter
// ------------------------------------------------------------------------
vsTextureAttribute *vsCOLLADAFixedEffect::getTextureFromParam(atString paramID)
{
    vsCOLLADAEffectParameter *surfaceParam;
    vsCOLLADAEffectParameter *samplerParam;
    vsTextureAttribute *surfaceTex;
    vsTextureAttribute *samplerTex;
    int mode;
    vsTextureAttribute *returnTex;
    char name[256];

    // Get the parameter, or bail if we can't find it
    samplerParam = getParameter(paramID);
    if (samplerParam == NULL)
         return NULL;

    // If this isn't a TEXTURE_2D parameter, bail
    if (samplerParam->getType() != VS_COLLADA_TEXTURE_2D)
        return NULL;

    // Get the surface parameter that matches this sampler
    surfaceParam = getParameter(samplerParam->getSourceSurfaceID());

    // If there is no surface assigned to this sampler, we can't produce
    // a valid texture, so bail
    if (surfaceParam == NULL)
        return NULL;

    // If the sampler doesn't have a texture at all, bail
    samplerTex = samplerParam->getTexture();
    if (samplerTex == NULL)
        return NULL;

    // If the surface doesn't have a texture at all, bail
    surfaceTex = surfaceParam->getTexture();
    if (surfaceTex == NULL)
        return NULL;

    // Reference the two textures while we're working with them
    surfaceTex->ref();
    samplerTex->ref();

    // Check the texture attributes of both the surface and sampler. If
    // the two textures are equivalent, we're done
    if (samplerTex->isEquivalent(surfaceTex))
        return samplerTex;
   
    // Otherwise, we need to make them equal (basically, we need the surface's
    // image with the sampler's texture environment and parameters)
    mode = samplerTex->getBoundaryMode(VS_TEXTURE_DIRECTION_S);
    surfaceTex->setBoundaryMode(VS_TEXTURE_DIRECTION_S, mode);
    mode = samplerTex->getBoundaryMode(VS_TEXTURE_DIRECTION_T);
    surfaceTex->setBoundaryMode(VS_TEXTURE_DIRECTION_T, mode);
    mode = samplerTex->getMagFilter();
    surfaceTex->setMagFilter(mode);
    mode = samplerTex->getMinFilter();
    surfaceTex->setMinFilter(mode);
    mode = samplerTex->getApplyMode();
    surfaceTex->setApplyMode(mode);
    mode = samplerTex->getGenMode();
    surfaceTex->setGenMode(mode);

    // Set the newly-configured surface texture as the sampler parameter's
    // texture, so the surface and sampler match
    samplerParam->set(surfaceTex);

    // Get rid of the old sampler texture
    vsObject::unrefDelete(samplerTex);

    // Create a clone of the surface texture (this will likely end up in the
    // textures map)
    returnTex = (vsTextureAttribute *)surfaceTex->clone();
   
    // Unreference the original surface texture and return the clone
    vsObject::unrefDelete(surfaceTex);
    return returnTex;
}

