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
    // Initialize the attribute to its default value of enabled
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
    // Set the transparency value based on our quality value
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

    // Modify the transparency value if occlusion is disabled
    if (occlusion == VS_FALSE)
        transpValue |= PFTR_NO_OCCLUDE;
    
    // Mark the nodes that have this attribute attached as needing
    // of an update
    markOwnersDirty();
}

// ------------------------------------------------------------------------
// Disables transparency
// ------------------------------------------------------------------------
void vsTransparencyAttribute::disable()
{
    // Set the transparency value to off
    transpValue = PFTR_OFF;
    
    // Mark the nodes that have this attribute attached as needing
    // of an update
    markOwnersDirty();
}

// ------------------------------------------------------------------------
// Returns a flag specifying if transparency is enabled
// ------------------------------------------------------------------------
int vsTransparencyAttribute::isEnabled()
{
    // Interpret the transparency value
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
    // Sanity checking
    if ((newQuality != VS_TRANSP_QUALITY_DEFAULT) &&
        (newQuality != VS_TRANSP_QUALITY_FAST) &&
        (newQuality != VS_TRANSP_QUALITY_HIGH))
    {
        printf("vsTransparencyAttribute::setQuality: Unrecognized quality "
            "constant\n");
        return;
    }

    // Store the quality value
    quality = newQuality;

    // If transparency is currently enabled, then call the enable()
    // function to recompute the transparency value
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
    // Set the occlusion value
    occlusion = VS_TRUE;

    // If transparency is currently enabled, then call the enable()
    // function to recompute the transparency value
    if (isEnabled())
        enable();
}

// ------------------------------------------------------------------------
// Disables the z-buffer when drawing transparent geometry
// ------------------------------------------------------------------------
void vsTransparencyAttribute::disableOcclusion()
{
    // Set the occlusion value
    occlusion = VS_FALSE;

    // If transparency is currently enabled, then call the enable()
    // function to recompute the transparency value
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
    
    // Create a duplicate transparency attribute
    newAttrib = new vsTransparencyAttribute();
    
    // Copy the transparency enable value
    if (isEnabled())
        newAttrib->enable();
    else
        newAttrib->disable();

    // Copy the quality value
    newAttrib->setQuality(getQuality());

    // Attach the duplicate attribute to the specified node
    theNode->addAttribute(newAttrib);
}

// ------------------------------------------------------------------------
// VESS internal function
// Saves the current attribute
// ------------------------------------------------------------------------
void vsTransparencyAttribute::saveCurrent()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    // Save the current transparency state in our save list
    attrSaveList[attrSaveCount++] = gState->getTransparency();
}

// ------------------------------------------------------------------------
// VESS internal function
// Sets the current attribute to this one
// ------------------------------------------------------------------------
void vsTransparencyAttribute::apply()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    // Set the current transparency state to this object
    gState->setTransparency(this);

    // Lock the transparency state if overriding is enabled
    if (overrideFlag)
        gState->lockTransparency(this);
}

// ------------------------------------------------------------------------
// VESS internal function
// Restores the current attribute to the last saved one
// ------------------------------------------------------------------------
void vsTransparencyAttribute::restoreSaved()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    // Unlock the transparency state if overriding was enabled
    if (overrideFlag)
        gState->unlockTransparency(this);

    // Reset the current transparency state to its previous value
    gState->setTransparency(
        (vsTransparencyAttribute *)(attrSaveList[--attrSaveCount]));
}

// ------------------------------------------------------------------------
// VESS internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsTransparencyAttribute::setState(pfGeoState *state)
{
    // Set the transparency value on the Performer geostate
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
    
    // NULL check
    if (!attribute)
        return VS_FALSE;

    // Equal pointer check
    if (this == attribute)
        return VS_TRUE;
    
    // Type check
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_TRANSPARENCY)
        return VS_FALSE;

    // Type cast
    attr = (vsTransparencyAttribute *)attribute;

    // Enabled check
    val1 = isEnabled();
    val2 = attr->isEnabled();
    if (val1 != val2)
        return VS_FALSE;

    // Quality check
    val1 = getQuality();
    val2 = attr->getQuality();
    if (val1 != val2)
        return VS_FALSE;

    // Occlusion check
    val1 = isOcclusionEnabled();
    val2 = attr->isOcclusionEnabled();
    if (val1 != val2)
        return VS_FALSE;

    // Attributes are equivalent if all checks pass
    return VS_TRUE;
}
