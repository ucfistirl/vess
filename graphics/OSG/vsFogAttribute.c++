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
//    VESS Module:  vsFogAttribute.c++
//
//    Description:  Specifies that geometry be drawn with fog effects
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsFogAttribute.h++"
#include "vsNode.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes the attribute to default values
// ------------------------------------------------------------------------
vsFogAttribute::vsFogAttribute()
{
    // Create a new osg Fog object
    osgFog = new osg::Fog();
    osgFog->ref();
    
    // Set defaults
    osgFog->setMode(osg::Fog::LINEAR);
    osgFog->setDensity(1.0);
    osgFog->setStart(0.0);
    osgFog->setEnd(10000.0);
    osgFog->setColor(osg::Vec4(1.0, 1.0, 1.0, 1.0));
    osgFog->setFogCoordinateSource(osg::Fog::FRAGMENT_DEPTH);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsFogAttribute::~vsFogAttribute()
{
    // Delete the osg Fog object
    osgFog->unref();
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsFogAttribute::getClassName()
{
    return "vsFogAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsFogAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_FOG;
}

// ------------------------------------------------------------------------
// Sets the type of equation used to calculate the fog density
// ------------------------------------------------------------------------
void vsFogAttribute::setEquationType(int equType)
{
    // Translate the VESS constant to OSG
    switch (equType)
    {
        case VS_FOG_EQTYPE_LINEAR:
            osgFog->setMode(osg::Fog::LINEAR);
            break;
        case VS_FOG_EQTYPE_EXP:
            osgFog->setMode(osg::Fog::EXP);
            break;
        case VS_FOG_EQTYPE_EXP2:
            osgFog->setMode(osg::Fog::EXP2);
            break;
        default:
            printf("vsFogAttribute::setEquationType: Unrecognized equation "
                "type\n");
            break;
    }

    recalcDensity();
}

// ------------------------------------------------------------------------
// Retrieves the type of equation being used to calculate the fog density
// ------------------------------------------------------------------------
int vsFogAttribute::getEquationType()
{
    // Translate the OSG fog mode to VESS
    switch (osgFog->getMode())
    {
        case osg::Fog::LINEAR:
            return VS_FOG_EQTYPE_LINEAR;
        case osg::Fog::EXP:
            return VS_FOG_EQTYPE_EXP;
        case osg::Fog::EXP2:
            return VS_FOG_EQTYPE_EXP2;
    }
    
    // If the fog type is unrecognized, return a default value
    return 0;
}

// ------------------------------------------------------------------------
// Sets the color of the fog
// ------------------------------------------------------------------------
void vsFogAttribute::setColor(double r, double g, double b)
{
    osgFog->setColor(osg::Vec4(r, g, b, 1.0));
}

// ------------------------------------------------------------------------
// Retrieves the color of the fog. Null pointers may be passed in for
// undesired data values.
// ------------------------------------------------------------------------
void vsFogAttribute::getColor(double *r, double *g, double *b)
{
    osg::Vec4 color;
    
    color = osgFog->getColor();
    
    // Return the elements of the fog color that the caller desires
    if (r)
        *r = color[0];
    if (g)
        *g = color[1];
    if (b)
        *b = color[2];
}

// ------------------------------------------------------------------------
// Sets the near and far threshold ranges for the fog
// ------------------------------------------------------------------------
void vsFogAttribute::setRanges(double nearDist, double farDist)
{
    osgFog->setStart(nearDist);
    osgFog->setEnd(farDist);

    recalcDensity();
}

// ------------------------------------------------------------------------
// Retrieves the near and far threshold ranges for the fog. NULL pointers
// may be passed in for undesired values.
// ------------------------------------------------------------------------
void vsFogAttribute::getRanges(double *nearDist, double *farDist)
{
    if (nearDist)
        *nearDist = osgFog->getStart();
    if (farDist)
        *farDist = osgFog->getEnd();
}

// ------------------------------------------------------------------------
// Private function
// Recalculates the GL fog density value for this attribute, using the far
// fog range and the fog calculation mode values.
// ------------------------------------------------------------------------
void vsFogAttribute::recalcDensity()
{
    //                                          -(density * z)
    // The equation for exponential fog is f = e              , where f is
    // the visiblity of the fogged object (in the range 0.0 [completely
    // obscured] to 1.0 [completely visible]), and z is the distance from
    // that object to the viewer. In order to calculate the density value,
    // we assume a very small value for f (not zero, because that would
    // require that the exponent be infinite), and take z to be the far
    // fog range value. Both of those values plugged into the fog equation
    // yield an equation for the desired density. (The procedure for the
    // exponential-squared fog equation is similar, with just the exponent
    // (density * z) being squared.)

    double farFogRange;
    double noVisibilityConstant = 0.01;
    double density;

    // Obtain the far fog range
    getRanges(NULL, &farFogRange);

    // Calculate the density based on the fog equation mode
    switch (getEquationType())
    {
        case VS_FOG_EQTYPE_LINEAR:
           // The linear fog mode doesn't use the density value; set it to
           // a default
           osgFog->setDensity(1.0);
           break;

        case VS_FOG_EQTYPE_EXP:
           // density = -(ln f) / z
           density = (-1.0 * log(noVisibilityConstant)) / farFogRange;
           osgFog->setDensity(density);
           break;

        case VS_FOG_EQTYPE_EXP2:
           // density = -sqrt(ln f) / z
           density = (-1.0 * sqrt(log(noVisibilityConstant))) / farFogRange;
           osgFog->setDensity(density);
           break;
    }
}

// ------------------------------------------------------------------------
// Private function
// Sets the modes on the StateSet of this node's OSG node to reflect the
// settings of this attribute
// ------------------------------------------------------------------------
void vsFogAttribute::setOSGAttrModes(vsNode *node)
{
    unsigned int attrMode;
    osg::StateSet *osgStateSet;
    
    // Calculate the attribute mode
    attrMode = osg::StateAttribute::ON;

    if (overrideFlag)
        attrMode |= osg::StateAttribute::OVERRIDE;

    // Set the Fog object on the node's StateSet using the calculated mode
    osgStateSet = getOSGStateSet(node);

    osgStateSet->setAttributeAndModes(osgFog, attrMode);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsFogAttribute::attach(vsNode *node)
{
    // Inherited attach
    vsStateAttribute::attach(node);

    // Update the new owner's osg StateSet
    setOSGAttrModes(node);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsFogAttribute::detach(vsNode *node)
{
    osg::StateSet *osgStateSet;
    osgStateSet = getOSGStateSet(node);

    // Setting the mode to INHERIT should remove this attribute from
    // the StateSet entirely
    osgStateSet->setAttributeAndModes(osgFog, osg::StateAttribute::INHERIT);

    // Inherited detach
    vsStateAttribute::detach(node);
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsFogAttribute::attachDuplicate(vsNode *theNode)
{
    vsFogAttribute *newAttrib;
    double r, g, b;
    double nearDist, farDist;

    // Create a new fog attribute on the specified node
    newAttrib = new vsFogAttribute();
    theNode->addAttribute(newAttrib);

    // Copy the fog parameters to the new attribute
    newAttrib->setEquationType(getEquationType());
    getColor(&r, &g, &b);
    newAttrib->setColor(r, g, b);
    getRanges(&nearDist, &farDist);
    newAttrib->setRanges(nearDist, farDist);
}

// ------------------------------------------------------------------------
// Internal function
// Determines if the specified attribute has state information that is
// equivalent to what this attribute has
// ------------------------------------------------------------------------
bool vsFogAttribute::isEquivalent(vsAttribute *attribute)
{
    vsFogAttribute *attr;
    int val1, val2;
    double r1, g1, b1, r2, g2, b2;
    double near1, far1, near2, far2;
    
    // NULL check
    if (!attribute)
        return false;

    // Equal pointer check
    if (this == attribute)
        return true;
    
    // Type check
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_FOG)
        return false;

    // Type cast
    attr = (vsFogAttribute *)attribute;

    // Equation type check
    val1 = getEquationType();
    val2 = attr->getEquationType();
    if (val1 != val2)
        return false;

    // Color check
    getColor(&r1, &g1, &b1);
    attr->getColor(&r2, &g2, &b2);
    if (!VS_EQUAL(r1,r2) || !VS_EQUAL(g1,g2) || !VS_EQUAL(b1,b2))
        return false;

    // Range check
    getRanges(&near1, &far1);
    attr->getRanges(&near2, &far2);
    if (!VS_EQUAL(near1,near2) || !VS_EQUAL(far1,far2))
        return false;

    // Attributes are equivalent if all checks pass
    return true;
}
