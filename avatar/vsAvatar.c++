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

#include <string.h>
#include "vsISTJoystickBox.h++"
#include "vsUnwinder.h++"
#include "vsFlockOfBirds.h++"
#include "vsSerialMotionStar.h++"
#include "vsFastrak.h++"
#include "vsIS600.h++"
#include "vsEthernetMotionStar.h++"
#include "vsWSSpaceball.h++"
#include "vsPinchGloveBox.h++"
#include "vsCyberGloveBox.h++"
#include "vs3TrackerArm.h++"
#include "vsCollision.h++"
#include "vsDrivingMotion.h++"
#include "vsFlyingMotion.h++"
#include "vsHeadMotion.h++"
#include "vsTerrainFollow.h++"
#include "vsTrackballMotion.h++"
#include "vsTrackedMotion.h++"
#include "vsWalkArticulation.h++"
#include "vsWalkInPlace.h++"

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsAvatar::vsAvatar()
{
    cfgFile = NULL;
    masterScene = NULL;
    objectArray = NULL;
    objNameArray = NULL;
    objTypeArray = NULL;
    objectCount = 0;
    isInitted = 0;
}

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsAvatar::vsAvatar(vsComponent *scene)
{
    cfgFile = NULL;
    masterScene = scene;
    objectArray = NULL;
    objNameArray = NULL;
    objTypeArray = NULL;
    objectCount = 0;
    isInitted = 0;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsAvatar::~vsAvatar()
{
    if (cfgFile)
        fclose(cfgFile);
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
    char objectType[256], objectName[256];
    void *newObject;
    int loop;
    
    if (isInitted)
    {
        printf("vsAvatar::init: Avatar has already been initialized\n");
        return;
    }

    isInitted = 1;

    // Check to see if the user wants to go without a config file
    if (configFile == NULL)
    {
        setup();
        return;
    }

    cfgFile = fopen(configFile, "r");
    if (!cfgFile)
    {
        printf("vsAvatar::init: Unable to open configuration file %s\n",
            configFile);
        return;
    }
    
    objectArray = new vsGrowableArray(10, 10);
    objNameArray = new vsGrowableArray(10, 10);
    objTypeArray = new vsGrowableArray(10, 10);
    objectCount = 0;

    lineType = 0;
    while (lineType != -1)
    {
        lineType = readCfgLine(lineBuffer);
        if (lineType != 1)
            continue;
        
        sscanf(lineBuffer, "%s %s", objectType, objectName);
        
        newObject = createObject(objectType);
        
        addObjectToArrays(newObject, objectName, objectType);
    }
    
    setup();
    
    // Clean up the string objects
    for (loop = 0; loop < objectCount; loop++)
    {
        free(objNameArray->getData(loop));
        free(objTypeArray->getData(loop));
    }
    
    // Clean up everything else
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
void vsAvatar::addObjectToArrays(void *object, char *name, char *type)
{
    if (!objectArray)
        return;

    objectArray->setData(objectCount, object);
    objNameArray->setData(objectCount, strdup(name));
    objTypeArray->setData(objectCount, strdup(type));
    objectCount++;
}

// ------------------------------------------------------------------------
// Protected function
// Reads a line from the open configuration file into the specified buffer.
// Blank lines and comments are weeded out. The leading token of each line
// is interpreted and removed. The function returns a 1 if a 'type' token
// is parsed, indicating a new object. A return value of 0 indicates a
// 'set' token was parsed, indicating data for an object under
// construction. If an 'end' token is parsed, or if the end-of-file is
// encountered, -1 is returned.
// ------------------------------------------------------------------------
int vsAvatar::readCfgLine(char *buffer)
{
    char *p;
    char inBuffer[256], keyword[256];
    int goodLine = 0;
    
    if (!cfgFile)
        return -1;

    fscanf(cfgFile, " \n");
    if (feof(cfgFile))
        return -1;

    while (!goodLine)
    {
        // Clear whitespace and check for end-of-file
        fscanf(cfgFile, " \n");
        if (feof(cfgFile))
            return -1;

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

        // Determine if there's anything left on the line
        if (strlen(inBuffer) == 0)
            continue;
        sscanf(inBuffer, "%s", keyword);
        
        // Figure out which type of line this is
        if (!strcmp(keyword, "end"))
        {
            // End-of-block
            buffer[0] = 0;
            return -1;
        }
        else if (!strcmp(keyword, "set"))
        {
            p = strchr(inBuffer, ' ');
            if (!p)
                continue;
            strcpy(buffer, &(p[1]));
            return 0;
        }
        else if (!strcmp(keyword, "type"))
        {
            p = strchr(inBuffer, ' ');
            if (!p)
                continue;
            strcpy(buffer, &(p[1]));
            return 1;
        }
        else
        {
            printf("vsAvatar::readCfgLine: Unrecognized keyword '%s'\n",
                keyword);
        }
    }
    
    return -1;
}

// ------------------------------------------------------------------------
// Protected function
// Helper function that searches the configuration object arrays for an
// object with a name equal to the 'targetStr' parameter. Returns a pointer
// to the object if found, NULL otherwise. This is a case-sensitive search.
// ------------------------------------------------------------------------
void *vsAvatar::findObject(char *targetStr)
{
    int loop;

    if (!targetStr || !objectArray)
        return NULL;

    for (loop = 0; loop < objectCount; loop++)
        if (strcmp(targetStr,
            (const char *)(objNameArray->getData(loop))) == 0)
            return objectArray->getData(loop);

    printf("vsAvatar::findObject: Can't find object '%s'\n", targetStr);
    return NULL;
}


// ------------------------------------------------------------------------
// Protected function
// Initiates construction of an object of the type specified by the given
// string. The various make* function do the actual work of creating the
// requested object. If this function is overridden, it should still be
// called from the child class' version to handle the object types listed
// here. All of the make* functions have access to the configuration file
// in order to read in required data.
// ------------------------------------------------------------------------
void *vsAvatar::createObject(char *idString)
{
    if (!strcmp(idString, "geometry"))
        return makeGeometry();
    else if (!strcmp(idString, "viewpoint"))
        return makeViewpoint();
    else if (!strcmp(idString, "inputDevice"))
        return makeInputDevice();
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
    else if (!strcmp(idString, "vsWSSpaceball"))
        return makeVsWSSpaceball();
    else if (!strcmp(idString, "vsPinchGloveBox"))
        return makeVsPinchGloveBox();
    else if (!strcmp(idString, "vsCyberGloveBox"))
        return makeVsCyberGloveBox();
    else if (!strcmp(idString, "vsKinematics"))
        return makeVsKinematics();
    else if (!strcmp(idString, "vs3TrackerArm"))
        return makeVs3TrackerArm();
    else if (!strcmp(idString, "vsCollision"))
        return makeVsCollision();
    else if (!strcmp(idString, "vsDrivingMotion"))
        return makeVsDrivingMotion();
    else if (!strcmp(idString, "vsFlyingMotion"))
        return makeVsFlyingMotion();
    else if (!strcmp(idString, "vsHeadMotion"))
        return makeVsHeadMotion();
    else if (!strcmp(idString, "vsTerrainFollow"))
        return makeVsTerrainFollow();
    else if (!strcmp(idString, "vsTrackballMotion"))
        return makeVsTrackballMotion();
    else if (!strcmp(idString, "vsTrackedMotion"))
        return makeVsTrackedMotion();
    else if (!strcmp(idString, "vsWalkArticulation"))
        return makeVsWalkArticulation();
    else if (!strcmp(idString, "vsWalkInPlace"))
        return makeVsWalkInPlace();
    
    return NULL;
}

// ------------------------------------------------------------------------
// Protected function
// Backwards compatability for the other, deprecated version of this
// function. Calls that function with the parameters it expects. New
// subclasses should override this function instead of that one. This
// function will be pure virtual in the next major VESS version.
// ------------------------------------------------------------------------
void vsAvatar::setup()
{
    setup(objectArray, objNameArray, objectCount);
}

// ------------------------------------------------------------------------
// * Deprecated *
// Protected function
// This function should be pure virtual, but is instead declared to produce
// an error message. This is because technically a subclass only needs to
// override one version of this function, but there is no way to tell the
// compiler that. So instead, this version of the function is called if
// neither version of the function is overridden.
// ------------------------------------------------------------------------
void vsAvatar::setup(vsGrowableArray *objArray, vsGrowableArray *strArray,
    int objCount)
{
    printf("vsAvatar::setup: A subclass of vsAvatar must override "
        "the setup function to operate\n");
    isInitted = 0;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a scene graph from data in the configuration file, and returns
// a pointer to the root node.
// ------------------------------------------------------------------------
void *vsAvatar::makeGeometry()
{
    vsDatabaseLoader *dbLoader;
    vsOptimizer *optimizer;
    vsNode *result;
    char cfgLine[256], token[256], strValue[256];
    int lineType = 0;
    char dbName[256];
    int intValue;
    int optFlag = 1;
    unsigned int isectVal = 0xFFFFFFFF;
    int autoAdd = 0;
    int emptyFlag = VS_FALSE;
    
    dbLoader = (vsSystem::systemObject)->getLoader();
    
    dbName[0] = 0;
    result = NULL;
    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "name"))
        {
            sscanf(cfgLine, "%*s %s", strValue);
            dbLoader->addImportantNodeName(strValue);
        }
        else if (!strcmp(token, "clearnames"))
            dbLoader->clearNames();
        else if (!strcmp(token, "allnames"))
        {
            sscanf(cfgLine, "%*s %d", &intValue);
            dbLoader->setLoaderMode(VS_DATABASE_MODE_NAME_XFORM, intValue);
        }
        else if (!strcmp(token, "units"))
        {
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
            sscanf(cfgLine, "%*s %s", dbName);
        else if (!strcmp(token, "empty"))
            emptyFlag = VS_TRUE;
        else if (!strcmp(token, "optimize"))
            sscanf(cfgLine, "%*s %d", &optFlag);
        else if (!strcmp(token, "addpath"))
        {
            sscanf(cfgLine, "%*s %s", strValue);
            dbLoader->addPath(strValue);
        }
        else if (!strcmp(token, "intersectValue"))
            sscanf(cfgLine, "%*s %x", &isectVal);
        else if (!strcmp(token, "addToScene"))
            sscanf(cfgLine, "%*s %d", &autoAdd);
        else
            printf("vsAvatar::makeGeometry: Unrecognized token '%s'\n",
                token);
    }

    if (emptyFlag)
	result = new vsComponent();
    else if (strlen(dbName) > 0)
    {
        result = dbLoader->loadDatabase(dbName);
        if (result && optFlag)
        {
            optimizer = new vsOptimizer;
            optimizer->optimize(result);
            delete optimizer;
        }
    }
    
    result->setIntersectValue(isectVal);
    
    if (autoAdd && masterScene)
        masterScene->addChild(result);
    
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsView and a vsViewpointAttribute together, and attaches them
// to a specifed vsPane and vsComponent, respectively.
// ------------------------------------------------------------------------
void *vsAvatar::makeViewpoint()
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
    vsVector posOffset(0.0, 0.0, 0.0);
    vsQuat oriOffset(0.0, 0.0, 0.0, 1.0);
    double xoffset = 0.0;
    double yoffset = 0.0;
    double zoffset = 0.0;
    double hoffset = 0.0;
    double poffset = 0.0;
    double roffset = 0.0;
    vsMatrix offsetMat, tempMat;

    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "geometry"))
        {
            lineLen = sscanf(cfgLine, "%*s %s %s", geoObjectName, nodeName);
	    if (lineLen < 2)
		geom = (vsComponent *)(findObject(geoObjectName));
	    else
	    {
		root = (vsComponent *)(findObject(geoObjectName));
		if (root)
		    geom = (vsComponent *)(root->findNodeByName(nodeName));
	    }
        }
        else if (!strcmp(token, "pane"))
        {
            sscanf(cfgLine, "%*s %d %d %d", &screenNum, &windowNum, &paneNum);
            screen = (vsSystem::systemObject)->getScreen(screenNum);
            if (screen)
            {
                window = screen->getChildWindow(windowNum);
                if (window)
                    pane = window->getChildPane(paneNum);
            }
        }
        else if (!strcmp(token, "positionOffset"))
            sscanf(cfgLine, "%*s %lf %lf %lf", &xoffset, &yoffset, &zoffset);
        else if (!strcmp(token, "orientationOffset"))
            sscanf(cfgLine, "%*s %lf %lf %lf", &hoffset, &poffset, &roffset);
        else
            printf("vsAvatar::makeViewpoint: Unrecognized token '%s'\n",
                token);
    }
    
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

    view = new vsView();
    pane->setView(view);
    result = new vsViewpointAttribute(view);
    geom->addAttribute(result);
    
    offsetMat.setTranslation(xoffset, yoffset, zoffset);
    tempMat.setEulerRotation(VS_EULER_ANGLES_ZXY_R, hoffset, poffset, roffset);
    offsetMat = offsetMat * tempMat;
    result->setOffsetMatrix(offsetMat);

    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Extracts a subclass of vsInputDevice out of a specified vsInputSystem.
