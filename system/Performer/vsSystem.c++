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
//    VESS Module:  vsSystem.c++
//
//    Description:  The main object in any VESS application. Exactly one
//                  of these objects should be in existance during the
//                  lifetime of the program.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsSystem.h++"

#include <sys/time.h>
#include <Performer/pf.h>
#include <Performer/pfutil.h>
#include <Performer/pfdu.h>

#include "vsLightAttribute.h++"
#include "vsComponent.h++"
#include "vsGeometry.h++"
#include "vsOptimizer.h++"
#include "vsGraphicsState.h++"
#include "vsDatabaseLoader.h++"

vsSystem *vsSystem::systemObject = NULL;

// ------------------------------------------------------------------------
// Constructor - Pre-initializes the system object and initializes
// Performer.
// *Note: Only one of these objects may exist in a program at any one time.
// Attemps to create more will return in a non-functional vsSystem object.
// ------------------------------------------------------------------------
vsSystem::vsSystem()
{
    pfWSConnection winConnection;
    int screenCount;

    isInitted = 0;

    if (systemObject)
    {
        printf("vsSystem::vsSystem: Only one vsSystem object may be in "
            "existance at any time\n");
        validObject = 0;
        return;
    }
    
    validObject = 1;
    systemObject = this;

    pfInit();
    pfuInit();

    // Configure the system for the available number of graphics pipelines
    winConnection = pfGetCurWSConnection();
    screenCount = ScreenCount(winConnection);
    if (screenCount > 1)
        pfMultipipe(screenCount);
}

// ------------------------------------------------------------------------
// Destructor - Shuts down Performer, usually resulting in the program
// exiting
// ------------------------------------------------------------------------
vsSystem::~vsSystem()
{
    if (!validObject)
        return;
        
    delete frameTimer;

    systemObject = NULL;
    pfExit();
}

// ------------------------------------------------------------------------
// Sets the multiprocessing mode for the application. This should be set
// before init (or simpleInit) is called if it is to be set at all.
// ------------------------------------------------------------------------
void vsSystem::setMultiprocessMode(int mpMode)
{
    switch (mpMode)
    {
	case VS_MPROC_DEFAULT:
	    // Let Performer decide
	    pfMultiprocess(PFMP_DEFAULT);
	    break;
	case VS_MPROC_SINGLE:
	    // One process only
	    pfMultiprocess(PFMP_APPCULLDRAW);
	    break;
	case VS_MPROC_MULTI:
	    // Split into three processes
	    pfMultiprocess(PFMP_APP_CULL_DRAW);
	    break;
    }
}

// ------------------------------------------------------------------------
// Notifies the system object that a particular filename extension will be
// in use for this program run and that the appropriate databse loader
// should be initialized. All calls to this function should be made before
// init (or simpleInit) is called.
// ------------------------------------------------------------------------
void vsSystem::addExtension(char *fileExtension)
{
    if (!validObject)
	return;

    if (isInitted)
    {
        printf("vsSystem::addExtension: Can't add extensions after "
	    "initialization of vsSystem object\n");
        return;
    }

    if (!pfdInitConverter(fileExtension))
    {
        printf("vsSystem::addExtension: Unable to initialize '%s' loader\n",
	    fileExtension);
        return;
    }
}

// ------------------------------------------------------------------------
// Initializes the system object. This involves constructing internally-
// used objects, forking multiple processes, and priming timestamp data.
// ------------------------------------------------------------------------
void vsSystem::init()
{
    if (!validObject)
	return;

    if (isInitted)
    {
	printf("vsSystem::init: vsSystem object is already initialized\n");
	return;
    }

    // * This call can potentially fork new processes, so every object
    // that must be visible to all processes should be created before this
    pfConfig();
    
    // Set up the vsScreen and vsPipe objects
    vsPipe::init();
    vsScreen::init();
    
    // Initialize the current time
    frameTimer = new vsTimer();
    
    isInitted = 1;
}

