// File vsSystem.c++

#include "vsSystem.h++"

#include <X11/Xlib.h>
#include <Performer/pf.h>
#include <Performer/pfutil.h>
#include "vsLightAttribute.h++"

vsSystem *vsSystem::systemObject = NULL;

// ------------------------------------------------------------------------
// Constructor - Initializes Performer, initializes some basic objects that
// depend on connections to Performer objects, initializes the given
// database loader, and sets up an empty object correspondence map.
// *Note: Only one of these objects may exist in a program at any one time.
// Attemps to create more will return in a non-functional vsSystem object.
// ------------------------------------------------------------------------
vsSystem::vsSystem(vsDatabaseLoader *fileLoader)
{
    pfWSConnection winConnection;
    int numScreens, loop;
    
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
    numScreens = ScreenCount(winConnection);
    if (numScreens > 1)
        pfMultipipe(numScreens);
    screenCount = numScreens;
    
    // Initialize the datbase loader object
    databaseLoader = fileLoader;
    if (databaseLoader)
        databaseLoader->init();

    graphicsState = new vsGraphicsState();

    // * This call can potentially fork new processes, so every object
    // that must be visible to all processes should be created before this
    pfConfig();
    
    // Set up the internal vsScreen and vsPipe objects
    for (loop = 0; loop < screenCount; loop++)
    {
        pipeArray[loop] = new vsPipe(loop);
        screenArray[loop] = new vsScreen(pipeArray[loop]);
    }
    
    nodeMap = new vsObjectMap();
}

// ------------------------------------------------------------------------
// Constructor - Quick-start program
// Initializes Performer, initializes basic VESS objects, creates a
// database loader using the specified filename as an extension and
// initializes it, creates a window with the specified name (the database
// filename if windowTitle is NULL) and size (default size if fullScreen is
// FALSE, full screen with no border if fullScreen is TRUE), creates a pane
// for the window, loads in the specified database from the given database
// file and with the specified list of node names, and attaches it to the
// pane, creates a viewpoint object at a default location, and adds a
// global ambient light source to the scene.
//   The nameList parameter is an array of pointers to node name strings.
// This list is terminated by a NULL pointer in place of a node name. A
// NULL pointer may be passed in for the entire list if important node
// names are not needed.
//   The last three parameters are used for retrieving pointers to some of
// the objects that this constructor generates. NULL may be passed in for
// those values that are undesired.
// *Note: Only one of these objects may exist in a program at any one time.
// Attemps to create more will return in a non-functional vsSystem object.
// ------------------------------------------------------------------------
vsSystem::vsSystem(char *databaseFilename, char **nameList, char *windowTitle,
                   int fullScreen, vsNode **sceneGraph, vsView **viewpoint,
                   vsWindow **window)
{
    pfWSConnection winConnection;
    int numScreens, loop;
    char *nodeName;
    vsWindow *defaultWindow;
    vsPane *defaultPane;
    vsView *defaultView;
    vsVector viewPt, upDir;
    vsNode *scene;
    vsLightAttribute *globalLight;

    if (systemObject)
    {
        printf("vsSystem::vsSystem: Only one vsSystem object may be in "
            "existance at any time\n");
        validObject = 0;
        return;
    }

    databaseLoader = new vsDatabaseLoader(databaseFilename);
    validObject = 1;
    systemObject = this;

    pfInit();
    pfuInit();

    // Configure the system for the available number of graphics pipelines
    winConnection = pfGetCurWSConnection();
    numScreens = ScreenCount(winConnection);
    if (numScreens > 1)
        pfMultipipe(numScreens);
    screenCount = numScreens;

    // Initialize the datbase loader object
    databaseLoader->init();
    if (nameList)
    {
        loop = 0;
        nodeName = nameList[0];
        while (nodeName)
        {
            databaseLoader->addImportantNodeName(nodeName);
            nodeName = nameList[++loop];
        }
    }

    graphicsState = new vsGraphicsState();

    // * This call can potentially fork new processes, so every object
    // that must be visible to all processes should be created before this
    pfConfig();
    
    // Set up the internal vsScreen and vsPipe objects
    for (loop = 0; loop < screenCount; loop++)
    {
        pipeArray[loop] = new vsPipe(loop);
        screenArray[loop] = new vsScreen(pipeArray[loop]);
    }
    
    nodeMap = new vsObjectMap();

    // Set up the default window, pane, and view objects
    defaultWindow = new vsWindow(getScreen(0), fullScreen);
    if (fullScreen)
        defaultWindow->setFullScreen();
    if (windowTitle)
        defaultWindow->setName(windowTitle);
    else
        defaultWindow->setName(databaseFilename);

    defaultPane = new vsPane(defaultWindow);
    defaultPane->autoConfigure(VS_PANE_PLACEMENT_FULL_WINDOW);

    defaultView = new vsView();
    defaultView->setViewpoint(0.0, 10.0, 10.0);
    viewPt.set(0.0, 0.0, 0.0);
    upDir.set(0.0, 0.0, 1.0);
    defaultView->lookAtPoint(viewPt, upDir);
    defaultPane->setView(defaultView);

    // Load the specified database
    scene = databaseLoader->loadDatabase(databaseFilename);
    defaultPane->setScene(scene);
    
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
}


