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
