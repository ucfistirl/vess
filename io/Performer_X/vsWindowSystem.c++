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

vsObjectMap *vsWindowSystem::windowMap = NULL;

// ------------------------------------------------------------------------
// Use the given vsWindow to obtain the main X Window, and select input 
// events to be received from that window.  Also, create the keyboard and
// mouse objects.
// ------------------------------------------------------------------------
vsWindowSystem::vsWindowSystem(vsWindow *mainWindow)
{
    int xSize, ySize;

    // Initialize variables
    vessWindow = mainWindow;
    display = NULL;
    window = 0x0;
    keyboard = NULL;
    mouse = NULL;
    
    // Check window for other window system
    if (getMap()->mapFirstToSecond(vessWindow))
    {
        printf("vsWindowSystem::vsWindowSystem: Specified vsWindow already "
            "has a vsWindowSystem\n");
        return;
    }

    // Get the X Display and Window
    display = pfGetCurWSConnection();
    window = mainWindow->getBaseLibraryObject()->getWSWindow();

    // Obtain the size of the window
    vessWindow->getSize(&xSize, &ySize);

    // Create the keyboard in Button mode, by default.  The user can 
    // change this later
    keyboard = new vsKeyboard(VS_KB_MODE_BUTTON);

    // Most mice have 2 axes and 3 buttons
    mouse = new vsMouse(2, 3, xSize, ySize);

    // Assume the mouse isn't in the window yet (an EnterNotify or 
    // PointerMotion event will change this)
    mouseInWindow = VS_FALSE;

    // Select the X Input events we want
    XSelectInput(display, window, PointerMotionHintMask | PointerMotionMask |
        ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask |
        EnterWindowMask | LeaveWindowMask);

    // Register the window
    getMap()->registerLink(vessWindow, this);
}

// ------------------------------------------------------------------------
// Delete the keyboard and mouse
// ------------------------------------------------------------------------
vsWindowSystem::~vsWindowSystem()
{
    // Delete the keyboard and mouse
    if (keyboard)
        delete keyboard;
    if (mouse)
        delete mouse;

    // Detach from the parent window
    if (getMap()->mapSecondToFirst(this))
	getMap()->removeLink(this, VS_OBJMAP_SECOND_LIST);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsWindowSystem::getClassName()
{
    return "vsWindowSystem";
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
// Return the window object map
// ------------------------------------------------------------------------
vsObjectMap *vsWindowSystem::getMap()
{
    if (!windowMap)
	windowMap = new vsObjectMap();

    return windowMap;
}

// ------------------------------------------------------------------------
// Static internal function
// Deletes the object map that holds the window mappings, if it exists
// ------------------------------------------------------------------------
void vsWindowSystem::deleteMap()
{
    if (windowMap)
    {
        delete windowMap;
        windowMap = NULL;
    }
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
// Return whether or not the mouse is currently in the window
// ------------------------------------------------------------------------
int vsWindowSystem::isMouseInWindow()
{
    return mouseInWindow;
}

// ------------------------------------------------------------------------
// Update function for window system.  This will route all the input events 
// to the correct devices
// ------------------------------------------------------------------------
void vsWindowSystem::update()
{
    XEvent            event;
    XWindowAttributes xattr;
    char              buffer[50];
    KeySym            keySym;
    Window            rootWin;
    Window            childWin;
    int               rootX, rootY, winX, winY;
    unsigned int      modMask;
    
    // Check to make sure that we have a valid display
    if (!display)
        return;

    // Process all the events we're interested in in the order they were
    // received
    while( XCheckWindowEvent( display, window, KeyReleaseMask | KeyPressMask |
                ButtonPressMask | ButtonReleaseMask | 
                EnterWindowMask | LeaveWindowMask |
                PointerMotionMask, &event ) )
    {
        switch( event.type )
        {
            case KeyPress:
                // Look up the string representing the key
                XLookupString(&(event.xkey), buffer, 
                    sizeof(buffer), &keySym, NULL); 

                // Pass the X KeySym and the string to the keyboard
                // to press the key
                keyboard->pressKey(keySym, buffer);
                break;

            case KeyRelease:
                // Look up the string representing the key
                XLookupString(&(event.xkey), buffer, 
                    sizeof(buffer), &keySym, NULL); 

                // Pass the X KeySym and the string to the keyboard
                // to release the key
                keyboard->releaseKey(keySym);
                break;

            case ButtonPress:
                // Press the appropriate vsInputButton on the mouse
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
                break;

            case ButtonRelease:
                // Release the appropriate vsInputButton on the mouse
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
                break;

            case MotionNotify:        
                // If we get a MotionNotify event, the mouse is definitely
                // in the window
                mouseInWindow = VS_TRUE;

                // Get the position of the mouse pointer
                if (XQueryPointer(display, window, &rootWin, &childWin, 
                   &rootX, &rootY, &winX, &winY, &modMask))
                {
                    // Set the position of the vsMouse axes
                    mouse->moveTo(winX, winY);
                }
                break;

            case EnterNotify:
                // The mouse pointer has entered the window
                mouseInWindow = VS_TRUE;
                break;

            case LeaveNotify:
                // The mouse pointer has left the window
                mouseInWindow = VS_FALSE;
                break;
        }
    }

    // Update the size of the parent window
    if (XGetWindowAttributes(display, window, &xattr) == 0)
    {
        winX = 0;
        winY = 0;
    }
    else
    {
        winX = xattr.width;
        winY = xattr.height;
    }

    // Change the mouse's axis ranges to match the window's size
    if (mouse)
    {
        mouse->getAxis(0)->setRange(0, winX);
        mouse->getAxis(0)->setIdlePosition(winX / 2);
        mouse->getAxis(1)->setRange(0, winY);
        mouse->getAxis(1)->setIdlePosition(winY / 2);
    }

    // Update the keyboard
    keyboard->update();
}
