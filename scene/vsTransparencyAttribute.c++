// File vsTransparencyAttribute.c++

#include "vsTransparencyAttribute.h++"

#include <Performer/pr.h>
#include "vsSystem.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes the Performer transparency value
// ------------------------------------------------------------------------
vsTransparencyAttribute::vsTransparencyAttribute()
{
    transpValue = PFTR_BLEND_ALPHA;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsTransparencyAttribute::~vsTransparencyAttribute()
{
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsTransparencyAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_TRANSPARENCY;
}

// ------------------------------------------------------------------------
// Enables transparency
// ------------------------------------------------------------------------
void vsTransparencyAttribute::enable()
{
    transpValue = PFTR_BLEND_ALPHA;
    
    markOwnersDirty();
}

// ------------------------------------------------------------------------
// Disables transparency
// ------------------------------------------------------------------------
void vsTransparencyAttribute::disable()
{
    transpValue = PFTR_OFF;
    
    markOwnersDirty();
}

// ------------------------------------------------------------------------
// Returns a flag specifying is transparency is enabled
// ------------------------------------------------------------------------
int vsTransparencyAttribute::isEnabled()
{
    if (transpValue == PFTR_BLEND_ALPHA)
        return VS_TRUE;
    else
        return VS_FALSE;
}

// ------------------------------------------------------------------------
// VESS internal function
// Saves the current attribute
// ------------------------------------------------------------------------
void vsTransparencyAttribute::saveCurrent()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    attrSaveList[attrSaveCount++] = gState->getTransparency();
}

// ------------------------------------------------------------------------
// VESS internal function
// Sets the current attribute to this one
// ------------------------------------------------------------------------
void vsTransparencyAttribute::apply()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    gState->setTransparency(this);
}

// ------------------------------------------------------------------------
// VESS internal function
// Restores the current attribute to the last saved one
// ------------------------------------------------------------------------
void vsTransparencyAttribute::restoreSaved()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    gState->setTransparency(
        (vsTransparencyAttribute *)(attrSaveList[--attrSaveCount]));
}

// ------------------------------------------------------------------------
// VESS internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsTransparencyAttribute::setState(pfGeoState *state)
{
    state->setMode(PFSTATE_TRANSPARENCY, transpValue);
}
