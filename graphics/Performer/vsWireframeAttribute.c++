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

#include "vsWireframeAttribute.h++"

#include "vsGraphicsState.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes the Performer transparency value
// ------------------------------------------------------------------------
vsWireframeAttribute::vsWireframeAttribute()
{
    // Initialize the attribute to its default value of enabled
    wireValue = PFTR_ON;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsWireframeAttribute::~vsWireframeAttribute()
{
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
// Enables transparency
// ------------------------------------------------------------------------
void vsWireframeAttribute::enable()
{
    // Turn wireframe on
    wireValue = PFTR_ON;
    
    // Mark the nodes that have this attribute attached as needing
    // of an update
    markOwnersDirty();
}

// ------------------------------------------------------------------------
// Disables transparency
// ------------------------------------------------------------------------
void vsWireframeAttribute::disable()
{
    // Turn wireframe off
    wireValue = PFTR_OFF;
    
    // Mark the nodes that have this attribute attached as needing
    // of an update
    markOwnersDirty();
}

// ------------------------------------------------------------------------
// Returns a flag specifying is transparency is enabled
// ------------------------------------------------------------------------
int vsWireframeAttribute::isEnabled()
{
    // Interpret the current wireframe value
    if (wireValue == PFTR_ON)
        return VS_TRUE;
    else
        return VS_FALSE;
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsWireframeAttribute::attachDuplicate(vsNode *theNode)
{
    vsWireframeAttribute *newAttrib;
    
    // Create a duplicate wireframe attribute
    newAttrib = new vsWireframeAttribute();
    
    // Copy the wireframe enable value
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
void vsWireframeAttribute::saveCurrent()
{
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    // Save the current wireframe state in our save list
    attrSaveList[attrSaveCount++] = gState->getWireframe();
}

// ------------------------------------------------------------------------
// Internal function
// Sets the current attribute to this one
// ------------------------------------------------------------------------
void vsWireframeAttribute::apply()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    // Set the current wireframe state to this object
    gState->setWireframe(this);

    // Lock the wireframe state if overriding is enabled
    if (overrideFlag)
        gState->lockWireframe(this);
}

// ------------------------------------------------------------------------
// Internal function
// Restores the current attribute to the last saved one
// ------------------------------------------------------------------------
void vsWireframeAttribute::restoreSaved()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    // Unlock the wireframe state if overriding was enabled
    if (overrideFlag)
        gState->unlockWireframe(this);

    // Reset the current wireframe state to its previous value
    gState->setWireframe(
        (vsWireframeAttribute *)(attrSaveList[--attrSaveCount]));
}

// ------------------------------------------------------------------------
// Internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsWireframeAttribute::setState(pfGeoState *state)
{
    // Set the wireframe enable on the Performer geostate
    state->setMode(PFSTATE_ENWIREFRAME, wireValue);
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
    
    // NULL check
    if (!attribute)
        return VS_FALSE;

    // Equal pointer check
    if (this == attribute)
        return VS_TRUE;
    
    // Type check
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_WIREFRAME)
        return VS_FALSE;

    // Type cast
    attr = (vsWireframeAttribute *)attribute;

    // State check
    val1 = isEnabled();
    val2 = attr->isEnabled();
    if (val1 != val2)
        return VS_FALSE;

    // Attributes are equivalent if all checks pass
    return VS_TRUE;
}
