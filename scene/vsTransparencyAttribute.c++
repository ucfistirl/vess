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
//    VESS Module:  vsTransparencyAttribute.c++
//
//    Description:  Attribute that specifies that geometry contains
//                  transparent or translucent parts and should be drawn
//                  accordingly
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsTransparencyAttribute.h++"

#include <Performer/pr.h>
#include "vsSystem.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes the Performer transparency value
// ------------------------------------------------------------------------
vsTransparencyAttribute::vsTransparencyAttribute()
{
    quality = VS_TRANSP_QUALITY_DEFAULT;
    occlusion = VS_TRUE;
    transpValue = PFTR_ON;
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
    switch (quality)
    {
        case VS_TRANSP_QUALITY_DEFAULT:
            transpValue = PFTR_ON;
            break;
        case VS_TRANSP_QUALITY_FAST:
            transpValue = PFTR_FAST;
            break;
        case VS_TRANSP_QUALITY_HIGH:
            transpValue = PFTR_HIGH_QUALITY;
            break;
    }

    if (occlusion == VS_FALSE)
        transpValue |= PFTR_NO_OCCLUDE;
    
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
// Returns a flag specifying if transparency is enabled
// ------------------------------------------------------------------------
int vsTransparencyAttribute::isEnabled()
{
    if (transpValue != PFTR_OFF)
        return VS_TRUE;
    else
        return VS_FALSE;
}

// ------------------------------------------------------------------------
// Sets the quality of the transparency rendering calculation
// ------------------------------------------------------------------------
void vsTransparencyAttribute::setQuality(int newQuality)
{
    if ((newQuality != VS_TRANSP_QUALITY_DEFAULT) &&
        (newQuality != VS_TRANSP_QUALITY_FAST) &&
        (newQuality != VS_TRANSP_QUALITY_HIGH))
    {
        printf("vsTransparencyAttribute::setQuality: Unrecognized quality "
            "constant\n");
        return;
    }

    quality = newQuality;

    if (isEnabled())
        enable();
}

// ------------------------------------------------------------------------
// Gets the quality of the transparency rendering calculation
// ------------------------------------------------------------------------
int vsTransparencyAttribute::getQuality()
{
    return quality;
}

// ------------------------------------------------------------------------
// Enables the z-buffer when drawing transparent geometry
// ------------------------------------------------------------------------
void vsTransparencyAttribute::enableOcclusion()
{
    occlusion = VS_TRUE;

    if (isEnabled())
        enable();
}

// ------------------------------------------------------------------------
// Disables the z-buffer when drawing transparent geometry
// ------------------------------------------------------------------------
void vsTransparencyAttribute::disableOcclusion()
{
    occlusion = VS_FALSE;

    if (isEnabled())
        enable();
}

// ------------------------------------------------------------------------
// Returns a flag specifying if occlusion is enabled
// ------------------------------------------------------------------------
int vsTransparencyAttribute::isOcclusionEnabled()
{
    return occlusion;
}

// ------------------------------------------------------------------------
// VESS internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsTransparencyAttribute::attachDuplicate(vsNode *theNode)
{
    vsTransparencyAttribute *newAttrib;
    
    newAttrib = new vsTransparencyAttribute();
    
    if (isEnabled())
        newAttrib->enable();
    else
        newAttrib->disable();
    newAttrib->setQuality(getQuality());

    theNode->addAttribute(newAttrib);
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
    if (overrideFlag)
        gState->lockTransparency(this);
}

// ------------------------------------------------------------------------
// VESS internal function
// Restores the current attribute to the last saved one
// ------------------------------------------------------------------------
void vsTransparencyAttribute::restoreSaved()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    if (overrideFlag)
        gState->unlockTransparency(this);
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

// ------------------------------------------------------------------------
// VESS internal function
// Determines if the specified attribute has state information that is
// equivalent to what this attribute has
// ------------------------------------------------------------------------
int vsTransparencyAttribute::isEquivalent(vsAttribute *attribute)
{
    vsTransparencyAttribute *attr;
    int val1, val2;
    
    if (!attribute)
        return VS_FALSE;

    if (this == attribute)
        return VS_TRUE;
    
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_TRANSPARENCY)
        return VS_FALSE;

    attr = (vsTransparencyAttribute *)attribute;

    val1 = isEnabled();
    val2 = attr->isEnabled();
    if (val1 != val2)
        return VS_FALSE;

    val1 = getQuality();
    val2 = attr->getQuality();
    if (val1 != val2)
        return VS_FALSE;

    val1 = isOcclusionEnabled();
    val2 = attr->isOcclusionEnabled();
    if (val1 != val2)
        return VS_FALSE;

    return VS_TRUE;
}
