// File vsBackfaceAttribute.c++

#include "vsBackfaceAttribute.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes backfacing to false
// ------------------------------------------------------------------------
vsBackfaceAttribute::vsBackfaceAttribute()
{
    backfaceVal = 0;
}

// ------------------------------------------------------------------------
// VESS internal function
// Constructor - Initializes backfacing to the given value
// ------------------------------------------------------------------------
vsBackfaceAttribute::vsBackfaceAttribute(int initVal)
{
    backfaceVal = initVal;
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
// Saves the current graphics library settings
// ------------------------------------------------------------------------
void vsBackfaceAttribute::saveCurrent()
{
    backCullSave = pfGetCullFace();
    backLightSave = (pfGetCurLModel())->getTwoSide();
}

// ------------------------------------------------------------------------
// VESS internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsBackfaceAttribute::apply()
{
    if (backfaceVal)
    {
        pfCullFace(PFCF_BACK);
        (pfGetCurLModel())->setTwoSide(PF_ON);
    }
    else
    {
        pfCullFace(PFCF_OFF);
        (pfGetCurLModel())->setTwoSide(PF_OFF);
    }
}

// ------------------------------------------------------------------------
// VESS internal function
// Restores the graphics library settings to the saved values
// ------------------------------------------------------------------------
void vsBackfaceAttribute::restoreSaved()
{
    pfCullFace(backCullSave);
    (pfGetCurLModel())->setTwoSide(backLightSave);
}
