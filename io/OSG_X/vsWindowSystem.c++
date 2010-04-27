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

#include <stdio.h>
#include "vsWindowSystem.h++"

// NOTE:  This implementation uses Open Scene Graph and X Windows functions

vsObjectMap *vsWindowSystem::windowMap = NULL;

// ------------------------------------------------------------------------
// Use the given vsWindow to obtain the main X Window, and select input 
// events to be received from that window.  Also, create the keyboard and
// mouse objects.
// ------------------------------------------------------------------------
vsWindowSystem::vsWindowSystem(vsWindow *mainWindow)
{
    int xSize, ySize;

    XWindowAttributes winAttrs;
    long              oldEventMask;

    // Initialize variables
    vessWindow = mainWindow;
    display = NULL;
    window = 0x0;
    keyboard = NULL;
    mouse = NULL;
    mouseCursorHidden = false;
    mouseGrabbed = false;
    mouseWrapped[0] =  mouseWrapped[1] = false;
    mouseWrapping[0] = mouseWrapping[1] = 0;
    
    // Check window for other window system
    if (getMap()->mapFirstToSecond(vessWindow))
    {
        printf("vsWindowSystem::vsWindowSystem: Specified vsWindow already "
            "has a vsWindowSystem\n");
        return;
    }

    // Get the X Display and Window
    display = mainWindow->getParentScreen()->getParentPipe()->getXDisplay();
    window = mainWindow->getBaseLibraryObject();

    // Obtain the size of the window
    vessWindow->getSize(&xSize, &ySize);

    // Create the keyboard in Button mode, by default.  The user can 
    // change this later
    keyboard = new vsKeyboard(VS_KB_MODE_BUTTON);
    keyboard->ref();

    // Most mice have at least 2 axes and 3 buttons, but many mice have a
    // wheel that X interprets as buttons 4 (scroll up) and 5 (scroll down).
    // We'll convert the wheel "buttons" into a non-normalized axis, so
    // user programs can track the relative axis changes
    mouse = new vsMouse(3, 3, xSize, ySize);
    mouse->ref();

    // Set the mouse wheel "buttons" to their default indices
    mouse_wheel_up_button_index = VS_WS_MOUSE_WHEEL_UP_BUTTON_DEFAULT;
    mouse_wheel_down_button_index = VS_WS_MOUSE_WHEEL_DOWN_BUTTON_DEFAULT;

    // Assume the mouse isn't in the window yet (an EnterNotify or 
    // PointerMotion event will change this)
    mouseInWindow = false;

    // Change the window's event mask to select the X Input events we want
    // First, get the current event mask
    XGetWindowAttributes(display, window, &winAttrs);
    oldEventMask = winAttrs.your_event_mask;

    // Add the events we want to the event mask
    XSelectInput(display, window, oldEventMask | PointerMotionHintMask | 
        PointerMotionMask | ButtonPressMask | ButtonReleaseMask | 
        KeyPressMask | KeyReleaseMask | EnterWindowMask | LeaveWindowMask);

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
        vsObject::unrefDelete(keyboard);
    if (mouse)
        vsObject::unrefDelete(mouse);

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
// Static internal function
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
bool vsWindowSystem::isMouseInWindow()
{
    return mouseInWindow;
}

// ------------------------------------------------------------------------
// Sets index of the two "buttons" used to represent mouse wheel scrolling.
// The popular Linux X servers (X.org and XFree86) use this technique to
// represent the wheel.  The button indices are 1-based to match the
// X server's configuration file
// ------------------------------------------------------------------------
void vsWindowSystem::setMouseWheelButtons(int upButton, int downButton)
{
    mouse_wheel_up_button_index = upButton;
    mouse_wheel_down_button_index = downButton;
}

// ------------------------------------------------------------------------
// Retrieves the indices of the two "buttons" used to represent mouse
// wheel scrolling
// ------------------------------------------------------------------------
void vsWindowSystem::getMouseWheelButtons(int *upButton, int *downButton)
{
    if (upButton != NULL)
        *upButton = mouse_wheel_up_button_index;
    if (downButton != NULL)
        *downButton = mouse_wheel_down_button_index;
}

// ------------------------------------------------------------------------
// Update function for window system.  This will route all the input events 
// to the correct devices
// ------------------------------------------------------------------------
void vsWindowSystem::update()
{
    XEvent            event;
    XEvent            nextEvent;
    XWindowAttributes xattr;
    char              buffer[50];
    KeySym            keySym;
    Window            rootWin;
    Window            childWin;
    int               rootX, rootY, winX, winY;
    unsigned int      modMask;
    vsInputAxis       *axis;
    double            pos;
    
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
                // See if the next event in the queue is a KeyPress event
                // for the same key (if so, it may be an auto-repeat event)
                if (XCheckWindowEvent(display, window, KeyPressMask,
                                      &nextEvent))
                {
                    // See if this event's keycode and timestamp matches the
                    // key release's keycode and timestamp
                    if ((nextEvent.xkey.keycode == event.xkey.keycode) &&
                        (nextEvent.xkey.time == event.xkey.time))
                    {
                        // This event is an auto-repeat of the same key, so
                        // ignore both of them
                    }
                    else
                    {
                        // Not an auto-repeat, put the event back and process
                        // the original event normally
                        XPutBackEvent(display, &nextEvent);

                        // Look up the string representing the key
                        XLookupString(&(event.xkey), buffer, 
                            sizeof(buffer), &keySym, NULL); 

                        // Pass the X KeySym and the string to the keyboard
                        // to release the key
                        keyboard->releaseKey(keySym);
                    }
                }
                else
                {
                    // Look up the string representing the key
                    XLookupString(&(event.xkey), buffer, 
                        sizeof(buffer), &keySym, NULL); 

                    // Pass the X KeySym and the string to the keyboard
                    // to release the key
                    keyboard->releaseKey(keySym);
                }
                break;

            case ButtonPress:

                // Press the appropriate vsInputButton on the mouse
                if (event.xbutton.button == Button1)
                {
                    mouse->getButton(0)->setPressed();
                }
                else if (event.xbutton.button == Button2)
                {
                    mouse->getButton(1)->setPressed();
                }
                else if (event.xbutton.button == Button3)
                {
                    mouse->getButton(2)->setPressed();
                }
                else if (event.xbutton.button == mouse_wheel_up_button_index)
                {
                    axis = mouse->getAxis(VS_MOUSE_WHEEL_AXIS);
                    pos = axis->getPosition();
                    axis->setPosition(pos + 1.0);
                }
                else if (event.xbutton.button == mouse_wheel_down_button_index)
                {
                    axis = mouse->getAxis(VS_MOUSE_WHEEL_AXIS);
                    pos = axis->getPosition();
                    axis->setPosition(pos - 1.0);
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
                mouseInWindow = true;

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
                mouseInWindow = true;
                break;

            case LeaveNotify:
                // The mouse pointer has left the window
                mouseInWindow = false;
                break;
        }
    }

    // Update the size of the parent window
    if (XGetWindowAttributes(display, window, &xattr) == 0)
    {
        winX = 0;
        winY = 0;
        rootWin = 0;
    }
    else
    {
        winX = xattr.width;
        winY = xattr.height;
        rootWin = xattr.root;
    }

    // Change the mouse's axis ranges to match the window's size
    if (mouse)
    {
        mouse->getAxis(0)->setRange(0, winX);
        mouse->getAxis(0)->setIdlePosition(winX / 2);
        mouse->getAxis(1)->setRange(0, winY);
        mouse->getAxis(1)->setIdlePosition(winY / 2);
    }

    // Reset the mouse wrapped flags
    mouseWrapped[0] = false; //x axis
    mouseWrapped[1] = false; //y axis

    // If we're wrapping for one of the mouse axes, do it now
    if( mouseWrapping[0] || mouseWrapping[1] )
    {
        int mousex, mousey;
        int newMouseX, newMouseY;

        // Wrapping pixel boundaries (when the mouse passes between these
        // boundaries and the edge of the window, it will wrap)
        int wrapLeft, wrapTop, wrapRight, wrapBottom;

        int screenWidth, screenHeight;

        // The location of the upper-left corner of the window on the screen
        int windowXLocation, windowYLocation;

        // Placeholder to get information from X
        Window child;

        // Get the current mouse positions (in window pixels)
        getMouseLocation( &mousex, &mousey );

        // Get the size of the screen
        vessWindow->getParentScreen()->getScreenSize(
            &screenWidth, &screenHeight);

        // Get the location of our window on the whole screen
        XTranslateCoordinates( display, window, rootWin,
            0, 0, &windowXLocation, &windowYLocation, &child );

        // Because there's the possibility of part of this window being
        // off-screen, we need to handle the code for mouse wrapping so that
        // the mouse will still wrap. (The mouse can not move offscreen in X11)
        // The following code calculates the pixel positions that mark the
        // edges of the window that are on-screen
        if( (windowXLocation + winX) >= screenWidth )
            wrapRight = screenWidth - windowXLocation;
        else
            wrapRight = winX;

        if( (windowYLocation + winY) >= screenHeight )
            wrapBottom = screenHeight - windowYLocation;
        else
            wrapBottom = winY;

        if( windowXLocation < 0 )
            wrapLeft = -windowXLocation;
        else
            wrapLeft = 0;

        if( windowYLocation < 0 )
            wrapTop = -windowYLocation;
        else
            wrapTop = 0;

        // If we're wrapping for the X axis...
        if( mouseWrapping[0] )
        {
            if( mousex < (wrapLeft + mouseWrapping[0]) )
            {
                // We're left of the threshold, warp to the right side of the
                // window
                mouseWrapped[0] = true;

                // Always move just beyond the right threshold so that we don't
                // immediately warp back (that's what the "*2" is here for. It
                // could possible work as "+1" instead
                newMouseX = wrapRight - mouseWrapping[0]*2;
            }
            else if( mousex > (wrapRight - mouseWrapping[0] - 1) )
            {
                // We're right of the threshold, warp to the left side of the
                // window
                mouseWrapped[0] = true;
                newMouseX = wrapLeft + mouseWrapping[0]*2;
            }
            else
                newMouseX = mousex;
        }
        else
            newMouseX = mousex;

        if( mouseWrapping[1] )
        {
            if( mousey < (wrapTop + mouseWrapping[1]) )
            {
                // We're above the threshold, warp to the bottom side of the
                // window
                mouseWrapped[1] = true;
                newMouseY = wrapBottom - mouseWrapping[1]*2;
            }
            else if( mousey > (wrapBottom - mouseWrapping[1] - 1) )
            {
                // We're below the threshold, warp to the upper side of the
                // window
                mouseWrapped[1] = true;
                newMouseY = wrapTop + mouseWrapping[1]*2;
            }
            else
                newMouseY = mousey;
        }
        else
            newMouseY = mousey;

        // First update the mouse axes to prepare it for the wrap
        if( mouseWrapped[0] )
            mouse->getAxis(0)->forceShiftPreviousPosition( newMouseX - mousex );
        if( mouseWrapped[1] )
            mouse->getAxis(1)->forceShiftPreviousPosition( newMouseY - mousey );

        // if we need to warp the mouse, do it now
        if( mouseWrapped[0] || mouseWrapped[1] )
            warpMouse( newMouseX, newMouseY );
    }

    // Update the keyboard
    keyboard->update();

    // Update the mouse as well
    mouse->update();
}

