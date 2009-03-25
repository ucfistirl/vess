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
//    VESS Module:  vsHiball.c++
//
//    Description:  Class to handle input from a Hiball tracking system.
//                  This class adds no additional functionality to its
//                  base vsVRPNTrackingSystem class.
//
//    Author(s):    Casey Thurston, Jason Daly
//
//------------------------------------------------------------------------

#include "vsHiball.h++"

// ------------------------------------------------------------------------
// Constructor, calls the base class constructor
// ------------------------------------------------------------------------
vsHiball::vsHiball(atString hostName, atString trackerServerName,
                   atString buttonServerName) 
        : vsVRPNTrackingSystem(hostName, trackerServerName, buttonServerName)
{
}

// ------------------------------------------------------------------------
// Constructor, calls the base class constructor
// ------------------------------------------------------------------------
vsHiball::vsHiball(atString hostName, atString localName,
                   atString trackerServerName, atString buttonServerName)
        : vsVRPNTrackingSystem(hostName, localName,
                               trackerServerName, buttonServerName)
{
}

// ------------------------------------------------------------------------
// Destructor, does nothing extra
// ------------------------------------------------------------------------
vsHiball::~vsHiball()
{
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char * vsHiball::getClassName()
{
    return "vsHiball";
}
