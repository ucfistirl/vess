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
//                  implementation is for Microsoft Windows systems.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_WINDOW_SYSTEM_HPP
#define VS_WINDOW_SYSTEM_HPP

#include <windows.h>
#include "vsObjectMap.h++"
#include "vsIOSystem.h++"
#include "vsWindow.h++"
#include "vsMouse.h++"
#include "vsKeyboard.h++"

#define VS_WS_MOUSE_WRAP_THRESHOLD_DEFAULT 6

class VS_IO_DLL vsWindowSystem : public vsIOSystem
{
protected:

    // Maps vsWindows to vsWindowSystems
    static vsObjectMap          *windowMap;

    // The VESS window
    vsWindow                    *vessWindow;

    // Handle to the Win32 window
    HWND                        window;

    // The mouse and keyboard objects
    vsMouse                     *mouse;
    vsKeyboard                  *keyboard;

    // Flag to indicate if the mouse is in the window
    bool                        mouseInWindow;

    // Flag to indicate if mouse is grabbed to the given window or not
    bool                        mouseGrabbed;
    
    // Rectangular area indicating the cursor's allowed movement before
    // we grabbed it (usually indicates the full screen)
    RECT                        oldCursorRect;

    // Flag to indicate if the mouse cursor is hidden or not
    bool                        mouseCursorHidden;

    // Flags to indicate if the mouse cursor wrapped on the last update or not
    bool                        mouseWrapped[2];

    // Sets how many pixels from the edge of the window that will cause the
    // mouse to wrap
    int                         mouseWrapping[2];    
    
    // Original window procedure for the window (set by the vsWindow class)
    WNDPROC                     originalWindowProc;
    
    // New window procedure to handle input events
    static LRESULT CALLBACK     inputWindowProc(HWND msWindow, UINT message,
                                                WPARAM wParam, LPARAM lParam);

VS_INTERNAL:

    static vsObjectMap    *getMap();
    static void           deleteMap();

    HWND                  getWindow();

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
