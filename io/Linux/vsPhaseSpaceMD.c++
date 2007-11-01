//------------------------------------------------------------------------
//
//    VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//    Copyright (c) 2005, University of Central Florida
//
//       See the file LICENSE for license information
//
//    E-mail:  vess@ist.ucf.edu
//    WWW:     http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//    VESS Module:  vsPhaseSpaceMD.c++
//
//    Description:  VESS I/O driver for the PhaseSpace Motion Digitizer,
//                  an active LED-based optical tracking system.  Since 
//                  the client-server communications are kept confidential
//                  by PhaseSpace, this implementation relies on the 
//                  PhaseSpace OWL API (libowl.so)
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#include "vsPhaseSpaceMD.h++"
#include "owl.h"
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

// ------------------------------------------------------------------------
// Creates a connection to a PhaseSpace server on the given host.  The
// master flag indicates whether this client can control certain parameters
// on the host (only one master client is allowed).  The postprocess flag
// indicates whether the client library should postprocess the data for
// extra filtering and recovery, at the cost of extra latency and CPU
// consumption.  The mode parameter is not used (as only Mode 4 is 
// operational at this time).
// ------------------------------------------------------------------------
vsPhaseSpaceMD::vsPhaseSpaceMD(const char *serverName, bool master,
                               bool postprocess, int mode)
{
    unsigned int clientFlags;
    OWLCamera *cameraData;
    int owlConnectResult;
    int i;

    // Initialize tracker count and arrays
    numTrackers = 0;
    memset(trackers, 0, sizeof(trackers));
    for (i = 0; i < VS_PSMD_MAX_TRACKERS; i++)
        trackerType[i] = VS_PSMD_INVALID_TRACKER;

    // We don't know the tracker mode yet (rigid or point)...
    this->mode = VS_PSMD_MODE_NONE;

    // Initialize other variables
    threaded = false;
    quitFlag = false;
    streaming = false;
    reportRate = VS_PSMD_DEFAULT_REPORT_RATE;
    
    // Set up the client flags
    clientFlags = 0;
    if (!master)
       clientFlags |= OWL_SLAVE;
    if (postprocess)
       clientFlags |= OWL_POSTPROCESS;

    // Always use mode 4 (it's the only one supported by the OWL library
    // so far
    if (mode != OWL_MODE4)
    {
        printf("vsPhaseSpaceMD::vsPhaseSpaceMD:\n");
        printf("  Mode 4 is the only mode supported at this time.\n");
    }
    
    // The flags were set to zero in the example app, so until this is 
    // better understood I'm using what is known to work --D.Smith
    //clientFlags |= OWL_MODE4;

    // Create the client link to the PhaseSpace server
    owlConnectResult = owlInit(serverName, clientFlags);
    if (owlConnectResult < 0)
    {
        printf("vsPhaseSpaceMD::vsPhaseSpaceMD:\n"
               "  ERROR connecting to Phasespace server.\n");
    }
    else
    {
        printf("vsPhaseSpaceMD::vsPhaseSpaceMD: connected to server %s...",
                serverName);
                    
        // Store the master flag
        this->master = master; 
    
        // Retrieve the camera data
        cameraData = new OWLCamera[VS_PSMD_MAX_CAMERAS];
        numCameras = owlGetCameras(cameraData, VS_PSMD_MAX_CAMERAS);
    
        // Copy the camera data to our array of cameras
        printf("vsPhaseSpaceMD::vsPhaseSpaceMD: storing data for %d cams...\n",
                    numCameras);

        for (i = 0; i < numCameras; i++)
        {
            cameras[i].id = cameraData[i].id;
            cameras[i].position[AT_X] = cameraData[i].pose[0];
            cameras[i].position[AT_Y] = cameraData[i].pose[1];
            cameras[i].position[AT_Z] = cameraData[i].pose[2];
            cameras[i].orientation[VS_W] = cameraData[i].pose[3];
            cameras[i].orientation[AT_X] = cameraData[i].pose[4];
            cameras[i].orientation[AT_Y] = cameraData[i].pose[5];
            cameras[i].orientation[AT_Z] = cameraData[i].pose[6];
        }
    
        // Clean up the OWLCamera structures
        delete [] cameraData;

        // Initialize all vsMotionTracker object pointers to NULL
        for (i = 0; i < VS_PSMD_MAX_TRACKERS; i++)
        {
            trackers[i] = NULL;
            privateTrackers[i] = NULL;
            privateConfidence[i] = 0.0;
        }
    }
}

