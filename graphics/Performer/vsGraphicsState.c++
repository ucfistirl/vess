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
// Static function
// Returns the currently active vsGraphicsState object
// ------------------------------------------------------------------------
vsGraphicsState *vsGraphicsState::getInstance()
{
    if (!classInstance)
	classInstance = new vsGraphicsState();

    return classInstance;
}

// ------------------------------------------------------------------------
// Static function
// Deletes the currently active vsGraphicsState object
// ------------------------------------------------------------------------
void vsGraphicsState::deleteInstance()
{
    if (classInstance)
	delete classInstance;

    classInstance = NULL;
}

// ------------------------------------------------------------------------
// Clears the internal graphics state and sets the graphics library state
// to default values
// ------------------------------------------------------------------------
void vsGraphicsState::clearState()
{
    backfaceAttr = NULL;
    fogAttr = NULL;
    materialAttr = NULL;
    shadingAttr = NULL;
    textureAttr = NULL;
    transparencyAttr = NULL;
    wireframeAttr = NULL;
    lightAttrCount = 0;

    backfaceLock = NULL;
    fogLock = NULL;
    materialLock = NULL;
    shadingLock = NULL;
    textureLock = NULL;
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
    
    state->setInherit(PFSTATE_ALL);

    if (backfaceAttr)
        backfaceAttr->setState(state);

    if (fogAttr)
        fogAttr->setState(state);

    if (materialAttr)
        materialAttr->setState(state);

    if (shadingAttr)
        shadingAttr->setState(state);

    if (textureAttr)
        textureAttr->setState(state);

    if (transparencyAttr)
        transparencyAttr->setState(state);

    if (wireframeAttr)
        wireframeAttr->setState(state);

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
    if (!backfaceLock)
        backfaceAttr = newAttrib;
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired fog state
// ------------------------------------------------------------------------
void vsGraphicsState::setFog(vsFogAttribute *newAttrib)
{
    if (!fogLock)
        fogAttr = newAttrib;
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired material state
// ------------------------------------------------------------------------
void vsGraphicsState::setMaterial(vsMaterialAttribute *newAttrib)
{
    if (!materialLock)
        materialAttr = newAttrib;
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired shading state
// ------------------------------------------------------------------------
void vsGraphicsState::setShading(vsShadingAttribute *newAttrib)
{
    if (!shadingLock)
        shadingAttr = newAttrib;
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired texture state
// ------------------------------------------------------------------------
void vsGraphicsState::setTexture(vsTextureAttribute *newAttrib)
{
    if (!textureLock)
        textureAttr = newAttrib;
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired transparency state
// ------------------------------------------------------------------------
void vsGraphicsState::setTransparency(vsTransparencyAttribute *newAttrib)
{
    if (!transparencyLock)
        transparencyAttr = newAttrib;
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired wireframe state
// ------------------------------------------------------------------------
void vsGraphicsState::setWireframe(vsWireframeAttribute *newAttrib)
{
    if (!wireframeLock)
        wireframeAttr = newAttrib;
}

// ------------------------------------------------------------------------
// Adds the light attribute to the graphics state list of local lights
// ------------------------------------------------------------------------
void vsGraphicsState::addLight(vsLightAttribute *lightAttrib)
{
    lightAttrList[lightAttrCount++] = lightAttrib;
}

// ------------------------------------------------------------------------
// Removes the light attribute from the graphics state list of local lights
// ------------------------------------------------------------------------
void vsGraphicsState::removeLight(vsLightAttribute *lightAttrib)
{
    int loop;
    
    for (loop = 0; loop < lightAttrCount; loop++)
        if (lightAttrib == lightAttrList[loop])
        {
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
    if ((index < 0) || (index >= lightAttrCount))
        return NULL;

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
    if (wireframeLock == lockAddr)
        wireframeLock = NULL;
}
