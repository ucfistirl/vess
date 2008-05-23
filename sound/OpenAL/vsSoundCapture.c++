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
//    VESS Module:  vsSoundBuffer.c++
//
//    Description:  See header file for description.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsSoundCapture.h++"

#ifndef _MSC_VER
   #include <unistd.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ------------------------------------------------------------------------
// Constructor - Default
// This constructor initializes a sound capture object with default
// parameters.
// ------------------------------------------------------------------------
vsSoundCapture::vsSoundCapture()
{
    // Initialize the buffer and capture variables.
    captureFormat = VS_SOUND_CAPTURE_DEFAULT_FORMAT;
    captureRate = VS_SOUND_CAPTURE_DEFAULT_RATE;
    queueSampleCapacity = VS_SOUND_CAPTURE_DEFAULT_CAPACITY;
    packetSampleCount = VS_SOUND_CAPTURE_DEFAULT_PACKETSIZE;

    // Determine the number of bytes required per sample.
    switch (captureFormat)
    {
        case VS_SBUF_FORMAT_MONO8:
        {
            bytesPerSample = 1;
        }
        break;

        case VS_SBUF_FORMAT_STEREO8:
        case VS_SBUF_FORMAT_MONO16:
        {
            bytesPerSample = 2;
        }
        break;

        case VS_SBUF_FORMAT_STEREO16:
        {
            bytesPerSample = 4;
        }
        break;

        default:
        {
            bytesPerSample = 0;
        }
        break;
    }

    // The size of a packet is measured in samples rather than bytes, so
    // multiply by bytes per sample to get an accurate buffer size.
    soundPacketBuffer = (void *)malloc(packetSampleCount * bytesPerSample);

    // Create the sound queue into which data from this capture object will go.
    //soundQueue = new vsMultiQueue(packetSampleCount * bytesPerSample * 100);
    soundQueue = new vsMultiQueue(queueSampleCapacity * bytesPerSample);
    soundQueue->ref();

    // Initialize the semaphores.
    pthread_mutex_init(&signalMutex, NULL);
    pthread_mutex_init(&bufferMutex, NULL);

    // By default, there is no device open.
    deviceOpen = false;
}

