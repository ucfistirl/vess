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
// Constructs a Flock of Birds on the specified ports with the given number
// of trackers.  The nTrackers parameter must be correctly specified (a 
// value of zero is not valid in multi-serial configurations).
// ------------------------------------------------------------------------
vsFlockOfBirds::vsFlockOfBirds(int portNumbers[], int nTrackers, int dFormat, 
                               long baud)
    : vsAscensionSerialTrackingSystem(portNumbers, nTrackers, dFormat, baud)
{
}
