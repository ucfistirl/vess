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
//    Author(s):    Bryan Kline, Casey Thurston
//
//------------------------------------------------------------------------

#include "vsWindow.h++"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/time.h>
#include <Performer/image.h>

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
    Display *xWindowDisplay;
    Window xWindowID;
    Window *childPointer;
    unsigned int childCount;
    Window parentID, rootID;
    XWindowAttributes xattr;
    int fbConfigAttrs[20];

    int result;
    
    // No panes attached to start with
    childPaneCount = 0;

    // Flag that we created a new X Window in this case (this affects how
    // we deal with it in the destructor)
    createdXWindow = true;
    
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

    // Set up a 24-bit double-buffered frame buffer
    fbConfigAttrs[0] = PFFB_RGBA;
    fbConfigAttrs[1] = PFFB_DOUBLEBUFFER;
    fbConfigAttrs[2] = PFFB_DEPTH_SIZE;
    fbConfigAttrs[3] = 24;
    fbConfigAttrs[4] = PFFB_RED_SIZE;
    fbConfigAttrs[5] = 8;
    fbConfigAttrs[6] = PFFB_GREEN_SIZE;
    fbConfigAttrs[7] = 8;
    fbConfigAttrs[8] = PFFB_BLUE_SIZE;
    fbConfigAttrs[9] = 8;
    fbConfigAttrs[10] = PFFB_STENCIL_SIZE;
    fbConfigAttrs[11] = 8;
    fbConfigAttrs[12] = 0;

    // If a stereo visual is specified, try to configure the frame buffer
    // for stereo
    if (stereo)
    {
        fbConfigAttrs[12] = PFFB_STEREO;
        fbConfigAttrs[13] = 0;
    }

    // Pass the frame buffer configuration to Performer
    performerPipeWindow->setFBConfigAttrs(fbConfigAttrs);

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

    // Force the window open by repeatedly calling the X server to
    // flush the stream
    xWindowDisplay = pfGetCurWSConnection();
    while (!(performerPipeWindow->isOpen()))
    {
        pfFrame();
        XFlush(xWindowDisplay);
    }

    // Get the window that Performer thinks is topmost, and then query the
    // X server to determine if that window really is the topmost one.
    xWindowID = performerPipeWindow->getWSWindow();

    // Keep trying until we reach the window we want
    do
    {
        // Query X for the ID of the window's parent window
        result = XQueryTree(xWindowDisplay, xWindowID, &rootID, &parentID,
            &childPointer, &childCount);
        XFree(childPointer);

        // Store the parent window ID, if it's something meaningful. (zero
        // means we're at the screen's root window; that's too far.)
        if (result == 0)
        {
            pfFrame();
            XFlush(xWindowDisplay);
        }
        else if (parentID != rootID)
            xWindowID = parentID;
    }
    while (rootID != parentID);

    // Store the ID of the topmost window
    topWindowID = xWindowID;

    // Attempt to determine the size of the window manager's border for
    // this window by checking the difference between Performer's idea
    // of the window size and X's one.
    XGetWindowAttributes(xWindowDisplay, topWindowID, &xattr);
    xPositionOffset = VS_WINDOW_DEFAULT_XPOS - xattr.x;
    yPositionOffset = VS_WINDOW_DEFAULT_YPOS - xattr.y;
    widthOffset = xattr.width - VS_WINDOW_DEFAULT_WIDTH;
    heightOffset = xattr.height - VS_WINDOW_DEFAULT_HEIGHT;
    
    // Set the window's location and size to default values
    setPosition(VS_WINDOW_DEFAULT_XPOS, VS_WINDOW_DEFAULT_YPOS);
    setSize(VS_WINDOW_DEFAULT_WIDTH, VS_WINDOW_DEFAULT_HEIGHT);
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
    Display *xWindowDisplay;
    Window xWindowID;
    Window *childPointer;
    unsigned int childCount;
    Window parentID, rootID;
    XWindowAttributes xattr;
    int fbConfigAttrs[20];

    int result;
    
    // No panes attached to start with
    childPaneCount = 0;
    
    // Flag that we created a new X Window in this case (this affects how
    // we deal with it in the destructor)
    createdXWindow = true;
    
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

    // Set up a 24-bit double-buffered frame buffer
    fbConfigAttrs[0] = PFFB_RGBA;
    fbConfigAttrs[1] = PFFB_DOUBLEBUFFER;
    fbConfigAttrs[2] = PFFB_DEPTH_SIZE;
    fbConfigAttrs[3] = 24;
    fbConfigAttrs[4] = PFFB_RED_SIZE;
    fbConfigAttrs[5] = 8;
    fbConfigAttrs[6] = PFFB_GREEN_SIZE;
    fbConfigAttrs[7] = 8;
    fbConfigAttrs[8] = PFFB_BLUE_SIZE;
    fbConfigAttrs[9] = 8;
    fbConfigAttrs[10] = PFFB_STENCIL_SIZE;
    fbConfigAttrs[11] = 8;
    fbConfigAttrs[12] = 0;

    // If a stereo visual is specified, try to configure the frame buffer
    // for stereo
    if (stereo)
    {
        fbConfigAttrs[12] = PFFB_STEREO;
        fbConfigAttrs[13] = 0;
    }

    // Pass the frame buffer configuration to Performer
    performerPipeWindow->setFBConfigAttrs(fbConfigAttrs);

    // Set the location and size of the window
    performerPipeWindow->setOriginSize(x, y, width, height);

    // WORKAROUND:  Performer 2.5.1 seems to have introduced an annoying
    //              glitch where the program will hang if a pfPipeWindow
    //              is opened too soon after calling pfInit()  A 1-
    //              second sleep seems to get around this.
    sleep(1);

    // Display the Performer window
    performerPipeWindow->open();

    // Force the window open by repeatedly calling the X server to
    // flush the stream
    xWindowDisplay = pfGetCurWSConnection();
    while (!(performerPipeWindow->isOpen()))
    {
        pfFrame();
        XFlush(xWindowDisplay);
    }

    // Get the window that Performer thinks is topmost, and then query the
    // X server to determine if that window really is the topmost one.
    xWindowID = performerPipeWindow->getWSWindow();

    // Keep trying until we reach the window we want
    do
    {
        // Query X for the ID of the window's parent window
        result = XQueryTree(xWindowDisplay, xWindowID, &rootID, &parentID,
            &childPointer, &childCount);
        XFree(childPointer);

        // Store the parent window ID, if it's something meaningful. (zero
        // means we're at the screen's root window; that's too far.)
        if (result == 0)
        {
            pfFrame();
            XFlush(xWindowDisplay);
        }
        else if (parentID != rootID)
            xWindowID = parentID;
    }
    while (rootID != parentID);

    // Store the ID of the topmost window
    topWindowID = xWindowID;

    // Attempt to determine the size of the window manager's border for
    // this window by checking the difference between Performer's idea
    // of the window size and X's one.
    XGetWindowAttributes(xWindowDisplay, topWindowID, &xattr);
    xPositionOffset = x - xattr.x;
    yPositionOffset = y - xattr.y;
    widthOffset = xattr.width - width;
    heightOffset = xattr.height - height;
    
    // Set the window's location and size to default values
    setPosition(x, y);
    setSize(width, height);
}

