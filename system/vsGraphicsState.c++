// File vsGraphicsState.c++

#include "vsGraphicsState.h++"

#define EQUAL(x,y) (fabs((x) - (y)) < 1E-6)

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsGraphicsState::vsGraphicsState()
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
    currentBackface = (vsBackfaceAttribute *)(-1);
    currentFog = (vsFogAttribute *)(-1);
    currentMaterial = (vsMaterialAttribute *)(-1);
    currentShading = (vsShadingAttribute *)(-1);
    currentTexture = (vsTextureAttribute *)(-1);
    currentTransparency = (vsTransparencyAttribute *)(-1);

    newBackface = NULL;
    newFog = NULL;
    newMaterial = NULL;
    newShading = NULL;
    newTexture = NULL;
    newTransparency = NULL;
    
    applyState();
}

// ------------------------------------------------------------------------
// Synchronizes the current graphics library state with the desired state
// ------------------------------------------------------------------------
void vsGraphicsState::applyState()
{
    if ((currentBackface != newBackface) &&
        !(isSameBackface(currentBackface, newBackface)))
    {
        if (newBackface)
            newBackface->setState();
        else
            vsBackfaceAttribute::setDefault();

        currentBackface = newBackface;
    }

    if ((currentFog != newFog) && !(isSameFog(currentFog, newFog)))
    {
        if (newBackface)
        {
            if (currentBackface == NULL)
                pfEnable(PFEN_FOG);
            newFog->setState();
        }
        else
            pfDisable(PFEN_FOG);

        currentFog = newFog;
    }

    if ((currentMaterial != newMaterial) &&
        !(isSameMaterial(currentMaterial, newMaterial)))
    {
        if (newMaterial)
            newMaterial->setState();

        currentMaterial = newMaterial;
    }

    if ((currentShading != newShading) &&
        !(isSameShading(currentShading, newShading)))
    {
        if (newShading)
            newShading->setState();
        else
            vsShadingAttribute::setDefault();

        currentShading = newShading;
    }

    if ((currentTexture != newTexture) &&
        !(isSameTexture(currentTexture, newTexture)))
    {
        if (newTexture)
        {
            if (currentTexture == NULL)
                pfEnable(PFEN_TEXTURE);
            newTexture->setState();
        }
        else
            pfDisable(PFEN_TEXTURE);

        currentTexture = newTexture;
    }

    if ((currentTransparency != newTransparency) &&
        !(isSameTransparency(currentTransparency, newTransparency)))
    {
        if (newTransparency)
            newTransparency->setState();
        else
            vsTransparencyAttribute::setDefault();

        currentTransparency = newTransparency;
    }
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired backface state
// ------------------------------------------------------------------------
void vsGraphicsState::setBackface(vsBackfaceAttribute *newAttrib)
{
    newBackface = newAttrib;
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired fog state
// ------------------------------------------------------------------------
void vsGraphicsState::setFog(vsFogAttribute *newAttrib)
{
    newFog = newAttrib;
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired material state
// ------------------------------------------------------------------------
void vsGraphicsState::setMaterial(vsMaterialAttribute *newAttrib)
{
    newMaterial = newAttrib;
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired shading state
// ------------------------------------------------------------------------
void vsGraphicsState::setShading(vsShadingAttribute *newAttrib)
{
    newShading = newAttrib;
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired texture state
// ------------------------------------------------------------------------
void vsGraphicsState::setTexture(vsTextureAttribute *newAttrib)
{
    newTexture = newAttrib;
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired transparency state
// ------------------------------------------------------------------------
void vsGraphicsState::setTransparency(vsTransparencyAttribute *newAttrib)
{
    newTransparency = newAttrib;
}

// ------------------------------------------------------------------------
// Retrieves the attribute that contains the desired (not neccessarily
// current) backface state
// ------------------------------------------------------------------------
vsBackfaceAttribute *vsGraphicsState::getBackface()
{
    return newBackface;
}

// ------------------------------------------------------------------------
// Retrieves the attribute that contains the desired (not neccessarily
// current) fog state
// ------------------------------------------------------------------------
vsFogAttribute *vsGraphicsState::getFog()
{
    return newFog;
}

// ------------------------------------------------------------------------
// Retrieves the attribute that contains the desired (not neccessarily
// current) material state
// ------------------------------------------------------------------------
vsMaterialAttribute *vsGraphicsState::getMaterial()
{
    return newMaterial;
}

// ------------------------------------------------------------------------
// Retrieves the attribute that contains the desired (not neccessarily
// current) shading state
// ------------------------------------------------------------------------
vsShadingAttribute *vsGraphicsState::getShading()
{
    return newShading;
}

// ------------------------------------------------------------------------
// Retrieves the attribute that contains the desired (not neccessarily
// current) texture state
// ------------------------------------------------------------------------
vsTextureAttribute *vsGraphicsState::getTexture()
{
    return newTexture;
}

// ------------------------------------------------------------------------
// Retrieves the attribute that contains the desired (not neccessarily
// current) transparecy state
// ------------------------------------------------------------------------
vsTransparencyAttribute *vsGraphicsState::getTransparency()
{
    return newTransparency;
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