// ------------------------------------------------------------------------
// Constructor
// This constructor initializes a sound capture object with the specified
// parameters for the capture format, sound sample rate, and buffer
// capacity.
// ------------------------------------------------------------------------
vsSoundCapture::vsSoundCapture(int format, int rate, int capacity,
    int packetsize)
{
    // Initialize the buffer and capture variables.
    captureFormat = format;
    captureRate = rate;
    queueSampleCapacity = capacity;
    packetSampleCount = packetsize;

    // Determine the number of bytes required per sample.
    switch (captureFormat)
    {
        case VS_SBUF_FORMAT_MONO8:
        {
            bytesPerSample = 1;
        }
        break;

        case VS_SBUF_FORMAT_STEREO8:
        case VS_SBUF_FORMAT_MONO16:
        {
            bytesPerSample = 2;
        }
        break;

        case VS_SBUF_FORMAT_STEREO16:
        {
            bytesPerSample = 4;
        }
        break;

        default:
        {
            bytesPerSample = 0;
        }
        break;
    }

    // The size of a packet is measured in samples rather than bytes, so
    // multiply by bytes per sample to get an accurate buffer size.
    soundPacketBuffer = (void *)malloc(packetSampleCount * bytesPerSample);

    // Create the sound queue into which data from this capture object will go.
    //soundQueue = new vsMultiQueue(packetSampleCount * bytesPerSample * 100);
    soundQueue = new vsMultiQueue(queueSampleCapacity * bytesPerSample);
    soundQueue->ref();

    // Initialize the semaphores.
    pthread_mutex_init(&signalMutex, NULL);
    pthread_mutex_init(&bufferMutex, NULL);

    // By default, there is no device open.
    deviceOpen = false;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsSoundCapture::~vsSoundCapture(void)
{
    // If a device has been opened, ensure that it is closed before proceeding.
    if (deviceOpen)
        closeDevice();

    // Free all of the mutex variables.
    pthread_mutex_destroy(&signalMutex);
    pthread_mutex_destroy(&bufferMutex);

    // Release the reference on the sound queue.
    vsObject::unrefDelete(soundQueue);

    // Free the local data buffer.
    free(soundPacketBuffer);
}

// ------------------------------------------------------------------------
// Return this objects class name.
// ------------------------------------------------------------------------
const char *vsSoundCapture::getClassName()
{
    return "vsSoundCapture";
}

// ------------------------------------------------------------------------
// This method attempts to open the sound device with the specified handle.
// This handle may be NULL. The method will return a boolean indicating
// whether or not the device was successfully opened.
// ------------------------------------------------------------------------
bool vsSoundCapture::openDevice(char *device)
{
    // Only continue if there is not already a device open.
    if (deviceOpen)
    {
        fprintf(stderr, "vsSoundCapture::openDevice: A device is already "
            "open.\n");
        return false;
    }

    // Clear any previous errors that may have accumulated.
    alcGetError(NULL);

/*
    // FIXME: This call will give a null-delimited list of device handles which
    // may potentially be used for capture. It may be of value to allow the
    // user to query potentially valid capture devices.
    const ALCchar *deviceHandleList;
    deviceHandleList = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
*/

    // Open the device with the given rate, format, and capacity, scaling the
    // capacity (measured in samples) to a value measured in bytes.
    captureDevice = alcCaptureOpenDevice(device, captureRate, captureFormat,
        packetSampleCount);

    // Make sure the device could be successfully opened.
    if ((alcGetError(captureDevice) != ALC_NO_ERROR) ||
        (captureDevice == NULL))
    {
        fprintf(stderr, "vsSoundCapture::vsSoundCapture - Unable to "
            "open device!\n");
        return false;
    }

    // Begin with the buffer mutex locked and capture in a paused state.
    pthread_mutex_lock(&bufferMutex);
    capturePaused = true;

    // Begin with the kill flag set to false.
    ceaseCapture = false;

    // Mark that there is now an open device.
    deviceOpen = true;

    // Create the thread to begin feeding into the queue.
    pthread_create(&captureThread, NULL, captureLoop, this);

    // Return true, indicating that the device was successfully opened.
    return true;
}

// ------------------------------------------------------------------------
// This method attempts to open the sound device with the specified handle.
// This handle may be NULL. The method will return a boolean indicating
// whether or not the device was successfully opened.
// ------------------------------------------------------------------------
void vsSoundCapture::closeDevice()
{
    // Only continue if there is a device open.
    if (!deviceOpen)
    {
        fprintf(stderr, "vsSoundCapture::closeDevice: A device has not been "
            "opened.\n");
        return;
    }

    // Sieze control of the signalling semaphore, mark that the thread should
    // finish execution, then release the signalling semaphore to let the loop 
    // exit.
    pthread_mutex_lock(&signalMutex);
    ceaseCapture = true;
    pthread_mutex_unlock(&signalMutex);

    // If the thread was paused, unpause it now so it can get the new signal.
    if (capturePaused)
        startResume();

    // Wait until the capture thread has closed, disregarding its return value.
    pthread_join(captureThread, NULL);

    // Release OpenAL's resources.
    alcCaptureCloseDevice(captureDevice);

    // Mark that the device has been closed.
    deviceOpen = false;
}

// ------------------------------------------------------------------------
// Returns the format of the data this class captures. These constants are
// defined in vsSoundBuffer.h++.
// ------------------------------------------------------------------------
int vsSoundCapture::getFormat()
{
    return captureFormat;
}

// ------------------------------------------------------------------------
// Returns the sample rate of the data this class captures.
// ------------------------------------------------------------------------
int vsSoundCapture::getRate()
{
    return captureRate;
}

// ------------------------------------------------------------------------
// Returns the number of bytes used for each sample captured by this class.
// ------------------------------------------------------------------------
int vsSoundCapture::getBytesPerSample()
{
    return bytesPerSample;
}

// ------------------------------------------------------------------------
// Returns the queue into which sound data is placed.
// ------------------------------------------------------------------------
vsMultiQueue *vsSoundCapture::getSoundQueue()
{
    return soundQueue;
}

// ------------------------------------------------------------------------
// Returns whether sound capture has been paused.
// ------------------------------------------------------------------------
bool vsSoundCapture::isPaused()
{
    return capturePaused;
}

// ------------------------------------------------------------------------
// If sound capture had previously been paused, allow it to resume.
// ------------------------------------------------------------------------
void vsSoundCapture::startResume()
{
    // Only resume if currently in a pause state.
    if (capturePaused)
    {
        // Make sure the device is open before manipulating it
        if (deviceOpen)
        {
            // Start capturing from the device
            alcCaptureStart(captureDevice);

            // Make sure everything is working.
            if (alcGetError(captureDevice) != ALC_NO_ERROR)
            {
                fprintf(stderr, "vsSoundCapture::startResume: "
                    "alcCaptureStart failed! Invalid device!\n");
            }
        }

        // Release the buffer mutex. This will allow the capture loop to
        // resume processing data.
        pthread_mutex_unlock(&bufferMutex);

        // Mark that the current state is not paused.
        capturePaused = false;
    }
}

// ------------------------------------------------------------------------
// If sound capture is currently running, halt it.
// ------------------------------------------------------------------------
void vsSoundCapture::pause()
{
    ALubyte  buffer[65535];
    ALint    count;

    // Only pause if not already in a pause state.
    if (!capturePaused)
    {
        // Make sure the device is open before manipulating it
        if (deviceOpen)
        {
            // Stop capturing from the device
            alcCaptureStop(captureDevice);

            // Make sure everything is working.
            if (alcGetError(captureDevice) != ALC_NO_ERROR)
            {
                fprintf(stderr, "vsSoundCapture::pause: "
                    "alcCaptureStop failed! Invalid device!\n");
            }
        }

        // Take control of the buffer mutex. This will prevent the capture loop
        // from processing further data.
        pthread_mutex_lock(&bufferMutex);

        // Grab any lingering samples in the device buffer and throw them
        // away
        alcGetIntegerv(captureDevice, ALC_CAPTURE_SAMPLES,
            sizeof(ALint), &count);
        alcCaptureSamples(captureDevice, buffer, count);

        // Mark that the current state is paused.
        capturePaused = true;
    }
}

// ------------------------------------------------------------------------
// Private Static function
// This is the main capture loop of the sound capture object. It queries
// the device for the number of available samples, and if that number is
// equal to the local buffer capacity it stores the samples in the main
// audio queue.
// ------------------------------------------------------------------------
void *vsSoundCapture::captureLoop(void *userData)
{
    ALint deviceSampleCount;

    // Store the device itself.
    vsSoundCapture *capture = (vsSoundCapture *)userData;

    // Sieze the semaphore used to signal that the function should cease.
    pthread_mutex_lock(&capture->signalMutex);

    // Read from the device until signalled otherwise.
    while (!capture->ceaseCapture)
    {
        // Release the signal semaphore for now.
        pthread_mutex_unlock(&capture->signalMutex);

        // This semaphore will be locked if the device is paused. Wait until
        // it is available.
        pthread_mutex_lock(&capture->bufferMutex);

        // See how many samples are available from the device.
        alcGetIntegerv(capture->captureDevice, ALC_CAPTURE_SAMPLES,
            sizeof(ALint), &deviceSampleCount);

        // See if the buffer can hold all of the data.
        if (deviceSampleCount >= capture->packetSampleCount)
        {
            // Capture a packet worth of data.
            alcCaptureSamples(capture->captureDevice,
                capture->soundPacketBuffer, capture->packetSampleCount);

            // Put the data into the sound queue.
            capture->soundQueue->enqueue(capture->soundPacketBuffer,
                capture->packetSampleCount * capture->bytesPerSample);
        }

        // Give back control of the microphone object
        pthread_mutex_unlock(&capture->bufferMutex);

        // Sleep a bit. FIXME: The sleep amount should be based on the capture
        // rate or the rate at which this thread is processing.
        usleep(VS_SOUND_CAPTURE_LOOP_SLEEP);

        // Take control of the signal semaphore again for the loop check.
        pthread_mutex_lock(&capture->signalMutex);
    }

    // Release the signal semaphore, as its signal has been passed.
    pthread_mutex_unlock(&capture->signalMutex);

    // Return from the thread
    return NULL;
}