// Motion models that can use a vsInputAxis or vsInputButton take one of
// these objects and get the axis or button from that.
// ------------------------------------------------------------------------
void *vsAvatar::makeInputDevice()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsInputDevice *result = NULL;
    char objName[256];
    int objNum;
    vsJoystickBox *joyBox;
    vsTrackingSystem *trackSys;
    vsPinchGloveBox *pinchBox;
    vsScreen *screen;
    vsWindow *window;
    vsWindowSystem *wsys;
    int screenIdx, windowIdx;
    vsWSSpaceball *wsSpaceball;
    vsCyberGloveBox *cyberBox;

    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "joystickBox"))
        {
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);
            joyBox = (vsJoystickBox *)(findObject(objName));
            if (joyBox)
                result = joyBox->getJoystick(objNum);
        }
        else if (!strcmp(token, "trackingSystem"))
        {
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);
            trackSys = (vsTrackingSystem *)(findObject(objName));
            if (trackSys)
                result = trackSys->getTracker(objNum);
        }
        else if (!strcmp(token, "pinchGloveBox"))
        {
            sscanf(cfgLine, "%*s %s", objName);
            pinchBox = (vsPinchGloveBox *)(findObject(objName));
            if (pinchBox)
                result = pinchBox->getGloves();
        }
        else if (!strcmp(token, "windowSystem"))
        {
            sscanf(cfgLine, "%*s %d %d %s", &screenIdx, &windowIdx, objName);
            screen = (vsSystem::systemObject)->getScreen(screenIdx);
            if (screen)
            {
                window = screen->getChildWindow(windowIdx);
                if (window)
                {
                    wsys = window->getWSystem();
                    if (!wsys)
                    {
                        wsys = new vsWindowSystem(window);
                        addObjectToArrays(wsys, "vsWindowSystem",
                            "vsWindowSystem");
                    }
                    if (!strcmp(objName, "mouse"))
                        result = wsys->getMouse();
                    else if (!strcmp(objName, "keyboard"))
                        result = wsys->getKeyboard();
                    else
                        printf("vsAvatar::makeInputDevice (windowSystem): "
                            "Unrecognized window system device '%s'\n",
                            objName);
                }
            }
        }
        else if (!strcmp(token, "WSSpaceball"))
        {
            sscanf(cfgLine, "%*s %s", objName);
            wsSpaceball = (vsWSSpaceball *)(findObject(objName));
            if (wsSpaceball)
                result = wsSpaceball->getSpaceball();
        }
        else if (!strcmp(token, "cyberGloveBox"))
        {
            sscanf(cfgLine, "%*s %s", objName);
            cyberBox = (vsCyberGloveBox *)(findObject(objName));
            if (cyberBox)
                result = cyberBox->getGlove();
        }
        else
            printf("vsAvatar::makeInputDevice: Unrecognized token '%s'\n",
                token);
    }
    
    if (!result)
        printf("vsAvatar::makeInputDevice: No vsInputSystem specified\n");

    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsISTJoystickBox from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