// ------------------------------------------------------------------------
// Clean up the data structures we've created, and terminate the worker
// thread, if we started one
// ------------------------------------------------------------------------
vsPhaseSpaceMD::~vsPhaseSpaceMD()
{
    int i;

    if (threaded)
    {
        // If we're threaded, signal the worker thread to terminate
        // and wait for it to finish what it's doing
        quitFlag = true;
        pthread_join(threadID, NULL);
        threaded = false;
    }
    else if (streaming)
    {
        // If we're not threaded, but we're currently streaming, stop
        // the data stream
        stopStream();
    }

    // Disable the LED markers 
    disableMarkerData();

    // Destroy the hardware tracker objects we've created
    if (mode == VS_PSMD_MODE_POINT)
    {
        // Destroy the single point tracker we always create in
        // point tracking mode, if we're the master client
        if (master)
            owlTracker(0, OWL_DESTROY);
    }
    else
    {
        // Destroy all rigid body trackers that have been created, if we're
        // the master client
        if (master)
        {
            for (i = 0; i < numTrackers; i++)
                owlTracker(i, OWL_DESTROY);
        }
    }

    // Wait a couple of seconds before we shut down the server link
    sleep(2);

    // Shut down the link to the PhaseSpace server
    owlDone();

    // Destroy all VESS motion trackers we've created
    for (i = 0; i < VS_PSMD_MAX_TRACKERS; i++) 
    {
        if (trackers[i] != NULL)
        {
            delete trackers[i];
        }
        trackers[i] = NULL;
    }
}

// ------------------------------------------------------------------------
// Specifies whether this system will be using point or rigid trackers 
// ------------------------------------------------------------------------
void vsPhaseSpaceMD::setTrackerMode(int mode)
{
    // Make sure that a valid mode (point or rigid) was passed
    if (mode != VS_PSMD_MODE_POINT && 
        mode != VS_PSMD_MODE_RIGID)
    {
        printf("vsPhaseSpaceMD::setTrackerMode: "
               "Invalid mode (%d). Mode unchanged.\n", mode);
        return;
    }
    
    // Set the mode
    this->mode = mode;

    if (mode == VS_PSMD_MODE_POINT)
    {
        printf("Configuring point tracking mode . . .\n");

        // Only 1 tracker and No markers yet...
        numTrackers = 1;
        numMarkers = 0;

        // Create the hardware tracker (if we're the master client)
        if (master)
        {
            printf("\tCreating tracker 0 as point tracker... ");
            owlTrackeri(0, OWL_CREATE, OWL_POINT_TRACKER);

            // Check for errors...
            if (owlGetStatus() == 0)
            {
                printf("%s\n", getErrorString());
            }
            else if(owlGetStatus() == 1)
            {
                printf("OK!\n");
            }
            else
                printf("Unknown result.\n");
        }
    }
    else if (mode == VS_PSMD_MODE_RIGID)
    {
        printf("Configuring rigid body tracking mode . . .\n");

        // No trackers and no markers yet...
        numTrackers = 0;
        numMarkers = 0;
    }
}

// ------------------------------------------------------------------------
// Returns whether this system is using point or rigid trackers 
// ------------------------------------------------------------------------
int vsPhaseSpaceMD::getTrackerMode()
{
    return mode;
}

