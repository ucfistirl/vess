// File vsSystem.c++

#include <X11/Xlib.h>
#include <Performer/pf.h>
//#include <Performer/pr.h>
#include "vsSystem.h++"

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

    pfConfig();
    
    // Set up the internal vsScreen and vsPipe objects
    for (loop = 0; loop < screenCount; loop++)
    {
        pipeArray[loop] = new vsPipe(loop);
        screenArray[loop] = new vsScreen(pipeArray[loop]);
    }
    
    nodeMap = new vsObjectMap();
    
    coordSystemXform.setIdentity();
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
// Retrieves the node map object for the system object.
// ------------------------------------------------------------------------
vsObjectMap *vsSystem::getNodeMap()
{
    return nodeMap;
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

// ------------------------------------------------------------------------
// Sets the global coordinate transform for this program. The global
// transform modifies the VESS default coordinate directions.
// ------------------------------------------------------------------------
void vsSystem::setCoordinateXform(vsMatrix newXform)
{
    coordSystemXform = newXform;
}

// ------------------------------------------------------------------------
// Retrieves the global coordinate transform for this program. This matrix
// is set to the identity matrix by default.
// ------------------------------------------------------------------------
vsMatrix vsSystem::getCoordinateXform()
{
    return coordSystemXform;
}
