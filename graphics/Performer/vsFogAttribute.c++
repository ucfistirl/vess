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
    // Create a new Performer fog object and set the default type and range
    performerFog = new pfFog();
    performerFog->setFogType(PFFOG_PIX_LIN);
    performerFog->setRange(0.0, 10000.0);
    
    // Reference the Performer fog object
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
    // Translate the VESS constant to Performer
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
    // Get the fog type from the Performer fog object and translate to VESS
    switch (performerFog->getFogType())
    {
        case PFFOG_PIX_LIN:
            return VS_FOG_EQTYPE_LINEAR;
        case PFFOG_PIX_EXP:
            return VS_FOG_EQTYPE_EXP;
        case PFFOG_PIX_EXP2:
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
    // Set the fog color on the Performer fog object
    performerFog->setColor(r, g, b);
}

// ------------------------------------------------------------------------
// Retrieves the color of the fog. Null pointers may be passed in for
// undesired data values.
// ------------------------------------------------------------------------
void vsFogAttribute::getColor(double *r, double *g, double *b)
{
    float red, green, blue;
    
    // Get the fog color from the Performer object
    performerFog->getColor(&red, &green, &blue);
    
    // Return the elements of the fog color that the caller desires
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
void vsFogAttribute::setRanges(double nearFog, double farFog)
{
    // Set the fog ranges on the Performer object
    performerFog->setRange(nearFog, farFog);
}

// ------------------------------------------------------------------------
// Retrieves the near and far threshold ranges for the fog. NULL pointers
// may be passed in for undesired values.
// ------------------------------------------------------------------------
void vsFogAttribute::getRanges(double *nearFog, double *farFog)
{
    float onset, opaque;

    // Get the fog ranges from the Perofmer object
    performerFog->getRange(&onset, &opaque);

    // Return the desired fog values
    if (nearFog)
        *nearFog = onset;
    if (farFog)
        *farFog = opaque;
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsFogAttribute::attachDuplicate(vsNode *theNode)
{
    vsFogAttribute *newAttrib;
    double r, g, b;
    double nearFog, farFog;

    // Create a new fog attribute and add it to the given node
    newAttrib = new vsFogAttribute();
    theNode->addAttribute(newAttrib);

    // Copy this attribute's settings to the new attribute
    // Equation type
    newAttrib->setEquationType(getEquationType());

    // Color
    getColor(&r, &g, &b);
    newAttrib->setColor(r, g, b);

    // Ranges
    getRanges(&nearFog, &farFog);
    newAttrib->setRanges(nearFog, farFog);
}

// ------------------------------------------------------------------------
// Internal function
// Saves the current attribute
// ------------------------------------------------------------------------
void vsFogAttribute::saveCurrent()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    // Save the current fog state in our save list
    attrSaveList[attrSaveCount++] = gState->getFog();
}

// ------------------------------------------------------------------------
// Internal function
// Sets the current attribute to this one
// ------------------------------------------------------------------------
void vsFogAttribute::apply()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    // Set the current fog state to this object
    gState->setFog(this);

    // Lock the fog state if overriding is enabled
    if (overrideFlag)
        gState->lockFog(this);
}

// ------------------------------------------------------------------------
// Internal function
// Restores the current attribute to the last saved one
// ------------------------------------------------------------------------
void vsFogAttribute::restoreSaved()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    // Unlock the fog state if overriding was enabled
    if (overrideFlag)
        gState->unlockFog(this);

    // Reset the current fog state to its previous value
    gState->setFog((vsFogAttribute *)(attrSaveList[--attrSaveCount]));
}

// ------------------------------------------------------------------------
// Internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsFogAttribute::setState(pfGeoState *state)
{
    // Set fog to enabled and set the fog object on the Performer geostate
    state->setMode(PFSTATE_ENFOG, PFFOG_ON);
    state->setAttr(PFSTATE_FOG, performerFog);
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