// ------------------------------------------------------------------------
// Updates the data in the motion tracker arrays.
// ------------------------------------------------------------------------
void vsPhaseSpaceMD::updateSystem()
{
    OWLRigid rigids[VS_PSMD_MAX_TRACKERS];
    OWLMarker markers[VS_PSMD_MAX_TRACKERS];
    int markerCount, rigidCount;
    bool found;
    atVector position;
    atQuat orientation;
    atQuat rotQuat;
    int i, j;

    // Rotation quat
    rotQuat.setAxisAngleRotation(1, 0, 0, 90.0);

    // Get the marker and rigid body data
    if (mode == VS_PSMD_MODE_POINT)
    {
        markerCount = owlGetMarkers(markers, VS_PSMD_MAX_TRACKERS);
        
        // Update the vsMotionTracker data
        for (i = 0; i < markerCount; i++)
        {
            // Don't update the tracker position unless the system is
            // confident about its measurement
            if (markers[i].cond > 0.0)
            {
                // Extract the marker data
                position.set(markers[i].x, markers[i].y, markers[i].z);
           
                // Phasespace coordinate system uses the OpenGL coord 
                // system.  We need to rotate -90 deg about the X-axis to 
                // convert it to VESS coordinates
                position = rotQuat.rotatePoint(position);     
        
                // See if we're running threaded or not
                if (threaded)
                {
                    // Update the private tracker data
                    pthread_mutex_lock(&trackerDataMutex); 
                    privateTrackers[i]->setPosition(position);
                    privateConfidence[i] = markers[i].cond;
                    pthread_mutex_unlock(&trackerDataMutex);
                }
                else    
                {
                    // Update the public tracker data
                    trackers[i]->setPosition(position);            
                    confidence[i] = markers[i].cond;
                }
            }
            else
            {
                // Don't update the position, just the confidence value.
                // See if we're running threaded or not
                if (threaded)
                {
                    // Update the private confidence value
                    pthread_mutex_lock(&trackerDataMutex); 
                    privateConfidence[i] = markers[i].cond;
                    pthread_mutex_unlock(&trackerDataMutex);
                }
                else    
                {
                    // Update the public confidence value
                    confidence[i] = markers[i].cond;
                }
            }
        }
    }
    else if (mode == VS_PSMD_MODE_RIGID)
    {
        rigidCount = owlGetRigid(rigids, numTrackers);
        
        // Update the vsMotionTracker data
        for (i = 0; i < rigidCount; i++)
        {
            // Don't update the tracker position unless the system is
            // confident about its measurement
            if (rigids[i].cond > 0.0)
            {
                // Extract the rigid body data
                position.set(rigids[i].pose[0], rigids[i].pose[1], 
                    rigids[i].pose[2]);
                orientation.set(rigids[i].pose[4], rigids[i].pose[5], 
                    rigids[i].pose[6], rigids[i].pose[3]);
           
                // Phasespace coordinate system uses the OpenGL coord 
                // system.  We need to rotate -90 deg about the X-axis to 
                // convert it to VESS coordinates
                position = rotQuat.rotatePoint(position);     
                orientation = rotQuat * orientation * rotQuat.getInverse();
        
                // See if we're running threaded or not
                if (threaded)
                {
                    // Update the private tracker data
                    pthread_mutex_lock(&trackerDataMutex); 
                    privateTrackers[i]->setPosition(position);
                    privateTrackers[i]->setOrientation(orientation);
                    privateConfidence[i] = rigids[i].cond;
                    pthread_mutex_unlock(&trackerDataMutex);
                }
                else    
                {
                    // Update the public tracker data
                    trackers[i]->setPosition(position);            
                    trackers[i]->setOrientation(orientation);            
                    confidence[i] = rigids[i].cond;
                }
            }
            else
            {
                // Don't update the position, just the confidence value.
                // See if we're running threaded or not
                if (threaded)
                {
                    // Update the private confidence value
                    pthread_mutex_lock(&trackerDataMutex); 
                    privateConfidence[i] = rigids[i].cond;
                    pthread_mutex_unlock(&trackerDataMutex);
                }
                else    
                {
                    // Update the public confidence value
                    confidence[i] = rigids[i].cond;
                }
            }
        }
    }
}

