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

class vsPane;

#include "vsWindow.h++"
#include "vsView.h++"
#include "vsNode.h++"
#include "vsScene.h++"
#include "vsComponent.h++"
#include <osgUtil/SceneView>

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
    VS_PANE_BUFFER_STEREO_ANAGLYPHIC,
    VS_PANE_BUFFER_STEREO_QUADBUFFER
};

#define VS_PANE_DEFAULT_EYE_SEPARATION  0.068
#define VS_PANE_DEFAULT_SCREEN_DISTANCE 0.5
#define VS_PANE_DEFAULT_FOV             45.0
#define VS_PANE_DEFAULT_ORTHO_PLANE     10.0

class vsPane : public vsObject
{
private:

    // Parent vsWindow
    vsWindow                *parentWindow;

    // Viewpoint
    vsView                  *sceneView;
    int                     viewChangeNum;

    // Root of the scene graph
    vsScene                 *sceneRoot;

    // Supporting Open Scene Graph objects
    osg::DisplaySettings    *osgDisplaySettings;
    osgUtil::SceneView      *osgSceneView;
    osgUtil::RenderStage    *renderStage;

    // Buffer mode (mono/stereo and stereo mode)
    vsPaneBufferMode        bufferMode;

    // Position and size normalized to window size, so the new pane 
    // position and size can be computed when the window resizes
    double                  xPosNorm, yPosNorm;
    double                  widthNorm, heightNorm;

    // Pane visibility flag.  Pane is not drawn if false
    bool                    paneVisible;

    // Stereo parameters
    double                  eyeSeparation;
    double                  screenDistance;

VS_INTERNAL:

    // Checks the visibility flag of the pane
    bool           isVisible();

    // Resizes the pane according to the parent window's size and
    // normalized pane position and size
    void           resize();

    // Updates the viewpoint based on changes to the vsView
    // object
    void           updateView();

public:

                          vsPane(vsWindow *parent);
    virtual               ~vsPane();

    // Inherited functions
    virtual const char    *getClassName();

    // Window, view, and scene accessors
    vsWindow              *getParentWindow();
    void                  setView(vsView *view);
    vsView                *getView();
    void                  setScene(vsScene *newScene);
    vsScene               *getScene();

    // Pane size and position functions
    void                  setSize(int width, int height);
    void                  getSize(int *width, int *height);
    void                  setPosition(int xPos, int yPos);
    void                  getPosition(int *xPos, int *yPos);
    void                  autoConfigure(int panePlacement);

    // Buffer mode functions
    void                  setBufferMode(vsPaneBufferMode newMode);
    vsPaneBufferMode      getBufferMode();
    
    // Pane visibility functions
    void                  showPane();
    void                  hidePane();

    // Background color
    void                  setBackgroundColor(double r, double g, double b);
    void                  getBackgroundColor(double *r, double *g, double *b);

    // Earth/sky operations
    void                  enableEarthSky();
    void                  disableEarthSky();
    void                  setESGroundHeight(double newHeight);
    double                getESGroundHeight();
    void                  setESColor(int which, double r, double g, double b);
    void                  getESColor(int which, double *r, double *g, 
                                     double *b);

    void                  enableStats();
    void                  disableStats();
    bool                  areStatsEnabled();

    // OpenGL buffer clear mask
    void                  setGLClearMask(int clearMask);
    int                   getGLClearMask();

    // OSG object accessor
    osgUtil::SceneView    *getBaseLibraryObject();
};

#endif
