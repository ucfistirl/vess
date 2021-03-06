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

#include <stdio.h>
#include <osg/Drawable>
#include <osg/Geode>
#include <osgDB/Registry>
#include <osgDB/DatabasePager>

#include "atList.h++"

#include "vsLightAttribute.h++"
#include "vsLocalLightCallback.h++"
#include "vsScene.h++"
#include "vsComponent.h++"
#include "vsGeometry.h++"
#include "vsOptimizer.h++"
#include "vsGraphicsState.h++"
#include "vsDatabaseLoader.h++"
#include "vsViewpointAttribute.h++"
#include "vsWindowSystem.h++"
#include "vsTextBuilder.h++"
#include "vsScentManager.h++"

#ifdef VS_SOUND_ENABLED
   #include "vsSoundManager.h++"
#endif

vsSystem *vsSystem::systemObject = NULL;

// ------------------------------------------------------------------------
// Constructor - Pre-initializes the system object and initializes
// Open Scene Graph.
// *Note: Only one of these objects may exist in a program at any one time.
// Attemps to create more will return in a non-functional vsSystem object.
// ------------------------------------------------------------------------
vsSystem::vsSystem()
{
    // Start out uninitialized
    isInitted = 0;

    // Singleton check
    if (systemObject)
    {
        printf("vsSystem::vsSystem: Only one vsSystem object may be in "
            "existance at any time\n");
        validObject = false;
        return;
    }

    // Create an OSG FrameStamp (This is required by OSG.  It will be shared 
    // by all panes and hence, all osgUtil::SceneView objects)
    osgFrameStamp = new osg::FrameStamp();
    osgFrameStamp->ref();
    
    // Mark this instance as valid and set the static class variable to 
    // point to this instance
    validObject = true;
    systemObject = this;
    
    // Initialize the remote interface
    remoteInterface = new vsRemoteInterface();

    // Create a sequencer to act as a "root sequencer"
    rootSequencer = new vsSequencer();
    rootSequencer->ref();
}

// ------------------------------------------------------------------------
// Destructor - Shuts down Open Scene Graph, usually resulting in the 
// program exiting
// ------------------------------------------------------------------------
vsSystem::~vsSystem()
{
    // Do nothing if this isn't a real system object
    if (!validObject)
        return;
    
    // Unreference the osg::FrameStamp
    osgFrameStamp->unref();
    
    // Delete statically-created objects in the various VESS classes
    vsGraphicsState::deleteInstance();
    vsViewpointAttribute::deleteMap();
    vsNode::deleteMap();
    vsTimer::deleteSystemTimer();
    vsWindowSystem::deleteMap();
    vsScreen::done();
    vsPipe::done();

    // Delete the remote interface
    delete remoteInterface;

    // Delete the "root sequencer"
    rootSequencer->unref();
    delete rootSequencer;

#ifdef VS_SOUND_ENABLED

    // Delete the sound manager
    vsSoundManager::deleteInstance();

#endif

    // Delete the scent manager
    vsScentManager::deleteInstance();

#ifdef VESS_DEBUG
    FILE *outfile = fopen("vess_objects.log", "w");
    if (outfile)
    {
        vsObject::printCurrentObjects(outfile);
        fclose(outfile);
    }

    vsObject::deleteObjectList();
#endif

    // Clear the static class member to NULL
    systemObject = NULL;
}

