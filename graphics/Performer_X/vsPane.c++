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
#include "vsViewpointAttribute.h++"

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

    // Start with no scene and no view object
    sceneRoot = NULL;
    sceneView = NULL;

    // Save the parent window pointer, and get its parent pipe and
    // screen objects
    parentWindow = parent;
    parentScreen = parent->getParentScreen();
    parentPipe = parentScreen->getParentPipe();

    // Create a new Performer channel using the parent pipe's pfPipe object
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

    // Add this pane to the parent window's child pane list
    parentWindow->addPane(this);

    // Initialize the channel's scene to nothing
    performerChannel->setScene(NULL);

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
    // Remove the Performer channel (pfChannels can't be deleted)
    performerChannel->setScene(NULL);
    performerChannel->unref();

    // Unreference the sceneRoot node
    if (sceneRoot != NULL)
        sceneRoot->unref();
    
    // Remove the pane from its window
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
void vsPane::setScene(vsScene *newScene)
{
    // Reference the new scene
    if (newScene != NULL)
        newScene->ref();

    // Unreference the old one
    if (sceneRoot != NULL)
        sceneRoot->unref();

    // Set the new scene as the pane's scene
    sceneRoot = newScene;

    // Set the channel's scene to use the new scene
    if (newScene != NULL)
        performerChannel->setScene(newScene->getBaseLibraryObject());
    else
        performerChannel->setScene(NULL);
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
    float left, right, bottom, top;
    int winWidth, winHeight;
    float widthFraction, heightFraction;

    // Get the current dimensions of the Performer channel
    performerChannel->getViewport(&left, &right, &bottom, &top);

    // Convert from pixel sizes to the fraction-of-a-window sizes
    // that Performer likes
    parentWindow->getSize(&winWidth, &winHeight);
    widthFraction = (float)width / (float)winWidth;
    heightFraction = (float)height / (float)winHeight;
    
    // Set the dimensions of the Performer channel
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
    
    // Get the size of the Performer channel
    performerChannel->getSize(&x, &y);
    
    // Return the desired values
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

    // Get the current dimensions of the Performer channel
    performerChannel->getViewport(&left, &right, &bottom, &top);
    
    // Convert from pixel sizes to the fraction-of-the-screen sizes
    // that Performer likes
    parentWindow->getSize(&winWidth, &winHeight);
    xPosFraction = (float)xPos / (float)winWidth;
    yPosFraction = 1.0f - ((float)yPos / (float)winHeight);
    
    // Set the dimensions of the Performer channel
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
    
    // Get the position of the Performer channel
    performerChannel->getOrigin(&x, &y);
    
    // Return the desired values
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

    // Interpret the placement constant
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
    // Check to see if we're using a stereo buffer mode
    if ((newMode == VS_PANE_BUFFER_STEREO_L) ||
        (newMode == VS_PANE_BUFFER_STEREO_R))
    {
        // If the current buffer mode is mono, we need to create a segment
        // of shared memory for the Performer channel, so that we can pass
        // the current buffer to the Performer draw callback.
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
        // to the DRAW process, so that Performer knows which buffer
        // to draw into.
        sharedData->bufferMode = newMode;
        performerChannel->passChanData();
    }
    else
    {
        // We're trying to switch to mono mode, see if we're currently in 
        // a stereo mode.
        if ((bufferMode == VS_PANE_BUFFER_STEREO_L) ||
            (bufferMode == VS_PANE_BUFFER_STEREO_R))
        {
            // Detach the channel DRAW callback
            performerChannel->setTravFunc(PFTRAV_DRAW, NULL);

            // Make sure the shared channel data exists before we try to
            // delete it
            if (sharedData != NULL)
            {
                // Stop passing channel data
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
// Makes this pane visible. Panes are visible by default.
// ------------------------------------------------------------------------
void vsPane::showPane()
{
    // Activate the draw traversal for this Performer channel
    performerChannel->setTravMode(PFTRAV_DRAW, PFDRAW_ON);
}

// ------------------------------------------------------------------------
// Makes this pane invisible. Geometry connected only to invisible panes
// is not traversed or rendered.
// ------------------------------------------------------------------------
void vsPane::hidePane()
{
    // Deactivate the draw traversal for this Performer channel
    performerChannel->setTravMode(PFTRAV_DRAW, PFDRAW_OFF);
}

// ------------------------------------------------------------------------
// Sets the color of the pane's background.  The background color is used
// when no earth/sky is enabled.
// ------------------------------------------------------------------------
void vsPane::setBackgroundColor(double r, double g, double b)
{
    // Set the color of the CLEAR mode of the Performer earth/sky to
    // specified color
    earthSky->setColor(PFES_CLEAR, r, g, b, 1.0);
}

// ------------------------------------------------------------------------
// Return the color of the pane's background.  The background color is used
// when no earth/sky is enabled.
// ------------------------------------------------------------------------
void vsPane::getBackgroundColor(double *r, double *g, double *b)
{
    float red, green, blue, alpha;

    // Get the color of the CLEAR mode of the Performer earth/sky
    earthSky->getColor(PFES_CLEAR, &red, &green, &blue, &alpha);

    // Return the color components with valid corresponding parameters
    if (r != NULL)
        *r = (double)red;
    if (g != NULL)
        *g = (double)green;
    if (b != NULL)
        *b = (double)blue;
}

// ------------------------------------------------------------------------
// Enables drawing of the earth/sky background in this channel
// ------------------------------------------------------------------------
void vsPane::enableEarthSky()
{
    // Set the Performer earth/sky clear mode to draw the earth and sky
    earthSky->setMode(PFES_BUFFER_CLEAR, PFES_SKY_GRND);
}

// ------------------------------------------------------------------------
// Disables drawing of the earth/sky background in this channel
// ------------------------------------------------------------------------
void vsPane::disableEarthSky()
{
    // Set the Performer earth/sky clear mode to clear to black
    earthSky->setMode(PFES_BUFFER_CLEAR, PFES_FAST);
}

// ------------------------------------------------------------------------
// Sets the altitude of the ground plane in the earth/sky background
// ------------------------------------------------------------------------
void vsPane::setESGroundHeight(double newHeight)
{
    // Set the Performer earth/sky ground height to the specified value
    earthSky->setAttr(PFES_GRND_HT, newHeight);
}

// ------------------------------------------------------------------------
// Retrieves the altitude of the ground plane in the earth/sky background
// ------------------------------------------------------------------------
double vsPane::getESGroundHeight()
{
    // Get the ground height from the Performer earth/sky
    return (earthSky->getAttr(PFES_GRND_HT));
}

// ------------------------------------------------------------------------
// Sets the aspect of the earth/sky background color specified by which to
// the specified color
// ------------------------------------------------------------------------
void vsPane::setESColor(int which, double r, double g, double b)
{
    // Set the specified color of the Performer earth/sky
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

    // Get the specified color from the Performer earth/sky
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
    
    // Return the desired values
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
    vsViewpointAttribute *viewAttr;
    
    // If there's a not vsView object, then there's nothing to do
    if (sceneView == NULL)
        return;

    // Get the view attribute from the viewObjectMap
    viewAttr = (vsViewpointAttribute *)
	((vsViewpointAttribute::getMap())->mapFirstToSecond(sceneView));

    // Update the viewpoint attribute
    if (viewAttr)
	viewAttr->update();
    
    // Get the view position and orientation
    viewMatrix = sceneView->getRotationMat();
    viewPos = sceneView->getViewpoint();
    for (loop = 0; loop < 3; loop++)
        viewMatrix[loop][3] = viewPos[loop];

    // Copy the view matrix to the Performer channel
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

    // Get the projection data from the view object and check to see if
    // it has changed since the last time we looked
    sceneView->getProjectionData(&projMode, &projHval, &projVval);
    if ((curProjMode != projMode) || (curProjHval != projHval) ||
	(curProjVval != projVval))
    {
        // Set the new projection based on the new mode
	if (projMode == VS_VIEW_PROJMODE_PERSP)
	    performerChannel->setFOV(projHval, projVval);
	else
	{
            // Determine which projection values are specified, and
	    // which must be assumed
	    if ((projHval <= 0.0) && (projVval <= 0.0))
	    {
		// Neither specified, default values
		performerChannel->makeOrtho(-10.0, 10.0, -10.0, 10.0);
	    }
	    else if (projHval <= 0.0)
	    {
		// Vertical specified, horizontal aspect match

                // Calculate the horizontal size from the vertical size
		// and the dimensions of the pane
		getSize(&paneWidth, &paneHeight);
		aspectMatch = (projVval / (double)paneHeight) *
		    (double)paneWidth;

                // Call Performer with the new aspect data
		performerChannel->makeOrtho(-aspectMatch, aspectMatch,
		    -projVval, projVval);
	    }
	    else if (projVval <= 0.0)
	    {
		// Horizontal specified, vertical aspect match

                // Calculate the vertical size from the horizontal size
		// and the dimensions of the pane
		getSize(&paneWidth, &paneHeight);
		aspectMatch = (projHval / (double)paneWidth) *
		    (double)paneHeight;

                // Call Performer with the new aspect data
		performerChannel->makeOrtho(-projHval, projHval, -aspectMatch,
		    aspectMatch);
	    }
	    else
	    {
		// Both specified, normal operation

                // Call Performer with the new aspect data
		performerChannel->makeOrtho(-projHval, projHval, -projVval,
		    projVval);
	    }
	}
	
        // Take note of the current projection mode so we can detect
	// if it changes again
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

    // Cast the void* userData parameter to the vsPaneSharedData structure
    // so that we can interpret it
    paneData = (vsPaneSharedData *)userData;

    // Select the appropriate buffer to use
    if (paneData->bufferMode == VS_PANE_BUFFER_STEREO_L)
        glDrawBuffer(GL_BACK_LEFT);
    else if (paneData->bufferMode == VS_PANE_BUFFER_STEREO_R)
        glDrawBuffer(GL_BACK_RIGHT);
    else
        glDrawBuffer(GL_BACK);

    // Clear the Performer channel
    chan->clear();

    // Call Performer's draw function.  The scene will be drawn into
    // the buffer selected above.
    pfDraw();
}
