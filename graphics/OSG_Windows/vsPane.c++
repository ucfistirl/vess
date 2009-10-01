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
#include <osg/AlphaFunc>
#include <osg/CullFace>
#include <osg/LightModel>
#include <osg/ShadeModel>
#include <osg/StateSet>
#include <osg/TexEnv>
#include <osgUtil/CullVisitor>
#include <osgUtil/GLObjectsVisitor>
#include <osgUtil/SceneView>
#include <osgUtil/StateGraph>
#include <osgUtil/UpdateVisitor>
#include <osgDB/Registry>
#include <osgDB/DatabasePager>
#include <stdio.h>

// ------------------------------------------------------------------------
// Constructor - Creates and connects the underlying Open Scene Graph 
// objects that this pane manages. Also configures some default rendering 
// settings.
// ------------------------------------------------------------------------
vsPane::vsPane(vsWindow *parent)
{
    osg::StateSet *defaultState;
    osg::State *sharedState;
    vsPane *firstPane;
    osg::CullFace *cullFace;
    osg::AlphaFunc *alphaFunc;
    osg::LightModel *lightModel;
    osg::ShadeModel *shadeModel;
    osg::TexEnv *texEnv;
    osgUtil::UpdateVisitor *updateVisitor;
    osgUtil::CullVisitor *cullVisitor;
    osgUtil::StateGraph *stateGraph;
    osgUtil::RenderStage *renderStage;
    int contextID;

    // Initialize the viewpoint and scene to NULL
    sceneView = NULL;
    viewChangeNum = 0;
    sceneRoot = NULL;

    // Get other parameters from the vsWindow passed in
    parentWindow = parent;

    // Panes are visible by default
    paneVisible = true;

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
    updateVisitor = new osgUtil::UpdateVisitor();
    cullVisitor = new osgUtil::CullVisitor();
    stateGraph = new osgUtil::StateGraph();
    renderStage = new osgUtil::RenderStage();

    // Configure the CullVisitor to use the rendering objects we created
    cullVisitor->setStateGraph(stateGraph);
    cullVisitor->setRenderStage(renderStage);

    // Configure the SceneView pipeline
    osgSceneView->setUpdateVisitor(updateVisitor);
    osgSceneView->setCullVisitor(cullVisitor);
    osgSceneView->setStateGraph(stateGraph);
    osgSceneView->setRenderStage(renderStage);

    // Set small feature culling to cull things smaller than a quarter of a
    // pixel.  This should be done on the scene view and not the cull visitor
    // because the scene view will override the cull visitor's settings.
    osgSceneView->setSmallFeatureCullingPixelSize(0.25);

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

    // Disable automatic clipping plane calculation
    osgSceneView->
        setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);

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

    // Make sure all normals are normalized
    defaultState->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

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
        vsObject::unrefDelete(sceneRoot);
    
    // Remove the pane from the its window
    parentWindow->removePane(this);
    
    // If a view has been set, clean up our reference
    if (sceneView != NULL)
    {
        vsObject::unrefDelete(sceneView);
    }
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsPane::getClassName()
{
    return "vsPane";
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
    // If we previously had a different view, clean it up
    if (sceneView != NULL)
    {
        sceneView->unref();
        sceneView = NULL;
    }
    
    // Save the new view object
    sceneView = view;
    
    // If we were given a valid view, reference it, and force the pane to
    // update the viewport based on the new view's settings
    if (sceneView != NULL)
    {
        sceneView->ref();
        viewChangeNum = sceneView->getChangeNum() - 1;
    }
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
    osgDB::DatabasePager *osgDBPager;

    // Unreference the old scene (if any)
    if (sceneRoot != NULL)
    {
        // See if the old scene had a database pager
        osgDBPager = sceneRoot->getDatabasePager();
        if (osgDBPager)
        {
            // Stop using this database pager in our SceneView object
            osgSceneView->getCullVisitor()->setDatabaseRequestHandler(NULL);

            // Tell the old database pager not to compile any GL objects for
            // us anymore
            osgDBPager->setCompileGLObjectsForContextID(
                osgSceneView->getState()->getContextID(), false);
        }

        // Now, unreference the scene
        vsObject::unrefDelete(sceneRoot);
    }

    // Reference the new scene
    if (newScene != NULL)
    {
        // Reference the new scene
        newScene->ref();

        // See if the new scene has a database pager
        osgDBPager = newScene->getDatabasePager();
        if (osgDBPager)
        {
            // Set the database handler on our SceneView's culling traverser
            osgSceneView->getCullVisitor()->
                setDatabaseRequestHandler(osgDBPager);

            // Tell the database pager to compile OpenGL objects for this
            // view's context
            osgDBPager->setCompileGLObjectsForContextID(
                osgSceneView->getState()->getContextID(), true);
        }
    }

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
    osg::Viewport *viewport;
    
    // Get the old settings to obtain the pane's origin
    x = (int)osgSceneView->getViewport()->x();
    y = (int)osgSceneView->getViewport()->y();
    oldWidth = (int)osgSceneView->getViewport()->width();
    oldHeight = (int)osgSceneView->getViewport()->height();

    // Compute the new normalized width and height
    parentWindow->getDrawableSize(&winWidth, &winHeight);
    widthNorm = (double)width / (double)winWidth;
    heightNorm = (double)height / (double)winHeight;

    // The vertical position must be recalculated
    yPosNorm = (double)(y + oldHeight - height) / (double)winHeight;

    // Set the new width and height
    viewport = osgSceneView->getViewport();
    viewport->setViewport(x, y + oldHeight - height, width, height);

    // Modify the view change value so that we recompute the projection
    // parameters on the next drawFrame
    viewChangeNum--;
}

