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

#include <Performer/pf/pfPipeWindow.h>

class vsWindow;

#include "vsScreen.h++"
#include "vsPane.h++"
#include "vsGrowableArray.h++"
#include "vsImage.h++"
#include "vsObjectMap.h++"

#define VS_WINDOW_DEFAULT_WIDTH  640
#define VS_WINDOW_DEFAULT_HEIGHT 480
#define VS_WINDOW_DEFAULT_XPOS   50
#define VS_WINDOW_DEFAULT_YPOS   50

class VS_GRAPHICS_DLL vsWindow : public vsObject
{
private:

    vsScreen              *parentScreen;

    vsGrowableArray       childPaneList;
    int                   childPaneCount;

    pfPipeWindow          *performerPipeWindow;

    static vsObjectMap    *windowMap;
    static vsObjectMap    *drawableMap;
    
VS_INTERNAL:

    void                  addPane(vsPane *newPane);
    void                  removePane(vsPane *targetPane);

    static vsObjectMap    *getWindowMap();
    static vsObjectMap    *getDrawableMap();
    static void           deleteMap();

public:

                       vsWindow(vsScreen *parent, bool hideBorder, bool stereo);
                       vsWindow(vsScreen *parent, int x, int y, int width,
                                int height, bool hideBorder, bool stereo);
                       vsWindow(vsScreen *parent, Window xWin);
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

    pfPipeWindow       *getBaseLibraryObject();
};

#endif