// ------------------------------------------------------------------------
// Destructor - Shuts down Performer, usually resulting in the program
// exiting
// ------------------------------------------------------------------------
vsSystem::~vsSystem()
{
    if (!validObject)
        return;
        
    pfExit();
    systemObject = NULL;
}

// ------------------------------------------------------------------------
// Retrieves one of the system's pipe objects, specified by index. The
// index of the first pipe is 0.
// ------------------------------------------------------------------------
vsPipe *vsSystem::getPipe(int index)
{
    if (!validObject)
        return NULL;
        
    if ((index < 0) || (index >= screenCount))
    {
        printf("vsSystem::getPipe: Bad pipe index\n");
        return NULL;
    }
    
    return pipeArray[index];
}

// ------------------------------------------------------------------------
// Retrieves one of the system's screen objects, specified by index. The
// index of the first screen is 0.
// ------------------------------------------------------------------------
vsScreen *vsSystem::getScreen(int index)
{
    if (!validObject)
        return NULL;
        
    if ((index < 0) || (index >= screenCount))
    {
        printf("vsSystem::getScreen: Bad screen index\n");
        return NULL;
    }
    
    return screenArray[index];
}

// ------------------------------------------------------------------------
// Retrieves the database loader object for this object
// ------------------------------------------------------------------------
vsDatabaseLoader *vsSystem::getLoader()
{
    if (!validObject)
        return NULL;
        
    return databaseLoader;
}

// ------------------------------------------------------------------------
// Passes a call to loadDatabase to the system object's database loader
// object, returning the result.
// ------------------------------------------------------------------------
vsNode *vsSystem::loadDatabase(char *databaseFilename)
{
    if (!validObject)
        return NULL;
        
    if (databaseLoader == NULL)
    {
        printf("vsSystem::loadDatabase: Database loader not specified\n");
        return NULL;
    }
    
    return (databaseLoader->loadDatabase(databaseFilename));
}

// ------------------------------------------------------------------------
// VESS internal function
// Retrieves the node map object for the system object
// ------------------------------------------------------------------------
vsObjectMap *vsSystem::getNodeMap()
{
    if (!validObject)
        return NULL;

    return nodeMap;
}

// ------------------------------------------------------------------------
// VESS internal function
// Retrieves the graphics state object for the system object
// ------------------------------------------------------------------------
vsGraphicsState *vsSystem::getGraphicsState()
{
    if (!validObject)
        return NULL;

    return graphicsState;
}

// ------------------------------------------------------------------------
// The main function for any VESS program. Prompts each active pane object
// to render its attached geometry into its parent window.
// ------------------------------------------------------------------------
void vsSystem::drawFrame()
{
    if (!validObject)
        return;
        
    int screenLoop, windowLoop, paneLoop;
    int windowCount, paneCount;
    vsScreen *targetScreen;
    vsWindow *targetWindow;
    vsPane *targetPane;
    
    // Update the viewpoint of each pane by its vsView object
    for (screenLoop = 0; screenLoop < screenCount; screenLoop++)
    {
        targetScreen = screenArray[screenLoop];
        windowCount = targetScreen->getChildWindowCount();
        for (windowLoop = 0; windowLoop < windowCount; windowLoop++)
        {
            targetWindow = targetScreen->getChildWindow(windowLoop);
            paneCount = targetWindow->getChildPaneCount();
            for (paneLoop = 0; paneLoop < paneCount; paneLoop++)
            {
                targetPane = targetWindow->getChildPane(paneLoop);
                targetPane->updateView();
            }
        }
    }
    
    // Last step is to let Performer do its thing
    pfFrame();
}
