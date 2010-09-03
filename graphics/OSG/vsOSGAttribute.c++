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
//    VESS Module:  vsOSGAttribute.c++
//
//    Description:  vsObject wrapper for osg::StateAttribute objects (and
//                  descendants)
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsOSGAttribute.h++"

// ------------------------------------------------------------------------
// Constructor.  Sets up the vsObject wrapper and references the OSG
// StateAttribute
// ------------------------------------------------------------------------
vsOSGAttribute::vsOSGAttribute(osg::StateAttribute *theAttribute)
{
    // Store and reference the OSG node
    osgAttribute = theAttribute;
    osgAttribute->ref();
}

// ------------------------------------------------------------------------
// Destructor.  Unreferences the wrapped OSG Attribute
// ------------------------------------------------------------------------
vsOSGAttribute::~vsOSGAttribute()
{
    // Unreference the OSG node
    osgAttribute->unref();
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsOSGAttribute::getClassName()
{
    return "vsOSGAttribute";
}

// ------------------------------------------------------------------------
// Return the OSG node instance
// ------------------------------------------------------------------------
osg::StateAttribute *vsOSGAttribute::getAttribute()
{
    return osgAttribute;
}

// ------------------------------------------------------------------------
// See if this OSG Attribute is the same as the given one
// ------------------------------------------------------------------------
bool vsOSGAttribute::equals(atItem *otherItem)
{
    vsOSGAttribute *otherAttribute;

    // First, make sure the other item is an OSG Attribute object
    otherAttribute = dynamic_cast<vsOSGAttribute *>(otherItem);
    if (otherAttribute == NULL)
    {
        // Can't be equivalent
        return false;
    }
    else
    {
        // We're interested in the wrapped OSG Attribute objects, and not their
        // VESS wrappers, so compare the addresses of the OSG Attributes
        if (this->getAttribute() == otherAttribute->getAttribute())
            return true;
    }

    // If we get this far, they're not equivalent
    return false;
}

// ------------------------------------------------------------------------
// Compare this vsOSGAttribute to the given one.  In this case, this means
// comparing the addresses of the wrapped OSG Attributes
// ------------------------------------------------------------------------
int vsOSGAttribute::compare(atItem *otherItem)
{
    vsOSGAttribute *otherAttribute;

    // First, make sure the other item is an OSG Attribute object
    otherAttribute = dynamic_cast<vsOSGAttribute *>(otherItem);
    if (otherAttribute == NULL)
    {
        // Not comparable as vsOSGAttributes.  Use the parent class method to
        // compare them
        return vsObject::compare(otherItem);
    }
    else
    {
        // We're interested in the wrapped OSG Attribute objects, and not their
        // VESS wrappers, so compare the addresses of the OSG Attribute objects
        return (otherAttribute->getAttribute() - this->getAttribute());
    }
}

