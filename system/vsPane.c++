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
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsPane.h++"

#include <Performer/pr/pfLight.h>
#include "vsGeometry.h++"
#include "vsComponent.h++"

// ------------------------------------------------------------------------
// Constructor - Creates and connects the underlying Performer objects that
// this pane manages. Also configures some default rendering settings.
// ------------------------------------------------------------------------
vsPane::vsPane(vsWindow *parent)
{
    vsScreen *parentScreen;
    vsPipe *parentPipe;
    pfPipeWindow *tempPWin;
    pfGeoState *defaultState;
    pfLightModel *lightModel;

    sceneRoot = NULL;
    sceneView = NULL;

    parentWindow = parent;
    parentScreen = parent->getParentScreen();
    parentPipe = parentScreen->getParentPipe();

    performerChannel = new pfChannel(parentPipe->getBaseLibraryObject());
    performerChannel->ref();

    // Performer automatically assigns a new channel to the first window
    // on the specified pipe; I'd rather do it myself.
    tempPWin = performerChannel->getPWin();
    if (tempPWin)
        tempPWin->removeChan(performerChannel);

    // Normally, this will be a monovision pane, so there is no need for
    // shared data
    bufferMode = VS_PANE_BUFFER_MONO;
    sharedData = NULL;

    parentWindow->addPane(this);

    performerScene = new pfScene();
    performerScene->ref();

    // Create the global geostate settings
    defaultState = new pfGeoState();
    defaultState->makeBasic();

    defaultState->setMode(PFSTATE_DECAL,
        PFDECAL_BASE_DISPLACE | PFDECAL_LAYER_OFFSET);
    defaultState->setMode(PFSTATE_CULLFACE, PFCF_BACK);
    defaultState->setMode(PFSTATE_ENLIGHTING, PF_ON);
    defaultState->setMode(PFSTATE_SHADEMODEL, PFSM_GOURAUD);
    defaultState->setMode(PFSTATE_ALPHAFUNC, PFAF_GREATER);
    defaultState->setVal(PFSTATE_ALPHAREF, 0.0);

    lightModel = new pfLightModel();
    lightModel->setLocal(PF_ON);
    lightModel->setTwoSide(PF_OFF);
    lightModel->setAmbient(0.0, 0.0, 0.0);
    defaultState->setAttr(PFSTATE_LIGHTMODEL, lightModel);

    performerScene->setGState(defaultState);

    performerChannel->setScene(performerScene);

    // Set up the earth/sky model
    earthSky = new pfEarthSky();
    earthSky->setAttr(PFES_GRND_HT, -100.0);
    performerChannel->setESky(earthSky);

    // Initialize the 'current view' parameters
    curNearClip = -1.0;
    curFarClip = -1.0;
    curProjMode = VS_VIEW_PROJMODE_PERSP;
    curProjHval = -1.0;
    curProjVval = -1.0;
    performerChannel->setFOV(-1.0, -1.0);
}

