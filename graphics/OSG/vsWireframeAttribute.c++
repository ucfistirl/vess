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
    osgPolyMode = new osg::PolygonMode();
    osgPolyMode->ref();
    
    osgPolyMode->setMode(osg::PolygonMode::FRONT_AND_BACK,
        osg::PolygonMode::LINE);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsWireframeAttribute::~vsWireframeAttribute()
{
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
    osgPolyMode->setMode(osg::PolygonMode::FRONT_AND_BACK,
        osg::PolygonMode::LINE);
}

// ------------------------------------------------------------------------
// Disables wireframe
// ------------------------------------------------------------------------
void vsWireframeAttribute::disable()
{
    osgPolyMode->setMode(osg::PolygonMode::FRONT_AND_BACK,
        osg::PolygonMode::FILL);
}

// ------------------------------------------------------------------------
// Returns a flag specifying is wireframe is enabled
// ------------------------------------------------------------------------
int vsWireframeAttribute::isEnabled()
{
    if (osgPolyMode->getMode(osg::PolygonMode::FRONT_AND_BACK) ==
        osg::PolygonMode::LINE)
        return VS_TRUE;
    else
        return VS_FALSE;
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
    
    attrMode = osg::StateAttribute::ON;

    if (overrideFlag)
        attrMode |= osg::StateAttribute::OVERRIDE;

    osgStateSet = getOSGStateSet(node);

    osgStateSet->setAttributeAndModes(osgPolyMode, attrMode);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsWireframeAttribute::attach(vsNode *node)
{
    vsStateAttribute::attach(node);

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
    osgStateSet = getOSGStateSet(node);

    osgStateSet->setAttributeAndModes(osgPolyMode,
        osg::StateAttribute::INHERIT);

    vsStateAttribute::detach(node);
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsWireframeAttribute::attachDuplicate(vsNode *theNode)
{
    vsWireframeAttribute *newAttrib;
    
    newAttrib = new vsWireframeAttribute();
    
    if (isEnabled())
        newAttrib->enable();
    else
        newAttrib->disable();

    theNode->addAttribute(newAttrib);
}

// ------------------------------------------------------------------------
// Internal function
// Determines if the specified attribute has state information that is
// equivalent to what this attribute has
// ------------------------------------------------------------------------
int vsWireframeAttribute::isEquivalent(vsAttribute *attribute)
{
    vsWireframeAttribute *attr;
    int val1, val2;
    
    if (!attribute)
        return VS_FALSE;

    if (this == attribute)
        return VS_TRUE;
    
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_WIREFRAME)
        return VS_FALSE;

    attr = (vsWireframeAttribute *)attribute;

    val1 = isEnabled();
    val2 = attr->isEnabled();
    if (val1 != val2)
        return VS_FALSE;

    return VS_TRUE;
}
