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

// ------------------------------------------------------------------------
// Constructor - Initializes the object
// ------------------------------------------------------------------------
vsAttribute::vsAttribute()
{
    // Initialize the node name to an empty string, and the attachment
    // flag to false
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
// Internal function
// Determines if this attribute is available to be attached to a node
// ------------------------------------------------------------------------
bool vsAttribute::canAttach()
{
    return true;
}

// ------------------------------------------------------------------------
// Internal function
// Marks this attribute as attached
// ------------------------------------------------------------------------
void vsAttribute::attach(vsNode *theNode)
{
    attachedFlag++;
}

// ------------------------------------------------------------------------
// Internal function
// Removes an attachment mark from this attribute
// ------------------------------------------------------------------------
void vsAttribute::detach(vsNode *theNode)
{
    attachedFlag--;
}

// ------------------------------------------------------------------------
// Internal function
// Attempts to attach a copy of this attribute to the specified node. The
// default action for this function is to do nothing.
// ------------------------------------------------------------------------
void vsAttribute::attachDuplicate(vsNode *theNode)
{
}

// ------------------------------------------------------------------------
// Internal function
// Empty virtual base function
// ------------------------------------------------------------------------
void vsAttribute::saveCurrent()
{
}

// ------------------------------------------------------------------------
// Internal function
// Empty virtual base function
// ------------------------------------------------------------------------
void vsAttribute::apply()
{
}

// ------------------------------------------------------------------------
// Internal function
// Empty virtual base function
// ------------------------------------------------------------------------
void vsAttribute::restoreSaved()
{
}

// ------------------------------------------------------------------------
// Internal function
// Empty virtual base function
// ------------------------------------------------------------------------
void vsAttribute::setState(osg::StateSet *stateSet)
{
}
