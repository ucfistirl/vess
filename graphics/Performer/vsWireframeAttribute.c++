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
    wireValue = PFTR_ON;
    
    markOwnersDirty();
}

// ------------------------------------------------------------------------
// Disables transparency
// ------------------------------------------------------------------------
void vsWireframeAttribute::disable()
{
    wireValue = PFTR_OFF;
    
    markOwnersDirty();
}

// ------------------------------------------------------------------------
// Returns a flag specifying is transparency is enabled
// ------------------------------------------------------------------------
int vsWireframeAttribute::isEnabled()
{
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
    
    newAttrib = new vsWireframeAttribute();
    
    if (isEnabled())
        newAttrib->enable();
    else
        newAttrib->disable();

    theNode->addAttribute(newAttrib);
}

// ------------------------------------------------------------------------
// Internal function
// Saves the current attribute
// ------------------------------------------------------------------------
void vsWireframeAttribute::saveCurrent()
{
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    attrSaveList[attrSaveCount++] = gState->getWireframe();
}

// ------------------------------------------------------------------------
// Internal function
// Sets the current attribute to this one
// ------------------------------------------------------------------------
void vsWireframeAttribute::apply()
{
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    gState->setWireframe(this);
    if (overrideFlag)
        gState->lockWireframe(this);
}

// ------------------------------------------------------------------------
// Internal function
// Restores the current attribute to the last saved one
// ------------------------------------------------------------------------
void vsWireframeAttribute::restoreSaved()
{
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    if (overrideFlag)
        gState->unlockWireframe(this);
    gState->setWireframe(
        (vsWireframeAttribute *)(attrSaveList[--attrSaveCount]));
}

// ------------------------------------------------------------------------
// Internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsWireframeAttribute::setState(pfGeoState *state)
{
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
    
    if (!attribute)
        return VS_FALSE;

    if (this == attribute)
        return VS_TRUE;
    
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_WIREFRAME)
        return VS_FALSE;

    attr = (vsWireframeAttribute *)attribute;

    val1 = isEnabled();
    val2 = attr->isEnabled();
    if (val1 != val2)
        return VS_FALSE;

    return VS_TRUE;
}