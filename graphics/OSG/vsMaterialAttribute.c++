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

#include "vsGraphicsState.h++"

// ------------------------------------------------------------------------
// Default Constructor - Creates a new OSG Material object and initializes
// it
// ------------------------------------------------------------------------
vsMaterialAttribute::vsMaterialAttribute()
{
    osgMaterial = new osg::Material();
    osgMaterial->ref();
    
    osgMaterial->setColorMode(osg::Material::OFF);
    osgMaterial->setAmbient(osg::Material::FRONT,
        osg::Vec4(1.0, 1.0, 1.0, 1.0));
    osgMaterial->setAmbient(osg::Material::BACK,
        osg::Vec4(1.0, 1.0, 1.0, 1.0));
    osgMaterial->setDiffuse(osg::Material::FRONT,
        osg::Vec4(1.0, 1.0, 1.0, 1.0));
    osgMaterial->setDiffuse(osg::Material::BACK,
        osg::Vec4(1.0, 1.0, 1.0, 1.0));
    osgMaterial->setSpecular(osg::Material::FRONT,
        osg::Vec4(1.0, 1.0, 1.0, 1.0));
    osgMaterial->setSpecular(osg::Material::BACK,
        osg::Vec4(1.0, 1.0, 1.0, 1.0));
    osgMaterial->setEmission(osg::Material::FRONT,
        osg::Vec4(1.0, 1.0, 1.0, 1.0));
    osgMaterial->setEmission(osg::Material::BACK,
        osg::Vec4(1.0, 1.0, 1.0, 1.0));
    osgMaterial->setShininess(osg::Material::FRONT, 0.0);
    osgMaterial->setShininess(osg::Material::BACK, 0.0);
}

