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
//    VESS Module:  vsWindow.c++
//
//    Description:  Class that represents an open window on any screen
//
//    Author(s):    Bryan Kline, Jason Daly
//
//------------------------------------------------------------------------

#include <stdio.h>
#include <osg/Image>
#include <osgDB/WriteFile>
#include <string>
#include "vsWindow.h++"
#include "resource.h"

// Static class member that maintains a mapping between vsWindow objects
// and their respective Microsoft Windows window instances
vsObjectMap *vsWindow::windowMap = NULL;

// Static class member that maintains a count of the number of windows
// created.  Used to assign a unique index to each window
int vsWindow::windowCount = 0;

// ------------------------------------------------------------------------
// Constructor - Initializes the window by creating a GLX window and 
// creating connections with that, verifying that the window is being 
// properly displayed, recording some size data from the window manager, 
// and configuring the window with its default position and size.  Also 
// configures the window's buffer settings to be either mono or stereo
// based on the value of the stereo parameter
// ------------------------------------------------------------------------
vsWindow::vsWindow(vsScreen *parent, bool hideBorder, bool stereo) 
         : childPaneList(1, 1)
{
    vsPipe *parentPipe;
    DWORD windowStyle;
    PIXELFORMATDESCRIPTOR pixelFormatDesc, stereoPFD;
    int pixelFormat;
    
    // Initialize the pane count
    childPaneCount = 0;

    // Assign this window an index and increment the window count.
    // Note: this procedure may need to be protected for thread-safeness 
    // if OSG becomes multi-threaded.
    windowNumber = windowCount++;
    
    // Get the parent vsScreen and vsPipe
    parentScreen = parent;
    parentPipe = parentScreen->getParentPipe();

	// Set up the window class.  First pick a name for it, use the
	// window number to keep it unique.
	sprintf(windowClassName, "VS_WINDOW_CLASS_%d", windowNumber);
	
	// Size in memory
	windowClass.cbSize = sizeof(WNDCLASSEX); 
	
    // Make sure each window gets its own device context
	windowClass.style = CS_OWNDC;
	
	// Main window procedure (message handler function)
	windowClass.lpfnWndProc = (WNDPROC)mainWindowProc;
	
	// No extra per-class or per-window memory
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	
	// Handle to application instance
	windowClass.hInstance = GetModuleHandle(NULL);
	
	// Large icon (use the standard application icon)
	windowClass.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_VESSV));
	
	// Application cursor (use the standard arrow cursor)
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	
	// Background color brush
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	
	// No menu bar
	windowClass.lpszMenuName = NULL;
	
	// A name for this window class
	windowClass.lpszClassName = windowClassName;
	
	// Small icon (use the standard application icon again)
	windowClass.hIconSm = LoadIcon(NULL, MAKEINTRESOURCE(IDI_VESSV));
	
	// Try to register the window class.  Print an error message and
	// bail if this fails.
	if (RegisterClassEx(&windowClass) == 0)
	{
	    printf("vsWindow::vsWindow:  Unable to register window class\n");
	    return;
	}
	
	// Set up the window style.  The following style flags are recommended
	// in the docs for OpenGL windows.
    windowStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    
    // Add the "overlapped" style if we don't want to hide the window border
    if (!hideBorder)
	    windowStyle |= WS_OVERLAPPEDWINDOW;
	
	// Now, try to open the window
	msWindow = CreateWindow(windowClassName, "VESS Window", windowStyle,
	    CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL,
	    GetModuleHandle(NULL), NULL);
	    
	// Set the oldWindowProc member to NULL, since we aren't subclassing
	// this window
	oldWindowProc = NULL;
	    
	// Print an error and bail out if the window doesn't open
	if (msWindow == NULL)
	{
	    printf("vsWindow::vsWindow:  Unable to open window\n");
	    return;
	}

    // Describe a pixel format containing our default attributes
    // as possible (32-bit color, 24-bit z-buffer, stencil buffer, double-
    // buffered)
  	memset(&pixelFormat, 0, sizeof(pixelFormat));
    pixelFormatDesc.nSize = sizeof(pixelFormat);
    pixelFormatDesc.nVersion = 1;
    pixelFormatDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
                              PFD_DOUBLEBUFFER | PFD_STEREO;
    pixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
    pixelFormatDesc.cColorBits = 32;
    pixelFormatDesc.cDepthBits = 24;
    pixelFormatDesc.cStencilBits = 1;
    pixelFormatDesc.iLayerType = PFD_MAIN_PLANE;
    
    // Get the device context from the window
    deviceContext = GetDC(msWindow);

    // Search the window's device context for the pixel format that most
    // closely matches the format we described
    pixelFormat = ChoosePixelFormat(deviceContext, &pixelFormatDesc);
    
    // See if we succeeded in getting a stereo pixel format.  If not,
    // continue normally but inform the user of the potential problem.
    DescribePixelFormat(deviceContext, pixelFormat, 
        sizeof(PIXELFORMATDESCRIPTOR), &stereoPFD);
    if ((stereoPFD.dwFlags & PFD_STEREO) == 0)
    {
        printf("vsWindow::vsWindow:  WARNING -- Unable to obtain a stereo "
            "pixel format!\n");
    }
    
    // Set the window's pixel format
    SetPixelFormat(deviceContext, pixelFormat, &pixelFormatDesc);
    
    // Create an OpenGL context for the window
    glContext = wglCreateContext(deviceContext);

    // If the context is not valid, bail out.
    if (glContext == NULL)
    {
        printf("vsWindow::vsWindow:  Unable to create OpenGL context\n");
        return;
    }
    
    // Display and update the window
    ShowWindow(msWindow, SW_SHOW);
    UpdateWindow(msWindow);

    // Add the window to its parent screen
    parentScreen->addWindow(this);
    
    // Register a mapping between this vsWindow and its MS window handle
    getMap()->registerLink(msWindow, this);
}

