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
//    VESS Module:  vsWireframeAttribute.c++
//
//    Description:  Attribute that specifies that geometry should be drawn
//                  in wireframe mode rather than filled
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsWireframeAttribute.h++"
#include "vsNode.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes an OSG PolygonMode object
// ------------------------------------------------------------------------
vsWireframeAttribute::vsWireframeAttribute()
{
    // Create an osg::PolygonMode object and reference it
    osgPolyMode = new osg::PolygonMode();
    osgPolyMode->ref();
    
    // Set the PolygonMode object to LINE mode for front and back faces
    // (this is what creates the wireframe effect)
    osgPolyMode->setMode(osg::PolygonMode::FRONT_AND_BACK,
        osg::PolygonMode::LINE);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsWireframeAttribute::~vsWireframeAttribute()
{
    // Unreference the PolygonMode object
    osgPolyMode->unref();
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsWireframeAttribute::getClassName()
{
    return "vsWireframeAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsWireframeAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_WIREFRAME;
}

// ------------------------------------------------------------------------
// Enables wireframe
// ------------------------------------------------------------------------
void vsWireframeAttribute::enable()
{
    // Set the PolygonMode object to LINE mode (wireframe)
    osgPolyMode->setMode(osg::PolygonMode::FRONT_AND_BACK,
        osg::PolygonMode::LINE);
}

// ------------------------------------------------------------------------
// Disables wireframe
// ------------------------------------------------------------------------
void vsWireframeAttribute::disable()
{
    // Set the PolygonMode object to FILL mode (normal rendering)
    osgPolyMode->setMode(osg::PolygonMode::FRONT_AND_BACK,
        osg::PolygonMode::FILL);
}

// ------------------------------------------------------------------------
// Returns a flag specifying is wireframe is enabled
// ------------------------------------------------------------------------
bool vsWireframeAttribute::isEnabled()
{
    // Fetch the current polygon mode from OSG and return TRUE if the
    // mode is set to LINE.  Otherwise, return FALSE
    if (osgPolyMode->getMode(osg::PolygonMode::FRONT_AND_BACK) ==
        osg::PolygonMode::LINE)
        return true;
    else
        return false;
}

// ------------------------------------------------------------------------
// Private function
// Sets the modes on the StateSet of this node's OSG node to reflect the
// settings of this attribute
// ------------------------------------------------------------------------
void vsWireframeAttribute::setOSGAttrModes(vsNode *node)
{
    unsigned int attrMode;
    osg::StateSet *osgStateSet;
    
    // Set the StateAttribute mode to ON initially
    attrMode = osg::StateAttribute::ON;

    // If the WireframeAttribute's overrideFlag is set, change the
    // StateAttribute mode to OVERRIDE
    if (overrideFlag)
        attrMode |= osg::StateAttribute::OVERRIDE;

    // Get the StateSet for this node
    osgStateSet = getOSGStateSet(node);

    // Set the PolygonMode on the StateSet using the StateAttribute
    // mode we came up with
    osgStateSet->setAttributeAndModes(osgPolyMode, attrMode);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsWireframeAttribute::attach(vsNode *node)
{
    // Do normal vsStateAttribute attaching
    vsStateAttribute::attach(node);

    // Set the attributes we're responsible for on the node's StateSet
    setOSGAttrModes(node);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsWireframeAttribute::detach(vsNode *node)
{
    osg::StateSet *osgStateSet;

    // Get the node's StateSet
    osgStateSet = getOSGStateSet(node);

    // Change the PolygonMode settings on the StateSet to INHERIT,
    // effectively removing the PolygonMode from the StateSet
    osgStateSet->setAttributeAndModes(osgPolyMode,
        osg::StateAttribute::INHERIT);

    // Finish detaching the attribute
    vsStateAttribute::detach(node);
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsWireframeAttribute::attachDuplicate(vsNode *theNode)
{
    vsWireframeAttribute *newAttrib;
    
    // Create a new vsWireframeAttribute
    newAttrib = new vsWireframeAttribute();
    
    // Set the enable state of the new attribute to match the current one
    if (isEnabled())
        newAttrib->enable();
    else
        newAttrib->disable();

    // Add the new attribute to the given node
    theNode->addAttribute(newAttrib);
}

// ------------------------------------------------------------------------
// Internal function
// Determines if the specified attribute has state information that is
// equivalent to what this attribute has
// ------------------------------------------------------------------------
bool vsWireframeAttribute::isEquivalent(vsAttribute *attribute)
{
    vsWireframeAttribute *attr;
    bool val1, val2;
    
    // Make sure the given attribute is valid
    if (!attribute)
        return false;

    // See if we're comparing this attribute to itself
    if (this == attribute)
        return true;
    
    // Make sure the given attribute is a wireframe attribute
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_WIREFRAME)
        return false;

    // Cast the attribute to a wireframe attribute
    attr = (vsWireframeAttribute *)attribute;

    // Compare the enable states of both attributes
    val1 = isEnabled();
    val2 = attr->isEnabled();
    if (val1 != val2)
        return false;

    // If we get this far, the attributes are equivalent
    return true;
}