// ------------------------------------------------------------------------
// Constructor - Initializes the window by creating a Performer pipe window
// object and creating connections with that, verifying that the window is
// being properly displayed, recording some size data from the window
// manager, and configuring the window with its default position and size.
// Also configures the window's buffer settings to be either mono or stereo
// based on the value of the stereo parameter
// ------------------------------------------------------------------------
vsWindow::vsWindow(vsScreen *parent, int offScreenWidth, int offScreenHeight)
   : childPaneList(1, 1)
{
    vsPipe *parentPipe;

    // No panes attached to start with
    childPaneCount = 0;
    
    // Flag that we created a new X Window in this case (this affects how
    // we deal with it in the destructor)
    createdXWindow = true;
    
    // Get the parent screen
    parentScreen = parent;
    parentPipe = parentScreen->getParentPipe();

    // Add this window to the parent screen's window list
    parentScreen->addWindow(this);

    // Create a new Performer rendering window
    performerPipeWindow = new pfPipeWindow(parentPipe->getBaseLibraryObject());
    performerPipeWindow->ref();

    // Window configuration
    performerPipeWindow->setMode(PFWIN_ORIGIN_LL, 0);

    // Set the location and size of the window
    performerPipeWindow->setOriginSize(0, 0, offScreenWidth, offScreenHeight);

    // Declare the window to be an off-screen type. Pbuffers are automatically
    // declared as unmanaged
    performerPipeWindow->setWinType(PFPWIN_TYPE_PBUFFER|PFPWIN_TYPE_UNMANAGED);

    // Set the configuration function for the window
    performerPipeWindow->setConfigFunc(vsWindow::InitPbuffer);

    // Tell the pipe window to use its configuration function during the next
    // draw process. This will cause the window to open.
    performerPipeWindow->config();
}

