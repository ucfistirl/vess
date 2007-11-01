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
//    VESS Module:  vsHandCollision.c++
//
//    Description:  A class to create an manage an array of spherical
//                  sensors used to detect collisions between a human
//                  hand model and the environment.  Includes support
//                  for detecting a grasp of an object.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#include "vsHandCollision.h++"
#include <stdio.h>

// ------------------------------------------------------------------------
// Creates a hand collision object using the given configuration file for
// scene information.  All named nodes are looked up under the given 
// component.
// ------------------------------------------------------------------------
vsHandCollision::vsHandCollision(char *configFileName, vsComponent *scene)
{
    FILE *fp;

    // Open the config file
    fp = fopen(configFileName, "r");
    if (fp == NULL)
    {
        printf("vsHandCollision::vsHandCollision:\n");
        printf("    Unable to open configuration file %s\n", configFileName);
    }
    else
    {
        // Read in the configuration data
        sceneComp = scene;
        loadConfiguration(fp);

        // Close the file
        fclose(fp);

        // Finish initialization
        init();
    }
}

// ------------------------------------------------------------------------
// Creates a hand collision object using the given configuration 
// parameters.  The handSeg parameter can be NULL if no collision 
// highlighting is desired.  Otherwise, it should specify a vsComponent
// above the hand geometry corresponding to the sensor at the same position
// in the sensor array.
// ------------------------------------------------------------------------
vsHandCollision::vsHandCollision(int nSensors, vsComponent *scene, 
                                 vsComponent *hand,
                                 vsComponent *sensors[], double radius[],
                                 int thumbMin, int thumbMax,
                                 vsComponent *handSeg[])
{
    // Initialize the array class members
    memset(sensorComp, 0, sizeof(sensorComp));
    memset(sensorRadius, 0, sizeof(sensorRadius));
    memset(handSegment, 0, sizeof(handSegment));

    // Copy the configuration data
    numSensors = nSensors; 
    sceneComp = scene;
    handComp = hand;
    memcpy(sensorComp, sensors, sizeof(vsComponent *) * numSensors);
    memcpy(sensorRadius, radius, sizeof(double) * numSensors);
    firstThumbSensor = thumbMin;
    lastThumbSensor = thumbMax;
    if (handSeg != NULL)
        memcpy(handSegment, handSeg, sizeof(vsComponent *) * numSensors);

    // Finish initialization
    init();
}

// ------------------------------------------------------------------------
// Common initialization function (used by both constructors)
// ------------------------------------------------------------------------
void vsHandCollision::init()
{
    int i;
    vsMaterialAttribute *mat;

    // Create sensor, thumb, and finger masks.  These are used to check the 
    // collision state vs. a subset of sensors
    sensorMask = 0;
    for (i = 0; i < numSensors; i++)
        sensorMask |= (1 << i);

    thumbMask = 0;
    for (i = firstThumbSensor; i <= lastThumbSensor; i++)
    {
        thumbMask |= (1 << i);
    }
    
    fingerMask = (sensorMask & ~thumbMask);

    printf("thumb sensors = %d to %d\n", firstThumbSensor, lastThumbSensor);
    printf("sensorMask = 0x%08X\n", sensorMask);
    printf("thumbMask = 0x%08X\n", thumbMask);
    printf("fingerMask = 0x%08X\n", fingerMask);

    // Create the vsSphereIntersect object
    sphIsect = new vsSphereIntersect();
    sphIsect->setSphereListSize(numSensors);
    sphIsect->enablePaths();
    sphIsect->setMask(VS_HC_DEFAULT_ISECT_MASK);

    // Create the highlighting material
    highlightMaterial = new vsMaterialAttribute();
    highlightMaterial->setName("highlight");
    highlightMaterial->setColor(VS_MATERIAL_SIDE_BOTH, 
        VS_MATERIAL_COLOR_AMBIENT, 1.0, 0.0, 0.0);
    highlightMaterial->setColor(VS_MATERIAL_SIDE_BOTH,
        VS_MATERIAL_COLOR_DIFFUSE, 1.0, 0.0, 0.0);
    highlightMaterial->setColor(VS_MATERIAL_SIDE_BOTH, 
        VS_MATERIAL_COLOR_SPECULAR, 0.0, 0.0, 0.0);
    highlightMaterial->setColor(VS_MATERIAL_SIDE_BOTH, 
        VS_MATERIAL_COLOR_EMISSIVE, 0.0, 0.0, 0.0);
    highlightMaterial->setOverride(true);

    // Scan the hand segments for existing materials and save them if
    // they exist.  We'll need to swap these with the highlight material
    // when highlighting is enabled.
    memset(oldMaterial, 0, sizeof(oldMaterial));
    for (i = 0; i < numSensors; i++)
    {
        mat = (vsMaterialAttribute *)handSegment[i]->getTypedAttribute(
            VS_ATTRIBUTE_TYPE_MATERIAL, 0);

        if (mat)
            oldMaterial[i] = mat;
    }

    // Start with highlighting disabled
    highlightEnabled = false;
}

