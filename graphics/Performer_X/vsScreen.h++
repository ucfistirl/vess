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
//    VESS Module:  vsScreen.h++
//
//    Description:  Class that represents a physical display device
//                  attached to a computer. Objects of this class should
//                  not be instantiated directly by the user but should
//                  instead be retrieved from the active vsSystem object.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_SCREEN_HPP
#define VS_SCREEN_HPP

class vsScreen;

#include "vsPipe.h++"
#include "vsWindow.h++"
#include "vsGrowableArray.h++"

#define VS_MAX_SCREEN_COUNT 10

class vsScreen : public vsObject
{
private:

    static vsScreen    *screenList[VS_MAX_SCREEN_COUNT];
    static int         screenCount;

    vsPipe             *parentPipe;

    vsGrowableArray    childWindowList;
    int                childWindowCount;

                       vsScreen(vsPipe *parent);
    virtual            ~vsScreen();

VS_INTERNAL:

    static void    init();
    static void    done();

    void           addWindow(vsWindow *newWindow);
    void           removeWindow(vsWindow *targetWindow);

public:

    virtual const char *getClassName();

    static vsScreen    *getScreen(int index);
    static int         getScreenCount();

    vsPipe             *getParentPipe();
    int                getChildWindowCount();
    vsWindow           *getChildWindow(int index);

    void               getScreenSize(int *width, int *height);
};

#endif
