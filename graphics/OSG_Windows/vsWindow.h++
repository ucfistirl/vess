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

#include <windows.h>

#include <wingdi.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <glext.h>
#include <wglext.h>

#include "vsScreen.h++"
#include "vsPane.h++"
#include "vsObjectMap.h++"
#include "vsGrowableArray.h++"
#include "vsImage.h++"

#define VS_WINDOW_DEFAULT_WIDTH  640
#define VS_WINDOW_DEFAULT_HEIGHT 480
#define VS_WINDOW_DEFAULT_XPOS   0
#define VS_WINDOW_DEFAULT_YPOS   0

class VS_GRAPHICS_DLL vsWindow : public vsObject
{
private:

    vsScreen                   *parentScreen;

    vsGrowableArray            childPaneList;
    int                        childPaneCount;

    int                        windowNumber;

    char                       windowClassName[50];
    WNDCLASSEX                 windowClass;
    HWND                       msWindow;
    HDC                        deviceContext;
    HGLRC                      glContext;
    WNDPROC                    oldWindowProc;
    HPBUFFERARB                pBuffer;
    int                        pixelFormat;
    

    // Maps vsWindow's to MS Windows HWND's
    static vsObjectMap         *windowMap;

    int                        xPositionOffset, yPositionOffset;
    int                        widthOffset, heightOffset;
    int                        drawableWidth, drawableHeight;

    int                        validObject;

    static int                 windowCount;

    static LRESULT CALLBACK    mainWindowProc(HWND msWindow, UINT message, 
                                              WPARAM wParam, LPARAM lParam);

    static LRESULT CALLBACK    subclassedWindowProc(HWND msWindow, UINT message,
                                                    WPARAM wParam, 
                                                    LPARAM lParam);

    bool                       isOffScreenWindow;

    // Function pointers for the WGL Extensions for off-screen windows
    static PFNWGLGETEXTENSIONSSTRINGARBPROC    wglGetExtensionsStringARB;
    static PFNWGLCHOOSEPIXELFORMATARBPROC      wglChoosePixelFormatARB;
    static PFNWGLRELEASEPBUFFERDCARBPROC       wglReleasePbufferDCARB;
    static PFNWGLDESTROYPBUFFERARBPROC         wglDestroyPbufferARB;
    static PFNWGLCREATEPBUFFERARBPROC          wglCreatePbufferARB;
    static PFNWGLGETPBUFFERDCARBPROC           wglGetPbufferDCARB;
    static PFNWGLQUERYPBUFFERARBPROC           wglQueryPbufferARB;

VS_INTERNAL:

    void                  addPane(vsPane *newPane);
    void                  removePane(vsPane *targetPane);

    int                   getWindowNumber();
    void                  makeCurrent();
    void                  swapBuffers();

    static vsObjectMap    *getMap();
    static void           deleteMap();

    WNDPROC               getWindowProc();

    void                  update();
    bool                  isOffScreen();

public:

                       vsWindow(vsScreen *parent, bool hideBorder, bool stereo);
                       vsWindow(vsScreen *parent, int x, int y, int width, 
                                int height, bool hideBorder, bool stereo);
                       vsWindow(vsScreen *parent, int offScreenWidth,
                                int offScreenHeight);
                       vsWindow(vsScreen *parent, HWND msWin);
    virtual            ~vsWindow();

    virtual const char *getClassName();

    vsScreen           *getParentScreen();
    int                getChildPaneCount();
    vsPane             *getChildPane(int index);

    void               setSize(int width, int height);
    void               getSize(int *width, int *height);
    void               getDrawableSize(int *width, int *height);
    void               setPosition(int xPos, int yPos);
    void               getPosition(int *xPos, int *yPos);
    void               setFullScreen();

    void               setName(char *newName);

    void               saveImage(char *filename);
    vsImage *          getImage();

    HWND               getBaseLibraryObject();
};

#endif
