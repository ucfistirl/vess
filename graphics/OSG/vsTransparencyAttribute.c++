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
    occlusion = VS_TRUE;
    quality = VS_TRANSP_QUALITY_DEFAULT;

    transpValue = VS_TRUE;
    
    osgDepth = new osg::Depth();
    osgDepth->ref();
    osgDepth->setWriteMask(VS_TRUE);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsTransparencyAttribute::~vsTransparencyAttribute()
{
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
// Enables transparency
// ------------------------------------------------------------------------
void vsTransparencyAttribute::enable()
{
    transpValue = VS_TRUE;
    
    markOwnersDirty();
}

// ------------------------------------------------------------------------
// Disables transparency
// ------------------------------------------------------------------------
void vsTransparencyAttribute::disable()
{
    transpValue = VS_FALSE;
    
    markOwnersDirty();
}

// ------------------------------------------------------------------------
// Returns a flag specifying if transparency is enabled
// ------------------------------------------------------------------------
int vsTransparencyAttribute::isEnabled()
{
    return transpValue;
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

//    if (isEnabled())
//        enable();
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

    osgDepth->setWriteMask(VS_TRUE);
}

// ------------------------------------------------------------------------
// Disables the z-buffer when drawing transparent geometry
// ------------------------------------------------------------------------
void vsTransparencyAttribute::disableOcclusion()
{
    occlusion = VS_FALSE;

    osgDepth->setWriteMask(VS_FALSE);
}

// ------------------------------------------------------------------------
// Returns a flag specifying if occlusion is enabled
// ------------------------------------------------------------------------
int vsTransparencyAttribute::isOcclusionEnabled()
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
    
    attrMode = osg::StateAttribute::ON;

    if (overrideFlag)
        attrMode |= osg::StateAttribute::OVERRIDE;

    osgStateSet = getOSGStateSet(node);

    osgStateSet->setAttributeAndModes(osgDepth, attrMode);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsTransparencyAttribute::attach(vsNode *node)
{
    vsStateAttribute::attach(node);

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

    // Setting the modes to INHERIT should remove these attributes from
    // the StateSet entirely
    osgStateSet->setAttributeAndModes(osgDepth, osg::StateAttribute::INHERIT);

    vsStateAttribute::detach(node);
}

// ------------------------------------------------------------------------
// Internal function
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

    if (isOcclusionEnabled())
        newAttrib->enableOcclusion();
    else
        newAttrib->disableOcclusion();

    theNode->addAttribute(newAttrib);
}

// ------------------------------------------------------------------------
// Internal function
// Saves the current attribute
// ------------------------------------------------------------------------
void vsTransparencyAttribute::saveCurrent()
{
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    attrSaveList[attrSaveCount++] = gState->getTransparency();
}

// ------------------------------------------------------------------------
// Internal function
// Sets the current attribute to this one
// ------------------------------------------------------------------------
void vsTransparencyAttribute::apply()
{
    vsGraphicsState *gState = vsGraphicsState::getInstance();

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
    vsGraphicsState *gState = vsGraphicsState::getInstance();

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
    if (transpValue)
    {
        stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
        stateSet->setRenderBinDetails(1, "DepthSortedBin");
        stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
    }
    else
    {
        stateSet->setRenderingHint(osg::StateSet::OPAQUE_BIN);
        stateSet->setRenderBinDetails(0, "RenderBin");
        stateSet->setMode(GL_BLEND, osg::StateAttribute::OFF);
    }
}

// ------------------------------------------------------------------------
// Internal function
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