void *vsAvatar::makeVsISTJoystickBox()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    int portNumber = -1;

    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "port"))
            sscanf(cfgLine, "%*s %d", &portNumber);
        else
            printf("vsAvatar::makeVsISTJoystickBox: Unrecognized token '%s'\n",
                token);
    }
    
    if (portNumber == -1)
    {
        printf("vsAvatar::makeVsISTJoystickBox: No port number specified\n");
        return NULL;
    }

    return (new vsISTJoystickBox(portNumber));
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsUnwinder from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
void *vsAvatar::makeVsUnwinder()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    int portNumber = -1;
    int joy1 = 1;
    int joy2 = 0;

    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "port"))
            sscanf(cfgLine, "%*s %d", &portNumber);
        else if (!strcmp(token, "joy1"))
            sscanf(cfgLine, "%*s %d", &joy1);
        else if (!strcmp(token, "joy2"))
            sscanf(cfgLine, "%*s %d", &joy2);
        else
            printf("vsAvatar::makeVsUnwinder: Unrecognized token '%s'\n",
                token);
    }
    
    if (portNumber == -1)
    {
        printf("vsAvatar::makeVsUnwinder: No port number specified\n");
        return NULL;
    }

    return (new vsUnwinder(portNumber, joy1, joy2));
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsFlockOfBirds from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
void *vsAvatar::makeVsFlockOfBirds()
{
    char cfgLine[256];
    char token[256];
    char strValue[256];
    int lineType = 0;
    int portNumbers[200];
    int whichPort;
    int nTrackers = 0;
    int dataFormat = VS_AS_DATA_POS_QUAT;
    int baud = 9600;
    int mode = VS_AS_MODE_FLOCK;
    int hemisphere = -1;
    int multiFlag = 0;
    int forkFlag = 0;
    vsFlockOfBirds *result;
    
    portNumbers[0] = -1;

    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "port"))
            sscanf(cfgLine, "%*s %d", &(portNumbers[0]));
        else if (!strcmp(token, "mport"))
        {
            sscanf(cfgLine, "%*s %d", &whichPort);
            sscanf(cfgLine, "%*s %*d %d", &(portNumbers[whichPort]));
            multiFlag = 1;
        }
        else if (!strcmp(token, "trackers"))
            sscanf(cfgLine, "%*s %d", &nTrackers);
        else if (!strcmp(token, "format"))
        {
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
            sscanf(cfgLine, "%*s %d", &baud);
        else if (!strcmp(token, "mode"))
        {
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
            sscanf(cfgLine, "%*s %d", &forkFlag);
        else if (!strcmp(token, "hemisphere"))
        {
            sscanf(cfgLine, "%*s %s", strValue);
            
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
        }
        else
            printf("vsAvatar::makeVsFlockOfBirds: Unrecognized token '%s'\n",
                token);
    }
    
    if (portNumbers[0] == -1)
    {
        printf("vsAvatar::makeVsFlockOfBirds: Port number(s) not specified\n");
        return NULL;
    }

    if (multiFlag)
        result = new vsFlockOfBirds(portNumbers, nTrackers, dataFormat,
            baud);
    else
        result = new vsFlockOfBirds(portNumbers[0], nTrackers, dataFormat,
            baud, mode);

    if (hemisphere != -1)
        result->setActiveHemisphere(VS_AS_ALL_TRACKERS, hemisphere);

    if (forkFlag)
        result->forkTracking();

    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsSerialMotionStar from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
void *vsAvatar::makeVsSerialMotionStar()
{
    char cfgLine[256];
    char token[256];
    char strValue[256];
    int lineType = 0;
    int portNumbers[200];
    int whichPort;
    int nTrackers = 0;
    int dataFormat = VS_AS_DATA_POS_QUAT;
    int baud = 9600;
    int hemisphere = -1;
    int multiFlag = 0;
    int forkFlag = 0;
    vsSerialMotionStar *result;
    
    portNumbers[0] = -1;

    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "port"))
            sscanf(cfgLine, "%*s %d", &(portNumbers[0]));
        else if (!strcmp(token, "mport"))
        {
            sscanf(cfgLine, "%*s %d", &whichPort);
            sscanf(cfgLine, "%*s %*d %d", &(portNumbers[whichPort]));
            multiFlag = 1;
        }
        else if (!strcmp(token, "trackers"))
            sscanf(cfgLine, "%*s %d", &nTrackers);
        else if (!strcmp(token, "format"))
        {
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
                printf("vsAvatar::makeVsSerialMotionStar (format): "
                    "Unrecognized format constant '%s'\n", strValue);
        }
        else if (!strcmp(token, "baud"))
            sscanf(cfgLine, "%*s %d", &baud);
        else if (!strcmp(token, "fork"))
            sscanf(cfgLine, "%*s %d", &forkFlag);
        else if (!strcmp(token, "hemisphere"))
        {
            sscanf(cfgLine, "%*s %s", strValue);
            
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
        }
        else
            printf("vsAvatar::makeVsSerialMotionStar: Unrecognized "
                "token '%s'\n", token);
    }
    
    if (portNumbers[0] == -1)
    {
        printf("vsAvatar::makeVsSerialMotionStar: Port number(s) not "
            "specified\n");
        return NULL;
    }

    if (multiFlag)
        result = new vsSerialMotionStar(portNumbers, nTrackers, dataFormat,
            baud);
    else
        result = new vsSerialMotionStar(portNumbers[0], nTrackers, dataFormat,
            baud);

    if (hemisphere != -1)
        result->setActiveHemisphere(VS_AS_ALL_TRACKERS, hemisphere);

    if (forkFlag)
        result->forkTracking();

    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsFastrak from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
