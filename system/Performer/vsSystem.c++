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
#include "vsWindowSystem.h++"

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

    // Start off uninitialized
    isInitted = 0;

    // Singleton check
    if (systemObject)
    {
        printf("vsSystem::vsSystem: Only one vsSystem object may be in "
            "existance at any time\n");
        validObject = false;
        return;
    }
    
    // Save this system object in the class pointer variable
    validObject = true;
    systemObject = this;

    // Initialize Performer
    pfInit();
    pfuInit();

    // Configure the system for the available number of graphics pipelines
    winConnection = pfGetCurWSConnection();
    screenCount = ScreenCount(winConnection);

    // Activate multipipe mode if appropriate
    if (screenCount > 1)
        pfMultipipe(screenCount);
}

// ------------------------------------------------------------------------
// Destructor - Shuts down Performer, usually resulting in the program
// exiting
// ------------------------------------------------------------------------
vsSystem::~vsSystem()
{
    // Do nothing if this isn't a real system object
    if (!validObject)
        return;

    // Delete statically-created objects in the various VESS classes
    vsGeometry::clearBinSortModes();
    vsGraphicsState::deleteInstance();
    vsViewpointAttribute::deleteMap();
    vsNode::deleteMap();
    vsTimer::deleteSystemTimer();
    vsWindowSystem::deleteMap();
    vsScreen::done();
    vsPipe::done();

#ifdef VESS_DEBUG
    FILE *outfile = fopen("vess_objects.log", "w");
    if (outfile)
    {
        vsObject::printCurrentObjects(outfile);
        fclose(outfile);
    }

    vsObject::deleteObjectList();
#endif
        
    // Clear the class pointer and shut down Performer
    systemObject = NULL;
    pfExit();
}

// ------------------------------------------------------------------------
// Sets the multiprocessing mode for the application. This should be set
// before init (or simpleInit) is called if it is to be set at all.
// ------------------------------------------------------------------------
void vsSystem::setMultiprocessMode(int mpMode)
{
    // Interpret the multiprocess constant
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
    // Do nothing if this isn't a real system object
    if (!validObject)
	return;

    // Do nothing if this object has already been initialized
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
    
    // Mark the object as initialized
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
			  bool fullScreen, vsNode **sceneGraph,
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

    // Do nothing if this isn't a real system object
    if (!validObject)
	return;

    // Do nothing if this object has already been initialized
    if (isInitted)
    {
	printf("vsSystem::init: vsSystem object is already initialized\n");
	return;
    }

    // Configure the system for the type of database being loaded
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
    
    // Mark the object as initialized
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

    // Return the requested values and finish
    if (sceneGraph)
        *sceneGraph = scene;
    if (viewpoint)
        *viewpoint = defaultView;
    if (window)
        *window = defaultWindow;

    // Clean up the loader and optimizer
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
    vsNode *childNode;
    int loop;
    
    // Mark this node as clean
    node->clean();

    // Activate all of the attributes on the node
    node->saveCurrentAttributes();
    node->applyAttributes();
    
    // Recurse on the node's children (if any)
    for (loop = 0; loop < node->getChildCount(); loop++)
    {
        childNode = node->getChild(loop);
        if (childNode->isDirty())
            preFrameTraverse(childNode);
    }
    
    // On the way back out, deactivate the attributes on the node
    node->restoreSavedAttributes();
}

// ------------------------------------------------------------------------
// The main function for any VESS program. Prompts each active pane object
// to render its attached geometry into its parent window.
// ------------------------------------------------------------------------
void vsSystem::drawFrame()
{
    // Do nothing if this isn't a real system object
    if (!validObject)
        return;
        
    // Do nothing if the object hasn't been initialized
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
    vsTreeMap *binModeList;
    vsGrowableArray binList(1, 1);
    vsGrowableArray modeList(1, 1);
    int binLoop;
    int binNum, binMode;

    // Get the bin mode list from the vsGeometry class
    binModeList = vsGeometry::getBinModeList();

    // If the bin mode list exists, and the geometry bin modes have changed, 
    // update all panes to use the new bin modes
    if ((vsGeometry::binModesChanged) && (binModeList != NULL))
    {
        // Get two parallel arrays from the bin mode list representing the 
        // bin numbers and their modes, respectively.
        binModeList->getSortedList(&binList, &modeList);

        // Loop over all screens
        for (screenLoop = 0; screenLoop < screenCount; screenLoop++)
        {
            targetScreen = vsScreen::getScreen(screenLoop);
            windowCount = targetScreen->getChildWindowCount();

            // Loop over all windows
            for (windowLoop = 0; windowLoop < windowCount; windowLoop++)
            {
                // Find the number of panes on the window
                targetWindow = targetScreen->getChildWindow(windowLoop);
                paneCount = targetWindow->getChildPaneCount();

                // Loop over all panes
                for (paneLoop = 0; paneLoop < paneCount; paneLoop++)
                {
                    // Get the next pane
                    targetPane = targetWindow->getChildPane(paneLoop);

                    // For each bin in the bin mode list, set the sort
                    // order on the pane's Performer channel
                    for (binLoop = 0; 
                        binLoop < binModeList->getEntryCount(); 
                        binLoop++)
                    {
                        // Get the bin number and mode from the lists
                        binNum = (int)binList[binLoop];
                        binMode = (int)modeList[binLoop];

                        // Set the bin drawing order to the same value as
                        // the bin number (e.g. bin 1 is drawn first,
                        // bin 2 is second, etc)
                        targetPane->getBaseLibraryObject()->
                            setBinOrder(binNum, binNum);
                            
                        // See if this bin is state- or depth- sorted
                        if (binMode == VS_GEOMETRY_SORT_DEPTH)
                        {
                            // Set this bin to depth (back-to-front) sorted
                            targetPane->getBaseLibraryObject()->
                                setBinSort(binNum, PFSORT_BACK_TO_FRONT, NULL);
                        }
                        else
                        {
                            // Set this bin to state-sorted
                            targetPane->getBaseLibraryObject()->
                                setBinSort(binNum, PFSORT_BY_STATE, NULL);
                        }
                    }
                }
            }
        }

        // Clear the flag that states the bin modes have changed
        vsGeometry::binModesChanged = false;
    }

    // Update the viewpoint of each pane by its vsView object
    for (screenLoop = 0; screenLoop < screenCount; screenLoop++)
    {
        targetScreen = vsScreen::getScreen(screenLoop);
        windowCount = targetScreen->getChildWindowCount();

        // Loop over all windows
        for (windowLoop = 0; windowLoop < windowCount; windowLoop++)
        {
            // Find the number of panes on the window
            targetWindow = targetScreen->getChildWindow(windowLoop);
            paneCount = targetWindow->getChildPaneCount();

            // Loop over all panes
            for (paneLoop = 0; paneLoop < paneCount; paneLoop++)
            {
                // Update the viewpoint of the pane
                targetPane = targetWindow->getChildPane(paneLoop);
                targetPane->updateView();

                // Run a VESS traversal over the pane's scene
                scene = targetPane->getScene();
		if (scene)
		{
		    (vsGraphicsState::getInstance())->clearState();
		    preFrameTraverse(scene);
		}

                // See if stats are enabled on the pane, and call
                // pfChannel::drawStats to draw them if so
                if (targetPane->areStatsEnabled())
                    targetPane->getBaseLibraryObject()->drawStats();
            }
        }
    }
    
    // Wait until the next frame boundary
    pfSync();
    
    // Mark the system timer for this frame
    vsTimer::getSystemTimer()->mark();
    
    // Start the processing for this frame
    pfFrame();
}