// ------------------------------------------------------------------------
// Constructor - Initializes the window using the given Performer pipe 
// window object.   The window is forced open if it is not already open.
// ------------------------------------------------------------------------
vsWindow::vsWindow(vsScreen *parent, Window xWin) : childPaneList(1, 1)
{
    vsPipe *parentPipe;
    Display *xWindowDisplay;
    Window xWindowID;
    Window *childPointer;
    unsigned int childCount;
    Window parentID, rootID;
    XWindowAttributes xattr;
    int result;

    // Check the value of the xWin parameter, and print a warning if it
    // looks like the user is trying to use the old vsWindow constructor
    if ((xWin == 0) || (xWin == 1) || (xWin == -1))
    {
        printf("vsWindow::vsWindow:  WARNING:  X Window parameter is ");
        printf("probably not valid (%d).\n", xWin);
        printf("    The vsWindow::vsWindow(parentScreen, hideBorder) form\n");
        printf("    of the vsWindow constructor was removed in VESS 3.0.0\n\n");
        printf("    If a BadWindow error appears below, make sure your code\n");
        printf("    is not using this outdated constructor.\n");
    }

    // Start with no panes attached
    childPaneCount = 0;
    
    // Flag that we did not create a new X Window in this case (instead, we
    // told Peformer to use an existing one)
    createdXWindow = false;
    
    // Get the parent screen and pipe
    parentScreen = parent;
    parentPipe = parentScreen->getParentPipe();
    
    // Create and configure the Performer PipeWindow to use the given X Window
    performerPipeWindow = new pfPipeWindow(parentPipe->getBaseLibraryObject());
    performerPipeWindow->ref();
    performerPipeWindow->setWinType(PFPWIN_TYPE_X);
    performerPipeWindow->setWSWindow(pfGetCurWSConnection(), xWin);
    performerPipeWindow->setWSDrawable(pfGetCurWSConnection(), xWin);
    
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

    // Force the window open by repeatedly calling the X server to
    // flush the stream
    xWindowDisplay = pfGetCurWSConnection();
    while (!(performerPipeWindow->isOpen()))
    {
        performerPipeWindow->open();
        pfFrame();
        XFlush(xWindowDisplay);
    }

    // Get the window that Performer thinks is topmost, and then query the
    // X server to determine if that window really is the topmost one.
    xWindowID = performerPipeWindow->getWSWindow();

    // Keep trying until we reach the window we want
    do
    {
        // Query X for the ID of the window's parent window
        result = XQueryTree(xWindowDisplay, xWindowID, &rootID, &parentID,
            &childPointer, &childCount);
        XFree(childPointer);

        // Store the parent window ID, if it's something meaningful. (zero
        // means we're at the screen's root window; that's too far.)
        if (result == 0)
        {
            pfFrame();
            XFlush(xWindowDisplay);
        }
        else if (parentID != rootID)
            xWindowID = parentID;
    }
    while (rootID != parentID);

    // Store the ID of the topmost window
    topWindowID = xWindowID;

    // Attempt to determine the size of the window manager's border for
    // this window by checking the difference between Performer's idea
    // of the window size and X's one.
    XGetWindowAttributes(xWindowDisplay, topWindowID, &xattr);
    xPositionOffset = VS_WINDOW_DEFAULT_XPOS - xattr.x;
    yPositionOffset = VS_WINDOW_DEFAULT_YPOS - xattr.y;
    widthOffset = xattr.width - VS_WINDOW_DEFAULT_WIDTH;
    heightOffset = xattr.height - VS_WINDOW_DEFAULT_HEIGHT;
}

