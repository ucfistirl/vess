#ifndef VS_SERIAL_MOTION_STAR_HPP
#define VS_SERIAL_MOTION_STAR_HPP

// Subclass of vsAscensionSerialTrackingSystem intended to be used
// for the MotionStar running via serial port(s).  This class currently 
// adds no functionality to the base class.
//
// One minor difference from the vsFlockOfBirds class is that the 
// vsSerialMotionStar class assumes Flock configuration (i.e., standalone
// operation is impossible with a MotionStar).  Thus there is no "mode" 
// parameter in the constructor.

#include "vsAscensionSerialTrackingSystem.h++"

class vsSerialMotionStar : public vsAscensionSerialTrackingSystem
{
public:

                 vsSerialMotionStar(int portNumber, int nTrackers, 
                                    int dataFormat, long baud);
                 vsSerialMotionStar(int portNumbers[], int nTrackers, 
                                    int dataFormat, long baud);
};

#endif
