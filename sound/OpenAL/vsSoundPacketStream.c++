#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vsGlobals.h++"
#include "vsSoundPacketStream.h++"

// ------------------------------------------------------------------------
// Create a vsSoundPacketStream
// ------------------------------------------------------------------------
vsSoundPacketStream::vsSoundPacketStream(int bufFormat, int bufFreq)
                   : vsSoundBuffer()
{
    // Keep the parameters around
    bufferFormat = bufFormat;
    bufferFrequency = bufFreq;

    // Set the buffer size to zero (the buffers that are queued may be of
    // any size)
    bufferSize = 0;

    // Initialize other data members
    sourceID = 0;
    sourceValid = false;
}

// ------------------------------------------------------------------------
// Destroy a vsSoundPacketStream
// ------------------------------------------------------------------------
vsSoundPacketStream::~vsSoundPacketStream()
{
    // Flush any buffers currently queued
    flushBuffers();
}

// ------------------------------------------------------------------------
// Return this class' name
// ------------------------------------------------------------------------
const char * vsSoundPacketStream::getClassName()
{
    return "vsSoundPacketStream";
}

// ------------------------------------------------------------------------
// VESS internal function -- Informs this object of which sound source 
// object it will be streaming to
// ------------------------------------------------------------------------
void vsSoundPacketStream::assignSource(int sid)
{
    // Remember the source ID and mark it valid
    sourceID = sid;
    sourceValid = true;
}

// ------------------------------------------------------------------------
// VESS internal function -- Informs this object that the source it was
// streaming to is no longer valid
// ------------------------------------------------------------------------
void vsSoundPacketStream::revokeSource()
{
    // Flush all buffers
    flushBuffers();

    // Invalidate the source
    sourceID = 0;
    sourceValid = false;
}

// ------------------------------------------------------------------------
// VESS internal function -- Marks both buffers as empty and zeroes their
// data
// ------------------------------------------------------------------------
void vsSoundPacketStream::flushBuffers()
{
    ALint numBuffers;
    ALuint buffers[1024];

    // See if we currently have a valid source
    if (sourceValid)
    {
        // Stop playback
        alSourceStop(sourceID);

        // Check for queued buffers
        alGetSourceiv(sourceID, AL_BUFFERS_QUEUED, &numBuffers);

        // Unqueue all buffers
        alSourceUnqueueBuffers(sourceID, numBuffers, buffers);

        // Delete the buffers we just unqueued
        alDeleteBuffers(numBuffers, buffers);
    }
}

// ------------------------------------------------------------------------
// Return the sound buffer type (a packet stream)
// ------------------------------------------------------------------------
int vsSoundPacketStream::getBufferType()
{
    return VS_SOUND_BUFFER_PACKET_STREAM;
}

// ------------------------------------------------------------------------
// Returns whether or not the stream is empty (ie: out of data to play)
// ------------------------------------------------------------------------
bool vsSoundPacketStream::isEmpty()
{
    int buffersQueued;
    int buffersProcessed;

    // See if the source is valid
    if (sourceValid)
    {
        // Check the number of buffers queued vs. the number processed
        alGetSourceiv(sourceID, AL_BUFFERS_QUEUED, &buffersQueued);
        alGetSourceiv(sourceID, AL_BUFFERS_PROCESSED, &buffersProcessed);

        // If the number of buffers processed is equal to the number queued
        // then we've run out of data to play
        if (buffersProcessed >= buffersQueued)
            return true;
        else
            return false;
    } 
    else
    {
        // If the source isn't valid, we're empty
        return true; 
    }
}

// ------------------------------------------------------------------------
// Creates a new buffer for the stream, fills it with the given data, and
// enqueues it for playback. Returns true if successful or false if not
// ------------------------------------------------------------------------
bool vsSoundPacketStream::queueBuffer(void *audioData, u_long length)
{
    ALuint buffer;

    // If we have no source, fail
    if (!sourceValid)
        return false;

    // Otherwise, create a new buffer for this data
    alGenBuffers(1, &buffer);
    alBufferData(buffer, bufferFormat, audioData, length, bufferFrequency);

    // Queue the buffer on the source
    alSourceQueueBuffers(sourceID, 1, &buffer);
}

// ------------------------------------------------------------------------
// Updates the stream by removing and deleting processed buffers
// ------------------------------------------------------------------------
void vsSoundPacketStream::update()
{
    ALint numBuffers;
    ALuint buffers[1024];

    // If the source isn't valid, don't bother going farther
    if (!sourceValid)
        return;

    // Look for processed buffers on the source's buffer queue
    alGetSourceiv(sourceID, AL_BUFFERS_PROCESSED, &numBuffers);
    if (numBuffers > 0)
    {
        // Dequeue and delete each processed buffer
        alSourceUnqueueBuffers(sourceID, numBuffers, buffers);
        alDeleteBuffers(numBuffers, buffers);
    }
}
