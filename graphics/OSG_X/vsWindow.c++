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
#include <sys/time.h>
#include <osg/Image>
#include <osgDB/WriteFile>
#include <string>
#include "vsWindow.h++"
#include <Xm/MwmUtil.h>

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
    Display *xWindowDisplay;
    XVisualInfo *visual;
    Window xWindowID;
    Window *childPointer;
    unsigned int childCount;
    Window parentID, rootID;
    XWindowAttributes winXAttr, topXAttr;
    Colormap colorMap;
    PropMotifWmHints motifHints;
    Atom property, propertyType;
    XSetWindowAttributes setWinAttrs;
    XEvent event;
    int result;

    // Default frame buffer configuration
    static int attributeList[20] =
    {
        GLX_RGBA,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 1,
        GLX_DOUBLEBUFFER,
        0
    };

    // If stereo is requested, add that to the configuration attribute list
    if (stereo)
    {
        attributeList[12] = GLX_STEREO;
        attributeList[13] = 0;
    }
    
    // Initialize the pane count
    childPaneCount = 0;
    
    // Assign this window an index and increment the window count.
    // Note: this procedure may need to be protected for thread-safeness 
    // if OSG becomes multi-threaded.
    windowNumber = windowCount++;
    
    // Save the parent screen and get the parent pipe object from the screen
    parentScreen = parent;
    parentPipe = parentScreen->getParentPipe();
    
    // Get the X display connection from the parent pipe
    xWindowDisplay = parentPipe->getXDisplay();
                   
    // Choose an XVisual most closely matching the default attributes
    visual = glXChooseVisual(xWindowDisplay, parentScreen->getScreenIndex(), 
        attributeList);

    // Make sure the visual is valid
    if (visual == NULL)
    {
        // Invalid visual, print an error
        printf("vsWindow::vsWindow: Unable to choose an appropriate frame-"
               "buffer configuration!\n");

        // Flag this window object as not valid
        validObject = false;

        // Bail out
        return;
    }

    // Create an OpenGL rendering context using direct rendering
    glContext = glXCreateContext(xWindowDisplay, visual, NULL, GL_TRUE);

    // Make sure the context is valid
    if (glContext == NULL)
    {
        // Invalid context, print an error
        printf("vsWindow::vsWindow:  Unable to create an OpenGL context!\n");

        // Flag this window object as invalid
        validObject = false;

        // Bail out
        return;
    }

    // Create a color map for the window
    colorMap = XCreateColormap(xWindowDisplay, 
                               RootWindow(xWindowDisplay, visual->screen),
                               visual->visual, AllocNone);

    // Make sure the colormap is valid
    if (colorMap == 0)
    {
        // Invalid colormap, print an error
        printf("vsWindow::vsWindow:  Unable to create colormap for visual!\n");

        // Signal this window object is not valid
        validObject = false;

        // Bail out
        return;
    }

    // Create the window
    setWinAttrs.colormap = colorMap;
    setWinAttrs.border_pixel = 0;
    setWinAttrs.event_mask = StructureNotifyMask;
    xWindow = XCreateWindow(xWindowDisplay, RootWindow(xWindowDisplay, 
                            visual->screen), VS_WINDOW_DEFAULT_XPOS, 
                            VS_WINDOW_DEFAULT_YPOS, VS_WINDOW_DEFAULT_WIDTH, 
                            VS_WINDOW_DEFAULT_HEIGHT, 0, visual->depth, 
                            InputOutput, visual->visual, 
                            CWBorderPixel|CWColormap|CWEventMask, 
                            &setWinAttrs);

    // Make sure the X window is valid
    if (xWindow == 0)
    {
        // Print an error
        printf("vsWindow::vsWindow:  Unable to create X Window!\n");

        // Signal that this window object is not valid
        validObject = false;

        // Bail out
        return;
    }

    // Make the border hidden if requested
    if (hideBorder)
    {
        // Use the Motif interface for hiding decorations.  Most modern
        // window managers honor this request.

        // Get the atom for the decorations property.  The last argument
        // specifies that we only want the property if it exists (i.e.:
        // don't create it).
        property = XInternAtom(xWindowDisplay, "_MOTIF_WM_HINTS", True);
        if (property == 0)
        {
            // The window manager doesn't support this property
            printf("vsWindow::vsWindow:  No window manager support for ");
            printf("decoration hints.\n");
            printf("                     Unable to hide the window border.\n");
        }
        else
        {
            // Set up the property, zero in the decorations field means
            // no decorations
            motifHints.flags = MWM_HINTS_DECORATIONS;
            motifHints.decorations = 0;

            // Change the property
            propertyType = property;
            XChangeProperty(xWindowDisplay, xWindow, property, propertyType,
                            sizeof(unsigned long) * 8, PropModeReplace, 
                            (unsigned char *) &motifHints,
                            PROP_MOTIF_WM_HINTS_ELEMENTS);
        }
        
    }

    // Map (ie: open) the window and wait for it to finish mapping
    XMapWindow(xWindowDisplay, xWindow);
    XIfEvent(xWindowDisplay, &event, vsWindow::waitForMap, (char *)xWindow);

    // Add the window to its parent screen
    parentScreen->addWindow(this);

    // For some reason (probably window manager interaction), the window
    // does not seem to appear at the position it was supposed to be 
    // created.  To fix this, we'll move it there and flush the display
    // to make sure it happens.
    setPosition(VS_WINDOW_DEFAULT_XPOS, VS_WINDOW_DEFAULT_YPOS);
    XFlush(xWindowDisplay);

    // After mapping the window, the window manager may reparent the
    // window to add its own stuff (decorations, etc.).  Query the X
    // Windows tree attached to this window to find the topmost window
    // in the tree.  This should let us measure the size of the window
    // manager decorations.

    // Start from the window we just mapped
    xWindowID = xWindow;

    // Keep trying until we reach the top window
    do
    {
        // Query the tree from the current window
        result = XQueryTree(xWindowDisplay, xWindowID, &rootID, &parentID,
            &childPointer, &childCount);

        // Free the child list that's returned (we don't need it for anything)
        XFree(childPointer);

        // See if the query succeeded
        if (result == 0)
        {
            // Failed, flush the display and try again
            XFlush(xWindowDisplay);
        }
        else
        {
            // Query succeeded, if we're not yet at the top, move the
            // current window id to the parent and query again.  Note that
            // we don't want the root window, because this is the entire
            // desktop.  We want the window one level down from the root.
            if (parentID != rootID)
                xWindowID = parentID;
        }
    }
    while (rootID != parentID);

    // Keep track of the topmost window
    topWindowID = xWindowID;

    // See if the window was reparented
    if (xWindow != topWindowID)
    {
        // Attempt to determine the size of the window manager's border for
        // this window by checking the position of the main window relative
        // to its parent, and finding the difference in width and height.
        XGetWindowAttributes(xWindowDisplay, xWindow, &winXAttr);
        XGetWindowAttributes(xWindowDisplay, topWindowID, &topXAttr);
        xPositionOffset = winXAttr.x;
        yPositionOffset = winXAttr.y;
        widthOffset = topXAttr.width - winXAttr.width;
        heightOffset = topXAttr.height - winXAttr.height;

        // Adjust the window using the offsets we computed
        setPosition(VS_WINDOW_DEFAULT_XPOS, VS_WINDOW_DEFAULT_YPOS);
        setSize(VS_WINDOW_DEFAULT_WIDTH, VS_WINDOW_DEFAULT_HEIGHT);
        XFlush(xWindowDisplay);
    }
    else
    {
        // Window was not reparented, initialize the offsets to zero
        xPositionOffset = 0;
        yPositionOffset = 0;
        widthOffset = 0;
        heightOffset = 0;
    }
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
    Display *xWindowDisplay;
    XVisualInfo *visual;
    Window xWindowID;
    Window *childPointer;
    unsigned int childCount;
    Window parentID, rootID;
    XWindowAttributes winXAttr, topXAttr;
    Colormap colorMap;
    PropMotifWmHints motifHints;
    Atom property, propertyType;
    XSetWindowAttributes setWinAttrs;
    XEvent event;
    int result;

    // Default attributes
    static int attributeList[20] =
    {
        GLX_RGBA,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 1,
        GLX_DOUBLEBUFFER,
        0
    };

    // If stereo is requested, add that to the attribute list
    if (stereo)
    {
        attributeList[12] = GLX_STEREO;
        attributeList[13] = 0;
    }
    
    // Initialize the pane count
    childPaneCount = 0;
    
    // Assign this window an index and increment the window count.
    // Note: this procedure may need to be protected for thread-safeness 
    // if OSG becomes multi-threaded.
    windowNumber = windowCount++;
    
    // Get the parent screen
    parentScreen = parent;

    // Get the parent pipe from the screen
    parentPipe = parentScreen->getParentPipe();
    
    // Get the X display connection from the pipe
    xWindowDisplay = parentPipe->getXDisplay();
                   
    // Choose an XVisual most closely matching the default attributes
    visual = glXChooseVisual(xWindowDisplay, parentScreen->getScreenIndex(), 
        attributeList);

    // Make sure the visual is valid
    if (visual == NULL)
    {
        // Invalid visual, print an error
        printf("vsWindow::vsWindow: Unable to choose an appropriate frame-"
               "buffer configuration!\n");

        // Flag this window object as invalid
        validObject = false;

        // Bail out
        return;
    }

    // Create an OpenGL rendering context using direct rendering
    glContext = glXCreateContext(xWindowDisplay, visual, NULL, GL_TRUE);

    // Make sure the context is valid
    if (glContext == NULL)
    {
        // Invalid context, print an error
        printf("vsWindow::vsWindow:  Unable to create an OpenGL context!\n");

        // Flag this object as invalid
        validObject = false;

        // Bail out
        return;
    }

    // Create a color map for the window
    colorMap = XCreateColormap(xWindowDisplay, 
                               RootWindow(xWindowDisplay, visual->screen),
                               visual->visual, AllocNone);

    // Make sure the colormap is valid
    if (colorMap == 0)
    {
        // Invalid context, print an error
        printf("vsWindow::vsWindow:  Unable to create colormap for visual!\n");

        // Signal this window object is not valid
        validObject = false;

        // Bail out
        return;
    }

    // Create the window
    setWinAttrs.colormap = colorMap;
    setWinAttrs.border_pixel = 0;
    setWinAttrs.event_mask = StructureNotifyMask;
    xWindow = XCreateWindow(xWindowDisplay, RootWindow(xWindowDisplay, 
                            visual->screen), x, y, width, height, 0, 
                            visual->depth, InputOutput, visual->visual, 
                            CWBorderPixel|CWColormap|CWEventMask, 
                            &setWinAttrs);

    // Make sure the X window is valid
    if (xWindow == 0)
    {
        // Print an error
        printf("vsWindow::vsWindow:  Unable to create X Window!\n");

        // Signal that this window object is not valid
        validObject = false;

        // Bail out
        return;
    }

    // Make the border hidden if requested
    if (hideBorder)
    {
        // Use the Motif interface for hiding decorations.  Most modern
        // window managers honor this request.

        // Get the atom for the decorations property.  The last argument
        // specifies that we only want the property if it exists (i.e.:
        // don't create it).
        property = XInternAtom(xWindowDisplay, "_MOTIF_WM_HINTS", True);
        if (property == 0)
        {
            // The window manager doesn't support this property
            printf("vsWindow::vsWindow:  No window manager support for ");
            printf("decoration hints.\n");
            printf("                     Unable to hide the window border.\n");
        }
        else
        {
            // Set up the property, zero in the decorations field means
            // no decorations
            motifHints.flags = MWM_HINTS_DECORATIONS;
            motifHints.decorations = 0;

            // Change the property
            propertyType = property;
            XChangeProperty(xWindowDisplay, xWindow, property, propertyType,
                            sizeof(unsigned long) * 8, PropModeReplace, 
                            (unsigned char *) &motifHints,
                            PROP_MOTIF_WM_HINTS_ELEMENTS);
        }
        
    }

    // Map (ie: open) the window and wait for it to finish mapping
    XMapWindow(xWindowDisplay, xWindow);
    XIfEvent(xWindowDisplay, &event, vsWindow::waitForMap, (char *)xWindow);

    // Add the window to its parent screen
    parentScreen->addWindow(this);

    // For some reason (probably window manager interaction), the window
    // does not seem to appear at the position it was supposed to be 
    // created.  To fix this, we'll move it there and flush the display
    // to make sure it happens.
    setPosition(x, y);
    XFlush(xWindowDisplay);

    // After mapping the window, the window manager may reparent the
    // window to add its own stuff (decorations, etc.).  Query the X
    // Windows tree attached to this window to find the topmost window
    // in the tree.  This should let us measure the size of the window
    // manager decorations.

    // Start from the window we just mapped
    xWindowID = xWindow;

    // Keep trying until we reach the top window
    do
    {
        // Query the tree from the current window
        result = XQueryTree(xWindowDisplay, xWindowID, &rootID, &parentID,
            &childPointer, &childCount);

        // Free the child list that's returned (we don't need it for anything)
        XFree(childPointer);

        // See if the query succeeded
        if (result == 0)
        {
            // Failed, flush the display and try again
            XFlush(xWindowDisplay);
        }
        else
        {
            // Query succeeded, if we're not yet at the top, move the
            // current window id to the parent and query again.  Note that
            // we don't want the root window, because this is the entire
            // desktop.  We want the window one level down from the root.
            if (parentID != rootID)
                xWindowID = parentID;
        }
    }
    while (rootID != parentID);

    // Keep track of the topmost window
    topWindowID = xWindowID;

    // See if the window was reparented
    if (xWindow != topWindowID)
    {
        // Attempt to determine the size of the window manager's border for
        // this window by checking the position of the main window relative
        // to its parent, and finding the difference in width and height.
        XGetWindowAttributes(xWindowDisplay, xWindow, &winXAttr);
        XGetWindowAttributes(xWindowDisplay, topWindowID, &topXAttr);
        xPositionOffset = x - winXAttr.x;
        yPositionOffset = y - winXAttr.y;
        widthOffset = topXAttr.width - width;
        heightOffset = topXAttr.height - height;

        // Adjust the window using the offsets we computed
        setPosition(x, y);
        setSize(width, height);
        XFlush(xWindowDisplay);
    }
    else
    {
        // Window was not reparented, initialize the offsets to zero
        xPositionOffset = 0;
        yPositionOffset = 0;
        widthOffset = 0;
        heightOffset = 0;
    }
}

