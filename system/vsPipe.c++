// File vsPipe.c++

#include "vsPipe.h++"

// ------------------------------------------------------------------------
// Constructor - Gets and stores the Performer pipe object associated with
// this object
// ------------------------------------------------------------------------
vsPipe::vsPipe(int index)
{
    performerPipe = pfGetPipe(index);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsPipe::~vsPipe()
{
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
// Returns the Performer object associated with this object
// ------------------------------------------------------------------------
pfPipe *vsPipe::getBaseLibraryObject()
{
    return performerPipe;
}

// ------------------------------------------------------------------------
// VESS internal function
// Sets the screen object (index 0) for this pipe object
// ------------------------------------------------------------------------
void vsPipe::setScreen(vsScreen *newScreen)
{
    childScreen = newScreen;
}
