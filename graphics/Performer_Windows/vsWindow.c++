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
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsWindow.h++"
#include <Performer/image.h>

// Static class member that maintains a mapping between vsWindow objects
// and their respective Microsoft Windows window instances
vsObjectMap *vsWindow::windowMap = NULL;

// Performer makes use of two handles per window, one is the HWND, which is
// stored in the windowMap above.  The other is a generic HANDLE to a "drawable"
// object.  A drawable can either be an off-screen pixel buffer or a real window.
// In the case of real windows, we need to store this handle as well, so we'll
// use a second map.
vsObjectMap *vsWindow::drawableMap = NULL;

// ------------------------------------------------------------------------
// Constructor - Initializes the window by creating a Performer pipe window
// object and creating connections with that, verifying that the window is
// being properly displayed, recording some size data from the window
// manager, and configuring the window with its default position and size.
// Also configures the window's buffer settings to be either mono or stereo
// based on the value of the stereo parameter
// ------------------------------------------------------------------------
vsWindow::vsWindow(vsScreen *parent, bool hideBorder, bool stereo) 
    : childPaneList(1, 1)
{
    vsPipe *parentPipe;
    static int fbConfigAttrs[20];

    int result;
    
    // No panes attached to start with
    childPaneCount = 0;
    
    // Flag that we created an MS Window along with the Performer window
    // (this affects what we do with it in the destructor)
    createdMSWindow = true;

    // Get the parent screen
    parentScreen = parent;
    parentPipe = parentScreen->getParentPipe();
    
    // Create a new Performer rendering window
    performerPipeWindow = new pfPipeWindow(parentPipe->getBaseLibraryObject());
    performerPipeWindow->ref();
    
    // Add this window to the parent screen's window list
    parentScreen->addWindow(this);
    
    // Window configuration
    performerPipeWindow->setMode(PFWIN_ORIGIN_LL, 0);
    if (hideBorder)
        performerPipeWindow->setMode(PFWIN_NOBORDER, 1);

    // If a stereo visual is specified, try to configure the frame buffer
    // for stereo now
    if (stereo)
    {
        // Set up a stereo/double-buffered frame buffer
        fbConfigAttrs[0] = PFFB_RGBA;
        fbConfigAttrs[1] = PFFB_DOUBLEBUFFER;
        fbConfigAttrs[2] = PFFB_STEREO;

        fbConfigAttrs[3] = PFFB_DEPTH_SIZE;
        fbConfigAttrs[4] = 24;

        fbConfigAttrs[5] = PFFB_RED_SIZE;
        fbConfigAttrs[6] = 1;

        fbConfigAttrs[7] = PFFB_STENCIL_SIZE;
        fbConfigAttrs[8] = 1;

        fbConfigAttrs[9] = 0;

        // Pass the frame buffer configuration to Performer
        performerPipeWindow->setFBConfigAttrs(fbConfigAttrs);
    }

    // Set the location and size of the window
    performerPipeWindow->setOriginSize(VS_WINDOW_DEFAULT_XPOS,
        VS_WINDOW_DEFAULT_YPOS, VS_WINDOW_DEFAULT_WIDTH,
        VS_WINDOW_DEFAULT_HEIGHT);

    // WORKAROUND:  Performer 2.5.1 seems to have introduced an annoying
    //              glitch where the program will hang if a pfPipeWindow
    //              is opened too soon after calling pfInit()  A 1-
    //              second sleep seems to get around this.
    sleep(1);

    // Display the Performer window
    performerPipeWindow->open();

    // Force the window open by repeatedly drawing new frames
    while (!(performerPipeWindow->isOpen()))
    {
        pfFrame();
    }

    // Set the window's location and size to default values
    setPosition(VS_WINDOW_DEFAULT_XPOS, VS_WINDOW_DEFAULT_YPOS);
    setSize(VS_WINDOW_DEFAULT_WIDTH, VS_WINDOW_DEFAULT_HEIGHT);

    // Register a mapping between this vsWindow and its corresponding
    // MS window handle
    getWindowMap()->registerLink(performerPipeWindow->getWSWindow(), this);

    // Also register a mapping between this vsWindow and its corresponding
    // drawable handle
    getDrawableMap()->registerLink(performerPipeWindow->getWSDrawable(), this);
}

