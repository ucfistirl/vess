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
//    VESS Module:  vsScreen.c++
//
//    Description:  Class that represents a physical display device
//                  attached to a computer. Objects of this class should
//                  not be instantiated directly by the user but should
//                  instead be retrieved from the active vsSystem object.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsScreen.h++"

vsScreen *vsScreen::screenList[VS_MAX_SCREEN_COUNT];
int vsScreen::screenCount = 0;

// ------------------------------------------------------------------------
// Static function
// Retrieves the specified vsPipe object from the list
// ------------------------------------------------------------------------
vsScreen *vsScreen::getScreen(int index)
{
    // Make sure the index doesn't exceed the screen count
    if (index >= screenCount)
	return NULL;

    // Return the requested screen
    return screenList[index];
}

// ------------------------------------------------------------------------
// Static function
// Returns the number of currently available vsScreen objects
// ------------------------------------------------------------------------
int vsScreen::getScreenCount()
{
    return screenCount;
}

// ------------------------------------------------------------------------
// Retrieves the parent pipe object of this screen
// ------------------------------------------------------------------------
vsPipe *vsScreen::getParentPipe()
{
    return parentPipe;
}

// ------------------------------------------------------------------------
// Retrieves the number of child windows attached to this screen
// ------------------------------------------------------------------------
int vsScreen::getChildWindowCount()
{
    return childWindowCount;
}

// ------------------------------------------------------------------------
// Retrieves one of the windows on this screen, specified by index. The
// index of the first window is 0.
// ------------------------------------------------------------------------
vsWindow *vsScreen::getChildWindow(int index)
{
    // Make sure the index is valid
    if ((index < 0) || (index >= childWindowCount))
    {
        printf("vsScreen::getChildWindow: Index out of bounds\n");
        return NULL;
    }

    // Return the requested window
    return (vsWindow *)(childWindowList[index]);
}

// ------------------------------------------------------------------------
// Retrieves the pixel size of this screen. NULL pointers may be passed in
// for undesired data values.
// ------------------------------------------------------------------------
void vsScreen::getScreenSize(int *width, int *height)
{
    int x, y;
    
    // Ask the pfPipe for its screen size
    parentPipe->getBaseLibraryObject()->getSize(&x, &y);
    
    // Return the width and height, if requested
    if (width)
        *width = x;
    if (height)
        *height = y;
}

// ------------------------------------------------------------------------
// Private constructor
// Initializes the child window list and sets this object as a child of its
// parent pipe
// ------------------------------------------------------------------------
vsScreen::vsScreen(vsPipe *parent) : childWindowList(1, 1)
{
    // Start with no windows
    childWindowCount = 0;

    // Get the parent pipe and set this object as its screen
    parentPipe = parent;
    parentPipe->setScreen(this);
}

// ------------------------------------------------------------------------
// Private destructor
// Deletes all child windows of this screen
// ------------------------------------------------------------------------
vsScreen::~vsScreen()
{
    // Delete all child windows
    // The vsWindow destructor includes a call to the parent vsScreen (this)
    // to remove it from the window list. Keep deleting vsWindows and
    // eventually the list will go away by itself.
    while (childWindowCount > 0)
        delete ((vsWindow *)(childWindowList[0]));
}

// ------------------------------------------------------------------------
// Static internal function
// Creates vsScreen objects and puts them in the static class list. The
// vsPipe::init() function should be called before this one is.
// ------------------------------------------------------------------------
void vsScreen::init()
{
    int loop;

    // Screen count is the same as the pipe count
    screenCount = vsPipe::getPipeCount();

    // Create a vsScreen for each vsPipe
    for (loop = 0; loop < screenCount; loop++)
	screenList[loop] = new vsScreen(vsPipe::getPipe(loop));
}

// ------------------------------------------------------------------------
// Static internal function
// Destroys each vsScreen in the static class list. The vsPipe::done()
// function should be called after this one.
// ------------------------------------------------------------------------
void vsScreen::done()
{
    int loop;

    // Destroy each vsScreen
    for (loop = 0; loop < screenCount; loop++)
        delete ((vsScreen *)(screenList[loop]));

    screenCount = 0;
}

// ------------------------------------------------------------------------
// VESS internal function
// Adds the specified window to this screen's list of child windows
// ------------------------------------------------------------------------
void vsScreen::addWindow(vsWindow *newWindow)
{
    // Add window to screen's internal list
    childWindowList[childWindowCount++] = newWindow;
}

// ------------------------------------------------------------------------
// VESS internal function
// Removes the specified window from this screen's list of child windows
// ------------------------------------------------------------------------
void vsScreen::removeWindow(vsWindow *targetWindow)
{
    // Remove window from screen's internal list
    int loop, sloop;
    
    // Search the window list for the target window
    for (loop = 0; loop < childWindowCount; loop++)
    {
        // Check if this is the window we're looking for
        if (targetWindow == childWindowList[loop])
        {
            // Found it, slide each remaining window down one space in
            // the list
            for (sloop = loop; sloop < (childWindowCount-1); sloop++)
                childWindowList[sloop] = childWindowList[sloop+1];

            // Decrement the window count
            childWindowCount--;
            return;
        }
    }

    // Complain if we couldn't find the window
    printf("vsScreen::removeWindow: Specified window not part of screen\n");
}
