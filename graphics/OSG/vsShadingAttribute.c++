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
    shadeModel = new osg::ShadeModel();
    shadeModel->ref();
    
    shadeModel->setMode(osg::ShadeModel::SMOOTH);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsShadingAttribute::~vsShadingAttribute()
{
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
    switch (shadeModel->getMode())
    {
        case osg::ShadeModel::SMOOTH:
            return VS_SHADING_GOURAUD;
        case osg::ShadeModel::FLAT:
            return VS_SHADING_FLAT;
    }

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
    
    attrMode = osg::StateAttribute::ON;

    if (overrideFlag)
        attrMode |= osg::StateAttribute::OVERRIDE;

    osgStateSet = getOSGStateSet(node);

    osgStateSet->setAttributeAndModes(shadeModel, attrMode);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsShadingAttribute::attach(vsNode *node)
{
    vsStateAttribute::attach(node);

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
    osgStateSet = getOSGStateSet(node);

    osgStateSet->setAttributeAndModes(shadeModel,
        osg::StateAttribute::INHERIT);

    vsStateAttribute::detach(node);
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsShadingAttribute::attachDuplicate(vsNode *theNode)
{
    vsShadingAttribute *newAttrib;
    
    newAttrib = new vsShadingAttribute();
    newAttrib->setShading(getShading());

    theNode->addAttribute(newAttrib);
}

// ------------------------------------------------------------------------
// Internal function
// Determines if the specified attribute has state information that is
// equivalent to what this attribute has
// ------------------------------------------------------------------------
int vsShadingAttribute::isEquivalent(vsAttribute *attribute)
{
    vsShadingAttribute *attr;
    int val1, val2;
    
    if (!attribute)
        return VS_FALSE;

    if (this == attribute)
        return VS_TRUE;
    
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_SHADING)
        return VS_FALSE;

    attr = (vsShadingAttribute *)attribute;

    val1 = getShading();
    val2 = attr->getShading();
    if (val1 != val2)
        return VS_FALSE;

    return VS_TRUE;
}
