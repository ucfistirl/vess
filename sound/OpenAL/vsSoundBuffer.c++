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
//    Description:  Sound buffer base class
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsSoundBuffer.h++"

// ------------------------------------------------------------------------
// Constructor for vsSoundBuffer (does nothing)
// ------------------------------------------------------------------------
vsSoundBuffer::vsSoundBuffer()
{
}

// ------------------------------------------------------------------------
// Destructor for vsSoundBuffer (does nothing)
// ------------------------------------------------------------------------
vsSoundBuffer::~vsSoundBuffer()
{
}

// ------------------------------------------------------------------------
// Return the sound data format
// ------------------------------------------------------------------------
int vsSoundBuffer::getFormat()
{
    return bufferFormat;
}

// ------------------------------------------------------------------------
// Return the sound data format
// ------------------------------------------------------------------------
int vsSoundBuffer::getFrequency()
{
    return bufferFrequency;
}

// ------------------------------------------------------------------------
// Return the buffer size
// ------------------------------------------------------------------------
int vsSoundBuffer::getBufferSize()
{
    return bufferSize;
}

// ------------------------------------------------------------------------
// Return the length of the buffer (in seconds)
// ------------------------------------------------------------------------
double vsSoundBuffer::getLength()
{
    int numSamples;
    int bytesPerSample, channelCount;
                                                                                
    if ((getChannelCount() < 1) || (getBytesPerSample() < 1))
    {
        // We don't know this audio format, so we can't compute the
        // buffer length
        return 0;
    }
     
    // Get the sample parameters
    bytesPerSample = getBytesPerSample();
    channelCount = getChannelCount();
    
    // If either parameter is zero, we can't compute the buffer length
    if ((bytesPerSample <= 0) || (channelCount <= 0))
    {
        // Return zero for the length, because we don't recognize the
        // audio format
        return 0;
    }
    else
    {
        // Compute the number of audio samples in the data.  Note that this
        // is integer division, since we're computing a sample count.  If 
        // the buffer size is valid for the format being used, there won't 
        // be any remainders from this division.
        numSamples = bufferSize / getBytesPerSample() / getChannelCount();
    }

    // Check the buffer frequency to see if it is positive (we don't want
    // to divide by zero
    if (bufferFrequency <= 0)
    {
        // We can't compute the length of this buffer, so return zero for
        // the length
        return 0;
    }
                                                                                
    // Divide the number of samples by the frequency (samples per second)
    // to get the number of seconds (note that this is now floating-point
    // division, as we're computing time in seconds)
    return ((double)numSamples / (double)bufferFrequency);
}

// ------------------------------------------------------------------------
// Return the number of channels in the audio data
// ------------------------------------------------------------------------
int vsSoundBuffer::getChannelCount()
{
    if ((bufferFormat == VS_SBUF_FORMAT_MONO8) || 
        (bufferFormat == VS_SBUF_FORMAT_MONO16))
        return 1;
    
    if ((bufferFormat == VS_SBUF_FORMAT_STEREO8) || 
        (bufferFormat == VS_SBUF_FORMAT_STEREO16))
        return 2;

    // We don't know this audio format, so we can't know how many channels
    // it contains
    return 0;
}

// ------------------------------------------------------------------------
// Return the number of bytes per audio sample
// ------------------------------------------------------------------------
int vsSoundBuffer::getBytesPerSample()
{
    if ((bufferFormat == VS_SBUF_FORMAT_MONO8) || 
        (bufferFormat == VS_SBUF_FORMAT_STEREO8))
        return 1;
    
    if ((bufferFormat == VS_SBUF_FORMAT_MONO16) || 
        (bufferFormat == VS_SBUF_FORMAT_STEREO16))
        return 2;

    // We don't know this audio format, so we can't know how many bytes
    // there are per audio sample
    return 0;
}