// ------------------------------------------------------------------------
// Constructor - Initializes the window by creating a Performer pipe window
// object and creating connections with that, verifying that the window is
// being properly displayed, recording some size data from the window
// manager, and configuring the window with its default position and size.
// Also configures the window's buffer settings to be either mono or stereo
// based on the value of the stereo parameter
// ------------------------------------------------------------------------
vsWindow::vsWindow(vsScreen *parent, int x, int y, int width, int height,
                   bool hideBorder, bool stereo) : childPaneList(1, 1)
{
    vsPipe *parentPipe;
    static int fbConfigAttrs[20];

    int result;
    
    // No panes attached to start with
    childPaneCount = 0;
    
    // Flag that we created an MS Window along with the Performer window
    // (this affects what we do with it in the destructor)
    createdMSWindow = true;

    // Get the parent screen
    parentScreen = parent;
    parentPipe = parentScreen->getParentPipe();
    
    // Create a new Performer rendering window
    performerPipeWindow = new pfPipeWindow(parentPipe->getBaseLibraryObject());
    performerPipeWindow->ref();
    
    // Add this window to the parent screen's window list
    parentScreen->addWindow(this);
    
    // Window configuration
    performerPipeWindow->setMode(PFWIN_ORIGIN_LL, 0);
    if (hideBorder)
        performerPipeWindow->setMode(PFWIN_NOBORDER, 1);

    // If a stereo visual is specified, try to configure the frame buffer
    // for stereo now
    if (stereo)
    {
        // Set up a stereo/double-buffered frame buffer
        fbConfigAttrs[0] = PFFB_RGBA;
        fbConfigAttrs[1] = PFFB_DOUBLEBUFFER;
        fbConfigAttrs[2] = PFFB_STEREO;

        fbConfigAttrs[3] = PFFB_DEPTH_SIZE;
        fbConfigAttrs[4] = 24;

        fbConfigAttrs[5] = PFFB_RED_SIZE;
        fbConfigAttrs[6] = 1;

        fbConfigAttrs[7] = PFFB_STENCIL_SIZE;
        fbConfigAttrs[8] = 1;

        fbConfigAttrs[9] = 0;

        // Pass the frame buffer configuration to Performer
        performerPipeWindow->setFBConfigAttrs(fbConfigAttrs);
    }

    // Set the location and size of the window
    performerPipeWindow->setOriginSize(x, y, width, height);

    // WORKAROUND:  Performer 2.5.1 seems to have introduced an annoying
    //              glitch where the program will hang if a pfPipeWindow
    //              is opened too soon after calling pfInit()  A 1-
    //              second sleep seems to get around this.
    sleep(1);

    // Display the Performer window
    performerPipeWindow->open();

    // Force the window open by repeatedly drawing new frames
    while (!(performerPipeWindow->isOpen()))
    {
        pfFrame();
    }

    // Set the window's location and size to the given values
    setPosition(x, y);
    setSize(width, height);

    // Register a mapping between this vsWindow and its corresponding
    // MS window handle
    getWindowMap()->registerLink(performerPipeWindow->getWSWindow(), this);

    // Also register a mapping between this vsWindow and its corresponding
    // drawable handle
    getDrawableMap()->registerLink(performerPipeWindow->getWSDrawable(), this);
}

