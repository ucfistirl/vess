// File vsBackfaceAttribute.c++

#include "vsBackfaceAttribute.h++"

#include <Performer/pr/pfLight.h>
#include <Performer/pr.h>
#include "vsSystem.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes backfacing to false
// ------------------------------------------------------------------------
vsBackfaceAttribute::vsBackfaceAttribute() : backAttrSave(1, 1, 1)
{
    backfaceVal = 0;
    
    saveCount = 0;
}

// ------------------------------------------------------------------------
// VESS internal function
// Constructor - Initializes backfacing to the given value
// ------------------------------------------------------------------------
vsBackfaceAttribute::vsBackfaceAttribute(int initVal) : backAttrSave(1, 1, 1)
{
    backfaceVal = initVal;

    attachedFlag = 1;
    saveCount = 0;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsBackfaceAttribute::~vsBackfaceAttribute()
{
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsBackfaceAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_BACKFACE;
}

// ------------------------------------------------------------------------
// Retrieves the category of this attribute
// ------------------------------------------------------------------------
int vsBackfaceAttribute::getAttributeCategory()
{
    return VS_ATTRIBUTE_CATEGORY_STATE;
}

// ------------------------------------------------------------------------
// Enables backfacing
// ------------------------------------------------------------------------
void vsBackfaceAttribute::enable()
{
    backfaceVal = 1;
}

// ------------------------------------------------------------------------
// Disables backfacing
// ------------------------------------------------------------------------
void vsBackfaceAttribute::disable()
{
    backfaceVal = 0;
}

// ------------------------------------------------------------------------
// Retrieves a flag stating if backfacing is enabled
// ------------------------------------------------------------------------
int vsBackfaceAttribute::isEnabled()
{
    return backfaceVal;
}

// ------------------------------------------------------------------------
// VESS internal function
// Saves the current attribute
// ------------------------------------------------------------------------
void vsBackfaceAttribute::saveCurrent()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    backAttrSave[saveCount++] = gState->getBackface();
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

    gState->setBackface((vsBackfaceAttribute *)(backAttrSave[--saveCount]));
}

// ------------------------------------------------------------------------
// VESS internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsBackfaceAttribute::setState()
{
    if (backfaceVal)
    {
        pfCullFace(PFCF_OFF);
        (pfGetCurLModel())->setTwoSide(PF_ON);
    }
    else
    {
        pfCullFace(PFCF_BACK);
        (pfGetCurLModel())->setTwoSide(PF_OFF);
    }
}

// ------------------------------------------------------------------------
// static VESS internal function
// Applies default backface settings to the graphics library
// ------------------------------------------------------------------------
void vsBackfaceAttribute::setDefault()
{
    pfCullFace(PFCF_BACK);
    (pfGetCurLModel())->setTwoSide(PF_OFF);
}
