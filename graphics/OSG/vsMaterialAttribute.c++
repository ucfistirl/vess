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
    // Create an osg::Material object and reference it
    osgMaterial = new osg::Material();
    osgMaterial->ref();
    
    // Set the color tracking mode to OFF (material colors will be used
    // regardless of the current OpenGL color setting
    osgMaterial->setColorMode(osg::Material::OFF);

    // Set ambient, diffuse, specular, and emissive material on front
    // and back to opaque white
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

    // Set the shininess to 0.0
    osgMaterial->setShininess(osg::Material::FRONT, 0.0);
    osgMaterial->setShininess(osg::Material::BACK, 0.0);
}

// ------------------------------------------------------------------------
// Internal function
// Constructor - Stores the given OSG Material object
// ------------------------------------------------------------------------
vsMaterialAttribute::vsMaterialAttribute(osg::Material *material)
{
    // Reference the given material object
    osgMaterial = material;
    osgMaterial->ref();
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsMaterialAttribute::~vsMaterialAttribute()
{
    // Unreference the OSG material
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

    // Set the front material, unless BACK was specified
    if (side != VS_MATERIAL_SIDE_BACK)
    {
        // Select which color we're going to change
        switch (whichColor)
        {
            case VS_MATERIAL_COLOR_AMBIENT:
                // Get the current ambient color
                color = osgMaterial->getAmbient(osg::Material::FRONT);

                // Change the red, green, and blue values to those specified
                color[0] = r;
                color[1] = g;
                color[2] = b;

                // Set the new ambient color
                osgMaterial->setAmbient(osg::Material::FRONT, color);
                break;

            case VS_MATERIAL_COLOR_DIFFUSE:
                // Get the current diffuse color
                color = osgMaterial->getDiffuse(osg::Material::FRONT);

                // Change the red, green, and blue values to those specified
                color[0] = r;
                color[1] = g;
                color[2] = b;

                // Set the new diffuse color
                osgMaterial->setDiffuse(osg::Material::FRONT, color);
                break;

            case VS_MATERIAL_COLOR_SPECULAR:
                // Get the current specular color
                color = osgMaterial->getSpecular(osg::Material::FRONT);

                // Change the red, green, and blue values to those specified
                color[0] = r;
                color[1] = g;
                color[2] = b;

                // Set the new specular color
                osgMaterial->setSpecular(osg::Material::FRONT, color);
                break;

            case VS_MATERIAL_COLOR_EMISSIVE:
                // Get the current emission color
                color = osgMaterial->getEmission(osg::Material::FRONT);

                // Change the red, green, and blue values to those specified
                color[0] = r;
                color[1] = g;
                color[2] = b;

                // Set the new emission color
                osgMaterial->setEmission(osg::Material::FRONT, color);
                break;
        }
    }

    // Set the back material, unless FRONT was specified
    if (side != VS_MATERIAL_SIDE_FRONT)
    {
        switch (whichColor)
        {
            case VS_MATERIAL_COLOR_AMBIENT:
                // Get the current ambient color
                color = osgMaterial->getAmbient(osg::Material::BACK);

                // Change the red, green, and blue values to those specified
                color[0] = r;
                color[1] = g;
                color[2] = b;

                // Set the new ambient color
                osgMaterial->setAmbient(osg::Material::BACK, color);
                break;

            case VS_MATERIAL_COLOR_DIFFUSE:
                // Get the current diffuse color
                color = osgMaterial->getDiffuse(osg::Material::BACK);

                // Change the red, green, and blue values to those specified
                color[0] = r;
                color[1] = g;
                color[2] = b;

                // Set the new diffuse color
                osgMaterial->setDiffuse(osg::Material::BACK, color);
                break;

            case VS_MATERIAL_COLOR_SPECULAR:
                // Get the current specular color
                color = osgMaterial->getSpecular(osg::Material::BACK);

                // Change the red, green, and blue values to those specified
                color[0] = r;
                color[1] = g;
                color[2] = b;

                // Set the new specular color
                osgMaterial->setSpecular(osg::Material::BACK, color);
                break;

            case VS_MATERIAL_COLOR_EMISSIVE:
                // Get the current emission color
                color = osgMaterial->getEmission(osg::Material::BACK);

                // Change the red, green, and blue values to those specified
                color[0] = r;
                color[1] = g;
                color[2] = b;

                // Set the new emission color
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
    
    // Get the color from the BACK, if specified
    if (side == VS_MATERIAL_SIDE_BACK)
    {
        // Select which color we need and get it from the OSG Material
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
    // Use the front color
    else
    {
        // Select which color we need and get it from the OSG Material
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

    // Return each color component if a valid pointer was specified for it
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
    // Set the given alpha value on the OSG material.  Use the side
    // specified (or both if BOTH is specified)
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
    // Return the alpha component of the osg::Material's diffuse color.
    // Fetch the value from the specified side
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
    // Set the specified shininess on the specified side of the osg::Material
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
    // Fetch and return the shininess from the specified side of the 
    // osg::Material
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
    // Translate the VESS color mode to its OSG counterpart, and
    // set the value on the osg::Material
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
    // Fetch the current color mode setting from the osg::Material, and
    // translate it to its VESS equivalent.
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

    // Return 0 if we don't recognize the color mode on the osg::Material
    return 0;
}

// ------------------------------------------------------------------------
// Internal function
// ------------------------------------------------------------------------
void vsMaterialAttribute::setOSGAttrModes(vsNode *node)
{
    unsigned int attrMode;
    osg::StateSet *osgStateSet;
    
    // Start with the osg::StateAttribute mode set to ON
    attrMode = osg::StateAttribute::ON;

    // If the override flag is set on this attribute, change the
    // StateAttribute setting to OVERRIDE
    if (overrideFlag)
        attrMode |= osg::StateAttribute::OVERRIDE;

    // Get the StateSet from the given node
    osgStateSet = getOSGStateSet(node);

    // Apply the osg::Material and osg::StateAttribute setting to the 
    // StateSet
    osgStateSet->setAttributeAndModes(osgMaterial, attrMode);
}

// ------------------------------------------------------------------------
// Internal function
// ------------------------------------------------------------------------
void vsMaterialAttribute::attach(vsNode *node)
{
    // Do standard vsStateAttribute attaching
    vsStateAttribute::attach(node);

    // Set the OSG attribute and modes on the attaching node
    setOSGAttrModes(node);
}

// ------------------------------------------------------------------------
// Internal function
// ------------------------------------------------------------------------
void vsMaterialAttribute::detach(vsNode *node)
{
    osg::StateSet *osgStateSet;

    // Get the StateSet from the attached node
    osgStateSet = getOSGStateSet(node);

    // Set the Material attribute on the StateSet back to INHERIT
    osgStateSet->setAttributeAndModes(osgMaterial,
        osg::StateAttribute::INHERIT);

    // Finish with vsStateAttribute detaching process
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
    
    // Create a new osg::Material, copying the current osg::Material
    newMaterial = new osg::Material(*osgMaterial);

    // Create a new vsMaterial attribute, using the new osg::Material
    newAttrib = new vsMaterialAttribute(newMaterial);

    // Attach the new attribute to the given node
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
    
    // Make sure the given attribute exists
    if (!attribute)
        return VS_FALSE;

    // Check for pointer equality (if we're comparing the attribute to
    // itself)
    if (this == attribute)
        return VS_TRUE;
    
    // Check the attribute type
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_MATERIAL)
        return VS_FALSE;

    // Cast the given attribute to a vsMaterialAttribute
    attr = (vsMaterialAttribute *)attribute;

    // Compare the front ambient colors
    getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_AMBIENT,
        &r1, &g1, &b1);
    attr->getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_AMBIENT,
        &r2, &g2, &b2);
    if (!VS_EQUAL(r1,r2) || !VS_EQUAL(g1,g2) || !VS_EQUAL(b1,b2))
        return VS_FALSE;

    // Compare the back ambient colors
    getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_AMBIENT,
        &r1, &g1, &b1);
    attr->getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_AMBIENT,
        &r2, &g2, &b2);
    if (!VS_EQUAL(r1,r2) || !VS_EQUAL(g1,g2) || !VS_EQUAL(b1,b2))
        return VS_FALSE;

    // Compare the front diffuse colors
    getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_DIFFUSE,
        &r1, &g1, &b1);
    attr->getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_DIFFUSE,
        &r2, &g2, &b2);
    if (!VS_EQUAL(r1,r2) || !VS_EQUAL(g1,g2) || !VS_EQUAL(b1,b2))
        return VS_FALSE;

    // Compare the back diffuse colors
    getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_DIFFUSE,
        &r1, &g1, &b1);
    attr->getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_DIFFUSE,
        &r2, &g2, &b2);
    if (!VS_EQUAL(r1,r2) || !VS_EQUAL(g1,g2) || !VS_EQUAL(b1,b2))
        return VS_FALSE;

    // Compare the front specular colors
    getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_SPECULAR,
        &r1, &g1, &b1);
    attr->getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_SPECULAR,
        &r2, &g2, &b2);
    if (!VS_EQUAL(r1,r2) || !VS_EQUAL(g1,g2) || !VS_EQUAL(b1,b2))
        return VS_FALSE;

    // Compare the back specular colors
    getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_SPECULAR,
        &r1, &g1, &b1);
    attr->getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_SPECULAR,
        &r2, &g2, &b2);
    if (!VS_EQUAL(r1,r2) || !VS_EQUAL(g1,g2) || !VS_EQUAL(b1,b2))
        return VS_FALSE;

    // Compare the front emissive colors
    getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_EMISSIVE,
        &r1, &g1, &b1);
    attr->getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_EMISSIVE,
        &r2, &g2, &b2);
    if (!VS_EQUAL(r1,r2) || !VS_EQUAL(g1,g2) || !VS_EQUAL(b1,b2))
        return VS_FALSE;

    // Compare the back emissive colors
    getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_EMISSIVE,
        &r1, &g1, &b1);
    attr->getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_EMISSIVE,
        &r2, &g2, &b2);
    if (!VS_EQUAL(r1,r2) || !VS_EQUAL(g1,g2) || !VS_EQUAL(b1,b2))
        return VS_FALSE;

    // Compare the front alpha values
    val1 = getAlpha(VS_MATERIAL_SIDE_FRONT);
    val2 = attr->getAlpha(VS_MATERIAL_SIDE_FRONT);
    if (!VS_EQUAL(val1,val2))
        return VS_FALSE;

    // Compare the back alpha values
    val1 = getAlpha(VS_MATERIAL_SIDE_BACK);
    val2 = attr->getAlpha(VS_MATERIAL_SIDE_BACK);
    if (!VS_EQUAL(val1,val2))
        return VS_FALSE;

    // Compare the front shininess values
    val1 = getShininess(VS_MATERIAL_SIDE_FRONT);
    val2 = attr->getShininess(VS_MATERIAL_SIDE_FRONT);
    if (!VS_EQUAL(val1,val2))
        return VS_FALSE;

    // Compare the back shininess values
    val1 = getShininess(VS_MATERIAL_SIDE_BACK);
    val2 = attr->getShininess(VS_MATERIAL_SIDE_BACK);
    if (!VS_EQUAL(val1,val2))
        return VS_FALSE;

    // Compare the front color mode values
    ival1 = getColorMode(VS_MATERIAL_SIDE_FRONT);
    ival2 = attr->getColorMode(VS_MATERIAL_SIDE_FRONT);
    if (ival1 != ival2)
        return VS_FALSE;

    // Compare the back color mode values
    ival1 = getColorMode(VS_MATERIAL_SIDE_BACK);
    ival2 = attr->getColorMode(VS_MATERIAL_SIDE_BACK);
    if (ival1 != ival2)
        return VS_FALSE;

    // If we get all the way here, the materials are equivalent
    return VS_TRUE;
}