// ------------------------------------------------------------------------
// Grab the mouse (i.e. confine it to the current window)
// ------------------------------------------------------------------------
void vsWindowSystem::grabMouse()
{
    // Make sure we have a display and that the mouse is not already grabbed
    if( display!=NULL && mouseGrabbed==false )
    {
        // Grab the mouse with X
        XGrabPointer( display, window, True, 0, GrabModeAsync,
            GrabModeAsync, window, None, CurrentTime );

        // Mark that we grabbed the mouse
        mouseGrabbed = true;
    }
}

// ------------------------------------------------------------------------
// Un-grab the mouse (if it is currently grabbed)
// ------------------------------------------------------------------------
void vsWindowSystem::unGrabMouse( )
{
    // Make sure we have a display and that the mouse is already grabbed
    if( display!=NULL && mouseGrabbed==true )
    {
        // Ungrab the mouse with X
        XUngrabPointer( display, CurrentTime );

        // Mark the mouse as ungrabbed
        mouseGrabbed = false;
    }
}

// ------------------------------------------------------------------------
// Is the mouse grabbed? (Is the mouse confined to the window?)
// ------------------------------------------------------------------------
bool vsWindowSystem::isMouseGrabbed()
{
    return mouseGrabbed;
}

// ------------------------------------------------------------------------
// Enable mouse wrapping (sets a default threshold)
// NOTE: only enables it if it's not already enabled (this prevents us from
// overwriting an already existing threshold)
// ------------------------------------------------------------------------
void vsWindowSystem::enableMouseWrap( int axis )
{
    // If mouse wrap is not already enabled, enable it now with our default
    if( isMouseWrapEnabled( axis )==false )
        setMouseWrapThreshold( axis, VS_WS_MOUSE_WRAP_THRESHOLD_DEFAULT );
}

