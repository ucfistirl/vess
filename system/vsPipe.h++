// File vsPipe.h++

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
