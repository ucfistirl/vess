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
//    VESS Module:  vsUpdatable.h++
//
//    Description:  Virtual base class that specifies that any subclass
//                  will always contain an update method
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_UPDATABLE_HPP
#define VS_UPDATABLE_HPP

#include "vsObject.h++"

class VESS_SYM vsUpdatable : public vsObject
{
public:

                    vsUpdatable();
    virtual         ~vsUpdatable();

    virtual void    update() = 0;
};

#endif
