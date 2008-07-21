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
//    VESS Module:  vsSerialMotionStar.h++
//
//    Description:  Subclass of vsAscensionSerialTrackingSystem intended
//                  to be used for the MotionStar running via serial
//                  port(s).  This class currently adds no functionality
//                  to the base class.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_SERIAL_MOTION_STAR_HPP
#define VS_SERIAL_MOTION_STAR_HPP

// One minor difference from the vsFlockOfBirds class is that the 
// vsSerialMotionStar class assumes Flock configuration (i.e., standalone
// operation is impossible with a MotionStar).  Thus there is no "mode" 
// parameter in the constructor.

#include "vsAscensionSerialTrackingSystem.h++"

class VESS_SYM vsSerialMotionStar : public vsAscensionSerialTrackingSystem
{
public:

                          vsSerialMotionStar(int portNumber, int nTrackers, 
                                             int dataFormat, long baud);
                          vsSerialMotionStar(int portNumbers[], int nTrackers, 
                                             int dataFormat, long baud);

    virtual const char    *getClassName();
};

#endif
