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
//    VESS Module:  vsOSGStateSet.c++
//
//    Description:  vsObject wrapper for osg::StateSet objects 
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsOSGStateSet.h++"

// ------------------------------------------------------------------------
// Constructor.  Sets up the vsObject wrapper and references the OSG
// StateSet
// ------------------------------------------------------------------------
vsOSGStateSet::vsOSGStateSet(osg::StateSet *theStateSet)
{
    // Store and reference the OSG state set
    osgStateSet = theStateSet;
    osgStateSet->ref();
}

// ------------------------------------------------------------------------
// Destructor.  Unreferences the wrapped OSG StateSet
// ------------------------------------------------------------------------
vsOSGStateSet::~vsOSGStateSet()
{
    // Unreference the OSG state set
    osgStateSet->unref();
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsOSGStateSet::getClassName()
{
    return "vsOSGStateSet";
}

// ------------------------------------------------------------------------
// Return the OSG node instance
// ------------------------------------------------------------------------
osg::StateSet *vsOSGStateSet::getStateSet()
{
    return osgStateSet;
}

// ------------------------------------------------------------------------
// See if this OSG StateSet is the same as the given one
// ------------------------------------------------------------------------
bool vsOSGStateSet::equals(atItem *otherItem)
{
    vsOSGStateSet *otherStateSet;

    // First, make sure the other item is an OSG StateSet object
    otherStateSet = dynamic_cast<vsOSGStateSet *>(otherItem);
    if (otherStateSet == NULL)
    {
        // Can't be equivalent
        return false;
    }
    else
    {
        // We're interested in the wrapped OSG StateSet objects, and not their
        // VESS wrappers, so compare the addresses of the OSG StateSets
        if (this->getStateSet() == otherStateSet->getStateSet())
            return true;
    }

    // If we get this far, they're not equivalent
    return false;
}

// ------------------------------------------------------------------------
// Compare this vsOSGStateSet to the given one.  In this case, this means
// comparing the addresses of the wrapped OSG StateSets
// ------------------------------------------------------------------------
int vsOSGStateSet::compare(atItem *otherItem)
{
    vsOSGStateSet *otherStateSet;

    // First, make sure the other item is an OSG StateSet object
    otherStateSet = dynamic_cast<vsOSGStateSet *>(otherItem);
    if (otherStateSet == NULL)
    {
        // Not comparable as vsOSGStateSets.  Use the parent class method to
        // compare them
        return vsObject::compare(otherItem);
    }
    else
    {
        // We're interested in the wrapped OSG StateSet objects, and not their
        // VESS wrappers, so compare the addresses of the OSG StateSet objects
        return (otherStateSet->getStateSet() - this->getStateSet());
    }
}