void *vsAvatar::makeVsFastrak()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    int portNumber = -1;
    int baud = 9600;
    int nTrackers = 0;
    int forkFlag = 0;
    vsVector hemiVectors[VS_FT_MAX_TRACKERS];
    int stationNum, loop;
    double hemiX, hemiY, hemiZ;
    vsFastrak *result;

    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);

        if (!strcmp(token, "port"))
            sscanf(cfgLine, "%*s %d", &portNumber);
        else if (!strcmp(token, "baud"))
            sscanf(cfgLine, "%*s %d", &baud);
        else if (!strcmp(token, "trackers"))
            sscanf(cfgLine, "%*s %d", &nTrackers);
        else if (!strcmp(token, "fork"))
            sscanf(cfgLine, "%*s %d", &forkFlag);
        else if (!strcmp(token, "trackerHemi"))
        {
            sscanf(cfgLine, "%*s %d %lf %lf %lf", &stationNum, &hemiX,
                &hemiY, &hemiZ);
            (hemiVectors[stationNum-1]).set(hemiX, hemiY, hemiZ);
        }
        else
            printf("vsAvatar::makeVsFastrak: Unrecognized token '%s'\n",
                token);
    }
    
    if (portNumber == -1)
    {
        printf("vsAvatar::makeVsFastrak: No port number specified\n");
        return NULL;
    }

    result = new vsFastrak(portNumber, baud, nTrackers);
    
    for (loop = 0; loop < VS_FT_MAX_TRACKERS; loop++)
        if (hemiVectors[loop].getMagnitude() > 1E-6)
            result->setActiveHemisphere(loop+1, hemiVectors[loop]);
    
    if (forkFlag)
        result->forkTracking();

    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsIS600 from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
void *vsAvatar::makeVsIS600()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    int portNumber = -1;
    int baud = 9600;
    int nTrackers = 0;
    int forkFlag = 0;
    vsIS600 *result;

    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);

        if (!strcmp(token, "port"))
            sscanf(cfgLine, "%*s %d", &portNumber);
        else if (!strcmp(token, "baud"))
            sscanf(cfgLine, "%*s %d", &baud);
        else if (!strcmp(token, "trackers"))
            sscanf(cfgLine, "%*s %d", &nTrackers);
        else if (!strcmp(token, "fork"))
            sscanf(cfgLine, "%*s %d", &forkFlag);
        else
            printf("vsAvatar::makeVsIS600: Unrecognized token '%s'\n",
                token);
    }
    
    if (portNumber == -1)
    {
        printf("vsAvatar::makeVsIS600: No port number specified\n");
        return NULL;
    }

    result = new vsIS600(portNumber, baud, nTrackers);
    
    if (forkFlag)
        result->forkTracking();

    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsEthernetMotionStar from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
void *vsAvatar::makeVsEthernetMotionStar()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    char serverName[256], strValue[256];
    int portNumber = -1;
    int nTrackers = 0;
    int dataFormat = VS_BN_FLOCK_POSITIONQUATERNION;
    int hemisphere = -1;
    int masterFlag = 1;
    int forkFlag = 0;
    vsEthernetMotionStar *result;
    
    serverName[0] = 0;

    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "port"))
            sscanf(cfgLine, "%*s %d", &portNumber);
        else if (!strcmp(token, "ip"))
            sscanf(cfgLine, "%*s %s", serverName);
        else if (!strcmp(token, "trackers"))
            sscanf(cfgLine, "%*s %d", &nTrackers);
        else if (!strcmp(token, "format"))
        {
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
            sscanf(cfgLine, "%*s %d", &masterFlag);
        else if (!strcmp(token, "fork"))
            sscanf(cfgLine, "%*s %d", &forkFlag);
        else if (!strcmp(token, "hemisphere"))
        {
            sscanf(cfgLine, "%*s %s", strValue);
            
            if (!strcmp(strValue, "VS_BN_FRONT_HEMISHPERE"))
                hemisphere = VS_BN_FRONT_HEMISHPERE;
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
        }
        else
            printf("vsAvatar::makeVsEthernetMotionStar: Unrecognized "
                "token '%s'\n", token);
    }

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

    result = new vsEthernetMotionStar(serverName, portNumber, nTrackers,
        masterFlag, dataFormat);

    if (hemisphere != -1)
        result->setActiveHemisphere(VS_MSTAR_ALL_TRACKERS, hemisphere);

    if (forkFlag)
        result->forkTracking();

    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsWSSpaceball from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