// ------------------------------------------------------------------------
// The function for the worker thread.  If forkTracking() is called, a new
// thread will be started that executes this looping method to constantly
// update the tracker data.
// ------------------------------------------------------------------------
void *vsPhaseSpaceMD::threadLoop(void *objectPtr)
{
    vsPhaseSpaceMD *instance;
    int *returnValue;
    int i;
    int numVsMotionTrackers = 0;

    // This is a static method, so we need to get a pointer to the instance
    // that spawned this thread
    instance = (vsPhaseSpaceMD *)objectPtr;

    // Start streaming data from the server
    instance->startStream();

    // Keep updating tracker data as long as we're not signaled to quit
    while (!instance->quitFlag)
    {
        // Update the tracker data
        instance->updateSystem();

        // Wait for a bit, then do it again
        usleep(10000);
    }

    // Delete the set of private trackers
    for (i = 0; i < VS_PSMD_MAX_TRACKERS; i++)
    {
        if (instance->privateTrackers[i] != NULL)
        {
            delete instance->privateTrackers[i];
            instance->privateTrackers[i] = NULL;
        }
    }

    // Stop streaming data
    instance->stopStream();

    // Free the tracker data mutex
    pthread_mutex_destroy(&instance->trackerDataMutex);

    // Exit the thread
    returnValue = new int;
    *returnValue = 0;
    pthread_exit(returnValue);

    return 0;
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsPhaseSpaceMD::getClassName()
{
    return "vsPhaseSpaceMD";
}

// ------------------------------------------------------------------------
// Return the number of trackers configured in the system
// ------------------------------------------------------------------------
int vsPhaseSpaceMD::getNumTrackers()
{
    // The number of trackers returned depends on whether we're using
    // point trackers or rigid bodies
    if (mode == VS_PSMD_MODE_POINT)
    {
        // Return the number of LED markers (point trackers)
        return numMarkers;
    }
    else if (mode == VS_PSMD_MODE_RIGID)
    {
        // Return the number of rigid body trackers
        return numTrackers;
    }
    else
    {
        // What mode is this?
        return 0;
    }
}

// ------------------------------------------------------------------------
// Return the tracker corresponding to the index given
// ------------------------------------------------------------------------
vsMotionTracker *vsPhaseSpaceMD::getTracker(int index)
{
    // How many vsMotionTracker objects do we have?
    int numVsMotionTrackers = 0;

    if (mode == VS_PSMD_MODE_POINT)
    {
        // Each MARKER holds value for the vsMotionTracker
        numVsMotionTrackers = numMarkers;
    }
    else if (mode == VS_PSMD_MODE_RIGID)
    {
        // Each TRACKER holds value for the vsMotionTracker
        numVsMotionTrackers = numTrackers;
    }
    
    // If the index given is out of bounds, return NULL.  Otherwise, return
    // the requested tracker.
    if ((index < 0) || (index > numVsMotionTrackers))
        return NULL;
    else
        return trackers[index];
}

// ------------------------------------------------------------------------
// Return the type of the tracker corresponding to the index given
// ------------------------------------------------------------------------
vsPSMDTrackerType vsPhaseSpaceMD::getTrackerType(int index)
{
    // If the index given is out of bounds, return that it is an INVALID
    // tracker.  Otherwise, return the type of the corresponding tracker.
    if ((index < 0) || (index > getNumTrackers()))
        return VS_PSMD_INVALID_TRACKER;
    else
        return trackerType[index];
}

// ------------------------------------------------------------------------
// Sets the scale factor of any positions reported by the tracking system.
//
// NOTE:  If this method is going to be used, it MUST be called before
//        constructing any rigid body trackers (with createRigidTracker())
// ------------------------------------------------------------------------
void vsPhaseSpaceMD::setScale(float newScale)
{
    // Don't do anything here if we're not the master client
    if (!master)
        return;

    // Make sure the scale factor is a positive floating point number
    if (newScale <= 0.0)
    {
        printf("vsPhaseSpaceMD::setScale:  Scale factor %g is invalid\n",
            newScale);
        return;
    }

    // If we've already created rigid body trackers, let the user know that
    // things might go weird after this.
    if (mode == VS_PSMD_MODE_RIGID)
    {
        printf("vsPhaseSpaceMD::setScale:  Scale factor should be set "
            "before rigid body\n");
        printf("    trackers are created.  Tracker data may become "
            "unreliable.\n");
    }

    // Set the new scale factor
    owlScale(newScale);
}


// ------------------------------------------------------------------------
// Sets the frame of reference for the tracking space.  The position and
// orientation given are applied as offsets to camera 0's real-world
// position and orientation.  Without any offsets, the origin of the 
// tracked space is camera 0.
// ------------------------------------------------------------------------
void vsPhaseSpaceMD::setReferenceFrame(atVector position, atQuat orientation)
{
    // Don't do anything here if we're not the master client
    if (!master)
        return;

    float pose[7];

    // Convert the vector and quat to an array of seven floats in the right
    // order
    pose[0] = position[AT_X];
    pose[1] = position[AT_Y];
    pose[2] = position[AT_Z];
    pose[3] = orientation[VS_W];
    pose[4] = orientation[AT_X];
    pose[5] = orientation[AT_Y];
    pose[6] = orientation[AT_Z];

    // Send the new pose to the system
    owlLoadPose(pose);
}

// ------------------------------------------------------------------------
// Enables the reporting of buttons in the data stream from the tracker
// ------------------------------------------------------------------------
void vsPhaseSpaceMD::enableButtonData()
{
    // Send the command to enable the button data in the tracking reports
    owlSetInteger(OWL_BUTTONS, OWL_ENABLE);
}

// ------------------------------------------------------------------------
// Disables the reporting of buttons in the data stream from the tracker
// ------------------------------------------------------------------------
void vsPhaseSpaceMD::disableButtonData()
{
    // Don't do anything here if we're not the master client
    if (!master)
        return;

    // Send the command to disable the button data in the tracking reports
    owlSetInteger(OWL_BUTTONS, OWL_DISABLE);
}

// ------------------------------------------------------------------------
// Enables the reporting of marker positions in the data stream from the 
// tracker
// ------------------------------------------------------------------------
void vsPhaseSpaceMD::enableMarkerData()
{
    // Send the command to enable the marker data in the tracking reports
    owlSetInteger(OWL_MARKERS, OWL_ENABLE);
}

// ------------------------------------------------------------------------
// Disables the reporting of marker positions in the data stream from the 
// tracker
// ------------------------------------------------------------------------
void vsPhaseSpaceMD::disableMarkerData()
{
    // Don't do anything here if we're not the master client
    if (!master)
        return;

    // Send the command to disable the marker data in the tracking reports
    owlSetInteger(OWL_MARKERS, OWL_DISABLE);
}

// ------------------------------------------------------------------------
// Change the number of frames that are interpolated to arrive at the final
// data report for any given frame.  The default is 4 frames.
// ------------------------------------------------------------------------
void vsPhaseSpaceMD::setInterpolationInterval(int numFrames)
{
    // Don't do anything here if we're not the master client
    if (!master)
        return;

    // Make sure numFrames is a non-negative integer
    if (numFrames < 0)
    {
        printf("vsPhaseSpaceMD::setInterpolationValue:\n");
        printf("    The number of interpolation frames must be 0 or "
            "greater.\n");
        return;
    }

    // Send the interpolation command
    owlSetInteger(OWL_INTERPOLATION, numFrames);
}

// ------------------------------------------------------------------------
// Set the rate at which data is sent from the tracking system (this has
// nothing to do with the measurement rate of the hardware).
// ------------------------------------------------------------------------
void vsPhaseSpaceMD::setReportRate(float newRate)
{
    // Don't do anything here if we're not the master client
    if (!master)
        return;

    // Make sure the report rate is valid
    if ((newRate < 0.0) || (newRate > OWL_MAX_FREQUENCY))
    {
        printf("vsPhaseSpaceMD::setReportRate:\n");
        printf("    Report rate must be between %0.2f and %0.2f\n",
            0.0f, OWL_MAX_FREQUENCY);
        return;
    }

    // Set the new report rate internally
    reportRate = newRate;

    // Send the command to adjust the report rate, if we're streaming data
    if (streaming)
        owlSetFloat(OWL_FREQUENCY, reportRate);
    
    // If the report rate is set to zero, this is the same as disabling 
    // streaming of data.  We need to adjust some internal variables if
    // this happens.
    if (fabs(newRate - 1.0E-6) < 0)
    {
        // Set the streaming variable to false, indicating we're no longer
        // streaming data
        streaming = false;

        // If the rate is near zero, we should make sure it's exactly zero
        reportRate = 0.0;
    }
}

// ------------------------------------------------------------------------
// Returns the number of cameras in the system
// ------------------------------------------------------------------------
int vsPhaseSpaceMD::getNumCameras()
{
    return numCameras;
}

// ------------------------------------------------------------------------
// Returns information about the given camera
// ------------------------------------------------------------------------
vsPSMDCamera *vsPhaseSpaceMD::getCamera(int index)
{
    // Make sure the camera index is valid, and return NULL if not
    if ((index < 0) || (index >= numCameras))
    {
        printf("vsPhaseSpaceMD::getCamera:  Camera index %d is invalid\n",
            index);
        return NULL;
    }

    // Return a pointer to the requested camera data
    return &cameras[index];
}

// ------------------------------------------------------------------------
// Creates a point tracker using the given tracker index, and LED 
// addresses.  Unlike many other tracking systems, the PhaseSpace system
// must be configured at run-time using this method and/or the
// createRigidTracker() method.
// ------------------------------------------------------------------------
void vsPhaseSpaceMD::createPointTracker(int ledIndex)
{
    // If we are in rigid body mode, print error and return...
    if (mode == VS_PSMD_MODE_RIGID)
    {
        printf("vsPhaseSpaceMD::createPointTracker: \n"
               "\tCannot create point tracker in rigid body mode.\n");
        return;
    }

    // If we are not already in point tracker mode, set it
    if (mode == VS_PSMD_MODE_NONE)
    {
        printf("Automatically setting mode to point tracker mode...\n");

        // Set the mode
        setTrackerMode(VS_PSMD_MODE_POINT);
    }

    // Only configure the server side if we're the master client
    if (master)
    {
        // Associate the given LED with the tracker
        printf("\tCreating marker for point tracker using "
               "LED %d... ", ledIndex); 
        owlMarkeri(MARKER(0, numMarkers), OWL_SET_LED, ledIndex);

        // Check for errors...
        if (owlGetStatus() == 0)
        {
            printf("%s\n", getErrorString());
        }
        else if(owlGetStatus() == 1)
        {
            printf("OK!\n");
        }
        else
            printf("Unknown result.\n");
    }

    // Create a vsMotionTracker to hold the data for this marker
    trackers[numMarkers] = new vsMotionTracker(numMarkers);
    privateTrackers[numMarkers] = new vsMotionTracker(numMarkers);
    trackerType[numMarkers] = VS_PSMD_POINT_TRACKER;

    // Increment the number of trackers
    numMarkers++;
}

// ------------------------------------------------------------------------
// Creates a rigid body tracker using the given tracker index, LED 
// addresses, and LED offsets.  Unlike many other tracking systems, the 
// PhaseSpace system must be configured at run-time using this method 
// and/or the createPointTracker() method.
// ------------------------------------------------------------------------
void vsPhaseSpaceMD::createRigidTracker(int ledCount, int *ledIndices, 
                                        atVector *ledOffsets)
{
    int i;
    float offset[3];

    // If we are in point mode, print error and return...
    if (mode == VS_PSMD_MODE_POINT)
    {
        printf("vsPhaseSpaceMD::createRigidTracker: \n"
               "\tCannot create rigid body tracker in point mode.\n");
        return;
    }

    // If we are not already in rigid body tracker mode, set it
    if (mode == VS_PSMD_MODE_NONE)
    {
        printf("Automatically setting mode to rigid body tracker mode...\n");

        // Set the mode
        setTrackerMode(VS_PSMD_MODE_RIGID);
    }

    // Only configure the server side if we're the master client
    if (master)
    {
        // Create the hardware tracker
        printf("\tCreating rigid body tracker #%d...", numTrackers); 
        owlTrackeri(numTrackers, OWL_CREATE, OWL_RIGID_TRACKER);

        // Check for errors...
        if (owlGetStatus() == 0)
            printf("%s\n", getErrorString());
        else if(owlGetStatus() == 1)
            printf("OK!\n");
        else
            printf("Unknown result.\n");

        // Associate and configure each given LED marker
        for (i = 0; i < ledCount; i++)
        {
            printf("   Adding marker using LED %d, (offset: %0.2lf, %0.2lf, "
                "%0.2lf)... ", ledIndices[i], ledOffsets[i][AT_X], 
                ledOffsets[i][AT_Y], ledOffsets[i][AT_Z]); 

            // Associate the LED with the tracker
            owlMarkeri(MARKER(numTrackers, i), OWL_SET_LED, ledIndices[i]);

            // Set the offset of this LED from the rigid body's origin
            offset[AT_X] = ledOffsets[i][AT_X];
            offset[AT_Y] = ledOffsets[i][AT_Y];
            offset[AT_Z] = ledOffsets[i][AT_Z];
            owlMarkerfv(MARKER(numTrackers, i), OWL_SET_POSITION, offset);

            // Check for errors...
            if (owlGetStatus() == 0)
                printf("%s\n", getErrorString());
            else if(owlGetStatus() == 1)
                printf("OK!\n");
            else
                printf("Unknown result.\n");
        }
    }

    // Create a vsMotionTracker to hold the data for this rigid body tracker
    trackers[numTrackers] = new vsMotionTracker(numTrackers);
    privateTrackers[numTrackers] = new vsMotionTracker(numTrackers);
    trackerType[numTrackers] = VS_PSMD_RIGID_BODY_TRACKER;

    // Increment the number of trackers
    numTrackers++;
}

// ------------------------------------------------------------------------
// Enables the given tracker
// ------------------------------------------------------------------------
void vsPhaseSpaceMD::enableTracker(int trackerNum)
{
    // Don't do anything here if we're not the master client
    if (!master)
        return;

    // Enable the tracker
    owlTracker(trackerNum, OWL_ENABLE);	
}

// ------------------------------------------------------------------------
// Disables the given tracker
// ------------------------------------------------------------------------
void vsPhaseSpaceMD::disableTracker(int trackerNum)
{
    // Don't do anything here if we're not the master client
    if (!master)
        return;

    // Disable the tracker
    owlTracker(trackerNum, OWL_DISABLE);	
}

// ------------------------------------------------------------------------
// Returns the confidence of the last measurement of the given tracker.
// A negative value means the tracker has been lost (i.e.: it was not
// visible during the last measurement cycle).
// ------------------------------------------------------------------------
float vsPhaseSpaceMD::getTrackerConfidence(int index)
{
    // If the given tracker index is invalid, report the confidence as if
    // the tracker is lost
    if ((index < 0) || (index > getNumTrackers()))
        return -1.0f;

    // Return the tracker's confidence value
    return confidence[index];
}

// ------------------------------------------------------------------------
// Starts the continuous streaming of data from the tracking system
// ------------------------------------------------------------------------
void vsPhaseSpaceMD::startStream()
{
    // If the report rate is greater than zero, start streaming data from
    // the system.  If not, let the user know that they need to adjust the
    // report rate for streaming to work
    if (fabs(reportRate) > 1.0E-6)
    {
        owlSetFloat(OWL_FREQUENCY, reportRate);
        owlSetInteger(OWL_STREAMING, OWL_ENABLE);
        streaming = true;
    }
    else
    {
        printf("vsPhaseSpaceMD::startStream:\n");  
        printf("    Report rate is currently 0.0.  Set the report rate\n");
        printf("    positive value before calling startStream()\n");
    }
}

// ------------------------------------------------------------------------
// Halts the continuous streaming of data from the tracking system
// ------------------------------------------------------------------------
void vsPhaseSpaceMD::stopStream()
{
    // Don't do anything here if we're not the master client
    if (!master)
        return;

    // Set the report frequency to zero on the server.  This disables 
    // streaming.
    owlSetFloat(OWL_FREQUENCY, 0.0f);
    owlSetInteger(OWL_STREAMING, OWL_DISABLE);
    streaming = false;
}

// ------------------------------------------------------------------------
// Spawns a worker thread to collect tracker data and keep it until the
// main application thread is ready for it.
// ------------------------------------------------------------------------
void vsPhaseSpaceMD::forkTracking()
{
    int i;
    int errorCode;
    int numVsMotionTrackers;
    
    // The number of trackers we need depends on the mode
    if (mode == VS_PSMD_MODE_POINT)
    {
        // One vsMotionTracker for each marker
        numVsMotionTrackers = numMarkers;
    }
    else if (mode == VS_PSMD_MODE_RIGID)
    {
        // One vsMotionTracker for each tracker
        numVsMotionTrackers = numTrackers;
    }
    else
    {
        printf("vsPhaseSpaceMD::forkTracking: \n\t"
               "*** Can't fork tracking.  No mode/trackers set.\n");
        return;
    }

    // First, create the private array of vsMotionTracker objects to
    // store the data collected by the thread
    for (i = 0; i < numVsMotionTrackers; i++)
    {
        privateTrackers[i] = new vsMotionTracker();
    }

    // Create a mutex to control access to the main tracker data in order
    // to prevent race conditions
    pthread_mutex_init(&trackerDataMutex, NULL);

    // Create the worker thread
    errorCode = 
        pthread_create(&threadID, NULL, &vsPhaseSpaceMD::threadLoop, 
            (void *)this);
    if (errorCode == 0)
    {
        printf("vsPhaseSpaceMD::forkTracking:\n");
        printf("    Worker thread ID is %d\n", threadID);

        // Flag that the object is now multithreaded
        threaded = true;
    }
    else
    {
        // Thread creation failed, clean up the structures we created earlier
        printf("vsPhaseSpaceMD:: forkTracking:\n");
        printf("    Unable to spawn worker thread!\n");

        // Delete the private trackers
        for (i = 0; i < numVsMotionTrackers; i++)
            delete privateTrackers[i];
        memset(privateTrackers, 0, sizeof(privateTrackers));

        // Destroy the tracker data mutex
        pthread_mutex_destroy(&trackerDataMutex);
    }
}

// ------------------------------------------------------------------------
// Update the motion tracker's with fresh data.  If we're running in a
// single thread, this gets new data from the server.  If we're running
// multithreaded, this will just copy the latest data retrieved by the
// worker thread to the main data area.
// ------------------------------------------------------------------------
void vsPhaseSpaceMD::update()
{
    int i;
    int numVsMotionTrackers;
    
    if (mode == VS_PSMD_MODE_POINT)
    {
        // Each MARKER holds value for a vsMotionTracker
        numVsMotionTrackers = numMarkers;
    }
    else if (mode == VS_PSMD_MODE_RIGID)
    {
        // Each TRACKER holds value for a vsMotionTracker
        numVsMotionTrackers = numTrackers;
    }
    else 
    {
        // Something is presumably wrong if this line executes
        numVsMotionTrackers = 0;
    }
                                                                                
    // Check to see if we've spawned a worker thread
    if (threaded)
    {
        // Copy the latest tracker data from the worker thread.  Make
        // sure to use the tracker data mutex to avoid race conditions.
        pthread_mutex_lock(&trackerDataMutex);
        
        for (i = 0; i < numVsMotionTrackers; i++)
        {
            // Copy the private tracker data to the public trackers
            trackers[i]->setPosition(privateTrackers[i]->getPositionVec());
            trackers[i]->setOrientation(
                privateTrackers[i]->getOrientationQuat());
            confidence[i] = privateConfidence[i];
        }
        pthread_mutex_unlock(&trackerDataMutex);
    }
    else
    {
        // Get the data directly from the server
        updateSystem();
    }
}

// ------------------------------------------------------------------------
// Returns a string describing the error as returned by owlGetError()
// ------------------------------------------------------------------------
const char *vsPhaseSpaceMD::getErrorString()
{
    switch (owlGetError())
    {
        case OWL_NO_ERROR:
            return "No error";
        case OWL_INVALID_VALUE:
            return "**ERROR: Invalid value";
        case OWL_INVALID_ENUM:
            return "**ERROR: Invalid enum";
        case OWL_INVALID_OPERATION:
            return "**ERROR: Invalid operation";
        default:
            return "Unknown";
    }
}
    
