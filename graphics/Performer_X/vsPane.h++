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
//    VESS Module:  vsPane.h++
//
//    Description:  Class that represents a portion of a window that has
//                  a 3D image drawn into it by the rendering engine
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_PANE_HPP
#define VS_PANE_HPP

#include <Performer/pf/pfChannel.h>
#include <Performer/pf/pfScene.h>

class vsPane;

#include "vsWindow.h++"
#include "vsView.h++"
#include "vsNode.h++"
#include "vsScene.h++"

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

enum vsPaneEarthSkyColor
{
    VS_PANE_ESCOLOR_SKY_NEAR,
    VS_PANE_ESCOLOR_SKY_FAR,
    VS_PANE_ESCOLOR_SKY_HORIZON,
    VS_PANE_ESCOLOR_GROUND_FAR,
    VS_PANE_ESCOLOR_GROUND_NEAR
};

enum vsPaneBufferMode
{
    VS_PANE_BUFFER_MONO,
    VS_PANE_BUFFER_STEREO_L,
    VS_PANE_BUFFER_STEREO_R
};

struct vsPaneSharedData
{
    vsPaneBufferMode bufferMode;
};

class vsPane
{
private:

    vsWindow            *parentWindow;
    vsView              *sceneView;

    vsScene             *sceneRoot;
    pfScene             *performerScene;

    pfChannel           *performerChannel;
    pfEarthSky          *earthSky;

    vsPaneBufferMode    bufferMode;
    vsPaneSharedData    *sharedData;

    double              curNearClip, curFarClip;
    int                 curProjMode;
    double              curProjHval, curProjVval;

VS_INTERNAL:

    void           updateView();

    static void    drawPane(pfChannel *chan, void *userData);

public:

                        vsPane(vsWindow *parent);
    virtual             ~vsPane();

    vsWindow            *getParentWindow();
    void                setView(vsView *view);
    vsView              *getView();
    void                setScene(vsScene *newScene);
    vsScene             *getScene();

    void                setSize(int width, int height);
    void                getSize(int *width, int *height);
    void                setPosition(int xPos, int yPos);
    void                getPosition(int *xPos, int *yPos);
    void                autoConfigure(int panePlacement);

    void                setBufferMode(vsPaneBufferMode newMode);
    vsPaneBufferMode    getBufferMode();
    
    void                setVisibilityMask(unsigned int newMask);
    unsigned int        getVisibilityMask();

    void                showPane();
    void                hidePane();

    void                setBackgroundColor(double r, double g, double b);
    void                getBackgroundColor(double *r, double *g, double *b);

    void                enableEarthSky();
    void                disableEarthSky();
    void                setESGroundHeight(double newHeight);
    double              getESGroundHeight();
    void                setESColor(int which, double r, double g, double b);
    void                getESColor(int which, double *r, double *g, double *b);

    pfChannel           *getBaseLibraryObject();
};

#endif