void *vsAvatar::makeVsWSSpaceball()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsWindowSystem *wsys = NULL;
    int btnCount = 1;
    int screenNum, windowNum;
    vsScreen *screen;
    vsWindow *window;

    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "window"))
        {
            sscanf(cfgLine, "%*s %d %d", &screenNum, &windowNum);
            screen = (vsSystem::systemObject)->getScreen(screenNum);
            if (screen)
            {
                window = screen->getChildWindow(windowNum);
                if (window)
                {
                    wsys = window->getWSystem();
                    if (!wsys)
                    {
                        wsys = new vsWindowSystem(window);
                        addObjectToArrays(wsys, "vsWindowSystem",
                            "vsWindowSystem");
                    }
                }
            }
        }
        else if (!strcmp(token, "buttons"))
            sscanf(cfgLine, "%*s %d", &btnCount);
        else
            printf("vsAvatar::makeVsWSSpaceball: Unrecognized token '%s'\n",
                token);
    }
    
    if (!wsys)
    {
        printf("vsAvatar::makeVsWSSpaceball: No window specified\n");
        return NULL;
    }

    return (new vsWSSpaceball(wsys, btnCount));
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsPinchGloveBox from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
void *vsAvatar::makeVsPinchGloveBox()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    int portNumber = -1;
    int baud = 9600;

    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "port"))
            sscanf(cfgLine, "%*s %d", &portNumber);
        else if (!strcmp(token, "baud"))
            sscanf(cfgLine, "%*s %d", &baud);
        else
            printf("vsAvatar::makeVsPinchGloveBox: Unrecognized token '%s'\n",
                token);
    }
    
    if (portNumber == -1)
    {
        printf("vsAvatar::makeVsPinchGloveBox: No port number specified\n");
        return NULL;
    }
    
    return (new vsPinchGloveBox(portNumber, baud));
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsCyberGloveBox from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
void *vsAvatar::makeVsCyberGloveBox()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    int portNumber = -1;
    int baud = 9600;
    int numSensors = 0;

    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "port"))
            sscanf(cfgLine, "%*s %d", &portNumber);
        else if (!strcmp(token, "baud"))
            sscanf(cfgLine, "%*s %d", &baud);
        else if (!strcmp(token, "sensors"))
            sscanf(cfgLine, "%*s %d", &numSensors);
        else
            printf("vsAvatar::makeVsCyberGloveBox: Unrecognized token '%s'\n",
                token);
    }
    
    if (portNumber == -1)
    {
        printf("vsAvatar::makeVsCyberGloveBox: No port number specified\n");
        return NULL;
    }
    
    return (new vsCyberGloveBox(portNumber, baud, numSensors));
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsKinematics from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
void *vsAvatar::makeVsKinematics()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsComponent *root;
    vsComponent *geom = NULL;
    int lineLen;
    int inertia = -1;
    vsVector massCenter(0.0, 0.0, 0.0);
    vsVector startPos(0.0, 0.0, 0.0);
    vsQuat startOrient(0.0, 0.0, 0.0, 1.0);
    char geoObjectName[256], nodeName[256];
    vsKinematics *result;
    double a, b, c;

    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "geometry"))
        {
            lineLen = sscanf(cfgLine, "%*s %s %s", geoObjectName, nodeName);
	    if (lineLen < 2)
		geom = (vsComponent *)(findObject(geoObjectName));
	    else
	    {
		root = (vsComponent *)(findObject(geoObjectName));
		if (root)
		    geom = (vsComponent *)(root->findNodeByName(nodeName));
	    }
        }
        else if (!strcmp(token, "inertia"))
            sscanf(cfgLine, "%*s %d", &inertia);
        else if (!strcmp(token, "center"))
        {
            sscanf(cfgLine, "%*s %lf %lf %lf", &a, &b, &c);
            massCenter.set(a, b, c);
        }
        else if (!strcmp(token, "position"))
        {
            sscanf(cfgLine, "%*s %lf %lf %lf", &a, &b, &c);
            startPos.set(a, b, c);
        }
        else if (!strcmp(token, "orientation"))
        {
            sscanf(cfgLine, "%*s %lf %lf %lf", &a, &b, &c);
            startOrient.setEulerRotation(VS_EULER_ANGLES_ZXY_R, a, b, c);
        }
        else
            printf("vsAvatar::makeVsKinematics: Unrecognized token '%s'\n",
                token);
    }
    
    if (!geom)
    {
        printf("vsAvatar::makeVsKinematics: Target node not specified\n");
        return NULL;
    }

    result = new vsKinematics(geom);
    
    if (inertia == 1)
        result->enableInertia();
    else if (inertia == 0)
        result->disableInertia();

    result->setCenterOfMass(massCenter);
    result->setPosition(startPos);
    result->setOrientation(startOrient);
    
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vs3TrackerArm from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
void *vsAvatar::makeVs3TrackerArm()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsTrackingSystem *tsys;
    vsMotionTracker *trackers[3];
    vsKinematics *kinematics[3];
    vsVector offsets[3];
    vsQuat preRotations[3];
    vsQuat postRotations[3];
    char objName[256];
    int whichJoint, trackerNum;
    int loop;
    double a, b, c;
    vs3TrackerArm *result;
    
    for (loop = 0; loop < 3; loop++)
    {
        trackers[loop] = NULL;
        kinematics[loop] = NULL;
        offsets[loop].set(0.0, 0.0, 0.0);
        preRotations[loop].set(0.0, 0.0, 0.0, 1.0);
        postRotations[loop].set(0.0, 0.0, 0.0, 1.0);
    }

    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "tracker"))
        {
            sscanf(cfgLine, "%*s %d %s %d", &whichJoint, objName,
                &trackerNum);
            tsys = (vsTrackingSystem *)(findObject(objName));
            if (tsys && (whichJoint >= 0) && (whichJoint < 3))
                trackers[whichJoint] = tsys->getTracker(trackerNum);
            else if ((whichJoint < 0) || (whichJoint >= 3))
                printf("vsAvatar::makeVs3TrackerArm (tracker): "
                    "Invalid joint index\n");
        }
        else if (!strcmp(token, "kinematics"))
        {
            sscanf(cfgLine, "%*s %d %s", &whichJoint, objName);
            kinematics[whichJoint] = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "offset"))
        {
            sscanf(cfgLine, "%*s %d %lf %lf %lf", &whichJoint, &a, &b, &c);
            offsets[whichJoint].set(a, b, c);
        }
        else if (!strcmp(token, "preRotate"))
        {
            sscanf(cfgLine, "%*s %d %lf %lf %lf", &whichJoint, &a, &b, &c);
            preRotations[whichJoint].setEulerRotation(VS_EULER_ANGLES_ZXY_R,
                a, b, c);
        }
        else if (!strcmp(token, "postRotate"))
        {
            sscanf(cfgLine, "%*s %d %lf %lf %lf", &whichJoint, &a, &b, &c);
            postRotations[whichJoint].setEulerRotation(VS_EULER_ANGLES_ZXY_R,
                a, b, c);
        }
        else
            printf("vsAvatar::makeVs3TrackerArm: Unrecognized token '%s'\n",
                token);
    }
    
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

    result = new vs3TrackerArm(trackers[0], kinematics[0], trackers[1],
        kinematics[1], trackers[2], kinematics[2]);

    result->setShoulderOffset(offsets[0]);
    result->setElbowOffset(offsets[1]);
    result->setWristOffset(offsets[2]);
    
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
// Creates a vsCollision from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
void *vsAvatar::makeVsCollision()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsKinematics *kinematics = NULL;
    int pointCount = 0;
    unsigned int isectMask = 0xFFFFFFFF;
    int cmode = VS_COLLISION_MODE_STOP;
    double margin = VS_COLLISION_DEFAULT_MARGIN;
    char objName[256], strValue[256];
    vsCollision *result;
    double x, y, z;
    int loop;
    int pointIdx;
    vsGrowableArray pointArray(8, 8);

    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "kinematics"))
        {
            sscanf(cfgLine, "%*s %s", objName);
            kinematics = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "point"))
        {
            sscanf(cfgLine, "%*s %d %lf %lf %lf", &pointIdx, &x, &y, &z);
            if ((pointIdx >= 0) && (pointIdx < VS_COLLISION_POINTS_MAX))
            {
                for (loop = pointCount; loop <= pointIdx; loop++)
                    pointArray[loop] = NULL;
                pointCount = pointIdx+1;
                if (pointArray[pointIdx])
                    delete ((vsVector *)(pointArray[pointIdx]));
                pointArray[pointIdx] = new vsVector(x, y, z);
            }
            else
                printf("vsAvatar::makeVsCollision (point): "
                    "Point index out of bounds\n");
        }
        else if (!strcmp(token, "intersectMask"))
            sscanf(cfgLine, "%*s %x", &isectMask);
        else if (!strcmp(token, "mode"))
        {
            sscanf(cfgLine, "%*s %s", strValue);
            if (!strcmp(strValue, "VS_COLLISION_MODE_STOP"))
                cmode = VS_COLLISION_MODE_STOP;
            else if (!strcmp(strValue, "VS_COLLISION_MODE_SLIDE"))
                cmode = VS_COLLISION_MODE_SLIDE;
            else if (!strcmp(strValue, "VS_COLLISION_MODE_BOUNCE"))
                cmode = VS_COLLISION_MODE_BOUNCE;
            else
                printf("vsAvatar::makeVsCollision (mode): "
                    "Unrecognized mode constant '%s'\n", strValue);
        }
        else if (!strcmp(token, "margin"))
            sscanf(cfgLine, "%*s %lf", &margin);
        else
            printf("vsAvatar::makeVsCollision: Unrecognized token '%s'\n",
                token);
    }
    
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

    result = new vsCollision(kinematics, masterScene);
    
    result->setPointCount(pointCount);
    for (loop = 0; loop < pointCount; loop++)
        if (pointArray[loop])
        {
            result->setPoint(loop, *((vsVector *)(pointArray[loop])));
            delete ((vsVector *)(pointArray[loop]));
        }
        else
            result->setPoint(loop, vsVector(0.0, 0.0, 0.0));

    result->setCollisionMode(cmode);
    result->setIntersectMask(isectMask);
    result->setMargin(margin);
    
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsDrivingMotion from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
void *vsAvatar::makeVsDrivingMotion()
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
    double maxSpeed = VS_DM_DEFAULT_MAX_SPEED;
    double steeringRate = VS_DM_DEFAULT_STEER_RATE;
    vsDrivingMotion *result;
    vsInputAxis *steerAxis = NULL;
    vsInputAxis *throttleAxis = NULL;
    vsInputButton *accelBtn = NULL;
    vsInputButton *stopBtn = NULL;
    vsInputButton *decelBtn = NULL;
    vsInputDevice *inputDev;
    int objNum;
    
    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "kinematics"))
        {
            sscanf(cfgLine, "%*s %s", objName);
            kinematics = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "mouse"))
        {
            sscanf(cfgLine, "%*s %s", objName);
            mouse = (vsMouse *)(findObject(objName));
        }
        else if (!strcmp(token, "steeringAxis"))
        {
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);
            inputDev = (vsInputDevice *)(findObject(objName));
            if (inputDev)
                steerAxis = inputDev->getAxis(objNum);
        }
        else if (!strcmp(token, "throttleAxis"))
        {
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);
            inputDev = (vsInputDevice *)(findObject(objName));
            if (inputDev)
                throttleAxis = inputDev->getAxis(objNum);
        }
        else if (!strcmp(token, "accelButton"))
        {
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);
            inputDev = (vsInputDevice *)(findObject(objName));
            if (inputDev)
                accelBtn = inputDev->getButton(objNum);
        }
        else if (!strcmp(token, "stopButton"))
        {
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);
            inputDev = (vsInputDevice *)(findObject(objName));
            if (inputDev)
                stopBtn = inputDev->getButton(objNum);
        }
        else if (!strcmp(token, "decelButton"))
        {
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);
            inputDev = (vsInputDevice *)(findObject(objName));
            if (inputDev)
                decelBtn = inputDev->getButton(objNum);
        }
        else if (!strcmp(token, "throttleMode"))
        {
            sscanf(cfgLine, "%*s %s", strValue);
            if (!strcmp(strValue, "VS_DM_THROTTLE_VELOCITY"))
                throttle = VS_DM_THROTTLE_VELOCITY;
            else if (!strcmp(strValue, "VS_DM_THROTTLE_ACCELERATION"))
                throttle = VS_DM_THROTTLE_ACCELERATION;
            else
                printf("vsAvatar::makeVsDrivingMotion (throttleMode): "
                    "Unrecognized throttle mode constant '%s'\n", strValue);
        }
        else if (!strcmp(token, "accelRate"))
            sscanf(cfgLine, "%*s %lf", &accelRate);
        else if (!strcmp(token, "maxSpeed"))
            sscanf(cfgLine, "%*s %lf", &maxSpeed);
        else if (!strcmp(token, "steeringMode"))
        {
            sscanf(cfgLine, "%*s %s", strValue);
            if (!strcmp(strValue, "VS_DM_STEER_RELATIVE"))
                steering = VS_DM_STEER_RELATIVE;
            else if (!strcmp(strValue, "VS_DM_STEER_ABSOLUTE"))
                steering = VS_DM_STEER_ABSOLUTE;
            else
                printf("vsAvatar::makeVsDrivingMotion (steeringMode): "
                    "Unrecognized steering mode constant '%s'\n", strValue);
        }
        else if (!strcmp(token, "steeringRate"))
            sscanf(cfgLine, "%*s %lf", &steeringRate);
        else
            printf("vsAvatar::makeVsDrivingMotion: Unrecognized token '%s'\n",
                token);
    }
    
    if (!kinematics)
    {
        printf("vsAvatar::makeVsDrivingMotion: Kinematics object not "
            "specified\n");
        return NULL;
    }
    
    if (mouse)
        result = new vsDrivingMotion(mouse, kinematics);
    else if (steerAxis && throttleAxis)
        result = new vsDrivingMotion(steerAxis, throttleAxis, kinematics);
    else if (steerAxis)
        result = new vsDrivingMotion(steerAxis, accelBtn, decelBtn, stopBtn,
            kinematics);
    else
    {
        printf("vsAvatar::makeVsDrivingMotion: No mouse or steering axis "
            "specified\n");
        return NULL;
    }

    result->setThrottleMode(throttle);
    result->setAccelerationRate(accelRate);
    result->setMaxSpeed(maxSpeed);
    result->setSteeringMode(steering);
    result->setSteeringRate(steeringRate);
    
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsFlyingMotion from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
void *vsAvatar::makeVsFlyingMotion()
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
    vsInputDevice *inputDev;
    int objNum;

    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "kinematics"))
        {
            sscanf(cfgLine, "%*s %s", objName);
            kinematics = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "mouse"))
        {
            sscanf(cfgLine, "%*s %s", objName);
            mouse = (vsMouse *)(findObject(objName));
        }
        else if (!strcmp(token, "headingAxis"))
        {
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);
            inputDev = (vsInputDevice *)(findObject(objName));
            if (inputDev)
                headingAxis = inputDev->getAxis(objNum);
        }
        else if (!strcmp(token, "pitchAxis"))
        {
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);
            inputDev = (vsInputDevice *)(findObject(objName));
            if (inputDev)
                pitchAxis = inputDev->getAxis(objNum);
        }
        else if (!strcmp(token, "throttleAxis"))
        {
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);
            inputDev = (vsInputDevice *)(findObject(objName));
            if (inputDev)
                throttleAxis = inputDev->getAxis(objNum);
        }
        else if (!strcmp(token, "accelButton"))
        {
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);
            inputDev = (vsInputDevice *)(findObject(objName));
            if (inputDev)
                accelBtn = inputDev->getButton(objNum);
        }
        else if (!strcmp(token, "stopButton"))
        {
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);
            inputDev = (vsInputDevice *)(findObject(objName));
            if (inputDev)
                stopBtn = inputDev->getButton(objNum);
        }
        else if (!strcmp(token, "decelButton"))
        {
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);
            inputDev = (vsInputDevice *)(findObject(objName));
            if (inputDev)
                decelBtn = inputDev->getButton(objNum);
        }
        else if (!strcmp(token, "headingMode"))
        {
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
            sscanf(cfgLine, "%*s %lf", &accelRate);
        else if (!strcmp(token, "turnRate"))
            sscanf(cfgLine, "%*s %lf", &turnRate);
        else if (!strcmp(token, "maxSpeed"))
            sscanf(cfgLine, "%*s %lf", &maxSpeed);
        else
            printf("vsAvatar::makeVsFlyingMotion: Unrecognized token '%s'\n",
                token);
    }

    if (!kinematics)
    {
        printf("vsAvatar::makeVsFlyingMotion: Kinematics object not "
            "specified\n");
        return NULL;
    }

    if (mouse)
        result = new vsFlyingMotion(mouse, kinematics);
    else if (headingAxis && pitchAxis && throttleAxis)
        result = new vsFlyingMotion(headingAxis, pitchAxis, throttleAxis,
            kinematics);
    else if (headingAxis && pitchAxis)
        result = new vsFlyingMotion(headingAxis, pitchAxis, accelBtn,
            decelBtn, stopBtn, kinematics);
    else
    {
        printf("vsAvatar::makeVsFlyingMotion: No mouse or insufficient "
            "control axes specified\n");
        return NULL;
    }

    result->setAxisModes(headingMode, pitchMode, throttleMode);
    result->setAccelerationRate(accelRate);
    result->setTurningRate(turnRate);
    result->setMaxSpeed(maxSpeed);

    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsHeadMotion from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
