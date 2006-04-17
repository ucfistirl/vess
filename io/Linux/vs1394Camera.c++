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
//    VESS Module:  vs1394Camera.c++
//
//    Description:  Input system for retrieving video data from an IIDC
//                  compliant camera attached to the computer via a 1394
//                  connection
//
//    Author(s):    Bryan Kline, Casey Thurston
//
//------------------------------------------------------------------------

#include "vs1394Camera.h++"

#include <string.h>

//------------------------------------------------------------------------
// Constructor
// Constructs the object to use the first camera it finds; the first
// camera on the first 1394 bus installed in the system.
//------------------------------------------------------------------------
vs1394Camera::vs1394Camera()
{
    // Initially, we aren't connected to a camera
    validCamera = false;
    activeStream = false;
    calibrationEnabled = false;

    // The stream should be null until it is created.
    videoQueue = NULL;
    currentFrameData = NULL;

    // Initialize the semaphores.
    pthread_mutex_init(&signalMutex, NULL);
    pthread_mutex_init(&cameraMutex, NULL);

    // Attempt to connect to the first camera on the first bus
    connectToCamera(0, 0);

    // If there was a problem connecting, print an error message
    if (!validCamera)
    {
        printf("vs1394Camera::vs1394Camera: Cannot establish connection to "
            "camera\n");
        return;
    }
}

//------------------------------------------------------------------------
// Constructor
// Constructs the object to use the specified camera on the specified
// 1394 bus. Both indices are zero based; an input of zero means the
// first bus or camera.
//------------------------------------------------------------------------
vs1394Camera::vs1394Camera(int busIndex, int cameraIndex)
{
    // Initially, we aren't connected to a camera
    validCamera = false;
    activeStream = false;
    calibrationEnabled = false;

    // The stream should be null until it is created.
    videoQueue = NULL;
    currentFrameData = NULL;

    // Initialize the semaphores.
    pthread_mutex_init(&signalMutex, NULL);
    pthread_mutex_init(&cameraMutex, NULL);

    // Attempt to connect to the specified camera on the specified bus
    connectToCamera(busIndex, cameraIndex);

    // If there was a problem connecting, print an error message
    if (!validCamera)
    {
        printf("vs1394Camera::vs1394Camera: Cannot establish connection to "
            "specified camera\n");
        return;
    }
}

