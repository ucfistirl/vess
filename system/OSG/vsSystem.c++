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

#include "vsLightAttribute.h++"
#include "vsLocalLightCallback.h++"
#include "vsScene.h++"
#include "vsComponent.h++"
#include "vsGeometry.h++"
#include "vsOptimizer.h++"
#include "vsGraphicsState.h++"
#include "vsGrowableArray.h++"
#include "vsDatabaseLoader.h++"
#include "vsViewpointAttribute.h++"
#include "vsWindowSystem.h++"
#include "vsTextBuilder.h++"
#include "vsClusterConfig.h++"

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
    cluster = NULL;
    slaves = NULL;
//    master = NULL;
    isSlave = false;
    readyToTerminate = false;
    
    // Initialize the remote interface
#ifdef VESS_DEBUG
    remoteInterface = new vsRemoteInterface("vessxml.dtd");
#else
    remoteInterface = new vsRemoteInterface();
#endif
}

// ------------------------------------------------------------------------
// Constructor - Same as the first constructor, but is enabled for cluster
// rendering.
// *Note: Only one of these objects may exist in a program at any one time.
// Attemps to create more will return in a non-functional vsSystem object.
// ------------------------------------------------------------------------
vsSystem::vsSystem(vsClusterConfig *config)
{
    int i;
    char slaveName[128];
    const unsigned char *slaveAddr;
    // Start out uninitialized
    isInitted = 0;

    // Singleton check
    if (systemObject)
    {
        printf("vsSystem::vsSystem: Only one vsSystem object may be in "
            "existance at any time\n");
        validObject = 0;
        return;
    }

    // Create an OSG FrameStamp (This is required by OSG.  It will be shared 
    // by all panes and hence, all osgUtil::SceneView objects)
    osgFrameStamp = new osg::FrameStamp();
    osgFrameStamp->ref();
    
    // Mark this instance as valid and set the static class variable to 
    // point to this instance
    validObject = 1;
    systemObject = this;
    //printf("Confirmed: I am getting at least this far\n");
    if(config && config->isValid())
    {
        cluster = config;
        slaves = (vsTCPNetworkInterface**)calloc(cluster->numSlaves(),
                sizeof(vsTCPNetworkInterface *));
        numSlaves = cluster->numSlaves();
        for(i=0;i < numSlaves;i++)
        {
            slaveAddr = cluster->getSlave(i);
            sprintf(slaveName,"%d.%d.%d.%d",(int)slaveAddr[0],(int)slaveAddr[1],
                   (int)slaveAddr[2],(int)slaveAddr[3]);
            slaves[i] = new vsTCPNetworkInterface(slaveName, 
                    VS_RI_DEFAULT_CONTROL_PORT);
            //slaves[i]->enableBlocking();
            //int x;
            while(slaves[i]->makeConnection() < 0);
            //printf("%d\n",x);
            slaves[i]->disableBlocking();
        }
        //master = NULL;
        isSlave = false;
    }
    else if(config == NULL)
    {
        cluster = NULL;
        slaves = NULL;
        //master = new vsRemoteInterface(VS_RI_DEFAULT_CONTROL_PORT);
        isSlave = true;
    }
    else
    {
        printf("vsSystem::vsSystem: Cluster rendering failure\n");
        validObject = 0;
        cluster = NULL;
        slaves = NULL;
  //      master = NULL;
        isSlave = false;
    }
    
    readyToTerminate = false;
    // Initialize the remote interface
#ifdef VESS_DEBUG
    remoteInterface = new vsRemoteInterface("vessxml.dtd");
#else
    remoteInterface = new vsRemoteInterface();
#endif

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
    int i;
    
    // Unreference the osg::FrameStamp
    osgFrameStamp->unref();
    
    // Delete statically-created objects in the various VESS classes
    vsTextBuilder::deleteVertexArray();
    vsGeometry::clearBinSortModes();
    vsGraphicsState::deleteInstance();
    vsViewpointAttribute::deleteMap();
    vsNode::deleteMap();
    vsTimer::deleteSystemTimer();
    vsWindowSystem::deleteMap();
    vsScreen::done();
    vsPipe::done();

    // Delete the remote interface
    delete remoteInterface;

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
    if(cluster)
        delete cluster;
    if(slaves)
    {
        for(i=0;i<numSlaves;i++)
        {
            if(slaves[i])
                delete slaves[i];
        }
        free(slaves);
    }
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
    vsVector upDir;
    vsVector dbCenter;
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
    vsGrowableArray *localLightArray;
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
                    callbackLightCount = localLightCallback->setLocalLights(
                        localLightArray, localLightArrayLength);
                }

                // If the callback has zero lights, remove the callback.
                if (callbackLightCount == 0)
                {
                    // Set this drawable's callback to NULL now.
                    drawable->setDrawCallback(NULL);

                    // Set the drawable to use display lists now.
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
                localLightCallback = new vsLocalLightCallback(localLightArray,
                    localLightArrayLength);
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
// The main function for any VESS program. Prompts each active pane object
// to render its attached geometry into its parent window.
// ------------------------------------------------------------------------
void vsSystem::drawFrame()
{
#ifdef WIN32
    MSG message;
#endif
    int numSlavesReportedIn;
    int screenLoop, windowLoop, paneLoop;
    int windowCount, paneCount;
    int i;
    vsScreen *targetScreen;
    vsWindow *targetWindow;
    vsPane *targetPane;
    vsScene *scene;
    char commStr[256];
    int screenCount = vsScreen::getScreenCount();

    // Do nothing if this isn't a real system object
    if (!validObject)
        return;
        
    // Do nothing if the object hasn't been initialized
    if (!isInitted)
    {
        printf("vsSystem::drawFrame: System object is not initialized\n");
        return;
    }
    
    // Have the remote interface process any requests, etc.
    remoteInterface->update();

    // If any of the vsGeometry's render bin modes changed last frame,
    // then we need to mark every geometry object in existance as dirty so
    // that the bin mode change gets applied to all geometry objects.
    if (vsGeometry::binModesChanged)
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
        vsGeometry::binModesChanged = false;
    }
    
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
                }
            }
        }
    }
    
    // Perform cluster rendering
    if (slaves != NULL && !isSlave)
    {
        // Send out relevant info to clients
        // Block until all clients acknowledge
        numSlavesReportedIn = 0;
        while (numSlavesReportedIn < numSlaves)
        {
            for (i=0; i<numSlaves; i++)
            {
                slaves[i]->read((u_char *)commStr, 256);
                if (!strcmp(commStr,"<?xml version=\"1.0\"?>\n"
                        "<vessxml version=\"1.0\">\n"
                        "<readytosync>\n"
                        "</readytosync>\n"
                        "</vessxml>"))
                {
                    numSlavesReportedIn++;
                    commStr[0]='\0';
                }
            }
        }
        
        // Send relase signal
        strcpy(commStr,"<?xml version=\"1.0\"?>\n"
                "<vessxml version=\"1.0\">\n"
                "<releasesync>\n"
                "</releasesync>\n"
                "</vessxml>");
        for (i=0; i<numSlaves; i++)
        {
            slaves[i]->write((u_char *)commStr, strlen(commStr)+1);
        }
    }
    else if (isSlave && !readyToTerminate)
    {
        // Collect info
        // Do processing on the info
        // --------------INCOMPLETE--------------
                
        // Send an acknowledgement
        strcpy(commStr,"<?xml version=\"1.0\"?>\n"
                "<vessxml version=\"1.0\">\n"
                "<readytosync>\n"
                "</readytosync>\n"
                "</vessxml>");
        remoteInterface->send((u_char *)commStr, strlen(commStr)+1);
        
        
        // Block until released by master
        readyToSwap = false;
        while (!readyToSwap && !readyToTerminate)
        {
            remoteInterface->update();
        }
        
    }
        

    
    // Mark the system timer for this frame
    vsTimer::getSystemTimer()->mark();

    // Update the OSG FrameStamp object
    frameNumber++;
    simTime += vsTimer::getSystemTimer()->getInterval();
    osgFrameStamp->setFrameNumber(frameNumber);
    osgFrameStamp->setReferenceTime(simTime);
    
    // Perform app, cull, and draw traversals for each pane
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

                // Do app and cull traversals
                targetPane->getBaseLibraryObject()->app();
                targetPane->getBaseLibraryObject()->cull();

                // Only draw the pane if the visibility flag is true
                if (targetPane->isVisible())
                {
                    targetPane->getBaseLibraryObject()->draw();
                }
            }

            // Swap buffers on this window
            targetWindow->swapBuffers();
        }
    }
    
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

// ------------------------------------------------------------------------
// Tells a cluster slave to swap now
// ------------------------------------------------------------------------
void vsSystem::releaseSync(void)
{
    readyToSwap = true;
}

// ------------------------------------------------------------------------
// Set a flag designating that the time has come to terminate
// ------------------------------------------------------------------------
void vsSystem::terminateCluster(void)
{
    int i;
    char commStr[256];
    
    readyToTerminate = true;
    
    // Send termination signal to all clients
    if (slaves)
    {
        strcpy(commStr,"<?xml version=\"1.0\"?>\n"
                "<vessxml version=\"1.0\">\n"
                "<terminatecluster>\n"
                "</terminatecluster>\n"
                "</vessxml>");
        for (i=0; i<numSlaves; i++)
        {
            slaves[i]->write((u_char *)commStr, strlen(commStr)+1);
        }
    }
}

// ------------------------------------------------------------------------
// Returns whether it is time to terminate
// ------------------------------------------------------------------------
bool vsSystem::hasBeenTerminated(void)
{
    return readyToTerminate;
}  