// ------------------------------------------------------------------------
// Internal function
// Constructor - Stores the given OSG Material object
// ------------------------------------------------------------------------
vsMaterialAttribute::vsMaterialAttribute(osg::Material *material)
{
    osgMaterial = material;
    osgMaterial->ref();
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsMaterialAttribute::~vsMaterialAttribute()
{
    osgMaterial->unref();
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsMaterialAttribute::getClassName()
{
    return "vsMaterialAttribute";
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
    osg::Vec4 color;

    if (side != VS_MATERIAL_SIDE_BACK)
    {
        switch (whichColor)
        {
            case VS_MATERIAL_COLOR_AMBIENT:
                color = osgMaterial->getAmbient(osg::Material::FRONT);
                color[0] = r;
                color[1] = g;
                color[2] = b;
                osgMaterial->setAmbient(osg::Material::FRONT, color);
                break;

            case VS_MATERIAL_COLOR_DIFFUSE:
                color = osgMaterial->getDiffuse(osg::Material::FRONT);
                color[0] = r;
                color[1] = g;
                color[2] = b;
                osgMaterial->setDiffuse(osg::Material::FRONT, color);
                break;

            case VS_MATERIAL_COLOR_SPECULAR:
                color = osgMaterial->getSpecular(osg::Material::FRONT);
                color[0] = r;
                color[1] = g;
                color[2] = b;
                osgMaterial->setSpecular(osg::Material::FRONT, color);
                break;

            case VS_MATERIAL_COLOR_EMISSIVE:
                color = osgMaterial->getEmission(osg::Material::FRONT);
                color[0] = r;
                color[1] = g;
                color[2] = b;
                osgMaterial->setEmission(osg::Material::FRONT, color);
                break;
        }
    }

    if (side != VS_MATERIAL_SIDE_FRONT)
    {
        switch (whichColor)
        {
            case VS_MATERIAL_COLOR_AMBIENT:
                color = osgMaterial->getAmbient(osg::Material::BACK);
                color[0] = r;
                color[1] = g;
                color[2] = b;
                osgMaterial->setAmbient(osg::Material::BACK, color);
                break;

            case VS_MATERIAL_COLOR_DIFFUSE:
                color = osgMaterial->getDiffuse(osg::Material::BACK);
                color[0] = r;
                color[1] = g;
                color[2] = b;
                osgMaterial->setDiffuse(osg::Material::BACK, color);
                break;

            case VS_MATERIAL_COLOR_SPECULAR:
                color = osgMaterial->getSpecular(osg::Material::BACK);
                color[0] = r;
                color[1] = g;
                color[2] = b;
                osgMaterial->setSpecular(osg::Material::BACK, color);
                break;

            case VS_MATERIAL_COLOR_EMISSIVE:
                color = osgMaterial->getEmission(osg::Material::BACK);
                color[0] = r;
                color[1] = g;
                color[2] = b;
                osgMaterial->setEmission(osg::Material::BACK, color);
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
    osg::Vec4 color;
    float red, green, blue;
    
    if (side == VS_MATERIAL_SIDE_BACK)
    {
        switch (whichColor)
        {
            case VS_MATERIAL_COLOR_AMBIENT:
                color = osgMaterial->getAmbient(osg::Material::BACK);
                break;
            case VS_MATERIAL_COLOR_DIFFUSE:
                color = osgMaterial->getDiffuse(osg::Material::BACK);
                break;
            case VS_MATERIAL_COLOR_SPECULAR:
                color = osgMaterial->getSpecular(osg::Material::BACK);
                break;
            case VS_MATERIAL_COLOR_EMISSIVE:
                color = osgMaterial->getEmission(osg::Material::BACK);
                break;
        }
    }
    else
    {
        switch (whichColor)
        {
            case VS_MATERIAL_COLOR_AMBIENT:
                color = osgMaterial->getAmbient(osg::Material::FRONT);
                break;
            case VS_MATERIAL_COLOR_DIFFUSE:
                color = osgMaterial->getDiffuse(osg::Material::FRONT);
                break;
            case VS_MATERIAL_COLOR_SPECULAR:
                color = osgMaterial->getSpecular(osg::Material::FRONT);
                break;
            case VS_MATERIAL_COLOR_EMISSIVE:
                color = osgMaterial->getEmission(osg::Material::FRONT);
                break;
        }
    }

    if (r)
        *r = color[0];
    if (g)
        *g = color[1];
    if (b)
        *b = color[2];
}

// ------------------------------------------------------------------------
// Sets the alpha transparency value for one side of the material
// ------------------------------------------------------------------------
void vsMaterialAttribute::setAlpha(int side, double alpha)
{
    if (side != VS_MATERIAL_SIDE_BACK)
        osgMaterial->setAlpha(osg::Material::FRONT, alpha);
    if (side != VS_MATERIAL_SIDE_FRONT)
        osgMaterial->setAlpha(osg::Material::BACK, alpha);
}

// ------------------------------------------------------------------------
// Retrieves the alpha transparency value for one side of the material
// ------------------------------------------------------------------------
double vsMaterialAttribute::getAlpha(int side)
{
    if (side == VS_MATERIAL_SIDE_BACK)
        return (osgMaterial->getDiffuse(osg::Material::BACK))[3];
    else
        return (osgMaterial->getDiffuse(osg::Material::FRONT))[3];
}

// ------------------------------------------------------------------------
// Sets the specular shininess exponent for one side of the material
// ------------------------------------------------------------------------
void vsMaterialAttribute::setShininess(int side, double shine)
{
    if (side != VS_MATERIAL_SIDE_BACK)
        osgMaterial->setShininess(osg::Material::FRONT, shine);
    if (side != VS_MATERIAL_SIDE_FRONT)
        osgMaterial->setShininess(osg::Material::BACK, shine);
}

// ------------------------------------------------------------------------
// Retrieves the specular shininess exponent for one side of the material
// ------------------------------------------------------------------------
double vsMaterialAttribute::getShininess(int side)
{
    if (side == VS_MATERIAL_SIDE_BACK)
        return osgMaterial->getShininess(osg::Material::BACK);
    else
        return osgMaterial->getShininess(osg::Material::FRONT);
}

// ------------------------------------------------------------------------
// Sets the color mode for the material. The color mode affects how colors
// placed on the geometry vertices affect the color(s) of the material. The
// mode is generally which color(s) of the material get replaced with the
// vertex colors.
// * Note - Different color modes for the two sides of the same geometry
// are not supported under OSG operation
// ------------------------------------------------------------------------
void vsMaterialAttribute::setColorMode(int side, int colorMode)
{
    switch (colorMode)
    {
        case VS_MATERIAL_CMODE_AMBIENT:
            osgMaterial->setColorMode(osg::Material::AMBIENT);
            break;
        case VS_MATERIAL_CMODE_DIFFUSE:
            osgMaterial->setColorMode(osg::Material::DIFFUSE);
            break;
        case VS_MATERIAL_CMODE_SPECULAR:
            osgMaterial->setColorMode(osg::Material::SPECULAR);
            break;
        case VS_MATERIAL_CMODE_EMISSIVE:
            osgMaterial->setColorMode(osg::Material::EMISSION);
            break;
        case VS_MATERIAL_CMODE_AMBIENT_DIFFUSE:
            osgMaterial->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
            break;
        case VS_MATERIAL_CMODE_NONE:
            osgMaterial->setColorMode(osg::Material::OFF);
            break;
    }
}

// ------------------------------------------------------------------------
// Retrieves the color mode for the material
// ------------------------------------------------------------------------
int vsMaterialAttribute::getColorMode(int side)
{
    switch (osgMaterial->getColorMode())
    {
        case osg::Material::AMBIENT:
            return VS_MATERIAL_CMODE_AMBIENT;
        case osg::Material::DIFFUSE:
            return VS_MATERIAL_CMODE_DIFFUSE;
        case osg::Material::SPECULAR:
            return VS_MATERIAL_CMODE_SPECULAR;
        case osg::Material::EMISSION:
            return VS_MATERIAL_CMODE_EMISSIVE;
        case osg::Material::AMBIENT_AND_DIFFUSE:
            return VS_MATERIAL_CMODE_AMBIENT_DIFFUSE;
        case osg::Material::OFF:
            return VS_MATERIAL_CMODE_NONE;
    }

    return 0;
}

// ------------------------------------------------------------------------
// Internal function
// ------------------------------------------------------------------------
void vsMaterialAttribute::setOSGAttrModes(vsNode *node)
{
    unsigned int attrMode;
    osg::StateSet *osgStateSet;
    
    attrMode = osg::StateAttribute::ON;

    if (overrideFlag)
        attrMode |= osg::StateAttribute::OVERRIDE;

    osgStateSet = getOSGStateSet(node);

    osgStateSet->setAttributeAndModes(osgMaterial, attrMode);
}

// ------------------------------------------------------------------------
// Internal function
// ------------------------------------------------------------------------
void vsMaterialAttribute::attach(vsNode *node)
{
    vsStateAttribute::attach(node);

    setOSGAttrModes(node);
}

// ------------------------------------------------------------------------
// Internal function
// ------------------------------------------------------------------------
void vsMaterialAttribute::detach(vsNode *node)
{
    osg::StateSet *osgStateSet;
    osgStateSet = getOSGStateSet(node);

    osgStateSet->setAttributeAndModes(osgMaterial,
        osg::StateAttribute::INHERIT);

    vsStateAttribute::detach(node);
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsMaterialAttribute::attachDuplicate(vsNode *theNode)
{
    vsMaterialAttribute *newAttrib;
    osg::Material *newMaterial;
    
    newMaterial = new osg::Material();

    newAttrib = new vsMaterialAttribute(newMaterial);

    theNode->addAttribute(newAttrib);
}

// ------------------------------------------------------------------------
// Internal function
// Determines if the specified attribute has state information that is
// equivalent to what this attribute has
// ------------------------------------------------------------------------
int vsMaterialAttribute::isEquivalent(vsAttribute *attribute)
{
    vsMaterialAttribute *attr;
    double val1, val2;
    int ival1, ival2;
    double r1, g1, b1, r2, g2, b2;
    
    if (!attribute)
        return VS_FALSE;

    if (this == attribute)
        return VS_TRUE;
    
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_MATERIAL)
        return VS_FALSE;

    attr = (vsMaterialAttribute *)attribute;

    getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_AMBIENT,
        &r1, &g1, &b1);
    attr->getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_AMBIENT,
        &r2, &g2, &b2);
    if (!VS_EQUAL(r1,r2) || !VS_EQUAL(g1,g2) || !VS_EQUAL(b1,b2))
        return VS_FALSE;

    getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_AMBIENT,
        &r1, &g1, &b1);
    attr->getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_AMBIENT,
        &r2, &g2, &b2);
    if (!VS_EQUAL(r1,r2) || !VS_EQUAL(g1,g2) || !VS_EQUAL(b1,b2))
        return VS_FALSE;

    getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_DIFFUSE,
        &r1, &g1, &b1);
    attr->getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_DIFFUSE,
        &r2, &g2, &b2);
    if (!VS_EQUAL(r1,r2) || !VS_EQUAL(g1,g2) || !VS_EQUAL(b1,b2))
        return VS_FALSE;

    getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_DIFFUSE,
        &r1, &g1, &b1);
    attr->getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_DIFFUSE,
        &r2, &g2, &b2);
    if (!VS_EQUAL(r1,r2) || !VS_EQUAL(g1,g2) || !VS_EQUAL(b1,b2))
        return VS_FALSE;

    getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_SPECULAR,
        &r1, &g1, &b1);
    attr->getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_SPECULAR,
        &r2, &g2, &b2);
    if (!VS_EQUAL(r1,r2) || !VS_EQUAL(g1,g2) || !VS_EQUAL(b1,b2))
        return VS_FALSE;

    getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_SPECULAR,
        &r1, &g1, &b1);
    attr->getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_SPECULAR,
        &r2, &g2, &b2);
    if (!VS_EQUAL(r1,r2) || !VS_EQUAL(g1,g2) || !VS_EQUAL(b1,b2))
        return VS_FALSE;

    getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_EMISSIVE,
        &r1, &g1, &b1);
    attr->getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_EMISSIVE,
        &r2, &g2, &b2);
    if (!VS_EQUAL(r1,r2) || !VS_EQUAL(g1,g2) || !VS_EQUAL(b1,b2))
        return VS_FALSE;

    getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_EMISSIVE,
        &r1, &g1, &b1);
    attr->getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_EMISSIVE,
        &r2, &g2, &b2);
    if (!VS_EQUAL(r1,r2) || !VS_EQUAL(g1,g2) || !VS_EQUAL(b1,b2))
        return VS_FALSE;

    val1 = getAlpha(VS_MATERIAL_SIDE_FRONT);
    val2 = attr->getAlpha(VS_MATERIAL_SIDE_FRONT);
    if (!VS_EQUAL(val1,val2))
        return VS_FALSE;

    val1 = getAlpha(VS_MATERIAL_SIDE_BACK);
    val2 = attr->getAlpha(VS_MATERIAL_SIDE_BACK);
    if (!VS_EQUAL(val1,val2))
        return VS_FALSE;

    val1 = getShininess(VS_MATERIAL_SIDE_FRONT);
    val2 = attr->getShininess(VS_MATERIAL_SIDE_FRONT);
    if (!VS_EQUAL(val1,val2))
        return VS_FALSE;

    val1 = getShininess(VS_MATERIAL_SIDE_BACK);
    val2 = attr->getShininess(VS_MATERIAL_SIDE_BACK);
    if (!VS_EQUAL(val1,val2))
        return VS_FALSE;

    ival1 = getColorMode(VS_MATERIAL_SIDE_FRONT);
    ival2 = attr->getColorMode(VS_MATERIAL_SIDE_FRONT);
    if (ival1 != ival2)
        return VS_FALSE;

    ival1 = getColorMode(VS_MATERIAL_SIDE_BACK);
    ival2 = attr->getColorMode(VS_MATERIAL_SIDE_BACK);
    if (ival1 != ival2)
        return VS_FALSE;

    return VS_TRUE;
}
