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
//                  implementation is for Microsoft Windows systems.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsWindowSystem.h++"
#include <windowsx.h>

// NOTE:  This implementation uses Open Scene Graph and MS Windows functions

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

    // Get the MS Window
    window = mainWindow->getBaseLibraryObject();

    // Obtain the size of the window
    vessWindow->getSize(&xSize, &ySize);

    // Create the keyboard in Button mode, by default.  The user can 
    // change this later
    keyboard = new vsKeyboard(VS_KB_MODE_BUTTON);

    // Assume the mouse has 2 axes and 3 buttons.  If only 2 buttons are
    // present, button 1 (the middle button) will simply never be pressed.
    mouse = new vsMouse(2, 3, xSize, ySize);

    // Assume the mouse isn't in the window yet
    mouseInWindow = false;
    
    // Subclass the MS Window associated with the vsWindow to install an
    // additional window procedure
    originalWindowProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, 
        (LONG_PTR)inputWindowProc);

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
        
    // Restore the original window procedure
    SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)originalWindowProc);

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
// Return the Microsoft Window
// ------------------------------------------------------------------------
HWND vsWindowSystem::getWindow()
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
// Grab the mouse (i.e. confine it to the current window)
// ------------------------------------------------------------------------
void vsWindowSystem::grabMouse()
{
    RECT windowRect;
    POINT upperLeft, lowerRight;
    
    // Make sure we have a display and that the mouse is not already grabbed
    if ((window != NULL) && (mouseGrabbed == false))
    {
        // Get the current cursor area
        GetClipCursor(&oldCursorRect);
        
        // Get the rectangle containing the window's client area
        GetClientRect(window, &windowRect);
        
        // Translate the rectangle's corner points to screen coordinates
        upperLeft.x = windowRect.left;
        upperLeft.y = windowRect.top;
        lowerRight.x = windowRect.right;
        lowerRight.y = windowRect.bottom;
        ClientToScreen(window, &upperLeft);
        ClientToScreen(window, &lowerRight);
        windowRect.left = upperLeft.x;
        windowRect.top = upperLeft.y;
        windowRect.right = lowerRight.x;
        windowRect.bottom = lowerRight.y;
        
        // Confine the cursor to the window's client area
        ClipCursor(&windowRect);

        // Mark that we grabbed the mouse
        mouseGrabbed = true;
    }
}

