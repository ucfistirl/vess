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
//    VESS Module:  vsPipe.h++
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

#ifndef VS_PIPE_HPP
#define VS_PIPE_HPP

class vsPipe;

#include <Performer/pf/pfPipe.h>
#include "vsGlobals.h++"
#include "vsScreen.h++"

class vsPipe
{
private:

    vsScreen    *childScreen;

    pfPipe      *performerPipe;

VS_INTERNAL:

    void        setScreen(vsScreen *newScreen);

public:

                vsPipe(int index);
    virtual     ~vsPipe();

    vsScreen    *getScreen(int index);

    pfPipe      *getBaseLibraryObject();
};

#endif
