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
    frontBufferEmpty = VS_TRUE;
    backBufferEmpty = VS_TRUE;
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
    frontBufferEmpty = VS_TRUE;
    alBufferData(frontBuffer, bufferFormat, zeroBuf, bufferSize, 
        bufferFrequency);

    // Flush the back buffer
    backBufferEmpty = VS_TRUE;
    alBufferData(backBuffer, bufferFormat, zeroBuf, bufferSize, 
        bufferFrequency);

    // Clean up
    free(zeroBuf);
}

// ------------------------------------------------------------------------
// VESS internal function -- Swaps the front and back buffers and marks the
// back buffer empty
// ------------------------------------------------------------------------
int vsSoundStream::swapBuffers()
{
    int tempBuffer;

    // Check the back buffer to see if it's empty
    if (backBufferEmpty)
    {
        // The back buffer is not ready, so just mark the front
        // buffer empty as well
        frontBufferEmpty = VS_TRUE;

        // Return false to indicate that we're out of data (the stream is
        // starved)
        return VS_FALSE;
    }

    // Swap the buffers
    tempBuffer = frontBuffer;
    frontBuffer = backBuffer;
    backBuffer = tempBuffer;

    // Mark the (new) back buffer empty
    backBufferEmpty = VS_TRUE;

    // Return true to indicate success (the stream has data available)
    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Returns whether or not either buffer is ready for new data
// ------------------------------------------------------------------------
int vsSoundStream::isBufferReady()
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
// playing.  Returns VS_TRUE if successful or VS_FALSE if not.  May fail if
// there is no empty buffer to hold the audio, or if no 
// vsSoundSourceAttribute has been created with this vsSoundStream instance
// ------------------------------------------------------------------------
int vsSoundStream::queueBuffer(void *audioData)
{
    // Make sure we are bound to a valid OpenAL source
    if (!alIsSource(sourceID))
    {
        printf("vsSoundStream::queueBuffer:  no vsSoundSourceAttribute"
            " to stream to\n");

        // Return false to indicate the queue operation failed
        return VS_FALSE;
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
        frontBufferEmpty = VS_FALSE;

        // Return true to indicate the queue operation succeeded.
        return VS_TRUE;
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
        backBufferEmpty = VS_FALSE;

        // Return true to indicate that the queue operation succeeded
        return VS_TRUE;
    }

    // Neither buffer is empty, so print an error
    printf("vsSoundStream::queueBuffer:  no buffers available to receive"
        " audio data\n");

    // Return false to indicate the queue operation failed
    return VS_FALSE;
}
