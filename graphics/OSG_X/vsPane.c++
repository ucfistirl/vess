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
//    VESS Module:  vsPane.c++
//
//    Description:  Class that represents a portion of a window that has
//                  a 3D image drawn into it by the rendering engine
//
//    Author(s):    Bryan Kline, Jason Daly
//
//------------------------------------------------------------------------

#include "vsPane.h++"

#include "vsGeometry.h++"
#include "vsComponent.h++"
#include "vsViewpointAttribute.h++"
#include <osg/StateSet>
#include <osg/CullFace>
#include <osg/LightModel>
#include <osg/ShadeModel>
#include <osgUtil/DisplayListVisitor>
#include <osgUtil/AppVisitor>
#include <osgUtil/CullVisitor>
#include <stdio.h>

// ------------------------------------------------------------------------
// Constructor - Creates and connects the underlying Open Scene Graph 
// objects that this pane manages. Also configures some default rendering 
// settings.
// ------------------------------------------------------------------------
vsPane::vsPane(vsWindow *parent)
{
    vsScreen *parentScreen;
    vsPipe *parentPipe;
    osg::StateSet *defaultState;
    osg::State *sharedState;
    vsPane *firstPane;
    osg::CullFace *cullFace;
    osg::AlphaFunc *alphaFunc;
    osg::LightModel *lightModel;
    osg::ShadeModel *shadeModel;
    osg::TexEnv *texEnv;
    osgUtil::AppVisitor *appVisitor;
    osgUtil::CullVisitor *cullVisitor;
    osgUtil::RenderGraph *renderGraph;
    osgUtil::RenderStage *renderStage;
    int contextID;
    int loop;

    // Initialize the viewpoint and scene to NULL
    sceneView = NULL;
    sceneRoot = NULL;

    // Get other parameters from the vsWindow passed in
    parentWindow = parent;
    parentScreen = parent->getParentScreen();
    parentPipe = parentScreen->getParentPipe();

    // Panes are visible by default
    paneVisible = VS_TRUE;

    // Normally, this will be a monovision pane, so set it up accordingly
    bufferMode = VS_PANE_BUFFER_MONO;
    eyeSeparation = VS_PANE_DEFAULT_EYE_SEPARATION;
    screenDistance = VS_PANE_DEFAULT_SCREEN_DISTANCE;

    // Set up the OSG display settings
    osgDisplaySettings = new osg::DisplaySettings();
    osgDisplaySettings->ref();
    osgDisplaySettings->setEyeSeparation(eyeSeparation);
    osgDisplaySettings->setScreenDistance(screenDistance);
    osgDisplaySettings->setStereo(false);

    // Create the OSG scene view
    osgSceneView = new osgUtil::SceneView(osgDisplaySettings);
    osgSceneView->ref();

    // Construct the App and Cull NodeVisitors and the rendering objects
    appVisitor = new osgUtil::AppVisitor();
    cullVisitor = new osgUtil::CullVisitor();
    renderGraph = new osgUtil::RenderGraph();
    renderStage = new osgUtil::RenderStage();

    // Configure the CullVisitor to use the rendering objects we created
    cullVisitor->setRenderGraph(renderGraph);
    cullVisitor->setRenderStage(renderStage);

    // Configure the SceneView pipeline
    osgSceneView->setAppVisitor(appVisitor);
    osgSceneView->setCullVisitor(cullVisitor);
    osgSceneView->setRenderGraph(renderGraph);
    osgSceneView->setRenderStage(renderStage);

    // If this is not the first vsPane in this window, set the OSG
    // SceneView object to share the graphics state between the various 
    // panes.  If it is the first pane, make sure the State has the correct
    // OpenGL context ID.  Note that we haven't added this pane to the 
    // window yet, so if this is the first pane, the window's pane count will 
    // still be zero.
    if (parentWindow->getChildPaneCount() > 0)
    {
        // Get the osg::State object from the first pane's SceneView
        firstPane = parentWindow->getChildPane(0);
        sharedState = firstPane->getBaseLibraryObject()->getState();

        // Use the first pane's state object on this pane's SceneView
        osgSceneView->setState(sharedState);
    }
    else
    {
        // Create a state object for the SceneView
        osgSceneView->setState(new osg::State());

        // Get the window index from the parent window
        contextID = parentWindow->getWindowNumber();

        // Use the window index as the context ID on the osg::State of 
        // this pane's SceneView
        osgSceneView->getState()->setContextID((unsigned int)contextID);
    }
    
    // Make the pane fill the window by default
    autoConfigure(VS_PANE_PLACEMENT_FULL_WINDOW);

    // Add the pane to the window
    parentWindow->addPane(this);

    // Set up the default settings for the SceneView, start with the
    // lighting mode
    osgSceneView->setLightingMode(osgUtil::SceneView::NO_SCENEVIEW_LIGHT);

    // Create the global OpenGL state settings
    defaultState = new osg::StateSet;
    defaultState->setGlobalDefaults();

    // Configure culling
    cullFace = new osg::CullFace();
    cullFace->setMode(osg::CullFace::BACK);
    defaultState->setAttributeAndModes(cullFace, osg::StateAttribute::ON);

    // Configure lighting
    defaultState->setMode(GL_LIGHTING, osg::StateAttribute::ON);

    // Configure the shade model
    shadeModel = new osg::ShadeModel();
    shadeModel->setMode(osg::ShadeModel::SMOOTH);
    defaultState->setAttributeAndModes(shadeModel, osg::StateAttribute::ON);

    // Configure depth testing
    defaultState->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

    // Configure alpha blending
    alphaFunc = new osg::AlphaFunc();
    alphaFunc->setFunction(osg::AlphaFunc::GREATER, 0.0f);
    defaultState->setAttributeAndModes(alphaFunc, osg::StateAttribute::ON);

    // Configure the light model
    lightModel = new osg::LightModel();
    lightModel->setLocalViewer(true);
    lightModel->setTwoSided(false);
    lightModel->setAmbientIntensity(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
    defaultState->setAttributeAndModes(lightModel, osg::StateAttribute::ON);

    // Set up a texture environment by default to speed up blending operations.
    // Use texture unit 0, as we don't currently support multitexturing.
    texEnv = new osg::TexEnv();
    texEnv->setMode(osg::TexEnv::MODULATE);
    defaultState->setTextureAttributeAndModes(0, texEnv, 
        osg::StateAttribute::ON);

    // Initialize the global state with the above values
    osgSceneView->setGlobalStateSet(defaultState);

    // Configure the background color
    setBackgroundColor(0.2, 0.2, 0.4);

    // Initialize the culling masks for mono and both stereo channels
    osgSceneView->setCullMask(0xFFFFFFFF);
    osgSceneView->setCullMaskLeft(0xFFFFFFFF);
    osgSceneView->setCullMaskRight(0xFFFFFFFF);

    // Initialize the scene
    osgSceneView->setSceneData(NULL);

    // Initialize the settings for the viewpoint and viewport
    curNearClip = -1.0;
    curFarClip = -1.0;
    curProjMode = VS_VIEW_PROJMODE_PERSP;
    curProjHval = -1.0;
    curProjVval = -1.0;
    curWidth = 0;
    curHeight = 0;
}

// ------------------------------------------------------------------------
// Destructor - Deletes the associated Open Scene Graph objects
// ------------------------------------------------------------------------
vsPane::~vsPane()
{
    // Unreference the SceneView and scene Group
    osgSceneView->setSceneData(NULL);
    osgSceneView->unref();

    // Unreference the display settings object
    osgDisplaySettings->unref();

    // Unreference the VESS scene
    if (sceneRoot != NULL)
        sceneRoot->unref();
    
    // Remove the pane from the its window
    parentWindow->removePane(this);
}

// ------------------------------------------------------------------------
// Returns the parent vsWindow for this pane
// ------------------------------------------------------------------------
vsWindow *vsPane::getParentWindow()
{
    return parentWindow;
}

// ------------------------------------------------------------------------
// Sets the viewpoint object for this pane
// ------------------------------------------------------------------------
void vsPane::setView(vsView *view)
{
    // Save the view object
    sceneView = view;

    // Set the osg::SceneView's camera to use the new vsView's osg::Camera
    osgSceneView->setCamera(view->getBaseLibraryObject());
}

// ------------------------------------------------------------------------
// Retrieves the viewpoint object for this pane
// ------------------------------------------------------------------------
vsView *vsPane::getView()
{
    return sceneView;
}

// ------------------------------------------------------------------------
// Sets the root node of the geometry that is to be displayed in this pane
// ------------------------------------------------------------------------
void vsPane::setScene(vsScene *newScene)
{
    // Reference the new scene
    if (newScene != NULL)
        newScene->ref();

    // Unreference the old one
    if (sceneRoot != NULL)
        sceneRoot->unref();

    // Set the new scene as the Pane's scene
    sceneRoot = newScene;

    // Set the SceneView's scene data to use the new scene
    if (newScene != NULL)
        osgSceneView->setSceneData(newScene->getBaseLibraryObject());
    else
        osgSceneView->setSceneData(NULL);
}

// ------------------------------------------------------------------------
// Retrieves the root node of the geometry being displayed in this pane
// ------------------------------------------------------------------------
vsScene *vsPane::getScene()
{
    return sceneRoot;
}

// ------------------------------------------------------------------------
// Sets the pixel size of this pane within its parent window
// ------------------------------------------------------------------------
void vsPane::setSize(int width, int height)
{
    int x, y, oldWidth, oldHeight;
    int winWidth, winHeight;
    
    // Get the old settings to obtain the pane's origin
    osgSceneView->getViewport()->getViewport(x, y, oldWidth, oldHeight);

    // Compute the new normalized width and height
    parentWindow->getSize(&winWidth, &winHeight);
    widthNorm = (double)width / (double)winWidth;
    heightNorm = (double)height / (double)winHeight;

    // Set the new width and height
    osgSceneView->getViewport()->setViewport(x, y, width, height);
}

// ------------------------------------------------------------------------
// Retrieves the pixel size of this pane. NULL pointers can be passed in
// for undesired data values.
// ------------------------------------------------------------------------
void vsPane::getSize(int *width, int *height)
{
    int x, y, paneWidth, paneHeight;
    
    // Get the viewport settings
    osgSceneView->getViewport()->getViewport(x, y, paneWidth, paneHeight);
    
    // Return the size in the parameters
    if (width != NULL)
        *width = paneWidth;
    if (height != NULL)
        *height = paneHeight;
}

// ------------------------------------------------------------------------
// Sets the location, in pixels, of this pane within its parent window
// ------------------------------------------------------------------------
void vsPane::setPosition(int xPos, int yPos)
{
    int x, y, width, height;
    int winWidth, winHeight;

    // Get the current viewport settings to obtain the pane's width and 
    // height
    osgSceneView->getViewport()->getViewport(x, y, width, height);

    // Compute the new normalized origin
    parentWindow->getSize(&winWidth, &winHeight);
    xPosNorm = (double)xPos / (double)winWidth;
    yPosNorm = (double)yPos / (double)winHeight;

    // Set the new origin
    osgSceneView->getViewport()->setViewport(xPos, yPos, width, height);
}

// ------------------------------------------------------------------------
// Retrieves the location of this pane within its parent window. NULL
// pointers can be passed in for undesired data values.
// ------------------------------------------------------------------------
void vsPane::getPosition(int *xPos, int *yPos)
{
    int x, y, width, height;
    
    // Obtain the viewport settings
    osgSceneView->getViewport()->getViewport(x, y, width, height);
    
    // Return the position in the parameters
    if (xPos != NULL)
        *xPos = x;
    if (yPos != NULL)
        *yPos = y;
}

// ------------------------------------------------------------------------
// Automaticially configures the size and location of the pane within
// its parent window, based on the placement constant passed in.
// ------------------------------------------------------------------------
void vsPane::autoConfigure(int panePlacement)
{
    osg::Viewport *viewport;
    int winWidth, winHeight;

    // Get the dimensions of the window
    parentWindow->getSize(&winWidth, &winHeight);

    // Get the viewport of the OSG SceneView
    viewport = osgSceneView->getViewport();

    // Configure the pane according to the panePlacement value, computing
    // both normalized positions, normalized width, and normalized height.
    // Also set up the OSG viewport accordingly.
    switch (panePlacement)
    {
        case VS_PANE_PLACEMENT_FULL_WINDOW:
            xPosNorm = 0.0;
            yPosNorm = 0.0;
            widthNorm = 1.0;
            heightNorm = 1.0;
            viewport->setViewport(0, 0, winWidth, winHeight);
            break;

        case VS_PANE_PLACEMENT_TOP_HALF:
            xPosNorm = 0.0;
            yPosNorm = 0.5;
            widthNorm = 1.0;
            heightNorm = 0.5;
            viewport->setViewport(0, winHeight/2, winWidth, winHeight/2);
            break;

        case VS_PANE_PLACEMENT_BOTTOM_HALF:
            xPosNorm = 0.0;
            yPosNorm = 0.0;
            widthNorm = 1.0;
            heightNorm = 0.5;
            viewport->setViewport(0, 0, winWidth, winHeight/2);
            break;

        case VS_PANE_PLACEMENT_LEFT_HALF:
            xPosNorm = 0.0;
            yPosNorm = 0.0;
            widthNorm = 0.5;
            heightNorm = 1.0;
            viewport->setViewport(0, 0, winWidth/2, winHeight);
            break;

        case VS_PANE_PLACEMENT_RIGHT_HALF:
            xPosNorm = 0.5;
            yPosNorm = 0.0;
            widthNorm = 0.5;
            heightNorm = 1.0;
            viewport->setViewport(winWidth/2, 0, winWidth/2, winHeight);
            break;

        case VS_PANE_PLACEMENT_TOP_LEFT_QUADRANT:
            xPosNorm = 0.0;
            yPosNorm = 0.5;
            widthNorm = 0.5;
            heightNorm = 0.5;
            viewport->setViewport(0, winHeight/2, winWidth/2, 
                winHeight/2);
            break;

        case VS_PANE_PLACEMENT_TOP_RIGHT_QUADRANT:
            xPosNorm = 0.5;
            yPosNorm = 0.5;
            widthNorm = 0.5;
            heightNorm = 0.5;
            viewport->setViewport(winWidth/2, winHeight/2, winWidth/2, 
                winHeight/2);
            break;
        case VS_PANE_PLACEMENT_BOTTOM_RIGHT_QUADRANT:
            xPosNorm = 0.5;
            yPosNorm = 0.0;
            widthNorm = 0.5;
            heightNorm = 0.5;
            viewport->setViewport(winWidth/2, 0, winWidth/2, winHeight/2);
            break;

        case VS_PANE_PLACEMENT_BOTTOM_LEFT_QUADRANT:
            xPosNorm = 0.0;
            yPosNorm = 0.0;
            widthNorm = 0.5;
            heightNorm = 0.5;
            viewport->setViewport(0, 0, winWidth/2, winHeight/2);
            break;

        default:
            printf("vsPane::autoConfigure: Invalid parameter");
            break;
    }
}

// ------------------------------------------------------------------------
// Sets the buffer mode (mono or one of the stereo modes) of this pane.
// ------------------------------------------------------------------------
void vsPane::setBufferMode(vsPaneBufferMode newMode)
{
    // Only make the changes if the new mode is different from the current
    // mode
    if (newMode != bufferMode)
    {
        switch (newMode)
        {
            // Monovision (no stereo)
            case VS_PANE_BUFFER_MONO:
                osgDisplaySettings->setStereo(false);
                break;

            // Anaglyphic (red/green) stereo
            case VS_PANE_BUFFER_STEREO_ANAGLYPHIC:
                osgDisplaySettings->setStereoMode(
                    osg::DisplaySettings::ANAGLYPHIC);
                osgDisplaySettings->setStereo(true);
                break;

            // Quad-buffered stereo
            case VS_PANE_BUFFER_STEREO_QUADBUFFER:
                osgDisplaySettings->setStereoMode(
                    osg::DisplaySettings::QUAD_BUFFER);
                osgDisplaySettings->setStereo(true);
                break;

            // Horizontal split (the pane is divided into two viewports
            // horizontally)
            case VS_PANE_BUFFER_STEREO_H_SPLIT:
                osgDisplaySettings->setStereoMode(
                    osg::DisplaySettings::HORIZONTAL_SPLIT);
                osgDisplaySettings->setStereo(true);
                break;

            // Vertical split (the pane is divided into two viewports
            // vertically)
            case VS_PANE_BUFFER_STEREO_V_SPLIT:
                osgDisplaySettings->setStereoMode(
                    osg::DisplaySettings::VERTICAL_SPLIT);
                osgDisplaySettings->setStereo(true);
                break;
        }

        // Keep track of the new buffer mode
        bufferMode = newMode;
    }
}

// ------------------------------------------------------------------------
// Returns the current buffer mode of this pane.
// ------------------------------------------------------------------------
vsPaneBufferMode vsPane::getBufferMode()
{
    return bufferMode;
}

// ------------------------------------------------------------------------
// Sets the visibility mask for this pane.  Under Open Scene Graph, each
// node can unfortunately have only one node mask.  For VESS, we use
// this mask for the intersection value, so the visibility mask will
// always be 0xFFFFFFFF.
// ------------------------------------------------------------------------
void vsPane::setVisibilityMask(unsigned int newMask)
{
    printf("vsPane::setVisibilityMask:  Visibility mask not supported under "
        "Open Scene Graph.\n");
}

// ------------------------------------------------------------------------
// Gets the visibility mask for this pane.  Open Scene Graph supports only
// one mask per node, and VESS uses this for the intersect mask.  Therefore
// the visibility mask will always be 0xFFFFFFFF.
// ------------------------------------------------------------------------
unsigned int vsPane::getVisibilityMask()
{
    return 0xFFFFFFFF;
}

// ------------------------------------------------------------------------
// Makes this pane visible. Panes are visible by default.
// ------------------------------------------------------------------------
void vsPane::showPane()
{
    paneVisible = VS_TRUE;
}

// ------------------------------------------------------------------------
// Makes this pane invisible. Geometry connected only to invisible panes
// is not traversed or rendered.
// ------------------------------------------------------------------------
void vsPane::hidePane()
{
    paneVisible = VS_FALSE;
}

// ------------------------------------------------------------------------
// Sets the color of the pane's background.  The background color is used 
// when no earth/sky is enabled.
// ------------------------------------------------------------------------
void vsPane::setBackgroundColor(double r, double g, double b)
{
    osg::Vec4 bgColor;

    // Set up an OSG vector with the given color
    bgColor.set((float)r, (float)g, (float)b, 1.0f);
    
    // Set the background color of the OSG SceneView object
    osgSceneView->setBackgroundColor(bgColor);
}

// ------------------------------------------------------------------------
// Return the color of the pane's background.  The background color is used 
// when no earth/sky is enabled.
// ------------------------------------------------------------------------
void vsPane::getBackgroundColor(double *r, double *g, double *b)
{
    osg::Vec4 bgColor;

    // Get the background color of the OSG SceneView object
    bgColor = osgSceneView->getBackgroundColor();

    // Return each color component if the corresponding parameter is valid
    if (r != NULL)
        *r = bgColor[0];
    if (g != NULL)
        *g = bgColor[1];
    if (b != NULL)
        *b = bgColor[2];
}

// ------------------------------------------------------------------------
// Enables drawing of the earth/sky background in this channel
// ------------------------------------------------------------------------
void vsPane::enableEarthSky()
{
    // No earth/sky support in OSG, do nothing
}

// ------------------------------------------------------------------------
// Disables drawing of the earth/sky background in this channel
// ------------------------------------------------------------------------
void vsPane::disableEarthSky()
{
    // No earth/sky support in OSG, do nothing
}

// ------------------------------------------------------------------------
// Sets the altitude of the ground plane in the earth/sky background
// ------------------------------------------------------------------------
void vsPane::setESGroundHeight(double newHeight)
{
    // No earth/sky support in OSG, do nothing
}

// ------------------------------------------------------------------------
// Retrieves the altitude of the ground plane in the earth/sky background
// ------------------------------------------------------------------------
double vsPane::getESGroundHeight()
{
    // No earth/sky support in OSG, so just return 0.0 as the ground 
    // height
    return 0.0;
}

// ------------------------------------------------------------------------
// Sets the aspect of the earth/sky background color specified by which to
// the specified color
// ------------------------------------------------------------------------
void vsPane::setESColor(int which, double r, double g, double b)
{
    // No earth/sky support in OSG, do nothing.
}

// ------------------------------------------------------------------------
// Retrieves the aspect of the earth/sky background color specified by
// which. NULL pointers may be passed in for unneeded return values.
// ------------------------------------------------------------------------
void vsPane::getESColor(int which, double *r, double *g, double *b)
{
    // No earth/sky support in OSG.  Always return zeroes.

    // Return zero for each non-NULL parameter
    if (r != NULL)
        *r = 0.0;
    if (g != NULL)
        *g = 0.0;
    if (b != NULL)
        *b = 0.0;
}

// ------------------------------------------------------------------------
// Returns the Open Scene Graph object associated with this object
// ------------------------------------------------------------------------
osgUtil::SceneView *vsPane::getBaseLibraryObject()
{
    return osgSceneView;
}

// ------------------------------------------------------------------------
// Internal function
// Returns whether or not this pane is visible (i.e.: should be drawn)
// ------------------------------------------------------------------------
int vsPane::isVisible()
{
    return paneVisible;
}

// ------------------------------------------------------------------------
// Internal function
// Updates the OSG view matrix with the information contained within
// this pane's viewpoint (vsView) object.
// ------------------------------------------------------------------------
void vsPane::updateView()
{
    vsMatrix viewMatrix, xformMatrix;
    osg::Matrix osgMatrix;
    int loop, sloop;
    vsVector viewPos;
    double near, far;
    int projMode;
    double projHval, projVval;
    double hFOV, vFOV;
    int paneWidth, paneHeight;
    double aspectMatch;
    vsViewpointAttribute *viewAttr;
    osg::Camera *osgCamera;
    
    // Do nothing if no vsView is attached
    if (sceneView == NULL)
        return;

    // If a viewpoint attribute is attached, update it
    viewAttr = (vsViewpointAttribute *)
	((vsViewpointAttribute::getMap())->mapFirstToSecond(sceneView));
    if (viewAttr)
	viewAttr->update();
    
    // Get the projection values from the vsView and the size of the pane,
    // and adjust the osgSceneView and osgCamera, if necessary
    osgCamera = osgSceneView->getCamera();
    getSize(&paneWidth, &paneHeight);
    sceneView->getProjectionData(&projMode, &projHval, &projVval);
    if ((curProjMode != projMode) || (curProjHval != projHval) ||
	(curProjVval != projVval) || (curWidth != paneWidth) ||
        (curHeight != paneHeight))
    {
        // Check the projection mode
	if (projMode == VS_VIEW_PROJMODE_PERSP)
        {
            // Get the size of the pane and calculate the aspect ratio.
            // If the height of the pane is zero or less, the aspect
            // ratio will default to 1.0 to avoid dividing by zero.
            // The actual result is irrelevant at this point, since
            // a zero-height pane is not visible anyway.
            if (paneHeight <= 0)
                aspectMatch = 1.0;
            else
                aspectMatch = (double)paneWidth / (double)paneHeight;

            // The aspect-matched FOV parameter is computed according
            // to the following formula:
            // 
            //   aspectMatch = tan(0.5 * projHVal) / tan(0.5 * projVval)
            // 
            // Check the horizontal and vertical values to see if and which
            // parameters are set to default values.
            if ((projHval <= 0.0) && (projVval <= 0.0))
            {
                // No FOV specified, so horizontal defaults to 
                // VS_PANE_DEFAULT_FOV and vertical is aspect-matched
                hFOV = VS_DEG2RAD(VS_PANE_DEFAULT_FOV);
                vFOV = 2 * atan2( tan(0.5 * hFOV), aspectMatch);
                hFOV = VS_RAD2DEG(hFOV);
                vFOV = VS_RAD2DEG(vFOV);
            }
            else if (projHval <= 0.0)
            {
                // Vertical FOV specified, but no horizontal, so 
                // aspect-match horizontal to vertical
                vFOV = VS_DEG2RAD(projVval);
                hFOV = 2 * atan(tan(0.5 * hFOV) * aspectMatch);
                hFOV = VS_RAD2DEG(hFOV);
                vFOV = VS_RAD2DEG(vFOV);
            }
            else if (projVval <= 0.0)
            {
                // Horizontal FOV specified, but no vertical, so 
                // aspect-match vertical to horizontal
                hFOV = VS_DEG2RAD(projHval);
                vFOV = 2 * atan2( tan(0.5 * hFOV), aspectMatch);
                hFOV = VS_RAD2DEG(hFOV);
                vFOV = VS_RAD2DEG(vFOV);
            }
            else
            {
                // Both FOV's specified, so set accordingly
                hFOV = projHval;
                vFOV = projVval;
            }

            // Set the OSG camera to the new FOV values
            osgCamera->setFOV(hFOV, vFOV, osgCamera->zNear(), 
                osgCamera->zFar());
        }
	else
	{
            // Check the horizontal and vertical values to see if and which
            // parameters are set to default values.
	    if ((projHval <= 0.0) && (projVval <= 0.0))
	    {
		// Neither specified, use default values
		osgCamera->setOrtho(-VS_PANE_DEFAULT_ORTHO_PLANE, 
                    VS_PANE_DEFAULT_ORTHO_PLANE, -VS_PANE_DEFAULT_ORTHO_PLANE, 
                    VS_PANE_DEFAULT_ORTHO_PLANE, osgCamera->zNear(), 
                    osgCamera->zFar());
	    }
	    else if (projHval <= 0.0)
	    {
		// Vertical specified, horizontal aspect match
		getSize(&paneWidth, &paneHeight);
		aspectMatch = (projVval / (double)paneHeight) *
		    (double)paneWidth;
		osgCamera->setOrtho(-aspectMatch, aspectMatch, -projVval, 
                    projVval, osgCamera->zNear(), osgCamera->zFar());
	    }
	    else if (projVval <= 0.0)
	    {
		// Horizontal specified, vertical aspect match
		getSize(&paneWidth, &paneHeight);
		aspectMatch = (projHval / (double)paneWidth) *
		    (double)paneHeight;
		osgCamera->setOrtho(-projHval, projHval, -aspectMatch, 
                    aspectMatch, osgCamera->zNear(), osgCamera->zFar());
	    }
	    else
	    {
		// Both specified, set values explicitly
		osgCamera->setOrtho(-projHval, projHval, -projVval, 
                    projVval, osgCamera->zNear(), osgCamera->zFar());
	    }
	}
	
        // Save the new projection values
	curProjMode = projMode;
	curProjHval = projHval;
	curProjVval = projVval;
        curWidth = paneWidth;
        curHeight = paneHeight;
    }
}

// ------------------------------------------------------------------------
// Internal function
// Resizes the pane according to the current window size and the normalized
// pane position and size settings.  Called in response to resizing of
// the parent window.
// ------------------------------------------------------------------------
void vsPane::resize()
{
    int winWidth, winHeight;
    int x, y, width, height;

    // Get the current size of the window
    parentWindow->getSize(&winWidth, &winHeight);

    // Compute the position and size of the pane in local window coordinates
    x = (int)(xPosNorm * (double)winWidth);
    y = (int)(yPosNorm * (double)winHeight);
    width = (int)(widthNorm * (double)winWidth), 
    height = (int)(heightNorm * (double)winHeight);

    // Update the OSG SceneView's viewport
    osgSceneView->setViewport(x, y, width, height);
}