// ------------------------------------------------------------------------
// Constructor - Initializes the window by creating a GLX window and 
// creating connections with that, verifying that the window is being 
// properly displayed, recording some size data from the window manager, 
// and configuring the window with its default position and size.  Also 
// configures the window's buffer settings to be either mono or stereo
// based on the value of the stereo parameter
// ------------------------------------------------------------------------
vsWindow::vsWindow(vsScreen *parent, int x, int y, int width, int height, 
                   bool hideBorder, bool stereo) : childPaneList(1, 1)
{
    vsPipe *parentPipe;
    DWORD windowStyle;
    PIXELFORMATDESCRIPTOR pixelFormatDesc, stereoPFD;
    int pixelFormat;
    
    // Initialize the pane count
    childPaneCount = 0;

    // Assign this window an index and increment the window count.
    // Note: this procedure may need to be protected for thread-safeness 
    // if OSG becomes multi-threaded.
    windowNumber = windowCount++;
    
    // Get the parent vsScreen and vsPipe
    parentScreen = parent;
    parentPipe = parentScreen->getParentPipe();

	// Set up the window class.  First pick a name for it, use the
	// window number to keep it unique.
	sprintf(windowClassName, "VS_WINDOW_CLASS_%d", windowNumber);
	
	// Size in memory
	windowClass.cbSize = sizeof(WNDCLASSEX); 
	
    // Make sure each window gets its own device context
	windowClass.style = CS_OWNDC;
	
	// Main window procedure (message handler function)
	windowClass.lpfnWndProc = (WNDPROC)mainWindowProc;
	
	// No extra per-class or per-window memory
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	
	// Handle to application instance
	windowClass.hInstance = GetModuleHandle(NULL);
	
	// Large icon (use the standard application icon)
	windowClass.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_VESSV));
	
	// Application cursor (use the standard arrow cursor)
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	
	// Background color brush
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	
	// No menu bar
	windowClass.lpszMenuName = NULL;
	
	// A name for this window class
	windowClass.lpszClassName = windowClassName;
	
	// Small icon (use the standard application icon again)
	windowClass.hIconSm = LoadIcon(NULL, MAKEINTRESOURCE(IDI_VESSV));
	
	// Try to register the window class.  Print an error message and
	// bail if this fails.
	if (RegisterClassEx(&windowClass) == 0)
	{
	    printf("vsWindow::vsWindow:  Unable to register window class\n");
	    return;
	}
	
	// Set up the window style.  The following style flags are recommended
	// in the docs for OpenGL windows.
    windowStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    
    // Add the "overlapped" style if we don't want to hide the window border
    if (!hideBorder)
	    windowStyle |= WS_OVERLAPPEDWINDOW;
	
	// Now, try to open the window
	msWindow = CreateWindow(windowClassName, "VESS Window", windowStyle,
	    x, y, width, height, NULL, NULL,
	    GetModuleHandle(NULL), NULL);
	    
	// Set the oldWindowProc member to NULL, since we aren't subclassing
	// this window
	oldWindowProc = NULL;
	    
	// Print an error and bail out if the window doesn't open
	if (msWindow == NULL)
	{
	    printf("vsWindow::vsWindow:  Unable to open window\n");
	    return;
	}

    // Describe a pixel format containing our default attributes
    // as possible (32-bit color, 24-bit z-buffer, stencil buffer, double-
    // buffered)
  	memset(&pixelFormat, 0, sizeof(pixelFormat));
    pixelFormatDesc.nSize = sizeof(pixelFormat);
    pixelFormatDesc.nVersion = 1;
    pixelFormatDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
                              PFD_DOUBLEBUFFER | PFD_STEREO;
    pixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
    pixelFormatDesc.cColorBits = 32;
    pixelFormatDesc.cDepthBits = 24;
    pixelFormatDesc.cStencilBits = 1;
    pixelFormatDesc.iLayerType = PFD_MAIN_PLANE;
    
    // Get the device context from the window
    deviceContext = GetDC(msWindow);

    // Search the window's device context for the pixel format that most
    // closely matches the format we described
    pixelFormat = ChoosePixelFormat(deviceContext, &pixelFormatDesc);
    
    // See if we succeeded in getting a stereo pixel format.  If not,
    // continue normally but inform the user of the potential problem.
    DescribePixelFormat(deviceContext, pixelFormat, 
        sizeof(PIXELFORMATDESCRIPTOR), &stereoPFD);
    if ((stereoPFD.dwFlags & PFD_STEREO) == 0)
    {
        printf("vsWindow::vsWindow:  WARNING -- Unable to obtain a stereo "
            "pixel format!\n");
    }
    
    // Set the window's pixel format
    SetPixelFormat(deviceContext, pixelFormat, &pixelFormatDesc);
    
    // Create an OpenGL context for the window
    glContext = wglCreateContext(deviceContext);

    // If the context is not valid, bail out.
    if (glContext == NULL)
    {
        printf("vsWindow::vsWindow:  Unable to create OpenGL context\n");
        return;
    }
    
    // Display and update the window
    ShowWindow(msWindow, SW_SHOW);
    UpdateWindow(msWindow);

    // Add the window to its parent screen
    parentScreen->addWindow(this);
    
    // Register a mapping between this vsWindow and its MS window handle
    getMap()->registerLink(msWindow, this);
}

