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
//    Author(s):    Bryan Kline
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

#define VS_WINDOW_DEFAULT_WIDTH  640
#define VS_WINDOW_DEFAULT_HEIGHT 480
#define VS_WINDOW_DEFAULT_XPOS   0
#define VS_WINDOW_DEFAULT_YPOS   0

class vsWindow
{
private:

    vsScreen           *parentScreen;

    vsGrowableArray    childPaneList;
    int                childPaneCount;

    int                windowNumber;
    
    Window             xWindow;
    GLXContext         glContext;
    
    Window             topWindowID;

    int                xPositionOffset, yPositionOffset;
    int                widthOffset, heightOffset;

    int                validObject;

    static int         windowCount;

    static Bool        waitForMap(Display *display, XEvent *event, char *arg);

VS_INTERNAL:

    void          addPane(vsPane *newPane);
    void          removePane(vsPane *targetPane);

    int           getWindowNumber();
    void          makeCurrent();
    void          swapBuffers();

    void          update();

public:

                      vsWindow(vsScreen *parent, int hideBorder);
                      vsWindow(vsScreen *parent, int xPosition, int yPosition,
                               int width, int height, int hideBorder);
                      vsWindow(vsScreen *parent, int hideBorder, int stereo);
                      vsWindow(vsScreen *parent, int xPosition, int yPosition,
                               int width, int height, int hideBorder, 
                               int stereo);
                      vsWindow(vsScreen *parent, Window xWin);
    virtual           ~vsWindow();
    
    vsScreen          *getParentScreen();
    int               getChildPaneCount();
    vsPane            *getChildPane(int index);

    void              setSize(int width, int height);
    void              getSize(int *width, int *height);
    void              setPosition(int xPos, int yPos);
    void              getPosition(int *xPos, int *yPos);
    void              setFullScreen();

    void              setName(char *newName);

    void              saveImage(char *filename);

    Window            getBaseLibraryObject();
};

#endif