// ------------------------------------------------------------------------
// Constructor - Initializes the window by making use of the existing X
// Window passed in.
// ------------------------------------------------------------------------
vsWindow::vsWindow(vsScreen *parent, Window xWin) : childPaneList(1, 1)
{
    vsPipe *parentPipe;
    Display *xWindowDisplay;
    XWindowAttributes winXAttr, topXAttr;
    VisualID visualID;
    XVisualInfo visualTemplate;
    int visualsMatched;
    XVisualInfo *visualList;
    Window xWindowID;
    Window *childPointer;
    unsigned int childCount;
    Window parentID, rootID;
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

    // Initialize the pane count
    childPaneCount = 0;

    // Assign this window an index and increment the window count.
    // Note: this procedure may need to be protected for thread-safeness 
    // if OSG becomes multi-threaded.
    windowNumber = windowCount++;
    
    // Get the parent vsScreen and vsPipe
    parentScreen = parent;
    parentPipe = parentScreen->getParentPipe();

    // Remember the window ID
    xWindow = xWin;
    
    // Get the X display from the pipe
    xWindowDisplay = parentPipe->getXDisplay();

    // Add the window to its parent screen
    parentScreen->addWindow(this);

    // Determine the visual characteristics of the window.  First, get
    // the window's attributes
    XGetWindowAttributes(xWindowDisplay, xWindow, &winXAttr);

    // Get the Visual's ID from the window
    visualID = XVisualIDFromVisual(winXAttr.visual);

    // Now, find the XVisualInfo structure that matches the window's visual
    // ID
    visualTemplate.visualid = visualID;
    visualList = XGetVisualInfo(xWindowDisplay, VisualIDMask, &visualTemplate, 
        &visualsMatched);

    // If we can't match the visual, then we can't create an OpenGL context 
    // for the window, so give up.
    if (visualsMatched <= 0)
    {
        // Print an error
        printf("vsWindow::vsWindow:  Unable to match visual ID of the given X "
            "Window!\n");

        // Flag this object as invalid
        validObject = false;

        // Bail out
        return;
    }
    else
    {
        // Print status
        printf("X Window has VisualID 0x%x\n", visualID);
        printf("Matching %d visual(s), choosing visual 0x%x\n", visualsMatched,
            visualList[0].visualid);
    }

    // Create an OpenGL rendering context using direct rendering.  Assume
    // the first visual matched is the one we want, since we queried by
    // exact ID number.
    glContext = glXCreateContext(xWindowDisplay, &(visualList[0]), NULL, 
        GL_TRUE);

    // At this point, we shouldn't need the visual list anymore; get rid of it.
    XFree(visualList);

    // Make sure the context is valid
    if (glContext == NULL)
    {
        // Invalid context, print an error
        printf("vsWindow::vsWindow:  Unable to create an OpenGL context!\n");

        // Flag this object as invalid
        validObject = false;

        // Bail out
        return;
    }

    // After mapping the window, the window manager may reparent the
    // window to add its own stuff (decorations, etc.).  Query the X
    // Windows tree attached to this window to find the topmost window
    // in the tree.  This should let us measure the size of the window
    // manager decorations.  Note that if the window is not yet mapped,
    // there will not be any decorations added yet.

    // Start from the window passed in
    xWindowID = xWin;

    // Keep trying until we reach the top window
    do
    {
        // Query the tree from the current window
        result = XQueryTree(xWindowDisplay, xWindowID, &rootID, &parentID,
            &childPointer, &childCount);

        // Free the child list that's returned (we don't need it for 
        // anything)
        XFree(childPointer);

        // See if the query succeeded
        if (result == 0)
        {
            // Failed, flush the display and try again
            XFlush(xWindowDisplay);
        }
        else 
        {
            // Query succeeded, if we're not yet at the top, move the
            // current window id to the parent and query again.  Note that
            // we don't want the root window, because this is the entire
            // desktop.  We want the window one level down from the root.
            if (parentID != rootID)
                xWindowID = parentID;
        }
    }
    while (rootID != parentID);

    // Keep track of the topmost window
    topWindowID = xWindowID;

    // Flush the display to ensure every event has been processed before
    // we take our measurements
    XFlush(xWindowDisplay);

    // See if the window was reparented
    if (xWin != topWindowID)
    {
        // Attempt to determine the size of the window manager's border for
        // this window by checking the position of the main window relative
        // to its parent, and finding the difference in width and height.
        XGetWindowAttributes(xWindowDisplay, xWindow, &winXAttr);
        XGetWindowAttributes(xWindowDisplay, topWindowID, &topXAttr);
        xPositionOffset = winXAttr.x;
        yPositionOffset = winXAttr.y;
        widthOffset = topXAttr.width - winXAttr.width;
        heightOffset = topXAttr.height - winXAttr.height;
    }
    else
    {
        // Window was not reparented, initialize the offsets to zero
        xPositionOffset = 0;
        yPositionOffset = 0;
        widthOffset = 0;
        heightOffset = 0;
    }
}

