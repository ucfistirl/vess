// File vsFogAttribute.c++

#include "vsFogAttribute.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes the attribute to default values
// ------------------------------------------------------------------------
vsFogAttribute::vsFogAttribute()
{
    performerFog = new pfFog();
    performerFog->setFogType(PFFOG_PIX_LIN);
    performerFog->setRange(0.0, 10000.0);
}

// ------------------------------------------------------------------------
// VESS internal function
// Constructor - Initialized the attribute from data contained in the
// given Performer fog object
// ------------------------------------------------------------------------
vsFogAttribute::vsFogAttribute(pfFog *fogObject)
{
    performerFog = fogObject;
    
    if (performerFog->getFogType() == PFFOG_PIX_SPLINE)
    {
        performerFog->setFogType(PFFOG_PIX_LIN);
        performerFog->setRange(0.0, 10000.0);
    }

    attachedFlag = 1;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsFogAttribute::~vsFogAttribute()
{
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
// VESS internal function
// Saves the current graphics library settings
// ------------------------------------------------------------------------
void vsFogAttribute::saveCurrent()
{
    if (pfGetEnable(PFEN_FOG))
        savedFog = pfGetCurFog();
    else
        savedFog = NULL;
}

// ------------------------------------------------------------------------
// VESS internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsFogAttribute::apply()
{
    if (!savedFog)
        pfEnable(PFEN_FOG);

    performerFog->apply();
}

// ------------------------------------------------------------------------
// VESS internal function
// Restores the graphics library settings to the saved values
// ------------------------------------------------------------------------
void vsFogAttribute::restoreSaved()
{
    if (savedFog)
        savedFog->apply();
    else
        pfDisable(PFEN_FOG);
}
