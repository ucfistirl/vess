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
#undef index

#include "vsObjectMap.h++"
#include "vsIOSystem.h++"
#include "vsWindow.h++"
#include "vsMouse.h++"
#include "vsKeyboard.h++"

#define VS_WS_MOUSE_WRAP_THRESHOLD_DEFAULT 6

class vsWindowSystem : public vsIOSystem
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
    bool               mouseInWindow;

    // Flag to indicate if mouse is grabbed to the given window or not
    bool               mouseGrabbed;

    // Flag to indicate if the mouse cursor is hidden or not
    bool               mouseCursorHidden;

    // Flags to indicate if the mouse cursor wrapped on the last update or not
    bool               mouseWrapped[2];

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
    virtual               ~vsWindowSystem();

    virtual const char    *getClassName();

    vsMouse               *getMouse();   
    vsKeyboard            *getKeyboard();   

    bool                  isMouseInWindow();

    void                  grabMouse();
    void                  unGrabMouse();
    bool                  isMouseGrabbed();

    void                  hideCursor();
    void                  showCursor();
    bool                  isCursorHidden();

    void                  getMouseLocation( int *x, int *y );
    void                  warpMouse( int x, int y );

    void                  enableMouseWrap( int axis );
    void                  disableMouseWrap( int axis );
    bool                  isMouseWrapEnabled( int axis );
    void                  setMouseWrapThreshold( int axis, int threshold );
    int                   getMouseWrapThreshold( int axis );
    bool                  didMouseWrap( int axis );

    virtual void          update();
};

#endif
