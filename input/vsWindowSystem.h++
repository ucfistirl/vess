#ifndef VS_WINDOW_SYSTEM_HPP
#define VS_WINDOW_SYSTEM_HPP

// Class to handle input events from the window system, specifically the 
// mouse and keyboard.  This implementation is for X Window systems

#include "X11/Xlib.h"
#include "X11/Xutil.h"
#include "X11/Xos.h"

#include "vsInputSystem.h++"
#include "vsWindow.h++"
#include "vsMouse.h++"
#include "vsKeyboard.h++"

class vsWindowSystem : public vsInputSystem
{
protected:

    // The X Display and window
    Display        *display;
    Window         window;

    vsMouse        *mouse;
    vsKeyboard     *keyboard;


VS_INTERNAL:

    Display         *getDisplay();
    Window          getWindow();

public:

                    vsWindowSystem(vsWindow *mainWindow);
                    ~vsWindowSystem();

    vsMouse         *getMouse();   
    vsKeyboard      *getKeyboard();   

    virtual void    update();
};

#endif
