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
//    Author(s):    Bryan Kline, Jason Daly, Casey Thurston
//
//------------------------------------------------------------------------

#include "vsWindow.h++"
#include <stdio.h>
#include <osg/Image>
#include <osgDB/WriteFile>
#include <string>

// Static class member that maintains a mapping between vsWindow objects
// and their respective Microsoft Windows window instances
vsObjectMap *vsWindow::windowMap = NULL;

// Static class member that maintains a count of the number of windows
// created.  Used to assign a unique index to each window
int vsWindow::windowCount = 0;

// WGL extensions function pointers.  These are looked up the first time
// the extension is used.
PFNWGLGETEXTENSIONSSTRINGARBPROC vsWindow::wglGetExtensionsStringARB = NULL;
PFNWGLCHOOSEPIXELFORMATARBPROC vsWindow::wglChoosePixelFormatARB = NULL;
PFNWGLRELEASEPBUFFERDCARBPROC vsWindow::wglReleasePbufferDCARB = NULL;
PFNWGLDESTROYPBUFFERARBPROC vsWindow::wglDestroyPbufferARB = NULL;
PFNWGLCREATEPBUFFERARBPROC vsWindow::wglCreatePbufferARB = NULL;
PFNWGLGETPBUFFERDCARBPROC vsWindow::wglGetPbufferDCARB = NULL;
PFNWGLQUERYPBUFFERARBPROC vsWindow::wglQueryPbufferARB = NULL;

// ------------------------------------------------------------------------
// Constructor - Initializes the window by creating a WGL window and 
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

    // Initialize the pane count
    childPaneCount = 0;

    // Flag that this window is not an offscreen window
    isOffScreenWindow = false;

    // Flag that we're creating the MS Window in this case (so we should
    // destroy it in the destructor)
    createdMSWindow = true;

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
    windowClass.hIcon = NULL;

    // Application cursor (use the standard arrow cursor)
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);

    // Background color brush
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);

    // No menu bar
    windowClass.lpszMenuName = NULL;

    // A name for this window class
    windowClass.lpszClassName = windowClassName;

    // Small icon (use the standard application icon again)
    windowClass.hIconSm = NULL;

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

    // The hideBorder parameter determines which kind of window we're going
    // to use.  If the border is to be hidden, we use a popup window.  If
    // not, we use an overlapped window.
    if (hideBorder)
        windowStyle |= WS_POPUP;
    else
        windowStyle |= WS_OVERLAPPEDWINDOW;

    // Now, try to open the window
    msWindow = CreateWindow(windowClassName, "VESS Window", windowStyle,
        VS_WINDOW_DEFAULT_XPOS, VS_WINDOW_DEFAULT_YPOS, 
        VS_WINDOW_DEFAULT_WIDTH, VS_WINDOW_DEFAULT_HEIGHT, NULL, NULL,
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
        PFD_DOUBLEBUFFER;
    pixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
    pixelFormatDesc.cColorBits = 32;
    pixelFormatDesc.cDepthBits = 24;
    pixelFormatDesc.cStencilBits = 1;
    pixelFormatDesc.iLayerType = PFD_MAIN_PLANE;

    // Add the stereo flag to the pixel format if requested
    if (stereo)
    {
        pixelFormatDesc.dwFlags |= PFD_STEREO;
    }
 
    // Get the device context from the window
    deviceContext = GetDC(msWindow);

    // Search the window's device context for the pixel format that most
    // closely matches the format we described
    pixelFormat = ChoosePixelFormat(deviceContext, &pixelFormatDesc);
    
    // If stereo was requested, see if we succeeded in getting a stereo pixel 
    // format.  If not, continue normally but inform the user of the potential 
    //problem.
    if (stereo)
    {
        DescribePixelFormat(deviceContext, pixelFormat, 
            sizeof(PIXELFORMATDESCRIPTOR), &stereoPFD);
        if ((stereoPFD.dwFlags & PFD_STEREO) == 0)
        {
            printf("vsWindow::vsWindow:  WARNING -- Unable to obtain a stereo "
                "pixel format!\n");
        }
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

    // Indicate that the window is not for off-screen rendering
    isOffScreenWindow = false;
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
    
    // Initialize the pane count
    childPaneCount = 0;

    // Flag that this window is an offscreen window
    isOffScreenWindow = false;

    // Flag that we're creating the MS Window in this case (so we should
    // destroy it in the destructor)
    createdMSWindow = true;

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
    windowClass.hIcon = NULL;

    // Application cursor (use the standard arrow cursor)
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);

    // Background color brush
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);

    // No menu bar
    windowClass.lpszMenuName = NULL;

    // A name for this window class
    windowClass.lpszClassName = windowClassName;

    // Small icon (use the standard application icon again)
    windowClass.hIconSm = NULL;

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

    // The hideBorder parameter determines which kind of window we're going
    // to use.  If the border is to be hidden, we use a popup window.  If
    // not, we use an overlapped window.
    if (hideBorder)
        windowStyle |= WS_POPUP;
    else
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
        PFD_DOUBLEBUFFER;
    pixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
    pixelFormatDesc.cColorBits = 32;
    pixelFormatDesc.cDepthBits = 24;
    pixelFormatDesc.cStencilBits = 1;
    pixelFormatDesc.iLayerType = PFD_MAIN_PLANE;

    // If stereo was requested, add the stereo pixel format flag
    if (stereo)
        pixelFormatDesc.dwFlags |= PFD_STEREO;

    // Get the device context from the window
    deviceContext = GetDC(msWindow);

    // Search the window's device context for the pixel format that most
    // closely matches the format we described
    pixelFormat = ChoosePixelFormat(deviceContext, &pixelFormatDesc);
    
    // If stereo was requested, see if we succeeded in getting a stereo pixel
    // format.  If not, continue normally but inform the user of the potential
    // problem.
    if (stereo)
    {
        DescribePixelFormat(deviceContext, pixelFormat, 
            sizeof(PIXELFORMATDESCRIPTOR), &stereoPFD);
        if ((stereoPFD.dwFlags & PFD_STEREO) == 0)
        {
            printf("vsWindow::vsWindow:  WARNING -- Unable to obtain a stereo "
                "pixel format!\n");
        }
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

    // Indicate that the window is not for off-screen rendering
    isOffScreenWindow = false;
}