// ------------------------------------------------------------------------
// Retrieves the pixel size of this pane. NULL pointers can be passed in
// for undesired data values.
// ------------------------------------------------------------------------
void vsPane::getSize(int *width, int *height)
{
    int x, y, paneWidth, paneHeight;
    
    // Get the viewport settings
    x = (int)osgSceneView->getViewport()->x();
    y = (int)osgSceneView->getViewport()->y();
    paneWidth = (int)osgSceneView->getViewport()->width();
    paneHeight = (int)osgSceneView->getViewport()->height();
    
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
    osg::Viewport *viewport;

    // Get the current viewport settings to obtain the pane's width and
    // height
    x = (int)osgSceneView->getViewport()->x();
    y = (int)osgSceneView->getViewport()->y();
    width = (int)osgSceneView->getViewport()->width();
    height = (int)osgSceneView->getViewport()->height();

    // Compute the new normalized origin
    parentWindow->getDrawableSize(&winWidth, &winHeight);
    xPosNorm = (double)xPos / (double)winWidth;
    yPosNorm = (double)(winHeight - (yPos + height)) / (double)winHeight;

    // Set the new origin (using the VESS standard upper-left origin)
    viewport = osgSceneView->getViewport();
    viewport->setViewport(xPos, winHeight - (yPos + height), width, height);
}

// ------------------------------------------------------------------------
// Retrieves the location of this pane within its parent window. NULL
// pointers can be passed in for undesired data values.
// ------------------------------------------------------------------------
void vsPane::getPosition(int *xPos, int *yPos)
{
    int x, y, width, height;
    int winWidth, winHeight;
 
    // Obtain the viewport settings
    x = (int)osgSceneView->getViewport()->x();
    y = (int)osgSceneView->getViewport()->y();
    width = (int)osgSceneView->getViewport()->width();
    height = (int)osgSceneView->getViewport()->height();

    // Convert the lower-left-origin-based OSG coordinates to upper-left
    // VESS coordinates. This requires manipulating only the y coordinate.
    parentWindow->getDrawableSize(&winWidth, &winHeight);

    // Return the position in the parameters
    if (xPos != NULL)
        *xPos = x;
    if (yPos != NULL)
        *yPos = (winHeight - (y + height));
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
    parentWindow->getDrawableSize(&winWidth, &winHeight);

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

    // Modify the view change value so that we recompute the projection
    // parameters on the next drawFrame
    viewChangeNum--;
}

// ------------------------------------------------------------------------
// Bring the pane to the front of the window (so it's drawn last)
// ------------------------------------------------------------------------
void vsPane::bringToFront()
{
    // Call the related function on the pane's parent window
    parentWindow->bringPaneToFront(this);
}

