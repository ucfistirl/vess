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
//    VESS Module:  vsAvatar.c++
//
//    Description:  Virtual base class for all avatar objects
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsAvatar.h++"

#include "vsDatabaseLoader.h++"
#include "vsISTJoystickBox.h++"
#include "vsUnwinder.h++"
#include "vsFlockOfBirds.h++"
#include "vsSerialMotionStar.h++"
#include "vsFastrak.h++"
#include "vsIS600.h++"
#include "vsEthernetMotionStar.h++"
#include "vsPolaris.h++"
#include "vsPinchGloveBox.h++"
#include "vsCyberGloveBox.h++"
#include "vsButtonAxis.h++"
#include "vs3TrackerArm.h++"
#include "vsAxisRotation.h++"
#include "vsCollision.h++"
#include "vsDrivingMotion.h++"
#include "vsFlyingMotion.h++"
#include "vsDifferentialTrackedOrientation.h++"
#include "vsPathMotion.h++"
#include "vsSequencer.h++"
#include "vsTerrainFollow.h++"
#include "vsTrackballMotion.h++"
#include "vsTrackedMotion.h++"
#include "vsVestSystem.h++"
#include "vsWalkArticulation.h++"
#include "vsWalkInPlace.h++"
#include "vsViewpointAttribute.h++"
#include "vsWindowSystem.h++"
#include "vsFPSMotion.h++"
#include "atString.h++"
#include "atStringTokenizer.h++"

// Include the vsWSSpaceball class if we're configured for a Linux or IRIX
// system
#if defined(__linux__) || defined(IRIX)
#include "vsWSSpaceball.h++"
#endif

// Include the vsLinuxJoystickSystem class only if we're configured for a 
// Linux system
#ifdef __linux__
#include "vsLinuxJoystickSystem.h++"
#endif

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsAvatar::vsAvatar()
{
    // Initialize class members
    cfgFile = NULL;
    masterScene = NULL;
    objectArray = NULL;
    objNameArray = NULL;
    objTypeArray = NULL;
    objectCount = 0;
    isInitted = false;
    geometryRoot = NULL;
}

// ------------------------------------------------------------------------
// Constructor.  Allows the scene to be specified via the parameter.
// ------------------------------------------------------------------------
vsAvatar::vsAvatar(vsNode *scene)
{
    // Initialize class members
    cfgFile = NULL;
    masterScene = scene;
    objectArray = NULL;
    objNameArray = NULL;
    objTypeArray = NULL;
    objectCount = 0;
    isInitted = false;
    geometryRoot = NULL;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsAvatar::~vsAvatar()
{
    // Close the config file
    if (cfgFile)
        fclose(cfgFile);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsAvatar::getClassName()
{
    return "vsAvatar";
}

// ------------------------------------------------------------------------
// Initialization function. Needs to be called before the avatar can be
// used. Reads the given configuration file, creates a series of objects
// from the configuration file data, and passes those objects to the
// avatar-specific setup function.
// ------------------------------------------------------------------------
void vsAvatar::init(char *configFile)
{
    char lineBuffer[256];
    int lineType;
    atStringTokenizer *tokens;
    atString *objectType;
    atString *objectName;
    vsObject *newObject;
    
    // Make sure init() is only called once
    if (isInitted)
    {
        printf("vsAvatar::init: Avatar has already been initialized\n");
        return;
    }

    isInitted = true;

    // Check to see if the user wants to go without a config file
    if (configFile == NULL)
    {
        setup();
        return;
    }

    // Open the config file
    cfgFile = fopen(configFile, "r");
    if (!cfgFile)
    {
        printf("vsAvatar::init: Unable to open configuration file %s\n",
            configFile);
        return;
    }
    
    // Create the parallel object/object name/object type arrays
    // and initialize the object count to 0
    objectArray = new vsArray();
    objNameArray = new atArray();
    objTypeArray = new atArray();
    objectCount = 0;

    // Parse the config file
    lineType = 0;
    while (lineType != VS_AVT_LINE_END)
    {
        // Read the next configuration line
        lineType = readCfgLine(lineBuffer);

        // Skip this line if it doesn't begin a new object
        if (lineType != VS_AVT_LINE_OBJECT)
            continue;

        // Get the object type and name from the line
        tokens = new atStringTokenizer(lineBuffer);
        objectType = tokens->getToken(" \n\r\t");
        objectName = tokens->getToken(" \n\r\t");
        delete tokens;
        
        // Create the new object based on the type field
        newObject = createObject(objectType->getString());
        
        // Add the object, the object name, and the object type
        // to the respective array
        addObjectToArrays(newObject, objectName, objectType);
    }
    
    // We're done configuring, so set up the vsAvatar with the given
    // configuration
    setup();
    
    // Clean up everything
    fclose(cfgFile);
    cfgFile = NULL;
    masterScene = NULL;
    delete objectArray;
    objectArray = NULL;
    delete objNameArray;
    objNameArray = NULL;
    delete objTypeArray;
    objTypeArray = NULL;
    objectCount = 0;
}

// ------------------------------------------------------------------------
// Retrieves the root node of the geometry for this avatar
// ------------------------------------------------------------------------
vsNode *vsAvatar::getGeometry()
{
    return geometryRoot;
}

// ------------------------------------------------------------------------
// Protected function
// Adds an object and its associated string data to the arrays that hold
// the current configuration objects. Has no effect if the avatar is not
// currently being initialized. The strings passed in are duplicated; the
// space they occupy can be reused after this function finishes.
// ------------------------------------------------------------------------
void vsAvatar::addObjectToArrays(vsObject *object, atString *name,
                                 atString *type)
{
    if (!objectArray)
        return;

    // Store the given data in our data arrays and increment the
    // current-stored-nuber-of-objects counter
    objectArray->addEntry(object);
    objNameArray->addEntry(name);
    objTypeArray->addEntry(type);
    objectCount++;
}

// ------------------------------------------------------------------------
// Protected function
// Reads a line from the open configuration file into the specified buffer.
// Blank lines and comments are weeded out. The leading token of each line
// is interpreted and removed. The function returns VS_AVT_LINE_OBJECT if a
// 'type' token is parsed, indicating a new object. A return value of 
// VS_AVT_LINE_PARAM indicates a 'set' token was parsed, indicating data 
// for an object under construction. If an 'end' token is parsed, or if the
// end-of-file is encountered, VS_AVT_LINE_END is returned.
// ------------------------------------------------------------------------
int vsAvatar::readCfgLine(char *buffer)
{
    char *p;
    char inBuffer[256], keyword[256];
    bool goodLine;

    // If there's no currently open configuration file, then just return
    // an 'end' value
    if (!cfgFile)
        return VS_AVT_LINE_END;

    // Strip whitespace
    fscanf(cfgFile, " \n");

    // If at the end-of-file, return an 'end' value
    if (feof(cfgFile))
        return VS_AVT_LINE_END;

    // Keep trying until we get a good line, or we run out of
    // config file
    goodLine = false; 
    while (!goodLine)
    {
        // Clear whitespace and check for end-of-file
        fscanf(cfgFile, " \n");
        if (feof(cfgFile))
            return VS_AVT_LINE_END;

        // Read in the line
        fgets(inBuffer, 255, cfgFile);

        // Strip newlines
        p = strchr(inBuffer, '\n');
        if (p)
            *p = 0;

        // Strip comments (comment character is "#")
        p = strchr(inBuffer, '#');
        if (p)
            *p = 0;

        // Determine if there's anything left on the line, and skip to
        // the next line if not
        if (strlen(inBuffer) == 0)
            continue;

        // Parse the first keyword (hopefully "type", "set", or "end")
        sscanf(inBuffer, "%s", keyword);
        
        // Figure out which type of line this is
        if (!strcmp(keyword, "end"))
        {
            // This line signals the end of the current object
            // configuration
            buffer[0] = 0;
            return VS_AVT_LINE_END;
        }
        else if (!strcmp(keyword, "set"))
        {
            // This line specifies a parameter to the object
            // currently being created
            p = strchr(inBuffer, ' ');
            if (!p)
                continue;

            // Copy the line to the buffer
            strcpy(buffer, &(p[1]));

            // Strip trailing whitespace
            p = buffer + strlen(buffer) - 1;
            while ((p > buffer) && ((*p == ' ') || (*p == '\t') ||
                   (*p == '\n') || (*p == '\r')))
            {
                // Set the whitespace character to NULL and back up a
                // character in the string
                *p = 0;
                p--;
            }

            // Return that we parsed a parameter line
            return VS_AVT_LINE_PARAM;
        }
        else if (!strcmp(keyword, "type"))
        {
            // This line is the beginning of a new object
            p = strchr(inBuffer, ' ');
            if (!p)
                continue;
            strcpy(buffer, &(p[1]));
            return VS_AVT_LINE_OBJECT;
        }
        else
        {
            printf("vsAvatar::readCfgLine: Unrecognized keyword '%s'\n",
                keyword);
        }
    }

    // Shouldn't ever reach this line
    return VS_AVT_LINE_END;
}

// ------------------------------------------------------------------------
// Protected function
// Helper function that searches the configuration object arrays for an
// object with a name equal to the 'targetStr' parameter. Returns a pointer
// to the object if found, NULL otherwise. This is a case-sensitive search.
// ------------------------------------------------------------------------
vsObject *vsAvatar::findObject(char *targetStr)
{
    int loop;
    atString *objName;

    // If the target is NULL, or if the object arrays aren't currently
    // in use, abort.
    if (!targetStr || !objectArray)
        return NULL;

    // Look for an object with the given name
    for (loop = 0; loop < objectCount; loop++)
    {
        // Check the target name against the loop'th object's name
        objName = (atString *)objNameArray->getEntry(loop);
        if (atString(targetStr).equals(objName))
        {
            // Found it!  Return a pointer to the associated object
            return objectArray->getEntry(loop);
        }
    }

    printf("vsAvatar::findObject: Can't find object '%s'\n", targetStr);
    return NULL;
}


// ------------------------------------------------------------------------
// Protected function
// Initiates construction of an object of the type specified by the given
// string. The various make* functions do the actual work of creating the
// requested object. If this function is overridden, it should still be
// called from the child class' version to handle the object types listed
// here. All of the make* functions have access to the configuration file
// in order to read in required data.
// ------------------------------------------------------------------------
vsObject *vsAvatar::createObject(char *idString)
{
    // Hand off processing to the object creation function corresponding
    // to the given type name

    if (!strcmp(idString, "geometry"))
        return makeGeometry();
    else if (!strcmp(idString, "viewpoint"))
        return makeViewpoint();
    else if (!strcmp(idString, "inputDevice"))
        return makeIODevice();
    else if (!strcmp(idString, "vsSequencer"))
        return makeVsSequencer();
    else if (!strcmp(idString, "vsISTJoystickBox"))
        return makeVsISTJoystickBox();
    else if (!strcmp(idString, "vsUnwinder"))
        return makeVsUnwinder();
    else if (!strcmp(idString, "vsFlockOfBirds"))
        return makeVsFlockOfBirds();
    else if (!strcmp(idString, "vsSerialMotionStar"))
        return makeVsSerialMotionStar();
    else if (!strcmp(idString, "vsFastrak"))
        return makeVsFastrak();
    else if (!strcmp(idString, "vsIS600"))
        return makeVsIS600();
    else if (!strcmp(idString, "vsEthernetMotionStar"))
        return makeVsEthernetMotionStar();
    else if (!strcmp(idString, "vsPolaris"))
        return makeVsPolaris();
#if defined(__linux__) || defined(IRIX)
    else if (!strcmp(idString, "vsWSSpaceball"))
        return makeVsWSSpaceball();
#endif
    else if (!strcmp(idString, "vsPinchGloveBox"))
        return makeVsPinchGloveBox();
    else if (!strcmp(idString, "vsCyberGloveBox"))
        return makeVsCyberGloveBox();
    else if (!strcmp(idString, "vsButtonAxis"))
        return makeVsButtonAxis();
    else if (!strcmp(idString, "vsKinematics"))
        return makeVsKinematics();
    else if (!strcmp(idString, "vs3TrackerArm"))
        return makeVs3TrackerArm();
    else if (!strcmp(idString, "vsAxisRotation"))
        return makeVsAxisRotation();
    else if (!strcmp(idString, "vsCollision"))
        return makeVsCollision();
    else if (!strcmp(idString, "vsDrivingMotion"))
        return makeVsDrivingMotion();
    else if (!strcmp(idString, "vsFlyingMotion"))
        return makeVsFlyingMotion();
    else if (!strcmp(idString, "vsDifferentialTrackedOrientation"))
        return makeVsDifferentialTrackedOrientation();
    else if (!strcmp(idString, "vsPathMotion"))
        return makeVsPathMotion();
    else if (!strcmp(idString, "vsTerrainFollow"))
        return makeVsTerrainFollow();
    else if (!strcmp(idString, "vsTrackballMotion"))
        return makeVsTrackballMotion();
    else if (!strcmp(idString, "vsTrackedMotion"))
        return makeVsTrackedMotion();
    else if (!strcmp(idString, "vsVestSystem"))
        return makeVsVestSystem();
    else if (!strcmp(idString, "vsWalkArticulation"))
        return makeVsWalkArticulation();
    else if (!strcmp(idString, "vsWalkInPlace"))
        return makeVsWalkInPlace();
#ifdef __linux__
    else if (!strcmp(idString, "vsLinuxJoystickSystem"))
        return makeVsLinuxJoystickSystem();
#endif
    else if (!strcmp(idString, "vsFPSMotion"))
        return makeVsFPSMotion();
    
    // If the type name is unrecognized, just return NULL
    return NULL;
}

// ------------------------------------------------------------------------
// Protected function
// Returns a serial port device name corresponding to the given input.  If
// the input is a simple number, it returns the device name associated with
// that port number, otherwise, it simply returns the input
// ------------------------------------------------------------------------
char *vsAvatar::getSerialPortName(char *portStr)
{
   long  number;
   char  *endPtr;

   // See if the given string is only a number
   number = strtol(portStr, &endPtr, 10);
   if (*endPtr == '\0')
   {
#ifdef __linux__
      sprintf(portStr, "/dev/ttyS%d", number - 1);
#endif

#ifdef WIN32
      sprintf(portStr, "COM%d", number - 1);
#endif
   }

   // Return the serial port device name
   return portStr;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a scene graph from data in the configuration file, and returns
// a pointer to the root node.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeGeometry()
{
    vsDatabaseLoader *dbLoader;
    vsOptimizer *optimizer;
    vsNode *result;
    vsNode *oldScene;
    vsComponent *newComponent;
    char cfgLine[256], token[256], strValue[256];
    int lineType = 0;
    char dbName[256];
    int intValue;
    bool optFlag = true;
    unsigned int isectVal = 0xFFFFFFFF;
    bool autoAdd = false;
    bool emptyFlag = false;
    
    // Create a database loader
    dbLoader = new vsDatabaseLoader();

    // Try to read all the required parameters
    dbName[0] = 0;
    result = NULL;
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "name"))
        {
            // Add the given name to the loader's "important node name" list
            sscanf(cfgLine, "%*s %s", strValue);
            dbLoader->addImportantNodeName(strValue);
        }
        else if (!strcmp(token, "clearnames"))
        {
            // Clear the loader's "important node name" list
            dbLoader->clearNames();
        }
        else if (!strcmp(token, "allnames"))
        {
            // Make all nodes important
            sscanf(cfgLine, "%*s %d", &intValue);
            
            if (intValue == 0)
				dbLoader->setLoaderMode(VS_DATABASE_MODE_NAME_ALL, false);
			else
				dbLoader->setLoaderMode(VS_DATABASE_MODE_NAME_ALL, true);
        }
        else if (!strcmp(token, "allxforms"))
        {
            // Make all nodes with transforms important
            sscanf(cfgLine, "%*s %d", &intValue);
            
            if (intValue == 0)
				dbLoader->setLoaderMode(VS_DATABASE_MODE_NAME_XFORM, false);
            else
				dbLoader->setLoaderMode(VS_DATABASE_MODE_NAME_XFORM, true);
        }
        else if (!strcmp(token, "units"))
        {
            // Set the database units
            sscanf(cfgLine, "%*s %s", strValue);
            if (!strcmp(strValue, "meters"))
                dbLoader->setUnits(VS_DATABASE_UNITS_METERS);
            else if (!strcmp(strValue, "feet"))
                dbLoader->setUnits(VS_DATABASE_UNITS_FEET);
            else if (!strcmp(strValue, "kilometers"))
                dbLoader->setUnits(VS_DATABASE_UNITS_KILOMETERS);
            else
                printf("vsAvatar::makeGeometry (units): "
                    "Unrecognized units '%s'\n", strValue);
        }
        else if (!strcmp(token, "filename"))
        {
            // Set the filename for the database
            sscanf(cfgLine, "%*s %s", dbName);
        }
        else if (!strcmp(token, "empty"))
        {
            // Signify that there will be no geometry
            emptyFlag = true;
        }
        else if (!strcmp(token, "optimize"))
        {
            // Set the optimize flag (0 = false, 1 = true)
            sscanf(cfgLine, "%*s %d", &intValue);

            if (intValue == 0)
                optFlag = false;
            else
                optFlag = true;
        }
        else if (!strcmp(token, "addpath"))
        {
            // Add a directory to the file search path
            sscanf(cfgLine, "%*s %s", strValue);
            dbLoader->addPath(strValue);
        }
        else if (!strcmp(token, "intersectValue"))
        {
            // Set the intersect value for the geometry
            sscanf(cfgLine, "%*s %x", &isectVal);
        }
        else if (!strcmp(token, "addToScene"))
        {
            // Set whether we should automatically add this avatar to
            // the scene
            sscanf(cfgLine, "%*s %d", &intValue);

            if (intValue == 0)
                autoAdd = false;
            else
                autoAdd = true;
        }
        else
            printf("vsAvatar::makeGeometry: Unrecognized token '%s'\n",
                token);
    }

    // Attempt to load the specified database file
    if (emptyFlag)
    {
        // If the empty flag is set, then there's no file to load;
        // just create a vsComponent with no children
        result = new vsComponent();
    }
    else if (strlen(dbName) > 0)
    {
        // If a filename is specified, try to load it now
        result = dbLoader->loadDatabase(dbName);
        if (result && optFlag)
        {
            // If the optimize flag is set, do optimization now
            optimizer = new vsOptimizer;
            optimizer->optimize(result);
            delete optimizer;
        }
    }
    
    // Set the intersect value
    if (result)
        result->setIntersectValue(isectVal);
    
    // Add the avatar geometry to the scene, if the autoAdd flag is set
    if (autoAdd && masterScene)
    {
        // If masterScene is a vsScene node, we'll need to do some
        // extra manipulation
        if (masterScene->getNodeType() == VS_NODE_TYPE_SCENE)
        {
            // See if the scene node has a child
            if (masterScene->getChildCount() > 0)
            {
                // Get the original scene data
                oldScene = masterScene->getChild(0);

                // If the oldScene node is a component, just add the avatar
                if (oldScene->getNodeType() == VS_NODE_TYPE_COMPONENT)
                {
                    oldScene->addChild(result);
                }
                else
                {
                    // oldScene is a kind of geometry node. Create a new 
                    // component to contain the existing scene data and the 
                    // avatar
                    newComponent = new vsComponent();

                    // Remove the old scene data from the masterScene node
                    masterScene->removeChild(oldScene);

                    // Add the original scene graph to the new component
                    newComponent->addChild(oldScene);

                    // Add the avatar to the new component
                    newComponent->addChild(result);

                    // Add the new component to the masterScene node
                    masterScene->addChild(newComponent);
                }
            }
            else
            {
                // No existing child, just add the avatar to the empty
                // scene node
                masterScene->addChild(result);
            }
        }
        // If masterScene is a Geometry (or Dynamic Geometry) node, we
        // can't add the avatar (no children on geometry nodes)
        else if ((masterScene->getNodeType() == VS_NODE_TYPE_GEOMETRY) ||
            (masterScene->getNodeType() == VS_NODE_TYPE_GEOMETRY))
        {
            printf("vsAvatar::makeGeometry:  Can't add avatar geometry to"
                   " a geometry node!\n");
        }
        // If masterScene is a component, just add the avatar
        else
        {
            masterScene->addChild(result);
        }
    }
    
    delete dbLoader;

    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsView and a vsViewpointAttribute together, and attaches them
// to a specifed vsPane and vsComponent, respectively.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeViewpoint()
{
    vsPane *pane = NULL;
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsViewpointAttribute *result;
    vsView *view;
    int screenNum, windowNum, paneNum;
    vsScreen *screen;
    vsWindow *window;
    char geoObjectName[256], nodeName[256];
    vsComponent *geom = NULL;
    vsComponent *root;
    int lineLen;
    atVector posOffset(0.0, 0.0, 0.0);
    atQuat oriOffset(0.0, 0.0, 0.0, 1.0);
    double xoffset = 0.0;
    double yoffset = 0.0;
    double zoffset = 0.0;
    double hoffset = 0.0;
    double poffset = 0.0;
    double roffset = 0.0;
    atMatrix offsetMat, tempMat;

    // Try to read all the parameters
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);

        // Interpret the first token
        if (!strcmp(token, "geometry"))
        {
            // Read in the name(s) of the object to find
            lineLen = sscanf(cfgLine, "%*s %s %s", geoObjectName, nodeName);

            // Find the vsComponent in the avatar's scene graph
            if (lineLen < 2)
            {
                // Search directly for the specified node
                geom = (vsComponent *)(findObject(geoObjectName));
            }
            else
            {
                // Search for the first node, and then search under that
                // one for the second node
                root = (vsComponent *)(findObject(geoObjectName));
                if (root)
                    geom = (vsComponent *)(root->findNodeByName(nodeName));
            }
        }
        else if (!strcmp(token, "pane"))
        {
            // Set the pane to which this viewpoint is attached
            sscanf(cfgLine, "%*s %d %d %d", &screenNum, &windowNum, &paneNum);
            screen = vsScreen::getScreen(screenNum);
            if (screen)
            {
                window = screen->getChildWindow(windowNum);
                if (window)
                    pane = window->getChildPane(paneNum);
            }
        }
        else if (!strcmp(token, "positionOffset"))
        {
            // Sets a translation for the viewpoint from the base position
            sscanf(cfgLine, "%*s %lf %lf %lf", &xoffset, &yoffset, &zoffset);
        }
        else if (!strcmp(token, "orientationOffset"))
        {
            // Sets a rotation for the viewpoint from the base orientation
            sscanf(cfgLine, "%*s %lf %lf %lf", &hoffset, &poffset, &roffset);
        }
        else
            printf("vsAvatar::makeViewpoint: Unrecognized token '%s'\n",
                token);
    }
    
    // Make sure both a pane and a node in the avatar's scene graph are 
    // specified
    if (!pane)
    {
        printf("vsAvatar::makeViewpoint: No pane specified\n");
        return NULL;
    }
    if (!geom)
    {
        printf("vsAvatar::makeViewpoint: No node specified for viewpoint "
            "to attach to\n");
        return NULL;
    }

    // Create a vsView and set the specified pane's view to it
    view = new vsView();
    pane->setView(view);

    // Create a vsViewpointAttribute and add it to the specified vsComponent
    result = new vsViewpointAttribute(view);
    geom->addAttribute(result);
    
    // Set the offsets for the vsViewpointAttribute
    offsetMat.setTranslation(xoffset, yoffset, zoffset);
    tempMat.setEulerRotation(AT_EULER_ANGLES_ZXY_R, hoffset, poffset, roffset);
    offsetMat = offsetMat * tempMat;
    result->setOffsetMatrix(offsetMat);

    // Return the create viewpoint attribute
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Extracts a subclass of vsIODevice out of a specified vsIOSystem.
// Motion models that can use a vsInputAxis or vsInputButton take one of
// these objects and get the axis or button from that.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeIODevice()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsIODevice *result = NULL;
    char objName[256];
    int objNum;
    int axisNum;
    vsJoystickBox *joyBox;
    vsTrackingSystem *trackSys;
    vsPinchGloveBox *pinchBox;
    vsScreen *screen;
    vsWindow *window;
    vsWindowSystem *wsys;
    int screenIdx, windowIdx;
    vsCyberGloveBox *cyberBox;

