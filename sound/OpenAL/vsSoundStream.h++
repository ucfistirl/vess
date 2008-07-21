#ifndef VS_SOUND_STREAM_HPP
#define VS_SOUND_STREAM_HPP

// Maintains a pair of buffers of audio data to be used for streaming audio 
// The buffers behave in a manner similar to double-buffered video.  One is
// being played while the other is available for filling with new audio data.
// This implementation uses OpenAL

#include <AL/al.h>
#include "vsSoundBuffer.h++"

// Buffer data format.  The actual definitions have been moved into the
// vsSoundBuffer class.  These definitions remain for backward compatibility
#define VS_SS_FORMAT_MONO8    VS_SBUF_FORMAT_MONO8
#define VS_SS_FORMAT_MONO16   VS_SBUF_FORMAT_MONO16
#define VS_SS_FORMAT_STEREO8  VS_SBUF_FORMAT_STEREO8
#define VS_SS_FORMAT_STEREO16 VS_SBUF_FORMAT_STEREO16

class VESS_SYM vsSoundStream : public vsSoundBuffer
{
protected:

    // Handle of the buffers (for OpenAL)
    // Two buffers should be sufficient (one is playing while the other
    // is being filled)
    ALuint    frontBuffer;
    ALuint    backBuffer;

    // Handle of our associated source object (for OpenAL) and a flag to
    // indicate whether it is valid or not
    ALuint    sourceID;
    bool      sourceValid;

    // Maintains whether each buffer is empty and ready for data or not.
    bool      frontBufferEmpty;
    bool      backBufferEmpty;

VS_INTERNAL:

    // Return the OpenAL buffer ID
    ALuint    getFrontBufferID();
    ALuint    getBackBufferID();

    // Sets or revokes the OpenAL source to which we're streaming
    void      assignSource(int sid);
    void      revokeSource();

    // Marks both buffers as empty
    void      flushBuffers();

    // Swaps the front and back buffers, making the back buffer ready
    // to receive more data and be requeued.  Return value indicates
    // success or failure.
    bool      swapBuffers();

public:

    // Constructor.  The parameters specify the size and format of the chunks
    // of audio data that will be supplied
                          vsSoundStream(int bufSize, int bufFormat, 
                                        int bufFreq);

    // Destructor.  Frees the buffers.
    virtual               ~vsSoundStream();

    virtual const char    *getClassName();

    virtual int           getBufferType();

    // Returns true if the back buffer is empty and ready for queuing
    bool                  isBufferReady();

    // Returns true if the stream is empty (both buffers are empty)
    bool                  isEmpty();

    // Fills the back buffer with the given audio data and queues it for
    // playing.  Return value indicates success or failure.
    bool                  queueBuffer(void *audioData);
};

#endif
