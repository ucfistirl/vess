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
//    VESS Module:  vsInputSystem.h++
//
//    Description:  Abstract base class for all classes that read input
//                  data from hardware
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_INPUT_SYSTEM_HPP
#define VS_INPUT_SYSTEM_HPP

class vsInputSystem
{
public:

                    vsInputSystem();
    virtual         ~vsInputSystem();

    virtual void    update() = 0;
};

#endif
