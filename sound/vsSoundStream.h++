#ifndef VS_SOUND_STREAM_HPP
#define VS_SOUND_STREAM_HPP

// Maintains a streaming buffer of audio data 
// This implementation uses OpenAL

class vsSoundStream : public vsSoundBuffer
{
protected:

    // Handle of the buffer (for OpenAL)
    int    bufferID;

public:

                vsSoundStream();
                ~vsSoundStream();

    int         addData(void *audioData, int size);
};

#endif