// ------------------------------------------------------------------------
// Disable mouse wrapping
// ------------------------------------------------------------------------
void vsWindowSystem::disableMouseWrap( int axis )
{
    // Disable mouse wrapping by setting the threshold to 0
    setMouseWrapThreshold( axis, 0 );
}

// ------------------------------------------------------------------------
// Return a boolean value indicating whether or not mouse wrapping is on
// for a given axis
// ------------------------------------------------------------------------
bool vsWindowSystem::isMouseWrapEnabled( int axis )
{
    // If the threshold is not equal to 0, we're in wrap mode
    if( getMouseWrapThreshold( axis ) != 0 )
        return true;
    else
        return false;
}

// ------------------------------------------------------------------------
// Set the threshold at which point the mouse wraps (this is the number of
// pixels from the edge of the window)
// ------------------------------------------------------------------------
void vsWindowSystem::setMouseWrapThreshold( int axis, int threshold )
{
    // We can't have negative thresholds
    if( (axis==0 || axis==1) && threshold>=0 )
        mouseWrapping[ axis ] = threshold;
}

// ------------------------------------------------------------------------
// Gets the threshold at which point the mouse wraps (this is the number of
// pixels from the edge of the window)
// ------------------------------------------------------------------------
int vsWindowSystem::getMouseWrapThreshold( int axis )
{
    if( axis==0 || axis==1 )
        return mouseWrapping[ axis ];
    else
        return 0;
}

