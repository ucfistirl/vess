// File vsTransparencyAttribute.c++

#include "vsTransparencyAttribute.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes the Performer transparency value
// ------------------------------------------------------------------------
vsTransparencyAttribute::vsTransparencyAttribute()
{
    transpValue = PFTR_BLEND_ALPHA;
}

// ------------------------------------------------------------------------
// VESS internal function
// Constructor - Sets up the attribute as already attached to a node
// ------------------------------------------------------------------------
vsTransparencyAttribute::vsTransparencyAttribute(int type)
{
    if (type == PFTR_OFF)
        transpValue = PFTR_OFF;
    else
        transpValue = PFTR_BLEND_ALPHA;

    attachedFlag = 1;
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
// Saves the current graphics library settings
// ------------------------------------------------------------------------
void vsTransparencyAttribute::saveCurrent()
{
    savedValue = pfGetTransparency();
}

// ------------------------------------------------------------------------
// VESS internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsTransparencyAttribute::apply()
{
    pfTransparency(transpValue);
}

// ------------------------------------------------------------------------
// VESS internal function
// Restores the graphics library settings to the saved values
// ------------------------------------------------------------------------
void vsTransparencyAttribute::restoreSaved()
{
    pfTransparency(savedValue);
}
