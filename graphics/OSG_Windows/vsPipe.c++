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
//    Author(s):    Bryan Kline, Jason Daly
//
//------------------------------------------------------------------------

#include <stdio.h>
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
// Private constructor, called from the vsSystem object using the init()
// function below.  
// ------------------------------------------------------------------------
vsPipe::vsPipe(int index)
{
    pipeIndex = index;
}

// ------------------------------------------------------------------------
// Private destructor, does nothing.
// ------------------------------------------------------------------------
vsPipe::~vsPipe()
{
}

// ------------------------------------------------------------------------
// Static internal function
// Creates vsPipe objects and puts them in the static class list.  Under
// Windows, only a single vsPipe object is created, as we treat the
// virtual desktop as a single "pipe".
// ------------------------------------------------------------------------
void vsPipe::init()
{
    // Zero the memory in the pipeList
    memset(pipeList, 0, sizeof(pipeList));
    
    // Create the vsPipe in the first list position
    pipeList[0] = new vsPipe(0);
    (pipeList[0])->ref();
    pipeCount = 1;
}

// ------------------------------------------------------------------------
// Static internal function
// Destroys all vsPipe objects in the static class list
// ------------------------------------------------------------------------
void vsPipe::done()
{
    // Destroy the vsPipe if it exists
    if (pipeList[0] != NULL)
        vsObject::unrefDelete(pipeList[0]);

    // Set the pipe count to 0
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