// ------------------------------------------------------------------------
// Constructor - Initializes the window using the given Performer pipe 
// window object.   The window is forced open if it is not already open.
// ------------------------------------------------------------------------
vsWindow::vsWindow(vsScreen *parent, HWND msWin) : childPaneList(1, 1)
{
    vsPipe *parentPipe;
    HWND msWindowID;
    int result;

    // Check the value of the msWin parameter, and print a warning if it
    // looks like the user is trying to use the old vsWindow constructor
    if ((msWin == (HWND)0x0) || (msWin == (HWND)0x1) || 
        (msWin == (HWND)0xFFFFFFFF))
    {
        printf("vsWindow::vsWindow:  WARNING:  HWND parameter is ");
        printf("probably not valid (%p).\n", msWin);
        printf("    The vsWindow::vsWindow(parentScreen, hideBorder) form\n");
        printf("    of the vsWindow constructor was removed in VESS 3.0.0\n\n");
    }

    // Start with no panes attached
    childPaneCount = 0;
    
    // Flag that we are not creating an MS Window along with the Performer 
    // window in this case (this affects what we do with it in the destructor)
    createdMSWindow = false;

    // Get the parent screen and pipe
    parentScreen = parent;
    parentPipe = parentScreen->getParentPipe();
    
    // Create and configure the Performer PipeWindow to use the given MS Window
    performerPipeWindow = new pfPipeWindow(parentPipe->getBaseLibraryObject());
    performerPipeWindow->setWinType(PFPWIN_TYPE_X);
    performerPipeWindow->setWSWindow(pfGetCurWSConnection(), msWin);
    performerPipeWindow->setWSDrawable(pfGetCurWSConnection(), msWin);
    
    // Add this window to the parent screen's window list
    parentScreen->addWindow(this);
    
    // Window configuration
    performerPipeWindow->setMode(PFWIN_ORIGIN_LL, 0);

    // WORKAROUND:  Performer 2.5.1 seems to have introduced an annoying
    //              glitch where the program will hang if a pfPipeWindow
    //              is opened too soon after calling pfInit()  A 1-
    //              second sleep seems to get around this.
    sleep(1);

    // Display the Performer window
    if (!performerPipeWindow->isOpen())
        performerPipeWindow->open();

    // Force the window open by repeatedly drawing frames to it
    while (!(performerPipeWindow->isOpen()))
    {
        pfFrame();
    }

    // Register a mapping between this vsWindow and its corresponding
    // MS window handle
    getWindowMap()->registerLink(performerPipeWindow->getWSWindow(), this);

    // Also register a mapping between this vsWindow and its corresponding
    // drawable handle
    getDrawableMap()->registerLink(performerPipeWindow->getWSDrawable(), this);
}

// ------------------------------------------------------------------------
// Destructor - Deletes any child panes that this window owns
// ------------------------------------------------------------------------
vsWindow::~vsWindow()
{
    // Check to see if we created the MS Window that this pfPipeWindow used
    if (createdMSWindow)
    {
        // We did create the window, so we should close it
        performerPipeWindow->close();
    }
    else
    {
        // We did not create the window, so we should not close it, but we
        // should destroy the GL context and child windows that Performer
        // likes to create (this is what closeGL() does).
        performerPipeWindow->closeGL();
    }

    // Performer bug: pfPipeWindows can't be deleted
    //delete performerPipeWindow;
    performerPipeWindow->unref();
    
    // Delete all child panes
    // The vsPane destructor includes a call to the parent vsWindow (this)
    // to remove it from the pane list. Keep deleting vsPanes and eventually
    // the list will go away by itself.
    while (childPaneCount > 0)
        delete ((vsPane *)(childPaneList[0]));
    
    // Remove this window from the parent screen's window list
    parentScreen->removeWindow(this);

    // Remove the window from the window map
    if (getWindowMap()->mapSecondToFirst(this))
        getWindowMap()->removeLink(this, VS_OBJMAP_SECOND_LIST);

    // Remove the window from the drawable map
    if (getDrawableMap()->mapSecondToFirst(this))
        getDrawableMap()->removeLink(this, VS_OBJMAP_SECOND_LIST);
}
    
// ------------------------------------------------------------------------
// Static internal function
// Return the window object map
// ------------------------------------------------------------------------
vsObjectMap *vsWindow::getWindowMap()
{
    if (!windowMap)
        windowMap = new vsObjectMap();

    return windowMap;
}

// ------------------------------------------------------------------------
// Static internal function
// Return the window object map
// ------------------------------------------------------------------------
vsObjectMap *vsWindow::getDrawableMap()
{
    if (!drawableMap)
        drawableMap = new vsObjectMap();

    return drawableMap;
}