// ------------------------------------------------------------------------
// Destructor - Deletes the associated Performer objects
// ------------------------------------------------------------------------
vsPane::~vsPane()
{
    performerChannel->setScene(NULL);
    performerChannel->unref();
    // pfChannels can't be deleted
    performerScene->unref();
    pfDelete(performerScene);
    
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
    sceneView = view;
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
void vsPane::setScene(vsComponent *newScene)
{
    pfNode *childNode;

    if (performerScene->getNumChildren() > 0)
    {
        childNode = performerScene->getChild(0);
            performerScene->replaceChild(childNode,
                ((vsComponent *)newScene)->getBaseLibraryObject());
    }
    else
    {
	performerScene->addChild(
	    ((vsComponent *)newScene)->getBaseLibraryObject());
    }
    
    sceneRoot = newScene;
}

// ------------------------------------------------------------------------
// Retrieves the root node of the geometry being displayed in this pane
// ------------------------------------------------------------------------
vsComponent *vsPane::getScene()
{
    return sceneRoot;
}

// ------------------------------------------------------------------------
// Sets the pixel size of this pane within its parent window
// ------------------------------------------------------------------------
void vsPane::setSize(int width, int height)
{
    float left, right, bottom, top;
    int winWidth, winHeight;
    float widthFraction, heightFraction;
    
    performerChannel->getViewport(&left, &right, &bottom, &top);

    parentWindow->getSize(&winWidth, &winHeight);
    widthFraction = (float)width / (float)winWidth;
    heightFraction = (float)height / (float)winHeight;
    
    performerChannel->setViewport(left, left + widthFraction,
        top - heightFraction, top);
}

// ------------------------------------------------------------------------
// Retrieves the pixel size of this pane. NULL pointers can be passed in
// for undesired data values.
// ------------------------------------------------------------------------
void vsPane::getSize(int *width, int *height)
{
    int x, y;
    
    performerChannel->getSize(&x, &y);
    
    if (width)
        *width = x;
    if (height)
        *height = y;
}

// ------------------------------------------------------------------------
// Sets the location, in pixels, of this pane within its parent window
// ------------------------------------------------------------------------
void vsPane::setPosition(int xPos, int yPos)
{
    float left, right, bottom, top;
    int winWidth, winHeight;
    float xPosFraction, yPosFraction;

    performerChannel->getViewport(&left, &right, &bottom, &top);
    
    parentWindow->getSize(&winWidth, &winHeight);
    xPosFraction = (float)xPos / (float)winWidth;
    yPosFraction = 1.0f - ((float)yPos / (float)winHeight);
    
    performerChannel->setViewport(xPosFraction,
        xPosFraction + (right - left), yPosFraction - (top - bottom),
        yPosFraction);
}

// ------------------------------------------------------------------------
// Retrieves the location of this pane within its parent window. NULL
// pointers can be passed in for undesired data values.
// ------------------------------------------------------------------------
void vsPane::getPosition(int *xPos, int *yPos)
{
    int x, y;
    
    performerChannel->getOrigin(&x, &y);
    
    if (xPos)
        *xPos = x;
    if (yPos)
        *yPos = y;
}

// ------------------------------------------------------------------------
// Automaticially configures the size and location of the pane within
// its parent window, based on the placement constant passed in.
// ------------------------------------------------------------------------
void vsPane::autoConfigure(int panePlacement)
{
    // Y coordinate inverted and scaled to 0.0 - 1.0 for
    // Performer's benefit

    switch (panePlacement)
    {
        case VS_PANE_PLACEMENT_FULL_WINDOW:
            performerChannel->setViewport(0.0, 1.0, 0.0, 1.0);
            break;
        case VS_PANE_PLACEMENT_TOP_HALF:
            performerChannel->setViewport(0.0, 1.0, 0.5, 1.0);
            break;
        case VS_PANE_PLACEMENT_BOTTOM_HALF:
            performerChannel->setViewport(0.0, 1.0, 0.0, 0.5);
            break;
        case VS_PANE_PLACEMENT_LEFT_HALF:
            performerChannel->setViewport(0.0, 0.5, 0.0, 1.0);
            break;
        case VS_PANE_PLACEMENT_RIGHT_HALF:
            performerChannel->setViewport(0.5, 1.0, 0.0, 1.0);
            break;
        case VS_PANE_PLACEMENT_TOP_LEFT_QUADRANT:
            performerChannel->setViewport(0.0, 0.5, 0.5, 1.0);
            break;
        case VS_PANE_PLACEMENT_TOP_RIGHT_QUADRANT:
            performerChannel->setViewport(0.5, 1.0, 0.5, 1.0);
            break;
        case VS_PANE_PLACEMENT_BOTTOM_RIGHT_QUADRANT:
            performerChannel->setViewport(0.5, 1.0, 0.0, 0.5);
            break;
        case VS_PANE_PLACEMENT_BOTTOM_LEFT_QUADRANT:
            performerChannel->setViewport(0.0, 0.5, 0.0, 0.5);
            break;
        default:
            printf("vsPane::autoConfigure: Invalid parameter");
            break;
    }
}

// ------------------------------------------------------------------------
// Sets the buffer mode of this pane.  If newMode specifies a stereo mode
// this will also register a Performer DRAW process callback.
// ------------------------------------------------------------------------
void vsPane::setBufferMode(vsPaneBufferMode newMode)
{
    if ((newMode == VS_PANE_BUFFER_STEREO_L) ||
        (newMode == VS_PANE_BUFFER_STEREO_R))
    {
        if (bufferMode == VS_PANE_BUFFER_MONO)
        {
            // Allocate a chunk of shared memory for the shared data
            sharedData = (vsPaneSharedData *)
                performerChannel->allocChanData(sizeof(vsPaneSharedData));

            // Set the DRAW process callback
            performerChannel->setTravFunc(PFTRAV_DRAW, vsPane::drawPane);
        }

        // Change the buffer mode
        bufferMode = newMode;

        // Set the buffer mode in the shared data block and pass it
        // to the DRAW process
        sharedData->bufferMode = newMode;
        performerChannel->passChanData();
    }
    else
    {
        if ((bufferMode == VS_PANE_BUFFER_STEREO_L) ||
            (bufferMode == VS_PANE_BUFFER_STEREO_R))
        {
            // Detach the channel DRAW callback
            performerChannel->setTravFunc(PFTRAV_DRAW, NULL);

            // Stop passing channel data
            if (sharedData != NULL)
            {
                performerChannel->setChanData(NULL, 0);

                // Deallocate the channel data
                pfDelete(sharedData);
                sharedData = NULL;
            }
        }

        // Change the buffer mode
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
// Sets the visibility mask for this pane. During the culling portion
// of a frame drawing cycle, a bitwise AND of the pane's visibility mask
// and each node's visibility value is performed; if the result of the AND
// is zero, that node (and all other nodes under it) are culled, not to be
// drawn.
// ------------------------------------------------------------------------
void vsPane::setVisibilityMask(unsigned int newMask)
{
    performerChannel->setTravMask(PFTRAV_DRAW, newMask);
}

// ------------------------------------------------------------------------
// Gets the visibility mask for this pane.
// ------------------------------------------------------------------------
unsigned int vsPane::getVisibilityMask()
{
    return (performerChannel->getTravMask(PFTRAV_DRAW));
}

// ------------------------------------------------------------------------
// Makes this pane visible. Panes are visible by default.
// ------------------------------------------------------------------------
void vsPane::showPane()
{
    performerChannel->setTravMode(PFTRAV_DRAW, PFDRAW_ON);
}

// ------------------------------------------------------------------------
// Makes this pane invisible. Geometry connected only to invisible panes
// is not traversed or rendered.
// ------------------------------------------------------------------------
void vsPane::hidePane()
{
    performerChannel->setTravMode(PFTRAV_DRAW, PFDRAW_OFF);
}

// ------------------------------------------------------------------------
// Enables drawing of the earth/sky background in this channel
// ------------------------------------------------------------------------
void vsPane::enableEarthSky()
{
    earthSky->setMode(PFES_BUFFER_CLEAR, PFES_SKY_GRND);
}

// ------------------------------------------------------------------------
// Disables drawing of the earth/sky background in this channel
// ------------------------------------------------------------------------
void vsPane::disableEarthSky()
{
    earthSky->setMode(PFES_BUFFER_CLEAR, PFES_FAST);
}

// ------------------------------------------------------------------------
// Sets the altitude of the ground plane in the earth/sky background
// ------------------------------------------------------------------------
void vsPane::setESGroundHeight(double newHeight)
{
    earthSky->setAttr(PFES_GRND_HT, newHeight);
}

// ------------------------------------------------------------------------
// Retrieves the altitude of the ground plane in the earth/sky background
// ------------------------------------------------------------------------
double vsPane::getESGroundHeight()
{
    return (earthSky->getAttr(PFES_GRND_HT));
}

// ------------------------------------------------------------------------
// Sets the aspect of the earth/sky background color specified by which to
// the specified color
// ------------------------------------------------------------------------
void vsPane::setESColor(int which, double r, double g, double b)
{
    switch (which)
    {
        case VS_PANE_ESCOLOR_SKY_NEAR:
            earthSky->setColor(PFES_SKY_TOP, r, g, b, 1.0);
            break;
        case VS_PANE_ESCOLOR_SKY_FAR:
            earthSky->setColor(PFES_SKY_BOT, r, g, b, 1.0);
            break;
        case VS_PANE_ESCOLOR_SKY_HORIZON:
            earthSky->setColor(PFES_HORIZ, r, g, b, 1.0);
            break;
        case VS_PANE_ESCOLOR_GROUND_FAR:
            earthSky->setColor(PFES_GRND_FAR, r, g, b, 1.0);
            break;
        case VS_PANE_ESCOLOR_GROUND_NEAR:
            earthSky->setColor(PFES_GRND_NEAR, r, g, b, 1.0);
            break;
        default:
            printf("vsPane::setESColor: Invalid color constant\n");
            break;
    }
}

// ------------------------------------------------------------------------
// Retrieves the aspect of the earth/sky background color specified by
// which. NULL pointers may be passed in for unneeded return values.
// ------------------------------------------------------------------------
void vsPane::getESColor(int which, double *r, double *g, double *b)
{
    float fr, fg, fb, fa;

    switch (which)
    {
        case VS_PANE_ESCOLOR_SKY_NEAR:
            earthSky->getColor(PFES_SKY_TOP, &fr, &fg, &fb, &fa);
            break;
        case VS_PANE_ESCOLOR_SKY_FAR:
            earthSky->getColor(PFES_SKY_BOT, &fr, &fg, &fb, &fa);
            break;
        case VS_PANE_ESCOLOR_SKY_HORIZON:
            earthSky->getColor(PFES_HORIZ, &fr, &fg, &fb, &fa);
            break;
        case VS_PANE_ESCOLOR_GROUND_FAR:
            earthSky->getColor(PFES_GRND_FAR, &fr, &fg, &fb, &fa);
            break;
        case VS_PANE_ESCOLOR_GROUND_NEAR:
            earthSky->getColor(PFES_GRND_NEAR, &fr, &fg, &fb, &fa);
            break;
        default:
            printf("vsPane::getESColor: Invalid color constant\n");
            fr = fg = fb = 0.0;
            break;
    }
    
    if (r)
        *r = fr;
    if (g)
        *g = fg;
    if (b)
        *b = fb;
}

// ------------------------------------------------------------------------
// Returns the Performer object associated with this object
// ------------------------------------------------------------------------
pfChannel *vsPane::getBaseLibraryObject()
{
    return performerChannel;
}

// ------------------------------------------------------------------------
// VESS internal function
// Updates the Performer view matrix with the information contained within
// this pane's viewpoint (vsView) object.
// ------------------------------------------------------------------------
void vsPane::updateView()
{
    vsMatrix viewMatrix, xformMatrix;
    pfMatrix performerMatrix;
    int loop, sloop;
    vsVector viewPos;
    double near, far;
    int projMode;
    double projHval, projVval;
    int paneWidth, paneHeight;
    double aspectMatch;
    
    if (sceneView == NULL)
        return;

    sceneView->updateFromAttribute();
    
    viewMatrix = sceneView->getRotationMat();
    viewPos = sceneView->getViewpoint();
    for (loop = 0; loop < 3; loop++)
        viewMatrix[loop][3] = viewPos[loop];

    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            performerMatrix[loop][sloop] = viewMatrix[sloop][loop];

    performerChannel->setViewMat(performerMatrix);

    // Update the viewing volume parameters in case they changed
    sceneView->getClipDistances(&near, &far);
    if ((curNearClip != near) || (curFarClip != far))
    {
	performerChannel->setNearFar(near, far);
	curNearClip = near;
	curFarClip = far;
    }

    sceneView->getProjectionData(&projMode, &projHval, &projVval);
    if ((curProjMode != projMode) || (curProjHval != projHval) ||
	(curProjVval != projVval))
    {
	if (projMode == VS_VIEW_PROJMODE_PERSP)
	    performerChannel->setFOV(projHval, projVval);
	else
	{
	    if ((projHval <= 0.0) && (projVval <= 0.0))
	    {
		// Neither specified, default values
		performerChannel->makeOrtho(-10.0, 10.0, -10.0, 10.0);
	    }
	    else if (projHval <= 0.0)
	    {
		// Vertical specified, horizontal aspect match
		getSize(&paneWidth, &paneHeight);
		aspectMatch = (projVval / (double)paneHeight) *
		    (double)paneWidth;
		performerChannel->makeOrtho(-aspectMatch, aspectMatch,
		    -projVval, projVval);
	    }
	    else if (projVval <= 0.0)
	    {
		// Horizontal specified, vertical aspect match
		getSize(&paneWidth, &paneHeight);
		aspectMatch = (projHval / (double)paneWidth) *
		    (double)paneHeight;
		performerChannel->makeOrtho(-projHval, projHval, -aspectMatch,
		    aspectMatch);
	    }
	    else
	    {
		// Both specified, normal operation
		performerChannel->makeOrtho(-projHval, projHval, -projVval,
		    projVval);
	    }
	}
	
	curProjMode = projMode;
	curProjHval = projHval;
	curProjVval = projVval;
    }
}

// ------------------------------------------------------------------------
// static VESS internal function - Performer callback
// Pre-DRAW callback to select which OpenGL buffer to draw the scene into
// prior to actually drawing the scene.  Note that this function is not
// called unless a VS_PANE_BUFFER_STEREO_* buffer mode is set (via
// setBufferMode())
// ------------------------------------------------------------------------
void vsPane::drawPane(pfChannel *chan, void *userData)
{
    vsPaneSharedData *paneData;

    paneData = (vsPaneSharedData *)userData;

    // Select the appropriate buffer to use
    if (paneData->bufferMode == VS_PANE_BUFFER_STEREO_L)
        glDrawBuffer(GL_BACK_LEFT);
    else if (paneData->bufferMode == VS_PANE_BUFFER_STEREO_R)
        glDrawBuffer(GL_BACK_RIGHT);
    else
        glDrawBuffer(GL_BACK);

    chan->clear();

    pfDraw();
}
