#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vsGlobals.h++"
#include "vsSoundStream.h++"

// ------------------------------------------------------------------------
// Create a vsSoundStream.  The bufSize parameter specifies the size of
// the buffers and is used when the queueBuffer() method is called to 
// transfer new audio data into the back buffer.
// ------------------------------------------------------------------------
vsSoundStream::vsSoundStream(int bufSize, int bufFormat, int bufFreq)
             : vsSoundBuffer()
{
    // Keep the parameters around
    bufferSize = bufSize;
    bufferFormat = bufFormat;
    bufferFrequency = bufFreq;

    // Generate the buffers
    alGenBuffers(1, &frontBuffer);
    alGenBuffers(1, &backBuffer);

    // Initialize
    frontBufferEmpty = true;
    backBufferEmpty = true;
    sourceID = 0xFFFFFFFF;
}

// ------------------------------------------------------------------------
// Destroy a vsSoundStream
// ------------------------------------------------------------------------
vsSoundStream::~vsSoundStream()
{
    // Free the buffers
    alDeleteBuffers(1, &frontBuffer);
    alDeleteBuffers(1, &backBuffer);
}

// ------------------------------------------------------------------------
// Return this class' name
// ------------------------------------------------------------------------
const char * vsSoundStream::getClassName()
{
    return "vsSoundStream";
}

// ------------------------------------------------------------------------
// VESS internal function -- Return the OpenAL id for the front buffer
// ------------------------------------------------------------------------
ALuint vsSoundStream::getFrontBufferID()
{
    return frontBuffer;
}

// ------------------------------------------------------------------------
// VESS internal function -- Return the OpenAL id for the back buffer
// ------------------------------------------------------------------------
ALuint vsSoundStream::getBackBufferID()
{
    return backBuffer;
}

// ------------------------------------------------------------------------
// VESS internal function -- Informs this object of which sound source 
// object it will be streaming to
// ------------------------------------------------------------------------
void vsSoundStream::setSourceID(int sid)
{
    sourceID = sid;
}

// ------------------------------------------------------------------------
// VESS internal function -- Marks both buffers as empty and zeroes their
// data
// ------------------------------------------------------------------------
void vsSoundStream::flushBuffers()
{
    void *zeroBuf;

    // Create a set of zero data for each buffer
    zeroBuf = malloc(bufferSize);
    memset(zeroBuf, 0, sizeof(zeroBuf));

    // Flush the front buffer
    frontBufferEmpty = true;
    alBufferData(frontBuffer, bufferFormat, zeroBuf, bufferSize, 
        bufferFrequency);

    // Flush the back buffer
    backBufferEmpty = true;
    alBufferData(backBuffer, bufferFormat, zeroBuf, bufferSize, 
        bufferFrequency);

    // Clean up
    free(zeroBuf);
}

// ------------------------------------------------------------------------
// VESS internal function -- Swaps the front and back buffers and marks the
// back buffer empty
// ------------------------------------------------------------------------
bool vsSoundStream::swapBuffers()
{
    int tempBuffer;

    // Check the back buffer to see if it's empty
    if (backBufferEmpty)
    {
        // The back buffer is not ready, so just mark the front
        // buffer empty as well
        frontBufferEmpty = true;

        // Return false to indicate that we're out of data (the stream is
        // starved)
        return false;
    }

    // Swap the buffers
    tempBuffer = frontBuffer;
    frontBuffer = backBuffer;
    backBuffer = tempBuffer;

    // Mark the (new) back buffer empty
    backBufferEmpty = true;

    // Return true to indicate success (the stream has data available)
    return true;
}

// ------------------------------------------------------------------------
// Returns whether or not either buffer is ready for new data
// ------------------------------------------------------------------------
bool vsSoundStream::isBufferReady()
{
    return (frontBufferEmpty || backBufferEmpty); 
}

// ------------------------------------------------------------------------
// Returns the size of each of the buffers.  This indicates how many
// bytes are expected for each call to queueBuffer()
// ------------------------------------------------------------------------
int vsSoundStream::getBufferSize()
{
    return bufferSize;
}

// ------------------------------------------------------------------------
// Fills the front or back buffer with the given data and queues it for 
// playing.  Returns true if successful or false if not.  May fail if
// there is no empty buffer to hold the audio, or if no 
// vsSoundSourceAttribute has been created with this vsSoundStream instance
// ------------------------------------------------------------------------
bool vsSoundStream::queueBuffer(void *audioData)
{
    // Make sure we are bound to a valid OpenAL source
    if (!alIsSource(sourceID))
    {
        printf("vsSoundStream::queueBuffer:  no vsSoundSourceAttribute"
            " to stream to\n");

        // Return false to indicate the queue operation failed
        return false;
    }

    // Fill the front buffer if it's empty
    if (frontBufferEmpty)
    {
        // Put the audio data in the front buffer using the format specified
        // in the constructor
        alBufferData(frontBuffer, bufferFormat, audioData, bufferSize,
            bufferFrequency);
        alSourceQueueBuffers(sourceID, 1, &frontBuffer);

        // Mark the front buffer as full
        frontBufferEmpty = false;

        // Return true to indicate the queue operation succeeded.
        return true;
    }

    // Otherwise, fill the back buffer if it's empty
    if (backBufferEmpty)
    {
        // Put the audio data in the front buffer using the format specified
        // in the constructor
        alBufferData(backBuffer, bufferFormat, audioData, bufferSize,
            bufferFrequency);
        alSourceQueueBuffers(sourceID, 1, &backBuffer);

        // Mark the back buffer as full
        backBufferEmpty = false;

        // Return true to indicate that the queue operation succeeded
        return true;
    }

    // Neither buffer is empty, so print an error
    printf("vsSoundStream::queueBuffer:  no buffers available to receive"
        " audio data\n");

    // Return false to indicate the queue operation failed
    return false;
}
