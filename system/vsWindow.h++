// File vsWindow.h++

#ifndef VS_WINDOW_HPP
#define VS_WINDOW_HPP

#include <Performer/pf/pfPipeWindow.h>

class vsWindow;
struct vsWindowListNode;

#include "vsScreen.h++"
#include "vsPane.h++"

#define VS_WINDOW_DEFAULT_WIDTH  640
#define VS_WINDOW_DEFAULT_HEIGHT 480
#define VS_WINDOW_DEFAULT_XPOS   50
#define VS_WINDOW_DEFAULT_YPOS   50

struct vsWindowListNode
{
    vsWindow            *data;
    vsWindowListNode    *next;
};

class vsWindow
{
private:

    vsScreen          *parentScreen;
    vsPaneListNode    *childPaneList;
    
    pfPipeWindow      *performerPipeWindow;
    
    Window            topWindowID;
    
    int               xPositionOffset, yPositionOffset;
    int               widthOffset, heightOffset;
    
VS_INTERNAL:

    void        addPane(vsPane *newPane);
    void        removePane(vsPane *targetPane);
    
public:

                    vsWindow(vsScreen *parent, int hideBorder);
    virtual         ~vsWindow();
    
    vsScreen        *getParentScreen();
    int             getChildPaneCount();
    vsPane          *getChildPane(int index);

    void            setSize(int width, int height);
    void            getSize(int *width, int *height);
    void            setPosition(int xPos, int yPos);
    void            getPosition(int *xPos, int *yPos);
    void            setFullScreen();
    
    void            setName(char *newName);

    pfPipeWindow    *getBaseLibraryObject();
};

#endif
