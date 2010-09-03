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
//    VESS Module:  vsHWND.c++
//
//    Description:  vsObject wrapper for HWND's (Microsoft window handles)
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsHWND.h++"

// ------------------------------------------------------------------------
// Constructor.  Sets up the vsObject wrapper
// ------------------------------------------------------------------------
vsHWND::vsHWND(HWND theWindow)
{
    // Store the handle
    msWindow = theWindow;
}

// ------------------------------------------------------------------------
// Destructor.  Does nothing.
// ------------------------------------------------------------------------
vsHWND::~vsHWND()
{
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsHWND::getClassName()
{
    return "vsHWND";
}

// ------------------------------------------------------------------------
// Return the window handle
// ------------------------------------------------------------------------
HWND vsHWND::getHWND()
{
    return msWindow;
}

// ------------------------------------------------------------------------
// See if this window handle is the same as the given one
// ------------------------------------------------------------------------
bool vsHWND::equals(atItem *otherItem)
{
    vsHWND *otherHWND;

    // First, make sure the other item is a vsHWND instance
    otherHWND = dynamic_cast<vsHWND *>(otherItem);
    if (otherHWND == NULL)
    {
        // Can't be equivalent
        return false;
    }
    else
    {
        // We're interested in the wrapped window handles, and not their
        // VESS wrappers, so compare the values of the two handles
        if (this->getHWND() == otherHWND->getHWND())
            return true;
    }

    // If we get this far, they're not equivalent
    return false;
}

// ------------------------------------------------------------------------
// Compare this vsHWND to the given one.  In this case, this means
// comparing the values of the wrapped window handles
// ------------------------------------------------------------------------
int vsHWND::compare(atItem *otherItem)
{
    vsHWND *otherHWND;

    // First, make sure the other item is an OSG Node object
    otherHWND = dynamic_cast<vsHWND *>(otherItem);
    if (otherHWND == NULL)
    {
        // Not comparable as vsHWNDs.  Use the parent class method to
        // compare them
        return vsObject::compare(otherItem);
    }
    else
    {
        // We're interested in the wrapped window handles, and not their
        // VESS wrappers, so compare the values of the two handles
        return (otherHWND->getHWND() - this->getHWND());
    }
}

