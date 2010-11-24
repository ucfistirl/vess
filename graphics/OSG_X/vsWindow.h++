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

#include "atOSDefs.h++"
#include <X11/Xlib.h>
#include <GL/glx.h>
#undef index
#include "vsScreen.h++"
#include "vsPane.h++"
#include "vsArray.h++"
#include "vsImage.h++"

#define VS_WINDOW_DEFAULT_WIDTH  640
#define VS_WINDOW_DEFAULT_HEIGHT 480
#define VS_WINDOW_DEFAULT_XPOS   0
#define VS_WINDOW_DEFAULT_YPOS   0

#define MWM_HINTS_FUNCTIONS   (1L << 0)
#define MWM_HINTS_DECORATIONS (1L << 1)
#define MWM_HINTS_INPUT_MODE  (1L << 2)
#define MWM_HINTS_STATUS      (1L << 3)

#define MWM_DECOR_ALL         (1L<<0)
#define MWM_DECOR_BORDER      (1L<<1)
#define MWM_DECOR_RESIZEH     (1L<<2)
#define MWM_DECOR_TITLE       (1L<<3)
#define MWM_DECOR_MENU        (1L<<4)
#define MWM_DECOR_MINIMIZE    (1L<<5)
#define MWM_DECOR_MAXIMIZE    (1L<<6)

#define MWM_FUNC_ALL          (1L<<0)
#define MWM_FUNC_RESIZE       (1L<<1)
#define MWM_FUNC_MOVE         (1L<<2)
#define MWM_FUNC_MINIMIZE     (1L<<3)
#define MWM_FUNC_MAXIMIZE     (1L<<4)
#define MWM_FUNC_CLOSE        (1L<<5)

#define PROP_MOTIF_WM_HINTS_ELEMENTS   5
#define PROP_MWM_HINTS_ELEMENTS     PROP_MOTIF_WM_HINTS_ELEMENTS

typedef struct
{
   unsigned long flags;
   unsigned long functions;
   unsigned long decorations;
   long          inputMode;
   unsigned long status;
} PropMotifWmHints;

class vsWindow : public vsObject
{
private:

    vsScreen           *parentScreen;

    vsArray            childPaneList;

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
    bool               createdXWindow;

VS_INTERNAL:

    void          addPane(vsPane *newPane);
    void          removePane(vsPane *targetPane);

    void          bringPaneToFront(vsPane *newPane);
    void          sendPaneToBack(vsPane *newPane);

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