// ------------------------------------------------------------------------
// Un-grab the mouse (if it is currently grabbed)
// ------------------------------------------------------------------------
void vsWindowSystem::unGrabMouse()
{
    // Make sure that the mouse is already grabbed
    if (mouseGrabbed == true)
    {
        // Ungrab the mouse cursor
        ClipCursor(&oldCursorRect);

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
void vsWindowSystem::enableMouseWrap(int axis)
{
    // If mouse wrap is not already enabled, enable it now with our default
    if(isMouseWrapEnabled(axis) == false)
        setMouseWrapThreshold(axis, VS_WS_MOUSE_WRAP_THRESHOLD_DEFAULT);
}

// ------------------------------------------------------------------------
// Disable mouse wrapping
// ------------------------------------------------------------------------
void vsWindowSystem::disableMouseWrap(int axis)
{
    // Disable mouse wrapping by setting the threshold to 0
    setMouseWrapThreshold(axis, 0);
}

// ------------------------------------------------------------------------
// Return a boolean value indicating whether or not mouse wrapping is on
// for a given axis
// ------------------------------------------------------------------------
bool vsWindowSystem::isMouseWrapEnabled(int axis)
{
    // If the threshold is not equal to 0, we're in wrap mode
    if (getMouseWrapThreshold(axis) != 0)
        return true;
    else
        return false;
}

// ------------------------------------------------------------------------
// Set the threshold at which point the mouse wraps (this is the number of
// pixels from the edge of the window)
// ------------------------------------------------------------------------
void vsWindowSystem::setMouseWrapThreshold(int axis, int threshold)
{
    // We can't have negative thresholds
    if (((axis == 0) || (axis == 1)) && (threshold >= 0))
        mouseWrapping[axis] = threshold;
}

// ------------------------------------------------------------------------
// Gets the threshold at which point the mouse wraps (this is the number of
// pixels from the edge of the window)
// ------------------------------------------------------------------------
int vsWindowSystem::getMouseWrapThreshold(int axis)
{
    if ((axis == 0) || (axis == 1))
        return mouseWrapping[axis];
    else
        return 0;
}

// ------------------------------------------------------------------------
// Did the mouse wrap on the last update()?
// ------------------------------------------------------------------------
bool vsWindowSystem::didMouseWrap(int axis)
{
    if ((axis >= 0) && (axis < 2))
        return mouseWrapped[axis];
    else
        return false;
}

// ------------------------------------------------------------------------
// Hide the mouse cursor (if it is not hidden)
// ------------------------------------------------------------------------
void vsWindowSystem::hideCursor()
{
    // Make sure the cursor is not already hidden
    if (mouseCursorHidden)
        return;

    // Hide the cursor
    ShowCursor(FALSE);

    // Mark the cursor as hidden
    mouseCursorHidden = true;
}

// ------------------------------------------------------------------------
// Show the mouse cursor (if it was hidden)
// ------------------------------------------------------------------------
void vsWindowSystem::showCursor()
{
    // If the cursor is hidden...
    if (mouseCursorHidden)
    {
        // Turn the new cursor off in the window
        ShowCursor(TRUE);

        // Mark the cursor as visible
        mouseCursorHidden = false;
    }
}

// ------------------------------------------------------------------------
// Is the mouse cursor currently hidden?
// ------------------------------------------------------------------------
bool vsWindowSystem::isCursorHidden()
{
    return mouseCursorHidden;
}

// ------------------------------------------------------------------------
// Warp (Jump) the mouse to the given location (actually moves the pointer
// in the X11 window)
// ------------------------------------------------------------------------
void vsWindowSystem::warpMouse(int x, int y)
{
    RECT windowRect;
    
    if (window)
    {
        // Tell the vsMouse to move to the given location
        mouse->moveTo(x, y);

        // Get the window's coordinates
        GetWindowRect(window, &windowRect);
        
        // Warp the pointer to the given location, translating the given
        // window coordinates to screen coordinates
        SetCursorPos(x + windowRect.left, y + windowRect.top);
    }
}

// ------------------------------------------------------------------------
// Get the current mouse position in the current window
// ------------------------------------------------------------------------
void vsWindowSystem::getMouseLocation( int *x, int *y )
{
    RECT windowRect;
    int winSizeX, winSizeY, midWinX, midWinY;

    if (GetWindowRect(window, &windowRect) == FALSE)
    {
        // If there's no window, the window size will be 0x0
        winSizeX = 0;
        winSizeY = 0;
    }
    else
    {
        winSizeX = windowRect.right - windowRect.left;
        winSizeY = windowRect.bottom - windowRect.top;
    }
    midWinX  = winSizeX / 2;
    midWinY  = winSizeY / 2;

    // Set the x location of the mouse if the x variable is not null
    if (x != NULL)
    {
        // Get the current mouse positions (in window pixels), rounding the
        // fractional axis position to the nearest pixel
        if (mouse->getAxis(0)->isNormalized())
            *x = (int)floor(mouse->getAxis(0)->getPosition()
                    * midWinX + midWinX + 0.5);
        else
            *x = (int)floor(mouse->getAxis(0)->getPosition() + 0.5);
    }

    // Set the y location of the mouse if the y variable is not null
    if (y != NULL)
    {
        // Get the current mouse positions (in window pixels), rounding the
        // fractional axis position to the nearest pixel
        if(mouse->getAxis(1)->isNormalized())
            *y = (int)floor( mouse->getAxis(1)->getPosition()
                    * midWinY + midWinY + 0.5);
        else
            *y = (int)floor(mouse->getAxis(1)->getPosition() + 0.5);
    }
}

// ------------------------------------------------------------------------
// Update function for window system.  This will route all the input events 
// to the correct devices
// ------------------------------------------------------------------------
void vsWindowSystem::update()
{
    RECT windowRect;
    UINT winX, winY;
    
    // Check to make sure that we have a valid window
    if (!window)
        return;

    // Update the size of the parent window
    if (GetClientRect(window, &windowRect) == FALSE)
    {
        winX = 0;
        winY = 0;
    }
    else
    {
        winX = windowRect.right - windowRect.left;
        winY = windowRect.bottom - windowRect.top;
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
        int midWinX, midWinY;
        int newMouseX, newMouseY;

        // Wrapping pixel boundaries (when the mouse passes between these
        // boundaries and the edge of the window, it will wrap)
        int wrapLeft, wrapTop, wrapRight, wrapBottom;

        int screenWidth, screenHeight;

        // The location of the upper-left corner of the window on the screen
        RECT windowRect;

        // Calculate the window midpoint
        midWinX  = winX / 2;
        midWinY  = winY / 2;

        // Get the current mouse positions (in window pixels)
        getMouseLocation( &mousex, &mousey );

        // Get the size of the screen
        vessWindow->getParentScreen()->getScreenSize(
            &screenWidth, &screenHeight);

        // Get the location of our window on the whole screen
        GetWindowRect(window, &windowRect);

        // Because there's the possibility of part of this window being
        // off-screen, we need to handle the code for mouse wrapping so that
        // the mouse will still wrap. (The mouse can not move offscreen in X11)
        // The following code calculates the pixel positions that mark the
        // edges of the window that are on-screen
        if ((windowRect.left + winX) >= (UINT)screenWidth)
            wrapRight = screenWidth - windowRect.left;
        else
            wrapRight = winX;

        if ((windowRect.top + winY) >= (UINT)screenHeight)
            wrapBottom = screenHeight - windowRect.top;
        else
            wrapBottom = winY;

        if (windowRect.left < 0)
            wrapLeft = -windowRect.left;
        else
            wrapLeft = 0;

        if (windowRect.top < 0)
            wrapTop = -windowRect.top;
        else
            wrapTop = 0;

        // If we're wrapping for the X axis...
        if (mouseWrapping[0])
        {
            if (mousex < (wrapLeft + mouseWrapping[0]))
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
// Window procedure to process keyboard and mouse events.  Subclasses the
// provided vsWindow to create a chain of window procedures.  The window
// procedure provided in the vsWindow class continues to handle the WM_SIZE
// messages.
// ------------------------------------------------------------------------
LRESULT CALLBACK vsWindowSystem::inputWindowProc(HWND msWindow, UINT message,
                                                 WPARAM wParam, LPARAM lParam)
{
    vsWindow *vessWindow;
    vsWindowSystem *windowSys;
    int xPos, yPos;
    
    // Get the VESS window and window system from the msWindow parameter
    vessWindow = (vsWindow *)vsWindow::getMap()->mapFirstToSecond(msWindow);
    windowSys = (vsWindowSystem *)getMap()->mapFirstToSecond(vessWindow);
    
    // Make sure we know about this window.  If not, just let Windows
    // handle the message
    if (vessWindow == NULL)
    {
        return DefWindowProc(msWindow, message, wParam, lParam);
    }

    // Make sure we have a window system.  If not, just let Windows
    // handle the message
    if (windowSys == NULL)
    {
        return DefWindowProc(msWindow, message, wParam, lParam);
    }
    
    switch (message)
    {
        case WM_KEYDOWN:
            // Pass the message to the keyboard.  The vsKeyboard object will
            // handle the keyboard mapping.
            windowSys->keyboard->pressKey((unsigned)wParam, (unsigned)lParam);
            break;
            
        case WM_KEYUP:
            // Pass the message to the keyboard.  The vsKeyboard object will
            // handle the keyboard mapping.
            windowSys->keyboard->releaseKey((unsigned)wParam, (unsigned)lParam);
            break;
            
        case WM_SYSKEYDOWN:
            // Let the ALT+TAB and ALT+ENTER messages have their normal
            // Windows behavior.  Call the default window proc if these
            // keys are received with a SYSKEYDOWN message
            if ((wParam == VK_TAB) || (wParam == VK_RETURN))
            {
                return DefWindowProc(msWindow, message, wParam, lParam);
            }
            else
            {
                // Otherwise, we'll process the key as a normal VESS keystroke
                windowSys->keyboard->pressKey((unsigned)wParam, 
                    (unsigned)lParam);
            }
            break;
            
        case WM_SYSKEYUP:
            // Let the ALT+TAB and ALT+ENTER messages have their normal
            // Windows behavior.  Call the default window proc if these
            // keys are received with a SYSKEYDOWN message
            if ((wParam == VK_TAB) || (wParam == VK_RETURN))
            {
                return DefWindowProc(msWindow, message, wParam, lParam);
            }
            else
            {
                // Otherwise, we'll process the key as a normal VESS keystroke
                windowSys->keyboard->releaseKey((unsigned)wParam, 
                    (unsigned)lParam);
            }
            break;
            
        case WM_MOUSEMOVE:
            // Get the mouse position from the lParam parameter
            xPos = GET_X_LPARAM(lParam);
            yPos = GET_Y_LPARAM(lParam);
            
            // Move the vsMouse to the new coordinates
            windowSys->mouse->moveTo(xPos, yPos);
            
            // Set the flag indicating the mouse is in the window
            windowSys->mouseInWindow = true;
            break;
            
        case WM_LBUTTONDOWN:
            // Capture the mouse to better emulate X Windows behavior.
            // This keeps mouse events coming to this window, even if the
            // pointer leaves the window.  It does not constrain the pointer 
            // in any way.
            SetCapture(msWindow);
            
            // Press the left mouse button
            windowSys->mouse->getButton(0)->setPressed();
            break;
            
        case WM_LBUTTONUP:
            // Release the captured mouse
            ReleaseCapture();
            
            // Release the left mouse button
            windowSys->mouse->getButton(0)->setReleased();
            break;
            
        case WM_MBUTTONDOWN:
            // Capture the mouse to better emulate X Windows behavior.
            // This keeps mouse events coming to this window, even if the
            // pointer leaves the window.  It does not constrain the pointer 
            // in any way.
            SetCapture(msWindow);
            
            // Press the middle mouse button
            windowSys->mouse->getButton(1)->setPressed();
            break;
        
        case WM_MBUTTONUP:
            // Release the captured mouse
            ReleaseCapture();
            
            // Release the middle mouse button
            windowSys->mouse->getButton(1)->setReleased();
            break;
            
        case WM_RBUTTONDOWN:
            // Capture the mouse to better emulate X Windows behavior.
            // This keeps mouse events coming to this window, even if the
            // pointer leaves the window.  It does not constrain the pointer 
            // in any way.
            SetCapture(msWindow);
            
            // Press the right mouse button
            windowSys->mouse->getButton(2)->setPressed();
            break;
            
        case WM_RBUTTONUP:
            // Release the captured mouse
            ReleaseCapture();
            
            // Release the right mouse button
            windowSys->mouse->getButton(2)->setReleased();
            break;
            
        default:
            // Not an input message, call the original window procedure (set 
            // by the vsWindow class)
            return CallWindowProc(windowSys->originalWindowProc, msWindow, 
                message, wParam, lParam);
    }
    
    return 0;
}                                
