#ifndef VS_SOUND_STREAM_HPP
#define VS_SOUND_STREAM_HPP

// Maintains a pair of buffers of audio data to be used for streaming audio 
// The buffers behave in a manner similar to double-buffered video.  One is
// being played while the other is available for filling with new audio data.
// This implementation uses OpenAL

#include <AL/al.h>
#include <AL/alc.h>
#include "vsSoundBuffer.h++"

// Buffer data format (simple mapping to the OpenAL format constants)
#define VS_SS_FORMAT_MONO8    AL_FORMAT_MONO8
#define VS_SS_FORMAT_MONO16   AL_FORMAT_MONO16
#define VS_SS_FORMAT_STEREO8  AL_FORMAT_STEREO8
#define VS_SS_FORMAT_STEREO16 AL_FORMAT_STEREO16

class vsSoundStream : public vsSoundBuffer
{
protected:

    // Handle of the buffers (for OpenAL)
    // Two buffers should be sufficient (one is playing while the other
    // is being filled)
    ALuint    frontBuffer;
    ALuint    backBuffer;

    // Handle of our associated source object (for OpenAL)
    ALuint    sourceID;

    // Size and format (for both buffers)
    int       bufferSize;
    int       bufferFormat;
    int       bufferFrequency;

    // Maintains whether each buffer is empty and ready for data or not.
    int       frontBufferEmpty;
    int       backBufferEmpty;

VS_INTERNAL:

    // Return the OpenAL buffer ID
    ALuint    getFrontBufferID();
    ALuint    getBackBufferID();

    // Sets the OpenAL source to which we're streaming
    void      setSourceID(int sid);

    // Marks both buffers as empty
    void      flushBuffers();

    // Swaps the front and back buffers, making the back buffer ready
    // to receive more data and be requeued.  Return value indicates
    // success or failure.
    int       swapBuffers();

public:

    // Constructor.  The parameters specify the size and format of the chunks
    // of audio data that will be supplied
                vsSoundStream(int bufSize, int bufFormat, int bufFreq);

    // Destructor.  Frees the buffers.
                ~vsSoundStream();

    // Returns VS_TRUE if the back buffer is empty and ready for queuing
    int         isBufferReady();

    // Fills the back buffer with the given audio data and queues it for
    // playing.  Return value indicates success or failure.
    int         queueBuffer(void *audioData);
};

#endif