// ------------------------------------------------------------------------
// Constructor - Initializes the window by making use of the existing MS
// Window passed in.  This requires "subclassing" the window class of the
// given window and installing a second main window procedure.
// ------------------------------------------------------------------------
vsWindow::vsWindow(vsScreen *parent, HWND msWin) : childPaneList(1, 1)
{
    vsPipe *parentPipe;
    PIXELFORMATDESCRIPTOR pixelFormatDesc, stereoPFD;
    int pixelFormat;
    
    // Initialize the pane count
    childPaneCount = 0;

    // Assign this window an index and increment the window count.
    // Note: this procedure may need to be protected for thread-safeness 
    // if OSG becomes multi-threaded.
    windowNumber = windowCount++;
    
    // Get the parent vsScreen and vsPipe
    parentScreen = parent;
    parentPipe = parentScreen->getParentPipe();
    
    // Subclass the window and install our own window procedure.  In this
    // case, we use the subclassedWindowProc method, which makes a call
    // to the window's original window procedure when we're done.  This 
    // way, the behavior of the window is preserved as much as possible.
    oldWindowProc = (WNDPROC)SetWindowLongPtr(msWindow, GWL_WNDPROC, 
        (LONG_PTR)subclassedWindowProc);
        
    // Describe a pixel format containing our default attributes
    // as possible (32-bit color, 24-bit z-buffer, stencil buffer, double-
    // buffered)
  	memset(&pixelFormat, 0, sizeof(pixelFormat));
    pixelFormatDesc.nSize = sizeof(pixelFormat);
    pixelFormatDesc.nVersion = 1;
    pixelFormatDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
                              PFD_DOUBLEBUFFER | PFD_STEREO;
    pixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
    pixelFormatDesc.cColorBits = 32;
    pixelFormatDesc.cDepthBits = 24;
    pixelFormatDesc.cStencilBits = 1;
    pixelFormatDesc.iLayerType = PFD_MAIN_PLANE;
    
    // Get the device context from the window
    deviceContext = GetDC(msWindow);

    // Search the window's device context for the pixel format that most
    // closely matches the format we described
    pixelFormat = ChoosePixelFormat(deviceContext, &pixelFormatDesc);
    
    // See if we succeeded in getting a stereo pixel format.  If not,
    // continue normally but inform the user of the potential problem.
    DescribePixelFormat(deviceContext, pixelFormat, 
        sizeof(PIXELFORMATDESCRIPTOR), &stereoPFD);
    if ((stereoPFD.dwFlags & PFD_STEREO) == 0)
    {
        printf("vsWindow::vsWindow:  WARNING -- Unable to obtain a stereo "
            "pixel format!\n");
    }
    
    // Set the window's pixel format
    SetPixelFormat(deviceContext, pixelFormat, &pixelFormatDesc);
    
    // Create an OpenGL context for the window
    glContext = wglCreateContext(deviceContext);

    // If the context is not valid, bail out.
    if (glContext == NULL)
    {
        printf("vsWindow::vsWindow:  Unable to create OpenGL context\n");
        return;
    }
    
    // Display and update the window
    ShowWindow(msWindow, SW_SHOW);
    UpdateWindow(msWindow);

    // Add the window to its parent screen
    parentScreen->addWindow(this);
    
    // Register a mapping between this vsWindow and its MS window handle
    getMap()->registerLink(msWindow, this);
}

