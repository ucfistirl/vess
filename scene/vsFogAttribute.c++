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

#include "vsSystem.h++"

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
// VESS internal function
// Constructor - Initializes the attribute from data contained in the
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
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsFogAttribute::attachDuplicate(vsNode *theNode)
{
    vsFogAttribute *newAttrib;
    pfFog *newFog;
    
    newFog = new pfFog();
    newFog->copy(performerFog);
    
    newAttrib = new vsFogAttribute(newFog);

    theNode->addAttribute(newAttrib);
}

// ------------------------------------------------------------------------
// VESS internal function
// Saves the current attribute
// ------------------------------------------------------------------------
void vsFogAttribute::saveCurrent()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    attrSaveList[attrSaveCount++] = gState->getFog();
}

// ------------------------------------------------------------------------
// VESS internal function
// Sets the current attribute to this one
// ------------------------------------------------------------------------
void vsFogAttribute::apply()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    gState->setFog(this);
}

// ------------------------------------------------------------------------
// VESS internal function
// Restores the current attribute to the last saved one
// ------------------------------------------------------------------------
void vsFogAttribute::restoreSaved()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    gState->setFog((vsFogAttribute *)(attrSaveList[--attrSaveCount]));
}

// ------------------------------------------------------------------------
// VESS internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsFogAttribute::setState(pfGeoState *state)
{
    state->setMode(PFSTATE_ENFOG, PFFOG_ON);
    state->setAttr(PFSTATE_FOG, performerFog);
}