// ------------------------------------------------------------------------
// Sets the multiprocessing mode for the application. This should be set
// before init (or simpleInit) is called if it is to be set at all.
// ------------------------------------------------------------------------
void vsSystem::setMultiprocessMode(int mpMode)
{
    // Open Scene Graph has no native support for multiprocessing
    switch (mpMode)
    {
        case VS_MPROC_DEFAULT:
        case VS_MPROC_SINGLE:
        case VS_MPROC_MULTI:
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
    // This step is not necessary in Open Scene Graph, since there
    // is no support for multiprocessing
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

    // Print an error message and bail out if the object is already
    // initialized
    if (isInitted)
    {
        printf("vsSystem::init: vsSystem object is already initialized\n");
        return;
    }

    // Set up the vsScreen and vsPipe objects
    vsPipe::init();
    vsScreen::init();
    
    // Initialize the osg FrameStamp object
    frameNumber = 0;
    simTime = 0.0;
    osgFrameStamp->setFrameNumber(frameNumber);
    osgFrameStamp->setReferenceTime(simTime);
    osgFrameStamp->setSimulationTime(simTime);
    
    // Mark this vsSystem instance as initialized
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
    atVector upDir;
    atVector dbCenter;
    double dbRadius;
    vsScene *scene;
    vsComponent *database;
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
    
    // Initialize the datbase loader object
    dbLoader = new vsDatabaseLoader();
    dbLoader->setLoaderMode(VS_DATABASE_MODE_NAME_XFORM, 1);

    // Set up the vsScreen and vsPipe objects
    vsPipe::init();
    vsScreen::init();
    
    // Mark the object as initialized
    isInitted = 1;

    // Quick start: Set up the default window, pane, and view objects
    // Note: When in full screen mode, hide the border as well
    defaultWindow = new vsWindow(vsScreen::getScreen(0), fullScreen, false);
    if (fullScreen)
        defaultWindow->setFullScreen();
    if (windowName)
        defaultWindow->setName(windowName);
    else
        defaultWindow->setName(databaseFilename);
    defaultPane = new vsPane(defaultWindow);
    defaultPane->autoConfigure(VS_PANE_PLACEMENT_FULL_WINDOW);

    // Load the specified database
    scene = new vsScene();
    database = dbLoader->loadDatabase(databaseFilename);
    optimizer = new vsOptimizer();
    optimizer->optimize(database);
    scene->addChild(database);
    defaultPane->setScene(scene);

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

    // Initialize the osg FrameStamp object
    frameNumber = 0;
    simTime = 0.0;
    osgFrameStamp->setFrameNumber(frameNumber);
    osgFrameStamp->setReferenceTime(simTime);
    osgFrameStamp->setSimulationTime(simTime);

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
    vsScene *scene;
    vsComponent *component;
    vsNode *childNode;
    vsGraphicsState *graphicsState;
    vsArray *localLightArray;
    vsLocalLightCallback *localLightCallback;
    osg::Geode *geode;
    osg::Drawable *drawable;
    int localLightArrayLength;
    int callbackLightCount;
    int loop;
    
    // Mark this node as clean
    node->clean();

    // Activate all of the attributes on the node
    node->saveCurrentAttributes();
    node->applyAttributes();

    // If the node is a scene, get its only child and try to traverse it.
    if (node->getNodeType() == VS_NODE_TYPE_SCENE)
    {
        scene = (vsScene *)node;
        childNode = scene->getChild(0);
        if ((childNode != NULL) && (childNode->isDirty()))
            preFrameTraverse(childNode);
    }
    // If the node is a component, get its children and try to traverse them.
    else if (node->getNodeType() == VS_NODE_TYPE_COMPONENT)
    {
        component = (vsComponent *)node;
        for (loop = 0; loop < component->getChildCount(); loop++)
        {
            childNode = component->getChild(loop);
            if (childNode->isDirty())
                preFrameTraverse(childNode);
        }
    }
    // If the node is a geometry node, handle any local lights that affect it.
    else if ((node->getNodeType() == VS_NODE_TYPE_GEOMETRY) ||
             (node->getNodeType() == VS_NODE_TYPE_DYNAMIC_GEOMETRY))
    {
        // Get the graphics state, we will use this further down.
        graphicsState = vsGraphicsState::getInstance();

        // Get the number of lights we must apply on this vsGeometry.
        localLightArrayLength = graphicsState->getLocalLightsCount();

        // Get the array of local lights.
        localLightArray = graphicsState->getLocalLightsArray();

        // Get the osg::Geode for this vsGeometry.
        geode = ((vsGeometry *)node)->getBaseLibraryObject();

        // Go through all the drawables on this Geode.
        for (loop = 0; loop < (int)geode->getNumDrawables(); loop++)
        {
            // Get the specific drawable.
            drawable = geode->getDrawable(loop);

            // Get the callback for this node.
            localLightCallback =
                (vsLocalLightCallback *)drawable->getDrawCallback();

            // If the drawable has a callback already, modify it.
            if (localLightCallback != NULL)
            {
                // If there are lights to apply, set them on the callback.
                if (localLightArrayLength > 0)
                {
                    callbackLightCount =
                        localLightCallback->setLocalLights(localLightArray);
                }

                // If the callback has zero lights, remove the callback.
                if (callbackLightCount == 0)
                {
                    // Set this drawable's callback to NULL now.
                    drawable->setDrawCallback(NULL);

                    // If this is a static geometry node, set the drawable
                    // to use display lists now.
                    if (node->getNodeType() == VS_NODE_TYPE_GEOMETRY)
                        drawable->setUseDisplayList(true);

                    // Unreference the callback.
                    localLightCallback->unref();
                    localLightCallback = NULL;
                }
            }
            // Else if there are lights and no callback, create a new one.
            else if (localLightArrayLength > 0)
            {
                // Create the callback with the lights to add.
                localLightCallback = new vsLocalLightCallback(localLightArray);
                localLightCallback->ref();

                // Set the callback on the drawable.
                drawable->setDrawCallback(localLightCallback);

                // This is VERY important, it causes OSG to call this
                // drawable's draw callback every time it will draw it.
                drawable->setUseDisplayList(false);
            }
        }
    }
 
    node->restoreSavedAttributes();
}

// ------------------------------------------------------------------------
// Returns the "root" sequencer that the user can attach his/her
// vsUpdatables to and have drawFrame call them if desired (this is not
// a required element -- the user can call the update() methods
// directly if desired).
// ------------------------------------------------------------------------
vsSequencer *vsSystem::getSequencer()
{
    // Return the pointer to the root sequencer
    return rootSequencer;
}

// ------------------------------------------------------------------------
// This function returns the number of outstanding database pages that have
// yet to be loaded into memory. This value is calculated as the number of 
// page requests that have been made plus the number of pages that remain 
// to be compiled. If a negative value is returned, then there was a 
// problem obtaining an instance of the OSG database pager.
// ------------------------------------------------------------------------
int vsSystem::getNumDBPagesToLoad()
{
    int screenLoop, windowLoop, paneLoop;
    int screenCount;
    int windowCount, paneCount;
    vsScreen *targetScreen;
    vsWindow *targetWindow;
    vsPane *targetPane;
    vsScene *scene;
    atList *sceneList;
    vsScene *listScene;
    osgDB::DatabasePager *osgDBPager;
    int totalCount;

    // Create a list of unique scenes in our current application
    sceneList = new atList();
    screenCount = vsScreen::getScreenCount();
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
                scene = targetPane->getScene();

                // See if this scene is already in our list
                listScene = (vsScene *)sceneList->getFirstEntry();
                while ((listScene != NULL) && (listScene != scene))
                    listScene = (vsScene *)sceneList->getNextEntry();

                // If we didn't find it, add it
                if (listScene == NULL)
                    sceneList->addEntry(scene);
            }
        }
    }

    // Initialize the counter
    totalCount = 0;

    // Now, iterate over the scene list
    listScene = (vsScene *)sceneList->getFirstEntry();
    while ((listScene != NULL) && (listScene != scene))
    {
        // Get this scene's database pager (if any)
        osgDBPager = listScene->getDatabasePager();
        if (osgDBPager)
        {
           // Increment the total count by this pager's task count
           totalCount +=
               osgDBPager->getFileRequestListSize() +
               osgDBPager->getDataToCompileListSize();
        }

        // Next scene
        listScene = (vsScene *)sceneList->getNextEntry();
    }

    // Get rid of the scene list (but not the scenes in it)
    sceneList->removeAllEntries();
    delete sceneList;

    // Return the count
    return totalCount;
}

