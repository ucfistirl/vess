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
    osgFog = new osg::Fog();
    osgFog->ref();
    
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
}

// ------------------------------------------------------------------------
// Retrieves the type of equation being used to calculate the fog density
// ------------------------------------------------------------------------
int vsFogAttribute::getEquationType()
{
    switch (osgFog->getMode())
    {
        case osg::Fog::LINEAR:
            return VS_FOG_EQTYPE_LINEAR;
        case osg::Fog::EXP:
            return VS_FOG_EQTYPE_EXP;
        case osg::Fog::EXP2:
            return VS_FOG_EQTYPE_EXP2;
    }
    
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
void vsFogAttribute::setRanges(double near, double far)
{
    osgFog->setStart(near);
    osgFog->setEnd(far);
}

// ------------------------------------------------------------------------
// Retrieves the near and far threshold ranges for the fog. NULL pointers
// may be passed in for undesired values.
// ------------------------------------------------------------------------
void vsFogAttribute::getRanges(double *near, double *far)
{
    if (near)
        *near = osgFog->getStart();
    if (far)
        *far = osgFog->getEnd();
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
    
    attrMode = osg::StateAttribute::ON;

    if (overrideFlag)
        attrMode |= osg::StateAttribute::OVERRIDE;

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
    vsStateAttribute::attach(node);

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

    osgStateSet->setAttributeAndModes(osgFog, osg::StateAttribute::INHERIT);

    vsStateAttribute::detach(node);
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsFogAttribute::attachDuplicate(vsNode *theNode)
{
    vsFogAttribute *newAttrib;

    newAttrib = new vsFogAttribute();
    theNode->addAttribute(newAttrib);

    newAttrib->setEquationType(getEquationType());

    double r, g, b;
    getColor(&r, &g, &b);
    newAttrib->setColor(r, g, b);

    double near, far;
    getRanges(&near, &far);
    newAttrib->setRanges(near, far);
}

// ------------------------------------------------------------------------
// Internal function
// Determines if the specified attribute has state information that is
// equivalent to what this attribute has
// ------------------------------------------------------------------------
int vsFogAttribute::isEquivalent(vsAttribute *attribute)
{
    vsFogAttribute *attr;
    int val1, val2;
    double r1, g1, b1, r2, g2, b2;
    double near1, far1, near2, far2;
    
    if (!attribute)
        return VS_FALSE;

    if (this == attribute)
        return VS_TRUE;
    
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_FOG)
        return VS_FALSE;

    attr = (vsFogAttribute *)attribute;

    val1 = getEquationType();
    val2 = attr->getEquationType();
    if (val1 != val2)
        return VS_FALSE;

    getColor(&r1, &g1, &b1);
    attr->getColor(&r2, &g2, &b2);
    if (!VS_EQUAL(r1,r2) || !VS_EQUAL(g1,g2) || !VS_EQUAL(b1,b2))
        return VS_FALSE;

    getRanges(&near1, &far1);
    attr->getRanges(&near2, &far2);
    if (!VS_EQUAL(near1,near2) || !VS_EQUAL(far1,far2))
        return VS_FALSE;

    return VS_TRUE;
}