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

#define VS_IB_DBLCLICK_INTERVAL  0.2

class vsInputButton
{
protected:

    // Indicates the state of the button
    int          pressed;

    // Indicates the time at which the button was last pressed
    double       lastPressedTime;      

    // Indicates whether or not the last press of the button was
    // a double-click
    int          doubleClicked;

    // The maximum time interval at which two consecutive presses 
    // are considered a "double-click"
    double       doubleClickInterval;

    // Returns the current system time in seconds (for the double-click
    // measurements)
    double       getTime();

VS_INTERNAL:

    void         setPressed(void);
    void         setReleased(void);

public:
  
                 vsInputButton(void);
                 ~vsInputButton(void);

    int          isPressed(void);
    int          wasDoubleClicked(void);

    void         setDoubleClickInterval(double interval);
};

#endif