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

#include <stdio.h>
#include "vsTransparencyAttribute.h++"
#include "vsNode.h++"
#include "vsGraphicsState.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes the Performer transparency value
// ------------------------------------------------------------------------
vsTransparencyAttribute::vsTransparencyAttribute()
{
    // Start with occlusion enabled and high quality transparency
    occlusion = true;
    quality = VS_TRANSP_QUALITY_DEFAULT;

    // Start with transparency enabled
    transpValue = true;
    
    // Create an osg::Depth object to handle occlusion settings, start
    // with occlusion on
    osgDepth = new osg::Depth();
    osgDepth->ref();
    osgDepth->setWriteMask(true);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsTransparencyAttribute::~vsTransparencyAttribute()
{
    // Unreference the Depth object
    osgDepth->unref();
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsTransparencyAttribute::getClassName()
{
    return "vsTransparencyAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsTransparencyAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_TRANSPARENCY;
}

// ------------------------------------------------------------------------
// Returns a clone of this attribute
// ------------------------------------------------------------------------
vsAttribute *vsTransparencyAttribute::clone()
{
    vsTransparencyAttribute *newAttrib;
    
    // Create a new vsTransparencyAttribute
    newAttrib = new vsTransparencyAttribute();
    
    // Copy this attribute's settings to the new attribute
    // Enable flag
    if (isEnabled())
        newAttrib->enable();
    else
        newAttrib->disable();

    // Quality
    newAttrib->setQuality(getQuality());

    // Occlusion
    if (isOcclusionEnabled())
        newAttrib->enableOcclusion();
    else
        newAttrib->disableOcclusion();

    // Return the clone
    return newAttrib;
}

// ------------------------------------------------------------------------
// Enables transparency
// ------------------------------------------------------------------------
void vsTransparencyAttribute::enable()
{
    // Enable transparency
    transpValue = true;
    
    // Mark all attached nodes dirty
    markOwnersDirty();
}

// ------------------------------------------------------------------------
// Disables transparency
// ------------------------------------------------------------------------
void vsTransparencyAttribute::disable()
{
    // Disable transparency
    transpValue = false;
    
    // Mark all attached nodes dirty
    markOwnersDirty();
}

// ------------------------------------------------------------------------
// Returns a flag specifying if transparency is enabled
// ------------------------------------------------------------------------
bool vsTransparencyAttribute::isEnabled()
{
    return transpValue;
}

// ------------------------------------------------------------------------
// Sets the quality of the transparency rendering calculation
// ------------------------------------------------------------------------
void vsTransparencyAttribute::setQuality(int newQuality)
{
    // Make sure the quality setting is one we recognize
    if ((newQuality != VS_TRANSP_QUALITY_DEFAULT) &&
        (newQuality != VS_TRANSP_QUALITY_FAST) &&
        (newQuality != VS_TRANSP_QUALITY_HIGH))
    {
        printf("vsTransparencyAttribute::setQuality: Unrecognized quality "
            "constant\n");
        return;
    }

    // Remember the new quality setting
    quality = newQuality;
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
    // Mark occlusion as on
    occlusion = true;

    // Enable z-buffer writes
    osgDepth->setWriteMask(true);
}

// ------------------------------------------------------------------------
// Disables the z-buffer when drawing transparent geometry
// ------------------------------------------------------------------------
void vsTransparencyAttribute::disableOcclusion()
{
    // Mark occlusion as off
    occlusion = false;

    // Disable z-buffer writes
    osgDepth->setWriteMask(false);
}

// ------------------------------------------------------------------------
// Returns a flag specifying if occlusion is enabled
// ------------------------------------------------------------------------
bool vsTransparencyAttribute::isOcclusionEnabled()
{
    return occlusion;
}

// ------------------------------------------------------------------------
// Private function
// Sets the modes on the StateSet of this node's OSG node to reflect the
// settings of this attribute
// ------------------------------------------------------------------------
void vsTransparencyAttribute::setOSGAttrModes(vsNode *node)
{
    unsigned int attrMode;
    osg::StateSet *osgStateSet;
    
    // Start with the osg::StateAttribute mode set to ON
    attrMode = osg::StateAttribute::ON;

    // If this attribute's override flag is set, change the StateAttribute
    // mode to OVERRIDE
    if (overrideFlag)
        attrMode |= osg::StateAttribute::OVERRIDE;

    // Get the node's StateSet
    osgStateSet = getOSGStateSet(node);

    // Set the osg::Depth attribute and mode on the StateSet
    osgStateSet->setAttributeAndModes(osgDepth, attrMode);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsTransparencyAttribute::attach(vsNode *node)
{
    // Do standard vsStateAttribute attaching
    vsStateAttribute::attach(node);

    // Set the OSG modes this attribute is in charge of on the node
    setOSGAttrModes(node);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsTransparencyAttribute::detach(vsNode *node)
{
    osg::StateSet *osgStateSet;
    osgStateSet = getOSGStateSet(node);

    // Setting the modes to INHERIT removes these attributes from
    // the StateSet entirely
    osgStateSet->setAttributeAndModes(osgDepth, osg::StateAttribute::INHERIT);

    // Detach from the node
    vsStateAttribute::detach(node);
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsTransparencyAttribute::attachDuplicate(vsNode *theNode)
{
    // Add a clone of this attribute to the given node
    theNode->addAttribute(this->clone());
}

// ------------------------------------------------------------------------
// Internal function
// Saves the current attribute
// ------------------------------------------------------------------------
void vsTransparencyAttribute::saveCurrent()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    // Save the current transparency settings
    attrSaveList[attrSaveCount++] = gState->getTransparency();
}

// ------------------------------------------------------------------------
// Internal function
// Sets the current attribute to this one
// ------------------------------------------------------------------------
void vsTransparencyAttribute::apply()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    // Set this attributes setting's on the vsGraphicsState
    gState->setTransparency(this);
    if (overrideFlag)
        gState->lockTransparency(this);
}

// ------------------------------------------------------------------------
// Internal function
// Restores the current attribute to the last saved one
// ------------------------------------------------------------------------
void vsTransparencyAttribute::restoreSaved()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    // Restore the previous transparency settings to the vsGraphicsState
    if (overrideFlag)
        gState->unlockTransparency(this);
    gState->setTransparency(
        (vsTransparencyAttribute *)(attrSaveList[--attrSaveCount]));
}

// ------------------------------------------------------------------------
// Internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsTransparencyAttribute::setState(osg::StateSet *stateSet)
{
    // Check if transparency is enabled
    if (transpValue)
    {
        // Tell OSG to put descendant geometry in the transparent RenderBin
        stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

        // Tell OSG to sort the transparent bin by depth
        stateSet->setRenderBinDetails(1, "DepthSortedBin");

        // Enable alpha blending
        stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
    }
    else
    {
        // Tell OSG to put descendant geometry in the regular opaque RenderBin
        stateSet->setRenderingHint(osg::StateSet::OPAQUE_BIN);

        // Tell OSG to sort the opaque bin by state
        stateSet->setRenderBinDetails(0, "RenderBin");

        // Disable alpha blending
        stateSet->setMode(GL_BLEND, osg::StateAttribute::OFF);
    }
}

// ------------------------------------------------------------------------
// Internal function
// Determines if the specified attribute has state information that is
// equivalent to what this attribute has
// ------------------------------------------------------------------------
bool vsTransparencyAttribute::isEquivalent(vsAttribute *attribute)
{
    vsTransparencyAttribute *attr;
    int val1, val2;
    bool b1, b2;
    
    // Make sure the given attribute is valid
    if (!attribute)
        return false;

    // Check to see if we're comparing this attribute to itself
    if (this == attribute)
        return true;
    
    // Make sure the given attribute is a transparency attribute
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_TRANSPARENCY)
        return false;

    // Cast the given attribute to a transparency attribute
    attr = (vsTransparencyAttribute *)attribute;

    // Compare enable flags
    b1 = isEnabled();
    b2 = attr->isEnabled();
    if (b1 != b2)
        return false;

    // Compare quality settings
    val1 = getQuality();
    val2 = attr->getQuality();
    if (val1 != val2)
        return false;

    // Compare occlusion settings
    b1 = isOcclusionEnabled();
    b2 = attr->isOcclusionEnabled();
    if (b1 != b2)
        return false;

    // If we get this far, the attributes are equivalent
    return true;
}
