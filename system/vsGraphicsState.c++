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
// Clears the internal graphics state and sets the graphics library state
// to default values
// ------------------------------------------------------------------------
void vsGraphicsState::clearState()
{
    // Clear the 'current attribute' values
    backfaceAttr = NULL;
    fogAttr = NULL;
    materialAttr = NULL;
    shadingAttr = NULL;
    textureAttr = NULL;
    transparencyAttr = NULL;
    wireframeAttr = NULL;
    lightAttrCount = 0;

    // Clear the 'attribute lock' pointers
    backfaceLock = NULL;
    fogLock = NULL;
    materialLock = NULL;
    shadingLock = NULL;
    textureLock = NULL;
    transparencyLock = NULL;
    wireframeLock = NULL;
}

// ------------------------------------------------------------------------
// VESS internal function
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

    // Call the shading attribute (if any) to make its state changes
    if (shadingAttr)
        shadingAttr->setState(state);

    // Call the texture attribute (if any) to make its state changes
    if (textureAttr)
        textureAttr->setState(state);

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
// Sets the attribute that contains the desired shading state
// ------------------------------------------------------------------------
void vsGraphicsState::setShading(vsShadingAttribute *newAttrib)
{
    // Set the current attribute, if it's not locked
    if (!shadingLock)
        shadingAttr = newAttrib;
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired texture state
// ------------------------------------------------------------------------
void vsGraphicsState::setTexture(vsTextureAttribute *newAttrib)
{
    // Set the current attribute, if it's not locked
    if (!textureLock)
        textureAttr = newAttrib;
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
// Retrieves the attribute that contains the current shading state
// ------------------------------------------------------------------------
vsShadingAttribute *vsGraphicsState::getShading()
{
    return shadingAttr;
}

// ------------------------------------------------------------------------
// Retrieves the attribute that contains the current texture state
// ------------------------------------------------------------------------
vsTextureAttribute *vsGraphicsState::getTexture()
{
    return textureAttr;
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
// Locks the current texture attribute, using the given address as a
// 'key'. The texture attribute cannot be changed again until it is
// unlocked with this key address.
// ------------------------------------------------------------------------
void vsGraphicsState::lockTexture(void *lockAddr)
{
    // If there's no existing lock, set the lock value
    if (!textureLock)
        textureLock = lockAddr;
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
void vsGraphicsState::unlockTexture(void *lockAddr)
{
    // If the unlock value matches the lock value, clear the lock
    if (textureLock == lockAddr)
        textureLock = NULL;
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
