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
//                  but should instead be retrieved from the active
//                  vsSystem object.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsPipe.h++"

vsPipe *vsPipe::pipeList[VS_MAX_PIPE_COUNT];
int vsPipe::pipeCount = 0;

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsPipe::getClassName()
{
    return "vsPipe";
}

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
    // For now, pipes can only have one screen; signal an error if the
    // caller is requesting something else
    if (index != 0)
        printf("vsPipe::getScreen: Bad screen index\n");

    return childScreen;
}

// ------------------------------------------------------------------------
// Returns the Performer object associated with this object
// ------------------------------------------------------------------------
pfPipe *vsPipe::getBaseLibraryObject()
{
    return performerPipe;
}

// ------------------------------------------------------------------------
// Private constructor - Gets and stores the Performer pipe object
// associated with this object
// ------------------------------------------------------------------------
vsPipe::vsPipe(int index)
{
    performerPipe = pfGetPipe(index);
}

// ------------------------------------------------------------------------
// Private destructor
// ------------------------------------------------------------------------
vsPipe::~vsPipe()
{
}

// ------------------------------------------------------------------------
// Static internal function
// Creates vsPipe objects and puts them in the static class list
// ------------------------------------------------------------------------
void vsPipe::init()
{
    pfWSConnection winConnection;
    int loop;

    // Get the Display from Performer.  Performer only supports 1 screen under
    // Windows;
    winConnection = pfGetCurWSConnection();
    pipeCount = 1;

    // Create the a vsPipe for each screen
    for (loop = 0; loop < pipeCount; loop++)
    {
        pipeList[loop] = new vsPipe(loop);
        (pipeList[loop])->ref();
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
        vsObject::unrefDelete(pipeList[loop]);

    pipeCount = 0;
}

// ------------------------------------------------------------------------
// Internal function
// Sets the screen object for this pipe object
// ------------------------------------------------------------------------
void vsPipe::setScreen(vsScreen *newScreen)
{
    childScreen = newScreen;
}
