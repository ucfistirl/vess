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
#include "vsTextureAttribute.h++"
#include "vsTransformAttribute.h++"
#include "vsMaterialAttribute.h++"
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

    // Start off uninitialized
    isInitted = 0;

    // Singleton verification
    if (systemObject)
    {
        printf("vsSystem::vsSystem: Only one vsSystem object may be in "
            "existance at any time\n");
        validObject = 0;
        return;
    }
    
    // Save this system object in the class pointer variable
    validObject = 1;
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
    
    // Create our database loader object
    databaseLoader = new vsDatabaseLoader();
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
// Initializes the system object. This involves constructing internally-
// used objects, forking multiple processes, and priming timestamp data.
// ------------------------------------------------------------------------
void vsSystem::init()
{
    int loop;
    struct timeval timeStruct;

    // Do nothing if this isn't a real system object
    if (!validObject)
	return;

    // Do nothing if this object has already been initialized
    if (isInitted)
    {
	printf("vsSystem::init: vsSystem object is already initialized\n");
	return;
    }

    // Initialize the database loader object
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
    
    // Create our object map and graphics state objects
    nodeMap = new vsObjectMap();
    graphicsState = new vsGraphicsState();

    // Initialize the current time
    gettimeofday(&timeStruct, NULL);
    lastFrameTimestamp = timeStruct.tv_sec + (timeStruct.tv_usec / 1000000.0);
    lastFrameDuration = 0.0;
    
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

    // Do nothing if this isn't a real system object
    if (!validObject)
	return;

    // Do nothing if this object has already been initialized
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
    
    // Create our object map and graphics state objects
    nodeMap = new vsObjectMap();
    graphicsState = new vsGraphicsState();

    // Mark the object as initialized
    isInitted = 1;

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
}

// ------------------------------------------------------------------------
// Retrieves one of the system's pipe objects, specified by index. The
// index of the first pipe is 0.
// ------------------------------------------------------------------------
vsPipe *vsSystem::getPipe(int index)
{
    // Do nothing if this isn't a real system object
    if (!validObject)
        return NULL;

    // Do nothing if the object hasn't been initialized
    if (!isInitted)
    {
	printf("vsSystem::getPipe: System object is not initialized\n");
	return NULL;
    }
        
    // Bounds check
    if ((index < 0) || (index >= screenCount))
    {
        printf("vsSystem::getPipe: Bad pipe index\n");
        return NULL;
    }
    
    // Reurn the desired pipe object
    return pipeArray[index];
}

// ------------------------------------------------------------------------
// Returns the number of screen objects
// ------------------------------------------------------------------------
int vsSystem::getScreenCount()
{
    return screenCount;
}

// ------------------------------------------------------------------------
// Retrieves one of the system's screen objects, specified by index. The
// index of the first screen is 0.
// ------------------------------------------------------------------------
vsScreen *vsSystem::getScreen(int index)
{
    // Do nothing if this isn't a real system object
    if (!validObject)
        return NULL;
        
    // Do nothing if the object hasn't been initialized
    if (!isInitted)
    {
	printf("vsSystem::getScreen: System object is not initialized\n");
	return NULL;
    }

    // Bounds check
    if ((index < 0) || (index >= screenCount))
    {
        printf("vsSystem::getScreen: Bad screen index\n");
        return NULL;
    }
    
    // Return the desired screen object
    return screenArray[index];
}

// ------------------------------------------------------------------------
// Retrieves the database loader object for this object
// ------------------------------------------------------------------------
vsDatabaseLoader *vsSystem::getLoader()
{
    // Do nothing if this isn't a real system object
    if (!validObject)
        return NULL;
        
    // Return the database loader
    return databaseLoader;
}

// ------------------------------------------------------------------------
// Passes a call to loadDatabase to the system object's database loader
// object, returning the result.
// ------------------------------------------------------------------------
vsComponent *vsSystem::loadDatabase(char *databaseFilename)
{
    // Do nothing if this isn't a real system object
    if (!validObject)
        return NULL;
        
    // Do nothing if the object hasn't been initialized
    if (!isInitted)
    {
	printf("vsSystem::loadDatabase: System object is not initialized\n");
	return NULL;
    }

    // Call the database loader to load the specified file, and return
    // whatever that returns
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
    
    // Mark this node as clean
    node->clean();

    // Activate all of the attributes on the node
    node->saveCurrentAttributes();
    node->applyAttributes();
    
    // If this node is a component, recurse on its children
    if (node->getNodeType() == VS_NODE_TYPE_COMPONENT)
    {
        component = (vsComponent *)node;
        for (loop = 0; loop < component->getChildCount(); loop++)
        {
            // Get the loop'th child of the component
            childNode = component->getChild(loop);

            // Recurse on the child if it needs to be cleaned
            if (childNode->isDirty())
                preFrameTraverse(childNode);
        }
    }
    
    // On the way back out, deactivate the attributes on the node
    node->restoreSavedAttributes();
}

