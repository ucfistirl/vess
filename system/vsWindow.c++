// File vsWindow.c++

#include "vsWindow.h++"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

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
    
    while (!(performerPipeWindow->isOpen()))
        pfFrame();

    // * Obtain the size of the window manager's border for our outer window
    xWindowDisplay = pfGetCurWSConnection();
    xWindowID = performerPipeWindow->getWSWindow();

    XQueryTree(xWindowDisplay, xWindowID, &rootID, &parentID, &childPointer,
        &childCount);
    XFree(childPointer);
    while (parentID == 0)
    {
        XFlush(xWindowDisplay);
        XQueryTree(xWindowDisplay, xWindowID, &rootID, &parentID,
            &childPointer, &childCount);
        XFree(childPointer);
    }

    XQueryTree(xWindowDisplay, parentID, &rootID, &topWindowID,
        &childPointer, &childCount);
    XFree(childPointer);
    while (topWindowID == 0)
    {
        XFlush(xWindowDisplay);
        XQueryTree(xWindowDisplay, parentID, &rootID, &topWindowID,
            &childPointer, &childCount);
        XFree(childPointer);
    }

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
            return;
        }

    printf("vsWindow::removePane: Specified pane not part of window\n");
}
