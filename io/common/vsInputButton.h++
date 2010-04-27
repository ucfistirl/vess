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

#define VS_IB_DBLCLICK_INTERVAL  0.4

enum ButtonState
{
   VS_IB_STABLE,
   VS_IB_THIS_FRAME,
   VS_IB_LAST_FRAME
};

class VESS_SYM vsInputButton : public vsUpdatable
{
protected:

    // Indicates the immediate state of the button (simple pressed/released)
    bool         pressed;

    // Temporal states of the button (indicates button press/release relative
    // to the button update() cycle)
    ButtonState   pressedState;
    ButtonState   releasedState;

    // Timer to measure time between button presses
    vsTimer      *buttonTimer;

    // Indicates whether or not the last press of the button was
    // a double-click
    bool         doubleClicked;

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

    bool                    isPressed(void);
    bool                    wasPressed(void);
    bool                    wasReleased(void);
    bool                    wasDoubleClicked(void);

    void                    setDoubleClickInterval(double interval);
};

#endif
