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
//    VESS Module:  vsVest.h++
//
//    Description:  Interface to IST's vibrating vest
//
//    Author(s):    Ryan Wilson
//
//------------------------------------------------------------------------

#ifndef VS_VEST_HPP
#define VS_VEST_HPP

#include "vsIODevice.h++"

class VESS_SYM vsVest : public vsIODevice
{
private:
    int                       numButtons;
    vsInputButton             **buttons;

public:

                              vsVest(int nButtons);
    virtual                   ~vsVest();

    // Inherited from vsObject
    virtual const char        *getClassName();

    // Input device methods
    virtual int               getNumAxes();
    virtual int               getNumButtons();

    virtual vsInputAxis       *getAxis(int index);
    virtual vsInputButton     *getButton(int index);
};

#endif
