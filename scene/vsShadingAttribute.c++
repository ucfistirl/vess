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

#include <Performer/pr.h>
#include "vsSystem.h++"

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
// VESS internal function
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
// VESS internal function
// Saves the current attribute
// ------------------------------------------------------------------------
void vsShadingAttribute::saveCurrent()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    attrSaveList[attrSaveCount++] = gState->getShading();
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

    gState->setShading((vsShadingAttribute *)(attrSaveList[--attrSaveCount]));
}

// ------------------------------------------------------------------------
// VESS internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsShadingAttribute::setState(pfGeoState *state)
{
    if (shadeVal == VS_SHADING_FLAT)
        state->setMode(PFSTATE_SHADEMODEL, PFSM_FLAT);
    else
        state->setMode(PFSTATE_SHADEMODEL, PFSM_GOURAUD);
}
