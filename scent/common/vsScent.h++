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
//    Description:  Abstract base class to abstract the properties of a 
//                  scent that can be delivered by an olfactory device.
//                  Each supported olfactory device must implement a
//                  descendant of this class.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#ifndef VS_SCENT_HPP
#define VS_SCENT_HPP

#include "vsObject.h++"

class VESS_SYM vsScent : public vsObject
{
protected:

    double    strength;

public:

                          vsScent();
                          ~vsScent();

    virtual const char    *getClassName();

    virtual double        getStrength() = 0;
    virtual void          setStrength(double newStrength) = 0;
};

#endif
