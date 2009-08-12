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
//    VESS Module:  vsHiball.h++
//
//    Description:  Class to handle input from a Hiball tracking system.
//                  There is no additional functionality added to the base
//                  vsVRPNTrackingSystem here.
//
//    Author(s):    Jason Daly, Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_HIBALL_HPP
#define VS_HIBALL_HPP

#include "vsVRPNTrackingSystem.h++"

class VESS_SYM vsHiball : public vsVRPNTrackingSystem
{
public:

                         vsHiball(atString hostName,
                                  atString trackerServerName,
                                  atString buttonServerName);
                         vsHiball(atString hostName, atString localName,
                                  atString trackerServerName,
                                  atString buttonServerName);
    virtual              ~vsHiball();

    virtual const char   *getClassName();
};

#endif

