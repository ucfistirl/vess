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

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsAvatar::vsAvatar()
{
    cfgFile = NULL;
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
    vsGrowableArray objectArray(10, 10);
    vsGrowableArray stringArray(10, 10);
    int objectCount;
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
        setup(&objectArray, &stringArray, 0);
        cfgFile = NULL;
        return;
    }

    cfgFile = fopen(configFile, "r");
    if (!cfgFile)
    {
        printf("vsAvatar::init: Unable to open configuration file %s\n",
            configFile);
        return;
    }
    
    objectCount = 0;

    lineType = 0;
    while (lineType != -1)
    {
        lineType = readCfgLine(lineBuffer);
        if (lineType != 1)
            continue;
        
        sscanf(lineBuffer, "%s %s", objectType, objectName);
        
        newObject = createObject(objectType);
        
        objectArray[objectCount] = newObject;
        stringArray[objectCount] = strdup(objectName);
        objectCount++;
    }
    
    setup(&objectArray, &stringArray, objectCount);
    
    // Clean up the string objects
    for (loop = 0; loop < objectCount; loop++)
	free(stringArray[loop]);
    
    fclose(cfgFile);
    cfgFile = NULL;
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
    
    return NULL;
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
        }
        else if (!strcmp(token, "filename"))
            sscanf(cfgLine, "%*s %s", dbName);
        else if (!strcmp(token, "optimize"))
            sscanf(cfgLine, "%*s %d", &optFlag);
	else if (!strcmp(token, "addpath"))
	{
	    sscanf(cfgLine, "%*s %s", strValue);
	    dbLoader->addPath(strValue);
	}
    }
    
    if (strlen(dbName) > 0)
    {
        result = dbLoader->loadDatabase(dbName);
        if (result && optFlag)
        {
            optimizer = new vsOptimizer;
            optimizer->optimize(result);
            delete optimizer;
        }
    }
    
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
    }
    
    if (portNumber == -1)
        return NULL;

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
    }
    
    if (portNumber == -1)
        return NULL;

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
	}
    }
    
    if (portNumbers[0] == -1)
        return NULL;

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
	}
    }
    
    if (portNumbers[0] == -1)
        return NULL;

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
    }
    
    if (portNumber == -1)
        return NULL;

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
    }
    
    if (portNumber == -1)
        return NULL;

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
	}
    }

    if (portNumber == -1)
        return NULL;
    if (strlen(serverName) < 1)
        return NULL;

    result = new vsEthernetMotionStar(serverName, portNumber, nTrackers,
        masterFlag, dataFormat);

    if (hemisphere != -1)
	result->setActiveHemisphere(VS_MSTAR_ALL_TRACKERS, hemisphere);

    if (forkFlag)
        result->forkTracking();

    return result;
}
