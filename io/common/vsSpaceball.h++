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
//    VESS Module:  vsSpaceball.h++
//
//    Description:  A class for storing and returning the state of a
//                  spaceball
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_SPACEBALL_HPP
#define VS_SPACEBALL_HPP

#include "vs6DInputDevice.h++"

#define VS_SB_MAX_BUTTONS 9

class VESS_SYM vsSpaceball : public vs6DInputDevice
{
protected:

    // Number of buttons on spaceball
    int              numButtons;  
    vsInputButton    *button[VS_SB_MAX_BUTTONS];

VS_INTERNAL:

    void    setPosition(atVector posVec);
    void    setOrientation(atVector ornVec, atMathEulerAxisOrder axisOrder);
    void    setOrientation(atMatrix ornMat);
    void    setOrientation(atQuat ornQuat);

public:

                             vsSpaceball(int nButtons);
    virtual                  ~vsSpaceball(void);

    virtual const char       *getClassName();

    virtual int              getNumButtons(void);
    virtual vsInputButton    *getButton(int index);
};

#endif
