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
//    VESS Module:  vsWindowSystem.h++
//
//    Description:  Class to handle input events from the window system,
//                  specifically the mouse and keyboard.  This
//                  implementation is for X Window systems.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_WINDOW_SYSTEM_HPP
#define VS_WINDOW_SYSTEM_HPP

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
