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
//    VESS Module:  vsSerialMotionStar.c++
//
//    Description:  Subclass of vsAscensionSerialTrackingSystem intended
//                  to be used for the MotionStar running via serial
//                  port(s).  This class currently adds no functionality
//                  to the base class.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsSerialMotionStar.h++"

// ------------------------------------------------------------------------
// Constructs a Serial MotionStar on the specified port with the given 
// number of FBB devices.  If nTrackers is zero, the class attempts to 
// determine the number automatically.
// ------------------------------------------------------------------------
vsSerialMotionStar::vsSerialMotionStar(int portNumber, int nTrackers, 
                                       int dFormat, long baud)
    : vsAscensionSerialTrackingSystem(portNumber, nTrackers, dFormat, baud,
                                      VS_AS_MODE_FLOCK)
{
}

// ------------------------------------------------------------------------
// Constructs a Serial MotionStar on the specified ports with the given 
// number of trackers.  The nTrackers parameter must be correctly specified 
// (a value of zero is not valid in multi-serial configurations).
// ------------------------------------------------------------------------
vsSerialMotionStar::vsSerialMotionStar(int portNumbers[], int nTrackers, 
                                       int dFormat, long baud)
    : vsAscensionSerialTrackingSystem(portNumbers, nTrackers, dFormat, baud)
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsSerialMotionStar::getClassName()
{
    return "vsSerialMotionStar";
}