// ------------------------------------------------------------------------
// Static internal function
// Deletes both of the object maps that hold the window and drawable 
// mappings, if they exist
// ------------------------------------------------------------------------
void vsWindow::deleteMap()
{
    if (windowMap)
    {
        delete windowMap;
        windowMap = NULL;
    }

    if (drawableMap)
    {
        delete drawableMap;
        drawableMap = NULL;
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
// Retrieves the parent screne of this window
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
    // Bounds check
    if ((index < 0) || (index >= childPaneCount))
    {
        printf("vsWindow::getChildPane: Index out of bounds\n");
        return NULL;
    }

    // Return the desired pane
    return (vsPane *)(childPaneList[index]);
}

// ------------------------------------------------------------------------
// Sets the size of this window in pixels
// ------------------------------------------------------------------------
void vsWindow::setSize(int width, int height)
{
    HWND msWindowID;

    // Obtain the MS handle for this window
    msWindowID = performerPipeWindow->getWSWindow();

    // Call the SetWindowPos function, telling it to ignore the position 
    // and z-order fields.  Also instruct the function not to activate the
    // window (don't give it focus or change the stacking order).
    SetWindowPos(msWindowID, NULL, 0, 0, width, height,
        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

// ------------------------------------------------------------------------
// Retrieves the size of this window in pixels. NULL pointers may be passed
// in for undesired data values.
// ------------------------------------------------------------------------
void vsWindow::getSize(int *width, int *height)
{
    HWND msWindowID;
    RECT windowRect;
    
    // Obtain the MS handle for this window
    msWindowID = performerPipeWindow->getWSWindow();
    
    // Get the dimensions of the window
    GetWindowRect(msWindowID, &windowRect);
    
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
// present (Windows calls this the "client area").  NULL pointers may be 
// passed in for undesired data values.
// ------------------------------------------------------------------------
void vsWindow::getDrawableSize(int *width, int *height)
{
    HWND msWindowID;
    RECT clientRect;
    
    // Obtain the MS handle for this window
    msWindowID = performerPipeWindow->getWSWindow();
    
    // Get the dimensions of the window
    GetClientRect(msWindowID, &clientRect);
    
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
    HWND msWindowID;

    // Obtain the X Display and Window objects for this window
    msWindowID = performerPipeWindow->getWSWindow();

    // Call the SetWindowPos function, telling it to ignore the size and 
    // z-order fields.  Also instruct the function not to activate the
    // window (don't give it focus or change the stacking order).
    SetWindowPos(msWindowID, NULL, xPos, yPos, 0, 0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

// ------------------------------------------------------------------------
// Retrieves the position of the window on the screen, in pixels from the
// top-left cornder of the screen. NULL pointers may be passed in for
// undesired data values.
// ------------------------------------------------------------------------
void vsWindow::getPosition(int *xPos, int *yPos)
{
    HWND msWindowID;
    RECT windowRect;
    
    // Obtain the MS handle for this window
    msWindowID = performerPipeWindow->getWSWindow();
    
    // Get the dimensions of the window
    GetWindowRect(msWindowID, &windowRect);
    
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

    // Get the size of the parent's screen
    parentScreen->getScreenSize(&screenWidth, &screenHeight);
    
    // Set the size of this window to the size of the parent's screen,
    // and the origin of the window in the top-left corner
    setPosition(0, 0);
    setSize(screenWidth, screenHeight);
}

// ------------------------------------------------------------------------
// Sets the name of the window. The window's name is usually displayed on
// its title bar.
// ------------------------------------------------------------------------
void vsWindow::setName(char *newName)
{
    HWND msWindowID;

    // Set the name of this window on the Performer window object
    performerPipeWindow->setName(newName);
    
    // Obtain the X Display and Window objects for this window
    msWindowID = performerPipeWindow->getWSWindow();

    // Call the Windows to display the new name of the window
    SetWindowText(msWindowID, newName);
}

// ------------------------------------------------------------------------
// Saves a copy of the image currently displayed in the window to the given
// file (in RGB format).
// ------------------------------------------------------------------------
void vsWindow::saveImage(char *filename)
{
    HWND msWindow;
    HDC devContext, memDevContext;
    int xpos, ypos;
    RECT windowRect;
    unsigned int width, height;
    unsigned int border, depth;
    HBITMAP bitmapHandle;
    HGDIOBJ oldBitmap;
    BITMAP bitmap;
    int bitmapSize;
    BITMAPINFO bitmapInfo;
    int bitmapDataSize;
    LPVOID bitmapData;

    unsigned long redPixel, greenPixel, bluePixel;
    unsigned short *redBuffer, *greenBuffer, *blueBuffer;
    int loop, sloop, index;
    IMAGE *imageOut;
    int tempInt;

    // Get the MS window handle and the handle to it's GDI device context
    msWindow = (HWND)performerPipeWindow->getWSWindow();
    devContext = GetDC(msWindow);
    
    // Create a new device context in memory to copy the window data to
    memDevContext = CreateCompatibleDC(NULL);
    
    // Get the current window rectangle and calculate width and height
    GetClientRect(msWindow, &windowRect);
    width = windowRect.right - windowRect.left;
    height = windowRect.bottom - windowRect.top;
    
    // Create a new bitmap structure to hold the window's contents
    bitmapHandle = CreateCompatibleBitmap(devContext, width, height);
        
    // Select the new bitmap into the new device context
    oldBitmap = SelectObject(memDevContext, bitmapHandle);
    
    // Copy the window's contents to the new device context
    BitBlt(memDevContext, 0, 0, width, height, devContext, 0, 0, 
        SRCCOPY);
    
    // Release the window's device context
    ReleaseDC(msWindow, devContext);
    
    // Get the size and shape info for the image
    bitmapSize = GetObject(bitmapHandle, sizeof(BITMAP), &bitmap);
    if (bitmapSize == 0)
    {
        // Unable to get the bitmap image, bail out
        printf("vsWindow::saveImage:  Unable to access contents of window\n");
        return;
    }

    // Fill out a BITMAPINFO structure to describe the desired bitmap format
    memset(&bitmapInfo, 0, sizeof(BITMAPINFO));
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 24;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    bitmapInfo.bmiHeader.biSizeImage = 0;
    bitmapInfo.bmiHeader.biXPelsPerMeter = 2834;
    bitmapInfo.bmiHeader.biYPelsPerMeter = 2834;
    bitmapInfo.bmiHeader.biClrUsed = 0;
    bitmapInfo.bmiHeader.biClrImportant = 0;
    
    // Get the bitmap data
    bitmapData = (LPVOID)malloc(width * height * 3);
    bitmapDataSize = GetDIBits(devContext, bitmapHandle, 0, height,
        bitmapData, &bitmapInfo, DIB_RGB_COLORS);

    // Create a buffer area for each color component of the image
    redBuffer = (unsigned short *)malloc(sizeof(unsigned short) * width);
    greenBuffer = (unsigned short *)malloc(sizeof(unsigned short) * width);
    blueBuffer = (unsigned short *)malloc(sizeof(unsigned short) * width);

    // Open the image file
    imageOut = iopen(filename, "w", RLE(1), 3, width, height, 3);
    if (!imageOut)
    {
        printf("vsWindow::saveImage: NULL image file pointer\n");
        return;
    }

    // Process the image, one pixel at a time
    for (loop = 0; loop < height; loop++)
    {
        for (sloop = 0; sloop < width; sloop++)
        {
            // Get the red, green, and blue pixels from the bitmap
            index = ((height-1) - loop)*width*3 + sloop * 3;
            
            // Red
            blueBuffer[sloop] = ((char *)bitmapData)[index];

            // Green
            greenBuffer[sloop] = ((char *)bitmapData)[index+1];

            // Blue
            redBuffer[sloop] = ((char *)bitmapData)[index+2];
        }

        // Dump each completed row to the image file
        tempInt = height - loop - 1;
        putrow(imageOut, redBuffer, tempInt, 0);
        putrow(imageOut, greenBuffer, tempInt, 1);
        putrow(imageOut, blueBuffer, tempInt, 2);
    }

    // Clean up
    iclose(imageOut);
    free(bitmapData);
}

// ------------------------------------------------------------------------
// Returns a copy of the image currently displayed in the window
// ------------------------------------------------------------------------
vsImage * vsWindow::getImage()
{
    vsImage * image;
    HWND msWindow;
    HDC devContext, memDevContext;
    int xpos, ypos;
    RECT windowRect;
    unsigned int width, height;
    unsigned int border, depth;
    HBITMAP bitmapHandle;
    HGDIOBJ oldBitmap;
    BITMAP bitmap;
    int bitmapSize;
    BITMAPINFO bitmapInfo;
    int bitmapDataSize;
    LPVOID bitmapData;

    unsigned long redPixel, greenPixel, bluePixel;
    unsigned short *redBuffer, *greenBuffer, *blueBuffer;
    int loop, sloop, index;
    IMAGE *imageOut;
    int tempInt;

    // Get the MS window handle and the handle to it's GDI device context
    msWindow = (HWND)performerPipeWindow->getWSWindow();
    devContext = GetDC(msWindow);
    
    // Create a new device context in memory to copy the window data to
    memDevContext = CreateCompatibleDC(NULL);
    
    // Get the current window rectangle and calculate width and height
    GetClientRect(msWindow, &windowRect);
    width = windowRect.right - windowRect.left;
    height = windowRect.bottom - windowRect.top;
    
    // Create a new bitmap structure to hold the window's contents
    bitmapHandle = CreateCompatibleBitmap(devContext, width, height);
        
    // Select the new bitmap into the new device context
    oldBitmap = SelectObject(memDevContext, bitmapHandle);
    
    // Copy the window's contents to the new device context
    BitBlt(memDevContext, 0, 0, width, height, devContext, 0, 0, 
        SRCCOPY);
    
    // Release the window's device context
    ReleaseDC(msWindow, devContext);
    
    // Get the size and shape info for the image
    bitmapSize = GetObject(bitmapHandle, sizeof(BITMAP), &bitmap);
    if (bitmapSize == 0)
    {
        // Unable to get the bitmap image, bail out
        printf("vsWindow::saveImage:  Unable to access contents of window\n");
        return NULL;
    }

    // Fill out a BITMAPINFO structure to describe the desired bitmap format
    memset(&bitmapInfo, 0, sizeof(BITMAPINFO));
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 24;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    bitmapInfo.bmiHeader.biSizeImage = 0;
    bitmapInfo.bmiHeader.biXPelsPerMeter = 2834;
    bitmapInfo.bmiHeader.biYPelsPerMeter = 2834;
    bitmapInfo.bmiHeader.biClrUsed = 0;
    bitmapInfo.bmiHeader.biClrImportant = 0;
    
    // Get the bitmap data
    bitmapData = (LPVOID)malloc(width * height * 3);
    bitmapDataSize = GetDIBits(devContext, bitmapHandle, 0, height,
        bitmapData, &bitmapInfo, DIB_RGB_COLORS);

    // Put the data into vsImage
    image = new vsImage( width, height, VS_IMAGE_FORMAT_RGB, 
        (unsigned char *)bitmapData);

    // Windows returns the image with the origin in the top left. We store our
    // image like OpenGL with the origin in the bottom left. Flip it
    image->flipVertical();

    // Clean up
    free(bitmapData);

    return image;
}

// ------------------------------------------------------------------------
// Returns the Performer object associated with this object
// ------------------------------------------------------------------------
pfPipeWindow *vsWindow::getBaseLibraryObject()
{
    return performerPipeWindow;
}

// ------------------------------------------------------------------------
// VESS internal function
// Adds the given pane to the window's list of child panes
// ------------------------------------------------------------------------
void vsWindow::addPane(vsPane *newPane)
{
    // Add pane to window's internal list
    childPaneList[childPaneCount++] = newPane;
    
    // Reference the pane
    newPane->ref();

    // Add pane (as pfChannel) to pfPipeWindow
    performerPipeWindow->addChan(newPane->getBaseLibraryObject());
}

// ------------------------------------------------------------------------
// VESS internal function
// Removes the given pane from the window's list of child panes
// ------------------------------------------------------------------------
void vsWindow::removePane(vsPane *targetPane)
{
    // Remove pane from window's internal list
    int loop, sloop;
    
    // Search the child pane list for the target pane
    for (loop = 0; loop < childPaneCount; loop++)
        if (targetPane == childPaneList[loop])
        {
            // Remove the target pane from the child list by sliding the
	    // other children down over the removed one
            for (sloop = loop; sloop < (childPaneCount-1); sloop++)
                childPaneList[sloop] = childPaneList[sloop+1];

            // One fewer child
            childPaneCount--;

            // Unreference the pane
            targetPane->unref();

            // Remove the pane's pfChannel object from this window's
	    // pfPipeWindow object
            performerPipeWindow->removeChan(targetPane->getBaseLibraryObject());
            return;
        }

    printf("vsWindow::removePane: Specified pane not part of window\n");
}
