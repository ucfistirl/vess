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

//class vsWindowSystem;

#include "vsObjectMap.h++"
#include "vsInputSystem.h++"
#include "vsWindow.h++"
#include "vsMouse.h++"
#include "vsKeyboard.h++"

#define VS_WS_MOUSE_WRAP_THRESHOLD_DEFAULT 6

class vsWindowSystem : public vsInputSystem
{
protected:

    // Maps vsWindows to vsWindowSystems
    static vsObjectMap *windowMap;

    // The VESS window
    vsWindow           *vessWindow;

    // The X Display and window
    Display            *display;
    Window             window;

    // The mouse and keyboard objects
    vsMouse            *mouse;
    vsKeyboard         *keyboard;

    // Flag to indicate if the mouse is in the window
    int                mouseInWindow;

    // Flag to indicate if mouse is grabbed to the given window or not
    int                mouseGrabbed;

    // Flag to indicate if the mouse cursor is hidden or not
    int                mouseCursorHidden;

    // Flags to indicate if the mouse cursor wrapped on the last update or not
    int                mouseWrapped[2];

    // Sets how many pixels from the edge of the window that will cause the
    // mouse to wrap
    int                mouseWrapping[2];

VS_INTERNAL:

    static vsObjectMap *getMap();
    static void        deleteMap();

    Display            *getDisplay();
    Window             getWindow();

public:

                          vsWindowSystem(vsWindow *mainWindow);
                          ~vsWindowSystem();

    virtual const char    *getClassName();

    vsMouse               *getMouse();   
    vsKeyboard            *getKeyboard();   

    int                   isMouseInWindow();

    void                  grabMouse();
    void                  unGrabMouse();
    int                   isMouseGrabbed();

    void                  hideCursor();
    void                  showCursor();
    int                   isCursorHidden();

    void                  getMouseLocation( int *x, int *y );
    void                  warpMouse( int x, int y );

    void                  enableMouseWrap( int axis );
    void                  disableMouseWrap( int axis );
    int                   isMouseWrapEnabled( int axis );
    void                  setMouseWrapThreshold( int axis, int threshold );
    int                   getMouseWrapThreshold( int axis );
    int                   didMouseWrap( int axis );

    virtual void          update();
};

#endif