// ------------------------------------------------------------------------
// Destructor - Deletes any child panes that this window owns
// ------------------------------------------------------------------------
vsWindow::~vsWindow()
{
    // Delete all child panes
    // The vsPane destructor includes a call to the parent vsWindow (this)
    // to remove it from the pane list. Keep deleting vsPanes and eventually
    // the list will go away by itself.
    while (childPaneCount > 0)
        delete ((vsPane *)(childPaneList[0]));
    
    // Remove the window from its screen
    parentScreen->removeWindow(this);
    
    // Remove the window mapping
    if (getMap()->mapSecondToFirst(this))
        getMap()->removeLink(this, VS_OBJMAP_SECOND_LIST);

    // Destroy the window
    wglDeleteContext(glContext);
    DestroyWindow(msWindow);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsWindow::getClassName()
{
    return "vsWindow";
}

// ------------------------------------------------------------------------
// Retrieves the parent screen of this window
// ------------------------------------------------------------------------
vsScreen *vsWindow::getParentScreen()
{
    return parentScreen;
}

// ------------------------------------------------------------------------
// Returns the number of child panes that this window owns
// ------------------------------------------------------------------------
int vsWindow::getChildPaneCount()
{
    return childPaneCount;
}

// ------------------------------------------------------------------------
// Retrieves the child pane of this window at the given index. The index
// of the first child pane is 0.
// ------------------------------------------------------------------------
vsPane *vsWindow::getChildPane(int index)
{
    // Make sure the index is valid
    if ((index < 0) || (index >= childPaneCount))
    {
        printf("vsWindow::getChildPane: Index out of bounds\n");
        return NULL;
    }

    // Return the requested pane
    return (vsPane *)(childPaneList[index]);
}

// ------------------------------------------------------------------------
// Sets the size of this window in pixels
// ------------------------------------------------------------------------
void vsWindow::setSize(int width, int height)
{
    // Call the SetWindowPos function, telling it to ignore the position 
    // and z-order fields.  Also instruct the function not to activate the
    // window (don't give it focus or change the stacking order).
    SetWindowPos(msWindow, NULL, 0, 0, width, height,
        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

// ------------------------------------------------------------------------
// Retrieves the size of this window in pixels. NULL pointers may be passed
// in for undesired data values.
// ------------------------------------------------------------------------
void vsWindow::getSize(int *width, int *height)
{
    RECT windowRect;
    
    // Get the dimensions of the window
    GetWindowRect(msWindow, &windowRect);
    
    // Return the width if requested
    if (width)
        *width = windowRect.right - windowRect.left;

    // Return the height if requested
    if (height)
        *height = windowRect.bottom - windowRect.top;
}

// ------------------------------------------------------------------------
// Sets the position of this window on the screen, in pixels from the
// top-left corner of the screen.
// ------------------------------------------------------------------------
void vsWindow::setPosition(int xPos, int yPos)
{
    // Call the SetWindowPos function, telling it to ignore the size and 
    // z-order fields.  Also instruct the function not to activate the
    // window (don't give it focus or change the stacking order).
    SetWindowPos(msWindow, NULL, xPos, yPos, 0, 0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

// ------------------------------------------------------------------------
// Retrieves the position of the window on the screen, in pixels from the
// top-left cornder of the screen. NULL pointers may be passed in for
// undesired data values.
// ------------------------------------------------------------------------
void vsWindow::getPosition(int *xPos, int *yPos)
{
    RECT windowRect;
    
    // Get the dimensions of the window
    GetWindowRect(msWindow, &windowRect);
    
    // Return the x-position if requested
    if (xPos)
        *xPos = windowRect.left;

    // Return the y-position if requested
    if (yPos)
        *yPos = windowRect.top;
}

// ------------------------------------------------------------------------
// Sets the position and size of the window to fill the entire screen
// ------------------------------------------------------------------------
void vsWindow::setFullScreen()
{
    int screenWidth, screenHeight;

    // Get the size of the screen
    parentScreen->getScreenSize(&screenWidth, &screenHeight);
    
    // Set the window's origin to the screen's origin
    setPosition(0, 0);

    // Set the window's size to fill the screen
    setSize(screenWidth, screenHeight);
}

// ------------------------------------------------------------------------
// Sets the name of the window. The window's name is usually displayed on
// its title bar.
// ------------------------------------------------------------------------
void vsWindow::setName(char *newName)
{
    SetWindowText(msWindow, newName);
}


// ------------------------------------------------------------------------
// Saves a copy of the image currently displayed in the window to the given
// file (in RGB format).
// ------------------------------------------------------------------------
void vsWindow::saveImage(char *filename)
{
    osg::Image *osgImage;
    std::string *imageFileName;
    int width, height;

    // Make sure the window's OpenGL context is the current context
    makeCurrent();
    
    // Get the current size of the window and apply the border offsets
    getSize(&width, &height);
    
    // Construct the Image object
    osgImage = new osg::Image();

    // Read the image from the frame buffer
    osgImage->readPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE);

    // Construct an STL string for the filename
    imageFileName = new std::string(filename);

    // Try to write the image to a file, print an error if there's a problem
    if (!osgDB::writeImageFile(*osgImage, *imageFileName))
        printf("vsWindow::saveImage:  Write failed\n");
}

// ------------------------------------------------------------------------
// Returns the Windows object associated with this object
// ------------------------------------------------------------------------
HWND vsWindow::getBaseLibraryObject()
{
    return msWindow;
}

// ------------------------------------------------------------------------
// Internal function
// Adds the given pane to the window's list of child panes
// ------------------------------------------------------------------------
void vsWindow::addPane(vsPane *newPane)
{
    // Add pane to window's internal list
    childPaneList[childPaneCount++] = newPane;

    // Reference the pane
    newPane->ref();
}

// ------------------------------------------------------------------------
// Internal function
// Removes the given pane from the window's list of child panes
// ------------------------------------------------------------------------
void vsWindow::removePane(vsPane *targetPane)
{
    // Remove pane from window's internal list
    int loop, sloop;
    
    // Iterate through the child pane list and look for the pane in 
    // question
    for (loop = 0; loop < childPaneCount; loop++)
    {
        // See if the current pane is the pane we want
        if (targetPane == childPaneList[loop])
        {
            // Found the target pane, slide the remaining panes down
            // in the list
            for (sloop = loop; sloop < (childPaneCount-1); sloop++)
                childPaneList[sloop] = childPaneList[sloop+1];

            // Decrement the pane count
            childPaneCount--;

            // Unreference the pane
            targetPane->unref();

            // We're done
            return;
        }
    }

    // If we get here, we didn't find the requested pane, so print an
    // error
    printf("vsWindow::removePane: Specified pane not part of window\n");
}

// ------------------------------------------------------------------------
// Internal function
// Return the index of this window
// ------------------------------------------------------------------------
int vsWindow::getWindowNumber()
{
    return windowNumber;
}

// ------------------------------------------------------------------------
// Internal function
// Makes the OpenGL context associated with this window the current
// context
// ------------------------------------------------------------------------
void vsWindow::makeCurrent()
{
    BOOL result;

    // Try to make this window's GLX context current
    result = wglMakeCurrent(deviceContext, glContext);

    // Print an error if the makeCurrent failed
    if (!result)
    {
        printf("vsWindow::makeCurrent:  Unable to attach OpenGL context to "
            "window!\n");
    }
}

// ------------------------------------------------------------------------
// Internal function
// Swaps drawing buffers on this Window
// ------------------------------------------------------------------------
void vsWindow::swapBuffers()
{
    BOOL result;
    
    // Try to swap the buffers on the MS window's GDI context
    result = SwapBuffers(deviceContext);
    
    // Print an error if the swapBuffers failed
    if (!result)
    {
        printf("vsWindow::swapBuffers:  Unable to swap buffers on the "
            "window!\n");
    }
}

// ------------------------------------------------------------------------
// Static internal function
// Return the window object map
// ------------------------------------------------------------------------
vsObjectMap *vsWindow::getMap()
{
    if (!windowMap)
        windowMap = new vsObjectMap();

    return windowMap;
}

// ------------------------------------------------------------------------
// Static internal function
// Deletes the object map that holds the window mappings, if it exists
// ------------------------------------------------------------------------
void vsWindow::deleteMap()
{
    if (windowMap)
    {
        delete windowMap;
        windowMap = NULL;
    }
}
    
// ------------------------------------------------------------------------
// Static callback function to handle MS-Windows events (this is the main
// window procedure).   This function only handles window resizing.
// ------------------------------------------------------------------------
LRESULT CALLBACK vsWindow::mainWindowProc(HWND msWindow, UINT message,
                                          WPARAM wParam, LPARAM lParam)
{
    int i;
    int width, height;
    vsWindow *window;
    
    // Get the vsWindow corresponding to the given HWND
    window = (vsWindow *)(getMap()->mapFirstToSecond(msWindow));
    
    // Make sure we know about this window.  If not, just let Windows
    // handle the message
    if (window == NULL)
    {
        return DefWindowProc(msWindow, message, wParam, lParam);
    }

    switch (message) 
    {
        case WM_SIZE:
            // Get the width and the height of the window
            width = LOWORD(lParam);
            height = HIWORD(lParam);
            
            // Resize each pane to match
            for (i = 0; i < window->childPaneCount; i++)
                ((vsPane *)(window->childPaneList[i]))->resize();
            break;
            
	    default:
	        // Call the Windows default window procedure for this message
            return DefWindowProc(msWindow, message, wParam, lParam);
	}
	return 0;
}

// ------------------------------------------------------------------------
// Static callback function to handle MS-Windows events (this is the main
// window procedure).   This function only handles window resizing.  This
// version makes a call to the additional window procedure stored in the
// oldWindowProc data member.
// ------------------------------------------------------------------------
LRESULT CALLBACK vsWindow::subclassedWindowProc(HWND msWindow, UINT message,
                                                WPARAM wParam, LPARAM lParam)
{
    int i;
    int width, height;
    vsWindow *window;
    LRESULT result;
    
    // Get the vsWindow corresponding to the given HWND
    window = (vsWindow *)(getMap()->mapFirstToSecond(msWindow));

    // Make sure we know about this window.  If not, just let Windows
    // handle the message
    if (window == NULL)
    {
        return DefWindowProc(msWindow, message, wParam, lParam);
    }

    switch (message) 
    {
        case WM_SIZE:
            // Get the width and the height of the window
            width = LOWORD(lParam);
            height = HIWORD(lParam);
            
            // Resize each pane to match
            for (i = 0; i < window->childPaneCount; i++)
                ((vsPane *)(window->childPaneList[i]))->resize();
            break;
	}
	
	// Call the previous window procedure (whether we handle a message or
	// not)
	result = CallWindowProc(window->oldWindowProc, msWindow, message, wParam, 
	    lParam);
	
	return result;
}

// ------------------------------------------------------------------------
// Internal function
// Update function to handle window system events.  Under Windows, events
// are handled through a callback mechanism, so this function does nothing.
// ------------------------------------------------------------------------
void vsWindow::update()
{
}