// ------------------------------------------------------------------------
// Send the pane to the back of the window (so it's drawn first)
// ------------------------------------------------------------------------
void vsPane::sendToBack()
{
    // Call the related function on the pane's parent window
    parentWindow->sendPaneToBack(this);
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
// Sets the distance between the eyes for stereo visuals
// ------------------------------------------------------------------------
void vsPane::setEyeSeparation(double newSeparation)
{
    osgDisplaySettings->setEyeSeparation(newSeparation);
}

// ------------------------------------------------------------------------
// Returns the current distance between the eyes for stereo visuals
// ------------------------------------------------------------------------
double vsPane::getEyeSeparation()
{
    return osgDisplaySettings->getEyeSeparation();
}

// ------------------------------------------------------------------------
// Makes this pane visible. Panes are visible by default.
// ------------------------------------------------------------------------
void vsPane::showPane()
{
    paneVisible = true;
}

// ------------------------------------------------------------------------
// Makes this pane invisible. Geometry connected only to invisible panes
// is not traversed or rendered.
// ------------------------------------------------------------------------
void vsPane::hidePane()
{
    paneVisible = false;
}

// ------------------------------------------------------------------------
// Sets the color of the pane's background.  The background color is used
// when no earth/sky is enabled.
// ------------------------------------------------------------------------
void vsPane::setBackgroundColor(double r, double g, double b)
{
    // Set up an OSG vector with the given color
    backgroundColor.set((float)r, (float)g, (float)b, 1.0f);
}

// ------------------------------------------------------------------------
// Return the color of the pane's background.  The background color is used 
// when no earth/sky is enabled.
// ------------------------------------------------------------------------
void vsPane::getBackgroundColor(double *r, double *g, double *b)
{
    // Return each color component if the corresponding parameter is valid
    if (r != NULL)
        *r = backgroundColor[0];
    if (g != NULL)
        *g = backgroundColor[1];
    if (b != NULL)
        *b = backgroundColor[2];
}

// ------------------------------------------------------------------------
// Enable drawing of statistical information in the pane
// ------------------------------------------------------------------------
void vsPane::enableStats()
{
    // No support for frame statistics in OSG
    printf("vsPane::enableStats():  Stats not supported under Open Scene "
        "Graph\n");
}

// ------------------------------------------------------------------------
// Disable drawing of statistical information in the pane
// ------------------------------------------------------------------------
void vsPane::disableStats()
{
}

// ------------------------------------------------------------------------
// Returns whether or not frame statistics are set to be drawn
// ------------------------------------------------------------------------
bool vsPane::areStatsEnabled()
{
    // No support for stats under OSG, so always return false
    return false;
}

// ------------------------------------------------------------------------
// Sets the bit mask that OSG uses when it's clearing the pane's buffer
// before rendering a scene
// ------------------------------------------------------------------------
void vsPane::setGLClearMask(int clearMask)
{
    osgSceneView->getCamera()->setClearMask(clearMask);
}

// ------------------------------------------------------------------------
// Gets the bit mask that OSG uses when it's clearing the pane's buffer
// before rendering a scene
// ------------------------------------------------------------------------
int vsPane::getGLClearMask()
{
    return (osgSceneView->getCamera()->getClearMask());
}

// ------------------------------------------------------------------------
// Sets the current level-of-detail scale factor.  A value of 2.0 computes
// LOD's as if they were being viewed from twice as far away.  A value of
// 0.0 always shows the highest LOD.
// ------------------------------------------------------------------------
void vsPane::setLODScale(double newScale)
{
    osgSceneView->setLODScale((float)newScale);
}

// ------------------------------------------------------------------------
// Returns the current LOD scale setting
// ------------------------------------------------------------------------
double vsPane::getLODScale()
{
    return osgSceneView->getLODScale();
}

//-------------------------------------------------------------------------
// Returns the projection matrix from the current OSG SceneView
//-------------------------------------------------------------------------
atMatrix vsPane::getProjectionMatrix()
{
    osg::Matrixd projMat;
    int i, j;
    atMatrix result;

    // Retrieve the projection matrix from OSG
    projMat = osgSceneView->getProjectionMatrix();

    // Transpose the matrix to get a atMatrix (OSG flips them as
    // compared to VESS)
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            result[i][j] = projMat(j,i);

    // Return the atMatrix
    return result;
}

// ------------------------------------------------------------------------
// Projects a point in world coordinates onto the pane, returning the
// normalized pane coordinates
// ------------------------------------------------------------------------
atVector vsPane::projectPointOntoPane(atVector worldXYZ)
{
    atMatrix projectionMat;
    atVector viewpoint;
    atMatrix translation;
    atMatrix rotation;
    atMatrix viewMat;
    atMatrix openGLXform;
    atMatrix mvp;
    atVector worldPoint, panePoint;
    atVector paneXYZ;

    // If we have no view, we can't do any projections
    if (sceneView == NULL)
        return atVector(0.0, 0.0, 0.0);

    // Get the projection matrix
    projectionMat = getProjectionMatrix();
    
    // Get the view matrix, including the necessary transform from VESS to
    // OpenGL coordinates
    viewpoint = sceneView->getViewpoint();
    translation.setTranslation(viewpoint[AT_X], viewpoint[AT_Y],
        viewpoint[AT_Z]);
    rotation = sceneView->getRotationMat();
    openGLXform.clear();
    openGLXform[0][0] = 1.0;
    openGLXform[1][2] = -1.0;
    openGLXform[2][1] = 1.0;
    openGLXform[3][3] = 1.0;
    viewMat = translation * rotation * openGLXform;
    viewMat.invert();

    // Create a point in homogeneous coordinates from the input point
    worldPoint.setSize(4);
    worldPoint.clearCopy(worldXYZ);
    worldPoint[AT_W] = 1.0;

    // Compute the ModelViewProjection (MVP) matrix (the model matrix is
    // identity here, because we require the input point to be in world
    // coordinates)
    mvp = projectionMat * viewMat; 

    // Transform the point by the MVP matrix, and then divide by the
    // homogeneous W coordinate.  This results in coordinates between -1.0
    // and 1.0 for all three dimensions
    panePoint = mvp.getFullXform(worldPoint);
    paneXYZ[AT_X] = panePoint[AT_X] / panePoint[AT_W];
    paneXYZ[AT_Y] = panePoint[AT_Y] / panePoint[AT_W];
    paneXYZ[AT_Z] = panePoint[AT_Z] / panePoint[AT_W];

    // Return the normalized coordinates
    return paneXYZ;
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
bool vsPane::isVisible()
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
    atMatrix viewMatrix, xformMatrix;
    osg::Matrixd osgMatrix;
    atVector viewPos;
    int projMode;
    double projHval, projVval;
    double hFOV, vFOV;
    int paneWidth, paneHeight;
    double aspectMatch;
    vsViewpointAttribute *viewAttr;
    atVector eye;
    atVector fwd;
    atVector up;
    atVector side;
    osg::Vec3d osgEye;
    osg::Matrixd osgViewMatrix;
    osg::Matrixd osgEyeMatrix;
    double nearClipDist, farClipDist;
    double projLeft, projRight, projBottom, projTop;
    
    // Do nothing if no vsView is attached
    if (sceneView == NULL)
        return;

    // If a viewpoint attribute is attached, update it
    viewAttr = (vsViewpointAttribute *)
        ((vsViewpointAttribute::getMap())->mapFirstToSecond(sceneView));
    if (viewAttr)
        viewAttr->update();
    
    // Get the width and height of the pane, as well as the Z-clip distances
    getSize(&paneWidth, &paneHeight);
    sceneView->getProjectionData(&projMode, &projHval, &projVval);
    sceneView->getClipDistances(&nearClipDist, &farClipDist);

    // Determine if the settings in the view object have changed, and update
    // the OSG SceneView if they have. This determination is done by comparing
    // the last known 'change number' of the view object to the current one;
    // the view object has changed if the change number changed.
    if (sceneView->getChangeNum() != viewChangeNum)
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
                hFOV = AT_DEG2RAD(VS_PANE_DEFAULT_FOV);
                vFOV = 2 * atan2( tan(0.5 * hFOV), aspectMatch);
                hFOV = AT_RAD2DEG(hFOV);
                vFOV = AT_RAD2DEG(vFOV);
            }
            else if (projHval <= 0.0)
            {
                // Vertical FOV specified, but no horizontal, so 
                // aspect-match horizontal to vertical
                vFOV = AT_DEG2RAD(projVval);
                hFOV = 2 * atan(tan(0.5 * vFOV) * aspectMatch);
                hFOV = AT_RAD2DEG(hFOV);
                vFOV = AT_RAD2DEG(vFOV);
            }
            else if (projVval <= 0.0)
            {
                // Horizontal FOV specified, but no vertical, so 
                // aspect-match vertical to horizontal
                hFOV = AT_DEG2RAD(projHval);
                vFOV = 2 * atan2( tan(0.5 * hFOV), aspectMatch);
                hFOV = AT_RAD2DEG(hFOV);
                vFOV = AT_RAD2DEG(vFOV);
            }
            else
            {
                // Both FOV's specified, so set accordingly
                hFOV = projHval;
                vFOV = projVval;

                aspectMatch = hFOV / vFOV;
            }

            // Set the OSG camera to the new FOV values
            osgSceneView->setProjectionMatrixAsPerspective(vFOV, aspectMatch,
                nearClipDist, farClipDist);
        }
        else if (projMode == VS_VIEW_PROJMODE_ORTHO)
        {
            // Check the horizontal and vertical values to see if and which
            // parameters are set to default values.
            if ((projHval <= 0.0) && (projVval <= 0.0))
            {
                // Neither specified, use default values
                osgSceneView->setProjectionMatrixAsOrtho(
                    -VS_PANE_DEFAULT_ORTHO_PLANE, VS_PANE_DEFAULT_ORTHO_PLANE,
                    -VS_PANE_DEFAULT_ORTHO_PLANE, VS_PANE_DEFAULT_ORTHO_PLANE,
                    nearClipDist, farClipDist);
            }
            else if (projHval <= 0.0)
            {
                // Vertical specified, horizontal aspect match
                aspectMatch = (projVval / (double)paneHeight) *
                    (double)paneWidth;
                osgSceneView->setProjectionMatrixAsOrtho(-aspectMatch,
                    aspectMatch, -projVval, projVval, nearClipDist,
                    farClipDist);
            }
            else if (projVval <= 0.0)
            {
                // Horizontal specified, vertical aspect match
                aspectMatch = (projHval / (double)paneWidth) *
                    (double)paneHeight;
                osgSceneView->setProjectionMatrixAsOrtho(-projHval, projHval,
                    -aspectMatch, aspectMatch, nearClipDist, farClipDist);
            }
            else
            {
                // Both specified, set values explicitly
                osgSceneView->setProjectionMatrixAsOrtho(-projHval, projHval,
                    -projVval, projVval, nearClipDist, farClipDist);
            }
        }
        else
        {
            // This is an off-axis projection, get the four sides of the
            // viewing volume
            sceneView->getOffAxisProjectionData(&projLeft, &projRight,
                &projBottom, &projTop);

            // Set the OSG scene view to use the off-axis projection
            // matrix
            osgSceneView->setProjectionMatrixAsFrustum(projLeft, projRight,
                projBottom, projTop, nearClipDist, farClipDist);
        }

        // Get the current view position and orientation
        eye = sceneView->getViewpoint();
        fwd = sceneView->getDirection();
        up = sceneView->getUpDirection();

        // Calculate the rest of the view's coordinate system
        side = fwd.getCrossProduct(up);
        side.normalize();

        // Make sure the up vector is really orthogonal to the other two
        up = side.getCrossProduct(fwd);
        up.normalize();

        // Set up the view matrix in OSG's format
        osgViewMatrix.set(side[0],   up[0], -fwd[0], 0.0,
                          side[1],   up[1], -fwd[1], 0.0,
                          side[2],   up[2], -fwd[2], 0.0,
                              0.0,     0.0,     0.0, 1.0);

        // Add in the eye translation
        osgEye.set(eye[AT_X], eye[AT_Y], eye[AT_Z]);
        osgEyeMatrix.makeTranslate(-osgEye);
        osgViewMatrix.preMult(osgEyeMatrix);

        // Set the scene's view matrix
        osgSceneView->setViewMatrix(osgViewMatrix);

        // Record the change number
        viewChangeNum = sceneView->getChangeNum();
    }
}