void *vsAvatar::makeVsHeadMotion()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsKinematics *kinematics = NULL;
    char objName[256];
    vsTrackingSystem *tsys;
    vsMotionTracker *trackers[2];
    vsQuat oriOffset(0.0, 0.0, 0.0, 1.0);
    int whichTracker, trackerNum;
    vsHeadMotion *result;
    double h, p, r;
    
    trackers[0] = NULL;
    trackers[1] = NULL;

    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "kinematics"))
        {
            sscanf(cfgLine, "%*s %s", objName);
            kinematics = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "tracker"))
        {
            sscanf(cfgLine, "%*s %d %s %d", &whichTracker, objName,
                &trackerNum);
            tsys = (vsTrackingSystem *)(findObject(objName));
            if (tsys && (whichTracker >= 0) && (whichTracker < 2))
                trackers[whichTracker] = tsys->getTracker(trackerNum);
            else if ((whichTracker < 0) && (whichTracker >= 2))
                printf("vsAvatar::makeVsHeadTracker (tracker): "
                    "Invalid tracker index\n");
        }
        else if (!strcmp(token, "orientationOffset"))
        {
            sscanf(cfgLine, "%*s %lf %lf %lf", &h, &p, &r);
            oriOffset.setEulerRotation(VS_EULER_ANGLES_ZXY_R, h, p, r);
        }
        else
            printf("vsAvatar::makeVsHeadMotion: Unrecognized token '%s'\n",
                token);
    }
    
    if (!kinematics)
    {
        printf("vsAvatar::makeVsHeadMotion: Kinematics object not "
            "specified\n");
        return NULL;
    }
    if (!(trackers[0]))
    {
        printf("vsAvatar::makeVsHeadMotion: Back tracker object not "
            "specified\n");
        return NULL;
    }
    if (!(trackers[1]))
    {
        printf("vsAvatar::makeVsHeadMotion: Head tracker object not "
            "specified\n");
        return NULL;
    }

    result = new vsHeadMotion(trackers[0], trackers[1], kinematics);
    
    result->setOrientationOffset(oriOffset);
    
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsTerrainFollow from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
void *vsAvatar::makeVsTerrainFollow()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsKinematics *kinematics = NULL;
    char objName[256];
    unsigned int isectMask = 0xFFFFFFFF;
    vsVector offset(0.0, 0.0, 0.0);
    double stepHeight = VS_TFOLLOW_DEFAULT_HEIGHT;
    double x, y, z;
    vsTerrainFollow *result;

    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "kinematics"))
        {
            sscanf(cfgLine, "%*s %s", objName);
            kinematics = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "offset"))
        {
            sscanf(cfgLine, "%*s %lf %lf %lf", &x, &y, &z);
            offset.set(x, y, z);
        }
        else if (!strcmp(token, "stepHeight"))
            sscanf(cfgLine, "%*s %lf", &stepHeight);
        else if (!strcmp(token, "intersectMask"))
            sscanf(cfgLine, "%*s %x", &isectMask);
        else
            printf("vsAvatar::makeVsTerrainFollow: Unrecognized token '%s'\n",
                token);
    }
    
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

    result = new vsTerrainFollow(kinematics, masterScene);
    
    result->setBaseOffset(offset);
    result->setStepHeight(stepHeight);
    result->setIntersectMask(isectMask);

    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsTrackballMotion from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