#if defined(__linux__) || defined(IRIX)
    vsWSSpaceball *wsSpaceball;
#endif

#ifdef __linux__
    vsLinuxJoystickSystem *joySys;
#endif

    // Read all of the parameters for this object
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "joystickBox"))
        {
            // Get a vsJoystick from a vsJoystickBox
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);
            joyBox = (vsJoystickBox *)(findObject(objName));
            if (joyBox)
                result = joyBox->getJoystick(objNum);
        }
#ifdef __linux__
        else if (!strcmp(token, "linuxJoystickSystem"))
        {
            // Get a vsJoystick from a vsLinuxJoystickSystem
            sscanf(cfgLine, "%*s %s", objName);
            joySys = (vsLinuxJoystickSystem *)(findObject(objName));
            if (joySys)
                result = joySys->getJoystick();
        }
#endif
        else if (!strcmp(token, "trackingSystem"))
        {
            // Get a vsMotionTracker from a vsTrackingSystem
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);
            trackSys = (vsTrackingSystem *)(findObject(objName));
            if (trackSys)
                result = trackSys->getTracker(objNum);
        }
        else if (!strcmp(token, "pinchGloveBox"))
        {
            // Get a vsChordGloves object from a vsPinchGloveBox
            sscanf(cfgLine, "%*s %s", objName);
            pinchBox = (vsPinchGloveBox *)(findObject(objName));
            if (pinchBox)
                result = pinchBox->getGloves();
        }
        else if (!strcmp(token, "windowSystem"))
        {
            // Get a vsMouse or vsKeyboard from a vsWindowSystem
            sscanf(cfgLine, "%*s %d %d %s", &screenIdx, &windowIdx, objName);
            screen = vsScreen::getScreen(screenIdx);
            if (screen)
            {
                // Attempt to obtain the specified window
                window = screen->getChildWindow(windowIdx);
                if (window)
                {
                    wsys = (vsWindowSystem *)
                        ((vsWindowSystem::getMap())->mapFirstToSecond(window));
                    if (!wsys)
                    {
                        wsys = new vsWindowSystem(window);
                        addObjectToArrays(wsys, new atString("vsWindowSystem"),
                            new atString("vsWindowSystem"));
                    }

                    // Determine if a keyboard or mouse is desired
                    if (!strcmp(objName, "mouse"))
                        result = wsys->getMouse();
                    else if (!strcmp(objName, "keyboard"))
                        result = wsys->getKeyboard();
                    else
                        printf("vsAvatar::makeIODevice (windowSystem): "
                            "Unrecognized window system device '%s'\n",
                            objName);
                }
            }
        }
#if defined(__linux__) || defined(IRIX)
        else if (!strcmp(token, "WSSpaceball"))
        {
            // Get a vsSpaceball from a vsWSSpaceball
            sscanf(cfgLine, "%*s %s", objName);
            wsSpaceball = (vsWSSpaceball *)(findObject(objName));
            if (wsSpaceball)
                result = wsSpaceball->getSpaceball();
        }
#endif
        else if (!strcmp(token, "cyberGloveBox"))
        {
            // Get a vsArticulationGlove from a vsCyberGloveBox
            sscanf(cfgLine, "%*s %s", objName);
            cyberBox = (vsCyberGloveBox *)(findObject(objName));
            if (cyberBox)
                result = cyberBox->getGlove();
        }
        else if (!strcmp(token, "invertAxis"))
        {
            // Invert one of the result device's axes
            sscanf(cfgLine, "%*s %d", &axisNum);

            // Don't try this unless we've already found the device
            // we're returning
            if ((result) && (axisNum < result->getNumAxes()))
                result->getAxis(axisNum)->setInverted(true);
        }
        else
            printf("vsAvatar::makeIODevice: Unrecognized token '%s'\n",
                token);
    }
    
    // Error checking
    if (!result)
        printf("vsAvatar::makeIODevice: No vsIOSystem specified\n");

    // Return the created input device
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsSequencer from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsSequencer()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    char objName[256];
    vsSequencer *result;
    vsUpdatable *updatable;
    double time;

    // Construct the sequencer _first_, so that we can add the updatable
    // objects directly to it, without having to store them and add them later
    result = new vsSequencer();

    // Read all the parameters
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);

        // Interpret the first token
        if (!strcmp(token, "add"))
        {
            // Read the object name
            sscanf(cfgLine, "%*s %s", objName);

            // Find the specified object and add it to the sequencer
            updatable = (vsUpdatable *)(findObject(objName));
            if (updatable)
                result->addUpdatable(updatable, objName);
        }
        else if (!strcmp(token, "addTimed"))
        {
            // Read the object name and update time
            sscanf(cfgLine, "%*s %s %lf", objName, &time);

            // Find the specified object and add it to the sequencer
            updatable = (vsUpdatable *)(findObject(objName));
            if (updatable)
                result->addUpdatable(updatable, time, objName);
        }
        else
            printf("vsAvatar::makeVsSequencer: Unrecognized token '%s'\n",
                token);
    }

    // Return the completed sequencer
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsISTJoystickBox from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsISTJoystickBox()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    char portDev[32];

    // Initialize
    portDev[0] = 0;

    // Read all the parameters
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);

        // Interpret the first token
        if (!strcmp(token, "port"))
        {
            // Set the serial port for the joystick box
            sscanf(cfgLine, "%*s %d", portDev);
            getSerialPortName(portDev);
        }
        else
            printf("vsAvatar::makeVsISTJoystickBox: Unrecognized token '%s'\n",
                token);
    }
    
    // Make sure the port was set
    if (portDev[0] == 0)
    {
        printf("vsAvatar::makeVsISTJoystickBox: No port specified\n");
        return NULL;
    }

    // Create and return the vsISTJoystickBox
    return (new vsISTJoystickBox(portDev));
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsUnwinder from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsUnwinder()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    char portDev[32];
    int joy1 = 1;
    int joy2 = 0;

    // Initialize
    portDev[0] = 0;

    // Read the parameters
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "port"))
        {
            // Set the serial port for the box
            sscanf(cfgLine, "%*s %s", portDev);
            getSerialPortName(portDev);
        }
        else if (!strcmp(token, "joy1"))
        {
            // Set whether the first joystick is connected or not
            sscanf(cfgLine, "%*s %d", &joy1);
        }
        else if (!strcmp(token, "joy2"))
        {
            // Set whether the second joystick is connected or not
            sscanf(cfgLine, "%*s %d", &joy2);
        }
        else
            printf("vsAvatar::makeVsUnwinder: Unrecognized token '%s'\n",
                token);
    }
    
    // Make sure the serial port was set properly
    if (portDev[0] == 0)
    {
        printf("vsAvatar::makeVsUnwinder: No port specified\n");
        return NULL;
    }

    // Create the unwinder object with the specified parameters and
    // return it
    return (new vsUnwinder(portDev, joy1, joy2));
}

