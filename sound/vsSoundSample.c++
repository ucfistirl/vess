#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <stdio.h>
#include <stdlib.h>
#include "vsSoundSample.h++"

// ------------------------------------------------------------------------
// Create a vsSoundSample from the given sound file.  (We only handle
// .WAV files at the moment)
// ------------------------------------------------------------------------
vsSoundSample::vsSoundSample(char *fileName)
             : vsSoundBuffer()
{
    ALsizei   size;
    ALsizei   bits;
    ALsizei   freq;
    ALsizei   format;
    ALboolean errorFlag;

    void      *soundData;

    // Generate the buffer
    alGenBuffers(1, &bufferID);

    errorFlag = alutLoadWAV(fileName, &soundData, &format, &size, &bits, &freq);
    if (errorFlag == AL_FALSE)
    {
        printf("vsSoundSample::vsSoundSample:  Unable to load file: %s\n", 
            fileName);
    }
    else
    {
        alBufferData(bufferID, format, soundData, size, freq);

        // OpenAL makes a local copy of the sound data once we fill an 
        // alBuffer so...
        free(soundData);
    }
}

// ------------------------------------------------------------------------
// Destroy a vsSoundSample
// ------------------------------------------------------------------------
vsSoundSample::~vsSoundSample()
{
    alDeleteBuffers(1, &bufferID);
}

// ------------------------------------------------------------------------
// VESS internal function -- Return the OpenAL id for this buffer
// ------------------------------------------------------------------------
ALuint vsSoundSample::getBufferID()
{
    return bufferID;
}