// ------------------------------------------------------------------------
// Constructor - Initializes the window by creating a GLX window and
// creating connections with that, verifying that the window is being
// properly displayed, recording some size data from the window manager,
// and configuring the window with its default position and size.  This
// constructor creates a window for off-screen rendering with a PBuffer.
// ------------------------------------------------------------------------
vsWindow::vsWindow(vsScreen *parent, int offScreenWidth, int offScreenHeight)
         : childPaneList(1, 1)
{
    vsPipe *parentPipe;
    UINT numFormats;
    char *wglExtensions, *token;
    bool wglPbufferFlag, wglPixelFormatFlag;
    
    int iAttributeList[] = {
        WGL_DRAW_TO_PBUFFER_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_COLOR_BITS_ARB, 32,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        0, 0};

    float fAttributeList[] = {0, 0};

    int bufferAttribList[] = {
        WGL_PBUFFER_LARGEST_ARB, GL_FALSE,
        0, 0};
    
    // Store the values for the width and height
    drawableWidth = offScreenWidth;
    drawableHeight = offScreenHeight;

    // Initialize the pane count
    childPaneCount = 0;

    // Flag that this window is an offscreen window
    isOffScreenWindow = true;

    // Flag that we're not creating an MS Window in this case (we create a
    // pbuffer instead) 
    createdMSWindow = true;

    // Assign this window an index and increment the window count.
    // Note: this procedure may need to be protected for thread-safeness
    // if OSG becomes multi-threaded.
    windowNumber = windowCount++;

    // Get the parent vsScreen and vsPipe
    parentScreen = parent;
    parentPipe = parentScreen->getParentPipe();

    // The off-screen window has no visible component
    msWindow = NULL;

    // Set the oldWindowProc member to NULL, since we aren't subclassing
    // this window
    oldWindowProc = NULL;

    // Get the current valid device context from the system
    deviceContext = wglGetCurrentDC();
    
    if (wglGetExtensionsStringARB == NULL)
    {
        // This function pointer must be fetched to determine which WGL
        // extensions are installed on this machine
        wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)
            wglGetProcAddress("wglGetExtensionsStringARB");

        // If the extensions are not installed, bail out
        if(wglGetExtensionsStringARB == NULL)
        {
            printf("vsWindow::vsWindow:  WGL Extensions not detected. "
                   "Cannot instantiate off-screen window\n");
            return;
        }

        // Get the extensions string
        wglExtensions = (char *)wglGetExtensionsStringARB(deviceContext);

        // Initialize the flags indicating the presence of the two WGL
        // extensions to false, since they have not yet been found
        wglPbufferFlag = false;
        wglPixelFormatFlag = false;
    
        // Start a tokenizer on the extensions string
        token = strtok(wglExtensions, " \n");
    
        // If the token matches either extensions string, set the flag to true
        if(strcmp(token, "WGL_ARB_pbuffer") == 0)
            wglPbufferFlag = true;
        else if(strcmp(token, "WGL_ARB_pixel_format") == 0)
            wglPixelFormatFlag = true;
        
        // Scan the tokenizer for the strings, setting the flags appropriately
        while(token = strtok(NULL, " \n"))
        {
            if(strcmp(token, "WGL_ARB_pbuffer") == 0)
                wglPbufferFlag = true;
            else if(strcmp(token, "WGL_ARB_pixel_format") == 0)
                wglPixelFormatFlag = true;
        }
    
        // If either the pbuffer or pixel format extensions are missing, the 
        // window cannot be instantiated
        if((!wglPbufferFlag) || (!wglPixelFormatFlag))
        {
            printf("vsWindow::vsWindow:  WGL extensions not installed!\n");
            return;
        }
    
        // Set the WGL function pointers
        wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)
            wglGetProcAddress("wglChoosePixelFormatARB");
        wglReleasePbufferDCARB = (PFNWGLRELEASEPBUFFERDCARBPROC)
            wglGetProcAddress("wglReleasePbufferDCARB");        
        wglDestroyPbufferARB = (PFNWGLDESTROYPBUFFERARBPROC)
            wglGetProcAddress("wglDestroyPbufferARB");
        wglCreatePbufferARB = (PFNWGLCREATEPBUFFERARBPROC)
            wglGetProcAddress("wglCreatePbufferARB");
        wglGetPbufferDCARB = (PFNWGLGETPBUFFERDCARBPROC)
            wglGetProcAddress("wglGetPbufferDCARB");
        wglQueryPbufferARB = (PFNWGLQUERYPBUFFERARBPROC)
            wglGetProcAddress("wglQueryPbufferARB");
    }
    
    // Make sure we got the functions we need for pixel formats
    if (wglChoosePixelFormatARB == NULL)
    {
        printf("vsWindow::vsWindow:  WGL pixel format extensions not "
            "installed!\n");
        return;
    }
    
    // Search the current device context for the pixel format that most
    // closely matches the format we described
    if( !wglChoosePixelFormatARB(deviceContext, iAttributeList, fAttributeList,
        1, &pixelFormat, &numFormats) )
    {
        // If we could not find a valid pixel format, bail out
        printf("vsWindow::vsWindow:  Unable to find valid pixel format\n");
        return;
    }

    // Make sure we got the functions we need for pbuffers
    if (wglCreatePbufferARB == NULL)
    {
        printf("vsWindow::vsWindow:  WGL pbuffer extensions not installed!\n");
        return;
    }
    
    // Create the Pbuffer based on the new pixel format
    pBuffer = wglCreatePbufferARB(deviceContext, pixelFormat, offScreenWidth,
        offScreenHeight, bufferAttribList);

    // Set the device context to that of the Pbuffer
    deviceContext = wglGetPbufferDCARB(pBuffer);

    // Create an OpenGL context for the Pbuffer
    glContext = wglCreateContext(deviceContext);

    // If the context is not valid, bail out.
    if (glContext == NULL)
    {
        printf("vsWindow::vsWindow:  Unable to create OpenGL context\n");
        return;
    }

    // Add the window to its parent screen
    parentScreen->addWindow(this);

    // Indicate that the window is used for off-screen rendering
    isOffScreenWindow = true;
}

