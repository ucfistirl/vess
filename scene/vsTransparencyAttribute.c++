// File vsTransparencyAttribute.c++

#include "vsTransparencyAttribute.h++"

#include <Performer/pr.h>
#include "vsSystem.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes the Performer transparency value
// ------------------------------------------------------------------------
vsTransparencyAttribute::vsTransparencyAttribute() : savedAttr(1, 1, 1)
{
    transpValue = PFTR_BLEND_ALPHA;
    
    attachedFlag = 0;
    saveCount = 0;
}

// ------------------------------------------------------------------------
// VESS internal function
// Constructor - Sets up the attribute as already attached to a node
// ------------------------------------------------------------------------
vsTransparencyAttribute::vsTransparencyAttribute(int type) : savedAttr(1, 1, 1)
{
    if (type == PFTR_OFF)
        transpValue = PFTR_OFF;
    else
        transpValue = PFTR_BLEND_ALPHA;

    attachedFlag = 1;
    saveCount = 0;
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
// Retrieves the category of this attribute
// ------------------------------------------------------------------------
int vsTransparencyAttribute::getAttributeCategory()
{
    return VS_ATTRIBUTE_CATEGORY_STATE;
}

// ------------------------------------------------------------------------
// Enables transparency
// ------------------------------------------------------------------------
void vsTransparencyAttribute::enable()
{
    transpValue = PFTR_BLEND_ALPHA;
}

// ------------------------------------------------------------------------
// Disables transparency
// ------------------------------------------------------------------------
void vsTransparencyAttribute::disable()
{
    transpValue = PFTR_OFF;
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

    savedAttr[saveCount++] = gState->getTransparency();
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
        (vsTransparencyAttribute *)(savedAttr[--saveCount]));
}

// ------------------------------------------------------------------------
// VESS internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsTransparencyAttribute::setState()
{
    pfTransparency(transpValue);
}

// ------------------------------------------------------------------------
// static VESS internal function
// Applies default transparency settings to the graphics library
// ------------------------------------------------------------------------
void vsTransparencyAttribute::setDefault()
{
    pfTransparency(PFTR_OFF);
}
