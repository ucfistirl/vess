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

class vsWindowSystem;

#include "vsInputSystem.h++"
#include "vsWindow.h++"
#include "vsMouse.h++"
#include "vsKeyboard.h++"

class vsWindowSystem : public vsInputSystem
{
protected:

    // The VESS window
    vsWindow       *vessWindow;

    // The X Display and window
    Display        *display;
    Window         window;

    // The mouse and keyboard objects
    vsMouse        *mouse;
    vsKeyboard     *keyboard;

    // Flag to indicate if the mouse is in the window
    int            mouseInWindow;

VS_INTERNAL:

    Display         *getDisplay();
    Window          getWindow();

public:

                    vsWindowSystem(vsWindow *mainWindow);
                    ~vsWindowSystem();

    vsMouse         *getMouse();   
    vsKeyboard      *getKeyboard();   

    int             isMouseInWindow();

    virtual void    update();
};

#endif