// ------------------------------------------------------------------------
// Did the mouse wrap on the last update()?
// ------------------------------------------------------------------------
bool vsWindowSystem::didMouseWrap( int axis )
{
    if( axis>=0 && axis<2 )
        return mouseWrapped[axis];
    else
        return false;
}

// ------------------------------------------------------------------------
// Hide the mouse cursor (if it is not hidden)
// ------------------------------------------------------------------------
void vsWindowSystem::hideCursor( )
{
    Pixmap blank;
    XColor dummyColor;
    char data[1]={0};
    Cursor blankCursor;

    // Make sure we have a display and that the cursor is not already hidden
    if( !display || mouseCursorHidden )
        return;

    // Create a pixmap that is 1x1 in size and is a transparent
    blank = XCreateBitmapFromData( display, window, data, 1, 1 );

    // Turn that pixmap into a cursor
    blankCursor = XCreatePixmapCursor( display, blank, blank, &dummyColor,
        &dummyColor, 0, 0 );

    // Free the space from creating the pixmap
    XFreePixmap( display, blank );

    // Make the cursor the current cursor
    XDefineCursor( display, window, blankCursor );

    // Free the cursor (this will happen when showCursor() is called)
    XFreeCursor( display, blankCursor );

    // Mark the cursor as hidden
    mouseCursorHidden = true;
}

// ------------------------------------------------------------------------
// Show the mouse cursor (if it was hidden)
// ------------------------------------------------------------------------
void vsWindowSystem::showCursor( )
{
    // If we have a display and the cursor is hidden...
    if( display && mouseCursorHidden )
    {
        // Turn the new cursor off in the window
        XUndefineCursor( display, window );

        // Mark the cursor as visible
        mouseCursorHidden = false;
    }
}

// ------------------------------------------------------------------------
// Is the mouse cursor currently hidden?
// ------------------------------------------------------------------------
bool vsWindowSystem::isCursorHidden( )
{
    return mouseCursorHidden;
}

// ------------------------------------------------------------------------
// Warp (Jump) the mouse to the given location (actually moves the pointer
// in the X11 window)
// ------------------------------------------------------------------------
void vsWindowSystem::warpMouse( int x, int y )
{
    if( display )
    {
        // Tell the vsMouse to move to the given location
        mouse->moveTo( x, y );

        // Warp the pointer to the given location in X
        XWarpPointer( display, None, window, 0,0,0,0, x, y );
    }
}

// ------------------------------------------------------------------------
// Get the current mouse position in the current window
// ------------------------------------------------------------------------
void vsWindowSystem::getMouseLocation( int *x, int *y )
{
    XWindowAttributes xattr;
    int winSizeX, winSizeY, midWinX, midWinY;

    if( !display || XGetWindowAttributes( display, window, &xattr )==0 )
    {
        // If there's no display or no window, the window size will be 0x0
        winSizeX = 0;
        winSizeY = 0;
    }
    else
    {
        winSizeX = xattr.width;
        winSizeY = xattr.height;
    }
    midWinX  = winSizeX / 2;
    midWinY  = winSizeY / 2;

    // Set the x location of the mouse if the x variable is not null
    if( x != NULL )
    {
        // Get the current mouse positions (in window pixels)
        if( mouse->getAxis(0)->isNormalized() )
            *x = (int)rint( mouse->getAxis(0)->getPosition()
                    * midWinX + midWinX );
        else
            *x = (int)rint( mouse->getAxis(0)->getPosition() );
    }

    // Set the y location of the mouse if the y variable is not null
    if( y != NULL )
    {
        // Get the current mouse positions (in window pixels)
        if( mouse->getAxis(1)->isNormalized() )
            *y = (int)rint( mouse->getAxis(1)->getPosition()
                    * midWinY + midWinY );
        else
            *y = (int)rint( mouse->getAxis(1)->getPosition() );
    }
}