void *vsAvatar::makeVsTrackballMotion()
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
    vsInputDevice *inputDev;
    int objNum;

    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "kinematics"))
        {
            sscanf(cfgLine, "%*s %s", objName);
            kinematics = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "mouse"))
        {
            sscanf(cfgLine, "%*s %s", objName);
            mouse = (vsMouse *)(findObject(objName));
        }
        else if (!strcmp(token, "horizontalAxis"))
        {
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);
            inputDev = (vsInputDevice *)(findObject(objName));
            if (inputDev)
                horizAxis = inputDev->getAxis(objNum);
        }
        else if (!strcmp(token, "verticalAxis"))
        {
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);
            inputDev = (vsInputDevice *)(findObject(objName));
            if (inputDev)
                vertiAxis = inputDev->getAxis(objNum);
        }
        else if (!strcmp(token, "xyButton"))
        {
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);
            inputDev = (vsInputDevice *)(findObject(objName));
            if (inputDev)
                xyBtn = inputDev->getButton(objNum);
        }
        else if (!strcmp(token, "zButton"))
        {
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);
            inputDev = (vsInputDevice *)(findObject(objName));
            if (inputDev)
                zBtn = inputDev->getButton(objNum);
        }
        else if (!strcmp(token, "rotateButton"))
        {
            sscanf(cfgLine, "%*s %s %d", objName, &objNum);
            inputDev = (vsInputDevice *)(findObject(objName));
            if (inputDev)
                rotBtn = inputDev->getButton(objNum);
        }
        else if (!strcmp(token, "translateSpeed"))
            sscanf(cfgLine, "%*s %lf", &translate);
        else if (!strcmp(token, "rotateSpeed"))
            sscanf(cfgLine, "%*s %lf", &rotate);
        else
            printf("vsAvatar::makeVsTrackballMotion: Unrecognized token "
                "'%s'\n", token);
    }
    
    if (!kinematics)
    {
        printf("vsAvatar::makeVsTrackballMotion: Kinematics object not "
            "specified\n");
        return NULL;
    }

    if (mouse)
        result = new vsTrackballMotion(mouse, kinematics);
    else if (horizAxis && vertiAxis)
        result = new vsTrackballMotion(horizAxis, vertiAxis, xyBtn, zBtn,
            rotBtn, kinematics);
    else
    {
        printf("vsAvatar::vsTrackballMotion: No mouse or insufficient "
            "control axes specified\n");
        return NULL;
    }

    result->setTranslationConstant(translate);
    result->setRotationConstant(rotate);
    
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsTrackedMotion from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
void *vsAvatar::makeVsTrackedMotion()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    vsKinematics *kinematics = NULL;
    char objName[256];
    vsTrackingSystem *tsys;
    vsMotionTracker *tracker = NULL;
    int trackerNum;
    int posEnable = 1;
    int oriEnable = 1;
    vsVector posOffset(0.0, 0.0, 0.0);
    vsQuat oriOffset(0.0, 0.0, 0.0, 1.0);
    double a, b, c;
    double posScale = 1.0;
    vsTrackedMotion *result;

    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "kinematics"))
        {
            sscanf(cfgLine, "%*s %s", objName);
            kinematics = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "tracker"))
        {
            sscanf(cfgLine, "%*s %s %d", objName, &trackerNum);
            tsys = (vsTrackingSystem *)(findObject(objName));
            if (tsys)
                tracker = tsys->getTracker(trackerNum);
        }
        else if (!strcmp(token, "positionEnable"))
            sscanf(cfgLine, "%*s %d", &posEnable);
        else if (!strcmp(token, "orientationEnable"))
            sscanf(cfgLine, "%*s %d", &oriEnable);
        else if (!strcmp(token, "positionOffset"))
        {
            sscanf(cfgLine, "%*s %lf %lf %lf", &a, &b, &c);
            posOffset.set(a, b, c);
        }
        else if (!strcmp(token, "orientationOffset"))
        {
            sscanf(cfgLine, "%*s %lf %lf %lf", &a, &b, &c);
            oriOffset.setEulerRotation(VS_EULER_ANGLES_ZXY_R, a, b, c);
        }
        else if (!strcmp(token, "positionScale"))
            sscanf(cfgLine, "%*s %lf", &posScale);
        else
            printf("vsAvatar::makeVsTrackedMotion: Unrecognized token "
                "'%s'\n", token);
    }
    
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

    result = new vsTrackedMotion(tracker, kinematics);
    
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
    
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsWalkArticulation from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
void *vsAvatar::makeVsWalkArticulation()
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
    
    dataFilename[0] = 0;
    for (loop = 0; loop < VS_WALK_ARTIC_JOINT_COUNT; loop++)
        jointKins[loop] = NULL;

    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "kinematics"))
        {
            sscanf(cfgLine, "%*s %s", objName);
            kinematics = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "datafile"))
            sscanf(cfgLine, "%*s %s", dataFilename);
        else if (!strcmp(token, "jointKinematics"))
        {
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

    result = new vsWalkArticulation(kinematics, dataFilename);
    
    for (loop = 0; loop < VS_WALK_ARTIC_JOINT_COUNT; loop++)
        if (jointKins[loop])
            result->setJointKinematics(loop, jointKins[loop]);
    
    return result;
}

