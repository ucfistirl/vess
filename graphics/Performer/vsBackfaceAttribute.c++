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

#include <Performer/pf.h>
#include <Performer/pr/pfLight.h>
#include "vsNode.h++"
#include "vsGraphicsState.h++"

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
    // Delete the Performer light mode object
    lightModel->unref();
    pfDelete(lightModel);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsBackfaceAttribute::getClassName()
{
    return "vsBackfaceAttribute";
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
    // Turn display of back faces on
    lightModel->setTwoSide(PF_ON);
    cullfaceVal = PFCF_OFF;
    
    // Mark the nodes that have this attribute attached as needing
    // of an update
    markOwnersDirty();
}

// ------------------------------------------------------------------------
// Disables backfacing
// ------------------------------------------------------------------------
void vsBackfaceAttribute::disable()
{
    // Turn display of back faces off
    lightModel->setTwoSide(PF_OFF);
    cullfaceVal = PFCF_BACK;

    // Mark the nodes that have this attribute attached as needing
    // of an update
    markOwnersDirty();
}

// ------------------------------------------------------------------------
// Retrieves a flag stating if backfacing is enabled
// ------------------------------------------------------------------------
int vsBackfaceAttribute::isEnabled()
{
    // Backfacing is on if Performer face culling is off
    if (cullfaceVal == PFCF_OFF)
        return VS_TRUE;

    return VS_FALSE;
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsBackfaceAttribute::attachDuplicate(vsNode *theNode)
{
    vsBackfaceAttribute *newAttrib;
    
    // Create a duplicate backface attribute
    newAttrib = new vsBackfaceAttribute();
    
    // Copy the backface enable mode
    if (isEnabled())
        newAttrib->enable();
    else
        newAttrib->disable();

    // Attach the duplicate attribute to the specified node
    theNode->addAttribute(newAttrib);
}

// ------------------------------------------------------------------------
// Internal function
// Saves the current attribute
// ------------------------------------------------------------------------
void vsBackfaceAttribute::saveCurrent()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    // Save the current backface state in our save list
    attrSaveList[attrSaveCount++] = gState->getBackface();
}

// ------------------------------------------------------------------------
// Internal function
// Sets the current attribute to this one
// ------------------------------------------------------------------------
void vsBackfaceAttribute::apply()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    // Set the current backface state to this object
    gState->setBackface(this);

    // Lock the backface state if overriding is enabled
    if (overrideFlag)
        gState->lockBackface(this);
}

// ------------------------------------------------------------------------
// Internal function
// Restores the current attribute to the last saved one
// ------------------------------------------------------------------------
void vsBackfaceAttribute::restoreSaved()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    // Unlock the wireframe state if overriding was enabled
    if (overrideFlag)
        gState->unlockBackface(this);

    // Reset the current wireframe state to its previous value
    gState->setBackface((vsBackfaceAttribute *)(attrSaveList[--attrSaveCount]));
}

// ------------------------------------------------------------------------
// Internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsBackfaceAttribute::setState(pfGeoState *state)
{
    // Set the face culling mode and light model on the Performer geostate
    state->setMode(PFSTATE_CULLFACE, cullfaceVal);
    state->setAttr(PFSTATE_LIGHTMODEL, lightModel);
}

// ------------------------------------------------------------------------
// Internal function
// Determines if the specified attribute has state information that is
// equivalent to what this attribute has
// ------------------------------------------------------------------------
int vsBackfaceAttribute::isEquivalent(vsAttribute *attribute)
{
    vsBackfaceAttribute *attr;
    int val1, val2;
    
    // NULL check
    if (!attribute)
        return VS_FALSE;

    // Equal pointer check
    if (this == attribute)
        return VS_TRUE;
    
    // Type check
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_BACKFACE)
        return VS_FALSE;

    // Type cast
    attr = (vsBackfaceAttribute *)attribute;
    
    // State check
    val1 = isEnabled();
    val2 = attr->isEnabled();
    if (val1 != val2)
        return VS_FALSE;

    // Attributes are equivalent if all checks pass
    return VS_TRUE;
}
