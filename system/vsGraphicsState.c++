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

#define EQUAL(x,y) (fabs((x) - (y)) < 1E-6)

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
// VESS internal function
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

// ------------------------------------------------------------------------
// Static function
// Compares two backface attributes for equivalence
// ------------------------------------------------------------------------
int vsGraphicsState::isSameBackface(vsAttribute *firstAttr,
    vsAttribute *secondAttr)
{
    vsBackfaceAttribute *first, *second;
    int val1, val2;
    
    first = (vsBackfaceAttribute *)firstAttr;
    second = (vsBackfaceAttribute *)secondAttr;
    
    if (first == second)
        return VS_TRUE;

    if (!first || !second)
        return VS_FALSE;

    val1 = first->isEnabled();
    val2 = second->isEnabled();
    if (val1 != val2)
        return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Static function
// Compares two fog attributes for equivalence
// ------------------------------------------------------------------------
int vsGraphicsState::isSameFog(vsAttribute *firstAttr, vsAttribute *secondAttr)
{
    vsFogAttribute *first, *second;
    int val1, val2;
    double r1, g1, b1, r2, g2, b2;
    double near1, far1, near2, far2;
    
    first = (vsFogAttribute *)firstAttr;
    second = (vsFogAttribute *)secondAttr;
    
    if (first == second)
        return VS_TRUE;

    if (!first || !second)
        return VS_FALSE;

    val1 = first->getEquationType();
    val2 = second->getEquationType();
    if (val1 != val2)
        return VS_FALSE;

    first->getColor(&r1, &g1, &b1);
    second->getColor(&r2, &g2, &b2);
    if (!EQUAL(r1,r2) || !EQUAL(g1,g2) || !EQUAL(b1,b2))
        return VS_FALSE;

    first->getRanges(&near1, &far1);
    second->getRanges(&near2, &far2);
    if (!EQUAL(near1,near2) || !EQUAL(far1,far2))
        return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Static function
// Compares two material attributes for equivalence
// ------------------------------------------------------------------------
int vsGraphicsState::isSameMaterial(vsAttribute *firstAttr,
    vsAttribute *secondAttr)
{
    vsMaterialAttribute *first, *second;
    double val1, val2;
    int ival1, ival2;
    double r1, g1, b1, r2, g2, b2;
    
    first = (vsMaterialAttribute *)firstAttr;
    second = (vsMaterialAttribute *)secondAttr;

    if (first == second)
        return VS_TRUE;

    if (!first || !second)
        return VS_FALSE;

    first->getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_AMBIENT,
        &r1, &g1, &b1);
    second->getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_AMBIENT,
        &r2, &g2, &b2);
    if (!EQUAL(r1,r2) || !EQUAL(g1,g2) || !EQUAL(b1,b2))
        return VS_FALSE;

    first->getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_AMBIENT,
        &r1, &g1, &b1);
    second->getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_AMBIENT,
        &r2, &g2, &b2);
    if (!EQUAL(r1,r2) || !EQUAL(g1,g2) || !EQUAL(b1,b2))
        return VS_FALSE;

    first->getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_DIFFUSE,
        &r1, &g1, &b1);
    second->getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_DIFFUSE,
        &r2, &g2, &b2);
    if (!EQUAL(r1,r2) || !EQUAL(g1,g2) || !EQUAL(b1,b2))
        return VS_FALSE;

    first->getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_DIFFUSE,
        &r1, &g1, &b1);
    second->getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_DIFFUSE,
        &r2, &g2, &b2);
    if (!EQUAL(r1,r2) || !EQUAL(g1,g2) || !EQUAL(b1,b2))
        return VS_FALSE;

    first->getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_SPECULAR,
        &r1, &g1, &b1);
    second->getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_SPECULAR,
        &r2, &g2, &b2);
    if (!EQUAL(r1,r2) || !EQUAL(g1,g2) || !EQUAL(b1,b2))
        return VS_FALSE;

    first->getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_SPECULAR,
        &r1, &g1, &b1);
    second->getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_SPECULAR,
        &r2, &g2, &b2);
    if (!EQUAL(r1,r2) || !EQUAL(g1,g2) || !EQUAL(b1,b2))
        return VS_FALSE;

    first->getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_EMISSIVE,
        &r1, &g1, &b1);
    second->getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_EMISSIVE,
        &r2, &g2, &b2);
    if (!EQUAL(r1,r2) || !EQUAL(g1,g2) || !EQUAL(b1,b2))
        return VS_FALSE;

    first->getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_EMISSIVE,
        &r1, &g1, &b1);
    second->getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_EMISSIVE,
        &r2, &g2, &b2);
    if (!EQUAL(r1,r2) || !EQUAL(g1,g2) || !EQUAL(b1,b2))
        return VS_FALSE;

    val1 = first->getAlpha(VS_MATERIAL_SIDE_FRONT);
    val2 = second->getAlpha(VS_MATERIAL_SIDE_FRONT);
    if (!EQUAL(val1,val2))
        return VS_FALSE;

    val1 = first->getAlpha(VS_MATERIAL_SIDE_BACK);
    val2 = second->getAlpha(VS_MATERIAL_SIDE_BACK);
    if (!EQUAL(val1,val2))
        return VS_FALSE;

    val1 = first->getShininess(VS_MATERIAL_SIDE_FRONT);
    val2 = second->getShininess(VS_MATERIAL_SIDE_FRONT);
    if (!EQUAL(val1,val2))
        return VS_FALSE;

    val1 = first->getShininess(VS_MATERIAL_SIDE_BACK);
    val2 = second->getShininess(VS_MATERIAL_SIDE_BACK);
    if (!EQUAL(val1,val2))
        return VS_FALSE;

    ival1 = first->getColorMode(VS_MATERIAL_SIDE_FRONT);
    ival2 = second->getColorMode(VS_MATERIAL_SIDE_FRONT);
    if (ival1 != ival2)
        return VS_FALSE;

    ival1 = first->getColorMode(VS_MATERIAL_SIDE_BACK);
    ival2 = second->getColorMode(VS_MATERIAL_SIDE_BACK);
    if (ival1 != ival2)
        return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Static function