// ------------------------------------------------------------------------
// Protected function
// Creates a vsWalkInPlace from data in the configuration file, and
// returns a pointer to it.
// ------------------------------------------------------------------------
void *vsAvatar::makeVsWalkInPlace()
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
    int forwardEnable = 1;
    int backEnable = 1;
    int sideEnable = 1;
    double forwardSpeed = VS_WIP_DEFAULT_FWD_SPD;
    double backSpeed = VS_WIP_DEFAULT_BCK_SPD;
    double sideSpeed = VS_WIP_DEFAULT_SS_SPD;
    double forwardThresh = VS_WIP_DEFAULT_FWD_THRESH;
    double backThresh = VS_WIP_DEFAULT_BCK_THRESH;
    double sideThresh = VS_WIP_DEFAULT_SS_THRESH;
    double moveAllow = VS_WIP_DEFAULT_ALLOWANCE;
    int moveLimitEnable = VS_TRUE;
    vsWalkInPlace *result;
    
    for (loop = 0; loop < 3; loop++)
        trackers[loop] = NULL;

    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
        
        if (!strcmp(token, "kinematics"))
        {
            sscanf(cfgLine, "%*s %s", objName);
            kinematics = (vsKinematics *)(findObject(objName));
        }
        else if (!strcmp(token, "tracker"))
        {
            sscanf(cfgLine, "%*s %d %s %d", &whichJoint, objName,
                &trackerNum);
            tsys = (vsTrackingSystem *)(findObject(objName));
            if (tsys && (whichJoint >= 0) && (whichJoint < 3))
                trackers[whichJoint] = tsys->getTracker(trackerNum);
        }
        else if (!strcmp(token, "forwardEnable"))
            sscanf(cfgLine, "%*s %d", &forwardEnable);
        else if (!strcmp(token, "backwardEnable"))
            sscanf(cfgLine, "%*s %d", &backEnable);
        else if (!strcmp(token, "sidestepEnable"))
            sscanf(cfgLine, "%*s %d", &sideEnable);
        else if (!strcmp(token, "forwardSpeed"))
            sscanf(cfgLine, "%*s %lf", &forwardSpeed);
        else if (!strcmp(token, "backwardSpeed"))
            sscanf(cfgLine, "%*s %lf", &backSpeed);
        else if (!strcmp(token, "sidestepSpeed"))
            sscanf(cfgLine, "%*s %lf", &sideSpeed);
        else if (!strcmp(token, "forwardThreshold"))
            sscanf(cfgLine, "%*s %lf", &forwardThresh);
        else if (!strcmp(token, "backwardThreshold"))
            sscanf(cfgLine, "%*s %lf", &backThresh);
        else if (!strcmp(token, "sidestepThreshold"))
            sscanf(cfgLine, "%*s %lf", &sideThresh);
        else if (!strcmp(token, "moveAllowance"))
            sscanf(cfgLine, "%*s %lf", &moveAllow);
        else if (!strcmp(token, "moveLimitEnable"))
            sscanf(cfgLine, "%*s %d", &moveLimitEnable);
        else
            printf("vsAvatar::makeVsWalkInPlace: Unrecognized token '%s'\n",
                token);
    }
    
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

    result = new vsWalkInPlace(trackers[0], trackers[1], trackers[2],
        kinematics);

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
    
    return result;
}
