// File vsScreen.h++

#ifndef VS_SCREEN_HPP
#define VS_SCREEN_HPP

class vsScreen;

#include "vsPipe.h++"
#include "vsWindow.h++"

class vsScreen
{
private:

    vsPipe              *parentPipe;
    vsWindowListNode    *childWindowList;

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