#ifdef __linux__
// ------------------------------------------------------------------------
// Protected function
// Creates a vsLinuxJoystickSystem from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsLinuxJoystickSystem()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    char portName[256];
    vsLinuxJoystickSystem *result;

    // Initialize
    portName[0] = 0;

    // Read the parameters for the object
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);

        // Interpret the first token
        if (!strcmp(token, "port"))
        {
            // Set the port (actually the device file) for the object
            // to use
            sscanf(cfgLine, "%*s %s", portName);
        }
        else
            printf("vsAvatar::makeVsLinuxJoystickSystem: "
                "Unrecognized token '%s'\n", token);
    }

    // Make sure the port device was set properly
    if (strlen(portName) == 0)
    {
        printf("vsAvatar::makeVsLinuxJoystickSystem: No port specified\n");
        return NULL;
    }

    // Create the joystick system object with the specified parameters and
    // return it
    return (new vsLinuxJoystickSystem(portName));
}
#endif

// ------------------------------------------------------------------------
// Protected function
// Creates a vsFlockOfBirds from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsFlockOfBirds()
{
    char cfgLine[256];
    char token[256];
    char strValue[256];
    int lineType = 0;
    char portDev[32];
    char *portDevs[200];
    int whichPort;
    int nTrackers = 0;
    int dataFormat = VS_AS_DATA_POS_QUAT;
    int baud = 9600;
    int mode = VS_AS_MODE_FLOCK;
    int hemisphere = -1;
    int defaultHemisphere = -1;
    int hemispheres[200];
    int argc;
    char *ptr;
    int intValue;
    bool multiFlag = false;
    bool forkFlag = false;
    vsFlockOfBirds *result;
    int i;
    
    // Initialize the port device array
    memset(portDevs, 0, sizeof(portDevs));

    // Initialize the hemispheres array
    for (i = 0; i < 200; i++)
       hemispheres[i] = -1;

    // Read the parameters for the object
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "port"))
        {
            // Set the serial port
            sscanf(cfgLine, "%*s %s", portDev);
            getSerialPortName(portDev);

            // Copy the device name to the port devices array
            portDevs[0] = (char *) malloc(32);
            strcpy(portDevs[0], portDev);
        }
        else if (!strcmp(token, "mport"))
        {
            // For multi-port systems, set the serial port for
            // the given bird
            sscanf(cfgLine, "%*s %d", &whichPort);
            sscanf(cfgLine, "%*s %*d %s", portDev);
            getSerialPortName(portDev);
            multiFlag = true;

            // Copy the device name to the port devices array
            portDevs[whichPort] = (char *) malloc(32);
            strcpy(portDevs[whichPort], portDev);
        }
        else if (!strcmp(token, "trackers"))
        {
            // Set the number of trackers in the system
            sscanf(cfgLine, "%*s %d", &nTrackers);
        }
        else if (!strcmp(token, "format"))
        {
            // Set the data format to use
            sscanf(cfgLine, "%*s %s", strValue);
            
            if (!strcmp(strValue, "VS_AS_DATA_POSITION"))
                dataFormat = VS_AS_DATA_POSITION;
            else if (!strcmp(strValue, "VS_AS_DATA_ANGLES"))
                dataFormat = VS_AS_DATA_ANGLES;
            else if (!strcmp(strValue, "VS_AS_DATA_MATRIX"))
                dataFormat = VS_AS_DATA_MATRIX;
            else if (!strcmp(strValue, "VS_AS_DATA_QUATERNION"))
                dataFormat = VS_AS_DATA_QUATERNION;
            else if (!strcmp(strValue, "VS_AS_DATA_POS_ANGLES"))
                dataFormat = VS_AS_DATA_POS_ANGLES;
            else if (!strcmp(strValue, "VS_AS_DATA_POS_MATRIX"))
                dataFormat = VS_AS_DATA_POS_MATRIX;
            else if (!strcmp(strValue, "VS_AS_DATA_POS_QUAT"))
                dataFormat = VS_AS_DATA_POS_QUAT;
            else
                printf("vsAvatar::makeVsFlockOfBirds (format): "
                    "Unrecognized format constant '%s'\n", strValue);
        }
        else if (!strcmp(token, "baud"))
        {
            // Set the baud rate
            sscanf(cfgLine, "%*s %d", &baud);
        }
        else if (!strcmp(token, "mode"))
        {
            // Set the system mode ("standalone" for a single bird, or
            // "flock" for multiple birds)
            sscanf(cfgLine, "%*s %s", strValue);
            
            if (!strcmp(strValue, "VS_AS_MODE_FLOCK"))
                mode = VS_AS_MODE_FLOCK;
            else if (!strcmp(strValue, "VS_AS_MODE_STANDALONE"))
                mode = VS_AS_MODE_STANDALONE;
            else
                printf("vsAvatar::makeVsFlockOfBirds (mode): "
                    "Unrecognized mode constant '%s'\n", strValue);
        }
        else if (!strcmp(token, "fork"))
        {
            // Set whether the object should be run in a forked process
            sscanf(cfgLine, "%*s %d", &intValue);

            if (intValue == 0)
                forkFlag = false;
            else
                forkFlag = true;
        }
        else if (!strcmp(token, "hemisphere"))
        {
            // Count the number of arguments to the "set hemisphere" command.
            // This determines whether we're setting the default hemisphere
            // or the hemisphere for an individual tracker
            argc = 1;
            ptr = cfgLine;
            while ((ptr = strpbrk(ptr, " \t\n\r")) != NULL)
            {
                argc++;
                ptr += strspn(ptr, " \t\n\r");
            }

            // Now, read the arguments
            if (argc == 2)
               sscanf(cfgLine, "%*s %s", strValue);
            else
               sscanf(cfgLine, "%*s %d %s", &intValue, strValue);

            // Parse the hemisphere setting
            if (!strcmp(strValue, "VS_AS_HSPH_FORWARD"))
                hemisphere = VS_AS_HSPH_FORWARD;
            else if (!strcmp(strValue, "VS_AS_HSPH_AFT"))
                hemisphere = VS_AS_HSPH_AFT;
            else if (!strcmp(strValue, "VS_AS_HSPH_UPPER"))
                hemisphere = VS_AS_HSPH_UPPER;
            else if (!strcmp(strValue, "VS_AS_HSPH_LOWER"))
                hemisphere = VS_AS_HSPH_LOWER;
            else if (!strcmp(strValue, "VS_AS_HSPH_LEFT"))
                hemisphere = VS_AS_HSPH_LEFT;
            else if (!strcmp(strValue, "VS_AS_HSPH_RIGHT"))
                hemisphere = VS_AS_HSPH_RIGHT;
            else
                printf("vsAvatar::makeVsFlockOfBirds (hemisphere): "
                    "Unrecognized hemisphere constant '%s'\n", strValue);

            // Set the hemisphere in which this tracker (if a tracker was
            // specified) or all trackers will operate
            if (argc == 2)
                defaultHemisphere = hemisphere;
            else
                hemispheres[intValue] = hemisphere;
        }
        else
            printf("vsAvatar::makeVsFlockOfBirds: Unrecognized token '%s'\n",
                token);
    }

    // Make sure at least one serial port was specified
    if (portDevs[0] == NULL)
    {
        printf("vsAvatar::makeVsFlockOfBirds: Port(s) not specified\n");
        return NULL;
    }

    // Call the appropriate constructor based on the number of serial ports
    // used
    if (multiFlag)
        result = new vsFlockOfBirds(portDevs, nTrackers, dataFormat,
            baud);
    else
        result = new vsFlockOfBirds(portDevs[0], nTrackers, dataFormat,
            baud, mode);

    // Set the default hemisphere, if it was configured in the config file
    if (defaultHemisphere != -1)
        result->setActiveHemisphere(VS_AS_ALL_TRACKERS, defaultHemisphere);

    // Set hemispheres for individual trackers, if configured
    for (i = 0; i < 200; i++)
        if (hemispheres[i] != -1)
            result->setActiveHemisphere(i, hemispheres[i]);

    // Fork the process if the system was configured to fork
    if (forkFlag)
        result->forkTracking();

    // Clean up the port devices array
    for (i = 0; i < 200; i++)
        if (portDevs[i] != NULL)
            free(portDevs[i]);

    // Return the created flock of birds object
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsSerialMotionStar from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsSerialMotionStar()
{
    char cfgLine[256];
    char token[256];
    char strValue[256];
    int lineType = 0;
    char portDev[32];
    char *portDevs[200];
    int whichPort;
    int nTrackers = 0;
    int dataFormat = VS_AS_DATA_POS_QUAT;
    int baud = 9600;
    int hemisphere = -1;
    int defaultHemisphere = -1;
    int hemispheres[200];
    int argc;
    char *ptr;
    int intValue;
    bool multiFlag = false;
    bool forkFlag = false;
    vsSerialMotionStar *result;
    int i;

    // Initialize the port devices array
    memset(portDevs, 0, sizeof(portDevs));
    
    // Initialize the hemispheres array
    for (i = 0; i < 200; i++)
       hemispheres[i] = -1;

    // Read the parameters
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "port"))
        {
            // Set the serial port
            sscanf(cfgLine, "%*s %s", portDev);
            getSerialPortName(portDev);

            // Copy the device name to the port devices array
            portDevs[0] = (char *) malloc(32);
            strcpy(portDevs[0], portDev);
        }
        else if (!strcmp(token, "mport"))
        {
            // For multi-port systems, set the serial port for
            // the given bird
            sscanf(cfgLine, "%*s %d", &whichPort);
            sscanf(cfgLine, "%*s %*d %s", portDev);
            getSerialPortName(portDev);
            multiFlag = true;

            // Copy the device name to the port devices array
            portDevs[whichPort] = (char *) malloc(32);
            strcpy(portDevs[whichPort], portDev);
        }
        else if (!strcmp(token, "trackers"))
        {
            // Set the number of trackers in the system
            sscanf(cfgLine, "%*s %d", &nTrackers);
        }
        else if (!strcmp(token, "format"))
        {
            // Set the data format to use
            sscanf(cfgLine, "%*s %s", strValue);
            
            // Interpret the data format string constant
            if (!strcmp(strValue, "VS_AS_DATA_POSITION"))
                dataFormat = VS_AS_DATA_POSITION;
            else if (!strcmp(strValue, "VS_AS_DATA_ANGLES"))
                dataFormat = VS_AS_DATA_ANGLES;
            else if (!strcmp(strValue, "VS_AS_DATA_MATRIX"))
                dataFormat = VS_AS_DATA_MATRIX;
            else if (!strcmp(strValue, "VS_AS_DATA_QUATERNION"))
                dataFormat = VS_AS_DATA_QUATERNION;
            else if (!strcmp(strValue, "VS_AS_DATA_POS_ANGLES"))
                dataFormat = VS_AS_DATA_POS_ANGLES;
            else if (!strcmp(strValue, "VS_AS_DATA_POS_MATRIX"))
                dataFormat = VS_AS_DATA_POS_MATRIX;
            else if (!strcmp(strValue, "VS_AS_DATA_POS_QUAT"))
                dataFormat = VS_AS_DATA_POS_QUAT;
            else
                printf("vsAvatar::makeVsSerialMotionStar (format): "
                    "Unrecognized format constant '%s'\n", strValue);
        }
        else if (!strcmp(token, "baud"))
        {
            // Set the baud rate
            sscanf(cfgLine, "%*s %d", &baud);
        }
        else if (!strcmp(token, "fork"))
        {
            // Set whether the object should be run in a forked process
            sscanf(cfgLine, "%*s %d", &intValue);

            if (intValue == 0)
                forkFlag = false;
            else
                forkFlag = true;
        }
        else if (!strcmp(token, "hemisphere"))
        {
            // Count the number of arguments to the "set hemisphere" command.
            // This determines whether we're setting the default hemisphere
            // or the hemisphere for an individual tracker
            argc = 1;
            ptr = cfgLine;
            while ((ptr = strpbrk(ptr, " \t\n\r")) != NULL)
            {
                argc++;
                ptr += strspn(ptr, " \t\n\r");
            }

            // Now, read the arguments
            if (argc == 2)
               sscanf(cfgLine, "%*s %s", strValue);
            else
               sscanf(cfgLine, "%*s %d %s", &intValue, strValue);

            // Parse the hemisphere setting
            if (!strcmp(strValue, "VS_AS_HSPH_FORWARD"))
                hemisphere = VS_AS_HSPH_FORWARD;
            else if (!strcmp(strValue, "VS_AS_HSPH_AFT"))
                hemisphere = VS_AS_HSPH_AFT;
            else if (!strcmp(strValue, "VS_AS_HSPH_UPPER"))
                hemisphere = VS_AS_HSPH_UPPER;
            else if (!strcmp(strValue, "VS_AS_HSPH_LOWER"))
                hemisphere = VS_AS_HSPH_LOWER;
            else if (!strcmp(strValue, "VS_AS_HSPH_LEFT"))
                hemisphere = VS_AS_HSPH_LEFT;
            else if (!strcmp(strValue, "VS_AS_HSPH_RIGHT"))
                hemisphere = VS_AS_HSPH_RIGHT;
            else
                printf("vsAvatar::makeVsSerialMotionStar (hemisphere): "
                    "Unrecognized hemisphere constant '%s'\n", strValue);

            // Set the hemisphere in which this tracker (if a tracker was
            // specified) or all trackers will operate
            if (argc == 2)
                defaultHemisphere = hemisphere;
            else
                hemispheres[intValue] = hemisphere;
        }
        else
            printf("vsAvatar::makeVsSerialMotionStar: Unrecognized "
                "token '%s'\n", token);
    }
    
    // Make sure at least one serial port was specified
    if (portDevs[0] == NULL)
    {
        printf("vsAvatar::makeVsSerialMotionStar: Port number(s) not "
            "specified\n");
        return NULL;
    }

    // Call the appropriate constructor based on the number of ports used
    if (multiFlag)
        result = new vsSerialMotionStar(portDevs, nTrackers, dataFormat,
            baud);
    else
        result = new vsSerialMotionStar(portDevs[0], nTrackers, dataFormat,
            baud);

    // Set the default hemisphere, if it was configured in the config file
    if (defaultHemisphere != -1)
        result->setActiveHemisphere(VS_AS_ALL_TRACKERS, defaultHemisphere);

    // Set hemispheres for individual trackers, if configured
    for (i = 0; i < 200; i++)
        if (hemispheres[i] != -1)
            result->setActiveHemisphere(i, hemispheres[i]);

    // Fork the process if configured to do so
    if (forkFlag)
        result->forkTracking();

    // Clean up the port devices array
    for (i = 0; i < 200; i++)
        if (portDevs[i] != NULL)
            free(portDevs[i]);

    // Return the created motion star object
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsFastrak from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsFastrak()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    char portDev[32];
    int baud = 9600;
    int nTrackers = 0;
    int intValue;
    bool forkFlag = false;
    atVector hemiVectors[VS_FT_MAX_TRACKERS];
    int stationNum, loop;
    double hemiX, hemiY, hemiZ;
    vsFastrak *result;

    // Initialize the port device
    portDev[0] = 0;

    // Read all the parameters for this object
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);

        // Interpret the first token
        if (!strcmp(token, "port"))
        {
            // Set the serial port
            sscanf(cfgLine, "%*s %s", &portDev);
            getSerialPortName(portDev);
        }
        else if (!strcmp(token, "baud"))
        {
            // Set the baud rate
            sscanf(cfgLine, "%*s %d", &baud);
        }
        else if (!strcmp(token, "trackers"))
        {
            // Set the number of trackers connected
            sscanf(cfgLine, "%*s %d", &nTrackers);
        }
        else if (!strcmp(token, "fork"))
        {
            // Set whether the system should be run from a
            // forked process
            sscanf(cfgLine, "%*s %d", &intValue);

            if (intValue == 0)
                forkFlag = false;
            else
                forkFlag = true;
        }
        else if (!strcmp(token, "trackerHemi"))
        {
            // Set the active hemisphere of the specified tracker
            sscanf(cfgLine, "%*s %d %lf %lf %lf", &stationNum, &hemiX,
                &hemiY, &hemiZ);
            (hemiVectors[stationNum-1]).set(hemiX, hemiY, hemiZ);
        }
        else
            printf("vsAvatar::makeVsFastrak: Unrecognized token '%s'\n",
                token);
    }
    
    // Make sure the serial port was specified
    if (portDev[0] == 0)
    {
        printf("vsAvatar::makeVsFastrak: No port specified\n");
        return NULL;
    }

    // Create the object
    result = new vsFastrak(portDev, baud, nTrackers);
    
    // Set the hemisphere of each tracker if specified
    for (loop = 0; loop < VS_FT_MAX_TRACKERS; loop++)
        if (hemiVectors[loop].getMagnitude() > 1E-6)
            result->setActiveHemisphere(loop+1, hemiVectors[loop]);
    
    // Fork the tracking process if the object was so configured
    if (forkFlag)
        result->forkTracking();

    // Return the created Fastrak object
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsIS600 from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsIS600()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    char portDev[32];
    int baud = 9600;
    int nTrackers = 0;
    int intValue;
    bool forkFlag = false;
    vsIS600 *result;

    // Initialize the port device
    portDev[0] = 0;

    // Read all of the settings for this object
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);

        // Interpret the first token
        if (!strcmp(token, "port"))
        {
            // Set the serial port
            sscanf(cfgLine, "%*s %s", portDev);
            getSerialPortName(portDev);
        }
        else if (!strcmp(token, "baud"))
        {
            // Set the baud rate
            sscanf(cfgLine, "%*s %d", &baud);
        }
        else if (!strcmp(token, "trackers"))
        {
            // Set the number of trackers in the system
            sscanf(cfgLine, "%*s %d", &nTrackers);
        }
        else if (!strcmp(token, "fork"))
        {
            // Set whether the system should be run from a forked process
            sscanf(cfgLine, "%*s %d", &intValue);

            if (intValue == 0)
                forkFlag = false;
            else
                forkFlag = true;
        }
        else
            printf("vsAvatar::makeVsIS600: Unrecognized token '%s'\n",
                token);
    }
    
    // Make sure the serial port was specified
    if (portDev[0] == 0)
    {
        printf("vsAvatar::makeVsIS600: No port specified\n");
        return NULL;
    }

    // Create the vsIS600 object
    result = new vsIS600(portDev, baud, nTrackers);
    
    // Fork the tracking process if so configured
    if (forkFlag)
        result->forkTracking();

    // Return the created IS600 object
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsEthernetMotionStar from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsEthernetMotionStar()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    char serverName[256], strValue[256];
    int portNumber = -1;
    int nTrackers = 0;
    int dataFormat = VS_BN_FLOCK_POSITIONQUATERNION;
    int hemisphere = -1;
    int defaultHemisphere = -1;
    int hemispheres[200];
    int i;
    int argc;
    char *ptr;
    int intValue;
    bool masterFlag = true;
    bool forkFlag = false;
    vsEthernetMotionStar *result;
    
    // Clear the server name as a sentinel value
    serverName[0] = 0;

    // Initialize the hemispheres array
    for (i = 0; i < 200; i++)
       hemispheres[i] = -1;

    // Read all the parameters for this object
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "port"))
        {
            // Set the UDP or TCP port used by the system
            sscanf(cfgLine, "%*s %d", &portNumber);
        }
        else if (!strcmp(token, "ip"))
        {
            // Set the MotionStar server name or IP address
            sscanf(cfgLine, "%*s %s", serverName);
        }
        else if (!strcmp(token, "trackers"))
        {
            // Set the number of trackers in the system
            sscanf(cfgLine, "%*s %d", &nTrackers);
        }
        else if (!strcmp(token, "format"))
        {
            // Set the data format used
            sscanf(cfgLine, "%*s %s", strValue);
            
            if (!strcmp(strValue, "VS_BN_FLOCK_NOBIRDDATA"))
                dataFormat = VS_BN_FLOCK_NOBIRDDATA;
            else if (!strcmp(strValue, "VS_BN_FLOCK_POSITION"))
                dataFormat = VS_BN_FLOCK_POSITION;
            else if (!strcmp(strValue, "VS_BN_FLOCK_ANGLES"))
                dataFormat = VS_BN_FLOCK_ANGLES;
            else if (!strcmp(strValue, "VS_BN_FLOCK_MATRIX"))
                dataFormat = VS_BN_FLOCK_MATRIX;
            else if (!strcmp(strValue, "VS_BN_FLOCK_POSITIONANGLES"))
                dataFormat = VS_BN_FLOCK_POSITIONANGLES;
            else if (!strcmp(strValue, "VS_BN_FLOCK_POSITIONMATRIX"))
                dataFormat = VS_BN_FLOCK_POSITIONMATRIX;
            else if (!strcmp(strValue, "VS_BN_FLOCK_QUATERNION"))
                dataFormat = VS_BN_FLOCK_QUATERNION;
            else if (!strcmp(strValue, "VS_BN_FLOCK_POSITIONQUATERNION"))
                dataFormat = VS_BN_FLOCK_POSITIONQUATERNION;
            else
                printf("vsAvatar::makeVsEthernetMotionStar (format): "
                    "Unrecognized format constant '%s'\n", strValue);
        }
        else if (!strcmp(token, "master"))
        {
            // Set whether this vsEthernetMotionStar object is a master
            // or a slave instance
            sscanf(cfgLine, "%*s %d", &intValue);

            if (intValue == 0)
                masterFlag = false; 
            else
                masterFlag = true;
        }
        else if (!strcmp(token, "fork"))
        {
            // Set whether this object should be run from a forked process
            sscanf(cfgLine, "%*s %d", &intValue);

            if (intValue == 0)
                forkFlag = false;
            else
                forkFlag = true;
        }
        else if (!strcmp(token, "hemisphere"))
        {
            // Count the number of arguments to the "set hemisphere" command.
            // This determines whether we're setting the default hemisphere
            // or the hemisphere for an individual tracker
            argc = 1;
            ptr = cfgLine;
            while ((ptr = strpbrk(ptr, " \t\n\r")) != NULL)
            {
                argc++;
                ptr += strspn(ptr, " \t\n\r");
            }

            // Now, read the arguments
            if (argc == 2)
               sscanf(cfgLine, "%*s %s", strValue);
            else
               sscanf(cfgLine, "%*s %d %s", &intValue, strValue);

            // Parse the hemisphere setting
            if (!strcmp(strValue, "VS_BN_FRONT_HEMISPHERE"))
                hemisphere = VS_BN_FRONT_HEMISPHERE;
            else if (!strcmp(strValue, "VS_BN_REAR_HEMISPHERE"))
                hemisphere = VS_BN_REAR_HEMISPHERE;
            else if (!strcmp(strValue, "VS_BN_UPPER_HEMISPHERE"))
                hemisphere = VS_BN_UPPER_HEMISPHERE;
            else if (!strcmp(strValue, "VS_BN_LOWER_HEMISPHERE"))
                hemisphere = VS_BN_LOWER_HEMISPHERE;
            else if (!strcmp(strValue, "VS_BN_LEFT_HEMISPHERE"))
                hemisphere = VS_BN_LEFT_HEMISPHERE;
            else if (!strcmp(strValue, "VS_BN_RIGHT_HEMISPHERE"))
                hemisphere = VS_BN_RIGHT_HEMISPHERE;
            else
                printf("vsAvatar::makeVsEthernetMotionStar (hemisphere): "
                    "Unrecognized hemisphere constant '%s'\n", strValue);

            // Set the hemisphere in which this tracker (if a tracker was
            // specified) or all trackers will operate
            if (argc == 2)
                defaultHemisphere = hemisphere;
            else
                hemispheres[intValue] = hemisphere;
        }
        else
            printf("vsAvatar::makeVsEthernetMotionStar: Unrecognized "
                "token '%s'\n", token);
    }

    // Make sure the server's name or IP and port are set
    if (portNumber == -1)
    {
        printf("vsAvatar::makeVsEthernetMotionStar: No port number "
            "specified\n");
        return NULL;
    }
    if (strlen(serverName) < 1)
    {
        printf("vsAvatar::makeVsEthernetMotionStar: No host address "
            "specified\n");
        return NULL;
    }

    // Construct the object
    result = new vsEthernetMotionStar(serverName, portNumber, nTrackers,
        masterFlag, dataFormat);

    // Set the default hemisphere, if it was configured in the config file
    if (defaultHemisphere != -1)
        result->setActiveHemisphere(VS_MSTAR_ALL_TRACKERS, defaultHemisphere);

    // Set hemispheres for individual trackers, if configured
    for (i = 0; i < 200; i++)
        if (hemispheres[i] != -1)
            result->setActiveHemisphere(i, hemispheres[i]);

    // Fork the process if so configured
    if (forkFlag)
        result->forkTracking();

    // Return the created motion star object
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsPolaris from data in the configuration file, and returns a 
// pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsPolaris()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    long baudRate = 9600;
    char portDev[32];
    int nTrackers = 0;
    bool refSet = false;
    double h, p, r;
    int intValue;
    bool forkFlag = false;
    vsPolaris *result;

    // Initialize the port device name
    portDev[0] = 0;
    
    // Read all the parameters for this object
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "port"))
        {
            // Set the serial port number used by the system
            sscanf(cfgLine, "%*s %s", portDev);
            getSerialPortName(portDev);
        }
        else if (!strcmp(token, "baud"))
        {
            // Set the baud rate used to communicate
            sscanf(cfgLine, "%*s %d", &baudRate);
        }
        else if (!strcmp(token, "trackers"))
        {
            // Set the number of trackers in the system
            sscanf(cfgLine, "%*s %d", &nTrackers);
        }
        else if (!strcmp(token, "refFrame"))
        {
            // Set the reference frame of the camera box
            sscanf(cfgLine, "%*s %lf %lf %lf", &h, &p, &r);
            refSet = true;
        }
        else if (!strcmp(token, "fork"))
        {
            // Set whether this object should be run from a forked process
            sscanf(cfgLine, "%*s %d", &intValue);

            if (intValue == 0)
                forkFlag = false;
            else
                forkFlag = true;
        }
        else
            printf("vsAvatar::makeVsEthernetMotionStar: Unrecognized "
                "token '%s'\n", token);
    }

    // Make sure the serial port number is set
    if (portDev[0] == 0)
    {
        printf("vsAvatar::makeVsPolaris: No port specified\n");
        return NULL;
    }

    // Construct the object
    result = new vsPolaris(portDev, baudRate, nTrackers);

    // See if the reference frame was altered
    if (refSet)
    {
        // Adjust the reference frame according to the given parameters
        result->setReferenceFrame(h, p, r);
    }

    // Fork the process if so configured
    if (forkFlag)
        result->forkTracking();

    // Return the created motion star object
    return result;
}

