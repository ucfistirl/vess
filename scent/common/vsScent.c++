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
//    VESS Module:  vsScent.c++
//
//    Description:  Abstract base class to abstract the properties of a 
//                  scent that can be delivered by an olfactory device.
//                  Each supported olfactory device must implement a
//                  descendant of this class.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsScent.h++"
#include "vsScentManager.h++"

// ------------------------------------------------------------------------
// Constructor for the vsScent base class.  Performs basic initialization
// ------------------------------------------------------------------------
vsScent::vsScent()
{
    // Initialize the scent strength to zero
    strength = 0.0;

    // Register the scent with the scent manager
    vsScentManager::getInstance()->addScent(this);
}

// ------------------------------------------------------------------------
// Destructor.  Does nothing
// ------------------------------------------------------------------------
vsScent::~vsScent()
{
    // Remove the scent from the scent manager
    vsScentManager::getInstance()->removeScent(this);
}

// ------------------------------------------------------------------------
// Returns the name of this class
// ------------------------------------------------------------------------
const char *vsScent::getClassName()
{
    return "vsScent";
}
