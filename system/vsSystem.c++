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
#include <X11/Xlib.h>
#include <Performer/pf.h>
#include <Performer/pfutil.h>
#include "vsLightAttribute.h++"
#include "vsComponent.h++"
#include "vsGeometry.h++"
#include "vsOptimizer.h++"

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
    
    databaseLoader = new vsDatabaseLoader();
}

// ------------------------------------------------------------------------
// Destructor - Shuts down Performer, usually resulting in the program
// exiting
// ------------------------------------------------------------------------
vsSystem::~vsSystem()
{
    if (!validObject)
        return;
        
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
// Initializes the system object. This involves constructing internally-
// used objects, forking multiple processes, and priming timestamp data.
// ------------------------------------------------------------------------
void vsSystem::init()
{
    int loop;
    struct timeval timeStruct;

    if (!validObject)
	return;

    if (isInitted)
    {
	printf("vsSystem::init: vsSystem object is already initialized\n");
	return;
    }

    databaseLoader->init();

    // * This call can potentially fork new processes, so every object
    // that must be visible to all processes should be created before this
    pfConfig();
    
    // Set up the internal vsScreen and vsPipe objects
    for (loop = 0; loop < screenCount; loop++)
    {
        pipeArray[loop] = new vsPipe(loop);
        screenArray[loop] = new vsScreen(pipeArray[loop]);
        (pipeArray[loop])->getBaseLibraryObject()->setScreen(loop);
    }
    
    nodeMap = new vsObjectMap();
    graphicsState = new vsGraphicsState();

    // Initialize the current time
    gettimeofday(&timeStruct, NULL);
    lastFrameTimestamp = timeStruct.tv_sec + (timeStruct.tv_usec / 1000000.0);
    lastFrameDuration = 0.0;
    
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
    int loop;
    vsWindow *defaultWindow;
    vsPane *defaultPane;
    vsView *defaultView;
    vsVector upDir;
    vsVector dbCenter;
    double dbRadius;
    vsComponent *scene;
    vsLightAttribute *globalLight;
    struct timeval timeStruct;
    vsOptimizer *optimizer;

    if (!validObject)
	return;

    if (isInitted)
    {
	printf("vsSystem::init: vsSystem object is already initialized\n");
	return;
    }

    // Initialize the datbase loader object
    databaseLoader->initExtension(databaseFilename);
    databaseLoader->init();
    databaseLoader->setLoaderMode(VS_DATABASE_MODE_NAME_XFORM, 1);

    // * This call can potentially fork new processes, so every object
    // that must be visible to all processes should be created before this
    pfConfig();
    
    // Set up the internal vsScreen and vsPipe objects
    for (loop = 0; loop < screenCount; loop++)
    {
        pipeArray[loop] = new vsPipe(loop);
        screenArray[loop] = new vsScreen(pipeArray[loop]);
        (pipeArray[loop])->getBaseLibraryObject()->setScreen(loop);
    }
    
    nodeMap = new vsObjectMap();
    graphicsState = new vsGraphicsState();

    // Quick start: Set up the default window, pane, and view objects
    defaultWindow = new vsWindow(getScreen(0), fullScreen);
    if (fullScreen)
        defaultWindow->setFullScreen();
    if (windowName)
        defaultWindow->setName(windowName);
    else
        defaultWindow->setName(databaseFilename);

    defaultPane = new vsPane(defaultWindow);
    defaultPane->autoConfigure(VS_PANE_PLACEMENT_FULL_WINDOW);

    // Load the specified database
    scene = databaseLoader->loadDatabase(databaseFilename);
    optimizer = new vsOptimizer();
    optimizer->optimize(scene);
    delete optimizer;
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

    // Initialize the current time
    gettimeofday(&timeStruct, NULL);
    lastFrameTimestamp = timeStruct.tv_sec + (timeStruct.tv_usec / 1000000.0);
    lastFrameDuration = 0.0;

    // Return the requested values and finish
    if (sceneGraph)
        *sceneGraph = scene;
    if (viewpoint)
        *viewpoint = defaultView;
    if (window)
        *window = defaultWindow;

    isInitted = 1;
}

// ------------------------------------------------------------------------
// Retrieves one of the system's pipe objects, specified by index. The
// index of the first pipe is 0.
// ------------------------------------------------------------------------
vsPipe *vsSystem::getPipe(int index)
{
    if (!validObject)
        return NULL;

    if (!isInitted)
    {
	printf("vsSystem::getPipe: System object is not initialized\n");
	return NULL;
    }
        
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
        
    if (!isInitted)
    {
	printf("vsSystem::getScreen: System object is not initialized\n");
	return NULL;
    }

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
vsComponent *vsSystem::loadDatabase(char *databaseFilename)
{
    if (!validObject)
        return NULL;
        
    if (!isInitted)
    {
	printf("vsSystem::loadDatabase: System object is not initialized\n");
	return NULL;
    }

    return (databaseLoader->loadDatabase(databaseFilename));
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
    
    if (node->getNodeType() == VS_NODE_TYPE_COMPONENT)
    {
        component = (vsComponent *)node;
        for (loop = 0; loop < component->getChildCount(); loop++)
        {
            childNode = component->getChild(loop);
            if (childNode->isDirty())
                preFrameTraverse(childNode);
        }
    }
    
    node->restoreSavedAttributes();
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
    struct timeval timeStruct;
    double currentTime;
    
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

                scene = targetPane->getScene();
		if (scene)
		{
		    graphicsState->clearState();
		    preFrameTraverse(scene);
		}
            }
        }
    }
    
    // Wait until the next frame boundary
    pfSync();
    
    // Check how much time has elapsed since the last time we were here
    gettimeofday(&timeStruct, NULL);
    currentTime = timeStruct.tv_sec + (timeStruct.tv_usec / 1000000.0);
    lastFrameDuration = currentTime - lastFrameTimestamp;
    lastFrameTimestamp = currentTime;
    
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

