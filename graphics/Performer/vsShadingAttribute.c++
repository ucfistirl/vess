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

#include "vsShadingAttribute.h++"

#include "vsGraphicsState.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes shading to Gouraud
// ------------------------------------------------------------------------
vsShadingAttribute::vsShadingAttribute()
{
    shadeVal = VS_SHADING_GOURAUD;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsShadingAttribute::~vsShadingAttribute()
{
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
    shadeVal = shadingMode;
    
    markOwnersDirty();
}

// ------------------------------------------------------------------------
// Retrieves the shading mode
// ------------------------------------------------------------------------
int vsShadingAttribute::getShading()
{
    return shadeVal;
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
// Saves the current attribute
// ------------------------------------------------------------------------
void vsShadingAttribute::saveCurrent()
{
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    attrSaveList[attrSaveCount++] = gState->getShading();
}

// ------------------------------------------------------------------------
// Internal function
// Sets the current attribute to this one
// ------------------------------------------------------------------------
void vsShadingAttribute::apply()
{
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    gState->setShading(this);
    if (overrideFlag)
        gState->lockShading(this);
}

// ------------------------------------------------------------------------
// Internal function
// Restores the current attribute to the last saved one
// ------------------------------------------------------------------------
void vsShadingAttribute::restoreSaved()
{
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    if (overrideFlag)
        gState->unlockShading(this);
    gState->setShading((vsShadingAttribute *)(attrSaveList[--attrSaveCount]));
}

// ------------------------------------------------------------------------
// Internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsShadingAttribute::setState(pfGeoState *state)
{
    if (shadeVal == VS_SHADING_FLAT)
        state->setMode(PFSTATE_SHADEMODEL, PFSM_FLAT);
    else
        state->setMode(PFSTATE_SHADEMODEL, PFSM_GOURAUD);
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
