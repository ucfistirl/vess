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
//    VESS Module:  vsPipe.c++
//
//    Description:  Class that represents one of the graphics rendering
//                  pipelines available on a computer. Objects of this
//                  class should not be instantiated directly by the user
//                  but should be retrieved from the class using the
//                  static getPipe() method.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsPipe.h++"

vsPipe *vsPipe::pipeList[VS_MAX_PIPE_COUNT];
int vsPipe::pipeCount = 0;

// ------------------------------------------------------------------------
// Static function
// Retrieves the specified vsPipe object from the list
// ------------------------------------------------------------------------
vsPipe *vsPipe::getPipe(int index)
{
    if (index >= pipeCount)
	return NULL;

    return pipeList[index];
}

// ------------------------------------------------------------------------
// Static function
// Returns the number of currently available vsPipe objects
// ------------------------------------------------------------------------
int vsPipe::getPipeCount()
{
    return pipeCount;
}

// ------------------------------------------------------------------------
// Returns one of the child screens of this pipe, selected with index. The
// index of the first screen is 0.
// ------------------------------------------------------------------------
vsScreen *vsPipe::getScreen(int index)
{
    if (index != 0)
        printf("vsPipe::getScreen: Bad screen index\n");

    return childScreen;
}

// ------------------------------------------------------------------------
// Returns the index of the pipe on this display
// ------------------------------------------------------------------------
int vsPipe::getBaseLibraryObject()
{
    return pipeIndex;
}

// ------------------------------------------------------------------------
// Private constructor - Stores the current X Windows display connection
// and the index of this pipe on that display
// ------------------------------------------------------------------------
vsPipe::vsPipe(Display *display, int index)
{
    xDisplay = display;
    pipeIndex = index;
}

// ------------------------------------------------------------------------
// Private destructor
// ------------------------------------------------------------------------
vsPipe::~vsPipe()
{
    pipeCount--;

    // Close the X Display when the last pipe is destroyed;
    if (pipeCount == 0)
        XCloseDisplay(xDisplay);
}

// ------------------------------------------------------------------------
// Static internal function
// Creates vsPipe objects and puts them in the static class list
// ------------------------------------------------------------------------
void vsPipe::init()
{
    Display *display;
    int     loop;

    // Open a connection to the X Server and count the number of screens
    printf("Opening default display:\n");
    display = XOpenDisplay(NULL);
    pipeCount = ScreenCount(display);
    printf("   Display %s (0x%x) contains %d screens\n", 
        DisplayString(display), display, pipeCount);

    // Create a vsPipe for each screen
    for (loop = 0; loop < pipeCount; loop++)
    {
        printf("Creating pipe #%d\n", loop);
	pipeList[loop] = new vsPipe(display, loop);
    }
}

// ------------------------------------------------------------------------
// Static internal function
// Destroys all vsPipe objects in the static class list
// ------------------------------------------------------------------------
void vsPipe::done()
{
    int loop;

    // Destroy each vsPipe
    for (loop = 0; loop < pipeCount; loop++)
	delete ((vsPipe *)(pipeList[loop]));

    pipeCount = 0;
}

// ------------------------------------------------------------------------
// Internal function
// Returns the currently open X Windows display
// ------------------------------------------------------------------------
Display *vsPipe::getXDisplay()
{
    return xDisplay;
}

// ------------------------------------------------------------------------
// Internal function
// Sets the screen object for this pipe object
// ------------------------------------------------------------------------
void vsPipe::setScreen(vsScreen *newScreen)
{
    childScreen = newScreen;
}