// ------------------------------------------------------------------------
// Destructor.  Cleans up the objects created by this class.
// ------------------------------------------------------------------------
vsHandCollision::~vsHandCollision()
{
    int i;

    // Remove the highlight material from all the hand segments (if any)
    for (i = 0; i < numSensors; i++)
    {
        if ((handSegment[i] != NULL) &&
            (handSegment[i]->getNamedAttribute("highlight")))
        {
            handSegment[i]->removeAttribute(highlightMaterial);
        }
    }

    // Delete the material and the intersection objects
    delete highlightMaterial;
    delete sphIsect;
}

// ------------------------------------------------------------------------
// Helper method to load the configuration information from the given file.
// ------------------------------------------------------------------------
void vsHandCollision::loadConfiguration(FILE *fp)
{
    char line[200];
    char *token;
    char *ch;
    bool numSensorsSet;
    int sensorCount;
    int index;
    char centerCompName[VS_HC_MAX_SENSORS][100];
    char sideCompName[VS_HC_MAX_SENSORS][100];
    char segmentCompName[VS_HC_MAX_SENSORS][100];
    vsNode *searchNode;
    vsComponent *sideComp;
    atVector center, side;
    atMatrix globalMat;
    int i;

    // Initialize error-checking variables
    numSensorsSet = false;
    sensorCount = 0;

    // Initialize temporary arrays
    memset(centerCompName, 0, sizeof(centerCompName));
    memset(sideCompName, 0, sizeof(sideCompName));
    memset(segmentCompName, 0, sizeof(segmentCompName));

    // Initialize data members
    handComp = NULL;
    firstThumbSensor = -1;
    lastThumbSensor = -1;
    memset(sensorComp, 0, sizeof(sensorComp));
    memset(sensorRadius, 0, sizeof(sensorRadius));
    memset(handSegment, 0, sizeof(handSegment));

    // Read the file one line at a time
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        // Remove the newline from the string
        ch = strchr(line, '\n');
        if (ch != NULL)
            *ch = 0;

        // Skip commented lines
        if (line[0] != '#')
        {
            // Get the first token from the line, and see if it is a "set"
            // command
            token = strtok(line, " ");
            if (strcmp(token, "set") == 0)
            {
                // Get the next token
                token = strtok(NULL, " ");

                // See what's being set
                if (strcmp(token, "numsensors") == 0)
                {
                    // Get the next token, which should be the number of
                    // sensors
                    token = strtok(NULL, " ");

                    // See if we got a token or not
                    if (token != NULL)
                    {
                        // Convert the token to an integer value
                        numSensors = atoi(token);

                        // Do range checking on the sensor count
                        if (numSensors < 0)
                            numSensors = 0;
                        if (numSensors > VS_HC_MAX_SENSORS)
                            numSensors = VS_HC_MAX_SENSORS;

                        // Set the flag indicating the number of sensors was
                        // set (required for a valid configuration)
                        numSensorsSet = true;
                    }
                    else
                    {
                        printf("vsHandCollision::loadConfiguration:\n");
                        printf("    Invalid number of sensors\n");
                    }
                }
                else if (strcmp(token, "hand") == 0)
                {
                    // Get the name of the hand's component
                    token = strtok(NULL, " ");

                    // See if we got a valid token from the line
                    if (token != NULL)
                    {
                        // Look for the node with the given name in the
                        // scene
                        searchNode = sceneComp->findNodeByName(token);

                        // Check the node for validity
                        if ((searchNode != NULL) && 
                            (searchNode->getNodeType() == 
                                VS_NODE_TYPE_COMPONENT))
                        {
                            handComp = (vsComponent *)searchNode;
                        }
                    }
                }
                else if (strcmp(token, "firstthumb") == 0)
                {
                    // Get the next token, which should be the number of
                    // sensors
                    token = strtok(NULL, " ");

                    printf("firstthumb token:  %s", token);
                    // See if we got a token or not
                    if (token != NULL)
                    {
                        // Convert the token to an integer value
                        firstThumbSensor = atoi(token);
                        printf("  value = %d\n", firstThumbSensor);
                    }
                    else
                    {
                        printf("vsHandCollision::loadConfiguration:\n");
                        printf("    Invalid first thumb sensor index\n");
                    }
                }
                else if (strcmp(token, "lastthumb") == 0)
                {
                    // Get the next token, which should be the number of
                    // sensors
                    token = strtok(NULL, " ");

                    printf("lastthumb token:  %s", token);
                    // See if we got a token or not
                    if (token != NULL)
                    {
                        // Convert the token to an integer value
                        lastThumbSensor = atoi(token);
                        printf("  value = %d\n", lastThumbSensor);
                    }
                    else
                    {
                        printf("vsHandCollision::loadConfiguration:\n");
                        printf("    Invalid last thumb sensor index\n");
                    }
                }
                else if (strcmp(token, "sensor") == 0)
                {
                    // Get the next token (the sensor index)
                    token = strtok(NULL, " ");

                    if (token != NULL)
                    {
                        // Convert the sensor index
                        index = atoi(token);

                        // Do range checking, and also check to see if
                        // we've already configured the given sensor.
                        // Skip this line if so.
                        if ((index < 0) || (index > VS_HC_MAX_SENSORS) ||
                            (centerCompName[index] != NULL))
                        {
                            // Get the center component name
                            token = strtok(NULL, " ");
                            if (token != NULL)
                                strcpy(centerCompName[index], token);

                            // Get the side point component name
                            token = strtok(NULL, " ");
                            if (token != NULL)
                                strcpy(sideCompName[index], token);
                        }
                    }
                }
                else if (strcmp(token, "segment") == 0)
                {
                    // Get the next token (the sensor index)
                    token = strtok(NULL, " ");

                    if (token != NULL)
                    {
                        // Convert the segment index
                        index = atoi(token);

                        // Do range checking, and also check to see if
                        // we've already configured the given segment.
                        // Skip this line if so.
                        if ((index < 0) || (index > VS_HC_MAX_SENSORS) ||
                            (segmentCompName[index] != NULL))
                        {
                            // Get the segment component name
                            token = strtok(NULL, " ");
                            if (token != NULL)
                                strcpy(segmentCompName[index], token);
                        }
                    }
                }
            }
        }
    }

    // Check to see if we got a valid sensor count and hand component
    if ((!numSensorsSet) || (handComp == NULL))
    {
        printf("vsHandCollision::loadConfiguration:\n");
        printf("   No sensors or hand component specified, unable to ");
        printf("continue!\n");
        return;
    }

    // Clamp the thumb sensor settings to the maximum number of sensors
    if (firstThumbSensor > numSensors)
        firstThumbSensor = numSensors;
    if (lastThumbSensor > numSensors)
        lastThumbSensor = numSensors;
    
    // Search the hand subgraph for the sensor and segment components
    for (i = 0; i < numSensors; i++)
    {
        // Get the center of the sphere
        searchNode = handComp->findNodeByName(centerCompName[i]);
        if ((searchNode != NULL) && 
            (searchNode->getNodeType() == VS_NODE_TYPE_COMPONENT))
        {
            // Cast the node to a component and remember it
            sensorComp[i] = (vsComponent *)searchNode;

            // Get a point on the side of the sphere
            searchNode = handComp->findNodeByName(sideCompName[i]);
            if ((searchNode != NULL) && 
                (searchNode->getNodeType() == VS_NODE_TYPE_COMPONENT))
            {
                // Cast the node to a temporary component
                sideComp = (vsComponent *)searchNode;
    
                // Compute the radius of the sphere in world coordinates
                sensorComp[i]->getBoundSphere(&center, NULL);
                sideComp->getBoundSphere(&side, NULL);
                globalMat = sensorComp[i]->getGlobalXform();

                // Compute and store the radius of the sensor in world-
                // coordinates, this eliminates unnecessary per-frame 
                // computations.  We also need to transform to world 
                // coordinates to account for any scaling of the hand model.
                center = globalMat.getPointXform(center);
                side = globalMat.getPointXform(side);
                sensorRadius[i] = (side - center).getMagnitude();

                // Increment the number of sensors found.  A valid 
                // configuration must have this count equal the numSensors 
                // parameter.
                sensorCount++;
            }
            else
            {
                // Error finding this sensor
                printf("vsHandCollision::loadConfiguration:\n");
                printf("    Unable to find sensor side '%s'\n",
                    sideCompName[i]);
            }
        }
        else
        {
            // Error finding this sensor
            printf("vsHandCollision::loadConfiguration:\n");
            printf("    Unable to find sensor center '%s'\n",
                centerCompName[i]);
        }

        // Search for the segment component
        searchNode = handComp->findNodeByName(segmentCompName[i]);
        if ((searchNode != NULL) && (searchNode->getNodeType() ==
            VS_NODE_TYPE_COMPONENT))
        {
            // Cast the node to a component and remember it
            handSegment[i] = (vsComponent *)searchNode;
        }
        else
        {
            // Error finding the segment geometry (not fatal)
            printf("vsHandCollision::loadConfiguration:\n");
            printf("    Unable to find hand segment '%s'\n",
                segmentCompName[i]);
        }
    }

    // Check for configuration errors and return failure if any exist
    if (sensorCount != numSensors)
    {
        printf("vsHandCollision::loadConfiguration:\n");
        printf("    One or more sensors not properly configured!\n");
    }
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsHandCollision::getClassName()
{
    return "vsHandCollision";
}

