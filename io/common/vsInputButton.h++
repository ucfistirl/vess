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
//    VESS Module:  vsInputButton.h++
//
//    Description:  Class for storing and returning the state of an input
//                  device's button
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_INPUT_BUTTON_HPP
#define VS_INPUT_BUTTON_HPP

#include "vsGlobals.h++"
#include "vsTimer.h++"
#include "vsUpdatable.h++"

#define VS_IB_DBLCLICK_INTERVAL  0.2

class VS_IO_DLL vsInputButton : public vsUpdatable
{
protected:

    // Indicates the state of the button
    int          pressed;

    // Timer to measure time between button presses
    vsTimer      *buttonTimer;

    // Indicates whether or not the last press of the button was
    // a double-click
    int          doubleClicked;

    // The maximum time interval at which two consecutive presses 
    // are considered a "double-click"
    double       doubleClickInterval;

VS_INTERNAL:

    void            setPressed(void);
    void            setReleased(void);

    virtual void    update();

public:
  
                            vsInputButton();
    virtual                 ~vsInputButton();

    virtual const char *    getClassName();

    int                     isPressed(void);
    int                     wasDoubleClicked(void);

    void                    setDoubleClickInterval(double interval);
};

#endif
