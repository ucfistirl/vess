#ifndef VS_INPUT_BUTTON_HPP
#define VS_INPUT_BUTTON_HPP

// Class for storing and returning the state of an input device's button

#include "vsGlobals.h++"

#define VS_IB_DBLCLICK_INTERVAL  0.2

class vsInputButton
{
protected:

    // Indicates the state of the button
    int          pressed;

    // Indicates the time at which the button was last pressed
    double       lastPressedTime;      

    // The maximum time interval at which two consecutive presses 
    // are considered a "double-click"
    double       doubleClickInterval;

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
