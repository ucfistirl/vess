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
#include "vsNode.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes backfacing to false
// ------------------------------------------------------------------------
vsBackfaceAttribute::vsBackfaceAttribute()
{
    // * Set the attribute to the default OFF settings
    
    // Create and configure the light model. Although only the two-sided
    // value will change over the lifetime of the object, the rest of
    // the settings are needed for the library to act consistently.
    lightModel = new osg::LightModel();
    lightModel->ref();
    lightModel->setAmbientIntensity(osg::Vec4(0.0, 0.0, 0.0, 1.0));
    lightModel->setColorControl(osg::LightModel::SEPARATE_SPECULAR_COLOR);
    lightModel->setLocalViewer(VS_TRUE);
    lightModel->setTwoSided(VS_FALSE);
    
    // Create an osg CullFace object and set it to cull the back faces
    // of geometry
    cullFace = new osg::CullFace();
    cullFace->ref();
    cullFace->setMode(osg::CullFace::BACK);
    
    backfaceEnabled = VS_FALSE;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsBackfaceAttribute::~vsBackfaceAttribute()
{
    // Delete the OSG objects
    lightModel->unref();
    cullFace->unref();
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsBackfaceAttribute::getClassName()
{
    return "vsBackfaceAttribute";
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
    // Enable backside lighting
    lightModel->setTwoSided(VS_TRUE);

    backfaceEnabled = VS_TRUE;

    // Update the owners' StateSets
    setAllOwnersOSGAttrModes();
}

// ------------------------------------------------------------------------
// Disables backfacing
// ------------------------------------------------------------------------
void vsBackfaceAttribute::disable()
{
    // Disable backside lighting
    lightModel->setTwoSided(VS_FALSE);

    backfaceEnabled = VS_FALSE;

    // Update the owners' osg StateSets
    setAllOwnersOSGAttrModes();
}

// ------------------------------------------------------------------------
// Retrieves a flag stating if backfacing is enabled
// ------------------------------------------------------------------------
int vsBackfaceAttribute::isEnabled()
{
    return backfaceEnabled;
}

// ------------------------------------------------------------------------
// Private function
// Sets the modes on the StateSet of this node's OSG node to reflect the
// settings of this attribute
// ------------------------------------------------------------------------
void vsBackfaceAttribute::setOSGAttrModes(vsNode *node)
{
    unsigned int attrMode;
    osg::StateSet *osgStateSet;
    
    // Calculate the OSG apply mode by combining the current state of this
    // attribute with the value of the override flag
    // *Note: The 'backface display enable' property of VESS that the
    // vsBackfaceAttribute embodies is the _opposite_ of the 'backface
    // culling enable' mode that osg (and OpenGL) uses. When backfaces
    // are on, culling should be set to off, and vice versa.
    if (backfaceEnabled)
        attrMode = osg::StateAttribute::OFF;
    else
        attrMode = osg::StateAttribute::ON;

    if (overrideFlag)
        attrMode |= osg::StateAttribute::OVERRIDE;

    // Get the osg StateSet and apply the osg objects to it using the
    // calculated apply mode
    osgStateSet = getOSGStateSet(node);

    // *Note: It should be safe to use the calculated attrMode with the
    // lightModel, even if it is set to OFF; ON and OFF aren't supposed
    // to matter for attributes, only for modes, and the lightModel
    // is an attribute.
    osgStateSet->setAttributeAndModes(lightModel, attrMode);
    osgStateSet->setAttributeAndModes(cullFace, attrMode);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsBackfaceAttribute::attach(vsNode *node)
{
    // Inherited attach
    vsStateAttribute::attach(node);

    // Update the new owner's osg StateSet
    setOSGAttrModes(node);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsBackfaceAttribute::detach(vsNode *node)
{
    osg::StateSet *osgStateSet;
    osgStateSet = getOSGStateSet(node);

    // Setting the modes to INHERIT should remove these attributes from
    // the StateSet entirely
    osgStateSet->setAttributeAndModes(lightModel, osg::StateAttribute::INHERIT);
    osgStateSet->setAttributeAndModes(cullFace, osg::StateAttribute::INHERIT);

    // Inherited detach
    vsStateAttribute::detach(node);
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsBackfaceAttribute::attachDuplicate(vsNode *theNode)
{
    vsBackfaceAttribute *newAttrib;
    
    // Create a duplicate backface attribute
    newAttrib = new vsBackfaceAttribute();
    
    // Copy the backface enable mode
    if (isEnabled())
        newAttrib->enable();
    else
        newAttrib->disable();

    // Attach the duplicate attribute to the specified node
    theNode->addAttribute(newAttrib);
}

// ------------------------------------------------------------------------
// Internal function
// Determines if the specified attribute has state information that is
// equivalent to what this attribute has
// ------------------------------------------------------------------------
int vsBackfaceAttribute::isEquivalent(vsAttribute *attribute)
{
    vsBackfaceAttribute *attr;
    int val1, val2;
    
    // NULL check
    if (!attribute)
        return VS_FALSE;

    // Equal pointer check
    if (this == attribute)
        return VS_TRUE;
    
    // Type check
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_BACKFACE)
        return VS_FALSE;

    // Type cast
    attr = (vsBackfaceAttribute *)attribute;
    
    // State check
    val1 = isEnabled();
    val2 = attr->isEnabled();
    if (val1 != val2)
        return VS_FALSE;

    // Attributes are equivalent if all checks pass
    return VS_TRUE;
}
