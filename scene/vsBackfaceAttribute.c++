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

#include <Performer/pr/pfLight.h>
#include <Performer/pr.h>
#include "vsSystem.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes backfacing to false
// ------------------------------------------------------------------------
vsBackfaceAttribute::vsBackfaceAttribute()
{
    // Set the attribute to the default OFF settings
    lightModel = new pfLightModel();
    lightModel->setLocal(PF_ON);
    lightModel->setTwoSide(PF_OFF);
    lightModel->setAmbient(0.0, 0.0, 0.0);
    lightModel->ref();
    
    cullfaceVal = PFCF_BACK;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsBackfaceAttribute::~vsBackfaceAttribute()
{
    lightModel->unref();
    pfDelete(lightModel);
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
    lightModel->setTwoSide(PF_ON);
    cullfaceVal = PFCF_OFF;
    
    markOwnersDirty();
}

// ------------------------------------------------------------------------
// Disables backfacing
// ------------------------------------------------------------------------
void vsBackfaceAttribute::disable()
{
    lightModel->setTwoSide(PF_OFF);
    cullfaceVal = PFCF_BACK;

    markOwnersDirty();
}

// ------------------------------------------------------------------------
// Retrieves a flag stating if backfacing is enabled
// ------------------------------------------------------------------------
int vsBackfaceAttribute::isEnabled()
{
    if (cullfaceVal == PFCF_OFF)
        return VS_TRUE;

    return VS_FALSE;
}

// ------------------------------------------------------------------------
// VESS internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsBackfaceAttribute::attachDuplicate(vsNode *theNode)
{
    vsBackfaceAttribute *newAttrib;
    
    newAttrib = new vsBackfaceAttribute();
    
    if (isEnabled())
        newAttrib->enable();
    else
        newAttrib->disable();

    theNode->addAttribute(newAttrib);
}

// ------------------------------------------------------------------------
// VESS internal function
// Saves the current attribute
// ------------------------------------------------------------------------
void vsBackfaceAttribute::saveCurrent()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    attrSaveList[attrSaveCount++] = gState->getBackface();
}

// ------------------------------------------------------------------------
// VESS internal function
// Sets the current attribute to this one
// ------------------------------------------------------------------------
void vsBackfaceAttribute::apply()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    gState->setBackface(this);
    if (overrideFlag)
        gState->lockBackface(this);
}

// ------------------------------------------------------------------------
// VESS internal function
// Restores the current attribute to the last saved one
// ------------------------------------------------------------------------
void vsBackfaceAttribute::restoreSaved()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    if (overrideFlag)
        gState->unlockBackface(this);
    gState->setBackface((vsBackfaceAttribute *)(attrSaveList[--attrSaveCount]));
}

// ------------------------------------------------------------------------
// VESS internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsBackfaceAttribute::setState(pfGeoState *state)
{
    state->setMode(PFSTATE_CULLFACE, cullfaceVal);
    state->setAttr(PFSTATE_LIGHTMODEL, lightModel);
}

// ------------------------------------------------------------------------
// VESS internal function
// Determines if the specified attribute has state information that is
// equivalent to what this attribute has
// ------------------------------------------------------------------------
int vsBackfaceAttribute::isEquivalent(vsAttribute *attribute)
{
    vsBackfaceAttribute *attr;
    int val1, val2;
    
    if (!attribute)
        return VS_FALSE;

    if (this == attribute)
        return VS_TRUE;
    
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_BACKFACE)
        return VS_FALSE;

    attr = (vsBackfaceAttribute *)attribute;
    
    val1 = isEnabled();
    val2 = attr->isEnabled();
    if (val1 != val2)
        return VS_FALSE;

    return VS_TRUE;
}
