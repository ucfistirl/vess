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
//    VESS Module:  vsAttribute.h++
//
//    Description:  Abstract base class for all objects that can be
//                  attached to various points on the scene graph.
//                  Attributes are attached to nodes in order to specify
//                  some alteration to the geometry at and below that
//                  node.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsAttribute.h++"

#include <string.h>
#include <Performer/pr/pfMemory.h>

// ------------------------------------------------------------------------
// Constructor - Initializes the object
// ------------------------------------------------------------------------
vsAttribute::vsAttribute()
{
    attributeName[0] = 0;
    attachedFlag = 0;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsAttribute::~vsAttribute()
{
}

// ------------------------------------------------------------------------
// Retrieves the value of the 'attached' flag for this attribute
// ------------------------------------------------------------------------
int vsAttribute::isAttached()
{
    return attachedFlag;
}

// ------------------------------------------------------------------------
// Sets the name of this attribute
// ------------------------------------------------------------------------
void vsAttribute::setName(char *newName)
{
    strncpy(attributeName, newName, VS_ATTRIBUTE_NAME_MAX_LENGTH);
    attributeName[VS_ATTRIBUTE_NAME_MAX_LENGTH - 1] = 0;
}

// ------------------------------------------------------------------------
// Retrieves the name of this attribute
// ------------------------------------------------------------------------
const char *vsAttribute::getName()
{
    return attributeName;
}

// ------------------------------------------------------------------------
// VESS internal function
// ------------------------------------------------------------------------
int vsAttribute::canAttach()
{
    return VS_TRUE;
}

// ------------------------------------------------------------------------
// VESS internal function
// Marks this attribute as attached
// ------------------------------------------------------------------------
void vsAttribute::attach(vsNode *theNode)
{
    attachedFlag++;
}

// ------------------------------------------------------------------------
// VESS internal function
// Removes an attachment mark from this attribute
// ------------------------------------------------------------------------
void vsAttribute::detach(vsNode *theNode)
{
    attachedFlag--;
}

// ------------------------------------------------------------------------
// VESS internal function
// Attempts to attach a copy of this attribute to the specified node. The
// default action for this function is to do nothing.
// ------------------------------------------------------------------------
void vsAttribute::attachDuplicate(vsNode *theNode)
{
}

// ------------------------------------------------------------------------
// VESS internal function
// Empty virtual base function
// ------------------------------------------------------------------------
void vsAttribute::saveCurrent()
{
}

// ------------------------------------------------------------------------
// VESS internal function
// Empty virtual base function
// ------------------------------------------------------------------------
void vsAttribute::apply()
{
}

// ------------------------------------------------------------------------
// VESS internal function
// Empty virtual base function
// ------------------------------------------------------------------------
void vsAttribute::restoreSaved()
{
}

// ------------------------------------------------------------------------
// VESS internal function
// Empty virtual base function
// ------------------------------------------------------------------------
void vsAttribute::setState(pfGeoState *state)
{
}