// ------------------------------------------------------------------------
// Basic VR application quick start initialization. Initializes the system
// object by constructing internally-used objects, forking multiple
// processes, and priming timestamp data. Also creates a default-sized
// window (full screen or not as desired), loads the given database, and
// creates a view object that looks onto the new scene. Returns pointers
// to the newly-created window, scene, and viewpoint object; NULL pointers
// may be specified for undesired return values.
// ------------------------------------------------------------------------
void vsSystem::simpleInit(char *databaseFilename, char *windowName,
			  int fullScreen, vsNode **sceneGraph,
			  vsView **viewpoint, vsWindow **window)
{
    vsWindow *defaultWindow;
    vsPane *defaultPane;
    vsView *defaultView;
    vsVector upDir;
    vsVector dbCenter;
    double dbRadius;
    vsScene *sceneRoot;
    vsComponent *scene;
    vsLightAttribute *globalLight;
    vsOptimizer *optimizer;
    vsDatabaseLoader *dbLoader;

    if (!validObject)
	return;

    if (isInitted)
    {
	printf("vsSystem::init: vsSystem object is already initialized\n");
	return;
    }

    addExtension(databaseFilename);

    // * This call can potentially fork new processes, so every object
    // that must be visible to all processes should be created before this
    pfConfig();
    
    // Initialize the datbase loader object
    dbLoader = new vsDatabaseLoader();
    dbLoader->setLoaderMode(VS_DATABASE_MODE_NAME_XFORM, 1);

    // Set up the vsScreen and vsPipe objects
    vsPipe::init();
    vsScreen::init();
    
    isInitted = 1;

    // Quick start: Set up the default window, pane, and view objects
    defaultWindow = new vsWindow(vsScreen::getScreen(0), fullScreen);
    if (fullScreen)
        defaultWindow->setFullScreen();
    if (windowName)
        defaultWindow->setName(windowName);
    else
        defaultWindow->setName(databaseFilename);

    defaultPane = new vsPane(defaultWindow);
    defaultPane->autoConfigure(VS_PANE_PLACEMENT_FULL_WINDOW);

    // Load the specified database
    sceneRoot = new vsScene();
    scene = dbLoader->loadDatabase(databaseFilename);
    optimizer = new vsOptimizer();
    optimizer->optimize(scene);
    sceneRoot->addChild(scene);
    defaultPane->setScene(sceneRoot);

    // Set up the viewpoint
    defaultView = new vsView();
    scene->getBoundSphere(&dbCenter, &dbRadius);
    defaultView->setViewpoint(dbCenter[0], dbCenter[1] + dbRadius,
	dbCenter[2] + dbRadius);
    upDir.set(0.0, 0.0, 1.0);
    defaultView->lookAtPoint(dbCenter, upDir);
    defaultPane->setView(defaultView);
    
    // Add a global ambient (white) light source
    globalLight = new vsLightAttribute();
    globalLight->setPosition(0.0, 0.0, 1.0, 0.0);
    globalLight->setAmbientColor(1.0, 1.0, 1.0);
    globalLight->setDiffuseColor(1.0, 1.0, 1.0);
    globalLight->setSpecularColor(1.0, 1.0, 1.0);
    globalLight->setScope(VS_LIGHT_MODE_GLOBAL);
    globalLight->on();
    scene->addAttribute(globalLight);

    // Initialize the current time
    frameTimer = new vsTimer();

    // Return the requested values and finish
    if (sceneGraph)
        *sceneGraph = scene;
    if (viewpoint)
        *viewpoint = defaultView;
    if (window)
        *window = defaultWindow;

    delete optimizer;
    delete dbLoader;
}

// ------------------------------------------------------------------------
// Private function
// Traverses the VESS scene graph in order to give processing time to each
// attribute in the scene
// ------------------------------------------------------------------------
void vsSystem::preFrameTraverse(vsNode *node)
{
    vsComponent *component;
    vsNode *childNode;
    int loop;
    
    node->clean();

    node->saveCurrentAttributes();
    node->applyAttributes();
    
    for (loop = 0; loop < node->getChildCount(); loop++)
    {
        childNode = node->getChild(loop);
        if (childNode->isDirty())
            preFrameTraverse(childNode);
    }
    
    node->restoreSavedAttributes();
}

// ------------------------------------------------------------------------
// The main function for any VESS program. Prompts each active pane object
// to render its attached geometry into its parent window.
// ------------------------------------------------------------------------
void vsSystem::drawFrame()
{
    if (!validObject)
        return;
        
    if (!isInitted)
    {
	printf("vsSystem::drawFrame: System object is not initialized\n");
	return;
    }

    int screenLoop, windowLoop, paneLoop;
    int windowCount, paneCount;
    vsScreen *targetScreen;
    vsWindow *targetWindow;
    vsPane *targetPane;
    vsNode *scene;
    int screenCount = vsScreen::getScreenCount();
    
    // Update the viewpoint of each pane by its vsView object
    for (screenLoop = 0; screenLoop < screenCount; screenLoop++)
    {
        targetScreen = vsScreen::getScreen(screenLoop);
        windowCount = targetScreen->getChildWindowCount();
        for (windowLoop = 0; windowLoop < windowCount; windowLoop++)
        {
            targetWindow = targetScreen->getChildWindow(windowLoop);
            paneCount = targetWindow->getChildPaneCount();
            for (paneLoop = 0; paneLoop < paneCount; paneLoop++)
            {
                targetPane = targetWindow->getChildPane(paneLoop);
                targetPane->updateView();

                scene = targetPane->getScene();
		if (scene)
		{
		    (vsGraphicsState::getInstance())->clearState();
		    preFrameTraverse(scene);
		}
            }
        }
    }
    
    // Wait until the next frame boundary
    pfSync();
    
    // Check how much time has elapsed since the last time we were here
    lastFrameDuration = frameTimer->getElapsed();
    frameTimer->mark();
    
    // Start the processing for this frame
    pfFrame();
}

// ------------------------------------------------------------------------
// Returns the amount of elapsed time between the last two calls to
// drawFrame.
// ------------------------------------------------------------------------
double vsSystem::getFrameTime()
{
    if (!validObject)
        return 0.0;
        
    if (!isInitted)
    {
	printf("vsSystem::drawFrame: System object is not initialized\n");
	return 0.0;
    }

    return lastFrameDuration;
}
