// File vsPane.c++

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
void vsPane::setScene(vsNode *newScene)
{
    pfNode *childNode;

    if (performerScene->getNumChildren() > 0)
    {
        childNode = performerScene->getChild(0);
        if (newScene->getNodeType() == VS_NODE_TYPE_GEOMETRY)
            performerScene->replaceChild(childNode,
                ((vsGeometry *)newScene)->getBaseLibraryObject());
        else
            performerScene->replaceChild(childNode,
                ((vsComponent *)newScene)->getBaseLibraryObject());
    }
    else
    {
        if (newScene->getNodeType() == VS_NODE_TYPE_GEOMETRY)
            performerScene->addChild(
                ((vsGeometry *)newScene)->getBaseLibraryObject());
        else
            performerScene->addChild(
                ((vsComponent *)newScene)->getBaseLibraryObject());
    }
    
    sceneRoot = newScene;
}

// ------------------------------------------------------------------------
// Retrieves the root node of the geometry being displayed in this pane
// ------------------------------------------------------------------------
vsNode *vsPane::getScene()
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

    sceneView->getClipDistances(&near, &far);
    performerChannel->setNearFar(near, far);
}

// ------------------------------------------------------------------------
// static VESS internal function - Performer callback
// When Performer is just starting to render the scene, it first calls
// this function, which is set as the callback function for the pfGeoState
// attached to the pfScene. This function clears the VESS internal graphics
// state.
// ------------------------------------------------------------------------
int vsPane::gstateCallback(pfGeoState *gstate, void *userData)
{
    (vsSystem::systemObject)->getGraphicsState()->clearState();
    
    return 0;
}

// ------------------------------------------------------------------------
// VESS internal function - debugging only
// Prompts Performer to print out debugging info consisting of the scene
// graph attached to this pane.
// ------------------------------------------------------------------------
void vsPane::_debugWriteScene()
{
    FILE *outFile = fopen("scene.out", "w");
    ((pfMemory *)performerScene)->print(PFTRAV_SELF | PFTRAV_DESCEND,
        PFPRINT_VB_ON, NULL, outFile);
    fclose(outFile);
}