#if defined(__linux__) || defined(IRIX)
// ------------------------------------------------------------------------
// Protected function
// Creates a vsWSSpaceball from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsWSSpaceball()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsWindowSystem *wsys = NULL;
    int btnCount = 1;
    int screenNum, windowNum;
    vsScreen *screen;
    vsWindow *window;

    // Read the settings for this object
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "window"))
        {
            // Set which window this spaceball is attached to. If the
            // screen or window number is invalid, no action is taken.
            sscanf(cfgLine, "%*s %d %d", &screenNum, &windowNum);
            screen = vsScreen::getScreen(screenNum);
            if (screen)
            {
                window = screen->getChildWindow(windowNum);
                if (window)
                {
                    // Get the vsWindowSystem from this window, or create
                    // one if necessary
                    wsys = (vsWindowSystem *)
                        ((vsWindowSystem::getMap())->mapFirstToSecond(window));
                    if (!wsys)
                    {
                        // Create a new window system object on the
                        // specified window
                        wsys = new vsWindowSystem(window);
                        addObjectToArrays(wsys, new atString("vsWindowSystem"),
                            new atString("vsWindowSystem"));
                    }
                }
            }
        }
        else if (!strcmp(token, "buttons"))
        {
            // Set the number of buttons on the spaceball
            sscanf(cfgLine, "%*s %d", &btnCount);
        }
        else
            printf("vsAvatar::makeVsWSSpaceball: Unrecognized token '%s'\n",
                token);
    }
    
    // Make sure the window was specified
    if (!wsys)
    {
        printf("vsAvatar::makeVsWSSpaceball: No window specified\n");
        return NULL;
    }

    // Create and return the object
    return (new vsWSSpaceball(wsys, btnCount));
}
#endif

