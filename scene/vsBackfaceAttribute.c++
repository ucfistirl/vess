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
//    VESS Module:  vsBackfaceAttribute.h++
//
//    Description:  Attribute for specifying the visibility of back-facing
//                  geometry
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsBackfaceAttribute.h++"

#include <Performer/pr/pfLight.h>
#include <Performer/pr.h>
#include "vsSystem.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes backfacing to false
// ------------------------------------------------------------------------
vsBackfaceAttribute::vsBackfaceAttribute()
{
    // Set the attribute to the default OFF settings
    lightModel = new pfLightModel();
    lightModel->setLocal(PF_ON);
    lightModel->setTwoSide(PF_OFF);
    lightModel->setAmbient(0.0, 0.0, 0.0);
    lightModel->ref();
    
    cullfaceVal = PFCF_BACK;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsBackfaceAttribute::~vsBackfaceAttribute()
{
    lightModel->unref();
    pfDelete(lightModel);
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsBackfaceAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_BACKFACE;
}

// ------------------------------------------------------------------------
// Enables backfacing
// ------------------------------------------------------------------------
void vsBackfaceAttribute::enable()
{
    lightModel->setTwoSide(PF_ON);
    cullfaceVal = PFCF_OFF;
    
    markOwnersDirty();
}

// ------------------------------------------------------------------------
// Disables backfacing
// ------------------------------------------------------------------------
void vsBackfaceAttribute::disable()
{
    lightModel->setTwoSide(PF_OFF);
    cullfaceVal = PFCF_BACK;

    markOwnersDirty();
}

// ------------------------------------------------------------------------
// Retrieves a flag stating if backfacing is enabled
// ------------------------------------------------------------------------
int vsBackfaceAttribute::isEnabled()
{
    if (cullfaceVal == PFCF_OFF)
        return VS_TRUE;

    return VS_FALSE;
}

// ------------------------------------------------------------------------
// VESS internal function
// Saves the current attribute
// ------------------------------------------------------------------------
void vsBackfaceAttribute::saveCurrent()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    attrSaveList[attrSaveCount++] = gState->getBackface();
}

// ------------------------------------------------------------------------
// VESS internal function
// Sets the current attribute to this one
// ------------------------------------------------------------------------
void vsBackfaceAttribute::apply()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    gState->setBackface(this);
}

// ------------------------------------------------------------------------
// VESS internal function
// Restores the current attribute to the last saved one
// ------------------------------------------------------------------------
void vsBackfaceAttribute::restoreSaved()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    gState->setBackface((vsBackfaceAttribute *)(attrSaveList[--attrSaveCount]));
}

// ------------------------------------------------------------------------
// VESS internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsBackfaceAttribute::setState(pfGeoState *state)
{
    state->setMode(PFSTATE_CULLFACE, cullfaceVal);
    state->setAttr(PFSTATE_LIGHTMODEL, lightModel);
}
