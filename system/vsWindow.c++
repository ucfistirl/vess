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

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/time.h>

// ------------------------------------------------------------------------
// Constructor - Initializes the window by creating a Performer pipe window
// object and creating connections with that, verifying that the window is
// being properly displayed, recording some size data from the window
// manager, and configuring the window with its default position and size.
// ------------------------------------------------------------------------
vsWindow::vsWindow(vsScreen *parent, int hideBorder) : childPaneList(1, 1)
{
    vsPipe *parentPipe;
    Display *xWindowDisplay;
    Window xWindowID;
    Window *childPointer;
    unsigned int childCount;
    Window parentID, rootID;
    XWindowAttributes xattr;
    int result;
    
    childPaneCount = 0;
    
    parentScreen = parent;
    parentPipe = parentScreen->getParentPipe();
    
    performerPipeWindow = new pfPipeWindow(parentPipe->getBaseLibraryObject());
    performerPipeWindow->ref();
    
    parentScreen->addWindow(this);
    
    performerPipeWindow->setMode(PFWIN_ORIGIN_LL, 0);
    if (hideBorder)
        performerPipeWindow->setMode(PFWIN_NOBORDER, 1);
    performerPipeWindow->setOriginSize(VS_WINDOW_DEFAULT_XPOS,
        VS_WINDOW_DEFAULT_YPOS, VS_WINDOW_DEFAULT_WIDTH,
        VS_WINDOW_DEFAULT_HEIGHT);
    performerPipeWindow->open();

    // Force the window open
    xWindowDisplay = pfGetCurWSConnection();

    while (!(performerPipeWindow->isOpen()))
    {
        pfFrame();
        XFlush(xWindowDisplay);
    }

    // Get the window that Performer thinks is topmost, and then query the
    // X server to determine if that window really is the topmost one.
    xWindowID = performerPipeWindow->getWSWindow();
//    printf("xWindowID(%d)\n", xWindowID);

    do
    {
        result = XQueryTree(xWindowDisplay, xWindowID, &rootID, &parentID,
            &childPointer, &childCount);
        XFree(childPointer);
//        printf("result(%d) rootID(%d) parentID(%d) xWindowID(%d)\n", result,
//            rootID, parentID, xWindowID);

        if (result == 0)
        {
            pfFrame();
            XFlush(xWindowDisplay);
        }
        else if (parentID != rootID)
            xWindowID = parentID;
    }
    while (rootID != parentID);
    topWindowID = xWindowID;
//    printf("topWindowID: %d\n", topWindowID);

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
    
    // Start off with no vsWindowSystem atatched
    currentWS = NULL;
}

// ------------------------------------------------------------------------
// Constructor - Initializes the window by creating a Performer pipe window
// object and creating connections with that, verifying that the window is
// being properly displayed, recording some size data from the window
// manager, and configuring the window with its default position and size.
// Also configures the window's buffer settings to be either mono or stereo
// based on the value of the stereo parameter
// ------------------------------------------------------------------------
vsWindow::vsWindow(vsScreen *parent, int hideBorder, int stereo) 
         : childPaneList(1, 1)
{
    vsPipe *parentPipe;
    Display *xWindowDisplay;
    Window xWindowID;
    Window *childPointer;
    unsigned int childCount;
    Window parentID, rootID;
    XWindowAttributes xattr;
    static int fbConfigAttrs[20];

    int result;
    
    childPaneCount = 0;
    
    parentScreen = parent;
    parentPipe = parentScreen->getParentPipe();
    
    performerPipeWindow = new pfPipeWindow(parentPipe->getBaseLibraryObject());
    performerPipeWindow->ref();
    
    parentScreen->addWindow(this);
    
    performerPipeWindow->setMode(PFWIN_ORIGIN_LL, 0);
    if (hideBorder)
        performerPipeWindow->setMode(PFWIN_NOBORDER, 1);

   if (stereo)
    {
        // Set up a stereo/double-buffered frame buffer
        fbConfigAttrs[0] = PFFB_RGBA;
        fbConfigAttrs[1] = PFFB_DOUBLEBUFFER;
        fbConfigAttrs[2] = PFFB_STEREO;

        fbConfigAttrs[3] = PFFB_DEPTH_SIZE;
        fbConfigAttrs[4] = 1;

        fbConfigAttrs[5] = PFFB_RED_SIZE;
        fbConfigAttrs[6] = 1;

        fbConfigAttrs[7] = PFFB_STENCIL_SIZE;
        fbConfigAttrs[8] = 1;

        fbConfigAttrs[9] = NULL;

        performerPipeWindow->setFBConfigAttrs(fbConfigAttrs);
    }

    performerPipeWindow->setOriginSize(VS_WINDOW_DEFAULT_XPOS,
        VS_WINDOW_DEFAULT_YPOS, VS_WINDOW_DEFAULT_WIDTH,
        VS_WINDOW_DEFAULT_HEIGHT);
    performerPipeWindow->open();

    // Attempt to determine the size of the window manager's border for
    // this window by checking the difference between Performer's idea
    // of the window size and X's one.
    xWindowDisplay = pfGetCurWSConnection();

    while (!(performerPipeWindow->isOpen()))
    {
        pfFrame();
        XFlush(xWindowDisplay);
    }

    // Get the window that Performer thinks is topmost, and then query the
    // X server to determine if that window really is the topmost one.
    xWindowID = performerPipeWindow->getWSWindow();
//    printf("xWindowID(%d)\n", xWindowID);

    do
    {
        result = XQueryTree(xWindowDisplay, xWindowID, &rootID, &parentID,
            &childPointer, &childCount);
        XFree(childPointer);
//        printf("result(%d) rootID(%d) parentID(%d) xWindowID(%d)\n", result,
//            rootID, parentID, xWindowID);

        if (result == 0)
        {
            pfFrame();
            XFlush(xWindowDisplay);
        }
        else if (parentID != rootID)
            xWindowID = parentID;
    }
    while (rootID != parentID);
    topWindowID = xWindowID;
//    printf("topWindowID: %d\n", topWindowID);

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

    // Start off with no vsWindowSystem atatched
    currentWS = NULL;
}

