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
//    VESS Module:  vsGraphicsState.c++
//
//    Description:  Object used internally by VESS to track the current
//                  graphics state during a scene graph traversal
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsGraphicsState.h++"

vsGraphicsState *vsGraphicsState::classInstance = NULL;

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsGraphicsState::vsGraphicsState() : lightAttrList(1, 1)
{
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsGraphicsState::~vsGraphicsState()
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsGraphicsState::getClassName()
{
    return "vsGraphicsState";
}

// ------------------------------------------------------------------------
// Static function
// Returns the currently active vsGraphicsState object
// ------------------------------------------------------------------------
vsGraphicsState *vsGraphicsState::getInstance()
{
    // Create a new instance if none exists
    if (!classInstance)
    {
        classInstance = new vsGraphicsState();
        classInstance->ref();
    }

    // Return the singleton instance
    return classInstance;
}

// ------------------------------------------------------------------------
// Static function
// Deletes the currently active vsGraphicsState object
// ------------------------------------------------------------------------
void vsGraphicsState::deleteInstance()
{
    // If the instance exists, destroy it
    if (classInstance)
        vsObject::unrefDelete(classInstance);

    // Reset the instance pointer to NULL
    classInstance = NULL;
}

// ------------------------------------------------------------------------
// Clears the internal graphics state and sets the graphics library state
// to default values
// ------------------------------------------------------------------------
void vsGraphicsState::clearState()
{
    int unit;

    // Clear the 'current attribute' values
    backfaceAttr = NULL;
    fogAttr = NULL;
    materialAttr = NULL;
    shaderAttr = NULL;
    shadingAttr = NULL;
    for (unit = 0; unit < VS_MAXIMUM_TEXTURE_UNITS; unit++)
    {
        textureAttr[unit] = NULL;
        textureCubeAttr[unit] = NULL;
        textureRectAttr[unit] = NULL;
    }
    transparencyAttr = NULL;
    wireframeAttr = NULL;
    lightAttrCount = 0;

    // Clear the 'attribute lock' pointers
    backfaceLock = NULL;
    fogLock = NULL;
    materialLock = NULL;
    shaderLock = NULL;
    shadingLock = NULL;
    for (unit = 0; unit < VS_MAXIMUM_TEXTURE_UNITS; unit++)
    {
        textureLock[unit] = NULL;
    }
    transparencyLock = NULL;
    wireframeLock = NULL;
}

// ------------------------------------------------------------------------
// Internal function
// Packages the current state into a Performer GeoState
// ------------------------------------------------------------------------
void vsGraphicsState::applyState(pfGeoState *state)
{
    int loop;
    pfLight **lightList;
    pfGStateFuncType preFunc, postFunc;
    void *data;
    
    // Start by setting the geostate to inherit everything from globals
    state->setInherit(PFSTATE_ALL);

    // Call the backface attribute (if any) to make its state changes
    if (backfaceAttr)
        backfaceAttr->setState(state);

    // Call the fog attribute (if any) to make its state changes
    if (fogAttr)
        fogAttr->setState(state);

    // Call the material attribute (if any) to make its state changes
    if (materialAttr)
        materialAttr->setState(state);

    // Call the shader attribute (if any) to make its state changes
    if (shaderAttr)
        shaderAttr->setState(state);

    // Call the shading attribute (if any) to make its state changes
    if (shadingAttr)
        shadingAttr->setState(state);

    for (loop = 0; loop < VS_MAXIMUM_TEXTURE_UNITS; loop++)
    {
        // Call the texture attribute (if any) to make its state changes
        if (textureAttr[loop])
            textureAttr[loop]->setState(state);

        // Call the texture cube attribute (if any) to make its state changes
        if (textureCubeAttr[loop])
            textureCubeAttr[loop]->setState(state);

        // Call the texture rectangle attribute to make its state changes
        if (textureRectAttr[loop])
            textureRectAttr[loop]->setState(state);
    }

    // Call the transparency attribute (if any) to make its state changes
    if (transparencyAttr)
        transparencyAttr->setState(state);

    // Call the wireframe attribute (if any) to make its state changes
    if (wireframeAttr)
        wireframeAttr->setState(state);

    // Call each light attribute (if any) to make its state changes
    state->getFuncs(&preFunc, &postFunc, &data);
    lightList = (pfLight **)data;
    for (loop = 0; loop < PF_MAX_LIGHTS; loop++)
        lightList[loop] = NULL;
    for (loop = 0; loop < lightAttrCount; loop++)
        ((vsLightAttribute *)(lightAttrList[loop]))->setState(state);
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired backface state
// ------------------------------------------------------------------------
void vsGraphicsState::setBackface(vsBackfaceAttribute *newAttrib)
{
    // Set the current attribute, if it's not locked
    if (!backfaceLock)
        backfaceAttr = newAttrib;
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired fog state
// ------------------------------------------------------------------------
void vsGraphicsState::setFog(vsFogAttribute *newAttrib)
{
    // Set the current attribute, if it's not locked
    if (!fogLock)
        fogAttr = newAttrib;
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired material state
// ------------------------------------------------------------------------
void vsGraphicsState::setMaterial(vsMaterialAttribute *newAttrib)
{
    // Set the current attribute, if it's not locked
    if (!materialLock)
        materialAttr = newAttrib;
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired shader state
// ------------------------------------------------------------------------
void vsGraphicsState::setShader(vsShaderAttribute *newAttrib)
{
    // Set the current attribute, if it's not locked
    if (!shaderLock)
        shaderAttr = newAttrib;
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired shading state
// ------------------------------------------------------------------------
void vsGraphicsState::setShading(vsShadingAttribute *newAttrib)
{
    // Set the current attribute, if it's not locked
    if (!shadingLock)
        shadingAttr = newAttrib;
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired texture unit's texture state
// ------------------------------------------------------------------------
void vsGraphicsState::setTexture(unsigned int unit,
                                 vsTextureAttribute *newAttrib)
{
    // Set the current attribute, if it's not locked
    if (!textureLock[unit])
    {
        textureAttr[unit] = newAttrib;
        textureCubeAttr[unit] = NULL;
        textureRectAttr[unit] = NULL;
    }
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired  texture unit's texture
// cube state
// ------------------------------------------------------------------------
void vsGraphicsState::setTextureCube(unsigned int unit,
                                     vsTextureCubeAttribute *newAttrib)
{
    // Set the current attribute, if it's not locked
    if (!textureLock[unit])
    {
        textureAttr[unit] = NULL;
        textureCubeAttr[unit] = newAttrib;
        textureRectAttr[unit] = NULL;
    }
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired  texture unit's texture
// rectangle state
// ------------------------------------------------------------------------
void vsGraphicsState::setTextureRect(unsigned int unit,
                                     vsTextureRectangleAttribute *newAttrib)
{
    // Set the current attribute, if it's not locked
    if (!textureLock[unit])
    {
        textureAttr[unit] = NULL;
        textureCubeAttr[unit] = NULL;
        textureRectAttr[unit] = newAttrib;
    }
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired transparency state
// ------------------------------------------------------------------------
void vsGraphicsState::setTransparency(vsTransparencyAttribute *newAttrib)
{
    // Set the current attribute, if it's not locked
    if (!transparencyLock)
        transparencyAttr = newAttrib;
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired wireframe state
// ------------------------------------------------------------------------
void vsGraphicsState::setWireframe(vsWireframeAttribute *newAttrib)
{
    // Set the current attribute, if it's not locked
    if (!wireframeLock)
        wireframeAttr = newAttrib;
}

// ------------------------------------------------------------------------
// Adds the light attribute to the graphics state list of local lights
// ------------------------------------------------------------------------
void vsGraphicsState::addLight(vsLightAttribute *lightAttrib)
{
    // Add the specified light attribute to our light attribute list,
    // and increment the list size
    lightAttrList[lightAttrCount++] = lightAttrib;
}

// ------------------------------------------------------------------------
// Removes the light attribute from the graphics state list of local lights
// ------------------------------------------------------------------------
void vsGraphicsState::removeLight(vsLightAttribute *lightAttrib)
{
    int loop;
    
    // Search for the specified light attribute in our list
    for (loop = 0; loop < lightAttrCount; loop++)
        if (lightAttrib == lightAttrList[loop])
        {
            // If found, remove the attribute from the list by copying
            // the last entry in the list over the attribute to be removed,
            // and decrement the list size
            lightAttrList[loop] = lightAttrList[--lightAttrCount];
            return;
        }
}

// ------------------------------------------------------------------------
// Retrieves the attribute that contains the current backface state
// ------------------------------------------------------------------------
vsBackfaceAttribute *vsGraphicsState::getBackface()
{
    return backfaceAttr;
}

// ------------------------------------------------------------------------
// Retrieves the attribute that contains the current fog state
// ------------------------------------------------------------------------
vsFogAttribute *vsGraphicsState::getFog()
{
    return fogAttr;
}

// ------------------------------------------------------------------------
// Retrieves the attribute that contains the current material state
// ------------------------------------------------------------------------
vsMaterialAttribute *vsGraphicsState::getMaterial()
{
    return materialAttr;
}

// ------------------------------------------------------------------------
// Retrieves the attribute that contains the current shader state
// ------------------------------------------------------------------------
vsShaderAttribute *vsGraphicsState::getShader()
{
    return shaderAttr;
}

// ------------------------------------------------------------------------
// Retrieves the attribute that contains the current shading state
// ------------------------------------------------------------------------
vsShadingAttribute *vsGraphicsState::getShading()
{
    return shadingAttr;
}

// ------------------------------------------------------------------------
// Retrieves the attribute that contains the current texture state
// for the given texture unit
// ------------------------------------------------------------------------
vsTextureAttribute *vsGraphicsState::getTexture(unsigned int unit)
{
    return textureAttr[unit];
}

// ------------------------------------------------------------------------
// Retrieves the attribute that contains the current texture cube state
// for the given texture unit
// ------------------------------------------------------------------------
vsTextureCubeAttribute *vsGraphicsState::getTextureCube(unsigned int unit)
{
    return textureCubeAttr[unit];
}

// ------------------------------------------------------------------------
// Retrieves the attribute that contains the current texture cube state
// for the given texture unit
// ------------------------------------------------------------------------
vsTextureRectangleAttribute *vsGraphicsState::getTextureRect(unsigned int unit)
{
    return textureRectAttr[unit];
}

// ------------------------------------------------------------------------
// Retrieves the attribute that contains the current transparency state
// ------------------------------------------------------------------------
vsTransparencyAttribute *vsGraphicsState::getTransparency()
{
    return transparencyAttr;
}

// ------------------------------------------------------------------------
// Retrieves the attribute that contains the current wireframe state
// ------------------------------------------------------------------------
vsWireframeAttribute *vsGraphicsState::getWireframe()
{
    return wireframeAttr;
}

// ------------------------------------------------------------------------
// Retrieves the attribute that contains the specified local light
// ------------------------------------------------------------------------
vsLightAttribute *vsGraphicsState::getLight(int index)
{
    // Bounds check
    if ((index < 0) || (index >= lightAttrCount))
        return NULL;

    // Return the desired light
    return (vsLightAttribute *)(lightAttrList[index]);
}

// ------------------------------------------------------------------------
// Retrieves the current number of active local lights
// ------------------------------------------------------------------------
int vsGraphicsState::getLightCount()
{
    return lightAttrCount;
}

// ------------------------------------------------------------------------
// Locks the current backface attribute, using the given address as a
// 'key'. The backface attribute cannot be changed again until it is
// unlocked with this key address.
// ------------------------------------------------------------------------
void vsGraphicsState::lockBackface(void *lockAddr)
{
    // If there's no existing lock, set the lock value
    if (!backfaceLock)
        backfaceLock = lockAddr;
}

// ------------------------------------------------------------------------
// Locks the current fog attribute, using the given address as a
// 'key'. The fog attribute cannot be changed again until it is
// unlocked with this key address.
// ------------------------------------------------------------------------
void vsGraphicsState::lockFog(void *lockAddr)
{
    // If there's no existing lock, set the lock value
    if (!fogLock)
        fogLock = lockAddr;
}

// ------------------------------------------------------------------------
// Locks the current material attribute, using the given address as a
// 'key'. The material attribute cannot be changed again until it is
// unlocked with this key address.
// ------------------------------------------------------------------------
void vsGraphicsState::lockMaterial(void *lockAddr)
{
    // If there's no existing lock, set the lock value
    if (!materialLock)
        materialLock = lockAddr;
}

// ------------------------------------------------------------------------
// Locks the current shader attribute, using the given address as a
// 'key'. The shader attribute cannot be changed again until it is
// unlocked with this key address.
// ------------------------------------------------------------------------
void vsGraphicsState::lockShader(void *lockAddr)
{
    // If there's no existing lock, set the lock value
    if (!shaderLock)
        shaderLock = lockAddr;
}

// ------------------------------------------------------------------------
// Locks the current shading attribute, using the given address as a
// 'key'. The shading attribute cannot be changed again until it is
// unlocked with this key address.
// ------------------------------------------------------------------------
void vsGraphicsState::lockShading(void *lockAddr)
{
    // If there's no existing lock, set the lock value
    if (!shadingLock)
        shadingLock = lockAddr;
}

// ------------------------------------------------------------------------
// Locks the texture attribute for the given texture unit,  using the
// given address as a 'key'. The texture attribute cannot be changed again
// until it is unlocked with this key address.
// ------------------------------------------------------------------------
void vsGraphicsState::lockTexture(unsigned int unit, void *lockAddr)
{
    // If there's no existing lock, set the lock value
    if (!textureLock[unit])
        textureLock[unit] = lockAddr;
}

// ------------------------------------------------------------------------
// Locks the current transparency attribute, using the given address as a
// 'key'. The transparency attribute cannot be changed again until it is
// unlocked with this key address.
// ------------------------------------------------------------------------
void vsGraphicsState::lockTransparency(void *lockAddr)
{
    // If there's no existing lock, set the lock value
    if (!transparencyLock)
        transparencyLock = lockAddr;
}

// ------------------------------------------------------------------------
// Locks the current wireframe attribute, using the given address as a
// 'key'. The wireframe attribute cannot be changed again until it is
// unlocked with this key address.
// ------------------------------------------------------------------------
void vsGraphicsState::lockWireframe(void *lockAddr)
{
    // If there's no existing lock, set the lock value
    if (!wireframeLock)
        wireframeLock = lockAddr;
}

// ------------------------------------------------------------------------
// Unlocks the current backface attribute, using the given address as a
// 'key'; this key must match the key that the attribute was locked with or
// the function will not work.
// ------------------------------------------------------------------------
void vsGraphicsState::unlockBackface(void *lockAddr)
{
    // If the unlock value matches the lock value, clear the lock
    if (backfaceLock == lockAddr)
        backfaceLock = NULL;
}

// ------------------------------------------------------------------------
// Unlocks the current fog attribute, using the given address as a
// 'key'; this key must match the key that the attribute was locked with or
// the function will not work.
// ------------------------------------------------------------------------
void vsGraphicsState::unlockFog(void *lockAddr)
{
    // If the unlock value matches the lock value, clear the lock
    if (fogLock == lockAddr)
        fogLock = NULL;
}

// ------------------------------------------------------------------------
// Unlocks the current material attribute, using the given address as a
// 'key'; this key must match the key that the attribute was locked with or
// the function will not work.
// ------------------------------------------------------------------------
void vsGraphicsState::unlockMaterial(void *lockAddr)
{
    // If the unlock value matches the lock value, clear the lock
    if (materialLock == lockAddr)
        materialLock = NULL;
}

// ------------------------------------------------------------------------
// Unlocks the current shader attribute, using the given address as a
// 'key'; this key must match the key that the attribute was locked with or
// the function will not work.
// ------------------------------------------------------------------------
void vsGraphicsState::unlockShader(void *lockAddr)
{
    // If the unlock value matches the lock value, clear the lock
    if (shaderLock == lockAddr)
        shaderLock = NULL;
}

// ------------------------------------------------------------------------
// Unlocks the current shading attribute, using the given address as a
// 'key'; this key must match the key that the attribute was locked with or
// the function will not work.
// ------------------------------------------------------------------------
void vsGraphicsState::unlockShading(void *lockAddr)
{
    // If the unlock value matches the lock value, clear the lock
    if (shadingLock == lockAddr)
        shadingLock = NULL;
}

// ------------------------------------------------------------------------
// Unlocks the current texture attribute, using the given address as a
// 'key'; this key must match the key that the attribute was locked with or
// the function will not work.
// ------------------------------------------------------------------------
void vsGraphicsState::unlockTexture(unsigned int unit, void *lockAddr)
{
    // If the unlock value matches the lock value, clear the lock
    if (textureLock[unit] == lockAddr)
        textureLock[unit] = NULL;
}

// ------------------------------------------------------------------------
// Unlocks the current transparency attribute, using the given address as a
// 'key'; this key must match the key that the attribute was locked with or
// the function will not work.
// ------------------------------------------------------------------------
void vsGraphicsState::unlockTransparency(void *lockAddr)
{
    // If the unlock value matches the lock value, clear the lock
    if (transparencyLock == lockAddr)
        transparencyLock = NULL;
}

// ------------------------------------------------------------------------
// Unlocks the current wireframe attribute, using the given address as a
// 'key'; this key must match the key that the attribute was locked with or
// the function will not work.
// ------------------------------------------------------------------------
void vsGraphicsState::unlockWireframe(void *lockAddr)
{
    // If the unlock value matches the lock value, clear the lock
    if (wireframeLock == lockAddr)
        wireframeLock = NULL;
}
