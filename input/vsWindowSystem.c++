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
//    VESS Module:  vsWindowSystem.c++
//
//    Description:  Class to handle input events from the window system,
//                  specifically the mouse and keyboard.  This
//                  implementation is for X Window systems.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsWindowSystem.h++"
#include "Performer/pf/pfPipeWindow.h"

// NOTE:  This implementation uses Performer and X Windows functions

// ------------------------------------------------------------------------
// Use the given vsWindow to obtain the main X Window, and select input 
// events to be received from that window.  Also, create the keyboard and
// mouse objects
// ------------------------------------------------------------------------
vsWindowSystem::vsWindowSystem(vsWindow *mainWindow)
{
    int xSize, ySize;

    // Initialize variables
    keyboard = NULL;
    mouse = NULL;

    // Get the X Display and Window
    display = pfGetCurWSConnection();
    window = mainWindow->getBaseLibraryObject()->getWSWindow();

    // Obtain the size of the window
    mainWindow->getSize(&xSize, &ySize);

    // Create the keyboard in Button mode, by default.  The user can 
    // change this later
    keyboard = new vsKeyboard(VS_KB_MODE_BUTTON);

    // Most mice have 2 axes and 3 buttons
    mouse = new vsMouse(2, 3, xSize, ySize);

    // Select the X Input events we want
    XSelectInput(display, window, PointerMotionHintMask | PointerMotionMask |
        ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask |
        StructureNotifyMask);
}

// ------------------------------------------------------------------------
// Delete the keyboard and mouse
// ------------------------------------------------------------------------
vsWindowSystem::~vsWindowSystem()
{
    if (keyboard)
        delete keyboard;

    if (mouse)
        delete mouse;
}

// ------------------------------------------------------------------------
// Return the mouse object
// ------------------------------------------------------------------------
vsMouse *vsWindowSystem::getMouse()
{
    return mouse;
}

// ------------------------------------------------------------------------
// Return the keyboard object
// ------------------------------------------------------------------------
vsKeyboard *vsWindowSystem::getKeyboard()
{
    return keyboard;
}

// ------------------------------------------------------------------------
// Return the display or screen
// ------------------------------------------------------------------------
Display *vsWindowSystem::getDisplay()
{
    return display;
}

// ------------------------------------------------------------------------
// Return the window
// ------------------------------------------------------------------------
Window vsWindowSystem::getWindow()
{
    return window;
}

// ------------------------------------------------------------------------
// Update function for window system.  This will route all the input events 
// to the correct devices
// ------------------------------------------------------------------------
void vsWindowSystem::update()
{
    XEvent       event;
    char         buffer[50];
    KeySym       keySym;
    Window       rootWin;
    Window       childWin;
    int          rootX, rootY, winX, winY;
    unsigned int modMask;

    // Process all the events we're interested in

    while (XCheckTypedWindowEvent(display, window, KeyPress, &event))
    {
        XLookupString(&(event.xkey), buffer, sizeof(buffer), &keySym, NULL); 

        keyboard->pressKey(keySym, buffer);
    }
    while (XCheckTypedWindowEvent(display, window, KeyRelease, &event))
    {
        XLookupString(&(event.xkey), buffer, sizeof(buffer), &keySym, NULL); 

        keyboard->releaseKey(keySym);
    }
    while (XCheckTypedWindowEvent(display, window, ButtonPress, &event))
    {
        switch (event.xbutton.button)
        {
            case Button1:
                mouse->getButton(0)->setPressed();
                break;
            case Button2:
                mouse->getButton(1)->setPressed();
                break;
            case Button3:
                mouse->getButton(2)->setPressed();
                break;
        }
    }
    while (XCheckTypedWindowEvent(display, window, ButtonRelease, &event))
    {
        switch (event.xbutton.button)
        {
            case Button1:
                mouse->getButton(0)->setReleased();
                break;
            case Button2:
                mouse->getButton(1)->setReleased();
                break;
            case Button3:
                mouse->getButton(2)->setReleased();
                break;
        }
    }
    while (XCheckTypedWindowEvent(display, window, MotionNotify, 
        &event))
    {
        if (XQueryPointer(display, window, &rootWin, &childWin, 
            &rootX, &rootY, &winX, &winY, &modMask))
        {
            mouse->moveTo(winX, winY);
        }
    }
    while (XCheckTypedWindowEvent(display, window, ConfigureNotify,
        &event))
    {
        if (mouse)
        {
            mouse->getAxis(0)->setRange(0, event.xconfigure.width);
            mouse->getAxis(0)->setIdlePosition(event.xconfigure.width / 2);
            mouse->getAxis(1)->setRange(0, event.xconfigure.height);
            mouse->getAxis(1)->setIdlePosition(event.xconfigure.height / 2);
        }
    }

    // Update the keyboard
    keyboard->update();
}
