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
//    VESS Module:  vsWindow.h++
//
//    Description:  Class that represents an open window on any screen
//
//    Author(s):    Bryan Kline, Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_WINDOW_HPP
#define VS_WINDOW_HPP

class vsWindow;

#include <X11/Xlib.h>
#include <GL/glx.h>
#undef index
#include "vsScreen.h++"
#include "vsPane.h++"
#include "vsGrowableArray.h++"
#include "vsImage.h++"

#define VS_WINDOW_DEFAULT_WIDTH  640
#define VS_WINDOW_DEFAULT_HEIGHT 480
#define VS_WINDOW_DEFAULT_XPOS   0
#define VS_WINDOW_DEFAULT_YPOS   0

class vsWindow : public vsObject
{
private:

    vsScreen           *parentScreen;

    vsGrowableArray    childPaneList;
    int                childPaneCount;

    int                windowNumber;

    Window             xWindow;
    GLXDrawable        drawable;
    GLXContext         glContext;
    GLXFBConfig        fbConfig;

    Window             topWindowID;

    int                xPositionOffset, yPositionOffset;
    int                widthOffset, heightOffset;
    int                drawableWidth, drawableHeight;

    static int         windowCount;

    static Bool        waitForMap(Display *display, XEvent *event, char *arg);

    bool               isOffScreenWindow;

VS_INTERNAL:

    void          addPane(vsPane *newPane);
    void          removePane(vsPane *targetPane);

    int           getWindowNumber();
    void          makeCurrent();
    void          swapBuffers();

    bool          isOffScreen();
    void          update();

public:

                          vsWindow(vsScreen *parent, bool hideBorder, 
                                   bool stereo);
                          vsWindow(vsScreen *parent, int x, int y, int width, 
                                   int height, bool hideBorder, bool stereo);
                          vsWindow(vsScreen *parent, int offScreenWidth,
                                   int offScreenHeight);
                          vsWindow(vsScreen *parent, Window xWin);
    virtual               ~vsWindow();

    virtual const char    *getClassName();
    
    vsScreen              *getParentScreen();
    int                   getChildPaneCount();
    vsPane                *getChildPane(int index);

    void                  setSize(int width, int height);
    void                  getSize(int *width, int *height);
    void                  getDrawableSize(int *width, int *height);
    void                  setPosition(int xPos, int yPos);
    void                  getPosition(int *xPos, int *yPos);
    void                  setFullScreen();

    void                  setName(char *newName);

    void                  saveImage(char *filename);
    vsImage               *getImage();

    Window                getBaseLibraryObject();
};

#endif
