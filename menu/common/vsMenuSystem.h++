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
//    VESS Module:  vsMenuSystem.h++
//
//    Description:  The vsMenuSystem is a handler class that manages input
//                  and state changes of a menu structure. It requires a
//                  window and an input system, creating a pane over the
//                  existing window for output and extracting devices for
//                  input.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_MENU_SYSTEM_HPP
#define VS_MENU_SYSTEM_HPP

#include <X11/Xlib.h>
#include <GL/glx.h>
#include "vsGeometry.h++"
#include "vsGlobals.h++"
#include "vsIntersect.h++"
#include "vsLightAttribute.h++"
#include "vsMaterialAttribute.h++"
#include "vsMenuFrame.h++"
#include "vsMenuIterator.h++"
#include "vsMenuObject.h++"
#include "vsMenuTree.h++"
#include "vsPane.h++"
#include "vsTextureAttribute.h++"
#include "vsVector.h++"
#include "vsWindow.h++"
#include "vsWindowSystem.h++"

#define VS_MENU_ACTION_COUNT 6

enum VS_SYSTEM_DLL vsMenuAction
{
    VS_MENU_ACTION_ACTIVATE,
    VS_MENU_ACTION_PREVIOUS,
    VS_MENU_ACTION_NEXT,
    VS_MENU_ACTION_DECREASE,
    VS_MENU_ACTION_INCREASE,
    VS_MENU_ACTION_CURSOR
};

class VS_SYSTEM_DLL vsMenuSystem
{
private:

    vsPane            *menuPane;
    vsScene           *menuScene;
    vsView            *menuView;

    vsComponent       *menuComponent;

    vsIntersect       *isectObject;

    bool              hasCursor;
    vsInputAxis       *xAxis;
    vsInputAxis       *yAxis;

    vsInputButton     *inputButtons[VS_MENU_ACTION_COUNT];
    bool              pressed[VS_MENU_ACTION_COUNT];

    vsMenuTree        *menuTree;
    vsMenuFrame       *menuFrame;
    vsMenuObject      *selectedObj;

VS_INTERNAL:

    bool        processAction(vsMenuAction action);

public:

                          vsMenuSystem();
                          vsMenuSystem(vsPane *pane,
                              vsWindowSystem *windowSystem);
    virtual               ~vsMenuSystem();

    virtual const char    *getClassName();

    void                  setPane(vsPane *pane);
    vsPane                *getPane();

    vsScene               *getScene();
    vsView                *getView();

    void                  setCursor(vsInputAxis *x, vsInputAxis *y);

    void                  setMenuTree(vsMenuTree *newTree);
    void                  rebuildMenu();

    void                  setFrame(vsMenuFrame *frame);
    vsMenuFrame           *getFrame();

    vsMenuObject          *getSelection();

    void                  setMenuButton(vsMenuAction action,
                              vsInputButton *newButton);

    void                  show();
    void                  hide();

    void                  update();
};

#endif
