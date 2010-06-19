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
//    VESS Module:  vsFlockOfBirds.h++
//
//    Description:  Subclass of vsAscensionSerialTrackingSystem intended
//                  to be used for the Flock of Birds.  This class
//                  currently adds no functionality to the base class.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsFlockOfBirds.h++"

// ------------------------------------------------------------------------
// Constructs a Flock of Birds on the specified port with the given number
// of FBB devices.  If nTrackers is zero, the class attempts to determine
// the number automatically.
// ------------------------------------------------------------------------
vsFlockOfBirds::vsFlockOfBirds(int portNumber, int nTrackers, int dFormat,
                               long baud, int mode)
    : vsAscensionSerialTrackingSystem(portNumber, nTrackers, dFormat, baud,
                                      mode)
  
{
}

// ------------------------------------------------------------------------
// Constructs a Flock of Birds on the specified port with the given number
// of FBB devices.  If nTrackers is zero, the class attempts to determine
// the number automatically.
// ------------------------------------------------------------------------
vsFlockOfBirds::vsFlockOfBirds(char *portDev, int nTrackers, int dFormat,
                               long baud, int mode)
    : vsAscensionSerialTrackingSystem(portDev, nTrackers, dFormat, baud,
                                      mode)
  
{
}

// ------------------------------------------------------------------------
// Constructs a Flock of Birds on the specified ports with the given number
// of trackers.  The nTrackers parameter must be correctly specified (a 
// value of zero is not valid in multi-serial configurations).
// ------------------------------------------------------------------------
vsFlockOfBirds::vsFlockOfBirds(int portNumbers[], int nTrackers, int dFormat, 
                               long baud)
    : vsAscensionSerialTrackingSystem(portNumbers, nTrackers, dFormat, baud)
{
}

// ------------------------------------------------------------------------
// Constructs a Flock of Birds on the specified ports with the given number
// of trackers.  The nTrackers parameter must be correctly specified (a 
// value of zero is not valid in multi-serial configurations).
// ------------------------------------------------------------------------
vsFlockOfBirds::vsFlockOfBirds(char *portDevs[], int nTrackers, int dFormat, 
                               long baud)
    : vsAscensionSerialTrackingSystem(portDevs, nTrackers, dFormat, baud)
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsFlockOfBirds::getClassName()
{
    return "vsFlockOfBirds";
}
