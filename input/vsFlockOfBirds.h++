#ifndef VS_FLOCK_OF_BIRDS_HPP
#define VS_FLOCK_OF_BIRDS_HPP

// Subclass of vsAscensionSerialTrackingSystem intended to be used
// for the Flock of Birds.  This class currently adds no functionality to
// the base class.

#include "vsAscensionSerialTrackingSystem.h++"

class vsFlockOfBirds : public vsAscensionSerialTrackingSystem
{
public:

                 vsFlockOfBirds(int portNumber, int nTrackers, 
                                int dataFormat, long baud, int mode);
                 vsFlockOfBirds(int portNumbers[], int nTrackers,
                                int dataFormat, long baud);
};

#endif