// ------------------------------------------------------------------------
// VESS internal function
// Retrieves the node map object for the system object
// ------------------------------------------------------------------------
vsObjectMap *vsSystem::getNodeMap()
{
    // Do nothing if this isn't a real system object
    if (!validObject)
        return NULL;

    // Return the object map
    return nodeMap;
}

// ------------------------------------------------------------------------
// VESS internal function
// Retrieves the graphics state object for the system object
// ------------------------------------------------------------------------
vsGraphicsState *vsSystem::getGraphicsState()
{
    // Do nothing if this isn't a real system object
    if (!validObject)
        return NULL;

    // Return the graphics state
    return graphicsState;
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
    struct timeval timeStruct;
    double currentTime;
    
    // Update the viewpoint of each pane by its vsView object
    for (screenLoop = 0; screenLoop < screenCount; screenLoop++)
    {
        // Find the number of windows on the screen
        targetScreen = screenArray[screenLoop];
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
    // Do nothing if this isn't a real system object
    if (!validObject)
        return 0.0;
        
    // Do nothing if the object hasn't been initialized
    if (!isInitted)
    {
	printf("vsSystem::drawFrame: System object is not initialized\n");
	return 0.0;
    }

    // Return the frame time
    return lastFrameDuration;
}

// ------------------------------------------------------------------------
// Writes a textual representation of the scene rooted at the given node
// out to the specified file.
// ------------------------------------------------------------------------
void vsSystem::printScene(vsNode *targetNode, FILE *outputFile)
{
    int counts[256];
    
    // Call out helper function to write the scene data
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

void printMatRow(FILE *fp, vsMatrix mat, int rowNum)
{
    fprintf(fp, "%8.4lf%8.4lf%8.4lf%8.4lf", mat[rowNum][0], mat[rowNum][1],
        mat[rowNum][2], mat[rowNum][3]);
}

void printMat(FILE *fp, vsMatrix mat)
{
    int rowNum;

    for (rowNum = 0; rowNum < 4; rowNum++)
    {
        printMatRow(fp, mat, rowNum);
        fprintf(fp, "\n");
    }
}

void printVec(FILE *fp, vsVector vec)
{
    int i, vecSize;

    vecSize = vec.getSize();

    // Enclose the components of the vector in angle brackets
    fprintf(fp, "<");

    // Print all but the last component with a trailing comma and space
    for (i = 0; i < vecSize-1; i++)
        fprintf(fp, "%0.4lf, ", vec[i]);

    // Print the last component and close with an angle bracket
    fprintf(fp, "%0.4lf>", vec[vecSize-1]);
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
    int size;
    vsMatrix mat;
    double r, g, b;
    int mode;
    int geoType, geoCount;
    int geoBinding;
    vsVector geoVec;
    int attrData;

    // Type
    switch (targetNode->getNodeType())
    {
        case VS_NODE_TYPE_GEOMETRY:
            fprintf(outfile, "Geometry: ");
            break;
        case VS_NODE_TYPE_DYNAMIC_GEOMETRY:
            fprintf(outfile, "Dynamic Geometry: ");
            break;
        case VS_NODE_TYPE_COMPONENT:
            fprintf(outfile, "Component: ");
            break;
/*
        case VS_NODE_TYPE_SCENE:
            fprintf(outfile, "Scene: ");
            break;
*/
    }

    // Name
    if (strlen(targetNode->getName()) > 0)
        fprintf(outfile, "\"%s\" ", targetNode->getName());

    // Address
    fprintf(outfile, "address %p ", targetNode);

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

        // Print vertex coordinates
        if (geoCount > 0)
        {
            writeBlanks(outfile, (treeDepth * 2) + 3);
            fprintf(outfile, "{\n");
            size = geometry->getDataListSize(VS_GEOMETRY_VERTEX_COORDS);
            for (loop = 0; loop < size; loop++)
            {
                writeBlanks(outfile, (treeDepth * 2) + 5);
                geoVec = geometry->getData(VS_GEOMETRY_VERTEX_COORDS, loop);
                printVec(outfile, geoVec);
                fprintf(outfile, "\n");
            }
            writeBlanks(outfile, (treeDepth * 2) + 3);
            fprintf(outfile, "}\n");
        }

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

            // Print out each of the remaining three data lists
            switch (loop)
            {
                case 0:
                    size = geometry->getDataListSize(VS_GEOMETRY_NORMALS);
                    if (size > 0)
                    {
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "{\n");
                        for (sloop = 0; sloop < size; sloop++)
                        {
                            writeBlanks(outfile, (treeDepth * 2) + 5);
                            geoVec = geometry->getData(VS_GEOMETRY_NORMALS,
                                sloop);
                            printVec(outfile, geoVec);
                            fprintf(outfile, "\n");
                        }
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "}\n");
                    }
                    break;

                case 1:
                    size = geometry->getDataListSize(VS_GEOMETRY_COLORS);
                    if (size > 0)
                    {
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "{\n");
                        for (sloop = 0; sloop < size; sloop++)
                        {
                            writeBlanks(outfile, (treeDepth * 2) + 5);
                            geoVec = geometry->getData(VS_GEOMETRY_COLORS,
                                sloop);
                            printVec(outfile, geoVec);
                            fprintf(outfile, "\n");
                        }
                       writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "}\n");
                    }
                    break;

                case 2:
                    size = geometry->
                        getDataListSize(VS_GEOMETRY_TEXTURE_COORDS);
                    if (size > 0)
                    {
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "{\n");
                        for (sloop = 0; sloop < size; sloop++)
                        {
                            writeBlanks(outfile, (treeDepth * 2) + 5);
                            geoVec =
                                geometry->getData(VS_GEOMETRY_TEXTURE_COORDS,
                                sloop);
                            printVec(outfile, geoVec);
                            fprintf(outfile, "\n");
                        }
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "}\n");
                    }
                    break;
            }
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
                writeBlanks(outfile, (treeDepth * 2) + 3);
                mat = ((vsTransformAttribute *)attribute)->getPreTransform();
                fprintf(outfile, "Pretransform:\n");
                writeBlanks(outfile, (treeDepth * 2) + 5);
                printMatRow(outfile, mat, 0);
                fprintf(outfile, "\n");
                writeBlanks(outfile, (treeDepth * 2) + 5);
                printMatRow(outfile, mat, 1);
                fprintf(outfile, "\n");
                writeBlanks(outfile, (treeDepth * 2) + 5);
                printMatRow(outfile, mat, 2);
                fprintf(outfile, "\n");
                writeBlanks(outfile, (treeDepth * 2) + 5);
                printMatRow(outfile, mat, 3);
                fprintf(outfile, "\n");
                writeBlanks(outfile, (treeDepth * 2) + 3);
                mat =
                    ((vsTransformAttribute *)attribute)->getDynamicTransform();
                fprintf(outfile, "Dynamic transform:\n");
                writeBlanks(outfile, (treeDepth * 2) + 5);
                printMatRow(outfile, mat, 0);
                fprintf(outfile, "\n");
                writeBlanks(outfile, (treeDepth * 2) + 5);
                printMatRow(outfile, mat, 1);
                fprintf(outfile, "\n");
                writeBlanks(outfile, (treeDepth * 2) + 5);
                printMatRow(outfile, mat, 2);
                fprintf(outfile, "\n");
                writeBlanks(outfile, (treeDepth * 2) + 5);
                printMatRow(outfile, mat, 3);
                fprintf(outfile, "\n");
                writeBlanks(outfile, (treeDepth * 2) + 3);
                mat = ((vsTransformAttribute *)attribute)->getPostTransform();
                fprintf(outfile, "Posttransform:\n");
               writeBlanks(outfile, (treeDepth * 2) + 5);
                printMatRow(outfile, mat, 0);
                fprintf(outfile, "\n");
                writeBlanks(outfile, (treeDepth * 2) + 5);
                printMatRow(outfile, mat, 1);
                fprintf(outfile, "\n");
                writeBlanks(outfile, (treeDepth * 2) + 5);
                printMatRow(outfile, mat, 2);
                fprintf(outfile, "\n");
                writeBlanks(outfile, (treeDepth * 2) + 5);
                printMatRow(outfile, mat, 3);
                fprintf(outfile, "\n");
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
                writeBlanks(outfile, (treeDepth * 2) + 3);
              fprintf(outfile, "Ambient:\n");
                writeBlanks(outfile, (treeDepth * 2) + 5);
                fprintf(outfile, "Front:  ");
                ((vsMaterialAttribute *)attribute)->
                    getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_AMBIENT,
                    &r, &g, &b);
                fprintf(outfile, "%0.2lf %0.2lf %0.2lf\n", r, g, b);
                writeBlanks(outfile, (treeDepth * 2) + 5);
                fprintf(outfile, "Back:   ");
                ((vsMaterialAttribute *)attribute)->
                    getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_AMBIENT,
                    &r, &g, &b);
                fprintf(outfile, "%0.2lf %0.2lf %0.2lf\n", r, g, b);
                writeBlanks(outfile, (treeDepth * 2) + 3);
                fprintf(outfile, "Diffuse:\n");
                writeBlanks(outfile, (treeDepth * 2) + 5);
                fprintf(outfile, "Front:  ");
                ((vsMaterialAttribute *)attribute)->
                    getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_DIFFUSE,
                    &r, &g, &b);
                fprintf(outfile, "%0.2lf %0.2lf %0.2lf\n", r, g, b);
                writeBlanks(outfile, (treeDepth * 2) + 5);
                fprintf(outfile, "Back:   ");
                ((vsMaterialAttribute *)attribute)->
                    getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_DIFFUSE,
                    &r, &g, &b);
                fprintf(outfile, "%0.2lf %0.2lf %0.2lf\n", r, g, b);
                writeBlanks(outfile, (treeDepth * 2) + 3);
                fprintf(outfile, "Specular:\n");
                writeBlanks(outfile, (treeDepth * 2) + 5);
                fprintf(outfile, "Front:  ");
                ((vsMaterialAttribute *)attribute)->
                    getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_SPECULAR,                    &r, &g, &b);
                fprintf(outfile, "%0.2lf %0.2lf %0.2lf\n", r, g, b);
                writeBlanks(outfile, (treeDepth * 2) + 5);
                fprintf(outfile, "Back:   ");
                ((vsMaterialAttribute *)attribute)->
                    getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_SPECULAR,
                    &r, &g, &b);
                fprintf(outfile, "%0.2lf %0.2lf %0.2lf\n", r, g, b);
                writeBlanks(outfile, (treeDepth * 2) + 3);
                fprintf(outfile, "Emissive:\n");
                writeBlanks(outfile, (treeDepth * 2) + 5);
                fprintf(outfile, "Front:  ");
                ((vsMaterialAttribute *)attribute)->
                    getColor(VS_MATERIAL_SIDE_FRONT, VS_MATERIAL_COLOR_EMISSIVE,                    &r, &g, &b);
                fprintf(outfile, "%0.2lf %0.2lf %0.2lf\n", r, g, b);
                writeBlanks(outfile, (treeDepth * 2) + 5);
                fprintf(outfile, "Back:   ");
                ((vsMaterialAttribute *)attribute)->
                    getColor(VS_MATERIAL_SIDE_BACK, VS_MATERIAL_COLOR_EMISSIVE,
                    &r, &g, &b);
                fprintf(outfile, "%0.2lf %0.2lf %0.2lf\n", r, g, b);
                writeBlanks(outfile, (treeDepth * 2) + 3);
                fprintf(outfile, "Color Mode:\n");
                writeBlanks(outfile, (treeDepth * 2) + 5);
                fprintf(outfile, "Front:  ");
                mode = ((vsMaterialAttribute *)attribute)->
                    getColorMode(VS_MATERIAL_SIDE_FRONT);
                switch (mode)
                {
                    case VS_MATERIAL_CMODE_AMBIENT:
                        fprintf(outfile, "AMBIENT\n");
                        break;
                    case VS_MATERIAL_CMODE_DIFFUSE:
                        fprintf(outfile, "DIFFUSE\n");
                        break;
                    case VS_MATERIAL_CMODE_SPECULAR:
                        fprintf(outfile, "SPECULAR\n");
                        break;
                    case VS_MATERIAL_CMODE_EMISSIVE:
                        fprintf(outfile, "EMISSIVE\n");
                       break;
                    case VS_MATERIAL_CMODE_AMBIENT_DIFFUSE:
                        fprintf(outfile, "AMBIENT_DIFFUSE\n");
                        break;
                    case VS_MATERIAL_CMODE_NONE:
                        fprintf(outfile, "NONE\n");
                        break;
                }
                writeBlanks(outfile, (treeDepth * 2) + 5);
                fprintf(outfile, "Back:   ");
                mode = ((vsMaterialAttribute *)attribute)->
                    getColorMode(VS_MATERIAL_SIDE_BACK);
                switch (mode)
                {
                    case VS_MATERIAL_CMODE_AMBIENT:
                        fprintf(outfile, "AMBIENT\n");
                        break;
                    case VS_MATERIAL_CMODE_DIFFUSE:
                        fprintf(outfile, "DIFFUSE\n");
                        break;
                    case VS_MATERIAL_CMODE_SPECULAR:
                        fprintf(outfile, "SPECULAR\n");
                        break;
                    case VS_MATERIAL_CMODE_EMISSIVE:
                        fprintf(outfile, "EMISSIVE\n");
                        break;
                    case VS_MATERIAL_CMODE_AMBIENT_DIFFUSE:
                        fprintf(outfile, "AMBIENT_DIFFUSE\n");
                        break;
                    case VS_MATERIAL_CMODE_NONE:
                        fprintf(outfile, "NONE\n");
                        break;
                }
                break;

            case VS_ATTRIBUTE_TYPE_TEXTURE:
                fprintf(outfile, "TEXTURE\n");
                writeBlanks(outfile, (treeDepth * 2) + 3);
                fprintf(outfile, "Apply Mode: ");
                switch (((vsTextureAttribute *)attribute)->getApplyMode())
                {
                    case VS_TEXTURE_APPLY_DECAL:
                        fprintf(outfile, "DECAL\n");
                        break;
                    case VS_TEXTURE_APPLY_MODULATE:
                        fprintf(outfile, "MODULATE\n");
                        break;
                    case VS_TEXTURE_APPLY_REPLACE:
                        fprintf(outfile, "REPLACE\n");
                        break;
                    default:
                        fprintf(outfile, "(Unknown Mode)\n");
                        break;
                }
                writeBlanks(outfile, (treeDepth * 2) + 3);
                fprintf(outfile, "Mag Filter: ");
                switch (((vsTextureAttribute *)attribute)->getMagFilter())
                {
                    case VS_TEXTURE_MAGFILTER_NEAREST:
                        fprintf(outfile, "NEAREST\n");
                        break;
                    case VS_TEXTURE_MAGFILTER_LINEAR:
                        fprintf(outfile, "LINEAR\n");
                        break;
                    default:
                        fprintf(outfile, "(Unknown Mode)\n");
                        break;
                }
                writeBlanks(outfile, (treeDepth * 2) + 3);
                fprintf(outfile, "Min Filter: ");
                switch (((vsTextureAttribute *)attribute)->getMinFilter())
                {
                    case VS_TEXTURE_MINFILTER_NEAREST:
                        fprintf(outfile, "NEAREST\n");
                        break;
                    case VS_TEXTURE_MINFILTER_LINEAR:
                        fprintf(outfile, "LINEAR\n");
                        break;
                    case VS_TEXTURE_MINFILTER_MIPMAP_NEAREST:
                        fprintf(outfile, "MIPMAP NEAREST\n");
                        break;
                    case VS_TEXTURE_MINFILTER_MIPMAP_LINEAR:
                        fprintf(outfile, "MIPMAP LINEAR\n");
                        break;
                    default:
                        fprintf(outfile, "(Unknown Mode)\n");
                        break;
                }
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

            case VS_ATTRIBUTE_TYPE_SOUND_SOURCE:
                fprintf(outfile, "SOUND_SOURCE\n");
                break;

            case VS_ATTRIBUTE_TYPE_SOUND_LISTENER:
                fprintf(outfile, "SOUND_LISTENER\n");
                break;

            case VS_ATTRIBUTE_TYPE_WIREFRAME:
                fprintf(outfile, "WIREFRAME\n");
                break;

            default:
                fprintf(outfile, "<unknown type>\n");
                break;
        }
    }

    // If the node has children, take care of them
/*
    if ((targetNode->getNodeType() == VS_NODE_TYPE_COMPONENT) ||
        (targetNode->getNodeType() == VS_NODE_TYPE_SCENE))
*/
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
