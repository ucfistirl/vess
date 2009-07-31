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
//    VESS Module:  vsConfigAvatar.h++
//
//    Description:  Avatar subclass that operates completely off of the
//                  data within a configuration file; no subclassing of
//                  this class should be required.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_CONFIGAVATAR_HPP
#define VS_CONFIGAVATAR_HPP

#include "vsAvatar.h++"

#include "vsKinematics.h++"

class VESS_SYM vsConfigAvatar : public vsAvatar
{
private:

    vsArray            updateList;
    
    virtual void       setup();

public:

                          vsConfigAvatar();
                          vsConfigAvatar(vsNode *scene);
    virtual               ~vsConfigAvatar();

    virtual const char    *getClassName();

    virtual void          update();
};

#endif
