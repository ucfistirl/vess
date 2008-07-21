#ifndef VS_SOUND_PACKET_STREAM_HPP
#define VS_SOUND_PACKET_STREAM_HPP

// This class works similarly to vsSoundPacketStream, but OpenAL buffers are
// created and deleted dynamically, instead of using two persistent buffers

#include <AL/al.h>
#include "vsSoundBuffer.h++"


class VESS_SYM vsSoundPacketStream : public vsSoundBuffer
{
protected:

    // Handle of our associated source object (for OpenAL) and a flag to
    // indicate whether it is valid or not
    ALuint    sourceID;
    bool      sourceValid;

VS_INTERNAL:

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

    // Constructor.  The parameters specify the format of the chunks
    // of audio data that will be supplied
                          vsSoundPacketStream(int bufFormat, int bufFreq);

    // Destructor.  Frees the buffers.
    virtual               ~vsSoundPacketStream();

    virtual const char    *getClassName();

    virtual int           getBufferType();

    // Returns true if the stream is empty
    bool                  isEmpty();

    // Creates a new OpenAL buffer with the given audio data and queues it for
    // playing.  Return value indicates success or failure.
    bool                  queueBuffer(void *audioData, u_long length);

    // Updates the stream (this method must be called regularly to handle
    // the source's buffer queue)
    void                  update();
};

#endif