// ------------------------------------------------------------------------
// Destructor - Deletes any child panes that this window owns
// ------------------------------------------------------------------------
vsWindow::~vsWindow()
{
    Display *display;

    display = parentScreen->getParentPipe()->getXDisplay();

    // Delete all child panes
    // The vsPane destructor includes a call to the parent vsWindow (this)
    // to remove it from the pane list. Keep deleting vsPanes and eventually
    // the list will go away by itself.
    while (childPaneCount > 0)
        delete ((vsPane *)(childPaneList[0]));
    
    // Remove the window from its screen
    parentScreen->removeWindow(this);

    // Destroy the window
    glXDestroyContext(display, glContext);
    XDestroyWindow(display, xWindow);
}
    
// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsWindow::getClassName()
{
    return "vsWindow";
}

// ------------------------------------------------------------------------
// Local callback function to indicate when the window has been opened
// ------------------------------------------------------------------------
Bool vsWindow::waitForMap(Display *display, XEvent *event, char *arg)
{
    // True if the event is a window map notification and the window
    // being mapped is the window passed in as the argument
    return (event->type == MapNotify) && (event->xmap.window == (Window)arg);
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
    Display *xWindowDisplay;

    // Obtain the X Display object
    xWindowDisplay = parentScreen->getParentPipe()->getXDisplay();

    // Send the request for X to resize the window
    XResizeWindow(xWindowDisplay, xWindow, width - widthOffset, 
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
    
    // Get the X display connection from the screen's parent pipe
    xWindowDisplay = parentScreen->getParentPipe()->getXDisplay();

    // Query the window attributes from X, and make sure the query
    // succeeds
    if (XGetWindowAttributes(xWindowDisplay, topWindowID, &xattr) == 0)
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
// Returns the size of the drawable area of this window.  This will be
// the same as the window size if there are no borders or decorations.
// ------------------------------------------------------------------------
void vsWindow::getDrawableSize(int *width, int *height)
{
    Display *xWindowDisplay;
    XWindowAttributes xattr;
    int x, y;
    
    // Get the X display connection from the screen's parent pipe
    xWindowDisplay = parentScreen->getParentPipe()->getXDisplay();

    // Query the window attributes from X, and make sure the query
    // succeeds
    if (XGetWindowAttributes(xWindowDisplay, xWindow, &xattr) == 0)
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

    // Obtain the X Display object
    xWindowDisplay = parentScreen->getParentPipe()->getXDisplay();

    // Send the request for X to reposition the window
    //XMoveWindow(xWindowDisplay, xWindow, xPos + xPositionOffset, 
    //    yPos + yPositionOffset);
    XMoveWindow(xWindowDisplay, xWindow, xPos, yPos);
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

    // Get the X display connection from the screen's parent pipe
    xWindowDisplay = parentScreen->getParentPipe()->getXDisplay();

    // Query the window attributes from X, and make sure the query
    // succeeds
    if (XGetWindowAttributes(xWindowDisplay, topWindowID, &xattr) == 0)
    {
        // Query failed, return zeroes as default
        x = 0;
        y = 0;
    }
    else
    {
        // Get the window position from the attributes structure
        x = xattr.x;
        y = xattr.y;
    }

    // Return the X position if requested
    if (xPos)
        *xPos = x;

    // Return the Y position if requested
    if (yPos)
        *yPos = y;
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
    XTextProperty nameProperty;
    Display *xWindowDisplay;

    // Obtain the X Display and Window objects for this window
    xWindowDisplay = parentScreen->getParentPipe()->getXDisplay();

    // Call the X window manager to display the new name of the window
    XStringListToTextProperty(&newName, 1, &nameProperty);
    XSetWMName(xWindowDisplay, xWindow, &nameProperty);
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
// Returns the Performer object associated with this object
// ------------------------------------------------------------------------
Window vsWindow::getBaseLibraryObject()
{
    return xWindow;
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
    Bool result;

    // Try to make this window's GLX context current
    result = glXMakeCurrent(parentScreen->getParentPipe()->getXDisplay(), 
        xWindow, glContext);

    // Print an error if the makeCurrent failed
    if (!result)
    {
        printf("vsWindow::makeCurrent:  Unable to attach OpenGL context to "
            "window!\n");
    }
}

// ------------------------------------------------------------------------
// Internal function
// Swaps the drawing buffers on this window
// ------------------------------------------------------------------------
void vsWindow::swapBuffers()
{
    // Call GLX to swap the buffers on the X Window
    glXSwapBuffers(parentScreen->getParentPipe()->getXDisplay(), xWindow);
}

// ------------------------------------------------------------------------
// Internal function
// Processes X events on this window
// ------------------------------------------------------------------------
void vsWindow::update()
{
    XEvent event;
    int    i;

    // Check for X events on this window
    while (XCheckWindowEvent(parentScreen->getParentPipe()->getXDisplay(), 
        xWindow, StructureNotifyMask, &event))
    {
        // Got an event, check the type
        switch (event.type)
        {
            case ConfigureNotify:
                // Resize each pane to match the new window dimensions
                for (i = 0; i < childPaneCount; i++)
                    ((vsPane *)childPaneList[i])->resize();
                break;
        }
    }
}
