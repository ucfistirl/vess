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
//    VESS Module:  vsMaterialAttribute.c++
//
//    Description:  Specifies that geometry should be drawn with the
//                  material properties given in this attribute
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsMaterialAttribute.h++"

#include "vsSystem.h++"

// ------------------------------------------------------------------------
// Default Constructor - Creates new Performer material objects and
// initializes them
// ------------------------------------------------------------------------
vsMaterialAttribute::vsMaterialAttribute()
{
    frontMaterial = new pfMaterial();
    frontMaterial->setSide(PFMTL_FRONT);
    frontMaterial->ref();

    backMaterial = new pfMaterial();
    backMaterial->setSide(PFMTL_BACK);
    backMaterial->ref();
}

// ------------------------------------------------------------------------
// VESS internal function
// Constructor - Stores the given Performer material objects and
// initializes them
// ------------------------------------------------------------------------
vsMaterialAttribute::vsMaterialAttribute(pfMaterial *front, pfMaterial *back)
{
    frontMaterial = front;
    frontMaterial->setSide(PFMTL_FRONT);
    frontMaterial->ref();

    backMaterial = back;
    backMaterial->setSide(PFMTL_BACK);
    backMaterial->ref();
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsMaterialAttribute::~vsMaterialAttribute()
{
    frontMaterial->unref();
    pfDelete(frontMaterial);
    backMaterial->unref();
    pfDelete(backMaterial);

    // Try removing a link between this attribute and one of the Performer
    // materials, in the case that the vsGeometry constructor put one in
    // in the first place.
    ((vsSystem::systemObject)->getNodeMap())->removeLink(this,
        VS_OBJMAP_FIRST_LIST);
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsMaterialAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_MATERIAL;
}

// ------------------------------------------------------------------------
// Sets one of the colors for this material
// ------------------------------------------------------------------------
void vsMaterialAttribute::setColor(int side, int whichColor, double r,
                                   double g, double b)
{
    if (side != VS_MATERIAL_SIDE_BACK)
    {
        switch (whichColor)
        {
            case VS_MATERIAL_COLOR_AMBIENT:
                frontMaterial->setColor(PFMTL_AMBIENT, r, g, b);
                break;
            case VS_MATERIAL_COLOR_DIFFUSE:
                frontMaterial->setColor(PFMTL_DIFFUSE, r, g, b);
                break;
            case VS_MATERIAL_COLOR_SPECULAR:
                frontMaterial->setColor(PFMTL_SPECULAR, r, g, b);
                break;
            case VS_MATERIAL_COLOR_EMISSIVE:
                frontMaterial->setColor(PFMTL_EMISSION, r, g, b);
                break;
        }
    }

    if (side != VS_MATERIAL_SIDE_FRONT)
    {
        switch (whichColor)
        {
            case VS_MATERIAL_COLOR_AMBIENT:
                backMaterial->setColor(PFMTL_AMBIENT, r, g, b);
                break;
            case VS_MATERIAL_COLOR_DIFFUSE:
                backMaterial->setColor(PFMTL_DIFFUSE, r, g, b);
                break;
            case VS_MATERIAL_COLOR_SPECULAR:
                backMaterial->setColor(PFMTL_SPECULAR, r, g, b);
                break;
            case VS_MATERIAL_COLOR_EMISSIVE:
                backMaterial->setColor(PFMTL_EMISSION, r, g, b);
                break;
        }
    }
}

// ------------------------------------------------------------------------
// Retrieves one of the colors for this material. NULL pointers may be
// passed for undesired values.
// ------------------------------------------------------------------------
void vsMaterialAttribute::getColor(int side, int whichColor, double *r,
                                   double *g, double *b)
{
    float red, green, blue;
    
    if (side == VS_MATERIAL_SIDE_BACK)
    {
        switch (whichColor)
        {
            case VS_MATERIAL_COLOR_AMBIENT:
                backMaterial->getColor(PFMTL_AMBIENT, &red, &green, &blue);
                break;
            case VS_MATERIAL_COLOR_DIFFUSE:
                backMaterial->getColor(PFMTL_DIFFUSE, &red, &green, &blue);
                break;
            case VS_MATERIAL_COLOR_SPECULAR:
                backMaterial->getColor(PFMTL_SPECULAR, &red, &green, &blue);
                break;
            case VS_MATERIAL_COLOR_EMISSIVE:
                backMaterial->getColor(PFMTL_EMISSION, &red, &green, &blue);
                break;
        }
    }
    else
    {
        switch (whichColor)
        {
            case VS_MATERIAL_COLOR_AMBIENT:
                frontMaterial->getColor(PFMTL_AMBIENT, &red, &green, &blue);
                break;
            case VS_MATERIAL_COLOR_DIFFUSE:
                frontMaterial->getColor(PFMTL_DIFFUSE, &red, &green, &blue);
                break;
            case VS_MATERIAL_COLOR_SPECULAR:
                frontMaterial->getColor(PFMTL_SPECULAR, &red, &green, &blue);
                break;
            case VS_MATERIAL_COLOR_EMISSIVE:
                frontMaterial->getColor(PFMTL_EMISSION, &red, &green, &blue);
                break;
        }
    }

    if (r)
        *r = red;
    if (g)
        *g = green;
    if (b)
        *b = blue;
}

// ------------------------------------------------------------------------
// Sets the alpha transparency value for one side of the material
// ------------------------------------------------------------------------
void vsMaterialAttribute::setAlpha(int side, double alpha)
{
    if (side != VS_MATERIAL_SIDE_BACK)
        frontMaterial->setAlpha(alpha);
    if (side != VS_MATERIAL_SIDE_FRONT)
        backMaterial->setAlpha(alpha);
}

// ------------------------------------------------------------------------
// Retrieves the alpha transparency value for one side of the material
// ------------------------------------------------------------------------
double vsMaterialAttribute::getAlpha(int side)
{
    if (side == VS_MATERIAL_SIDE_BACK)
        return backMaterial->getAlpha();
    else
        return frontMaterial->getAlpha();
}

// ------------------------------------------------------------------------
// Sets the specular shininess exponent for one side of the material
// ------------------------------------------------------------------------
void vsMaterialAttribute::setShininess(int side, double shine)
{
    if (side != VS_MATERIAL_SIDE_BACK)
        frontMaterial->setShininess(shine);
    if (side != VS_MATERIAL_SIDE_FRONT)
        backMaterial->setShininess(shine);
}

// ------------------------------------------------------------------------
// Retrieves the specular shininess exponent for one side of the material
// ------------------------------------------------------------------------
double vsMaterialAttribute::getShininess(int side)
{
    if (side == VS_MATERIAL_SIDE_BACK)
        return backMaterial->getShininess();
    else
        return frontMaterial->getShininess();
}

// ------------------------------------------------------------------------
// Sets the color mode for one side of the material. The color mode
// affects how colors placed on the geometry vertices affect the color(s)
// of the material. The mode is generally which color(s) of the material
// get replaced with the vertex colors.
// ------------------------------------------------------------------------
void vsMaterialAttribute::setColorMode(int side, int colorMode)
{
    if (side != VS_MATERIAL_SIDE_BACK)
    {
        switch (colorMode)
        {
            case VS_MATERIAL_CMODE_AMBIENT:
                frontMaterial->setColorMode(PFMTL_FRONT, PFMTL_CMODE_AMBIENT);
                break;
            case VS_MATERIAL_CMODE_DIFFUSE:
                frontMaterial->setColorMode(PFMTL_FRONT, PFMTL_CMODE_DIFFUSE);
                break;
            case VS_MATERIAL_CMODE_SPECULAR:
                frontMaterial->setColorMode(PFMTL_FRONT, PFMTL_CMODE_SPECULAR);
                break;
            case VS_MATERIAL_CMODE_EMISSIVE:
                frontMaterial->setColorMode(PFMTL_FRONT, PFMTL_CMODE_EMISSION);
                break;
            case VS_MATERIAL_CMODE_AMBIENT_DIFFUSE:
                frontMaterial->setColorMode(PFMTL_FRONT,
                    PFMTL_CMODE_AMBIENT_AND_DIFFUSE);
                break;
            case VS_MATERIAL_CMODE_NONE:
                frontMaterial->setColorMode(PFMTL_FRONT, PFMTL_CMODE_OFF);
                break;
        }
    }

    if (side != VS_MATERIAL_SIDE_FRONT)
    {
        switch (colorMode)
        {
            case VS_MATERIAL_CMODE_AMBIENT:
                backMaterial->setColorMode(PFMTL_BACK, PFMTL_CMODE_AMBIENT);
                break;
            case VS_MATERIAL_CMODE_DIFFUSE:
                backMaterial->setColorMode(PFMTL_BACK, PFMTL_CMODE_DIFFUSE);
                break;
            case VS_MATERIAL_CMODE_SPECULAR:
                backMaterial->setColorMode(PFMTL_BACK, PFMTL_CMODE_SPECULAR);
                break;
            case VS_MATERIAL_CMODE_EMISSIVE:
                backMaterial->setColorMode(PFMTL_BACK, PFMTL_CMODE_EMISSION);
                break;
            case VS_MATERIAL_CMODE_AMBIENT_DIFFUSE:
                backMaterial->setColorMode(PFMTL_BACK,
                    PFMTL_CMODE_AMBIENT_AND_DIFFUSE);
                break;
            case VS_MATERIAL_CMODE_NONE:
                backMaterial->setColorMode(PFMTL_BACK, PFMTL_CMODE_OFF);
                break;
        }
    }
}

// ------------------------------------------------------------------------
// Retrieves the color mode for one side of the material
// ------------------------------------------------------------------------
int vsMaterialAttribute::getColorMode(int side)
{
    if (side == VS_MATERIAL_SIDE_BACK)
    {
        switch (backMaterial->getColorMode(PFMTL_BACK))
        {
            case PFMTL_CMODE_AMBIENT:
                return VS_MATERIAL_CMODE_AMBIENT;
            case PFMTL_CMODE_DIFFUSE:
                return VS_MATERIAL_CMODE_DIFFUSE;
            case PFMTL_CMODE_SPECULAR:
                return VS_MATERIAL_CMODE_SPECULAR;
            case PFMTL_CMODE_EMISSION:
                return VS_MATERIAL_CMODE_EMISSIVE;
            case PFMTL_CMODE_AMBIENT_AND_DIFFUSE:
                return VS_MATERIAL_CMODE_AMBIENT_DIFFUSE;
            case PFMTL_CMODE_OFF:
                return VS_MATERIAL_CMODE_NONE;
        }
    }
    else
    {
        switch (frontMaterial->getColorMode(PFMTL_FRONT))
        {
            case PFMTL_CMODE_AMBIENT:
                return VS_MATERIAL_CMODE_AMBIENT;
            case PFMTL_CMODE_DIFFUSE:
                return VS_MATERIAL_CMODE_DIFFUSE;
            case PFMTL_CMODE_SPECULAR:
                return VS_MATERIAL_CMODE_SPECULAR;
            case PFMTL_CMODE_EMISSION:
                return VS_MATERIAL_CMODE_EMISSIVE;
            case PFMTL_CMODE_AMBIENT_AND_DIFFUSE:
                return VS_MATERIAL_CMODE_AMBIENT_DIFFUSE;
            case PFMTL_CMODE_OFF:
                return VS_MATERIAL_CMODE_NONE;
        }
    }

    return 0;
}

// ------------------------------------------------------------------------
// VESS internal function
// Saves the current attribute
// ------------------------------------------------------------------------
void vsMaterialAttribute::saveCurrent()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    attrSaveList[attrSaveCount++] = gState->getMaterial();
}

// ------------------------------------------------------------------------
// VESS internal function
// Sets the current attribute to this one
// ------------------------------------------------------------------------
void vsMaterialAttribute::apply()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    gState->setMaterial(this);
}

// ------------------------------------------------------------------------
// VESS internal function
// Restores the current attribute to the last saved one
// ------------------------------------------------------------------------
void vsMaterialAttribute::restoreSaved()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    gState->setMaterial((vsMaterialAttribute *)(attrSaveList[--attrSaveCount]));
}

// ------------------------------------------------------------------------
// VESS internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsMaterialAttribute::setState(pfGeoState *state)
{
    state->setAttr(PFSTATE_FRONTMTL, frontMaterial);
    state->setAttr(PFSTATE_BACKMTL, backMaterial);
}