// ------------------------------------------------------------------------
// Destructor - Deletes any child panes that this window owns
// ------------------------------------------------------------------------
vsWindow::~vsWindow()
{
    // See if we created the X Window that this pfPipeWindow used
    if (createdXWindow)
    {
        // We did create the X Window, so we should close it
        performerPipeWindow->close();
    }
    else
    {
        // We did not create the X Window, so we should not close it, but
        // we should destroy the GL context and child windows that Performer
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
    Display *xWindowDisplay;
    Window xWindowID;

    // Obtain the X Display and Window objects for this window
    xWindowDisplay = pfGetCurWSConnection();
    xWindowID = performerPipeWindow->getWSWindow();

    // Send the request for X to resize the window
    XResizeWindow(xWindowDisplay, xWindowID, width - widthOffset,
        height - heightOffset);
}

// ------------------------------------------------------------------------
// Retrieves the size of this window in pixels. NULL pointers may be passed
// in for undesired data values.
// ------------------------------------------------------------------------
void vsWindow::getSize(int *width, int *height)
{
    Display *xWindowDisplay;
    XWindowAttributes xattr;
    int x, y;
    
    // Get the current X Display object
    xWindowDisplay = pfGetCurWSConnection();

    // Attempt to get the size of the window; just mark the size as zero
    // if the attempt fails
    if (XGetWindowAttributes(xWindowDisplay, topWindowID, &xattr) == 0)
    {
        x = 0;
        y = 0;
    }
    else
    {
        x = xattr.width;
        y = xattr.height;
    }

    // Return the desired values
    if (width)
        *width = x;
    if (height)
        *height = y;
}

// ------------------------------------------------------------------------
// Returns the size of the drawable area of this window.  This will be
// the same as the window size if there are no borders or decorations.
// ------------------------------------------------------------------------
void vsWindow::getDrawableSize(int *width, int *height)
{
    Display *xWindowDisplay;
    int result;
    XWindowAttributes xattr;
    int x, y;

    // Get the X display connection from the screen's parent pipe
    xWindowDisplay = pfGetCurWSConnection();

    // Query the window attributes from X, and make sure the query
    // succeeds
    result = XGetWindowAttributes(xWindowDisplay, 
        performerPipeWindow->getWSWindow(), &xattr);
    if (result == 0)
    {
        // The query failed, return zeroes as default
        x = 0;
        y = 0;
    }
    else
    {
        // Get the window size from the attributes structure
        x = xattr.width;
        y = xattr.height;
    }

    // Return the width if requested
    if (width)
        *width = x;

    // Return the height if requested
    if (height)
        *height = y;
}

// ------------------------------------------------------------------------
// Sets the position of this window on the screen, in pixels from the
// top-left corner of the screen.
// ------------------------------------------------------------------------
void vsWindow::setPosition(int xPos, int yPos)
{
    Display *xWindowDisplay;
    Window xWindowID;

    // Obtain the X Display and Window objects for this window
    xWindowDisplay = pfGetCurWSConnection();
    xWindowID = performerPipeWindow->getWSWindow();

    // Send the request for X to reposition the window
    XMoveWindow(xWindowDisplay, xWindowID, xPos + xPositionOffset,
        yPos + yPositionOffset);
}

// ------------------------------------------------------------------------
// Retrieves the position of the window on the screen, in pixels from the
// top-left cornder of the screen. NULL pointers may be passed in for
// undesired data values.
// ------------------------------------------------------------------------
void vsWindow::getPosition(int *xPos, int *yPos)
{
    Display *xWindowDisplay;
    XWindowAttributes xattr;
    int x, y;

    // Get the current X Display object
    xWindowDisplay = pfGetCurWSConnection();

    // Attempt to get the position of the window; just mark the position
    // as zero if the attempt fails
    if (XGetWindowAttributes(xWindowDisplay, topWindowID, &xattr) == 0)
    {
        x = 0;
        y = 0;
    }
    else
    {
        x = xattr.x;
        y = xattr.y;
    }

    // Return the desired values (if requested)
    if (xPos)
        *xPos = x;
    if (yPos)
        *yPos = y;
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
    XTextProperty nameProperty;
    Display *xWindowDisplay;
    Window xWindowID;

    // Set the name of this window on the Performer window object
    performerPipeWindow->setName(newName);
    
    // Obtain the X Display and Window objects for this window
    xWindowDisplay = pfGetCurWSConnection();
    xWindowID = performerPipeWindow->getWSWindow();

    // Call the X window manager to display the new name of the window
    XStringListToTextProperty(&newName, 1, &nameProperty);
    XSetWMName(xWindowDisplay, xWindowID, &nameProperty);
}

// ------------------------------------------------------------------------
// Saves a copy of the image currently displayed in the window to the given
// file (in RGB format).
// ------------------------------------------------------------------------
void vsWindow::saveImage(char *filename)
{
    Display *xWindowDisplay;
    Drawable winDrawable;
    Window rootWin;
    int xpos, ypos;
    unsigned int width, height;
    unsigned int border, depth;
    XImage *image;

    unsigned long pixelData;
    unsigned long redMask, greenMask, blueMask;
    unsigned long redMax, greenMax, blueMax;
    unsigned long redPixel, greenPixel, bluePixel;
    int redShift, greenShift, blueShift;
    unsigned short *redBuffer, *greenBuffer, *blueBuffer;
    int loop, sloop;
    IMAGE *imageOut;
    unsigned short redVals[4096], greenVals[4096], blueVals[4096];
    int tempInt;

    vsCallbackList *callbackList;
    void *sharedBuffer;
    int offset;

    // Get the connections to the X window system and to the drawable region
    // of the window
    xWindowDisplay = pfGetCurWSConnection();
    winDrawable = performerPipeWindow->getWSDrawable();

    // If the window is a pbuffer, XGetGeometry cannot be called on its
    // drawable, so we have to make GL calls through Performer
    if (performerPipeWindow->getWinType() & PFPWIN_TYPE_PBUFFER)
    {
        // Query the drawable for its width and height
        glXQueryDrawable(xWindowDisplay, winDrawable, GLX_WIDTH, &width);
        glXQueryDrawable(xWindowDisplay, winDrawable, GLX_HEIGHT, &height);

        // Get the callback list of the last child pane, so that the image
        // taken is the one drawn last
        callbackList =
            getChildPane(childPaneCount -1)->getPerformerCallbackList();

        // Give the callback list a save image callback, storing the shared
        // memory on which the callback acts into the temporary buffer. The
        // image is retreived in RGB format, which uses three bytes per pixel
        sharedBuffer = callbackList->
            appendCallback(saveImage, width * height * 3);

        // Keep trying to acquire the data until the image has been written
        while (!callbackList->acquireData(sharedBuffer))
        {
            // Force a traversal of the callback list in the draw process
            pfFrame();
        }

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
                // Set the base offset of the pixel, considering that each
                // pixel is actually three bytes
                offset = ((loop * width) + sloop) * 3;

                // Split the main buffer into colored pixel buffers
                redBuffer[sloop] = ((unsigned char *)sharedBuffer)[offset];
                greenBuffer[sloop] = ((unsigned char *)sharedBuffer)[offset+1];
                blueBuffer[sloop] = ((unsigned char *)sharedBuffer)[offset+2];
            }

            // Dump each completed row to the image file
            tempInt = height - loop - 1;
            putrow(imageOut, redBuffer, tempInt, 0);
            putrow(imageOut, greenBuffer, tempInt, 1);
            putrow(imageOut, blueBuffer, tempInt, 2);
        }

        // Free the shared memory that Performer used to create the image
        pfFree(sharedBuffer);
    }
    else
    {
        // Get the size and shape info for the window
        XGetGeometry(xWindowDisplay, winDrawable, &rootWin, &xpos, &ypos,
            &width, &height, &border, &depth);

        // Insure we begin at winDrawable's origin.  This is done because the
        // XGetGeometry call returns the x and y pos relative to the parent's
        // window.
        xpos = 0;
        ypos = 0;

        // Capture the contents of the window into an X image struture
        image = XGetImage(xWindowDisplay, winDrawable, xpos, ypos, width, height,
            AllPlanes, ZPixmap);
        if (!image)
        {
            printf("vsWindow::saveImage: Unable to access contents of window\n");
            return;
        }
    
        // * Juggle the 'mask' bits around (as given by the X image structure)
        // to determine which color data bits occupy what space within each
        // pixel data unit. This information is stored as a bit shift indicating
        // the number of bits to move before we get to the start of that color
        // component's bits, and a bit mask used to mask out bits from other
        // color components. Also construct a lookup table to allow for quick
        // scaling from whatever is stored in the data into the 0-255 range
        // that the RGB format wants.
    
        // Computing the shift and mask involves shifting the mask down until
        // the low bit becomes one. The mask is them saved like that, and the
        // number of shifts is recorded.
    
        // Red size and offset
        redMax = image->red_mask;
        redShift = 0;
        while (!(redMax & 1))
        {
            redShift++;
            redMax >>= 1;
        }
        redMask = image->red_mask;
        // Red scale lookup table
        for (loop = 0; loop <= redMax; loop++)
        {
            redVals[loop] = ((loop * 255) / redMax);
        }

        // Green size and offset
        greenMax = image->green_mask;
        greenShift = 0;
        while (!(greenMax & 1))
        {
            greenShift++;
            greenMax >>= 1;
        }
        greenMask = image->green_mask;
        // Green scale lookup table
        for (loop = 0; loop <= greenMax; loop++)
        {
            greenVals[loop] = ((loop * 255) / greenMax);
        }

        // Blue size and offset
        blueMax = image->blue_mask;
        blueShift = 0;
        while (!(blueMax & 1))
        {
            blueShift++;
            blueMax >>= 1;
        }
        blueMask = image->blue_mask;
        // Blue scale lookup table
        for (loop = 0; loop <= blueMax; loop++)
        {
            blueVals[loop] = ((loop * 255) / blueMax);
        }

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
                // Call an X function to extract one of the image pixels from
                // the X image object
                pixelData = XGetPixel(image, sloop, loop);

                // Decode the three component pixel values using the associated
                // bit mask, bit shift value, and lookup table. Store the
                // resulting value in the pixel array for that component.

                // Red
                redPixel = pixelData & redMask;
                redPixel >>= redShift;
                redBuffer[sloop] = redVals[redPixel];
    
                // Green
                greenPixel = pixelData & greenMask;
                greenPixel >>= greenShift;
                greenBuffer[sloop] = greenVals[greenPixel];

                // Blue
                bluePixel = pixelData & blueMask;
                bluePixel >>= blueShift;
                blueBuffer[sloop] = blueVals[bluePixel];
            }

            // Dump each completed row to the image file
            tempInt = height - loop - 1;
            putrow(imageOut, redBuffer, tempInt, 0);
            putrow(imageOut, greenBuffer, tempInt, 1);
            putrow(imageOut, blueBuffer, tempInt, 2);
        }

        // Get rid of the xImage now that we're done with it
	XDestroyImage(image);
    }

    // Clean up
    iclose(imageOut);
    free(redBuffer);
    free(blueBuffer);
    free(greenBuffer);
}

