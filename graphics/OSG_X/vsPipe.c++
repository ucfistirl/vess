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
    // Make sure the index doesn't exceed the number of pipes
    if (index >= pipeCount)
	return NULL;

    // Return the requested pipe
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
    // Make sure the screen index is valid
    if (index != 0)
        printf("vsPipe::getScreen: Bad screen index\n");

    // Return the requested screen
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
    // Remember the display and pipe index
    xDisplay = display;
    pipeIndex = index;
}

// ------------------------------------------------------------------------
// Private destructor
// ------------------------------------------------------------------------
vsPipe::~vsPipe()
{
    // Decrement the pipe count
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
    display = XOpenDisplay(NULL);
    pipeCount = ScreenCount(display);

    // Create a vsPipe for each screen
    for (loop = 0; loop < pipeCount; loop++)
    {
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

    // Set the pipe count to 0
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
