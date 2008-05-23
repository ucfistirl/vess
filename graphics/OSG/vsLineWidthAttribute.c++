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
//    VESS Module:  vsLineWidthAttribute.c++
//
//    Description:  Specifies the line width in pixels for geometries
//
//    Author(s):    Jeremy Elbourn
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsLineWidthAttribute.h++"
#include "vsNode.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes the attribute to default values
// -----------------------------------------------------------------------
vsLineWidthAttribute::vsLineWidthAttribute()
{
    // Create new osg LineWidth object
    osgLineWidth = new osg::LineWidth();
    osgLineWidth->ref();
   
    // Default line width is one
    osgLineWidth->setWidth(1);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsLineWidthAttribute::~vsLineWidthAttribute()
{
    // Delete the osg LineWdith object
    osgLineWidth->unref();
}

// ------------------------------------------------------------------------
// Private function
// Sets the modes on the StateSet of this node's OSG node to reflect the
// settings of this attribute
// ------------------------------------------------------------------------
void vsLineWidthAttribute::setOSGAttrModes(vsNode *node)
{
    unsigned int attrMode;
    osg::StateSet *osgStateSet;
    
    // Calculate the attribute mode
    attrMode = osg::StateAttribute::ON;

    if (overrideFlag)
        attrMode |= osg::StateAttribute::OVERRIDE;

    // Set the LineWidth object on the node's StateSet using the calculated
    // mode
    osgStateSet = getOSGStateSet(node);

    osgStateSet->setAttributeAndModes(osgLineWidth, attrMode);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsLineWidthAttribute::attach(vsNode *node)
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
void vsLineWidthAttribute::detach(vsNode *node)
{
    osg::StateSet *osgStateSet;
    osgStateSet = getOSGStateSet(node);

    // Setting the mode to INHERIT should remove this attribute from
    // the StateSet entirely
    osgStateSet->setAttributeAndModes(osgLineWidth,
        osg::StateAttribute::INHERIT);

    // Inherited detach
    vsStateAttribute::detach(node);
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsLineWidthAttribute::attachDuplicate(vsNode *theNode)
{
    // Add a clone of this attribute to the given node
    theNode->addAttribute(this->clone());
}

// ------------------------------------------------------------------------
// Internal function
// Determines if the specified attribute has state information that is
// equivalent to what this attribute has
// ------------------------------------------------------------------------
bool vsLineWidthAttribute::isEquivalent(vsAttribute *attribute)
{
    vsLineWidthAttribute *attr;
   
    // NULL check
    if (!attribute)
        return false;

    // Equal pointer check
    if (this == attribute)
        return true;
    
    // Type check
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_LINE_WIDTH)
        return false;

    // Type cast
    attr = (vsLineWidthAttribute *)attribute;

    // Line Width check
    return (osgLineWidth->getWidth() == attr->getLineWidth());
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsLineWidthAttribute::getClassName()
{
    return "vsLineWidthAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsLineWidthAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_LINE_WIDTH;
}

// ------------------------------------------------------------------------
// Returns a clone of this attribute
// ------------------------------------------------------------------------
vsAttribute *vsLineWidthAttribute::clone()
{
    vsLineWidthAttribute *newAttrib;

    // Create a new line width attribute on the specified node
    newAttrib = new vsLineWidthAttribute();

    // Copy the line width parameter to the new attribute
    newAttrib->setLineWidth(osgLineWidth->getWidth());

    // Return the clone
    return newAttrib;
}

// ------------------------------------------------------------------------
// Sets the line width
// ------------------------------------------------------------------------
void vsLineWidthAttribute::setLineWidth(double newWidth)
{
    osgLineWidth->setWidth(newWidth);
}

// ------------------------------------------------------------------------
// Returns the line width
// ------------------------------------------------------------------------
double vsLineWidthAttribute::getLineWidth()
{
    return osgLineWidth->getWidth();
}

