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
