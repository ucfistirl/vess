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
    if (index >= screenCount)
	return NULL;

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
    if ((index < 0) || (index >= childWindowCount))
    {
        printf("vsScreen::getChildWindow: Index out of bounds\n");
        return NULL;
    }

    return (vsWindow *)(childWindowList[index]);
}

// ------------------------------------------------------------------------
// Retrieves the pixel size of this screen. NULL pointers may be passed in
// for undesired data values.
// ------------------------------------------------------------------------
void vsScreen::getScreenSize(int *width, int *height)
{
    int x, y;
    
    parentPipe->getBaseLibraryObject()->getSize(&x, &y);
    
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
    childWindowCount = 0;

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

    screenCount = vsPipe::getPipeCount();

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
    
    for (loop = 0; loop < childWindowCount; loop++)
        if (targetWindow == childWindowList[loop])
        {
            for (sloop = loop; sloop < (childWindowCount-1); sloop++)
                childWindowList[sloop] = childWindowList[sloop+1];
            childWindowCount--;
            return;
        }

    printf("vsScreen::removeWindow: Specified window not part of screen\n");
}