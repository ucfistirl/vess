// File vsPane.h++

#ifndef VS_PANE_HPP
#define VS_PANE_HPP

#include <Performer/pf/pfChannel.h>
#include <Performer/pf/pfScene.h>

class vsPane;

#include "vsWindow.h++"
#include "vsView.h++"
#include "vsNode.h++"

enum vsPanePlacement
{
    VS_PANE_PLACEMENT_FULL_WINDOW,
    VS_PANE_PLACEMENT_TOP_HALF,
    VS_PANE_PLACEMENT_BOTTOM_HALF,
    VS_PANE_PLACEMENT_LEFT_HALF,
    VS_PANE_PLACEMENT_RIGHT_HALF,
    VS_PANE_PLACEMENT_TOP_LEFT_QUADRANT,
    VS_PANE_PLACEMENT_TOP_RIGHT_QUADRANT,
    VS_PANE_PLACEMENT_BOTTOM_RIGHT_QUADRANT,
    VS_PANE_PLACEMENT_BOTTOM_LEFT_QUADRANT
};

class vsPane
{
private:

    vsWindow       *parentWindow;
    vsView         *sceneView;

    vsNode         *sceneRoot;
    pfScene        *performerScene;

    pfChannel      *performerChannel;

VS_INTERNAL:

    void          updateView();
    
    static int    gstateCallback(pfGeoState *gstate, void *userData);

    void          _debugWriteScene();

public:

                 vsPane(vsWindow *parent);
    virtual      ~vsPane();

    vsWindow     *getParentWindow();
    void         setView(vsView *view);
    vsView       *getView();
    void         setScene(vsNode *newScene);
    vsNode       *getScene();

    void         setSize(int width, int height);
    void         getSize(int *width, int *height);
    void         setPosition(int xPos, int yPos);
    void         getPosition(int *xPos, int *yPos);
    void         autoConfigure(int panePlacement);

    void         showPane();
    void         hidePane();

    pfChannel    *getBaseLibraryObject();
};

#endif