//------------------------------------------------------------------------
// Destructor
// Stops the camera stream if needed before disposing of the connection
//------------------------------------------------------------------------
vs1394Camera::~vs1394Camera()
{
    // If we're currently connected to a camera, dispose of that connection
    if (validCamera)
        disconnectFromCamera();
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vs1394Camera::getClassName()
{
    return "vs1394Camera";
}

//------------------------------------------------------------------------
// Updates the camera by fetching the next frame of video from the camera
// if a video stream is currently open.
//------------------------------------------------------------------------
void vs1394Camera::update()
{
    // If there's no camera, there's nothing to do
    if (!validCamera)
        return;

    // If we're not streaming, there's nothing to do
    if (!activeStream)
        return;

    // Peek the top frame from the video stream into the current data pointer.
//    videoQueue->dequeue((char *)currentFrameData, NULL, videoReferenceID);

    // If calibration mode is enabled, then perform the calibration here
    if (calibrationEnabled)
    {
        calibrateColor();
        calibrateBrightness();
    }
}

//------------------------------------------------------------------------
// Closes the connection to the current camera, and attempts to establish
// a connection with the specified camera. Both indices are zero based; an
// input of zero means the first bus or camera.
//------------------------------------------------------------------------
void vs1394Camera::selectCamera(int busIndex, int cameraIndex)
{
    // If we're already connected to a camera, close that connection
    if (validCamera)
        disconnectFromCamera();

    // Attempt to connect to the new camera
    connectToCamera(busIndex, cameraIndex);

    // If there was a problem connecting, print an error message
    if (!validCamera)
    {
        printf("vs1394Camera::selectCamera: Cannot establish connection to "
            "specified camera\n");
        return;
    }
}

//------------------------------------------------------------------------
// Queries the current camera to determine if it is capable of handling
// video frames of the specified size
//------------------------------------------------------------------------
bool vs1394Camera::isValidFrameSize(int size)
{
    quadlet_t formats, modes;

    // If there's no connected camera, fail
    if (!validCamera)
        return false;

    // Check the supported transmission formats bitfield to determine if the
    // format mode required for the specified frame size is supported by the
    // camera. If so, then query if the exact frame size specified is also
    // supported.

    // Acquire exclusive access to the camera.
    pthread_mutex_lock(&cameraMutex);

    // Get the current supported formats
    if (dc1394_query_supported_formats(busHandle, cameraNodeID, &formats)
        != DC1394_SUCCESS)
    {
        // Yield exclusive access to the camera.
        pthread_mutex_unlock(&cameraMutex);

        printf("vs1394Camera::isValidFrameSize: "
            "Error communicating with camera\n");
        return false;
    }

    // Check for format compatability
    if (!(formats & getFormatMask(size)))
    {
        // Yield exclusive access to the camera.
        pthread_mutex_unlock(&cameraMutex);

        return false;
    }

    // Get the supported modes for the desired format
    if (dc1394_query_supported_modes(busHandle, cameraNodeID,
        getFormatConst(size), &modes) != DC1394_SUCCESS)
    {
        // Yield exclusive access to the camera.
        pthread_mutex_unlock(&cameraMutex);

        printf("vs1394Camera::isValidFrameSize: "
            "Error communicating with camera\n");
        return false;
    }

    // Yield exclusive access to the camera.
    pthread_mutex_unlock(&cameraMutex);

    // Check for mode compatability
    if (!(modes & getModeMask(size)))
        return false;

    // Mode is supported
    return true;
}

//------------------------------------------------------------------------
// Sets the video frame size of the current camera to the specified size.
// Will fail if the camera cannot handle frames of the specified size.
// * Note that if a stream is already open, that this change will not take
// effect immediately; the stream must be closed and then reopened to
// change its size.
//------------------------------------------------------------------------
void vs1394Camera::setFrameSize(int size)
{
    // If there's no connected camera, fail
    if (!validCamera)
        return;

    // Make sure that the camera is capable of handling this size
    if (!(isValidFrameSize(size)))
        return;

    // Store the new frame size
    frameSize = size;
}

//------------------------------------------------------------------------
// Gets the video frame size of the current camera
//------------------------------------------------------------------------
int vs1394Camera::getFrameSize()
{
    return frameSize;
}

//------------------------------------------------------------------------
// Convenience function that gets the width of one frame of data from the
// camera
//------------------------------------------------------------------------
int vs1394Camera::getFrameWidth()
{
    // Get the frame width from the frame size constant
    switch (frameSize)
    {
        case VS_1394CAM_SIZE_640X480:
            return 640;
        case VS_1394CAM_SIZE_800X600:
            return 800;
        case VS_1394CAM_SIZE_1024X768:
            return 1024;
        case VS_1394CAM_SIZE_1280X960:
            return 1280;
        case VS_1394CAM_SIZE_1600X1200:
            return 1600;
        default:
            return 0;
    }

    return 0;
}

//------------------------------------------------------------------------
// Convenience function that gets the height of one frame of data from the
// camera
//------------------------------------------------------------------------
int vs1394Camera::getFrameHeight()
{
    // Get the frame height from the frame size constant
    switch (frameSize)
    {
        case VS_1394CAM_SIZE_640X480:
            return 480;
        case VS_1394CAM_SIZE_800X600:
            return 600;
        case VS_1394CAM_SIZE_1024X768:
            return 768;
        case VS_1394CAM_SIZE_1280X960:
            return 960;
        case VS_1394CAM_SIZE_1600X1200:
            return 1200;
        default:
            return 0;
    }

    return 0;
}

//------------------------------------------------------------------------
// Queries the current camera to determine if it is capable of streaming
// at the specified frame rate while using the current frame size.
//------------------------------------------------------------------------
bool vs1394Camera::isValidFrameRate(int rate)
{
    quadlet_t frameRates;

    // If there's no connected camera, fail
    if (!validCamera)
        return false;

    // Check the supported transmission framerates bitfield to determine if
    // the frame rate specified is supported by the camera for the current
    // frame size.

    // Acquire exclusive access to the camera.
    pthread_mutex_lock(&cameraMutex);

    // Get the current supported framerates
    if (dc1394_query_supported_framerates(busHandle, cameraNodeID,
        getFormatConst(frameSize), getModeConst(frameSize), &frameRates)
        != DC1394_SUCCESS)
    {
        // Yield exclusive access to the camera.
        pthread_mutex_unlock(&cameraMutex);

        printf("vs1394Camera::isValidFrameRate: "
            "Error communicating with camera\n");
        return false;
    }

    // Yield exclusive access to the camera.
    pthread_mutex_unlock(&cameraMutex);

    // Check for compatability
    if (!(frameRates & getFramerateMask(rate)))
        return false;

    // Frame rate supported
    return true;
}

//------------------------------------------------------------------------
// Sets the frame rate of the current camera to the specified rate. Will
// fail if the camera cannot transmit at the specified rate using the
// current frame size.
// * Note that if a stream is already open, that this change will not take
// effect immediately; the stream must be closed and then reopened to
// change its speed.
//------------------------------------------------------------------------
void vs1394Camera::setFrameRate(int rate)
{
    // If there's no connected camera, fail
    if (!validCamera)
        return;

    // Make sure that the camera is capable of handling this frame rate
    if (!(isValidFrameRate(rate)))
        return;

    // Store the new frame rate
    frameRate = rate;
}

//------------------------------------------------------------------------
// Gets the frame rate of the current camera
//------------------------------------------------------------------------
int vs1394Camera::getFrameRate()
{
    return frameRate;
}

//------------------------------------------------------------------------
// Sets the devfs device name to connect to for video streaming
// * Note that if a stream is already open, that this change will not take
// effect immediately; the stream must be closed and then reopened to
// change its speed.
//------------------------------------------------------------------------
void vs1394Camera::setDeviceName(char *deviceName)
{
    // Copy the name, truncating to 80 characters
    if (deviceName)
        strncpy(videoDeviceName, deviceName, 80);

    // Force the name to be null-terminated if a null character wasn't already
    // copied over
    videoDeviceName[80] = 0;
}

//------------------------------------------------------------------------
// Gets the devfs device name to connect to for video streaming
//------------------------------------------------------------------------
const char *vs1394Camera::getDeviceName()
{
    return videoDeviceName;
}

//------------------------------------------------------------------------
// Starts the video stream for the current camera. A video stream must
// be going before frames of video data can be retrieved from the camera.
//
// Create the thread.
//------------------------------------------------------------------------
void vs1394Camera::startStream()
{
    // If there's no connected camera, fail
    if (!validCamera)
        return;

    // If there's already a stream going, then there's nothing to do
    if (activeStream)
        return;

    // Attempt to set up the camera for DMA (video1394) transfer, including
    // setting up the camera information data structure
    if (dc1394_dma_setup_capture(busHandle, cameraNodeID, 0,
        getFormatConst(frameSize), getModeConst(frameSize), SPEED_400,
        getFramerateConst(frameRate), 4, 1, videoDeviceName, &cameraInfo)
        != DC1394_SUCCESS)
    {
        printf("vs1394Camera::startStream: Unable to setup camera\n");
        return;
    }

    // Attempt to start the flow of video data from the camera
    if (dc1394_start_iso_transmission(busHandle, cameraInfo.node)
        != DC1394_SUCCESS)
    {
        // Dispose of the cameraInfo structure's allocated data
        dc1394_release_camera(busHandle, &cameraInfo);

        printf("vs1394Camera::startStream: Unable to start data stream\n");
        return;
    }

    // Create a new video stream using the current frame data, storing an
    // arbitrary number of images.
    videoQueue = new vsVideoQueue(getFrameWidth(), getFrameHeight(), 5);
    //videoReferenceID = videoQueue->addReference();
    videoQueue->ref();

    // Allocate a pointer to hold the current frame locally. RGB format uses
    // three bytes per pixel.
    currentFrameData =
        (unsigned char *)malloc(getFrameWidth() * getFrameHeight() * 3);

    // For now, capture should be allowed to proceed.
    ceaseCapture = false;

    // Finally, create the thread.
    pthread_create(&captureThread, NULL, captureLoop, this);

    // Everything's working; mark the stream as going
    activeStream = true;

    // Calibration defaults to off
    calibrationEnabled = false;
}

//------------------------------------------------------------------------
// Stops the video stream from the current camera
//
// Close the thread.
//------------------------------------------------------------------------
void vs1394Camera::stopStream()
{
    // If there's no connected camera, fail
    if (!validCamera)
        return;

    // If there's no stream going, then there's nothing to do
    if (!activeStream)
        return;

    // Sieze control of the signalling semaphore.
    pthread_mutex_lock(&signalMutex);

    // Mark that the thread should finish execution.
    ceaseCapture = true;

    // Release the signalling semaphore to allow the loop to notice it.
    pthread_mutex_unlock(&signalMutex);

    // Wait until the capture thread has closed, disregarding its return value.
    pthread_join(captureThread, NULL);

    // Destroy the vsVideoQueue.
    vsObject::unrefDelete(videoQueue);
    videoQueue = NULL;

    // Free the current frame pointer.
    free(currentFrameData);
    currentFrameData = NULL;

    // Release the storage space used by the camera information structure
    dc1394_dma_release_camera(busHandle, &cameraInfo);

    // Mark the stream as stopped
    activeStream = false;
}

//------------------------------------------------------------------------
// Queries whether or not there is currently an active video stream on
// the current camera
//------------------------------------------------------------------------
bool vs1394Camera::isStreamGoing()
{
    return activeStream;
}

/*
//------------------------------------------------------------------------
// Returns the vsVideoStream of the current active stream, or NULL if
// there is no stream.
//------------------------------------------------------------------------
vsVideoStream *vs1394Camera::getVideoStream()
{
    return videoStream;
}
*/

//------------------------------------------------------------------------
// Returns the vsVideoQueue of the current active stream, or NULL if there
// is no stream.
//------------------------------------------------------------------------
vsVideoQueue *vs1394Camera::getVideoQueue()
{
    return videoQueue;
}

//------------------------------------------------------------------------
// Gets a pointer to the current frame's video data. Note that this
// pointer can change after each update() call.
//------------------------------------------------------------------------
const char *vs1394Camera::getCurrentFramePtr()
{
    return (const char *)currentFrameData;
}

//------------------------------------------------------------------------
// Enables white-balancing calibration. For best results, the camera
// should be pointed at a white reference object during the calibration.
//------------------------------------------------------------------------
void vs1394Camera::enableWhiteBalance()
{
    calibrationEnabled = true;
}

//------------------------------------------------------------------------
// Disables white-balancing calibration
//------------------------------------------------------------------------
void vs1394Camera::disableWhiteBalance()
{
    calibrationEnabled = false;
}

//------------------------------------------------------------------------
// Sets the value of one of the camera parameters
//------------------------------------------------------------------------
void vs1394Camera::setParameterValue(int param, unsigned int value)
{
    int whichParam;
    unsigned int redVal, blueVal;
    unsigned int minVal, maxVal;
    unsigned int myVal;
    dc1394bool_t boolVal;

    // If there's no connected camera, fail
    if (!validCamera)
        return;

    // Translate from VESS constants to library constants
    whichParam = getParameterConst(param);

    // Check for unrecognized constants
    if (whichParam == -1)
    {
        printf("vs1394Camera::setParameterValue: Invalid parameter "
            "constant\n");
        return;
    }

    // Acquire exclusive access to the camera.
    pthread_mutex_lock(&cameraMutex);

    // Check to see if the specified parameter is recognized by the camera
    if (dc1394_is_feature_present(busHandle, cameraNodeID, whichParam,
        &boolVal) != DC1394_SUCCESS)
    {
        // Yield exclusive access to the camera.
        pthread_mutex_unlock(&cameraMutex);

        printf("vs1394Camera::setParameterValue: Error communicating with "
            "camera\n");
        return;
    }
    // If the camera doesn't support that parameter, then there's nothing to do
    if (!boolVal)
    {
        // Yield exclusive access to the camera.
        pthread_mutex_unlock(&cameraMutex);
        return;
    }

    // Check to see if the specified parameter can be modified
    if (dc1394_has_manual_mode(busHandle, cameraNodeID, whichParam, &boolVal)
        != DC1394_SUCCESS)
    {
        // Yield exclusive access to the camera.
        pthread_mutex_unlock(&cameraMutex);

        printf("vs1394Camera::setParameterValue: Error communicating with "
            "camera\n");
        return;
    }
    // If the camera doesn't support modification of that parameter, then we
    // can't do anything
    if (!boolVal)
    {
        // Yield exclusive access to the camera.
        pthread_mutex_unlock(&cameraMutex);
        return;
    }

    // Make sure the specified parameter is in 'manual' mode, if it has an
    // 'automatic' setting
    if (dc1394_has_auto_mode(busHandle, cameraNodeID, whichParam, &boolVal)
        != DC1394_SUCCESS)
    {
        // Yield exclusive access to the camera.
        pthread_mutex_unlock(&cameraMutex);

        printf("vs1394Camera::setParameterValue: Error communicating with "
            "camera\n");
        return;
    }
    // Check for 'automatic' capability
    if (boolVal)
    {
        // Get the 'is automatic mode enabled?' setting
        if (dc1394_is_feature_auto(busHandle, cameraNodeID, whichParam,
            &boolVal) != DC1394_SUCCESS)
        {
            // Yield exclusive access to the camera.
            pthread_mutex_unlock(&cameraMutex);

            printf("vs1394Camera::setParameterValue: Error communicating with "
                "camera\n");
            return;
        }
        // If it's in 'automatic' mode, change that
        if (boolVal)
        {
            // Switch to 'manual' mode
            if (dc1394_auto_on_off(busHandle, cameraNodeID, whichParam, 0)
                != DC1394_SUCCESS)
            {
                // Yield exclusive access to the camera.
                pthread_mutex_unlock(&cameraMutex);

                printf("vs1394Camera::setParameterValue: Error communicating "
                    "with camera\n");
                return;
            }
        }
    }

    // Get the minimum and maximum values for this parameter; clamp the
    // parameter value to these values if needed
    myVal = value;

    if (dc1394_get_min_value(busHandle, cameraNodeID, whichParam, &minVal)
        != DC1394_SUCCESS)
    {
        // Yield exclusive access to the camera.
        pthread_mutex_unlock(&cameraMutex);

        printf("vs1394Camera::setParameterValue: Error communicating with "
            "camera\n");
        return;
    }
    // Clamp to the minimum value
    if (myVal < minVal)
        myVal = minVal;

    if (dc1394_get_max_value(busHandle, cameraNodeID, whichParam, &maxVal)
        != DC1394_SUCCESS)
    {
        // Yield exclusive access to the camera.
        pthread_mutex_unlock(&cameraMutex);

        printf("vs1394Camera::setParameterValue: Error communicating with "
            "camera\n");
        return;
    }
    // Clamp to the maximum value
    if (myVal > maxVal)
        myVal = maxVal;

    // * Now that we have the final parameter value, send it to the camera

    // Determine what to do based on the constant
    if (whichParam == FEATURE_WHITE_BALANCE)
    {
        // Special case; this one library constant corresponds to two
        // different VESS constants

        // Get both color balance parameters from the camera
        if (dc1394_get_white_balance(busHandle, cameraNodeID, &blueVal,
            &redVal) != DC1394_SUCCESS)
        {
            // Yield exclusive access to the camera.
            pthread_mutex_unlock(&cameraMutex);

            printf("vs1394Camera::setParameterValue: Error communicating with "
                "camera\n");
            return;
        }

        // Change the value of only the specified parameter
        if (param == VS_1394CAM_PARAM_BLUE_BALANCE)
            blueVal = myVal;
        else // (param == VS_1394CAM_PARAM_BLUE_BALANCE)
            redVal = myVal;

        // Send the changed parameter back to the camera
        if (dc1394_set_white_balance(busHandle, cameraNodeID, blueVal, redVal)
            != DC1394_SUCCESS)
        {
            // Yield exclusive access to the camera.
            pthread_mutex_unlock(&cameraMutex);

            printf("vs1394Camera::setParameterValue: Error communicating with "
                "camera\n");
            return;
        }
    }
    else
    {
        // Handle all of the 'ordinary' constants here

        // Send the new parameter value to the camera
        if (dc1394_set_feature_value(busHandle, cameraNodeID, whichParam,
            myVal) != DC1394_SUCCESS)
        {
            // Yield exclusive access to the camera.
            pthread_mutex_unlock(&cameraMutex);

            printf("vs1394Camera::setParameterValue: Error communicating with "
                "camera\n");
            return;
        }
    }

    // Yield exclusive access to the camera.
    pthread_mutex_unlock(&cameraMutex);
}

//------------------------------------------------------------------------
// Gets the value of one of the camera parameters
//------------------------------------------------------------------------
unsigned int vs1394Camera::getParameterValue(int param)
{
    int whichParam;
    unsigned int myVal;
    unsigned int blueVal, redVal;
    dc1394bool_t boolVal;

    // If there's no connected camera, fail
    if (!validCamera)
        return 0;

    // Translate from VESS constants to library constants
    whichParam = getParameterConst(param);

    // Acquire exclusive access to the camera.
    pthread_mutex_lock(&cameraMutex);

    // Check to see if the specified parameter is recognized by the camera
    if (dc1394_is_feature_present(busHandle, cameraNodeID, whichParam,
        &boolVal) != DC1394_SUCCESS)
    {
        // Yield exclusive access to the camera.
        pthread_mutex_unlock(&cameraMutex);

        printf("vs1394Camera::getParameterValue: Error communicating with "
            "camera\n");
        return 0;
    }
    // If the camera doesn't support that parameter, then return a default
    // value
    if (!boolVal)
    {
        // Yield exclusive access to the camera.
        pthread_mutex_unlock(&cameraMutex);
        return 0;
    }

    // Check to see if the specified parameter can be read from the camera
    if (dc1394_can_read_out(busHandle, cameraNodeID, whichParam, &boolVal)
        != DC1394_SUCCESS)
    {
        // Yield exclusive access to the camera.
        pthread_mutex_unlock(&cameraMutex);

        printf("vs1394Camera::getParameterValue: Error communicating with "
            "camera\n");
        return 0;
    }

    // If the parameter can't be read from the camera, return a default value.
    if (!boolVal)
    {
        // Yield exclusive access to the camera.
        pthread_mutex_unlock(&cameraMutex);
        return 0;
    }

    // Read the specified parameter from the camera and determine what to do.
    if (whichParam == FEATURE_WHITE_BALANCE)
    {
        // Special case; this one library constant corresponds to two
        // different VESS constants

        // Get both color balance parameters from the camera
        if (dc1394_get_white_balance(busHandle, cameraNodeID, &blueVal,
            &redVal) != DC1394_SUCCESS)
        {
            // Yield exclusive access to the camera.
            pthread_mutex_unlock(&cameraMutex);

            printf("vs1394Camera::getParameterValue: Error communicating with "
                "camera\n");
            return 0;
        }

        // Return the value of the specified parameter
        if (param == VS_1394CAM_PARAM_BLUE_BALANCE)
        {
            // Yield exclusive access to the camera.
            pthread_mutex_unlock(&cameraMutex);
            return blueVal;
        }
        else
        {
            // Yield exclusive access to the camera.
            pthread_mutex_unlock(&cameraMutex);
            return redVal;
        }
    }
    else
    {
        // Handle all of the 'ordinary' constants here

        // Get the parameter value from the camera
        if (dc1394_get_feature_value(busHandle, cameraNodeID, whichParam,
            &myVal) != DC1394_SUCCESS)
        {
            // Yield exclusive access to the camera.
            pthread_mutex_unlock(&cameraMutex);

            printf("vs1394Camera::getParameterValue: Error communicating with "
                "camera\n");
            return 0;
        }

        // Yield exclusive access to the camera.
        pthread_mutex_unlock(&cameraMutex);

        // Return the value of the specified parameter
        return myVal;
    }

    // Yield exclusive access to the camera.
    pthread_mutex_unlock(&cameraMutex);

    // Should never get here
    return 0;
}

//------------------------------------------------------------------------
// Gets the minimum value of one of the camera parameters
//------------------------------------------------------------------------
unsigned int vs1394Camera::getParameterMinValue(int param)
{
    int whichParam;
    unsigned int minVal;
    dc1394bool_t parameterSupported;

    // If there's no connected camera, fail
    if (!validCamera)
        return 0;

    // Translate from VESS constants to library constants
    whichParam = getParameterConst(param);

    // Acquire exclusive access to the camera.
    pthread_mutex_lock(&cameraMutex);

    // Check to see if the specified parameter is recognized by the camera
    if (dc1394_is_feature_present(busHandle, cameraNodeID, whichParam,
        &parameterSupported) != DC1394_SUCCESS)
    {
        // Yield exclusive access to the camera.
        pthread_mutex_unlock(&cameraMutex);

        printf("vs1394Camera::getParameterMinValue: Error communicating with "
            "camera\n");
        return 0;
    }

    // If the parameter isn't supported, return a default value.
    if (!parameterSupported)
    {
        // Yield exclusive access to the camera.
        pthread_mutex_unlock(&cameraMutex);
        return 0;
    }


    // Read the specified minimum from the camera
    if (dc1394_get_min_value(busHandle, cameraNodeID, whichParam, &minVal)
        != DC1394_SUCCESS)
    {
        // Yield exclusive access to the camera.
        pthread_mutex_unlock(&cameraMutex);

        printf("vs1394Camera::getParameterMinValue: Error communicating with "
            "camera\n");
        return 0;
    }

    // Yield exclusive access to the camera.
    pthread_mutex_unlock(&cameraMutex);

    // Return the minimum value
    return minVal;
}

//------------------------------------------------------------------------
// Gets the maximum value of one of the camera parameters
//------------------------------------------------------------------------
unsigned int vs1394Camera::getParameterMaxValue(int param)
{
    int whichParam;
    unsigned int maxVal;
    dc1394bool_t parameterSupported;

    // If there's no connected camera, fail
    if (!validCamera)
        return 0;

    // Translate from VESS constants to library constants
    whichParam = getParameterConst(param);

    // Acquire exclusive access to the camera.
    pthread_mutex_lock(&cameraMutex);

    // Check to see if the specified parameter is recognized by the camera
    if (dc1394_is_feature_present(busHandle, cameraNodeID, whichParam,
        &parameterSupported) != DC1394_SUCCESS)
    {
        // Yield exclusive access to the camera.
        pthread_mutex_unlock(&cameraMutex);

        printf("vs1394Camera::getParameterMaxValue: Error communicating with "
            "camera\n");
        return 0;
    }

    // If the parameter isn't supported, return a default value.
    if (!parameterSupported)
    {
        // Yield exclusive access to the camera.
        pthread_mutex_unlock(&cameraMutex);
        return 0;
    }

    // Read the specified maximum from the camera
    if (dc1394_get_max_value(busHandle, cameraNodeID, whichParam, &maxVal)
        != DC1394_SUCCESS)
    {
        // Yield exclusive access to the camera.
        pthread_mutex_unlock(&cameraMutex);

        printf("vs1394Camera::getParameterMaxValue: Error communicating with "
            "camera\n");
        return 0;
    }

    // Yield exclusive access to the camera.
    pthread_mutex_unlock(&cameraMutex);

    // Return the maximum value
    return maxVal;
}

//------------------------------------------------------------------------
// Static Private function
// This is the loop function of the capture thread. It captures the most
// recent frame from the camera, timestamps it, and pushes it into the
// video stream.
//------------------------------------------------------------------------
void *vs1394Camera::captureLoop(void *userData)
{
    vs1394Camera *camera;
    vsTimer *videoTimer;

    // Store the pointer to the camera.
    camera = (vs1394Camera *)userData;

    // Create a timer.
    videoTimer = new vsTimer();

    // Sieze the semaphore used to signal that the function should cease.
    pthread_mutex_lock(&camera->signalMutex);

    // Read from the device until signalled otherwise.
    while (!camera->ceaseCapture)
    {
        // Release the signal semaphore for now.
        pthread_mutex_unlock(&camera->signalMutex);

        // Acquire exclusive access to the camera so that the appropriate
        // capture calls may be made.
        pthread_mutex_lock(&camera->cameraMutex);

        // Get a new frame from the camera
        if (dc1394_dma_single_capture(&camera->cameraInfo) == DC1394_SUCCESS)
        {
            // Stuff the data into the video stream.
            camera->videoQueue->enqueue(
                (char *)camera->cameraInfo.capture_buffer,
                videoTimer->getElapsed());

            // We're already done with the buffer, so free it.
            dc1394_dma_done_with_buffer(&camera->cameraInfo);
        }

        // Give back control of the camera.
        pthread_mutex_unlock(&camera->cameraMutex);

        // Take control of the signal semaphore again for the loop check.
        pthread_mutex_lock(&camera->signalMutex);
    }

    // Release the signal semaphore, as its signal has been passed.
    pthread_mutex_unlock(&camera->signalMutex);

    // Exit the thread so the stream can be closed.
    pthread_exit(NULL);
}

//------------------------------------------------------------------------
// Private function
// Attempts to determine the presence of a camera with the specified index
// on the specified 1394 bus. If present, uses it's available features
// to come up with default settings for frame size and frame rate.
//------------------------------------------------------------------------
void vs1394Camera::connectToCamera(int busIndex, int cameraIndex)
{
    int numCameras;
    nodeid_t *cameraList;
    int numNodes;
    int sizeLoop, rateLoop;
    bool done;

    // Create a connection to the specified 1394 bus through OHCI
    busHandle = dc1394_create_handle(busIndex);
    if (!busHandle)
    {
        printf("vs1394Camera::connectToCamera: Unable to establish OHCI "
            "handle\n");
        return;
    }

    // Determine the number of cameras on this bus
    cameraList = dc1394_get_camera_nodes(busHandle, &numCameras, 0);
    if (cameraList == NULL)
    {
        printf("vs1394Camera::connectToCamera: Unable to get camera list\n");
        dc1394_destroy_handle(busHandle);
        return;
    }

    // Make sure that the camera with the specified index exists
    if (numCameras <= cameraIndex)
    {
        printf("DC1394Camera::DC1394Camera: Camera with index %d not found "
            "(only %d cameras on bus)\n", cameraIndex, numCameras);
        dc1394_free_camera_nodes(cameraList);
        dc1394_destroy_handle(busHandle);
        return;
    }

    // Store the camera node ID
    cameraNodeID = cameraList[cameraIndex];

    // Dispose of the list of camera node IDs
    dc1394_free_camera_nodes(cameraList);

    // Make sure that the camera isn't the highest-numbered node on the bus.
    // If it is, then we may not be able to properly open a data connection
    // to it.
    numNodes = raw1394_get_nodecount(busHandle);
    if (cameraNodeID == (numNodes - 1))
        printf("vs1394Camera::connectToCamera (warning): Selected camera is "
            "root node of bus; data transfers may not work properly\n");

    // If we got this far, then the connection is ready
    validCamera = true;
    activeStream = false;
    hasFrame = false;
    calibrationEnabled = false;

    // Create default settings for the transmission parameters
    done = false;
    // Check all frame sizes for compatability, smallest to largest
    for (sizeLoop = VS_1394CAM_SIZE_640X480;
        (sizeLoop <= VS_1394CAM_SIZE_1600X1200) && (!done); sizeLoop++)
    {
        if (isValidFrameSize(sizeLoop))
        {
            // Temporarily set the frame size so that the frame rate queries
            // will take the size into account
            setFrameSize(sizeLoop);

            // Check all frame rates for compatability, fastest to slowest
            for (rateLoop = VS_1394CAM_RATE_60;
                (rateLoop >= VS_1394CAM_RATE_1_875) && (!done); rateLoop--)
            {
                if (isValidFrameRate(rateLoop))
                {
                    // Set the frame rate
                    setFrameRate(rateLoop);

                    // Break out of the loop
                    done = true;
                }
            }
        }
    }

    // Set the video device name to default
    sprintf(videoDeviceName, "/dev/video1394/%d", cameraIndex);
}

//------------------------------------------------------------------------
// Private function
// Relinquishes control of the current camera, disabling any streams or
// other connections
//------------------------------------------------------------------------
void vs1394Camera::disconnectFromCamera()
{
    // If we're currently streaming, close the stream
    if (activeStream)
        stopStream();

    // Destroy the connection to the 1394 bus
    dc1394_destroy_handle(busHandle);

    // Mark that we're not connected anymore
    validCamera = false;
}

//------------------------------------------------------------------------
// Private function
// Get the format constant associated with the specified frame size
//------------------------------------------------------------------------
int vs1394Camera::getFormatConst(int fSize)
{
    // Determine the format by interpreting the frame size constant
    switch (fSize)
    {
        case VS_1394CAM_SIZE_640X480:
            // format 0
            return FORMAT_VGA_NONCOMPRESSED;

        case VS_1394CAM_SIZE_800X600:
            // format 1
            return FORMAT_SVGA_NONCOMPRESSED_1;

        case VS_1394CAM_SIZE_1024X768:
            // format 1
            return FORMAT_SVGA_NONCOMPRESSED_1;

        case VS_1394CAM_SIZE_1280X960:
            // format 2
            return FORMAT_SVGA_NONCOMPRESSED_2;

        case VS_1394CAM_SIZE_1600X1200:
            // format 2
            return FORMAT_SVGA_NONCOMPRESSED_2;

        default:
            printf("vs1394Camera::getFormatConst: Unrecognized frame size "
                "constant\n");
            break;
    }

    // Failure
    return -1;
}

//------------------------------------------------------------------------
// Private function
// Get the mode constant associated with the specified frame size
//------------------------------------------------------------------------
int vs1394Camera::getModeConst(int fSize)
{
    // Determine the mode by interpreting the frame size constant
    switch (fSize)
    {
        case VS_1394CAM_SIZE_640X480:
            // mode 4
            return MODE_640x480_RGB;

        case VS_1394CAM_SIZE_800X600:
            // mode 1
            return MODE_800x600_RGB;

        case VS_1394CAM_SIZE_1024X768:
            // mode 4
            return MODE_1024x768_RGB;

        case VS_1394CAM_SIZE_1280X960:
            // mode 1
            return MODE_1280x960_RGB;

        case VS_1394CAM_SIZE_1600X1200:
            // mode 4
            return MODE_1600x1200_RGB;

        default:
            printf("vs1394Camera::getModeConst: Unrecognized frame size "
                "constant\n");
            break;
    }

    // Failure
    return -1;
}

//------------------------------------------------------------------------
// Private function
// Get the framerate constant associated with the specified frame rate
//------------------------------------------------------------------------
int vs1394Camera::getFramerateConst(int fRate)
{
    // Determine the rate by interpreting the frame rate constant
    switch (fRate)
    {
        case VS_1394CAM_RATE_1_875:
            // rate 0 (1.875 Hz)
            return FRAMERATE_1_875;

        case VS_1394CAM_RATE_3_75:
            // rate 1 (3.75 Hz)
            return FRAMERATE_3_75;

        case VS_1394CAM_RATE_7_5:
            // rate 2 (7.5 Hz)
            return FRAMERATE_7_5;

        case VS_1394CAM_RATE_15:
            // rate 3 (15 Hz)
            return FRAMERATE_15;

        case VS_1394CAM_RATE_30:
            // rate 4 (30 Hz)
            return FRAMERATE_30;

        case VS_1394CAM_RATE_60:
            // rate 5 (60 Hz)
            return FRAMERATE_60;

        default:
            printf("vs1394Camera::getFramerateConst: Unrecognized frame rate "
                "constant\n");
            break;
    }

    // Failure
    return -1;
}

//------------------------------------------------------------------------
// Private function
// Get the format bitfield mask associated with the specified frame size
//------------------------------------------------------------------------
unsigned int vs1394Camera::getFormatMask(int fSize)
{
    // Determine the format by interpreting the frame size constant
    switch (fSize)
    {
        case VS_1394CAM_SIZE_640X480:
            // format 0 (FORMAT_VGA_NONCOMPRESSED)
            return 0x80000000;

        case VS_1394CAM_SIZE_800X600:
            // format 1 (FORMAT_SVGA_NONCOMPRESSED_1)
            return 0x40000000;

        case VS_1394CAM_SIZE_1024X768:
            // format 1 (FORMAT_SVGA_NONCOMPRESSED_1)
            return 0x40000000;

        case VS_1394CAM_SIZE_1280X960:
            // format 2 (FORMAT_SVGA_NONCOMPRESSED_2)
            return 0x20000000;

        case VS_1394CAM_SIZE_1600X1200:
            // format 2 (FORMAT_SVGA_NONCOMPRESSED_2)
            return 0x20000000;

        default:
            printf("vs1394Camera::getFormatMask: Unrecognized frame size "
                "constant\n");
            break;
    }

    // Failure
    return 0x00000000;
}

//------------------------------------------------------------------------
// Private function
// Get the mode bitfield mask associated with the specified frame size
//------------------------------------------------------------------------
unsigned int vs1394Camera::getModeMask(int fSize)
{
    // Determine the mode by interpreting the frame size constant
    switch (fSize)
    {
        case VS_1394CAM_SIZE_640X480:
            // mode 4 (MODE_640x480_RGB)
            return 0x08000000;

        case VS_1394CAM_SIZE_800X600:
            // mode 1 (MODE_800x600_RGB)
            return 0x40000000;

        case VS_1394CAM_SIZE_1024X768:
            // mode 4 (MODE_1024x768_RGB)
            return 0x08000000;

        case VS_1394CAM_SIZE_1280X960:
            // mode 1 (MODE_1280x960_RGB)
            return 0x40000000;

        case VS_1394CAM_SIZE_1600X1200:
            // mode 4 (MODE_1600x1200_RGB)
            return 0x08000000;

        default:
            printf("vs1394Camera::getModeMask: Unrecognized frame size "
                "constant\n");
            break;
    }

    // Failure
    return 0x00000000;
}

//------------------------------------------------------------------------
// Private function
// Get the framerate bitfield mask associated with the specified frame
// rate
//------------------------------------------------------------------------
unsigned int vs1394Camera::getFramerateMask(int fRate)
{
    // Determine the rate by interpreting the frame rate constant
    switch (fRate)
    {
        case VS_1394CAM_RATE_1_875:
            // rate 0 (1.875 Hz)
            return 0x80000000;

        case VS_1394CAM_RATE_3_75:
            // rate 1 (3.75 Hz)
            return 0x40000000;

        case VS_1394CAM_RATE_7_5:
            // rate 2 (7.5 Hz)
            return 0x20000000;

        case VS_1394CAM_RATE_15:
            // rate 3 (15 Hz)
            return 0x10000000;

        case VS_1394CAM_RATE_30:
            // rate 4 (30 Hz)
            return 0x08000000;

        case VS_1394CAM_RATE_60:
            // rate 5 (60 Hz)
            return 0x04000000;

        default:
            printf("vs1394Camera::getFramerateMask: Unrecognized frame rate "
                "constant\n");
            break;
    }

    // Failure
    return 0x00000000;
}

//------------------------------------------------------------------------
// Private function
// Get the parameter constant associated with the specified parameter
//------------------------------------------------------------------------
int vs1394Camera::getParameterConst(int param)
{
    // Determine the parameter by interpreting the parameter constant
    switch (param)
    {
        case VS_1394CAM_PARAM_BRIGHTNESS:
            return FEATURE_BRIGHTNESS;
        case VS_1394CAM_PARAM_EXPOSURE:
            return FEATURE_EXPOSURE;
        case VS_1394CAM_PARAM_SHARPNESS:
            return FEATURE_SHARPNESS;
        case VS_1394CAM_PARAM_BLUE_BALANCE:
            return FEATURE_WHITE_BALANCE;
        case VS_1394CAM_PARAM_RED_BALANCE:
            return FEATURE_WHITE_BALANCE;
        case VS_1394CAM_PARAM_HUE:
            return FEATURE_HUE;
        case VS_1394CAM_PARAM_SATURATION:
            return FEATURE_SATURATION;
        case VS_1394CAM_PARAM_GAMMA:
            return FEATURE_GAMMA;
        case VS_1394CAM_PARAM_SHUTTER:
            return FEATURE_SHUTTER;
        case VS_1394CAM_PARAM_GAIN:
            return FEATURE_GAIN;
        case VS_1394CAM_PARAM_IRIS:
            return FEATURE_IRIS;
        case VS_1394CAM_PARAM_FOCUS:
            return FEATURE_FOCUS;
        case VS_1394CAM_PARAM_TEMPERATURE:
            return FEATURE_TEMPERATURE;
        case VS_1394CAM_PARAM_TRIGGER:
            return FEATURE_TRIGGER;
        case VS_1394CAM_PARAM_ZOOM:
            return FEATURE_ZOOM;
        case VS_1394CAM_PARAM_PAN:
            return FEATURE_PAN;
        case VS_1394CAM_PARAM_TILT:
            return FEATURE_TILT;
        case VS_1394CAM_PARAM_OPTICAL_FILTER:
            return FEATURE_OPTICAL_FILTER;
        case VS_1394CAM_PARAM_CAPTURE_SIZE:
            return FEATURE_CAPTURE_SIZE;
        case VS_1394CAM_PARAM_CAPTURE_QUALITY:
            return FEATURE_CAPTURE_QUALITY;

        default:
            printf("vs1394Camera::getParameterConst: Unrecognized parameter "
                "constant\n");
            break;
    }

    // Failure
    return -1;
}

//------------------------------------------------------------------------
// Private function
// Calibrate the camera by computing the average color of the frame from
// the last update call, and modifying the camera's color balance
// parameters such that the resulting average color is as close to white
// (or gray) as possible.
//------------------------------------------------------------------------
void vs1394Camera::calibrateColor()
{
    long redTotal, greenTotal, blueTotal;
    long loop, pixelCount;
    unsigned char *dataPtr;
    unsigned int value;

    // Requires a frame to be loaded
    if (currentFrameData)
    {
        // Calculate the number of pixels worth of data we're processing
        pixelCount = getFrameWidth() * getFrameHeight();

        // Clear the accumulators
        redTotal = 0;
        greenTotal = 0;
        blueTotal = 0;

        // Start at the first byte of the image
        dataPtr = (unsigned char *)currentFrameData;

        // Sum up all of the pixels in the image
        for (loop = 0; loop < pixelCount; loop++)
        {
            // Pixels occur in RGB order.
            redTotal += (*dataPtr++);
            greenTotal += (*dataPtr++);
            blueTotal += (*dataPtr++);
        }

        // Divide the accumulated totals by the number of pixels to get the
        // average for each color
        redTotal /= pixelCount;
        greenTotal /= pixelCount;
        blueTotal /= pixelCount;

        // Balance the blue against the green
        if (blueTotal > (greenTotal + 1))
        {
            value = getParameterValue(VS_1394CAM_PARAM_BLUE_BALANCE);
            setParameterValue(VS_1394CAM_PARAM_BLUE_BALANCE, value-1);
        }
        if (blueTotal < (greenTotal - 1))
        {
            value = getParameterValue(VS_1394CAM_PARAM_BLUE_BALANCE);
            setParameterValue(VS_1394CAM_PARAM_BLUE_BALANCE, value+1);
        }

        // Balance the red against the green
        if (redTotal > (greenTotal + 1))
        {
            value = getParameterValue(VS_1394CAM_PARAM_RED_BALANCE);
            setParameterValue(VS_1394CAM_PARAM_RED_BALANCE, value-1);
        }
        if (redTotal < (greenTotal - 1))
        {
            value = getParameterValue(VS_1394CAM_PARAM_RED_BALANCE);
            setParameterValue(VS_1394CAM_PARAM_RED_BALANCE, value+1);
        }
    }
}

//------------------------------------------------------------------------
// Private function
// Calibrate the camera by computing the average brightness of the frame
// from the last update call, and modifying the camera's brightness
// parameters such that the resulting average brightness is very close to
// saturated white, though not completely white.
//------------------------------------------------------------------------
void vs1394Camera::calibrateBrightness()
{
    long byteTotal;
    long loop, byteCount;
    unsigned char *dataPtr;
    unsigned int value;

    // Requires a frame to be loaded
    if (currentFrameData)
    {
        // Calculate the number of bytes worth of data we're processing
        byteCount = getFrameWidth() * getFrameHeight() * 3;

        // Clear the accumulator
        byteTotal = 0;

        // Start at the first byte of the image
        dataPtr = (unsigned char *)currentFrameData;

        // Sum up all of the bytes in the image
        for (loop = 0; loop < byteCount; loop++)
            byteTotal += (*dataPtr++);

        // Divide the total by the number of bytes to get the average.
        byteTotal /= byteCount;

        // Balance the brightness against a sentinel value
        if (byteTotal > (VS_1394_CAMERA_TARGET_BRIGHTNESS + 1))
        {
            value = getParameterValue(VS_1394CAM_PARAM_BRIGHTNESS);
            setParameterValue(VS_1394CAM_PARAM_BRIGHTNESS, value-1);
        }
        if (byteTotal < (VS_1394_CAMERA_TARGET_BRIGHTNESS - 1))
        {
            value = getParameterValue(VS_1394CAM_PARAM_BRIGHTNESS);
            setParameterValue(VS_1394CAM_PARAM_BRIGHTNESS, value+1);
        }
    }
}
