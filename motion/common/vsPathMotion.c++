// ------------------------------------------------------------------------
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
// ------------------------------------------------------------------------
//
//    VESS Module:  vsPathMotion.c++
//
//    Description:  Motion model that moves an object through a specified
//                  set of key positions and orientations
//
//    Author(s):    Bryan Kline
//
// ------------------------------------------------------------------------

#include "vsPathMotion.h++"

#include "vsTimer.h++"

// ------------------------------------------------------------------------
// Constructor
// Initializes private variables
// ------------------------------------------------------------------------
vsPathMotion::vsPathMotion(vsKinematics *kinematics)
{
    // Reference the kinematics object
    objectKin = kinematics;
    objectKin->ref();

    // Set current play status to STOPPED
    currentPlayMode = VS_PATH_STOPPED;

    // Set default interpolation modes to linear
    posMode = VS_PATH_POS_IMODE_LINEAR;
    oriMode = VS_PATH_ORI_IMODE_SLERP;

    // Set default cycling mode to restart, set target iterations to one,
    // and initialize the current iteration to zero
    cycleMode = VS_PATH_CYCLE_RESTART;
    cycleCount = 1;
    currentCycleCount = 0;

    // Set ROUNDED default radius
    roundCornerRadius = 1.0;

    // Set default point to look at
    lookPoint.set(0.0, 0.0, 0.0);

    // Set default up direction (none)
    upDirection.set(0.0, 0.0, 0.0);

    // Set the current number of key points to zero; the segmentList
    // growable array has already been initialized
    pointCount = 0;

    // Set the 'current' values to zeroed defaults
    currentPos.set(0.0, 0.0, 0.0);
    currentOri.set(0.0, 0.0, 0.0, 1.0);

    // Initialize the path information values
    currentSegmentIdx = 0;
    currentSegmentTime = 0.0;
    totalTime = 0.0;
    totalPathTime = 0.0;
}

// ------------------------------------------------------------------------
// Copy constructor.
// ------------------------------------------------------------------------
vsPathMotion::vsPathMotion(vsPathMotion *original)
{
    int index;
    vsPathMotionSegment *tempSeg;
    vsPathMotionSegment *tempNewSeg;
    
    // Copy the kinematics pointer over. (The PathMotions will reference
    // the same kinematics object.)
    objectKin = original->objectKin;
    objectKin->ref();
   
    // Copy the play mode
    currentPlayMode = original->currentPlayMode;
    
    // Copy the position and orientation interpolation modes
    posMode = original->posMode;
    oriMode = original->oriMode;
    
    // Copy the path cycling settings
    cycleMode = original->cycleMode;
    cycleCount = original->cycleCount;
    currentCycleCount = original->currentCycleCount;
    
    // Copy the radius for rounded corner interpolation
    roundCornerRadius = original->roundCornerRadius;
    
    // Copy the look point and up directions for look-at orienation
    // interpolation
    lookPoint = original->lookPoint;
    upDirection = original->upDirection;
    
    // Set up the point list with the correct number of points (we'll
    // replace the actual point entries below)
    pointCount = 0;
    setPointListSize(original->pointList.getNumEntries()); 

    // Copy the current position and orientation
    currentPos = original->currentPos;
    currentOri = original->currentOri;
    
    // Copy the current segment and time information
    currentSegmentIdx = original->currentSegmentIdx;
    currentSegmentTime = original->currentSegmentTime;
    totalTime = original->totalTime;
    totalPathTime = original->totalPathTime;
    
    // Copy the data for each path point
    for(index = 0; index < pointCount; index++)
    {
        // Get the segment data structure from the original path motion.
        tempSeg = (vsPathMotionSegment*) (original->pointList.getEntry(index));
        
        // Make sure the segment is not NULL. If it is, we've reached the end
        // of the list, so break out of this loop.
        if (tempSeg == NULL)
        {
            break;
        }
        
        // Clone the point data at this segment, and put it in our list
        // at the correct position (this will replace the empty point that
        // we created above)
        tempNewSeg = tempSeg->clone();
        pointList.setEntry(index, tempNewSeg);
    }
}
            