// ------------------------------------------------------------------------
// Internal function
// Updates the background color with the Earth/Sky color specified in the
// vsScene object.
// ------------------------------------------------------------------------
void vsPane::updateClearState()
{
    double esUniform[3];
    osg::Vec4 esClearColor;

    // See if the scene has EarthSky enabled.
    if ((sceneRoot != NULL) && (sceneRoot->isEarthSkyEnabled()))
    {
        sceneRoot->getESColor(VS_SCENE_ESCOLOR_UNIFORM, &(esUniform[0]),
            &(esUniform[1]), &(esUniform[2]));
        esClearColor[0] = esUniform[0];
        esClearColor[1] = esUniform[1];
        esClearColor[2] = esUniform[2];
        esClearColor[3] = 1.0;
        osgSceneView->setClearColor(esClearColor);
    }
    else
    {
        // Set the background color of the OSG SceneView object
        osgSceneView->setClearColor(backgroundColor);
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
    osg::Viewport *viewport;

    // Get the current size of the window
    parentWindow->getDrawableSize(&winWidth, &winHeight);

    // Compute the position and size of the pane in local window coordinates
    x = (int)(xPosNorm * (double)winWidth);
    y = (int)(yPosNorm * (double)winHeight);
    width = (int)(widthNorm * (double)winWidth), 
    height = (int)(heightNorm * (double)winHeight);

    // Update the OSG SceneView's viewport
    viewport = osgSceneView->getViewport();
    viewport->setViewport(x, y, width, height);
}