// ------------------------------------------------------------------------
// Constructor - Initializes the window by making use of the existing MS
// Window passed in.  This requires "subclassing" the window class of the
// given window and installing a second main window procedure.
// ------------------------------------------------------------------------
vsWindow::vsWindow(vsScreen *parent, HWND msWin) : childPaneList(1, 1)
{
    vsPipe *parentPipe;
    PIXELFORMATDESCRIPTOR pixelFormatDesc;
    
    // Initialize the pane count
    childPaneCount = 0;
    
    // Flag that this window is not an offscreen window
    isOffScreenWindow = false;

    // Flag that we're not creating an MS Window in this case (we're using
    // an existing one, so we shouldn't destroy it later)
    createdMSWindow = false;

    // Remember the msWin parameter
    msWindow = msWin;

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

    // Indicate that the window is not for off-screen rendering
    isOffScreenWindow = false;
}

// ------------------------------------------------------------------------
// Destructor - Deletes any child panes that this window owns
// ------------------------------------------------------------------------
vsWindow::~vsWindow()
{
    // Make this window's context current
    makeCurrent();
    
    // Delete all child panes
    // The vsPane destructor includes a call to the parent vsWindow (this)
    // to remove it from the pane list. Keep deleting vsPanes and eventually
    // the list will go away by itself.
    while (childPaneCount > 0)
        delete ((vsPane *)(childPaneList[0]));

    // Remove the window from its screen
    parentScreen->removeWindow(this);

    // Delete the rendering context
    wglDeleteContext(glContext);

    // Handle off-screen windows seperately
    if(isOffScreenWindow)
    {
        // Destroy the device context of the Pbuffer
        wglReleasePbufferDCARB(pBuffer, deviceContext);

        // Destroy the Pbuffer itself
        wglDestroyPbufferARB(pBuffer);
    }
    else
    {
        // Remove the window mapping
        if (getMap()->mapSecondToFirst(this))
            getMap()->removeLink(this, VS_OBJMAP_SECOND_LIST);

        // Destroy the MS window, if we created it
        if (createdMSWindow)
            DestroyWindow(msWindow);
    }
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
    // Resizing an off-screen window requires destroying and re-creating
    // its Pbuffer and associated rendering context
    if(isOffScreenWindow)
    {
        int bufferAttribList[] = {
            WGL_PBUFFER_LARGEST_ARB, GL_FALSE,
            0, 0};

        // Destroy the old Pbuffer
        wglDeleteContext(glContext);
        wglReleasePbufferDCARB(pBuffer, deviceContext);
        wglDestroyPbufferARB(pBuffer);

        // Obtain a safe device context
        deviceContext = wglGetCurrentDC();

        // Use the original pixel format chosen to create a new Pbuffer
        pBuffer = wglCreatePbufferARB(deviceContext, pixelFormat, width,
            height, bufferAttribList);

        // Finally update the device context and gl rendering context
        deviceContext = wglGetPbufferDCARB(pBuffer);
        glContext = wglCreateContext(deviceContext);

        // Update the drawable width and height for future get calls
        drawableWidth = width;
        drawableHeight = height;

        return;
    }

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
    // If the window is off-screen, this must be handled seperately
    if(isOffScreenWindow)
    {
        // Return the width if requested
        if(width)
            *width = drawableWidth;

        // Return the height if requested
        if(height)
            *height = drawableHeight;

        return;
    }

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
// Retrieves the size of the drawable area of this window in pixels.  This
// will be the same as the window's size if no borders or decorations are
// present (Windows calls this the "client area".  NULL pointers may be 
// passed in for undesired data values.
// ------------------------------------------------------------------------
void vsWindow::getDrawableSize(int *width, int *height)
{
    // If the window is off-screen, this must be handled seperately
    if(isOffScreenWindow)
    {
        // Return the width if requested
        if(width)
            *width = drawableWidth;

        // Return the height if requested
        if(height)
            *height = drawableHeight;

        return;
    }

    RECT clientRect;
    
    // Get the dimensions of the window
    GetClientRect(msWindow, &clientRect);
    
    // Return the width if requested
    if (width)
        *width = clientRect.right - clientRect.left;

    // Return the height if requested
    if (height)
        *height = clientRect.bottom - clientRect.top;
}

// ------------------------------------------------------------------------
// Sets the position of this window on the screen, in pixels from the
// top-left corner of the screen.
// ------------------------------------------------------------------------
void vsWindow::setPosition(int xPos, int yPos)
{
    // If the window is off-screen, position is irrelevant
    if(isOffScreenWindow)
        return;

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
    // If the window is off-screen, position is irrelevant
    if(isOffScreenWindow)
    {
        // Return a default x-position if requested
        if (xPos)
            *xPos = 0;

        // Return a default y-position if requested
        if (yPos)
            *yPos = 0;

        return;
    }

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
    // If the window is offScreen, it doesn't have a name
    if(isOffScreenWindow)
        return;

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
    getDrawableSize(&width, &height);

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
// Get a copy of the image currently displayed in the window
// ------------------------------------------------------------------------
vsImage * vsWindow::getImage()
{
    int width, height;
    vsImage * image;

    // Make sure the window's OpenGL context is the current context
    makeCurrent();

    // Get the current size of the window and apply the border offsets
    getDrawableSize(&width, &height);

    // Allocate our temporary buffer
    unsigned char * buffer = new unsigned char[ width * height * 3 ];

    // Read the image from the frame buffer
    glPixelStorei( GL_PACK_ALIGNMENT, 1 );
    glReadPixels( 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE,
            (GLvoid *)buffer );

    // Copy everything into a vsImage structure
    image = new vsImage( width, height, VS_IMAGE_FORMAT_RGB, buffer );

    // Free the buffer
    delete [] buffer;

    return image;
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
// Brings the given pane to the front of the window (that is, it puts it
// _last_ in the window's pane list, so it's drawn last)
// ------------------------------------------------------------------------
void vsWindow::bringPaneToFront(vsPane *targetPane)
{
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

            // Put the target pane at the end of the list
            childPaneList[childPaneCount-1] = targetPane;

            // We're done
            return;
        }
    }

    // If we get here, we didn't find the requested pane, so print an
    // error
    printf("vsWindow::removePane: Specified pane not part of window\n");
}

// ------------------------------------------------------------------------
// Sends the given pane to the back of the window (that is, it puts it
// first in the window's pane list, so it's drawn first)
// ------------------------------------------------------------------------
void vsWindow::sendPaneToBack(vsPane *targetPane)
{
    int loop, sloop;

    // Iterate through the child pane list and look for the pane in 
    // question
    for (loop = 0; loop < childPaneCount; loop++)
    {
        // See if the current pane is the pane we want
        if (targetPane == childPaneList[loop])
        {
            // Found the target pane, slide the preceding panes up
            // in the list to make room for the target pane at the
            // beginning
            for (sloop = loop; sloop > 0; sloop--)
                childPaneList[sloop-1] = childPaneList[sloop];

            // Put the target pane at the front of the list
            childPaneList[0] = targetPane;

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
    // If the window is off-screen, make sure the Pbuffer is still valid
    if(isOffScreenWindow)
    {
        int flag;

        // Query the Pbuffer to see if it was lost
        wglQueryPbufferARB(pBuffer, WGL_PBUFFER_LOST_ARB, &flag);

        // If the Pbuffer is no longer valid, re-create it
        if(flag != 0)
        {
            int bufferAttribList[] = {
                WGL_PBUFFER_LARGEST_ARB, GL_FALSE,
                0, 0};

            // Destroy the old Pbuffer
            wglDeleteContext(glContext);
            wglReleasePbufferDCARB(pBuffer, deviceContext);
            wglDestroyPbufferARB(pBuffer);

            // Obtain a safe device context
            deviceContext = wglGetCurrentDC();

            // Use the original pixel format chosen to create a new Pbuffer
            pBuffer = wglCreatePbufferARB(deviceContext, pixelFormat,
                drawableWidth, drawableHeight, bufferAttribList);

            // Finally update the device context and gl rendering context
            deviceContext = wglGetPbufferDCARB(pBuffer);
            glContext = wglCreateContext(deviceContext);
        }
    }

    bool result;

    // Try to make this window's GLX context current
    result = wglMakeCurrent(deviceContext, glContext);

    // Print an error if the makeCurrent failed
    if (!result)
    {
        printf("vsWindow::makeCurrent:  Unable to attach OpenGL context to "
            "drawing surface!\n");
    }
}

// ------------------------------------------------------------------------
// Internal function
// Swaps the drawing buffers on this window if the window is on-screen.
// Note: OSG always draws to the back buffer, so off-screen windows must be
// double-buffered under OSG. However, when rendering single frames, as one
// might wish to do with an off-screen window, the double-buffering adds a
// single frame of delay. The check made here eliminates that delay.
// ------------------------------------------------------------------------
void vsWindow::swapBuffers()
{
    bool result = true;
    
    // Make sure the window is on-screen before trying to swap
    if(!isOffScreenWindow)
    {
        // Try to swap the buffers on the MS window's GDI context
        result = SwapBuffers(deviceContext);
    }

    // Print an error if the swapBuffers failed
    if (!result)
    {
        printf("vsWindow::swapBuffers:  Unable to swap buffers on the "
            "drawing surface!\n");
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

// ------------------------------------------------------------------------
// Internal function
// Returns a boolean value representing whether the window is off-screen
// ------------------------------------------------------------------------
bool vsWindow::isOffScreen()
{
    return isOffScreenWindow;
}
