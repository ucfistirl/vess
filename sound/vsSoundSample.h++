#ifndef VS_SOUND_SAMPLE_HPP
#define VS_SOUND_SAMPLE_HPP

// Loads and maintains a static buffer of audio data from a given sound file
// This implementation uses OpenAL

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alkludge.h>
#include "vsGlobals.h++"
#include "vsSoundBuffer.h++"

class vsSoundSample : public vsSoundBuffer
{
protected:

    // Handle of the buffer (for OpenAL)
    ALuint      bufferID;

VS_INTERNAL:

    virtual ALuint    getBufferID();

public:

                      vsSoundSample(char *filename);
    virtual           ~vsSoundSample();
};

#endif