// ------------------------------------------------------------------------
// Destructor - Deletes any child panes that this window owns
// ------------------------------------------------------------------------
vsWindow::~vsWindow()
{
    // Performer bug: pfPipeWindows can't be deleted
    //delete performerPipeWindow;
    performerPipeWindow->unref();
    
    // Delete all child panes
    // The vsPane destructor includes a call to the parent vsWindow (this)
    // to remove it from the pane list. Keep deleting vsPanes and eventually
    // the list will go away by itself.
    while (childPaneCount > 0)
        delete ((vsPane *)(childPaneList[0]));
    
    parentScreen->removeWindow(this);
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
    if ((index < 0) || (index >= childPaneCount))
    {
        printf("vsWindow::getChildPane: Index out of bounds\n");
        return NULL;
    }

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
    
    xWindowDisplay = pfGetCurWSConnection();

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

    if (width)
        *width = x;
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

    xWindowDisplay = pfGetCurWSConnection();

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

    parentScreen->getScreenSize(&screenWidth, &screenHeight);
    
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

    // Get the connections to the X window system and to the drawable region
    // of the window
    xWindowDisplay = pfGetCurWSConnection();
    winDrawable = performerPipeWindow->getWSDrawable();
    
    // Get the size and shape info for the window
    XGetGeometry(xWindowDisplay, winDrawable, &rootWin, &xpos, &ypos,
        &width, &height, &border, &depth);

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
    // pixel data unit. Also construct a lookup table to allow for quick
    // scaling from whatever is stored in the data into the 0-255 range
    // that the RGB format wants.

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
        redVals[loop] = ((loop * 255) / redMax);

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
        greenVals[loop] = ((loop * 255) / greenMax);

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
        blueVals[loop] = ((loop * 255) / blueMax);

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
            pixelData = XGetPixel(image, sloop, loop);

            redPixel = pixelData & redMask;
            redPixel >>= redShift;
            redBuffer[sloop] = redVals[redPixel];

            greenPixel = pixelData & greenMask;
            greenPixel >>= greenShift;
            greenBuffer[sloop] = greenVals[greenPixel];

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

    // Clean up
    iclose(imageOut);

    XDestroyImage(image);
}

// ------------------------------------------------------------------------
// Returns the vsWindowSystem object associated with this window
// ------------------------------------------------------------------------
vsWindowSystem *vsWindow::getWSystem()
{
    return currentWS;
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
    
    for (loop = 0; loop < childPaneCount; loop++)
        if (targetPane == childPaneList[loop])
        {
            for (sloop = loop; sloop < (childPaneCount-1); sloop++)
                childPaneList[sloop] = childPaneList[sloop+1];
            childPaneCount--;
            performerPipeWindow->removeChan(targetPane->getBaseLibraryObject());
            return;
        }

    printf("vsWindow::removePane: Specified pane not part of window\n");
}

// ------------------------------------------------------------------------
// VESS internal function
// Notifies the window that the specified vsWindowSystem is attached
// ------------------------------------------------------------------------
void vsWindow::attachWSystem(vsWindowSystem *wSystem)
{
    if (currentWS)
        printf("vsWindow::attachWSystem:: Window already has a "
            "vsWindowSystem attached\n");
    else
        currentWS = wSystem;
}

// ------------------------------------------------------------------------
// VESS internal function
// Notifies the window that the specified vsWindowSystem is unattached
// ------------------------------------------------------------------------
void vsWindow::removeWSystem()
{
    currentWS = NULL;
}
