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

// ------------------------------------------------------------------------
// Constructor - Gets and stores the Performer pipe object associated with
// this object
// ------------------------------------------------------------------------
vsPipe::vsPipe(int index)
{
    // Get the pfPipe object with the specified index from the Performer
    // library
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
// VESS internal function
// Sets the screen object (index 0) for this pipe object
// ------------------------------------------------------------------------
void vsPipe::setScreen(vsScreen *newScreen)
{
    childScreen = newScreen;
}