// ------------------------------------------------------------------------
// The main function for any VESS program. Prompts each active pane object
// to render its attached geometry into its parent window.
// ------------------------------------------------------------------------
void vsSystem::drawFrame()
{
#ifdef WIN32
    MSG message;
#endif
    int screenLoop, windowLoop, paneLoop;
    int windowCount, paneCount;
    vsScreen *targetScreen;
    vsWindow *targetWindow;
    vsPane *targetPane;
    vsScene *scene;
    atList *sceneList;
    vsScene *sceneInList;
    int screenCount = vsScreen::getScreenCount();
    double lastFrameInterval, availableTime;
    osgDB::DatabasePager *osgDBPager;

    // Do nothing if this isn't a real system object
    if (!validObject)
        return;
        
    // Do nothing if the object hasn't been initialized
    if (!isInitted)
    {
        printf("vsSystem::drawFrame: System object is not initialized\n");
        return;
    }
    
    // Tell the "root" sequencer to update itself and everything attached
    rootSequencer->update();

    // Have the remote interface process any requests, etc.
    remoteInterface->update();

    // If any of the vsGeometry's render bin modes changed last frame,
    // then we need to mark every geometry object in existance as dirty so
    // that the bin mode change gets applied to all geometry objects.
    if (vsRenderBin::binModesChanged)
    {
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
                    scene = targetPane->getScene();

                    if (scene)
                        scene->dirty();
                }
            }
        }
        
        // Now that we've done our job, don't do it again next frame
        vsRenderBin::binModesChanged = false;
    }
    
    // Mark the system timer for this frame
    vsTimer::getSystemTimer()->mark();
    lastFrameInterval = vsTimer::getSystemTimer()->getInterval();

    // Update the OSG FrameStamp object
    frameNumber++;
    simTime += lastFrameInterval;
    osgFrameStamp->setFrameNumber(frameNumber);
    osgFrameStamp->setReferenceTime(simTime);
    osgFrameStamp->setSimulationTime(simTime);

    // If we're doing any database paging, merge any new database components
    // that the pager has recently loaded with the appropriate scenes
    availableTime = lastFrameInterval;

    // Create a list to store unique scenes that we're drawing this frame
    sceneList = new atList();

    // Update the viewpoint of each pane by its vsView object and
    // do a pre-frame traversal to set up the graphics state for each
    // pane's scene
    for (screenLoop = 0; screenLoop < screenCount; screenLoop++)
    {
        targetScreen = vsScreen::getScreen(screenLoop);
        windowCount = targetScreen->getChildWindowCount();

        // Loop over all windows
        for (windowLoop = 0; windowLoop < windowCount; windowLoop++)
        {
            // Find the number of panes on the window
            targetWindow = targetScreen->getChildWindow(windowLoop);

            // Make the window's OpenGL context current
            targetWindow->makeCurrent();

            // Update the window to process events
            targetWindow->update();

            // Get the number of panes on the window
            paneCount = targetWindow->getChildPaneCount();

            // Get each pane ready to draw the scene
            for (paneLoop = 0; paneLoop < paneCount; paneLoop++)
            {
                // Get the next pane from the window
                targetPane = targetWindow->getChildPane(paneLoop);

                // Update the view object on this pane with the new
                // vsView data
                targetPane->updateView();

                // See if the background color needs to be updated.
                targetPane->updateClearState();

                // Get the scene from this pane
                scene = targetPane->getScene();

                // Make sure the scene is valid
                if (scene)
                {
                    // Clear the VESS graphics state
                    (vsGraphicsState::getInstance())->clearState();

                    // Give the GraphicsState the scene it will deal with now.
                    (vsGraphicsState::getInstance())->setCurrentScene(scene);

                    // Perform the VESS pre-frame traversal on the scene
                    // if it is dirty.
                    if (scene->isDirty())
                        preFrameTraverse(scene);

                    // See if this scene is already in our list
                    sceneInList = (vsScene *)sceneList->getFirstEntry();
                    while ((sceneInList != NULL) && (sceneInList != scene))
                        sceneInList = (vsScene *)sceneList->getNextEntry();

                    // If we didn't find it, add it
                    if (sceneInList == NULL)
                        sceneList->addEntry(scene);
                }
            }
        }
    }

    // Traverse the list of scenes
    sceneInList = (vsScene *)sceneList->getFirstEntry();
    while (sceneInList != NULL)
    {
        // Get the scene's database pager (if any)
        osgDBPager = sceneInList->getDatabasePager();
        if (osgDBPager)
        {
            // Update the scene graph and signal the pager that we're starting
            // to draw
            osgDBPager->updateSceneGraph(*osgFrameStamp);
            osgDBPager->signalBeginFrame(osgFrameStamp);
        }

        // Next scene
        sceneInList = (vsScene *)sceneList->getNextEntry();
    }
  
    // Perform update, cull, and draw traversals for each pane
    for (screenLoop = 0; screenLoop < screenCount; screenLoop++)
    {
        targetScreen = vsScreen::getScreen(screenLoop);
        windowCount = targetScreen->getChildWindowCount();
        for (windowLoop = 0; windowLoop < windowCount; windowLoop++)
        {
            targetWindow = targetScreen->getChildWindow(windowLoop);

            // Make the window's OpenGL context current
            targetWindow->makeCurrent();

            // Traverse and draw the scene in each pane
            paneCount = targetWindow->getChildPaneCount();
            for (paneLoop = 0; paneLoop < paneCount; paneLoop++)
            {
                // Get the current pane
                targetPane = targetWindow->getChildPane(paneLoop);

                // Set the frame stamp on the pane's SceneView object
                targetPane->getBaseLibraryObject()->
                    setFrameStamp(osgFrameStamp);

                // Do update and cull traversals
                targetPane->getBaseLibraryObject()->update();
                targetPane->getBaseLibraryObject()->cull();

                // Only draw the pane if the visibility flag is true
                if (targetPane->isVisible())
                {
                    targetPane->getBaseLibraryObject()->draw();

                    // See if we're doing database paging
                    scene = targetPane->getScene();
                    if (scene)
                        osgDBPager = scene->getDatabasePager();
                    else
                        osgDBPager = NULL;
                    if (osgDBPager)
                    {
                        // Tell the target pane to flush any GL objects that
                        // have been deleted
                        targetPane->getBaseLibraryObject()->
                            flushDeletedGLObjects(availableTime);
                    }
                }
            }

            // Swap buffers on this window
            targetWindow->swapBuffers();
        }
    }

    // Traverse the list of scenes again
    sceneInList = (vsScene *)sceneList->getFirstEntry();
    while (sceneInList != NULL)
    {
        // Get the scene's database pager (if any)
        osgDBPager = sceneInList->getDatabasePager();
        if (osgDBPager)
        {
            // Tell the database pager that we're done drawing
            osgDBPager->signalEndFrame();
        }

        // Next scene
        sceneInList = (vsScene *)sceneList->getNextEntry();
    }

    // Toss the scene list (but not the scenes in it!)
    sceneList->removeAllEntries();
    delete sceneList;
  
#ifdef WIN32
    // Windows only:  the message pump.  Check for Windows messages
    // in the message queue, and dispatch them to the message handler 
    // if any are waiting.
    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
    {
        DispatchMessage(&message);
    }
#endif
}
