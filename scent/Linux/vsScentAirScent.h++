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
//    VESS Module:  vsScent.h++
//
//    Description:  Descendant of the vsScent class providing support for
//                  the ScentAir olfactory device.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_SCENT_AIR_SCENT_HPP
#define VS_SCENT_AIR_SCENT_HPP

#include "vsScentAirSystem.h++"
#include "vsScent.h++"

#define VS_SASCENT_DEFAULT_CYCLE_TIME 10.0

class vsScentAirScent : public vsScent
{
protected:

    vsScentAirSystem    *scentAir;
    int                 scentChannel;
    double              strength;

    double              cycleTime;

public:

                          vsScentAirScent(vsScentAirSystem *system, 
                                           int channel);
                          ~vsScentAirScent();

    virtual const char    *getClassName();

    void                  setCycleTime(double newTime);
    double                getCycleTime();

    virtual double        getStrength();
    virtual void          setStrength(double newStrength);
};

#endif
