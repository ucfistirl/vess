// File vsShadingAttribute.c++

#include "vsShadingAttribute.h++"

#include <Performer/pr.h>
#include "vsSystem.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes shading to Gouraud
// ------------------------------------------------------------------------
vsShadingAttribute::vsShadingAttribute() : shadeAttrSave(1, 1, 1)
{
    shadeVal = VS_SHADING_GOURAUD;

    saveCount = 0;
}

// ------------------------------------------------------------------------
// VESS internal function
// Constructor - Initializes shading to the given value
// ------------------------------------------------------------------------
vsShadingAttribute::vsShadingAttribute(int performerShading)
    : shadeAttrSave(1, 1, 1)
{
    if (performerShading == PFSM_FLAT)
        shadeVal = VS_SHADING_FLAT;
    else
        shadeVal = VS_SHADING_GOURAUD;

    attachedFlag = 1;
    saveCount = 0;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsShadingAttribute::~vsShadingAttribute()
{
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsShadingAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_SHADING;
}

// ------------------------------------------------------------------------
// Retrieves the category of this attribute
// ------------------------------------------------------------------------
int vsShadingAttribute::getAttributeCategory()
{
    return VS_ATTRIBUTE_CATEGORY_STATE;
}

// ------------------------------------------------------------------------
// Sets the shading mode
// ------------------------------------------------------------------------
void vsShadingAttribute::setShading(int shadingMode)
{
    shadeVal = shadingMode;
}

// ------------------------------------------------------------------------
// Retrieves the shading mode
// ------------------------------------------------------------------------
int vsShadingAttribute::getShading()
{
    return shadeVal;
}

// ------------------------------------------------------------------------
// VESS internal function
// Saves the current attribute
// ------------------------------------------------------------------------
void vsShadingAttribute::saveCurrent()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    shadeAttrSave[saveCount++] = gState->getShading();
}

// ------------------------------------------------------------------------
// VESS internal function
// Sets the current attribute to this one
// ------------------------------------------------------------------------
void vsShadingAttribute::apply()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    gState->setShading(this);
}

// ------------------------------------------------------------------------
// VESS internal function
// Restores the current attribute to the last saved one
// ------------------------------------------------------------------------
void vsShadingAttribute::restoreSaved()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    gState->setShading((vsShadingAttribute *)(shadeAttrSave[--saveCount]));
}

// ------------------------------------------------------------------------
// VESS internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsShadingAttribute::setState()
{
    if (shadeVal == VS_SHADING_FLAT)
        pfShadeModel(PFSM_FLAT);
    else
        pfShadeModel(PFSM_GOURAUD);
}

// ------------------------------------------------------------------------
// static VESS internal function
// Applies default shading settings to the graphics library
// ------------------------------------------------------------------------
void vsShadingAttribute::setDefault()
{
    pfShadeModel(PFSM_GOURAUD);
}
