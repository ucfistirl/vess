// File vsScreen.h++

#ifndef VS_SCREEN_HPP
#define VS_SCREEN_HPP

class vsScreen;

#include "vsPipe.h++"
#include "vsWindow.h++"
#include "vsGrowableArray.h++"

class vsScreen
{
private:

    vsPipe             *parentPipe;

    vsGrowableArray    childWindowList;
    int                childWindowCount;

VS_INTERNAL:

    void        addWindow(vsWindow *newWindow);
    void        removeWindow(vsWindow *targetWindow);

public:

                vsScreen(vsPipe *parent);
    virtual     ~vsScreen();

    vsPipe      *getParentPipe();
    int         getChildWindowCount();
    vsWindow    *getChildWindow(int index);

    void        getScreenSize(int *width, int *height);
};

#endif