// ------------------------------------------------------------------------
// Set the intersection object's mask to the given mask
// ------------------------------------------------------------------------
void vsHandCollision::setIntersectMask(unsigned long mask)
{
    // Give the new mask to the intersection object
    sphIsect->setMask(mask);
}

// ------------------------------------------------------------------------
// Set the hand's intersection value to the given value
// ------------------------------------------------------------------------
void vsHandCollision::setHandIntersectValue(unsigned long value)
{
    // Apply the new value to the hand component
    handComp->setIntersectValue(value);
}

// ------------------------------------------------------------------------
// Check to see if the given sensor is currently colliding
// ------------------------------------------------------------------------
bool vsHandCollision::isColliding(int sensorIndex)
{
    // Check the collision state to see if the given sensor is colliding
    return (collisionState & (1 << sensorIndex));
}

// ------------------------------------------------------------------------
// Check to see if the hand is grasping the given object.  This is true if 
// any thumb sensor and any other sensor are intersecting it.
// ------------------------------------------------------------------------
bool vsHandCollision::isGraspingObject(vsComponent *object)
{
    bool thumbFlag, fingerFlag;
    int sensor, node;
    vsGrowableArray *sensorPath;

    // Initialize the thumb and finger flags to indicate that they are not
    // touching the object
    thumbFlag = false;
    fingerFlag = false;

    // Check the collision state with the finger and thumb masks and see
    // if at least one sensor in both sets are colliding.
    if ((collisionState & thumbMask) && (collisionState & fingerMask))
    {
        // Iterate over the thumb sensors until we find a sensor that
        // meets our criteria, or we run out
        sensor = firstThumbSensor;
        thumbFlag = false;
        while ((!thumbFlag) && (sensor <= lastThumbSensor))
        {
            if (collisionState & (1 << sensor))
            {
                // Get the traversal path from the intersection object
                sensorPath = sphIsect->getIsectPath(sensor);

                // Scan the path array until we run out of nodes, or we find
                // the object's node
                node = 0;
                while ((!thumbFlag) && (sensorPath->getData(node) != NULL))
                {
                    // If the current path node matches the object's node
                    // the thumb is touching the object in question
                    if (sensorPath->getData(node) == (void *)object)
                        thumbFlag = true;

                    // Move on to the next node
                    node++;
                }
            }

            // Move on to the next sensor
            sensor++;
        }

        // Iterate over the finger sensors until we find a sensor that
        // meets our criteria, or we run out
        sensor = 0;
        fingerFlag = false;
        while ((!fingerFlag) && (sensor <= numSensors))
        {
            // If we've hit the thumb sensor range, skip past it
            if (sensor == firstThumbSensor)
            {
                sensor += lastThumbSensor - firstThumbSensor;
            }
            else
            {
                if (collisionState & (1 << sensor))
                {
                    // Get the traversal path from the intersection object
                    sensorPath = sphIsect->getIsectPath(sensor);

                    // Scan the path array until we run out of nodes, or we find
                    // the object's node
                    node = 0;
                    while ((!fingerFlag) && (sensorPath->getData(node) != NULL))
                    {
                        // If the current path node matches the object's node
                        // the thumb is touching the object in question
                        if (sensorPath->getData(node) == (void *)object)
                            fingerFlag = true;

                        // Move on to the next node
                        node++;
                    }
                }
            }

            // Move on to the next sensor
            sensor++;
        }
    }

    // If both thumb and finger flags are set, the object is grasped
    return (thumbFlag && fingerFlag);
}