// ------------------------------------------------------------------------
// Returns a copy of the image currently displayed in the window
// ------------------------------------------------------------------------
vsImage * vsWindow::getImage()
{
    vsImage * image;
    Display *xWindowDisplay;
    Drawable winDrawable;
    Window rootWin;
    int xpos, ypos;
    unsigned int width, height;
    unsigned int border, depth;
    XImage *ximage;

    unsigned long pixelData;
    unsigned long redMask, greenMask, blueMask;
    unsigned long redMax, greenMax, blueMax;
    unsigned long redPixel, greenPixel, bluePixel;
    int redShift, greenShift, blueShift;
    int loop, sloop;
    unsigned short redVals[4096], greenVals[4096], blueVals[4096];
    unsigned char * tempBuffer;
    int index;

    vsCallbackList *callbackList;
    void *sharedBuffer;

    // Get the connections to the X window system and to the drawable region
    // of the window
    xWindowDisplay = pfGetCurWSConnection();
    winDrawable = performerPipeWindow->getWSDrawable();

    // If the window is a pbuffer, XGetGeometry cannot be called on its
    // drawable, so we have to make GL calls through Performer
    if (performerPipeWindow->getWinType() & PFPWIN_TYPE_PBUFFER)
    {
        // Query the drawable for its width and height
        glXQueryDrawable(xWindowDisplay, winDrawable, GLX_WIDTH, &width);
        glXQueryDrawable(xWindowDisplay, winDrawable, GLX_HEIGHT, &height);

        // Get the callback list of the last child pane, so that the image
        // taken is the one drawn last
        callbackList =
            getChildPane(childPaneCount - 1)->getPerformerCallbackList();

        // Give the callback list a save image callback, storing the shared
        // memory on which the callback acts into the temporary buffer. The
        // image is retreived in RGB format, which uses three bytes per pixel
        sharedBuffer = callbackList->
            appendCallback(saveImage, width * height * 3);

        // Keep trying to acquire the data until the image has been written
        while (!callbackList->acquireData(sharedBuffer))
        {
            // Force a traversal of the callback list in the draw process
            pfFrame();
        }

        // Store the image data
        image = new vsImage(width, height, VS_IMAGE_FORMAT_RGB,
            (unsigned char *)sharedBuffer);

        // Free the shared memory that Performer used to create the image
        pfFree(sharedBuffer);
    }
    else
    {
        // Get the size and shape info for the window
        XGetGeometry(xWindowDisplay, winDrawable, &rootWin, &xpos, &ypos,
            &width, &height, &border, &depth);

        // Insure we begin at winDrawable's origin.  This is done because the
        // XGetGeometry call returns the x and y pos relative to the parent's
        // window.
        xpos = 0;
        ypos = 0;

        // Capture the contents of the window into an X image struture
        ximage = XGetImage(xWindowDisplay, winDrawable, xpos, ypos, width, height,
            AllPlanes, ZPixmap);
        if (!ximage)
        {
            printf("vsWindow::saveImage: Unable to access contents of window\n");
            return NULL;
        }
    
        // * Juggle the 'mask' bits around (as given by the X image structure)
        // to determine which color data bits occupy what space within each
        // pixel data unit. This information is stored as a bit shift indicating
        // the number of bits to move before we get to the start of that color
        // component's bits, and a bit mask used to mask out bits from other
        // color components. Also construct a lookup table to allow for quick
        // scaling from whatever is stored in the data into the 0-255 range
        // that the RGB format wants.
    
        // Computing the shift and mask involves shifting the mask down until
        // the low bit becomes one. The mask is them saved like that, and the
        // number of shifts is recorded.
    
        // Red size and offset
        redMax = ximage->red_mask;
        redShift = 0;
        while (!(redMax & 1))
        {
            redShift++;
            redMax >>= 1;
        }
        redMask = ximage->red_mask;
        // Red scale lookup table
        for (loop = 0; loop <= redMax; loop++)
        {
            redVals[loop] = ((loop * 255) / redMax);
        }

        // Green size and offset
        greenMax = ximage->green_mask;
        greenShift = 0;
        while (!(greenMax & 1))
        {
            greenShift++;
            greenMax >>= 1;
        }
        greenMask = ximage->green_mask;
        // Green scale lookup table
        for (loop = 0; loop <= greenMax; loop++)
        {
            greenVals[loop] = ((loop * 255) / greenMax);
        }

        // Blue size and offset
        blueMax = ximage->blue_mask;
        blueShift = 0;
        while (!(blueMax & 1))
        {
            blueShift++;
            blueMax >>= 1;
        }
        blueMask = ximage->blue_mask;
        // Blue scale lookup table
        for (loop = 0; loop <= blueMax; loop++)
        {
            blueVals[loop] = ((loop * 255) / blueMax);
        }

        // Create a buffer area for each color component of the image
        tempBuffer = new unsigned char[ width * height * 3];

        // Process the image, one pixel at a time
        for (loop = 0, index = 0; loop < height; loop++)
        {
            for (sloop = 0; sloop < width; sloop++)
            {
                // Call an X function to extract one of the image pixels from
                // the X image object
                pixelData = XGetPixel(ximage, sloop, loop);
    
                // Decode the three component pixel values using the associated
                // bit mask, bit shift value, and lookup table. Store the
                // resulting value in the pixel array for that component.
    
                // Red
                redPixel = pixelData & redMask;
                redPixel >>= redShift;
                tempBuffer[ index++ ] = redVals[redPixel];
    
                // Green
                greenPixel = pixelData & greenMask;
                greenPixel >>= greenShift;
                tempBuffer[ index++ ] = greenVals[greenPixel];
    
                // Blue
                bluePixel = pixelData & blueMask;
                bluePixel >>= blueShift;
                tempBuffer[ index++ ] = blueVals[bluePixel];
            }
    
        }

        // Clean up the XImage
        XDestroyImage(ximage);

        // Put the data into vsImage
        image = new vsImage( width, height, VS_IMAGE_FORMAT_RGB, tempBuffer );

        // X returns the image with the origin in the top left. We store our
        // image like OpenGL with the origin in the bottom left. Flip it
        image->flipVertical();

        // Delete the temporary buffer
        delete [] tempBuffer;
    }
    
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
    {
        if (targetPane == childPaneList[loop])
        {
            // Remove the target pane from the child list by sliding the
	    // other children down over the removed one
            for (sloop = loop; sloop < (childPaneCount-1); sloop++)
            {
                childPaneList[sloop] = childPaneList[sloop+1];
            }

            // One fewer child
            childPaneCount--;

            // Unreference the pane
            targetPane->unref();

            // Remove the pane's pfChannel object from this window's
	    // pfPipeWindow object
            performerPipeWindow->removeChan(targetPane->getBaseLibraryObject());
            return;
        }
    }

    printf("vsWindow::removePane: Specified pane not part of window\n");
}