// ------------------------------------------------------------------------
// Destructor
// Deletes privately allocated variables
// ------------------------------------------------------------------------
vsPathMotion::~vsPathMotion()
{
    // Unreference the kinematics object, and delete it if it is no
    // longer in use
    vsObject::unrefDelete(objectKin);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsPathMotion::getClassName()
{
    return "vsPathMotion";
}

// ------------------------------------------------------------------------
// Sets the position interpolation mode
// ------------------------------------------------------------------------
void vsPathMotion::setPositionMode(int mode)
{
    posMode = mode;
}

// ------------------------------------------------------------------------
// Gets the position interpolation mode
// ------------------------------------------------------------------------
int vsPathMotion::getPositionMode()
{
    return posMode;
}

// ------------------------------------------------------------------------
// Sets the orientation interpolation mode
// ------------------------------------------------------------------------
void vsPathMotion::setOrientationMode(int mode)
{
    oriMode = mode;
}

// ------------------------------------------------------------------------
// Gets the orientation interpolation mode
// ------------------------------------------------------------------------
int vsPathMotion::getOrientationMode()
{
    return oriMode;
}

// ------------------------------------------------------------------------
// Sets the path repetition mode
// ------------------------------------------------------------------------
void vsPathMotion::setCycleMode(int mode)
{
    cycleMode = mode;
}

// ------------------------------------------------------------------------
// Sets the number of times to cycle through the path before stopping.
// The constant VS_PATH_REPEAT_FOREVER directs the path to run an infinite
// number of times.
// ------------------------------------------------------------------------
void vsPathMotion::setCycleCount(int cycles)
{
    if (cycleCount >= 0)
        cycleCount = cycles;
    else
        printf("vsPathMotion::setCycleCount: Invalid cycle count (%d)\n",
            cycles);
}

// ------------------------------------------------------------------------
// Gets the path repetition mode
// ------------------------------------------------------------------------
int vsPathMotion::getCycleMode()
{
    return cycleMode;
}

// ------------------------------------------------------------------------
// Gets the number of times to cycle through the path before stopping.
// A return value of VS_PATH_REPEAT_FOREVER indicates that the path is set
// to run indefinitely.
// ------------------------------------------------------------------------
int vsPathMotion::getCycleCount()
{
    return cycleCount;
}

// ------------------------------------------------------------------------
// Sets the radius of the circular arcs used for the corners of the path
// when in position ROUNDED mode
// ------------------------------------------------------------------------
void vsPathMotion::setCornerRadius(double radius)
{
    if (radius >= 0.0)
        roundCornerRadius = radius;
}

// ------------------------------------------------------------------------
// Gets the radius of the circular arcs used for the corners of the path
// when in position ROUNDED mode
// ------------------------------------------------------------------------
double vsPathMotion::getCornerRadius()
{
    return roundCornerRadius;
}

// ------------------------------------------------------------------------
// Sets the point to keep the orientation pointed towards when in ATPOINT
// orientation interpolation mode
// ------------------------------------------------------------------------
void vsPathMotion::setLookAtPoint(atVector point)
{
    lookPoint.clearCopy(point);
}

// ------------------------------------------------------------------------
// Gets the point to keep the orientation pointed towards when in ATPOINT
// orientation interpolation mode
// ------------------------------------------------------------------------
atVector vsPathMotion::getLookAtPoint()
{
    return lookPoint;
}

// ------------------------------------------------------------------------
// Sets the 'up' direction for the path, which is used when the
// orientation interpolation mode is ATPOINT or FORWARD, and which
// determines how to calculate the roll component of the orientation,
// which is unspecified in those modes. Passing in a zero vector for the
// up direction causes the path to use the up direction from the previous
// orientation each frame, causing a sort of 'free-rolling' effect.
// ------------------------------------------------------------------------
void vsPathMotion::setUpDirection(atVector up)
{
    upDirection.clearCopy(up);
}

// ------------------------------------------------------------------------
// Gets the 'up' direction for the path
// ------------------------------------------------------------------------
atVector vsPathMotion::getUpDirection()
{
    return upDirection;
}

// ------------------------------------------------------------------------
// Resizes the list of key points to the specified size. This method
// should be called before any calls to modify the points themselves.
// ------------------------------------------------------------------------
void vsPathMotion::setPointListSize(int size)
{
    vsPathMotionSegment *segData;

    // Bounds checking
    if (size < 0)
    {
        printf("vsPathMotion::setPointListSize: 'size' parameter (%d) "
            "out of bounds\n", size);
        return;
    }

    // Compare the requested size of the point list to the current one;
    // don't do anything if the requested size is the same as the current
    // size.
    if (size > pointCount)
    {
        while (pointCount < size)
        {
            // Create a new segment structure and set it to defaults
            segData = new vsPathMotionSegment();
            segData->setPosition(atVector(0.0, 0.0, 0.0));
            segData->setOrientation(atQuat(0.0, 0.0, 0.0, 1.0));
            segData->setTravelTime(1.0);
            segData->setPauseTime(0.0);

            // Add the new structure to the list
            pointList.setEntry(pointCount, segData);

            // Increment the point counter
            pointCount++;
        }
    }
    else if (size < pointCount)
    {
        // Decrease the size of the point list, deleting the segment
        // structures that are getting discarded.
        while (pointCount > size)
        {
            // Remove the segment at the appropriate index
            pointList.removeEntryAtIndex(pointCount-1);

            // Decrement the point counter
            pointCount--;
        }
    }
}

// ------------------------------------------------------------------------
// Gets the current size of the point list
// ------------------------------------------------------------------------
int vsPathMotion::getPointListSize()
{
    return pointCount;
}

// ------------------------------------------------------------------------
// Sets the position of one of the key points of the path
// ------------------------------------------------------------------------
void vsPathMotion::setPosition(int point, atVector position)
{
    vsPathMotionSegment *segData;

    // Bounds checking
    if ((point < 0) || (point >= pointCount))
    {
        printf("vsPathMotion::setPosition: 'point' parameter (%d) out of "
            "bounds\n", point);
        return;
    }

    // Get the segment structure from the points array
    segData = (vsPathMotionSegment *)(pointList.getEntry(point));

    // Set the position, forcing the vector size to stay 3
    segData->setPosition(position);
}

// ------------------------------------------------------------------------
// Sets the orientation of the of the key points of the path
// ------------------------------------------------------------------------
void vsPathMotion::setOrientation(int point, atQuat orientation)
{
    vsPathMotionSegment *segData;

    // Bounds checking
    if ((point < 0) || (point >= pointCount))
    {
        printf("vsPathMotion::setOrientation: 'point' parameter (%d) out "
            "of bounds\n", point);
        return;
    }

    // Get the segment structure from the points array
    segData = (vsPathMotionSegment *)(pointList.getEntry(point));

    // Set the orientation
    segData->setOrientation(orientation);
}

// ------------------------------------------------------------------------
// Sets the traversal time for the segment of the path located between
// this point and the next point on the path
// ------------------------------------------------------------------------
void vsPathMotion::setTime(int point, double seconds)
{
    vsPathMotionSegment *segData;

    // Bounds checking
    if ((point < 0) || (point >= pointCount))
    {
        printf("vsPathMotion::setTime: 'point' parameter (%d) out "
            "of bounds\n", point);
        return;
    }

    // Get the segment structure from the points array
    segData = (vsPathMotionSegment *)(pointList.getEntry(point));

    // Set the travel time
    segData->setTravelTime(seconds);
}

// ------------------------------------------------------------------------
// Sets the amount of time to wait at this point before continuing on to
// traverse the next segment of the path. Specifying the constant
// VS_PATH_WAIT_FOREVER for the time causes the path to go into PAUSED
// mode, to wait at this point until a resume call is made.
// ------------------------------------------------------------------------
void vsPathMotion::setPauseTime(int point, double seconds)
{
    vsPathMotionSegment *segData;

    // Bounds checking
    if ((point < 0) || (point >= pointCount))
    {
        printf("vsPathMotion::setPauseTime: 'point' parameter (%d) out "
            "of bounds\n", point);
        return;
    }

    // Get the segment structure from the points array
    segData = (vsPathMotionSegment *)(pointList.getEntry(point));

    // Set the pause time
    segData->setPauseTime(seconds);
}

// ------------------------------------------------------------------------
// Gets the position of one of the key points of the path
// ------------------------------------------------------------------------
atVector vsPathMotion::getPosition(int point)
{
    vsPathMotionSegment *segData;

    // Bounds checking
    if ((point < 0) || (point >= pointCount))
    {
        printf("vsPathMotion::getPosition: 'point' parameter (%d) out of "
            "bounds\n", point);
        return atVector(0.0, 0.0, 0.0);
    }

    // Get the segment structure from the points array
    segData = (vsPathMotionSegment *)(pointList.getEntry(point));

    // Get the position
    return segData->getPosition();
}

// ------------------------------------------------------------------------
// Gets the orientation of the of the key points of the path
// ------------------------------------------------------------------------
atQuat vsPathMotion::getOrientation(int point)
{
    vsPathMotionSegment *segData;

    // Bounds checking
    if ((point < 0) || (point >= pointCount))
    {
        printf("vsPathMotion::getOrientation: 'point' parameter (%d) out "
            "of bounds\n", point);
        return atQuat(0.0, 0.0, 0.0, 0.0);
    }

    // Get the segment structure from the points array
    segData = (vsPathMotionSegment *)(pointList.getEntry(point));

    // Get the orientation
    return segData->getOrientation();
}

// ------------------------------------------------------------------------
// Gets the traversal time for the segment of the path located between
// this point and the next point on the path
// ------------------------------------------------------------------------
double vsPathMotion::getTime(int point)
{
    vsPathMotionSegment *segData;

    // Bounds checking
    if ((point < 0) || (point >= pointCount))
    {
        printf("vsPathMotion::getTime: 'point' parameter (%d) out "
            "of bounds\n", point);
        return 0.0;
    }

    // Get the segment structure from the points array
    segData = (vsPathMotionSegment *)(pointList.getEntry(point));

    // Get the travel time
    return segData->getTravelTime();
}

// ------------------------------------------------------------------------
// Gets the amount of time to wait at this point before continuing on to
// traverse the next segment of the path. The constant
// VS_PATH_WAIT_FOREVER indicates the path should go into PAUSED mode, to
// wait at this point until a resume call is made.
// ------------------------------------------------------------------------
double vsPathMotion::getPauseTime(int point)
{
    vsPathMotionSegment *segData;

    // Bounds checking
    if ((point < 0) || (point >= pointCount))
    {
        printf("vsPathMotion::getPauseTime: 'point' parameter (%d) out "
            "of bounds\n", point);
        return 0.0;
    }

    // Get the segment structure from the points array
    segData = (vsPathMotionSegment *)(pointList.getEntry(point));

    // Set the pause time
    return segData->getPauseTime();
}

// ------------------------------------------------------------------------
// Automatically calculates the amount of time to spend travelling each
// segment of the path, based on the lengths of each of the path segments
// and the total length of the path. Requires that the positions of each
// of the key points are already set, and the position interpolation mode
// is already set. Has no effect if the position interpolation mode is
// NONE.
// ------------------------------------------------------------------------
void vsPathMotion::autoSetTimes(double totalPathSeconds)
{
    double *segmentLengths;
    double totalPathLength;
    int loop;
    vsPathMotionSegment *segment;
    atVector prevPos, currentPos, nextPos, nextNextPos;
    atVector *prevPosPt, *currentPosPt, *nextPosPt, *nextNextPosPt;

    // Check for NONE position interpolation mode
    if (posMode == VS_PATH_POS_IMODE_NONE)
    {
        printf("vsPathMotion::autoSetTimes: Cannot calculate times when "
            "position interpolation mode is NONE");
        return;
    }

    // Create an array to hold the length of each path segment, and
    // initialize the total length of the path to zero
    segmentLengths = (double *)(malloc(pointCount * sizeof(double)));
    totalPathLength = 0.0;

    // Compute the length of each path segment
    for (loop = 0; loop < pointCount; loop++)
    {
        // Get the point data for the ends of this segment, as well as one
        // point to each side of the segment. If the cycle mode is CLOSED LOOP,
        // then these extra points can loop around the path ends. If the mode
        // is RESTART, then we get NULL data for the points off the ends of the
        // path.

        // Previous
        segment = getSegmentData(loop - 1);
        if (segment)
        {
            prevPos = segment->getPosition();
            prevPosPt = &prevPos;
        }
        else
            prevPosPt = NULL;

        // Current
        segment = getSegmentData(loop);
        if (segment)
        {
            currentPos = segment->getPosition();
            currentPosPt = &currentPos;
        }
        else
            currentPosPt = NULL;

        // Next
        segment = getSegmentData(loop + 1);
        if (segment)
        {
            nextPos = segment->getPosition();
            nextPosPt = &nextPos;
        }
        else
            nextPosPt = NULL;

        // One-after-next
        segment = getSegmentData(loop + 2);
        if (segment)
        {
            nextNextPos = segment->getPosition();
            nextNextPosPt = &nextNextPos;
        }
        else
            nextNextPosPt = NULL;

        // Calculate the length of the segment represented by the four
        // points
        switch (posMode)
        {
            case VS_PATH_POS_IMODE_LINEAR:
                segmentLengths[loop] = calcSegLengthLinear(currentPosPt,
                    nextPosPt);
                break;

            case VS_PATH_POS_IMODE_ROUNDED:
                segmentLengths[loop] = calcSegLengthRoundCorner(prevPosPt,
                    currentPosPt, nextPosPt, nextNextPosPt);
                break;

            case VS_PATH_POS_IMODE_SPLINE:
                segmentLengths[loop] = calcSegLengthSpline(prevPosPt,
                    currentPosPt, nextPosPt, nextNextPosPt);
                break;

            default:
                printf("vsPathMotion::autoSetTimes: Unrecognized position"
                    " interpolation mode constant\n");

                // Free the memory before returning.
                free(segmentLengths);
                return;
        }

        // Add the length of the segment to the total path length
        totalPathLength += segmentLengths[loop];
    }

    // Set the travel time for each segment of the path based on the
    // segment's length
    for (loop = 0; loop < pointCount; loop++)
    {
        segment = (vsPathMotionSegment *)(pointList.getEntry(loop));
        segment->setTravelTime((segmentLengths[loop] / totalPathLength) *
            totalPathSeconds);
    }

    // Done. Clean up.
    free(segmentLengths);
}

// ------------------------------------------------------------------------
// Starts the path motion
// ------------------------------------------------------------------------
void vsPathMotion::startResume()
{
    // If we're stopped, reset the path information before starting
    if (currentPlayMode == VS_PATH_STOPPED)
    {
        currentSegmentIdx = 0;
        currentSegmentTime = 0.0;
        currentCycleCount = 0;
    }

    currentPlayMode = VS_PATH_PLAYING;
}

// ------------------------------------------------------------------------
// Pauses the path motion
// ------------------------------------------------------------------------
void vsPathMotion::pause()
{
    currentPlayMode = VS_PATH_PAUSED;
}

// ------------------------------------------------------------------------
// Stops the path motion, resetting it back to the beginning
// ------------------------------------------------------------------------
void vsPathMotion::stop()
{
    currentPlayMode = VS_PATH_STOPPED;
}

// ------------------------------------------------------------------------
// Gets the current play mode
// ------------------------------------------------------------------------
int vsPathMotion::getPlayMode()
{
    return currentPlayMode;
}

// ------------------------------------------------------------------------
// Gets the index of the path segment that we're currently traversing
// ------------------------------------------------------------------------
int vsPathMotion::getCurrentSegment()
{
    return currentSegmentIdx;
}

// ------------------------------------------------------------------------
// Sets any or all of the data in the object from the instructions
// contained in the specified external data file
// ------------------------------------------------------------------------
void vsPathMotion::configureFromFile(char *filename)
{
    FILE *infile;
    char lineBuf[256], commandStr[256], constantStr[256];
    int intValue;
    double doubleValue;
    atVector vector(3);
    atQuat quat;

    // Attempt to open the specified file
    infile = fopen(filename, "r");
    if (!infile)
    {
        printf("vsPathMotion::configureFromFile: Unable to open file '%s'\n",
            filename);
        return;
    }

    // Strip leading whitespace
    fscanf(infile, " \n");

    // Go through each line in the file and interpret the commands
    while (!feof(infile))
    {
        // Read in the line
        fgets(lineBuf, 255, infile);

        // Scan in the first token in the line, which should be the name of
        // the particular command to process
        sscanf(lineBuf, "%s", commandStr);

        // Interpret the command
        if (!strcmp(commandStr, "setPositionMode"))
        {
            // Read in the position mode constant and set it
            sscanf(lineBuf, "%*s %s", constantStr);
            if (!strcmp(constantStr, "VS_PATH_POS_IMODE_NONE"))
                setPositionMode(VS_PATH_POS_IMODE_NONE);
            else if (!strcmp(constantStr, "VS_PATH_POS_IMODE_LINEAR"))
                setPositionMode(VS_PATH_POS_IMODE_LINEAR);
            else if (!strcmp(constantStr, "VS_PATH_POS_IMODE_ROUNDED"))
                setPositionMode(VS_PATH_POS_IMODE_ROUNDED);
            else if (!strcmp(constantStr, "VS_PATH_POS_IMODE_SPLINE"))
                setPositionMode(VS_PATH_POS_IMODE_SPLINE);
            else
                printf("vsPathMotion::configureFromFile (setPositionMode): "
                    "Unrecognized position mode constant '%s'\n", constantStr);
        }
        else if (!strcmp(commandStr, "setOrientationMode"))
        {
            // Read in the orientation mode constant and set it
            sscanf(lineBuf, "%*s %s", constantStr);
            if (!strcmp(constantStr, "VS_PATH_ORI_IMODE_NONE"))
                setOrientationMode(VS_PATH_ORI_IMODE_NONE);
            else if (!strcmp(constantStr, "VS_PATH_ORI_IMODE_SLERP"))
                setOrientationMode(VS_PATH_ORI_IMODE_SLERP);
            else if (!strcmp(constantStr, "VS_PATH_ORI_IMODE_NLERP"))
                setOrientationMode(VS_PATH_ORI_IMODE_NLERP);
            else if (!strcmp(constantStr, "VS_PATH_ORI_IMODE_SPLINE"))
                setOrientationMode(VS_PATH_ORI_IMODE_SPLINE);
            else if (!strcmp(constantStr, "VS_PATH_ORI_IMODE_ATPOINT"))
                setOrientationMode(VS_PATH_ORI_IMODE_ATPOINT);
            else if (!strcmp(constantStr, "VS_PATH_ORI_IMODE_FORWARD"))
                setOrientationMode(VS_PATH_ORI_IMODE_FORWARD);
            else
                printf("vsPathMotion::configureFromFile (setOrientationMode):"
                    "Unrecognized orientation mode constant '%s'\n",
                    constantStr);
        }
        else if (!strcmp(commandStr, "setCycleMode"))
        {
            // Read in the cycle mode constant and set it
            sscanf(lineBuf, "%*s %s", constantStr);
            if (!strcmp(constantStr, "VS_PATH_CYCLE_RESTART"))
                setCycleMode(VS_PATH_CYCLE_RESTART);
            else if (!strcmp(constantStr, "VS_PATH_CYCLE_CLOSED_LOOP"))
                setCycleMode(VS_PATH_CYCLE_CLOSED_LOOP);
            else
                printf("vsPathMotion::configureFromFile (setCycleMode):"
                    "Unrecognized cycle mode constant '%s'\n", constantStr);
        }
        else if (!strcmp(commandStr, "setCycleCount"))
        {
            // Read in the cycle count value and set it
            sscanf(lineBuf, "%*s %d", &intValue);
            setCycleCount(intValue);
        }
        else if (!strcmp(commandStr, "setCornerRadius"))
        {
            // Read in the corner radius value and set it
            sscanf(lineBuf, "%*s %lf", &doubleValue);
            setCornerRadius(doubleValue);
        }
        else if (!strcmp(commandStr, "setLookAtPoint"))
        {
            // Read in the look-at-point vector and set it
            sscanf(lineBuf, "%*s %lf %lf %lf", &(vector[0]), &(vector[1]),
                &(vector[2]));
            setLookAtPoint(vector);
        }
        else if (!strcmp(commandStr, "setUpDirection"))
        {
            // Read in the up-direction vector and set it
            sscanf(lineBuf, "%*s %lf %lf %lf", &(vector[0]), &(vector[1]),
                &(vector[2]));
            setUpDirection(vector);
        }
        else if (!strcmp(commandStr, "setPointListSize"))
        {
            // Read in the list size value and set it
            sscanf(lineBuf, "%*s %d", &intValue);
            setPointListSize(intValue);
        }
        else if (!strcmp(commandStr, "setPosition"))
        {
            // Read in the position index and vector and set it
            sscanf(lineBuf, "%*s %d %lf %lf %lf", &intValue, &(vector[0]),
                &(vector[1]), &(vector[2]));
            setPosition(intValue, vector);
        }
        else if (!strcmp(commandStr, "setOrientation"))
        {
            // Read in the orientation index and quat and set it
            sscanf(lineBuf, "%*s %d %lf %lf %lf %lf", &intValue, &(quat[0]),
                &(quat[1]), &(quat[2]), &(quat[3]));
            setOrientation(intValue, quat);
        }
        else if (!strcmp(commandStr, "setEulerOrientation"))
        {
            // Read in the orientation in terms of Euler angles (heading,
            // pitch, roll), convert that to a quaternion, and set that as the
            // orientation
            sscanf(lineBuf, "%*s %d %lf %lf %lf", &intValue, &(vector[0]),
                &(vector[1]), &(vector[2]));
            quat.setEulerRotation(AT_EULER_ANGLES_ZXY_R, vector[0], vector[1],
                vector[2]);
            setOrientation(intValue, quat);
        }
        else if (!strcmp(commandStr, "setTime"))
        {
            // Read in the time index and value and set it
            sscanf(lineBuf, "%*s %d %lf", &intValue, &doubleValue);
            setTime(intValue, doubleValue);
        }
        else if (!strcmp(commandStr, "setPauseTime"))
        {
            // Read in the pause time index and value and set it
            sscanf(lineBuf, "%*s %d %lf", &intValue, &doubleValue);
            setPauseTime(intValue, doubleValue);
        }
        else if (!strcmp(commandStr, "autoSetTimes"))
        {
            // Read in the total path time and run the auto set function
            sscanf(lineBuf, "%*s %lf", &doubleValue);
            autoSetTimes(doubleValue);
        }
        else if (!strcmp(commandStr, "startResume"))
        {
            // Start the sequence
            startResume();
        }
        else if (!strcmp(commandStr, "pause"))
        {
            // Pause the sequence
            pause();
        }
        else if (!strcmp(commandStr, "stop"))
        {
            // Stop the sequence
            stop();
        }
        else
        {
            printf("vsPathMotion::configureFromFile: Unrecognized command "
                "'%s'\n", commandStr);
        }

        // Strip trailing whitespace
        fscanf(infile, " \n");
    }

    // Clean up
    fclose(infile);
}

// ------------------------------------------------------------------------
// The default update function for the vsPathMotion. Since no delta-time
// was supplied, it simply calls the main update function with the
// delta-time determined from the system clock.
// ------------------------------------------------------------------------
void vsPathMotion::update()
{
   update((vsTimer::getSystemTimer())->getInterval());
}

// ------------------------------------------------------------------------
// Updates the motion model by creating new interpolations for the current
// position and orientation, and applies them to the model's kinematics
// object. The deltaTime should be measured in seconds.
// ------------------------------------------------------------------------
void vsPathMotion::update(double deltaTime)
{
    vsPathMotionSegment *prevSeg, *currentSeg, *nextSeg, *nextNextSeg;
    atVector prevSegPos, currentSegPos, nextSegPos, nextNextSegPos;
    atVector *prevSegPosPt, *currentSegPosPt, *nextSegPosPt, *nextNextSegPosPt;
    atQuat prevSegOri, currentSegOri, nextSegOri, nextNextSegOri;
    atQuat *prevSegOriPt, *currentSegOriPt, *nextSegOriPt, *nextNextSegOriPt;
    double frameTime, segmentTotalTime;
    atVector newPosition;
    atQuat newOrientation;
    double parameter;

    // If the current play mode is STOPPED, then don't do anything
    if (currentPlayMode == VS_PATH_STOPPED)
        return;

    // If there aren't any points defined, then there's nothing we can do;
    // abort.
    if (pointCount < 1)
        return;

    // If the current play mode is PLAYING, then update the time of the
    // path based on the draw time from the last frame
    if (currentPlayMode == VS_PATH_PLAYING)
    {
        // Determine how much time passed last frame
        frameTime = deltaTime;

        // Get the data for the current segment
        currentSeg = (vsPathMotionSegment *)
            (pointList.getEntry(currentSegmentIdx));

        // Determine how much time we actually need to spend on this
        // segment, counting in temporary pauses
        segmentTotalTime = currentSeg->getTravelTime();
        if (currentSeg->getPauseTime() > 0.0)
            segmentTotalTime += currentSeg->getPauseTime();

        // Add the 'time last frame' value to the time spent on the
        // current segment
        currentSegmentTime += frameTime;
               
        // If we've spent longer on this segment than its total time,
        // then transition to the next segment
        while (currentSegmentTime >= segmentTotalTime && deltaTime > 0.0)
        {
            // * Advance to the next path segment

            // Switch to the next segment index
            currentSegmentIdx++;
            currentSegmentTime -= segmentTotalTime;

            // Check if we've hit the end of the path and need to start
            // over
            if (currentSegmentIdx >= pointCount)
            {
                currentSegmentIdx = 0;
                currentCycleCount++;
            }

            // If we've completed a number of cycles equal to the
            // user-specified cycle count, and we're not looping
            // infinitely, then stop. Set the location on the path to the
            // end of the path, and mark the path mode as STOPPED; this
            // is the last trip through this loop we will make.
            if ((cycleCount != VS_PATH_CYCLE_FOREVER) &&
                (currentCycleCount >= cycleCount))
            {
                // Set the play mode to stopped, and set the current location
                // on the path to the end of the path
                currentPlayMode = VS_PATH_STOPPED;
                currentSegmentIdx = pointCount - 1;
                currentSeg = (vsPathMotionSegment *)
                    (pointList.getEntry(currentSegmentIdx));
                currentSegmentTime = currentSeg->getTravelTime();
                if (currentSeg->getPauseTime() > 0.0)
                    currentSegmentTime += currentSeg->getPauseTime();
                break;
            }

            // Get the data corresponding to the new segment
            currentSeg = (vsPathMotionSegment *)
                (pointList.getEntry(currentSegmentIdx));

            // If the new segment has a negative pause time, then go
            // into 'pause indefinitely' mode. Set the current time on the
            // segment to zero, so that the computation step will place
            // the position and orientation at the beginning of this
            // segment.
            if (currentSeg->getPauseTime() < 0.0)
            {
                currentPlayMode = VS_PATH_PAUSED;
                currentSegmentTime = 0.0;
            }

            // Check if we're on the last segment of the path (the part of
            // the path after the last point), and if we're in RESTART
            // cycle mode. Since there can't be any actual path to travel
            // in this case, force the travel time for that segment to be
            // zero. (There _is_ still a segment there, as there is the
            // possibility that a delay time can be set there.)
            if ((currentSegmentIdx == (pointCount - 1)) &&
                (cycleMode == VS_PATH_CYCLE_RESTART))
            {
                currentSeg->setTravelTime(0.0);
            }

            // Calculate the new segment total path time
            segmentTotalTime = currentSeg->getTravelTime();
            if (currentSeg->getPauseTime() > 0.0)
                segmentTotalTime += currentSeg->getPauseTime();

        } // if (currentSegmentTime >= segmentTotalTime)
        
        // If we're going backwards, see if we need to transition to the
        // previous segment.
        while (currentSegmentTime < 0.0 && deltaTime < 0.0)
        {
            // * Advance to the previous path segment

            // Switch to the previous segment index
            currentSegmentIdx--;
            currentSegmentTime += segmentTotalTime;

            // Check if we've hit the beginning of the path and need to rewind
            // to the end
            if (currentSegmentIdx < 0)
            {
                currentSegmentIdx = pointCount-1;
                currentCycleCount--;
            }
                   
            // If we've completed a number of cycles equal to the
            // user-specified cycle count, and we're not looping
            // infinitely, then stop. Set the location on the path to the
            // end of the path, and mark the path mode as STOPPED; this
            // is the last trip through this loop we will make.
 
            // NOTE: This means that if a vsPathMotion is set to play, and
            // the very first update has a negative delta-time, and the cycle
            // count is not set to VS_PATH_CYCLE_FOREVER, then the vsPathMotion
            // will immediately stop.
            if ((cycleCount != VS_PATH_CYCLE_FOREVER) &&
                (currentCycleCount < 0))
            {
                // Set the play mode to stopped, and set the current location
                // on the path to the end of the path
                currentPlayMode = VS_PATH_STOPPED;
                currentSegmentIdx = pointCount - 1;
                currentSeg = (vsPathMotionSegment *)
                    (pointList.getEntry(currentSegmentIdx));
                currentSegmentTime = currentSeg->getTravelTime();
                if (currentSeg->getPauseTime() > 0.0)
                    currentSegmentTime += currentSeg->getPauseTime();
                break;
            }

            // Get the data corresponding to the new segment
            currentSeg = (vsPathMotionSegment *)
                (pointList.getEntry(currentSegmentIdx));

            // If the new segment has a negative pause time, then go
            // into 'pause indefinitely' mode. Set the current time on the
            // segment to zero, so that the computation step will place
            // the position and orientation at the beginning of this
            // segment.
            if (currentSeg->getPauseTime() < 0.0)
            {
                currentPlayMode = VS_PATH_PAUSED;
                currentSegmentTime = 0.0;
            }

            // Check if we're on the first segment of the path (the part of
            // the path before the first point), and if we're in RESTART
            // cycle mode. Since there can't be any actual path to travel
            // in this case, force the travel time for that segment to be
            // zero. (There _is_ still a segment there, as there is the
            // possibility that a delay time can be set there.)
            if ((currentSegmentIdx == 0) &&
                (cycleMode == VS_PATH_CYCLE_RESTART))
            {
                currentSeg->setTravelTime(0.0);
            }

            // Calculate the new segment total path time
            segmentTotalTime = currentSeg->getTravelTime();
            if (currentSeg->getPauseTime() > 0.0)
                segmentTotalTime += currentSeg->getPauseTime();

        }
    } // if (currentPlayMode == VS_PATH_PLAYING)

    // * Using the current segment parameters, recompute the position and
    // orientation of the object on the path

    // Get the point data for the ends of this segment, as well as one
    // point to each side of the segment. If the cycle mode is CLOSED LOOP,
    // then these extra points can loop around the path ends. If the mode
    // is REPEAT, then we get NULL data for the points off the ends of the
    // path.
    if(deltaTime >= 0)
    {
        prevSeg = getSegmentData(currentSegmentIdx - 1);
        currentSeg = getSegmentData(currentSegmentIdx);
        nextSeg = getSegmentData(currentSegmentIdx + 1);
        nextNextSeg = getSegmentData(currentSegmentIdx + 2);
    }
    // Otherwise, the delta-time is negative, so get the segments as if we're
    // going backwards.
    else
    {
        prevSeg = getSegmentData(currentSegmentIdx + 1);
        currentSeg = getSegmentData(currentSegmentIdx);
        nextSeg = getSegmentData(currentSegmentIdx - 1);
        nextNextSeg = getSegmentData(currentSegmentIdx - 2);
    }

    // Compute the interpolation parameter, which in this case is the
    // amount of time spent on this segment (minus the pause time, if any)
    // over the total travel time for the segment.
    if (currentSeg->getPauseTime() > 0.0)
        parameter = (currentSegmentTime - currentSeg->getPauseTime()) /
            currentSeg->getTravelTime();
    else
        parameter = currentSegmentTime / currentSeg->getTravelTime();
            
    if (parameter < 0.0)
        parameter = 0.0;
    else if (parameter > 1.0)
        parameter = 1.0;

    // Get the positions from the segments
    if (prevSeg)
    {
        prevSegPos = prevSeg->getPosition();
        prevSegPosPt = &prevSegPos;
    }
    else
        prevSegPosPt = NULL;
    if (currentSeg)
    {
        currentSegPos = currentSeg->getPosition();
        currentSegPosPt = &currentSegPos;
    }
    else
        currentSegPosPt = NULL;
    if (nextSeg)
    {
        nextSegPos = nextSeg->getPosition();
        nextSegPosPt = &nextSegPos;
    }
    else
        nextSegPosPt = NULL;
    if (nextNextSeg)
    {
        nextNextSegPos = nextNextSeg->getPosition();
        nextNextSegPosPt = &nextNextSegPos;
    }
    else
        nextNextSegPosPt = NULL;

    // Interpolate the position based on the interpolation mode
    switch (posMode)
    {
        case VS_PATH_POS_IMODE_NONE:
            newPosition = objectKin->getPosition();
            break;

        case VS_PATH_POS_IMODE_LINEAR:
            newPosition = interpolatePosLinear(currentSegPosPt, nextSegPosPt,
                parameter);
            break;

        case VS_PATH_POS_IMODE_ROUNDED:
            newPosition = interpolatePosRoundCorner(prevSegPosPt,
                currentSegPosPt, nextSegPosPt, nextNextSegPosPt, parameter);
            break;

        case VS_PATH_POS_IMODE_SPLINE:
            newPosition = interpolatePosSpline(prevSegPosPt, currentSegPosPt,
                nextSegPosPt, nextNextSegPosPt, parameter);
            break;
    }

    // Get the orientations from the segments
    if (prevSeg)
    {
        prevSegOri = prevSeg->getOrientation();
        prevSegOriPt = &prevSegOri;
    }
    else
        prevSegOriPt = NULL;
    if (currentSeg)
    {
        currentSegOri = currentSeg->getOrientation();
        currentSegOriPt = &currentSegOri;
    }
    else
        currentSegOriPt = NULL;
    if (nextSeg)
    {
        nextSegOri = nextSeg->getOrientation();
        nextSegOriPt = &nextSegOri;
    }
    else
        nextSegOriPt = NULL;
    if (nextNextSeg)
    {
        nextNextSegOri = nextNextSeg->getOrientation();
        nextNextSegOriPt = &nextNextSegOri;
    }
    else
        nextNextSegOriPt = NULL;

    // Then interpolate the orientation based on the interpolation mode
    switch (oriMode)
    {
        case VS_PATH_ORI_IMODE_NONE:
            newOrientation = objectKin->getOrientation();
            break;

        case VS_PATH_ORI_IMODE_SLERP:
            newOrientation = interpolateOriSlerp(currentSegOriPt, nextSegOriPt,
                parameter);
            break;

        case VS_PATH_ORI_IMODE_NLERP:
            newOrientation = interpolateOriNlerp(currentSegOriPt, nextSegOriPt,
                parameter);
            break;

        case VS_PATH_ORI_IMODE_SPLINE:
            newOrientation = interpolateOriSpline(prevSegOriPt, currentSegOriPt,
                nextSegOriPt, nextNextSegOriPt, parameter);
            break;

        case VS_PATH_ORI_IMODE_ATPOINT:
            newOrientation = interpolateOriToPt(newPosition, lookPoint);
            break;

        case VS_PATH_ORI_IMODE_FORWARD:
            newOrientation = interpolateOriToPt(currentPos, newPosition);
            break;
    }

    // Finally, copy the new computed position and orientation into the
    // kinematics object, and also keep a copy of it for next time
    objectKin->setPosition(newPosition);
    currentPos = newPosition;
    if (!AT_EQUAL(newOrientation.getMagnitude(), 0.0))
    {
        objectKin->setOrientation(newOrientation);
        currentOri = newOrientation;
    }
}

// ------------------------------------------------------------------------
// Gets the current position
// ------------------------------------------------------------------------
atVector vsPathMotion::getCurrentPosition()
{
    return currentPos;
}

// ------------------------------------------------------------------------
// Gets the current orientation
// ------------------------------------------------------------------------
atQuat vsPathMotion::getCurrentOrientation()
{
    return currentOri;
}

// ------------------------------------------------------------------------
// Private function
// Calculates the linear (straight line) length of the path segment
// defined by the two points. Returns zero if either parameter is NULL.
// ------------------------------------------------------------------------
double vsPathMotion::calcSegLengthLinear(atVector *vec1, atVector *vec2)
{
    // NULL parameter check
    if (!vec1 || !vec2)
        return 0.0;

    // Standard distance calculation
    return (*vec1 - *vec2).getMagnitude();
}

// ------------------------------------------------------------------------
// Private function
// Calculates the length of the path segment defined by the two points
// vec1 and vec2, accounting for the rounded-corner property of the ends
// of the segment. Uses vec0 and vec3 to calculate the size of the
// circular arcs at the ends of the segment. Returns zero if vec1 or vec2
// are NULL. Assumes no arc for the beginning or end of the segment if
// vec0 or vec3 are NULL, respectively.
// ------------------------------------------------------------------------
double vsPathMotion::calcSegLengthRoundCorner(atVector *vec0, atVector *vec1,
    atVector *vec2, atVector *vec3)
{
    double result;
    atVector vecA, vecB;
    double theta, arcedOverLength, roundRadius, arcLength;
    double tempDouble;

    // NULL parameter check
    if (!vec1 || !vec2)
        return 0.0;

    // Start with the streight-line distance
    result = (*vec1 - *vec2).getMagnitude();

    // If the first part of the segment is arced, substitute it's arc
    // length for the amount of straight-line it arcs over
    if (vec0)
    {
        // Create two vectors corresponding to the two path segments on
        // either side of the main control point
        vecA = *vec0 - *vec1;
        vecB = *vec2 - *vec1;

        // Calculate the degree measure of the arc that will connect the two
        // straight-line segment pieces. This angle is the compliment of the
        // angle between the two vectors.
        theta = 180.0 - fabs(vecA.getAngleBetween(vecB));

        // Determine how much of the straight-line portion of the adjacent
        // segments is being ignored in favor of the round-corner arc. This
        // value cannot be more than half of the straight-line length of the
        // segments, so clip to those half-lengths if needed.
        arcedOverLength = (roundCornerRadius * sin(AT_DEG2RAD(theta / 2.0))) /
            sin(AT_DEG2RAD(90 - (theta / 2.0)));

        tempDouble = vecA.getMagnitude() / 2.0;
        if (tempDouble < arcedOverLength)
            arcedOverLength = tempDouble;

        tempDouble = vecB.getMagnitude() / 2.0;
        if (tempDouble < arcedOverLength)
            arcedOverLength = tempDouble;

        // Now that we know the ignored segment length, compute the actual
        // arc radius from that value
        roundRadius = (arcedOverLength * sin(AT_DEG2RAD(90 - (theta / 2.0)))) /
            sin(AT_DEG2RAD(theta / 2.0));

        // Compute the actual arc length for the arc segment
        arcLength = AT_DEG2RAD(theta / 2.0) * roundRadius;

        // Correct the total length of the segment by subtracting the
        // arcedOverLength and adding in instead the arc length of
        // the arc segment
        result -= arcedOverLength;
        result += arcLength;
    }

    // If the last part of the segment is arced, substitute it's arc
    // length for the amount of straight-line it arcs over
    if (vec3)
    {
        // Create two vectors corresponding to the two path segments on
        // either side of the main control point
        vecA = *vec3 - *vec2;
        vecB = *vec1 - *vec2;

        // Calculate the degree measure of the arc that will connect the two
        // straight-line segment pieces. This angle is the compliment of the
        // angle between the two vectors.
        theta = 180.0 - fabs(vecA.getAngleBetween(vecB));

        // Determine how much of the straight-line portion of the adjacent
        // segments is being ignored in favor of the round-corner arc. This
        // value cannot be more than half of the straight-line length of the
        // segments, so clip to those half-lengths if needed.
        arcedOverLength = (roundCornerRadius * sin(AT_DEG2RAD(theta / 2.0))) /
            sin(AT_DEG2RAD(90 - (theta / 2.0)));

        tempDouble = vecA.getMagnitude() / 2.0;
        if (tempDouble < arcedOverLength)
            arcedOverLength = tempDouble;

        tempDouble = vecB.getMagnitude() / 2.0;
        if (tempDouble < arcedOverLength)
            arcedOverLength = tempDouble;

        // Now that we know the ignored segment length, compute the actual
        // arc radius from that value
        roundRadius = (arcedOverLength * sin(AT_DEG2RAD(90 - (theta / 2.0)))) /
            sin(AT_DEG2RAD(theta / 2.0));

        // Compute the actual arc length for the arc segment
        arcLength = AT_DEG2RAD(theta / 2.0) * roundRadius;

        // Correct the total length of the segment by subtracting the
        // arcedOverLength and adding in instead the arc length of
        // the arc segment
        result -= arcedOverLength;
        result += arcLength;
    }

    return result;
}

// ------------------------------------------------------------------------
// Private function
// Calculates the length of the path segment defined by the two points
// vec1 and vec2, accounting for the cubic-spline property of the segment.
// Uses vec0 and vec3 to calculate the normals of the spline curve at the
// end points of the segment. Returns zero if vec1 or vec2 are NULL.
// Assumes values for vec0 and vec3 if either is NULL.
// ------------------------------------------------------------------------
double vsPathMotion::calcSegLengthSpline(atVector *vec0, atVector *vec1,
    atVector *vec2, atVector *vec3)
{
    // NULL parameter check
    if (!vec1 || !vec2)
        return 0.0;

    // Break the spline into four pieces, and call the subsegment arc
    // length determination function on each piece. This is done because
    // it is possible that the recursive algorithm will fail if it is
    // applied to the entire spline segment at once; this is due to the
    // ability of a cublic spline to cross over itself or over its center
    // line multiple times.
    return (calcSubsegLengthSpline(vec0, vec1, vec2, vec3, 0.00, 0.25) +
            calcSubsegLengthSpline(vec0, vec1, vec2, vec3, 0.25, 0.50) +
            calcSubsegLengthSpline(vec0, vec1, vec2, vec3, 0.50, 0.75) +
            calcSubsegLengthSpline(vec0, vec1, vec2, vec3, 0.75, 1.00));
}

// ------------------------------------------------------------------------
// Private function
// Helper function used by spline arc length calculations
// ------------------------------------------------------------------------
double vsPathMotion::calcSubsegLengthSpline(atVector *vec0, atVector *vec1,
    atVector *vec2, atVector *vec3, double start, double end)
{
    // This function uses a recursive algorithm to find the arc length
    // of the spline segment located between the two indicated parameters
    // on the spline defined by the given control points.

    atVector startPt, midPt, endPt;
    double mid;
    double fullLength, firstHalfLength, secondHalfLength;

    // Calculate the parameter halfway between the start and end
    // parameters
    mid = ((start + end) / 2.0);

    // Compute the points on the spline for the start parameter, the end
    // parameter, and a parameter value halfway between the two.
    startPt = interpolatePosSpline(vec0, vec1, vec2, vec3, start);
    endPt = interpolatePosSpline(vec0, vec1, vec2, vec3, end);
    midPt = interpolatePosSpline(vec0, vec1, vec2, vec3, mid);

    // Compute the straight-line dtstance between the starting and ending
    // points
    fullLength = (startPt - endPt).getMagnitude();

    // Compute the straight-line distance between the starting and halfway
    // points, and the halfway and ending points
    firstHalfLength = (startPt - midPt).getMagnitude();
    secondHalfLength = (midPt - endPt).getMagnitude();

    // If the sum of the two half-lengths is 'close enough' to the full
    // straight-line length, then we assume that we don't need to recurse
    // any more and just return the half-lengths sum.
    if (AT_EQUAL(fullLength, (firstHalfLength + secondHalfLength)))
        return (firstHalfLength + secondHalfLength);

    // The full straight-line estimate is not close enough; break the
    // estimation process up into two subparts and try again.
    return (calcSubsegLengthSpline(vec0, vec1, vec2, vec3, start, mid) +
            calcSubsegLengthSpline(vec0, vec1, vec2, vec3, mid, end));
}

// ------------------------------------------------------------------------
// Private function
// Calculates a point on the path segment specified by the two points
// using linear interpolation. Returns one of the points if the other
// point is NULL, or a zero vector if both points are NULL.
// ------------------------------------------------------------------------
atVector vsPathMotion::interpolatePosLinear(atVector *vec1, atVector *vec2,
    double parameter)
{
    atVector result(0.0, 0.0, 0.0);

    // NULL parameter check
    if (!vec1 && !vec2)
        return result;
    if (!vec1)
        return *vec2;
    if (!vec2)
        return *vec1;

    // Linear interpolation: [A + (B-A)*t], rewritten to [A*(1-t) + B*t]
    result = vec1->getScaled(1.0 - parameter) + vec2->getScaled(parameter);
    return result;
}

// ------------------------------------------------------------------------
// Private function
// Calculates a point on the rounded-corner path segment specified by the
// points vec1 and vec2. Uses vec0 and vec3 to calculate the circular arcs
// at the ends of the segment. Returns one of vec1 or vec2 if the other
// is NULL, or a zero vector if both are NULL. Assumes no arc for the
// beginning or end of the segment of vec0 or vec3 are NULL, respectively.
// ------------------------------------------------------------------------
atVector vsPathMotion::interpolatePosRoundCorner(atVector *vec0, atVector *vec1,
    atVector *vec2, atVector *vec3, double parameter)
{
    atVector result(0.0, 0.0, 0.0);
    double roundRadius;
    double theta;
    atVector mainControlPoint;
    atVector vecA, vecB;
    atVector vecAArcVec, vecBArcVec, arcCenter;
    double arcAngle;
    double arcedOverLength, arcLength, halfSegmentLength;
    double arcEndParameter, subsegmentParameter;
    double lineLength;
    double tempDouble;

    // NULL parameter check
    if (!vec1 && !vec2)
        return result;
    if (!vec1)
        return *vec2;
    if (!vec2)
        return *vec1;

    // Behave differently depending on whether the target point is on the
    // first or second half of the segment
    if (parameter < 0.5)
    {
        // If there's no 'previous' control point to specify the arc, then
        // just resort to linear interpolation
        if (!vec0)
            return interpolatePosLinear(vec1, vec2, parameter);

        // Select the point vec1 to be the focus for our calculations
        mainControlPoint = *vec1;

        // Create two vectors corresponding to the two path segments on
        // either side of the main control point
        vecA = *vec0 - *vec1;
        vecB = *vec2 - *vec1;
    }
    else // (parameter >= 0.5)
    {
        // If there's no 'next' control point to specify the arc, then
        // just resort to linear interpolation
        if (!vec3)
            return interpolatePosLinear(vec1, vec2, parameter);

        // Select the point vec2 to be the focus for our calculations
        mainControlPoint = *vec2;

        // Create two vectors corresponding to the two path segments on
        // either side of the main control point
        vecA = *vec3 - *vec2;
        vecB = *vec1 - *vec2;

        // 'Flip' the parameter value around the halfway mark, so that it
        // appears to the rest of the function that we're on the first
        // half of the segment. (This simplifies a lot of things later.)
        parameter = 1.0 - parameter;
    }

    // Calculate the degree measure of the arc that will connect the two
    // straight-line segment pieces. This angle is the compliment of the
    // angle between the two vectors.
    theta = 180.0 - fabs(vecA.getAngleBetween(vecB));

    // Determine how much of the straight-line portion of the adjacent
    // segments is being ignored in favor of the round-corner arc. This
    // value cannot be more than half of the straight-line length of the
    // segments, so clip to those half-lengths if needed.
    arcedOverLength = (roundCornerRadius * sin(AT_DEG2RAD(theta / 2.0))) /
        sin(AT_DEG2RAD(90 - (theta / 2.0)));

    tempDouble = vecA.getMagnitude() / 2.0;
    if (tempDouble < arcedOverLength)
        arcedOverLength = tempDouble;

    tempDouble = vecB.getMagnitude() / 2.0;
    if (tempDouble < arcedOverLength)
        arcedOverLength = tempDouble;

    // Now that we know the ignored segment length, compute the actual
    // arc radius from that value
    roundRadius = (arcedOverLength * sin(AT_DEG2RAD(90 - (theta / 2.0)))) /
        sin(AT_DEG2RAD(theta / 2.0));

    // Compute the actual arc length for the arc segment
    arcLength = AT_DEG2RAD(theta / 2.0) * roundRadius;

    // Compute the total length of the half-segment
    halfSegmentLength =
        arcLength + ((vecB.getMagnitude() / 2.0) - arcedOverLength);

    // Determine if the desired point is on the arc or on the straight
    // portion of the segment. This determination is made by comparing the
    // interpolation parameter with the parametric value for the end of
    // the arc.
    arcEndParameter = (arcLength / halfSegmentLength) / 2.0;
    if (parameter >= arcEndParameter)
    {
        // Straight-line subsegment

        // Calculate the fraction of the straight-line subsegment that
        // we should have crossed
        subsegmentParameter =
            (parameter - arcEndParameter) / (0.5 - arcEndParameter);

        // Using the subsegment fraction, compute the total absolute
        // distance that the target point should be from the main control
        // point. This equation is the parameter on the line-only
        // subsegment, multiplied by the length of the
        // line-only-subsegment, plus the distance passed over by the arc
        // portion of the segment.
        lineLength = (subsegmentParameter * 
            ((vecB.getMagnitude() / 2.0) - arcedOverLength)) +
            arcedOverLength;

        // Create a displacement vector by scaling the vector B to be
        // lineLength long, and create the result point by adding that
        // vector to the main control point.
        result = mainControlPoint +
            vecB.getNormalized().getScaled(lineLength);
    }
    else
    {
        // Arc subsegment

        // Calculate the fraction of the arc subsegment that we should
        // have crossed
        subsegmentParameter = parameter / arcEndParameter;

        // Calculate the two endpoints of the arc segment; these are
        // the two points that are arcedOverLength away from the main
        // control point, along the two vectors A and B.
        vecAArcVec = vecA.getNormalized().getScaled(arcedOverLength);
        vecBArcVec = vecB.getNormalized().getScaled(arcedOverLength);

        // Calculate the center point of the arc's circle. This point
        // is in the direction of the arc's bisector, and is a number
        // of units away equal to the arcedOverLength divided by the
        // sine of half of theta.
        arcCenter = (vecAArcVec + vecBArcVec).getNormalized();
        arcCenter.scale(arcedOverLength / sin(AT_DEG2RAD(theta / 2.0)));

        // Translate the two arc segment endpoints into the coordinate
        // system defined by the arc center as the origin
        vecAArcVec -= arcCenter;
        vecBArcVec -= arcCenter;

        // Perform a spherical linear interpolation between the two
        // arc segment endpoints to get the direction of the target
        // point from the arc center. Take note that the actual piece
        // of arc that we're interested in is only half of the entire
        // arc segment; we have to adjust the parameter accordingly.
        arcAngle = 180.0 - theta;
        subsegmentParameter = 0.5 + (subsegmentParameter / 2.0);

        // slerp
        result = vecAArcVec.getScaled(
                sin(AT_DEG2RAD((1.0 - subsegmentParameter) * arcAngle)) /
                sin(AT_DEG2RAD(arcAngle))) +
            vecBArcVec.getScaled(
                sin(AT_DEG2RAD(subsegmentParameter * arcAngle)) /
                sin(AT_DEG2RAD(arcAngle)));

        // Scale the resulting vector so that it is roundRadius in length
        result.normalize();
        result.scale(roundRadius);

        // Translate the result back into the standard coordinate
        // frame by first translating it to be relative to the main
        // control point, and then translate again to get to global
        // coordinates.
        result += arcCenter;
        result += mainControlPoint;
    }

    // Return the resulting point
    return result;
}

// ------------------------------------------------------------------------
// Private function
// Calculates a point on the spline-based path segment specified by the
// points vec1 and vec2. Uses vec0 and vec3 to calculate the tangents of
// the spline curve at the end points of the segment. Returns one of vec1
// or vec2 if the other is NULL, or a zero vector if both are NULL.
// Assumes values for vec0 and vec3 if either is NULL.
// ------------------------------------------------------------------------
atVector vsPathMotion::interpolatePosSpline(atVector *vec0, atVector *vec1,
    atVector *vec2, atVector *vec3, double parameter)
{
    // Uses a Catmull-Rom derivation of a spline, which is essentially
    // just a Hermite spline, with a special formula for calculating the
    // curve tangents at the control points.

    atVector result(0.0, 0.0, 0.0);
    atVector startTangent, endTangent;
    double paramSqr, paramCube;

    // NULL parameter check
    if (!vec1 && !vec2)
        return result;
    if (!vec1)
        return *vec2;
    if (!vec2)
        return *vec1;

    // Calculate the tangent values at the segment endpoints

    // Average (vec1 - vec0) and (vec2 - vec1) to get the starting tangent.
    // If vec0 isn't defined, then just use (vec2 - vec1).
    // [tangent = ((v1 - v0) + (v2 - v1)) / 2 = (v2 - v0) / 2]
    if (vec0)
        startTangent = (*vec2 - *vec0).getScaled(0.5);
    else
        startTangent = (*vec2 - *vec1);

    // Average (vec2 - vec1) and (vec3 - vec2) to get the ending tangent.
    // If vec3 isn't defined, then just use (vec2 - vec1).
    // [tangent = ((v2 - v1) + (v3 - v2)) / 2 = (v3 - v1) / 2]
    if (vec3)
        endTangent = (*vec3 - *vec1).getScaled(0.5);
    else
        endTangent = (*vec2 - *vec1);

    // Calculate the powers of the parameter (t^2 and t^3)
    paramSqr = parameter * parameter;
    paramCube = paramSqr * parameter;

    // Calculate the point on the spline, using the equation for a
    // Hermite spline:
    // P(t) = (2t^3 - 3t^2 + 1)P(0) + (-2t^3 + 3t^2)P(1) +
    //        (t^3 - 2t^2 + t)P'(0) + (t^3 - t^2)P'(1)
    // where P(0) is vec1, P(1) is vec2, P'(0) is the start point tangent,
    // and P'(1) is the end point tangent.
    result = vec1->getScaled(2*paramCube - 3*paramSqr + 1) +
             vec2->getScaled(-2*paramCube + 3*paramSqr) +
             startTangent.getScaled(paramCube - 2*paramSqr + parameter) +
             endTangent.getScaled(paramCube - paramSqr);

    return result;
}

// ------------------------------------------------------------------------
// Private function
// Calculates a quaternion that is interpolated between the two given
// rotational quaternions using spherical linear interpolation. Returns
// one of the quaterions if the other is NULL, or a no-rotation quaternion
// if they are both NULL.
// ------------------------------------------------------------------------
atQuat vsPathMotion::interpolateOriSlerp(atQuat *ori1, atQuat *ori2,
    double parameter)
{
    atQuat result(0.0, 0.0, 0.0, 1.0);

    // NULL parameter check
    if (!ori1 && !ori2)
        return result;
    if (!ori1)
        return *ori2;
    if (!ori2)
        return *ori1;

    // Quaternions can do slerp by themselves; return their answer
    return (ori1->slerp(*ori2, parameter));
}

// ------------------------------------------------------------------------
// Private function
// Calculates a quaternion that is interpolated between the two given
// rotational quaternions using spherical linear interpolation. Returns
// one of the quaterions if the other is NULL, or a no-rotation quaternion
// if they are both NULL.
// ------------------------------------------------------------------------
atQuat vsPathMotion::interpolateOriNlerp(atQuat *ori1, atQuat *ori2,
    double parameter)
{
    atQuat result(0.0, 0.0, 0.0, 1.0);

    // NULL parameter check
    if (!ori1 && !ori2)
        return result;
    if (!ori1)
        return *ori2;
    if (!ori2)
        return *ori1;

    // Quaternions can do nlerp by themselves; return their answer
    return (ori1->nlerp(*ori2, parameter));
}

// ------------------------------------------------------------------------
// Private function
// Calculates a quaternion that is interpolated between the two given
// rotational quaternions ori1 and ori2 using spline interpolation. The
// quaternions ori0 and ori3 are used to compute the 'tangent'
// orientations at the ends of the spline segment. Returns one of ori1 or
// ori2 if the other is NULL, or a no-rotation quaternion if they are both
// NULL. Assumes values for ori0 or ori3 if either is NULL.
// ------------------------------------------------------------------------
atQuat vsPathMotion::interpolateOriSpline(atQuat *ori0, atQuat *ori1,
    atQuat *ori2, atQuat *ori3, double parameter)
{
    // Based on the algorithm for spline interpolation described in
    // 'Animating Rotation with Quaternion Curves', Ken Shoemake, SIGGRAPH
    // 1985.

    atQuat result(0.0, 0.0, 0.0, 1.0);
    atQuat oriZero, oriThree;
    atQuat aQuat, bQuat;
    atQuat tempQuat;
    atQuat q11, q12, q13, q21, q22;

    // NULL parameter check
    if (!ori1 && !ori2)
        return result;
    if (!ori1)
        return *ori2;
    if (!ori2)
        return *ori1;

    // If ori0 or ori3 are not defined, then fabricate values for them
    if (ori0)
        oriZero = *ori0;
    else
        oriZero = *ori1 * (ori2->getConjugate() * (*ori1));
    if (ori3)
        oriThree = *ori3;
    else
        oriThree = *ori2 * (ori1->getConjugate() * (*ori2));

    // Calculate the new control points for this piece of the curve
    aQuat = quatHalfway(oriZero, *ori1, *ori2);
    bQuat = quatHalfway(oriThree, *ori2, *ori1);

    // Use the de Casteljau algorithm to compute the resulting orientation
    q11 = ori1->slerp(aQuat, parameter).getNormalized();
    q12 = aQuat.slerp(bQuat, parameter).getNormalized();
    q13 = bQuat.slerp(*ori2, parameter).getNormalized();

    q21 = q11.slerp(q12, parameter).getNormalized();
    q22 = q12.slerp(q13, parameter).getNormalized();

    result = q21.slerp(q22, parameter).getNormalized();

    // Done.
    return result;
}

// ------------------------------------------------------------------------
// Private function
// Calculates a rotation quaternion that rotates from the basis
// orientation to an orientation that points an object at currentPt in the
// direction of facePt.
// ------------------------------------------------------------------------
atQuat vsPathMotion::interpolateOriToPt(atVector currentPt, atVector facePt)
{
    atQuat result(0.0, 0.0, 0.0, 0.0);
    atVector forwardVec, upVec;
    atVector yVec, zVec;

    // Determine the forward-facing direction by computing the vector from
    // the current poing to the face point. If this vector is zero-length,
    // abort.
    forwardVec = facePt - currentPt;
    if (AT_EQUAL(forwardVec.getMagnitude(), 0.0))
        return result;
    forwardVec.normalize();

    // Create the basis vectors
    yVec.set(0.0, 1.0, 0.0);
    zVec.set(0.0, 0.0, 1.0);

    // Determine the up-facing direction by examining the up direction
    // vector set by the user. If it is zero, get the up direction from
    // the current orientation. Otherwise, copy the up direction and
    // normalize it.
    if (AT_EQUAL(upDirection.getMagnitude(), 0.0))
    {
        // Rotate a generic up vector by the current orientation to get
        // the current up vector
        upVec = objectKin->getOrientation().rotatePoint(zVec);
    }
    else
    {
        // Copy the direction and force it to be unit length
        upVec = upDirection.getNormalized();
    }

    // Create the desired orientation as the quaternion that rotates the
    // forward and up basis vectors to our desired forward and up vectors
    result.setVecsRotation(yVec, zVec, forwardVec, upVec);

    // Done
    return result;
}

// ------------------------------------------------------------------------
// Private function
// Helper function used by quaternion spline calculations
// ------------------------------------------------------------------------
atQuat vsPathMotion::quatHalfway(atQuat a, atQuat b, atQuat c)
{
    // This function creates a new spline control point, based on the
    // rotations in the q[n-1], q[n], and q[n+1] control points. This new
    // control point lies halfway between where the rotation from q[n-1]
    // to q[n] would go if it kept going (extrapolation), and where the
    // rotation from q[n] to q[n+1] goes. This extra rotation is further
    // reduced by a factor fo three, to account for how much effect the
    // extra control point has on the final shape of the rotation curve.

    atQuat abQuat, bcQuat, bisect;
    double x, y, z, degrees;

    // Find the relative rotations from q[n-1] to q[n] and from q[n] to
    // q[n+1]
    abQuat = a.getConjugate() * b;
    bcQuat = b.getConjugate() * c;

    // Force the quaternions to be on the same side of the 4-D sphere, to
    // prevent the calculations from trying to take the 'long way around'
    if (abQuat.getDotProduct(bcQuat) < 0.0)
        bcQuat.scale(-1.0);

    // Create the bisector as just the ordinary average of the two
    // relative rotations
    bisect = abQuat + bcQuat;
    bisect.normalize();

    // Reduce the magnitude of the averaged rotation by three
    bisect.getAxisAngleRotation(&x, &y, &z, &degrees);
    if (degrees > 180.0)
        degrees -= 360.0;
    bisect.setAxisAngleRotation(x, y, z, degrees / 3.0);

    // Recombine the new relative rotation with the initial center control
    // rotation to get the final control rotation
    return (b * bisect);
}

// ------------------------------------------------------------------------
// Private function
// Helper function used by update and autoSetTimes functions
// ------------------------------------------------------------------------
vsPathMotionSegment *vsPathMotion::getSegmentData(int idx)
{
    // This function obtains the segment data structure for the specified
    // point index. Can also handle out-of-bounds data requests; in this
    // case, the function returns NULL if the cycle mode is RESTART, or
    // wraps around the ends of the data array if the segment mode is
    // CLOSED LOOP.

    // If in-bounds, always return something usable
    if ((idx >= 0) && (idx < pointCount))
        return (vsPathMotionSegment *)(pointList.getEntry(idx));

    // If out-of-bounds and in RESTART mode, return NULL
    if (cycleMode == VS_PATH_CYCLE_RESTART)
        return NULL;

    // Out-of-bounds and CLOSED LOOP; wrap around.
    return (vsPathMotionSegment *)
        (pointList.getEntry((idx + pointCount) % pointCount));
}

// ------------------------------------------------------------------------
// Return the vsKinematics controlled by this path motion
// ------------------------------------------------------------------------
vsKinematics *vsPathMotion::getKinematics()
{
    return objectKin;
}

// ------------------------------------------------------------------------
// A function to change the kinematics object that this path motion holds.
// ------------------------------------------------------------------------
void vsPathMotion::setKinematics(vsKinematics *newKin)
{
    vsObject::unrefDelete(objectKin);
    objectKin = newKin;
    objectKin->ref();
}
