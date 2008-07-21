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

#ifndef VS_FLOCK_OF_BIRDS_HPP
#define VS_FLOCK_OF_BIRDS_HPP


#include "vsAscensionSerialTrackingSystem.h++"

class VESS_SYM vsFlockOfBirds : public vsAscensionSerialTrackingSystem
{
public:

                          vsFlockOfBirds(int portNumber, int nTrackers, 
                                         int dataFormat, long baud, int mode);
                          vsFlockOfBirds(int portNumbers[], int nTrackers,
                                         int dataFormat, long baud);

    virtual const char    *getClassName();
};

#endif