// ------------------------------------------------------------------------
// Writes a textual representation of the scene rooted at the given node
// out to the specified file.
// ------------------------------------------------------------------------
void vsSystem::printScene(vsNode *targetNode, FILE *outputFile)
{
    int counts[256];
    
    writeScene(targetNode, outputFile,  0, counts);
}

// ------------------------------------------------------------------------
// Private function
// Writes the specified number of space characters to the given file
// ------------------------------------------------------------------------
void vsSystem::writeBlanks(FILE *outfile, int count)
{
    for (int loop = 0; loop < count; loop++)
        fprintf(outfile, " ");
}

// ------------------------------------------------------------------------
// Private function
// Recursive function that writes the specified scene to the given (open)
// file. The countArray contains the current child number at each depth
// level of the VESS tree.
// ------------------------------------------------------------------------
void vsSystem::writeScene(vsNode *targetNode, FILE *outfile, int treeDepth,
    int *countArray)
{
    vsComponent *component;
    vsGeometry *geometry;
    vsAttribute *attribute;
    int loop, sloop;
    vsVector texCoord;
    int geoType, geoCount;
    int geoBinding;
    int attrData;
    
    // Type
    if (targetNode->getNodeType() == VS_NODE_TYPE_GEOMETRY)
        fprintf(outfile, "Geometry ");
    else
        fprintf(outfile, "Component ");
    
    // Name
    if (strlen(targetNode->getName()) > 0)
        fprintf(outfile, "\"%s\" ", targetNode->getName());

    // Is instanced?
    if (targetNode->getParentCount() > 1)
        fprintf(outfile, "(instanced) ");

    fprintf(outfile, "\n");

    // If the node is a vsGeometry, write out all of the primitive and
    // binding info
    if (targetNode->getNodeType() == VS_NODE_TYPE_GEOMETRY)
    {
        geometry = (vsGeometry *)targetNode;

        // Primitive type & count
        writeBlanks(outfile, (treeDepth * 2) + 1);
        geoType = geometry->getPrimitiveType();
        geoCount = geometry->getPrimitiveCount();
        fprintf(outfile, "%d ", geoCount);
        switch (geoType)
        {
            case VS_GEOMETRY_TYPE_POINTS:
                fprintf(outfile, "POINTS");
                break;
            case VS_GEOMETRY_TYPE_LINES:
                fprintf(outfile, "LINES");
                break;
            case VS_GEOMETRY_TYPE_LINE_STRIPS:
                fprintf(outfile, "LINE STRIPS");
                break;
            case VS_GEOMETRY_TYPE_LINE_LOOPS:
                fprintf(outfile, "LINE LOOPS");
                break;
            case VS_GEOMETRY_TYPE_TRIS:
                fprintf(outfile, "TRIS");
                break;
            case VS_GEOMETRY_TYPE_TRI_STRIPS:
                fprintf(outfile, "TRI STRIPS");
                break;
            case VS_GEOMETRY_TYPE_TRI_FANS:
                fprintf(outfile, "TRI FANS");
                break;
            case VS_GEOMETRY_TYPE_QUADS:
                fprintf(outfile, "QUADS");
                break;
            case VS_GEOMETRY_TYPE_QUAD_STRIPS:
                fprintf(outfile, "QUAD STRIPS");
                break;
            case VS_GEOMETRY_TYPE_POLYS:
                fprintf(outfile, "POLYS");
                break;
            default:
                fprintf(outfile, "?");
                break;
        }
        fprintf(outfile, "\n");
        
        // Bindings
        for (loop = 0; loop < 3; loop++)
        {
            writeBlanks(outfile, (treeDepth * 2) + 1);
            switch (loop)
            {
                case 0:
                    fprintf(outfile, "NORMALS (%d): ",
                        geometry->getDataListSize(VS_GEOMETRY_NORMALS));
                    geoType = VS_GEOMETRY_NORMALS;
                    break;
                case 1:
                    fprintf(outfile, "COLORS (%d): ",
                        geometry->getDataListSize(VS_GEOMETRY_COLORS));
                    geoType = VS_GEOMETRY_COLORS;
                    break;
                case 2:
                    fprintf(outfile, "TEXCOORDS (%d): ",
                        geometry->getDataListSize(VS_GEOMETRY_TEXTURE_COORDS));
                    geoType = VS_GEOMETRY_TEXTURE_COORDS;
                    break;
            }
            geoBinding = geometry->getBinding(geoType);
            switch (geoBinding)
            {
                case VS_GEOMETRY_BIND_NONE:
                    fprintf(outfile, "NONE");
                    break;
                case VS_GEOMETRY_BIND_OVERALL:
                    fprintf(outfile, "OVERALL");
                    break;
                case VS_GEOMETRY_BIND_PER_PRIMITIVE:
                    fprintf(outfile, "PER PRIMITIVE");
                    break;
                case VS_GEOMETRY_BIND_PER_VERTEX:
                    fprintf(outfile, "PER VERTEX");
                    break;
            }
            fprintf(outfile, "\n");
        }
    }

    // Attributes
    for (loop = 0; loop < targetNode->getAttributeCount(); loop++)
    {
        attribute = targetNode->getAttribute(loop);
        writeBlanks(outfile, (treeDepth * 2) + 1);
        fprintf(outfile, "Attribute: address %p, references %d, type ",
            attribute, attribute->isAttached());
        switch (attribute->getAttributeType())
        {
            case VS_ATTRIBUTE_TYPE_TRANSFORM:
                fprintf(outfile, "TRANSFORM\n");
                break;
            case VS_ATTRIBUTE_TYPE_SWITCH:
                fprintf(outfile, "SWITCH\n");
                break;
            case VS_ATTRIBUTE_TYPE_SEQUENCE:
                fprintf(outfile, "SEQUENCE\n");
                break;
            case VS_ATTRIBUTE_TYPE_LOD:
                fprintf(outfile, "LOD\n");
                break;
            case VS_ATTRIBUTE_TYPE_LIGHT:
                fprintf(outfile, "LIGHT\n");
                break;
            case VS_ATTRIBUTE_TYPE_FOG:
                fprintf(outfile, "FOG\n");
                break;
            case VS_ATTRIBUTE_TYPE_MATERIAL:
                fprintf(outfile, "MATERIAL\n");
                break;
            case VS_ATTRIBUTE_TYPE_TEXTURE:
                fprintf(outfile, "TEXTURE\n");
                break;
            case VS_ATTRIBUTE_TYPE_TRANSPARENCY:
                attrData = ((vsTransparencyAttribute *)attribute)->isEnabled();
                if (attrData)
                    fprintf(outfile, "TRANSPARENCY (on)\n");
                else
                    fprintf(outfile, "TRANSPARENCY (off)\n");
                break;
            case VS_ATTRIBUTE_TYPE_BILLBOARD:
                fprintf(outfile, "BILLBOARD\n");
                break;
            case VS_ATTRIBUTE_TYPE_VIEWPOINT:
                fprintf(outfile, "VIEWPOINT\n");
                break;
            case VS_ATTRIBUTE_TYPE_BACKFACE:
                attrData = ((vsBackfaceAttribute *)attribute)->isEnabled();
                if (attrData)
                    fprintf(outfile, "BACKFACE (on)\n");
                else
                    fprintf(outfile, "BACKFACE (off)\n");
                break;
            case VS_ATTRIBUTE_TYPE_DECAL:
                fprintf(outfile, "DECAL\n");
                break;
            case VS_ATTRIBUTE_TYPE_SHADING:
                attrData = ((vsShadingAttribute *)attribute)->getShading();
                if (attrData == VS_SHADING_FLAT)
                    fprintf(outfile, "SHADING (flat)\n");
                else
                    fprintf(outfile, "SHADING (gouraud)\n");
                break;
        }
    }
    
    // If the node is a vsComponent, take care of its children
    if (targetNode->getNodeType() == VS_NODE_TYPE_COMPONENT)
    {
        component = (vsComponent *)targetNode;
        writeBlanks(outfile, treeDepth * 2);
        fprintf(outfile, "%d children:\n", component->getChildCount());
        
        // For each child, call this function again
        for (loop = 0; loop < component->getChildCount(); loop++)
        {
            countArray[treeDepth] = loop+1;
            writeBlanks(outfile, (treeDepth + 1) * 2);
            for (sloop = 0; sloop <= treeDepth; sloop++)
            {
                if (sloop != 0)
                    fprintf(outfile, ".");
                fprintf(outfile, "%d", countArray[sloop]);
            }
            fprintf(outfile, ") ");
            writeScene(component->getChild(loop), outfile, treeDepth+1,
                countArray);
        }
    }
}
