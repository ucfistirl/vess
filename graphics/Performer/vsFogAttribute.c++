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

#include "vsFogAttribute.h++"

#include "vsNode.h++"
#include "vsGraphicsState.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes the attribute to default values
// ------------------------------------------------------------------------
vsFogAttribute::vsFogAttribute()
{
    performerFog = new pfFog();
    performerFog->setFogType(PFFOG_PIX_LIN);
    performerFog->setRange(0.0, 10000.0);
    
    performerFog->ref();
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsFogAttribute::~vsFogAttribute()
{
    performerFog->unref();
    pfDelete(performerFog);
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
            performerFog->setFogType(PFFOG_PIX_LIN);
            break;
        case VS_FOG_EQTYPE_EXP:
            performerFog->setFogType(PFFOG_PIX_EXP);
            break;
        case VS_FOG_EQTYPE_EXP2:
            performerFog->setFogType(PFFOG_PIX_EXP2);
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
    switch (performerFog->getFogType())
    {
        case PFFOG_PIX_LIN:
            return VS_FOG_EQTYPE_LINEAR;
        case PFFOG_PIX_EXP:
            return VS_FOG_EQTYPE_EXP;
        case PFFOG_PIX_EXP2:
            return VS_FOG_EQTYPE_EXP2;
    }
    
    return 0;
}

// ------------------------------------------------------------------------
// Sets the color of the fog
// ------------------------------------------------------------------------
void vsFogAttribute::setColor(double r, double g, double b)
{
    performerFog->setColor(r, g, b);
}

// ------------------------------------------------------------------------
// Retrieves the color of the fog. Null pointers may be passed in for
// undesired data values.
// ------------------------------------------------------------------------
void vsFogAttribute::getColor(double *r, double *g, double *b)
{
    float red, green, blue;
    
    performerFog->getColor(&red, &green, &blue);
    
    if (r)
        *r = red;
    if (g)
        *g = green;
    if (b)
        *b = blue;
}

// ------------------------------------------------------------------------
// Sets the near and far threshold ranges for the fog
// ------------------------------------------------------------------------
void vsFogAttribute::setRanges(double near, double far)
{
    performerFog->setRange(near, far);
}

// ------------------------------------------------------------------------
// Retrieves the near and far threshold ranges for the fog. NULL pointers
// may be passed in for undesired values.
// ------------------------------------------------------------------------
void vsFogAttribute::getRanges(double *near, double *far)
{
    float onset, opaque;

    performerFog->getRange(&onset, &opaque);

    if (near)
        *near = onset;
    if (far)
        *far = opaque;
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
// Saves the current attribute
// ------------------------------------------------------------------------
void vsFogAttribute::saveCurrent()
{
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    attrSaveList[attrSaveCount++] = gState->getFog();
}

// ------------------------------------------------------------------------
// Internal function
// Sets the current attribute to this one
// ------------------------------------------------------------------------
void vsFogAttribute::apply()
{
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    gState->setFog(this);
    if (overrideFlag)
        gState->lockFog(this);
}

// ------------------------------------------------------------------------
// Internal function
// Restores the current attribute to the last saved one
// ------------------------------------------------------------------------
void vsFogAttribute::restoreSaved()
{
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    if (overrideFlag)
        gState->unlockFog(this);
    gState->setFog((vsFogAttribute *)(attrSaveList[--attrSaveCount]));
}

// ------------------------------------------------------------------------
// Internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsFogAttribute::setState(pfGeoState *state)
{
    state->setMode(PFSTATE_ENFOG, PFFOG_ON);
    state->setAttr(PFSTATE_FOG, performerFog);
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