// ------------------------------------------------------------------------
// Pass-through function for low-level access to the intersection valid
// flag
// ------------------------------------------------------------------------
bool vsHandCollision::getIsectValid(int sensorIndex)
{
    return sphIsect->getIsectValid(sensorIndex);
}

// ------------------------------------------------------------------------
// Pass-through function for low-level access to the intersection point
// ------------------------------------------------------------------------
atVector vsHandCollision::getIsectPoint(int sensorIndex)
{
    return sphIsect->getIsectPoint(sensorIndex);
}

// ------------------------------------------------------------------------
// Pass-through function for low-level access to the intersection normal
// ------------------------------------------------------------------------
atVector vsHandCollision::getIsectNorm(int sensorIndex)
{
    return sphIsect->getIsectNorm(sensorIndex);
}

// ------------------------------------------------------------------------
// Pass-through function for low-level access to the intersection transform
// ------------------------------------------------------------------------
atMatrix vsHandCollision::getIsectXform(int sensorIndex)
{
    return sphIsect->getIsectXform(sensorIndex);
}

// ------------------------------------------------------------------------
// Pass-through function for low-level access to the intersected primitive
// index
// ------------------------------------------------------------------------
vsGeometry *vsHandCollision::getIsectGeometry(int sensorIndex)
{
    return sphIsect->getIsectGeometry(sensorIndex);
}

