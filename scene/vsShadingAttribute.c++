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
    // Set the default shading model to Gouraud (smooth) shading
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
    // Set the shading mode to the specified value
    shadeVal = shadingMode;
    
    // Mark the nodes that have this attribute attached as needing
    // of an update
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
    
    // Create a duplicate shading attribute
    newAttrib = new vsShadingAttribute();

    // Copy the shading mode value
    newAttrib->setShading(getShading());

    // Attach the duplicate attribute to the specified node
    theNode->addAttribute(newAttrib);
}

// ------------------------------------------------------------------------
// VESS internal function
// Saves the current attribute
// ------------------------------------------------------------------------
void vsShadingAttribute::saveCurrent()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    // Save the current shading state in our save list
    attrSaveList[attrSaveCount++] = gState->getShading();
}

// ------------------------------------------------------------------------
// VESS internal function
// Sets the current attribute to this one
// ------------------------------------------------------------------------
void vsShadingAttribute::apply()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    // Set the current shading state to this object
    gState->setShading(this);

    // Lock the shading state if overriding is enabled
    if (overrideFlag)
        gState->lockShading(this);
}

// ------------------------------------------------------------------------
// VESS internal function
// Restores the current attribute to the last saved one
// ------------------------------------------------------------------------
void vsShadingAttribute::restoreSaved()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    // Unlock the shading state if overriding was enabled
    if (overrideFlag)
        gState->unlockShading(this);

    // Reset the current shading state to its previous value
    gState->setShading((vsShadingAttribute *)(attrSaveList[--attrSaveCount]));
}

// ------------------------------------------------------------------------
// VESS internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsShadingAttribute::setState(pfGeoState *state)
{
    // Set the shading mode on the geostate based on our shading mode
    if (shadeVal == VS_SHADING_FLAT)
        state->setMode(PFSTATE_SHADEMODEL, PFSM_FLAT);
    else
        state->setMode(PFSTATE_SHADEMODEL, PFSM_GOURAUD);
}

// ------------------------------------------------------------------------
// VESS internal function
// Determines if the specified attribute has state information that is
// equivalent to what this attribute has
// ------------------------------------------------------------------------
int vsShadingAttribute::isEquivalent(vsAttribute *attribute)
{
    vsShadingAttribute *attr;
    int val1, val2;
    
    // NULL check
    if (!attribute)
        return VS_FALSE;

    // Equal pointer check
    if (this == attribute)
        return VS_TRUE;
    
    // Type check
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_SHADING)
        return VS_FALSE;

    // Type cast
    attr = (vsShadingAttribute *)attribute;

    // Mode check
    val1 = getShading();
    val2 = attr->getShading();
    if (val1 != val2)
        return VS_FALSE;

    // Attributes are equivalent if all checks pass
    return VS_TRUE;
}