// ------------------------------------------------------------------------
// VESS internal function - Performer callback
// This is the Config function for a Pbuffer. It is called once during the
// first draw process because this is the only time the necessary OpenGL
// calls for creating a pbuffer can be made.
// ------------------------------------------------------------------------
void vsWindow::InitPbuffer(pfPipeWindow *pipeWindow)
{
    pfWSConnection    display;
    GLXFBConfig       *configList;
    GLXPbuffer        pBuffer;
    GLXContext        glContext;
    int               screenIndex;
    int               width;
    int               height;
    int               configCount;

    // Default frame buffer configuration
    int frameBufferAttributes[20] =
    {
        GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 8,
        GLX_DOUBLEBUFFER, 1,
        0
    };

    // pBuffer configuration: This will create a pBuffer of the requested
    // width and height. Its contents are preserved, meaning images held
    // should survive screen modifications. Also this will not create the
    // largest available pBuffer if there is not enough memory; it will
    // give an invalid context instead.
    int pBufferAttributes[10] =
    {
        GLX_PBUFFER_WIDTH, 0,
        GLX_PBUFFER_HEIGHT, 0,
        GLX_LARGEST_PBUFFER, GL_TRUE,
        GLX_PRESERVED_CONTENTS, GL_FALSE
    };

    // Grab the display and screen index from Performer
    display = pfGetCurWSConnection();
    screenIndex = pipeWindow->getScreen();

    // Grab the size of the pfPipeWindow to use for the pBuffer
    pipeWindow->getSize(&width, &height);

    // Set the pBuffer width and height attributes to this width and height
    pBufferAttributes[1] = width;
    pBufferAttributes[3] = height;

    // Get the list of valid framebuffer configurations
    configList = glXChooseFBConfig(display, screenIndex,
        frameBufferAttributes, &configCount);

    // Make sure at least one valid framebuffer configuration was returned
    if (configCount == 0)
    {
        pfNotify(PFNFY_WARN, PFNFY_PRINT, "No valid framebuffer "
            "configurations found!");
    }
    else
    {
        // Give the pfPipeWindow the first valid configuration
        pipeWindow->setFBConfig(configList[0]);

        // Create the pBuffer that we will pass to Performer
        pBuffer = glXCreatePbuffer(display, configList[0], pBufferAttributes);

        // Create the rendering context for the pBuffer
        glContext = glXCreateNewContext(display, configList[0], GLX_RGBA_TYPE,
            NULL, GL_TRUE);
 
        // Tell the pfPipeWindow to use our pBuffer as its drawable
        pipeWindow->setWSDrawable(display, pBuffer);

        // Tell the pfPipeWindow to use our GLXContext
        pipeWindow->setGLCxt(glContext);

        // Free the memory used for the config list
        XFree(configList);

        // Open the window
        pipeWindow->open();
    }
}