// Compares two shading attributes for equivalence
// ------------------------------------------------------------------------
int vsGraphicsState::isSameShading(vsAttribute *firstAttr,
    vsAttribute *secondAttr)
{
    vsShadingAttribute *first, *second;
    int val1, val2;
    
    first = (vsShadingAttribute *)firstAttr;
    second = (vsShadingAttribute *)secondAttr;
    
    if (first == second)
        return VS_TRUE;

    if (!first || !second)
        return VS_FALSE;

    val1 = first->getShading();
    val2 = second->getShading();
    if (val1 != val2)
        return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Static function
// Compares two texture attributes for equivalence
// ------------------------------------------------------------------------
int vsGraphicsState::isSameTexture(vsAttribute *firstAttr,
    vsAttribute *secondAttr)
{
    vsTextureAttribute *first, *second;
    unsigned char *image1, *image2;
    int xval1, yval1, xval2, yval2, val1, val2;
    
    first = (vsTextureAttribute *)firstAttr;
    second = (vsTextureAttribute *)secondAttr;
    
    if (first == second)
        return VS_TRUE;

    if (!first || !second)
        return VS_FALSE;

    first->getImage(&image1, &xval1, &yval1, &val1);
    second->getImage(&image2, &xval2, &yval2, &val2);
    if ((image1 != image2) || (xval1 != xval2) || (yval1 != val2) ||
        (val1 != val2))
        return VS_FALSE;

    val1 = first->getBoundaryMode(VS_TEXTURE_DIRECTION_S);
    val2 = second->getBoundaryMode(VS_TEXTURE_DIRECTION_S);
    if (val1 != val2)
        return VS_FALSE;

    val1 = first->getBoundaryMode(VS_TEXTURE_DIRECTION_T);
    val2 = second->getBoundaryMode(VS_TEXTURE_DIRECTION_T);
    if (val1 != val2)
        return VS_FALSE;

    val1 = first->getApplyMode();
    val2 = second->getApplyMode();
    if (val1 != val2)
        return VS_FALSE;

    val1 = first->getMagFilter();
    val2 = second->getMagFilter();
    if (val1 != val2)
        return VS_FALSE;

    val1 = first->getMinFilter();
    val2 = second->getMinFilter();
    if (val1 != val2)
        return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Static function
// Compares two transparency attributes for equivalence
// ------------------------------------------------------------------------
int vsGraphicsState::isSameTransparency(vsAttribute *firstAttr,
    vsAttribute *secondAttr)
{
    vsTransparencyAttribute *first, *second;
    int val1, val2;
    
    first = (vsTransparencyAttribute *)firstAttr;
    second = (vsTransparencyAttribute *)secondAttr;
    
    if (first == second)
        return VS_TRUE;

    if (!first || !second)
        return VS_FALSE;

    val1 = first->isEnabled();
    val2 = second->isEnabled();
    if (val1 != val2)
        return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Static function
// Compares two wireframe attributes for equivalence
// ------------------------------------------------------------------------
int vsGraphicsState::isSameWireframe(vsAttribute *firstAttr,
    vsAttribute *secondAttr)
{
    vsWireframeAttribute *first, *second;
    int val1, val2;
    
    first = (vsWireframeAttribute *)firstAttr;
    second = (vsWireframeAttribute *)secondAttr;
    
    if (first == second)
        return VS_TRUE;

    if (!first || !second)
        return VS_FALSE;

    val1 = first->isEnabled();
    val2 = second->isEnabled();
    if (val1 != val2)
        return VS_FALSE;

    return VS_TRUE;
}