// ------------------------------------------------------------------------
// Pass-through function for low-level access to the intersected geometry
// ------------------------------------------------------------------------
int vsHandCollision::getIsectPrimNum(int sensorIndex)
{
    return sphIsect->getIsectPrimNum(sensorIndex);
}

// ------------------------------------------------------------------------
// Pass-through function for low-level access to the intersection path
// ------------------------------------------------------------------------
vsGrowableArray *vsHandCollision::getIsectPath(int sensorIndex)
{
    return sphIsect->getIsectPath(sensorIndex);
}

// ------------------------------------------------------------------------
// Return the bit vector indicating the current collision state
// ------------------------------------------------------------------------
unsigned long vsHandCollision::getCollisionState()
{
    return collisionState;
}

// ------------------------------------------------------------------------
// Turns on highlighting of colliding geometry
// ------------------------------------------------------------------------
void vsHandCollision::enableHighlighting()
{
    highlightEnabled = true;
}

// ------------------------------------------------------------------------
// Turns off highlighting of colliding geometry
// ------------------------------------------------------------------------
void vsHandCollision::disableHighlighting()
{
    highlightEnabled = false;
}

// ------------------------------------------------------------------------
// Performs intersection traversal and updates the collision state of
// all hand sensors.
// ------------------------------------------------------------------------
void vsHandCollision::update()
{
    atMatrix globalTransform;
    atVector center, side;
    int i;
    bool hit;

    // Adjust the sensor positions
    for (i = 0; i < numSensors; i++)
    {
        sensorComp[i]->getBoundSphere(&center, NULL);

        // Get the global transform of the given sensor
        globalTransform = sensorComp[i]->getGlobalXform();

        // Transform the center point by the global transform
        center = globalTransform.getPointXform(center);

        // Put the sensor's new settings in the intersection object
        sphIsect->setSphere(i, center, sensorRadius[i]);
    }

    // Initialize the collision state
    collisionState = 0;

    // Do the intersection traversal
    sphIsect->intersect(sceneComp);

    // Collect the results
    for (i = 0; i < numSensors; i++)
    {
        // Check for collisions
        if (sphIsect->getIsectValid(i))
        {
            // Set the collision flag to true
            hit = true;

            // Set the corresponding bit to indicate a collision for the
            // specific sensor
            collisionState |= (1 << i);
        }
        else
        {
            // No collision, set the hit flag to false
            hit = false;
        }

        // If the sensor is colliding and highlighting is enabled,
        // add the highlight material to the corresponding hand segment.
        if (hit)
        {
            // Attach the highlight attribute if highlighting is enabled
            if ((highlightEnabled) && 
                (handSegment[i]->getNamedAttribute("highlight") == NULL))
            {
                // Remove the existing material, if there is one
                if (oldMaterial[i] != NULL)
                    handSegment[i]->removeAttribute(oldMaterial[i]);

                handSegment[i]->addAttribute(highlightMaterial);
            }
        }
        else
        {
            // Detach the highlight attribute if it's attached
            if (handSegment[i]->getNamedAttribute("highlight"))
            {
                handSegment[i]->removeAttribute(highlightMaterial);

                // Reattach the original material if one existed
                if (oldMaterial[i] != NULL)
                    handSegment[i]->addAttribute(oldMaterial[i]);
            }
        }
    }
}
