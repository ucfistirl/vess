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
//    VESS Module:  vsIOSystem.h++
//
//    Description:  Abstract base class for all classes that read input
//                  data from hardware
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_IO_SYSTEM_HPP
#define VS_IO_SYSTEM_HPP

#include "vsUpdatable.h++"

class VESS_SYM vsIOSystem : public vsUpdatable
{
public:

                    vsIOSystem();
    virtual         ~vsIOSystem();

    virtual void    update() = 0;
};

#endif