// ------------------------------------------------------------------------
// Protected function
// Creates a vsPinchGloveBox from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsPinchGloveBox()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    char portDev[32];
    int baud = 9600;

    // Initialize the port device name
    portDev[0] = 0;

    // Get the settings for this object
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "port"))
        {
            // Set the serial port
            sscanf(cfgLine, "%*s %s", portDev);
            getSerialPortName(portDev);
        }
        else if (!strcmp(token, "baud"))
        {
            // Set the baud rate
            sscanf(cfgLine, "%*s %d", &baud);
        }
        else
            printf("vsAvatar::makeVsPinchGloveBox: Unrecognized token '%s'\n",
                token);
    }
    
    // Make sure the serial port is properly set
    if (portDev[0] == 0)
    {
        printf("vsAvatar::makeVsPinchGloveBox: No port specified\n");
        return NULL;
    }
    
    // Create and return the object
    return (new vsPinchGloveBox(portDev, baud));
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsCyberGloveBox from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsCyberGloveBox()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    char portDev[32];
    int baud = 9600;
    int numSensors = 0;

    // Initialize the port device name
    portDev[0] = 0;

    // Read all the parameters for this object
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "port"))
        {
            // Set the serial port
            sscanf(cfgLine, "%*s %s", portDev);
            getSerialPortName(portDev);
        }
        else if (!strcmp(token, "baud"))
        {
            // Set the baud rate
            sscanf(cfgLine, "%*s %d", &baud);
        }
        else if (!strcmp(token, "sensors"))
        {
            // Set the number of sensors in the glove
            sscanf(cfgLine, "%*s %d", &numSensors);
        }
        else
            printf("vsAvatar::makeVsCyberGloveBox: Unrecognized token '%s'\n",
                token);
    }
    
    // Make sure the serial port is properly set
    if (portDev[0] == 0)
    {
        printf("vsAvatar::makeVsCyberGloveBox: No port specified\n");
        return NULL;
    }
    
    // Create and return the object
    return (new vsCyberGloveBox(portDev, baud, numSensors));
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsButtonAxis from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsButtonAxis()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsInputButton *positiveButton = NULL;
    vsInputButton *negativeButton = NULL;
    vsInputButton *centerButton = NULL;
    double axisMin = 0;
    double axisMax = 0;
    double positiveSpeed = -1;
    double negativeSpeed = -1;
    double centerSpeed = -1;
    double idleSpeed = -1;
    vsButtonAxis *result;

    vsIODevice *inputDev;
    char objName[256];
    int objNum;

    // Read all the parameters for this object
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "positiveButton"))
        {
            // Set the positive button
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);

            // Find the specified object and get the specified button from it
            inputDev = (vsIODevice *)(findObject(objName));
            if (inputDev)
                positiveButton = inputDev->getButton(objNum);
        }
        else if (!strcmp(token, "negativeButton"))
        {
            // Set the negative button
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);

            // Find the specified object and get the specified button from it
            inputDev = (vsIODevice *)(findObject(objName));
            if (inputDev)
                negativeButton = inputDev->getButton(objNum);
        }
        else if (!strcmp(token, "centerButton"))
        {
            // Set the center button
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);

            // Find the specified object and get the specified button from it
            inputDev = (vsIODevice *)(findObject(objName));
            if (inputDev)
                centerButton = inputDev->getButton(objNum);
        }
        else if (!strcmp(token, "axisMin"))
        {
            // Set the minimum value of the axis
            sscanf(cfgLine, "%*s %lf", &axisMin);
        }
        else if (!strcmp(token, "axisMax"))
        {
            // Set the maximum value of the axis
            sscanf(cfgLine, "%*s %lf", &axisMax);
        }
        else if (!strcmp(token, "positiveSpeed"))
        {
            // Set the positive button speed
            sscanf(cfgLine, "%*s %lf", &positiveSpeed);
        }
        else if (!strcmp(token, "negativeSpeed"))
        {
            // Set the negative button speed
            sscanf(cfgLine, "%*s %lf", &negativeSpeed);
        }
        else if (!strcmp(token, "centerSpeed"))
        {
            // Set the center button speed
            sscanf(cfgLine, "%*s %lf", &centerSpeed);
        }
        else if (!strcmp(token, "idleSpeed"))
        {
            // Set the idle speed
            sscanf(cfgLine, "%*s %lf", &idleSpeed);
        }
        else
            printf("vsAvatar::makeVsButtonAxis: Unrecognized token '%s'\n",
                token);
    }
    
    // Force the axis minimum and maximum values to be valid
    if (axisMin >= axisMax)
    {
        axisMin = VS_AXIS_DEFAULT_MIN;
        axisMax = VS_AXIS_DEFAULT_MAX;
    }

    // Create and return the object
    result = new vsButtonAxis(positiveButton, negativeButton, centerButton,
        axisMin, axisMax);
    result->setPositiveButtonSpeed(positiveSpeed);
    result->setNegativeButtonSpeed(negativeSpeed);
    result->setCenterButtonSpeed(centerSpeed);
    result->setIdleSpeed(idleSpeed);

    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsKinematics from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsKinematics()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsComponent *root;
    vsComponent *geom = NULL;
    int lineLen;
    int inertia = -1;
    atVector massCenter(0.0, 0.0, 0.0);
    atVector startPos(0.0, 0.0, 0.0);
    atQuat startOrient(0.0, 0.0, 0.0, 1.0);
    char geoObjectName[256], nodeName[256];
    vsKinematics *result;
    double a, b, c;

    // Read all the parameters for this object
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "geometry"))
        {
            // Set the vsComponent that this kinematics object will control

            // Read in the name(s) of the object to find
            lineLen = sscanf(cfgLine, "%*s %s %s", geoObjectName, nodeName);

            // Find the vsComponent in the avatar's scene graph
            if (lineLen < 2)
            {
                // Search directly for the specified node
                geom = (vsComponent *)(findObject(geoObjectName));
            }
            else
            {
                // Search for the first node, and then search under that
                // one for the second node
                root = (vsComponent *)(findObject(geoObjectName));
                if (root)
                    geom = (vsComponent *)(root->findNodeByName(nodeName));
            }

        }
        else if (!strcmp(token, "inertia"))
        {
            // Set whether or not to enable inertia in the vsKinematics
            sscanf(cfgLine, "%*s %d", &inertia);
        }
        else if (!strcmp(token, "center"))
        {
            // Set the center of mass for the vsKinematics
            sscanf(cfgLine, "%*s %lf %lf %lf", &a, &b, &c);
            massCenter.set(a, b, c);
        }
        else if (!strcmp(token, "position"))
        {
            // Set the initial position of the kinematics
            sscanf(cfgLine, "%*s %lf %lf %lf", &a, &b, &c);
            startPos.set(a, b, c);
        }
        else if (!strcmp(token, "orientation"))
        {
            // Set the initial orientation of the kinematics
            sscanf(cfgLine, "%*s %lf %lf %lf", &a, &b, &c);
            startOrient.setEulerRotation(AT_EULER_ANGLES_ZXY_R, a, b, c);
        }
        else
            printf("vsAvatar::makeVsKinematics: Unrecognized token '%s'\n",
                token);
    }
    
    // Make sure a vsComponent is specified
    if (!geom)
    {
        printf("vsAvatar::makeVsKinematics: Target node not specified\n");
        return NULL;
    }

    // Construct the vsKinematics object
    result = new vsKinematics(geom);

    // Enable/disable inertia if configured to do so
    if (inertia == 1)
        result->enableInertia();
    else if (inertia == 0)
        result->disableInertia();

    // Set the center of mass, position, and orientation as well
    result->setCenterOfMass(massCenter);
    result->setPosition(startPos);
    result->setOrientation(startOrient);
    
    // Return the created vsKinematics object
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vs3TrackerArm from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVs3TrackerArm()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsTrackingSystem *tsys;
    vsMotionTracker *trackers[3];
    vsKinematics *kinematics[3];
    atVector offsets[3];
    atQuat preRotations[3];
    atQuat postRotations[3];
    char objName[256];
    int whichJoint, trackerNum;
    int loop;
    double a, b, c;
    vs3TrackerArm *result;
    
    // Initialize the parameters
    for (loop = 0; loop < 3; loop++)
    {
        trackers[loop] = NULL;
        kinematics[loop] = NULL;
        offsets[loop].set(0.0, 0.0, 0.0);
        preRotations[loop].set(0.0, 0.0, 0.0, 1.0);
        postRotations[loop].set(0.0, 0.0, 0.0, 1.0);
    }

    // Read all the parameter settings
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "tracker"))
        {
            // Set one of the arm trackers
            sscanf(cfgLine, "%*s %d %s %d", &whichJoint, objName,
                &trackerNum);

            // Find the tracking system
            tsys = (vsTrackingSystem *)(findObject(objName));

            // Get the tracker from the tracking system
            if (tsys && (whichJoint >= 0) && (whichJoint < 3))
                trackers[whichJoint] = tsys->getTracker(trackerNum);
            else if ((whichJoint < 0) || (whichJoint >= 3))
                printf("vsAvatar::makeVs3TrackerArm (tracker): "
                    "Invalid joint index\n");
        }
        else if (!strcmp(token, "kinematics"))
        {
            // Set the kinematics object for one of the joints
            sscanf(cfgLine, "%*s %d %s", &whichJoint, objName);
            kinematics[whichJoint] = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "offset"))
        {
            // Set the tracker offset for one of the joints
            sscanf(cfgLine, "%*s %d %lf %lf %lf", &whichJoint, &a, &b, &c);
            offsets[whichJoint].set(a, b, c);
        }
        else if (!strcmp(token, "preRotate"))
        {
            // Set the pre-rotation for one of the joints
            sscanf(cfgLine, "%*s %d %lf %lf %lf", &whichJoint, &a, &b, &c);
            preRotations[whichJoint].setEulerRotation(AT_EULER_ANGLES_ZXY_R,
                a, b, c);
        }
        else if (!strcmp(token, "postRotate"))
        {
            // Set the post-rotation for one of the joints
            sscanf(cfgLine, "%*s %d %lf %lf %lf", &whichJoint, &a, &b, &c);
            postRotations[whichJoint].setEulerRotation(AT_EULER_ANGLES_ZXY_R,
                a, b, c);
        }
        else
            printf("vsAvatar::makeVs3TrackerArm: Unrecognized token '%s'\n",
                token);
    }
    
    // Make sure three trackers and three kinematics were specified
    for (loop = 0; loop < 3; loop++)
    {
        if (!trackers[loop])
        {
            printf("vsAvatar::makeVs3TrackerArm: Tracker %d not specified\n",
                loop);
            return NULL;
        }
        if (!kinematics[loop])
        {
            printf("vsAvatar::makeVs3TrackerArm: Kinematics object %d not "
                "specified\n", loop);
            return NULL;
        }
    }

    // Create the motion model
    result = new vs3TrackerArm(trackers[0], kinematics[0], trackers[1],
        kinematics[1], trackers[2], kinematics[2]);

    // Apply the tracker offsets
    result->setShoulderOffset(offsets[0]);
    result->setElbowOffset(offsets[1]);
    result->setWristOffset(offsets[2]);

    // Apply the rotation offsets   
    result->setShoulderPreRot(preRotations[0]);
    result->setShoulderPostRot(postRotations[0]);
    result->setElbowPreRot(preRotations[1]);
    result->setElbowPostRot(postRotations[1]);
    result->setWristPreRot(preRotations[2]);
    result->setWristPostRot(postRotations[2]);

    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsAxisRotation from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsAxisRotation()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsKinematics *kinematics = NULL;
    char objName[256];
    int objNum;
    double headingWidth = VS_AR_DEFAULT_HEADING_WIDTH;
    double pitchWidth = VS_AR_DEFAULT_PITCH_WIDTH;
    double headingSpeed = VS_AR_DEFAULT_HEADING_SPEED;
    double pitchSpeed = VS_AR_DEFAULT_PITCH_SPEED;
    atQuat defaultOrient = atQuat(0.0, 0.0, 0.0, 1.0);
    double heading, pitch, roll;
    vsAxisRotation *result;
    vsIODevice *inputDev;
    vsInputAxis *headingAxis = NULL;
    vsInputAxis *pitchAxis = NULL;
    vsInputButton *resetBtn = NULL;
    
    // Read the parameters for this object
    while (lineType != VS_AVT_LINE_END)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "kinematics"))
        {
            // Set the kinematics object
            sscanf(cfgLine, "%*s %s", objName);
            kinematics = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "headingAxis"))
        {
            // Set the heading axis
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);

            // Find the specified object and get the specified axis
            // from it
            inputDev = (vsIODevice *)(findObject(objName));
            if (inputDev)
                headingAxis = inputDev->getAxis(objNum);
        }
        else if (!strcmp(token, "pitchAxis"))
        {
            // Set the pitch axis
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);

            // Find the specified object and get the specified axis
            // from it
            inputDev = (vsIODevice *)(findObject(objName));
            if (inputDev)
                pitchAxis = inputDev->getAxis(objNum);
        }
        else if (!strcmp(token, "resetButton"))
        {
            // Set the reset button
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);

            // Find the specified object and get the specified button
            // from it
            inputDev = (vsIODevice *)(findObject(objName));
            if (inputDev)
                resetBtn = inputDev->getButton(objNum);
        }
        else if (!strcmp(token, "headingWidth"))
        {
            // Set the width of rotation for the heading
            sscanf(cfgLine, "%*s %lf", &headingWidth);
        }
        else if (!strcmp(token, "pitchWidth"))
        {
            // Set the width of rotation for the pitch
            sscanf(cfgLine, "%*s %lf", &pitchWidth);
        }
        else if (!strcmp(token, "headingSpeed"))
        {
            // Set the rotation speed for the heading
            sscanf(cfgLine, "%*s %lf", &headingSpeed);
        }
        else if (!strcmp(token, "pitchSpeed"))
        {
            // Set the rotation speed for the pitch
            sscanf(cfgLine, "%*s %lf", &pitchSpeed);
        }
        else if (!strcmp(token, "resetOrientation"))
        {
            // Get the default orientation in Euler angle form
            sscanf(cfgLine, "%*s %lf %lf %lf\n", &heading, &pitch, &roll);

            // Set the default orientation quaternion using the angles
            defaultOrient.setEulerRotation(AT_EULER_ANGLES_ZXY_R, heading, 
                pitch, roll);
        }
        else
            printf("vsAvatar::makeVsAxisRotation: Unrecognized token '%s'\n",
                token);
    }
    
    // Make sure a kinematics object is specified
    if (!kinematics)
    {
        printf("vsAvatar::makeVsAxisRotation: Kinematics object not "
            "specified\n");
        return NULL;
    }
    
    if ((headingAxis || pitchAxis) && (resetBtn))
    {
        // Create a vsAxisRotation with a reset button enabled
        result = new vsAxisRotation(headingAxis, pitchAxis, resetBtn,
            kinematics);
    }
    else if (headingAxis || pitchAxis)
    {
        // Create a vsAxisRotation with the given heading and pitch axes
        result = new vsAxisRotation(headingAxis, pitchAxis, kinematics);
    }
    else
    {
        // Need to have at least one of the axes for a useful object
        printf("vsAvatar::makeVsAxisRotation: No heading or pitch axis "
            "specified\n");
        return NULL;
    }

    // Set the remaining parameters
    result->setHeadingWidth(headingWidth);
    result->setPitchWidth(pitchWidth);
    result->setHeadingSpeed(headingSpeed);
    result->setPitchSpeed(pitchSpeed);
    result->setStartingOrientation(defaultOrient);

    // Center the rotations to start with
    result->center();

    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsCollision from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsCollision()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsKinematics *kinematics = NULL;
    unsigned int isectMask = 0xFFFFFFFF;
    int cmode = VS_COLLISION_MODE_STOP;
    double margin = VS_COLLISION_DEFAULT_MARGIN;
    char objName[256], strValue[256];
    vsCollision *result;
    double x, y, z;
    int loop;
    int pointIdx;
    atArray pointArray;
    atVector *point;

    // Read the parameters for this object
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "kinematics"))
        {
            // Set the kinematics that will be collided
            sscanf(cfgLine, "%*s %s", objName);
            kinematics = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "point"))
        {
            // Specify one of the "hot points" for collision detection
            sscanf(cfgLine, "%*s %d %lf %lf %lf", &pointIdx, &x, &y, &z);
            if ((pointIdx >= 0) && (pointIdx < VS_COLLISION_POINTS_MAX))
            {
                // See if there's already a point at this index, and delete
                // it if so
                point = (atVector *) pointArray.getEntry(pointIdx);
                if (point != NULL)
                    delete point;

                // Set the new point at this index
                pointArray.setEntry(pointIdx, new atVector(x, y, z));
            }
            else
                printf("vsAvatar::makeVsCollision (point): "
                    "Point index out of bounds\n");
        }
        else if (!strcmp(token, "intersectMask"))
        {
            // Specify the intersect mask for the intersection tests
            sscanf(cfgLine, "%*s %x", &isectMask);
        }
        else if (!strcmp(token, "mode"))
        {
            // Specify the mode, that is, how collisions will be handled
            sscanf(cfgLine, "%*s %s", strValue);
            if (!strcmp(strValue, "VS_COLLISION_MODE_STOP"))
            {
                // All collisions will cause the kinematics to stop
                // completely
                cmode = VS_COLLISION_MODE_STOP;
            }
            else if (!strcmp(strValue, "VS_COLLISION_MODE_SLIDE"))
            {
                // Oblique collisions will cause the object to slide
                // along the collided surface
                cmode = VS_COLLISION_MODE_SLIDE;
            }
            else if (!strcmp(strValue, "VS_COLLISION_MODE_BOUNCE"))
            {
                // Collisions will result in the object bouncing off of
                // the surface
                cmode = VS_COLLISION_MODE_BOUNCE;
            }
            else
                printf("vsAvatar::makeVsCollision (mode): "
                    "Unrecognized mode constant '%s'\n", strValue);
        }
        else if (!strcmp(token, "margin"))
        {
            // Set the collision margin (how close the object has to be
            // to a surface to trigger a collision)
            sscanf(cfgLine, "%*s %lf", &margin);
        }
        else
            printf("vsAvatar::makeVsCollision: Unrecognized token '%s'\n",
                token);
    }
    
    // Make sure we have a scene and that a kinematics object was specified
    if (!kinematics)
    {
        printf("vsAvatar::makeVsCollision: Kinematics object not specified\n");
        return NULL;
    }
    if (!masterScene)
    {
        printf("vsAvatar::makeVsCollision: Collision object requires a scene "
            "to be specified in the avatar constructor\n");
        return NULL;
    }

    // Create the collision object
    result = new vsCollision(kinematics, masterScene);
    
    // Set up the hot points as specified in the file
    result->setPointCount(pointArray.getNumEntries());
    for (loop = 0; loop < pointArray.getNumEntries(); loop++)
    {
        // Get the point
        point = (atVector *) pointArray.getEntry(loop);
        if (point != NULL)
        {
            result->setPoint(loop, *point);
        }
        else
            result->setPoint(loop, atVector(0.0, 0.0, 0.0));
    }

    // Set the remaining parameters
    result->setCollisionMode(cmode);
    result->setIntersectMask(isectMask);
    result->setMargin(margin);
    
    // Return the created vsCollision object
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsDrivingMotion from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsDrivingMotion()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsKinematics *kinematics = NULL;
    vsMouse *mouse = NULL;
    char objName[256], strValue[256];
    vsDMThrottleMode throttle = VS_DM_DEFAULT_THROTTLE_MODE;
    vsDMSteeringMode steering = VS_DM_DEFAULT_STEERING_MODE;
    double accelRate = VS_DM_DEFAULT_ACCEL_RATE;
    double maxForwardSpeed = VS_DM_DEFAULT_MAX_SPEED;
    double maxReverseSpeed = VS_DM_DEFAULT_MAX_SPEED;
    double steeringRate = VS_DM_DEFAULT_STEER_RATE;
    vsDrivingMotion *result;
    vsInputAxis *steerAxis = NULL;
    vsInputAxis *throttleAxis = NULL;
    vsInputButton *accelBtn = NULL;
    vsInputButton *stopBtn = NULL;
    vsInputButton *decelBtn = NULL;
    vsIODevice *inputDev;
    int objNum;
    
    // Read the parameters for this object
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "kinematics"))
        {
            // Set the kinematics object
            sscanf(cfgLine, "%*s %s", objName);
            kinematics = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "mouse"))
        {
            // Set up a mouse for driving control
            sscanf(cfgLine, "%*s %s", objName);
            mouse = (vsMouse *)(findObject(objName));
        }
        else if (!strcmp(token, "steeringAxis"))
        {
            // Set the steering axis
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);

            // Find the specified object and get the specified axis
            // from it
            inputDev = (vsIODevice *)(findObject(objName));
            if (inputDev)
                steerAxis = inputDev->getAxis(objNum);
        }
        else if (!strcmp(token, "throttleAxis"))
        {
            // Set the throttle axis
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);

            // Find the specified object and get the specified axis
            // from it
            inputDev = (vsIODevice *)(findObject(objName));
            if (inputDev)
                throttleAxis = inputDev->getAxis(objNum);
        }
        else if (!strcmp(token, "accelButton"))
        {
            // Set the accelerate button
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);

            // Find the specified object and get the specified button
            // from it
            inputDev = (vsIODevice *)(findObject(objName));
            if (inputDev)
                accelBtn = inputDev->getButton(objNum);
        }
        else if (!strcmp(token, "stopButton"))
        {
            // Set the stop button
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);

            // Find the specified object and get the specified button
            // from it
            inputDev = (vsIODevice *)(findObject(objName));
            if (inputDev)
                stopBtn = inputDev->getButton(objNum);
        }
        else if (!strcmp(token, "decelButton"))
        {
            // Set the decelerate button
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);

            // Find the specified object and get the specified button
            // from it
            inputDev = (vsIODevice *)(findObject(objName));
            if (inputDev)
                decelBtn = inputDev->getButton(objNum);
        }
        else if (!strcmp(token, "throttleMode"))
        {
            // Set the throttle mode
            sscanf(cfgLine, "%*s %s", strValue);
            if (!strcmp(strValue, "VS_DM_THROTTLE_VELOCITY"))
            {
                // Throttle controls velocity directly
                throttle = VS_DM_THROTTLE_VELOCITY;
            }
            else if (!strcmp(strValue, "VS_DM_THROTTLE_ACCELERATION"))
            {
                // Throttle controls acceleration
                throttle = VS_DM_THROTTLE_ACCELERATION;
            }
            else
                printf("vsAvatar::makeVsDrivingMotion (throttleMode): "
                    "Unrecognized throttle mode constant '%s'\n", strValue);
        }
        else if (!strcmp(token, "accelRate"))
        {
            // Set the acceleration rate
            sscanf(cfgLine, "%*s %lf", &accelRate);
        }
        else if (!strcmp(token, "maxSpeed"))
        {
            // Set both maximum speeds
            sscanf(cfgLine, "%*s %lf", &maxForwardSpeed);
            maxReverseSpeed = maxForwardSpeed;
        }
        else if (!strcmp(token, "maxForwardSpeed"))
        {
            // Set the maximum forward speed
            sscanf(cfgLine, "%*s %lf", &maxForwardSpeed);
        }
        else if (!strcmp(token, "maxReverseSpeed"))
        {
            // Set the maximum reverse speed
            sscanf(cfgLine, "%*s %lf", &maxReverseSpeed);
        }
        else if (!strcmp(token, "steeringMode"))
        {
            // Set the steering mode
            sscanf(cfgLine, "%*s %s", strValue);
            if (!strcmp(strValue, "VS_DM_STEER_RELATIVE"))
            {
                // Steering rate is relative to current speed
                steering = VS_DM_STEER_RELATIVE;
            }
            else if (!strcmp(strValue, "VS_DM_STEER_ABSOLUTE"))
            {
                // Steering rate is constant
                steering = VS_DM_STEER_ABSOLUTE;
            }
            else
                printf("vsAvatar::makeVsDrivingMotion (steeringMode): "
                    "Unrecognized steering mode constant '%s'\n", strValue);
        }
        else if (!strcmp(token, "steeringRate"))
        {
            // Set the steering rate
            sscanf(cfgLine, "%*s %lf", &steeringRate);
        }
        else
            printf("vsAvatar::makeVsDrivingMotion: Unrecognized token '%s'\n",
                token);
    }
    
    // Make sure a kinematics object is specified
    if (!kinematics)
    {
        printf("vsAvatar::makeVsDrivingMotion: Kinematics object not "
            "specified\n");
        return NULL;
    }
    
    // Call the appropriate constructor, based on the controls configured
    if (mouse)
    {
        // Create a mouse-controlled vsDriving motion
        result = new vsDrivingMotion(mouse, kinematics);
    }
    else if (steerAxis && throttleAxis)
    {
        // Create a vsDrivingMotion with a throttle control for speed
        result = new vsDrivingMotion(steerAxis, throttleAxis, kinematics);
    }
    else if (steerAxis)
    {
        // Create a vsDrivingMotion with button controls for speed
        result = new vsDrivingMotion(steerAxis, accelBtn, decelBtn, stopBtn,
            kinematics);
    }
    else
    {
        printf("vsAvatar::makeVsDrivingMotion: No mouse or steering axis "
            "specified\n");
        return NULL;
    }

    // Set the remaining parameters
    result->setThrottleMode(throttle);
    result->setAccelerationRate(accelRate);
    result->setMaxForwardSpeed(maxForwardSpeed);
    result->setMaxReverseSpeed(maxReverseSpeed);
    result->setSteeringMode(steering);
    result->setSteeringRate(steeringRate);
    
    // Return the created vsDrivingMotion object
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsFlyingMotion from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsFlyingMotion()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsKinematics *kinematics = NULL;
    char objName[256], strValue[256];
    vsMouse *mouse = NULL;
    vsFlyingAxisMode headingMode = VS_FM_DEFAULT_HEADING_MODE;
    vsFlyingAxisMode pitchMode = VS_FM_DEFAULT_PITCH_MODE;
    vsFlyingAxisMode throttleMode = VS_FM_DEFAULT_THROTTLE_MODE;
    double accelRate = VS_FM_DEFAULT_ACCEL_RATE;
    double turnRate = VS_FM_DEFAULT_TURNING_RATE;
    double maxSpeed = VS_FM_DEFAULT_MAX_SPEED;
    vsFlyingMotion *result;
    vsInputAxis *headingAxis = NULL;
    vsInputAxis *pitchAxis = NULL;
    vsInputAxis *throttleAxis = NULL;
    vsInputButton *accelBtn = NULL;
    vsInputButton *stopBtn = NULL;
    vsInputButton *decelBtn = NULL;
    vsIODevice *inputDev;
    int objNum;

    // Read in the object parameters
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "kinematics"))
        {
            // Set the kinematics
            sscanf(cfgLine, "%*s %s", objName);
            kinematics = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "mouse"))
        {
            // Set up mouse controls for flying
            sscanf(cfgLine, "%*s %s", objName);
            mouse = (vsMouse *)(findObject(objName));
        }
        else if (!strcmp(token, "headingAxis"))
        {
            // Set the heading axis
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);

            // Find the specified input device and extract the specified
            // axis from it
            inputDev = (vsIODevice *)(findObject(objName));
            if (inputDev)
                headingAxis = inputDev->getAxis(objNum);
        }
        else if (!strcmp(token, "pitchAxis"))
        {
            // Set the pitch axis
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);

            // Find the specified input device and extract the specified
            // axis from it
            inputDev = (vsIODevice *)(findObject(objName));
            if (inputDev)
                pitchAxis = inputDev->getAxis(objNum);
        }
        else if (!strcmp(token, "throttleAxis"))
        {
            // Set the throttle axis
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);

            // Find the specified input device and extract the specified
            // axis from it
            inputDev = (vsIODevice *)(findObject(objName));
            if (inputDev)
                throttleAxis = inputDev->getAxis(objNum);
        }
        else if (!strcmp(token, "accelButton"))
        {
            // Set the accelerate button
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);

            // Find the specified input device and extract the specified
            // button from it
            inputDev = (vsIODevice *)(findObject(objName));
            if (inputDev)
                accelBtn = inputDev->getButton(objNum);
        }
        else if (!strcmp(token, "stopButton"))
        {
            // Set the stop button
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);

            // Find the specified input device and extract the specified
            // button from it
            inputDev = (vsIODevice *)(findObject(objName));
            if (inputDev)
                stopBtn = inputDev->getButton(objNum);
        }
        else if (!strcmp(token, "decelButton"))
        {
            // Set the decelerate button
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);

            // Find the specified input device and extract the specified
            // button from it
            inputDev = (vsIODevice *)(findObject(objName));
            if (inputDev)
                decelBtn = inputDev->getButton(objNum);
        }
        else if (!strcmp(token, "headingMode"))
        {
            // Set the heading axis mode
            sscanf(cfgLine, "%*s %s", strValue);
            if (!strcmp(strValue, "VS_FM_MODE_INCREMENTAL"))
                headingMode = VS_FM_MODE_INCREMENTAL;
            else if (!strcmp(strValue, "VS_FM_MODE_ABSOLUTE"))
                headingMode = VS_FM_MODE_ABSOLUTE;
            else if (!strcmp(strValue, "VS_FM_MODE_NO_CHANGE"))
                headingMode = VS_FM_MODE_NO_CHANGE;
            else
                printf("vsAvatar::makeVsFlyingMotion (headingMode): "
                    "Unrecognized mode constant '%s'\n", strValue);
        }
        else if (!strcmp(token, "pitchMode"))
        {
            // Set the pitch axis mode
            sscanf(cfgLine, "%*s %s", strValue);
            if (!strcmp(strValue, "VS_FM_MODE_INCREMENTAL"))
                pitchMode = VS_FM_MODE_INCREMENTAL;
            else if (!strcmp(strValue, "VS_FM_MODE_ABSOLUTE"))
                pitchMode = VS_FM_MODE_ABSOLUTE;
            else if (!strcmp(strValue, "VS_FM_MODE_NO_CHANGE"))
                pitchMode = VS_FM_MODE_NO_CHANGE;
            else
                printf("vsAvatar::makeVsFlyingMotion (pitchMode): "
                    "Unrecognized mode constant '%s'\n", strValue);
        }
        else if (!strcmp(token, "throttleMode"))
        {
            // Set the throttle mode
            sscanf(cfgLine, "%*s %s", strValue);
            if (!strcmp(strValue, "VS_FM_MODE_INCREMENTAL"))
                throttleMode = VS_FM_MODE_INCREMENTAL;
            else if (!strcmp(strValue, "VS_FM_MODE_ABSOLUTE"))
                throttleMode = VS_FM_MODE_ABSOLUTE;
            else if (!strcmp(strValue, "VS_FM_MODE_NO_CHANGE"))
                throttleMode = VS_FM_MODE_NO_CHANGE;
            else
                printf("vsAvatar::makeVsFlyingMotion (throttleMode): "
                    "Unrecognized mode constant '%s'\n", strValue);
        }
        else if (!strcmp(token, "accelRate"))
        {
            // Set the acceleration rate
            sscanf(cfgLine, "%*s %lf", &accelRate);
        }
        else if (!strcmp(token, "turnRate"))
        {
            // Set the turning rate for incremental axes
            sscanf(cfgLine, "%*s %lf", &turnRate);
        }
        else if (!strcmp(token, "maxSpeed"))
        {
            // Set the maximum flying speed
            sscanf(cfgLine, "%*s %lf", &maxSpeed);
        }
        else
            printf("vsAvatar::makeVsFlyingMotion: Unrecognized token '%s'\n",
                token);
    }

    // Make sure a kinematics was specified
    if (!kinematics)
    {
        printf("vsAvatar::makeVsFlyingMotion: Kinematics object not "
            "specified\n");
        return NULL;
    }

    // Construct the vsFlyingMotion based on the controls configured
    if (mouse)
    {
        // Create a mouse-operated vsFlyingMotion
        result = new vsFlyingMotion(mouse, kinematics);
    }
    else if (headingAxis && pitchAxis && throttleAxis)
    {
        // Create a vsFlyingMotion with a throttle
        result = new vsFlyingMotion(headingAxis, pitchAxis, throttleAxis,
            kinematics);
    }
    else if (headingAxis && pitchAxis)
    {
        // Create a vsFlyingMotion with button controls for speed
        result = new vsFlyingMotion(headingAxis, pitchAxis, accelBtn,
            decelBtn, stopBtn, kinematics);
    }
    else
    {
        printf("vsAvatar::makeVsFlyingMotion: No mouse or insufficient "
            "control axes specified\n");
        return NULL;
    }

    // Set the remaining parameters
    result->setAxisModes(headingMode, pitchMode, throttleMode);
    result->setAccelerationRate(accelRate);
    result->setTurningRate(turnRate);
    result->setMaxSpeed(maxSpeed);

    // Return the created vsFlyingMotion object
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsDifferentialTrackedOrientation from data in the 
// configuration file, and returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsDifferentialTrackedOrientation()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsKinematics *kinematics = NULL;
    char objName[256];
    vsTrackingSystem *tsys;
    vsMotionTracker *trackers[2];
    atQuat oriOffset(0.0, 0.0, 0.0, 1.0);
    int whichTracker, trackerNum;
    vsDifferentialTrackedOrientation *result;
    double h, p, r;
    
    // Initialize the tracker pointer values to sentinel values
    trackers[0] = NULL;
    trackers[1] = NULL;

    // Read in the object parameters
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "kinematics"))
        {
            // Set the kinematics object
            sscanf(cfgLine, "%*s %s", objName);
            kinematics = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "tracker"))
        {
            // Set up one of the motion trackers
            sscanf(cfgLine, "%*s %d %s %d", &whichTracker, objName,
                &trackerNum);

            // Find the tracking system object
            tsys = (vsTrackingSystem *)(findObject(objName));

            // Make sure the tracker index specified makes sense
            if (tsys && (whichTracker >= 0) && (whichTracker < 2))
            {
                // Get the tracker from the tracking system
                trackers[whichTracker] = tsys->getTracker(trackerNum);
            }
            else if ((whichTracker < 0) && (whichTracker >= 2))
            {
                printf("vsAvatar::makeVsDifferentialTrackedOrientation:\n");
                printf("Invalid tracker index\n");
            }
        }
        else if (!strcmp(token, "orientationOffset"))
        {
            // Set the orientation offset
            sscanf(cfgLine, "%*s %lf %lf %lf", &h, &p, &r);
            oriOffset.setEulerRotation(AT_EULER_ANGLES_ZXY_R, h, p, r);
        }
        else
        {
            printf("vsAvatar::makeVsDifferentialTrackedOrientation: Unrecognized token '%s'\n", 
                token);
        }
    }
    
    // Make sure we have two trackers and a vsKinematics
    if (!kinematics)
    {
        printf("vsAvatar::makeVsDifferentialTrackedOrientation:\n");
        printf("    Kinematics object not specified\n");
        return NULL;
    }
    if (!(trackers[0]))
    {
        printf("vsAvatar::makeVsDifferentialTrackedOrientation:\n");
        printf("    Reference tracker object not specified\n");
        return NULL;
    }
    if (!(trackers[1]))
    {
        printf("vsAvatar::makeVsDifferentialTrackedOrientation:\n");
        printf("    Differential tracker object not specified\n");
        return NULL;
    }

    // Create the object
    result = new vsDifferentialTrackedOrientation(trackers[0], trackers[1], 
        kinematics);
    
    // Set the orientation offset
    result->setOrientationOffset(oriOffset);
    
    // Return the created vsHeadMotion object
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsPathMotion from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsPathMotion()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    char objName[256];
    vsKinematics *kinematics = NULL;
    char configFile[256];
    vsPathMotion *result;

    configFile[0] = 0;

    // Read all of the parameters for the object
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "kinematics"))
        {
            // Set the kinematics object
            sscanf(cfgLine, "%*s %s", objName);
            kinematics = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "dataFileName"))
        {
            // Set the name of the path configuration file
            sscanf(cfgLine, "%*s %s", configFile);
        }
        else
            printf("vsAvatar::makeVsPathMotion: Unrecognized token '%s'\n",
                token);
    }

    // Make sure we have a kinematics object
    if (!kinematics)
    {
        printf("vsAvatar::makeVsPathMotion: Kinematics object not "
            "specified\n");
        return NULL;
    }

    // Construct the object
    result = new vsPathMotion(kinematics);

    // Configure the object with the data from the configuration file,
    // if specified
    if (configFile[0] != 0)
        result->configureFromFile(configFile);

    // Return the new vsPathMotion object
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsTerrainFollow from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsTerrainFollow()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsKinematics *kinematics = NULL;
    char objName[256];
    unsigned int isectMask = 0xFFFFFFFF;
    atVector offset(0.0, 0.0, 0.0);
    double stepHeight = VS_TFOLLOW_DEFAULT_HEIGHT;
    double x, y, z;
    vsTerrainFollow *result;

    // Read all of the parameters for the object
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "kinematics"))
        {
            // Set the kinematics object
            sscanf(cfgLine, "%*s %s", objName);
            kinematics = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "offset"))
        {
            // Set the terrain following offset from the avatar's base
            // position
            sscanf(cfgLine, "%*s %lf %lf %lf", &x, &y, &z);
            offset.set(x, y, z);
        }
        else if (!strcmp(token, "stepHeight"))
        {
            // Set how high a step the avatar can ascend
            sscanf(cfgLine, "%*s %lf", &stepHeight);
        }
        else if (!strcmp(token, "intersectMask"))
        {
            // Set the intersect mask for terrain following intersection
            // tests
            sscanf(cfgLine, "%*s %x", &isectMask);
        }
        else
            printf("vsAvatar::makeVsTerrainFollow: Unrecognized token '%s'\n",
                token);
    }
    
    // Make sure we have a scene and a kinematics object
    if (!kinematics)
    {
        printf("vsAvatar::makeVsTerrainFollow: Kinematics object not "
            "specified\n");
        return NULL;
    }
    if (!masterScene)
    {
        printf("vsAvatar::makeVsTerrainFollow: Terrain follow object "
            "requires a scene to be specified in the avatar constructor\n");
        return NULL;
    }

    // Construct the object
    result = new vsTerrainFollow(kinematics, masterScene);
    
    // Set the remaining parameters
    result->setBaseOffset(offset);
    result->setStepHeight(stepHeight);
    result->setIntersectMask(isectMask);

    // Return the created vsTerrainFollow object
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsTrackballMotion from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsTrackballMotion()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsKinematics *kinematics = NULL;
    char objName[256];
    vsMouse *mouse = NULL;
    double translate = VS_TBM_DEFAULT_TRANSLATE_CONST;
    double rotate = VS_TBM_DEFAULT_ROTATE_CONST;
    vsTrackballMotion *result;
    vsInputAxis *horizAxis = NULL;
    vsInputAxis *vertiAxis = NULL;
    vsInputButton *xyBtn = NULL;
    vsInputButton *zBtn = NULL;
    vsInputButton *rotBtn = NULL;
    vsIODevice *inputDev;
    int objNum;

    // Read all the parameters for the object
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "kinematics"))
        {
            // Set the kinematics object
            sscanf(cfgLine, "%*s %s", objName);
            kinematics = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "mouse"))
        {
            // Set up mouse control
            sscanf(cfgLine, "%*s %s", objName);
            mouse = (vsMouse *)(findObject(objName));
        }
        else if (!strcmp(token, "horizontalAxis"))
        {
            // Read the name of the input device to find and the number
            // of the axis to use from it
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);

            // Find the specified input device and extract the specified
            // axis from it
            inputDev = (vsIODevice *)(findObject(objName));
            if (inputDev)
                horizAxis = inputDev->getAxis(objNum);
        }
        else if (!strcmp(token, "verticalAxis"))
        {
            // Read the name of the input device to find and the number
            // of the axis to use from it
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);

            // Find the specified input device and extract the specified
            // axis from it
            inputDev = (vsIODevice *)(findObject(objName));
            if (inputDev)
                vertiAxis = inputDev->getAxis(objNum);
        }
        else if (!strcmp(token, "xyButton"))
        {
            // Read the name of the input device to find and the number
            // of the button to use from it
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);

            // Find the specified input device and extract the specified
            // button from it
            inputDev = (vsIODevice *)(findObject(objName));
            if (inputDev)
                xyBtn = inputDev->getButton(objNum);
        }
        else if (!strcmp(token, "zButton"))
        {
            // Read the name of the input device to find and the number
            // of the button to use from it
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);

            // Find the specified input device and extract the specified
            // button from it
            inputDev = (vsIODevice *)(findObject(objName));
            if (inputDev)
                zBtn = inputDev->getButton(objNum);
        }
        else if (!strcmp(token, "rotateButton"))
        {
            // Read the name of the input device to find and the number
            // of the button to use from it
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);

            // Find the specified input device and extract the specified
            // button from it
            inputDev = (vsIODevice *)(findObject(objName));
            if (inputDev)
                rotBtn = inputDev->getButton(objNum);
        }
        else if (!strcmp(token, "translateSpeed"))
        {
            // Set the speed for translation
            sscanf(cfgLine, "%*s %lf", &translate);
        }
        else if (!strcmp(token, "rotateSpeed"))
        {
            // Set the speed for rotation
            sscanf(cfgLine, "%*s %lf", &rotate);
        }
        else
            printf("vsAvatar::makeVsTrackballMotion: Unrecognized token "
                "'%s'\n", token);
    }
    
    // Make sure we have a valid kinematics object
    if (!kinematics)
    {
        printf("vsAvatar::makeVsTrackballMotion: Kinematics object not "
            "specified\n");
        return NULL;
    }

    // Construct the object based on the controls that were configured
    if (mouse)
    {
        // Create a mouse-controlled motion model with default 
        // configuration
        result = new vsTrackballMotion(mouse, kinematics);
    }
    else if (horizAxis && vertiAxis)
    {
        // Create a vsTrackballMotion with the given axis and button
        // configuration
        result = new vsTrackballMotion(horizAxis, vertiAxis, xyBtn, zBtn,
            rotBtn, kinematics);
    }
    else
    {
        printf("vsAvatar::vsTrackballMotion: No mouse or insufficient "
            "control axes specified\n");
        return NULL;
    }

    // Set the remaining parameters
    result->setTranslationConstant(translate);
    result->setRotationConstant(rotate);
    
    // Return the created vsTrackballMotion object
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsTrackedMotion from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsTrackedMotion()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsKinematics *kinematics = NULL;
    char objName[256];
    vsTrackingSystem *tsys;
    vsMotionTracker *tracker = NULL;
    int trackerNum;
    int intValue;
    bool posEnable = true;
    bool oriEnable = true;
    atVector posOffset(0.0, 0.0, 0.0);
    atQuat oriOffset(0.0, 0.0, 0.0, 1.0);
    double a, b, c;
    double posScale = 1.0;
    vsTrackedMotion *result;

    // Read the parameters for the object
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "kinematics"))
        {
            // Set the kinematics object
            sscanf(cfgLine, "%*s %s", objName);
            kinematics = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "tracker"))
        {
            // Set the motion tracker
            sscanf(cfgLine, "%*s %s %d", objName, &trackerNum);
            tsys = (vsTrackingSystem *)(findObject(objName));
            if (tsys)
                tracker = tsys->getTracker(trackerNum);
        }
        else if (!strcmp(token, "positionEnable"))
        {
            // Set whether position tracking is enabled or not
            sscanf(cfgLine, "%*s %d", &intValue);

            if (intValue == 0)
                posEnable = false;
            else
                posEnable = true;
        }
        else if (!strcmp(token, "orientationEnable"))
        {
            // Set whether orientation tracking is enabled or not
            sscanf(cfgLine, "%*s %d", &intValue);

            if (intValue == 0)
                oriEnable = false;
            else
                oriEnable = true;
        }
        else if (!strcmp(token, "positionOffset"))
        {
            // Set the position offset
            sscanf(cfgLine, "%*s %lf %lf %lf", &a, &b, &c);
            posOffset.set(a, b, c);
        }
        else if (!strcmp(token, "orientationOffset"))
        {
            // Set the orientation offset
            sscanf(cfgLine, "%*s %lf %lf %lf", &a, &b, &c);
            oriOffset.setEulerRotation(AT_EULER_ANGLES_ZXY_R, a, b, c);
        }
        else if (!strcmp(token, "positionScale"))
        {
            // Set the position tracking scale factor
            sscanf(cfgLine, "%*s %lf", &posScale);
        }
        else
            printf("vsAvatar::makeVsTrackedMotion: Unrecognized token "
                "'%s'\n", token);
    }
    
    // Make sure we have a valid kinematics and motion tracker
    if (!kinematics)
    {
        printf("vsAvatar::makeVsTrackedMotion: Kinematics object not "
            "specified\n");
        return NULL;
    }
    if (!tracker)
    {
        printf("vsAvatar::makeVsTrackedMotion: Tracker not specified\n");
        return NULL;
    }

    // Create the motion model
    result = new vsTrackedMotion(tracker, kinematics);
    
    // Set the remaining parameters
    if (posEnable)
        result->enablePositionTracking();
    else
        result->disablePositionTracking();
    if (oriEnable)
        result->enableOrientationTracking();
    else
        result->disableOrientationTracking();
    result->setPositionOffset(posOffset);
    result->setOrientationOffset(oriOffset);
    result->setPositionScale(posScale);
    
    // Return the created vsTrackedMotion object
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsVestSystem object that will communicate with IST's vest
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsVestSystem()
{
    char configLine[256];
    char command[256];
    int port = 1;
    vsVestSystem * vest = NULL;

    while( readCfgLine( configLine )==VS_AVT_LINE_PARAM )
    {
        sscanf( configLine, "%s", command );

        if( !strcmp( command, "port" ) )
            sscanf( configLine, "%*s %d", &port );
        else
            fprintf(stderr,"vsAvatar::makeVsVestSystem unknown command: %s\n",
                    command );
    }

    return (vsObject *)(new vsVestSystem(port));
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsWalkArticulation from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsWalkArticulation()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsKinematics *kinematics = NULL;
    char objName[256], jointName[256];
    char dataFilename[256];
    vsKinematics *jointKins[VS_WALK_ARTIC_JOINT_COUNT];
    int loop;
    int jointIdx;
    vsWalkArticulation *result;
    
    // Initialize parameters
    dataFilename[0] = 0;
    for (loop = 0; loop < VS_WALK_ARTIC_JOINT_COUNT; loop++)
        jointKins[loop] = NULL;

    // Read the parameters for the object
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "kinematics"))
        {
            // Set the main kinematics object
            sscanf(cfgLine, "%*s %s", objName);
            kinematics = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "datafile"))
        {
            // Set the filename for the joint angle data
            sscanf(cfgLine, "%*s %s", dataFilename);
        }
        else if (!strcmp(token, "jointKinematics"))
        {
            // Set the kinematics object for the joints
            sscanf(cfgLine, "%*s %s %s", jointName, objName);
            if (!strcmp(jointName, "VS_WALK_ARTIC_LEFT_HIP"))
                jointIdx = VS_WALK_ARTIC_LEFT_HIP;
            else if (!strcmp(jointName, "VS_WALK_ARTIC_LEFT_KNEE"))
                jointIdx = VS_WALK_ARTIC_LEFT_KNEE;
            else if (!strcmp(jointName, "VS_WALK_ARTIC_LEFT_ANKLE"))
                jointIdx = VS_WALK_ARTIC_LEFT_ANKLE;
            else if (!strcmp(jointName, "VS_WALK_ARTIC_RIGHT_HIP"))
                jointIdx = VS_WALK_ARTIC_RIGHT_HIP;
            else if (!strcmp(jointName, "VS_WALK_ARTIC_RIGHT_KNEE"))
                jointIdx = VS_WALK_ARTIC_RIGHT_KNEE;
            else if (!strcmp(jointName, "VS_WALK_ARTIC_RIGHT_ANKLE"))
                jointIdx = VS_WALK_ARTIC_RIGHT_ANKLE;
            else
                jointIdx = -1;
            if (jointIdx != -1)
                jointKins[jointIdx] = (vsKinematics *)(findObject(objName));
            else
                printf("vsAvatar::makeVsWalkArticulation (jointKinematics): "
                    "Unrecognized joint constant '%s'\n", jointName);
        }
        else
            printf("vsAvatar::makeVsWalkArticulation: Unrecognized token "
                "'%s'\n", token);
    }
    
    // Make sure we have all the data we need
    if (!kinematics)
    {
        printf("vsAvatar::makeVsWalkArticulation: Kinematics object not "
            "specified\n");
        return NULL;
    }
    if (!strlen(dataFilename))
    {
        printf("vsAvatar::makeVsWalkArticulation: Articulation data file "
            "not specified\n");
        return NULL;
    }

    // Create the vsWalkArticulation object using the vsKinematics
    // object and the name of the articulation data file
    result = new vsWalkArticulation(kinematics, dataFilename);
    
    // For each joint that was specified, pass that joint's vsKinematics
    // object to the walk articulation object
    for (loop = 0; loop < VS_WALK_ARTIC_JOINT_COUNT; loop++)
        if (jointKins[loop])
            result->setJointKinematics(loop, jointKins[loop]);
    
    // Return the created vsWalkArticulation object
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsWalkInPlace from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsWalkInPlace()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsKinematics *kinematics = NULL;
    char objName[256];
    vsTrackingSystem *tsys;
    vsMotionTracker *trackers[3];
    int whichJoint, trackerNum;
    int loop;
    int intValue;
    bool forwardEnable = true;
    bool backEnable = true;
    bool sideEnable = true;
    double forwardSpeed = VS_WIP_DEFAULT_FWD_SPD;
    double backSpeed = VS_WIP_DEFAULT_BCK_SPD;
    double sideSpeed = VS_WIP_DEFAULT_SS_SPD;
    double forwardThresh = VS_WIP_DEFAULT_FWD_THRESH;
    double backThresh = VS_WIP_DEFAULT_BCK_THRESH;
    double sideThresh = VS_WIP_DEFAULT_SS_THRESH;
    double moveAllow = VS_WIP_DEFAULT_ALLOWANCE;
    bool moveLimitEnable = true;
    vsWalkInPlace *result;
    
    // Initialize the motion trackers
    for (loop = 0; loop < 3; loop++)
        trackers[loop] = NULL;

    // Read in parameters for the object
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "kinematics"))
        {
            // Set the kinematics object
            sscanf(cfgLine, "%*s %s", objName);
            kinematics = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "tracker"))
        {
            // Set the motion trackers
            sscanf(cfgLine, "%*s %d %s %d", &whichJoint, objName,
                &trackerNum);
            tsys = (vsTrackingSystem *)(findObject(objName));
            if (tsys && (whichJoint >= 0) && (whichJoint < 3))
                trackers[whichJoint] = tsys->getTracker(trackerNum);
        }
        else if (!strcmp(token, "forwardEnable"))
        {
            // Set whether forward motion is enabled
            sscanf(cfgLine, "%*s %d", &intValue);

            if (intValue == 0)
                forwardEnable = false;
            else
                forwardEnable = true;
        }
        else if (!strcmp(token, "backwardEnable"))
        {
            // Set whether backward motion is enabled
            sscanf(cfgLine, "%*s %d", &intValue);

            if (intValue == 0)
                backEnable = false;
            else
                backEnable = true;
        }
        else if (!strcmp(token, "sidestepEnable"))
        {
            // Set whether sidestep motion is enabled
            sscanf(cfgLine, "%*s %d", &intValue);

            if (intValue == 0)
                sideEnable = false;
            else
                sideEnable = true;
        }
        else if (!strcmp(token, "forwardSpeed"))
        {
            // Set the speed of forward motion
            sscanf(cfgLine, "%*s %lf", &forwardSpeed);
        }
        else if (!strcmp(token, "backwardSpeed"))
        {
            // Set the speed of backward motion
            sscanf(cfgLine, "%*s %lf", &backSpeed);
        }
        else if (!strcmp(token, "sidestepSpeed"))
        {
            // Set the speed of sidestep motion
            sscanf(cfgLine, "%*s %lf", &sideSpeed);
        }
        else if (!strcmp(token, "forwardThreshold"))
        {
            // Set the amount of tracker separation distance 
            // necessary for forward motion
            sscanf(cfgLine, "%*s %lf", &forwardThresh);
        }
        else if (!strcmp(token, "backwardThreshold"))
        {
            // Set the amount of tracker separation distance 
            // necessary for backward motion
            sscanf(cfgLine, "%*s %lf", &backThresh);
        }
        else if (!strcmp(token, "sidestepThreshold"))
        {
            // Set the amount of tracker separation distance 
            // necessary for sidestep motion
            sscanf(cfgLine, "%*s %lf", &sideThresh);
        }
        else if (!strcmp(token, "moveAllowance"))
        {
            // Set the maximum distance the user can move
            // before being required to take another step
            sscanf(cfgLine, "%*s %lf", &moveAllow);
        }
        else if (!strcmp(token, "moveLimitEnable"))
        {
            // Enable/disable the movement allowance limit
            sscanf(cfgLine, "%*s %d", &intValue);

            if (intValue == 0)
                moveLimitEnable = false;
            else
                moveLimitEnable = true;
        }
        else
            printf("vsAvatar::makeVsWalkInPlace: Unrecognized token '%s'\n",
                token);
    }
    
    // Make sure we have a kinematics object and three motion trackers
    if (!kinematics)
    {
        printf("vsAvatar::makeVsWalkInPlace: Kinematics object %d not "
            "specified\n", loop);
        return NULL;
    }
    for (loop = 0; loop < 3; loop++)
    {
        if (!(trackers[loop]))
        {
            printf("vsAvatar::makeVsWalkInPlace: Tracker %d not specified\n",
                loop);
            return NULL;
        }
    }

    // Create the motion model
    result = new vsWalkInPlace(trackers[0], trackers[1], trackers[2],
        kinematics);

    // Set the movement allowances
    if (forwardEnable)
        result->enableForward();
    else
        result->disableForward();
    if (backEnable)
        result->enableBackward();
    else
        result->disableBackward();
    if (sideEnable)
        result->enableSideStep();
    else
        result->disableSideStep();

    // Set the remaining parameters
    result->setForwardSpeed(forwardSpeed);
    result->setBackwardSpeed(backSpeed);
    result->setSideStepSpeed(sideSpeed);
    result->setForwardThreshold(forwardThresh);
    result->setBackwardThreshold(backThresh);
    result->setSideStepThreshold(sideThresh);
    result->setMovementAllowance(moveAllow);
    if (moveLimitEnable)
        result->enableMovementLimit();
    else
        result->disableMovementLimit();
    
    // Return the created vsWalkInPlace object
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsFPSMotion from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
vsObject *vsAvatar::makeVsFPSMotion()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsKinematics *rootKinematics = NULL;
    vsKinematics *viewKinematics = NULL;
    char objName[256];
    vsInputAxis *forwardAxis = NULL;
    vsInputAxis *strafeAxis = NULL;
    vsInputAxis *headingAxis = NULL;
    vsInputAxis *pitchAxis = NULL;
    vsIODevice *ioDev;
    int axisNum;
    vsMouse *mouse = NULL;
    double maxForwardSpeed = VS_FPSM_DEFAULT_MAX_SPEED;
    double maxReverseSpeed = VS_FPSM_DEFAULT_MAX_SPEED;
    double maxStrafeSpeed = VS_FPSM_DEFAULT_MAX_SPEED;
    double headingRate = VS_FPSM_DEFAULT_HEADING_RATE;
    double pitchRate = VS_FPSM_DEFAULT_PITCH_RATE;
    double minPitch = -VS_FPSM_DEFAULT_PITCH_LIMIT;
    double maxPitch = VS_FPSM_DEFAULT_PITCH_LIMIT;
    char axisMode[256];
    vsFPSMAxisMode headingMode, pitchMode;
    vsFPSMotion *result;
    
    // Read in parameters for the object
    while (lineType != VS_AVT_LINE_END)
    {
        // Get the next line from the config file
        lineType = readCfgLine(cfgLine);
        if (lineType != VS_AVT_LINE_PARAM)
            continue;

        // Read the first token on the config line
        sscanf(cfgLine, "%s", token);
        
        // Interpret the first token
        if (!strcmp(token, "rootKinematics"))
        {
            // Set the kinematics object
            sscanf(cfgLine, "%*s %s", objName);
            rootKinematics = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "viewKinematics"))
        {
            // Set the kinematics object
            sscanf(cfgLine, "%*s %s", objName);
            viewKinematics = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "forwardAxis"))
        {
            // Fetch the input axis for forward/backward control
            sscanf(cfgLine, "%*s %s %d", objName, &axisNum);
            ioDev = (vsIODevice *)(findObject(objName));
            if (ioDev != NULL)
                forwardAxis = ioDev->getAxis(axisNum);
        }
        else if (!strcmp(token, "strafeAxis"))
        {
            // Fetch the input axis for strafe (left/right) control
            sscanf(cfgLine, "%*s %s %d", objName, &axisNum);
            ioDev = (vsIODevice *)(findObject(objName));
            if (ioDev != NULL)
                strafeAxis = ioDev->getAxis(axisNum);
        }
        else if (!strcmp(token, "headingAxis"))
        {
            // Fetch the input axis for heading (yaw) control
            sscanf(cfgLine, "%*s %s %d", objName, &axisNum);
            ioDev = (vsIODevice *)(findObject(objName));
            if (ioDev != NULL)
                headingAxis = ioDev->getAxis(axisNum);
        }
        else if (!strcmp(token, "pitchAxis"))
        {
            // Fetch the input axis for pitch control
            sscanf(cfgLine, "%*s %s %d", objName, &axisNum);
            ioDev = (vsIODevice *)(findObject(objName));
            if (ioDev != NULL)
                pitchAxis = ioDev->getAxis(axisNum);
        }
        else if (!strcmp(token, "mouse"))
        {
            // Fetch the input axis for pitch control
            sscanf(cfgLine, "%*s %s %d", objName, &axisNum);
            mouse = (vsMouse *)(findObject(objName));
        }
        else if (!strcmp(token, "maxForwardSpeed"))
        {
            // Set the maximum forward movement speed
            sscanf(cfgLine, "%*s %lf", &maxForwardSpeed);
        }
        else if (!strcmp(token, "maxReverseSpeed"))
        {
            // Set the maximum backward movement speed
            sscanf(cfgLine, "%*s %lf", &maxReverseSpeed);
        }
        else if (!strcmp(token, "maxStrafeSpeed"))
        {
            // Set the maximum strafe (side-to-side) movement speed
            sscanf(cfgLine, "%*s %lf", &maxStrafeSpeed);
        }
        else if (!strcmp(token, "headingRate"))
        {
            // Set the maximum rate at which an incremental axis turns
            sscanf(cfgLine, "%*s %lf", &headingRate);
        }
        else if (!strcmp(token, "pitchRate"))
        {
            // Set the maximum rate at which an incremental axis turns
            sscanf(cfgLine, "%*s %lf", &headingRate);
        }
        else if (!strcmp(token, "headingMode"))
        {
            // Set whether the heading is controlled directly or 
            // incrementally by the heading axis
            sscanf(cfgLine, "%*s %s", axisMode);

            if (strcmp(axisMode, "VS_FPSM_MODE_INCREMENTAL") == 0)
                headingMode = VS_FPSM_MODE_INCREMENTAL;
            else if (strcmp(axisMode, "VS_FPSM_MODE_ABSOLUTE") == 0)
                headingMode = VS_FPSM_MODE_ABSOLUTE;
            else
                printf("vsAvatar::makeVsFPSMotion:  Unknown axis mode '%s'\n",
                    axisMode);
        }
        else if (!strcmp(token, "pitchMode"))
        {
            // Set whether the heading is controlled directly or 
            // incrementally by the heading axis
            sscanf(cfgLine, "%*s %s", axisMode);

            if (strcmp(axisMode, "VS_FPSM_MODE_INCREMENTAL") == 0)
                pitchMode = VS_FPSM_MODE_INCREMENTAL;
            else if (strcmp(axisMode, "VS_FPSM_MODE_ABSOLUTE") == 0)
                pitchMode = VS_FPSM_MODE_ABSOLUTE;
            else
                printf("vsAvatar::makeVsFPSMotion:  Unknown axis mode '%s'\n",
                    axisMode);
        }
        else if (!strcmp(token, "minPitch"))
        {
            // Set the minimum pitch value allowed
            sscanf(cfgLine, "%*s %lf", &minPitch);
        }
        else if (!strcmp(token, "maxPitch"))
        {
            // Set the maximum pitch value allowed
            sscanf(cfgLine, "%*s %lf", &maxPitch);
        }
        else
            printf("vsAvatar::makeVsFPSMotion: Unrecognized token '%s'\n",
                token);
    }
    
    // We need to have at least the root kinematics defined.  If it isn't
    // defined, bail out and return NULL for the new motion model.
    if (rootKinematics == NULL)
    {
        printf("vsAvatar::makeVsFPSMotion: Root kinematics object not "
            "specified\n");
        return NULL;
    }

    // If the root kinematics is specified, but not the view kinematics,
    // assume the user wants to use the root kinematics for pitch control
    // as well.
    if (viewKinematics == NULL)
        viewKinematics = rootKinematics;

    // Note that no axis need be specified.  The user is free to create
    // a motion model with no controls, if they so desire.

    // If a mouse was specified, use the mouse constructor for the vsFPSMotion.
    // Otherwise, use the 4 axis constructor
    if (mouse != NULL)
    {
        // Create the motion model using the mouse object
        result = new vsFPSMotion(forwardAxis, strafeAxis, mouse, 
            rootKinematics, viewKinematics);
    }
    else
    {
        // Create the motion model using the four axis objects
        result = new vsFPSMotion(forwardAxis, strafeAxis, headingAxis, 
            pitchAxis, rootKinematics, viewKinematics);
    }

    // Set the remaining parameters
    result->setMaxForwardSpeed(maxForwardSpeed);
    result->setMaxReverseSpeed(maxReverseSpeed);
    result->setMaxStrafeSpeed(maxStrafeSpeed);
    result->setHeadingRate(headingRate);
    result->setPitchRate(pitchRate);
    result->setHeadingAxisMode(headingMode);
    result->setPitchAxisMode(pitchMode);
    result->setPitchLimits(minPitch, maxPitch);
    
    // Return the created vsFPSMotion object
    return result;
}
