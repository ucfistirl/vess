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
//    VESS Module:  vsShadingAttribute.c++
//
//    Description:  Attribute that specifies the shading model used for
//                  the geometry
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsShadingAttribute.h++"
#include "vsNode.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes shading to Gouraud
// ------------------------------------------------------------------------
vsShadingAttribute::vsShadingAttribute()
{
    // Create a new osg::ShadeModel attribute and reference it
    shadeModel = new osg::ShadeModel();
    shadeModel->ref();
    
    // Initialize the shade model to SMOOTH
    shadeModel->setMode(osg::ShadeModel::SMOOTH);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsShadingAttribute::~vsShadingAttribute()
{
    // Unreference the OSG ShadeModel
    shadeModel->unref();
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsShadingAttribute::getClassName()
{
    return "vsShadingAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsShadingAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_SHADING;
}

// ------------------------------------------------------------------------
// Sets the shading mode
// ------------------------------------------------------------------------
void vsShadingAttribute::setShading(int shadingMode)
{
    // Translate the requested shading mode to the OSG counterpart and
    // set the ShadeModel to use it
    switch (shadingMode)
    {
        case VS_SHADING_GOURAUD:
            shadeModel->setMode(osg::ShadeModel::SMOOTH);
            break;
        case VS_SHADING_FLAT:
            shadeModel->setMode(osg::ShadeModel::FLAT);
            break;
        default:
            printf("vsShadingAttribute::setShading: Unrecognized "
                "shading mode constant\n");
            break;
    }
}

// ------------------------------------------------------------------------
// Retrieves the shading mode
// ------------------------------------------------------------------------
int vsShadingAttribute::getShading()
{
    // Fetch and translate the osg::ShadeModel's mode to its VESS version
    switch (shadeModel->getMode())
    {
        case osg::ShadeModel::SMOOTH:
            return VS_SHADING_GOURAUD;
        case osg::ShadeModel::FLAT:
            return VS_SHADING_FLAT;
    }

    // Return -1 if we don't recognize the osg::ShadeModel's mode
    return -1;
}

// ------------------------------------------------------------------------
// Private function
// Sets the modes on the StateSet of this node's OSG node to reflect the
// settings of this attribute
// ------------------------------------------------------------------------
void vsShadingAttribute::setOSGAttrModes(vsNode *node)
{
    unsigned int attrMode;
    osg::StateSet *osgStateSet;
    
    // Start with the osg::StateAttribute mode set to ON
    attrMode = osg::StateAttribute::ON;

    // If the vsShadingAttribute's override flag is set, change the
    // osg::StateAttribute's mode to OVERRIDE
    if (overrideFlag)
        attrMode |= osg::StateAttribute::OVERRIDE;

    // Get the StateSet on the given node
    osgStateSet = getOSGStateSet(node);

    // Apply the osg::ShadeModel on the StateSet
    osgStateSet->setAttributeAndModes(shadeModel, attrMode);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsShadingAttribute::attach(vsNode *node)
{
    // Do standard vsStateAttribute attaching
    vsStateAttribute::attach(node);

    // Set up the osg::StateSet on this node to use the osg::ShadeModel
    // we've created
    setOSGAttrModes(node);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsShadingAttribute::detach(vsNode *node)
{
    osg::StateSet *osgStateSet;

    // Get the node's StateSet and reset the ShadeModel mode to inherit
    osgStateSet = getOSGStateSet(node);
    osgStateSet->setAttributeAndModes(shadeModel,
        osg::StateAttribute::INHERIT);

    // Do standard vsStateAttribute detaching
    vsStateAttribute::detach(node);
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsShadingAttribute::attachDuplicate(vsNode *theNode)
{
    vsShadingAttribute *newAttrib;
    
    // Create a new vsShadingAttribute and copy the shading mode from this
    // attribute to the new one
    newAttrib = new vsShadingAttribute();
    newAttrib->setShading(getShading());

    // Add the new attribute to the given node
    theNode->addAttribute(newAttrib);
}

// ------------------------------------------------------------------------
// Internal function
// Determines if the specified attribute has state information that is
// equivalent to what this attribute has
// ------------------------------------------------------------------------
bool vsShadingAttribute::isEquivalent(vsAttribute *attribute)
{
    vsShadingAttribute *attr;
    int val1, val2;
    
    // Make sure the given attribute is valid, return FALSE if not
    if (!attribute)
        return false;

    // Check if we're comparing the attribute to itself
    if (this == attribute)
        return true;
    
    // Make sure the given attribute is a shading attribute
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_SHADING)
        return false;

    // Cast the given attribute to a shading attribute
    attr = (vsShadingAttribute *)attribute;

    // Compare the two shading settings
    val1 = getShading();
    val2 = attr->getShading();
    if (val1 != val2)
        return false;

    // Return true if we get this far (they are equivalent)
    return true;
}