// ------------------------------------------------------------------------
// static VESS internal function - Performer callback
// Post-DRAW callback to read the pixels from the current frame and save
// them into shared memory where they can be retrieved later. This callback
// is added to the performer callback list when the user needs to save an
// image, and deletes itself so that the image is only saved once.
// ------------------------------------------------------------------------
void vsWindow::saveImage(pfChannel *chan, void *userData)
{
    pfPipeWindow      *pipeWindow;
    pfGLContext       pipeWindowContext;
    pfWSConnection    display;
    pfWSDrawable      pBuffer;
    unsigned int      width;
    unsigned int      height;
    int               numChans;
    int               chanIndex;

    // Retrieve the parent pipe window from the channel
    pipeWindow = chan->getPWin();

    // Find the number of channels on the parent pipe window
    numChans = pipeWindow->getNumChans();

    // Find the index of this channel inside the parent pipe window
    chanIndex = pipeWindow->getChanIndex(chan);

    // We only want to save the image if this is the last channel, because
    // this ensures that all of the other channels have already been drawn
    if (numChans - 1 == chanIndex)
    {
        // Get the current window system connection from Performer
        display = pfGetCurWSConnection();

        // Grab the pBuffer drawable surface from the pipe window
        pBuffer = pipeWindow->getCurWSDrawable();

        // Get the dimensions of the drawable
        glXQueryDrawable(display, pBuffer, GLX_WIDTH, &width);
        glXQueryDrawable(display, pBuffer, GLX_HEIGHT, &height);

        // Get the GL context of the pipe window
        pipeWindowContext = pipeWindow->getGLCxt();

        // Make the pipe window context current
        glXMakeCurrent(display, pBuffer, pipeWindowContext);

        // Read the image from the pBuffer into the shared user data
        glPixelStorei( GL_PACK_ALIGNMENT, 1 );
        glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE,
            (GLvoid *)vsCallbackList::getData(userData));

        // Tell the callback node to delete itself from the list
        vsCallbackList::nodeRemove(userData);
    }
}

