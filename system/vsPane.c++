// File vsPane.c++

#include <Performer/pr/pfMaterial.h>
#include <Performer/pr/pfLight.h>
#include "vsGeometry.h++"
#include "vsComponent.h++"
#include "vsPane.h++"

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
    pfMaterial *frontMtl, *backMtl;
    pfLightModel *lightModel;
    
    sceneRoot = NULL;
    sceneView = NULL;
    
    parentWindow = parent;
    parentScreen = parent->getParentScreen();
    parentPipe = parentScreen->getParentPipe();
    
    performerChannel = new pfChannel(parentPipe->getBaseLibraryObject());

    // Performer automatically assigns a new channel to the first window
    // on the specified pipe; I'd rather do it myself.
    tempPWin = performerChannel->getPWin();
    if (tempPWin)
        tempPWin->removeChan(performerChannel);
    
    parentWindow->addPane(this);
    
    performerScene = new pfScene();
    defaultState = new pfGeoState();
    defaultState->makeBasic();

    defaultState->setMode(PFSTATE_ENLIGHTING, PF_ON);
    lightModel = new pfLightModel();
    lightModel->setLocal(VS_TRUE);
    lightModel->setTwoSide(VS_FALSE);
    lightModel->setAmbient(0.0, 0.0, 0.0);
    defaultState->setAttr(PFSTATE_LIGHTMODEL, lightModel);
    defaultState->setMode(PFSTATE_CULLFACE, PFCF_BACK);
    frontMtl = new pfMaterial();
    frontMtl->setSide(PFMTL_FRONT);
    defaultState->setAttr(PFSTATE_FRONTMTL, frontMtl);
    backMtl = new pfMaterial();
    frontMtl->setSide(PFMTL_BACK);
    defaultState->setAttr(PFSTATE_BACKMTL, backMtl);
    
    performerScene->setGState(defaultState);

    performerChannel->setScene(performerScene);
}

// ------------------------------------------------------------------------
// Destructor - Deletes the associated Performer objects
// ------------------------------------------------------------------------
vsPane::~vsPane()
{
    delete performerChannel;
    
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
void vsPane::autoConfigure(vsPanePlacement value)
{
    // Y coordinate inverted and scaled to 0.0 - 1.0 for
    // Performer's benefit

    switch (value)
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
    
    viewMatrix = sceneView->getBasisRotationMat();
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
