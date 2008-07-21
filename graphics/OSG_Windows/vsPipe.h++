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
//                  but should instead be retrieved using the getPipe()
//                  static class method after the vsSystem object is
//                  constructed.
//
//    Author(s):    Bryan Kline, Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_PIPE_HPP
#define VS_PIPE_HPP

class vsPipe;

#include "vsGlobals.h++"
#include "vsObject.h++"
#include "vsScreen.h++"

#define VS_MAX_PIPE_COUNT 10

class VESS_SYM vsPipe : public vsObject
{
private:

    static vsPipe *pipeList[VS_MAX_PIPE_COUNT];
    static int    pipeCount;

    // Index of this pipe
    int           pipeIndex;

    vsScreen      *childScreen;

                  vsPipe(int index);
    virtual       ~vsPipe();

VS_INTERNAL:

    static void init();
    static void done();

    void        setScreen(vsScreen *newScreen);

public:

    virtual const char *getClassName();

    static vsPipe      *getPipe(int index);
    static int         getPipeCount();

    vsScreen           *getScreen(int index);

    int                getBaseLibraryObject();
};

#endif
