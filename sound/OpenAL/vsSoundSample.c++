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
//    VESS Module:  vsSoundSample.c++
//
//    Description:  Loads and maintains a static buffer of audio data from
//                  a given sound file
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

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
    ALsizei   freq;
    ALenum    format;
    ALboolean loop;

    void      *soundData;

    // Generate a single OpenAL sound buffer for the audio data
    alGenBuffers(1, &bufferID);

    // Load the WAV file, keep track of the data format parameters as well
    alutLoadWAVFile((ALbyte *)fileName, &format, &soundData, &size, &freq, 
        &loop);

    // Make sure the load succeeds
    if (soundData == NULL)
    {
        // Load failed, print an error
        printf("vsSoundSample::vsSoundSample:  Unable to load file: %s\n", 
            fileName);
    }
    else
    {
        // Pass the WAV file data and parameters to the buffer
        alBufferData(bufferID, format, soundData, size, freq);

        // OpenAL makes a local copy of the sound data once we fill an 
        // alBuffer so we can discard the local copy of the sound data.
        free(soundData);
    }
}

// ------------------------------------------------------------------------
// Destroy a vsSoundSample
// ------------------------------------------------------------------------
vsSoundSample::~vsSoundSample()
{
    // Free the OpenAL buffer we generated
    alDeleteBuffers(1, &bufferID);
}

// ------------------------------------------------------------------------
// VESS internal function -- Return the OpenAL id for this buffer
// ------------------------------------------------------------------------
ALuint vsSoundSample::getBufferID()
{
    return bufferID;
}

// ------------------------------------------------------------------------
// Returns the corresponding base library object (an OpenAL buffer ID
// as an ALuint in this case)
// ------------------------------------------------------------------------
ALuint vsSoundSample::getBaseLibraryObject()
{
    return bufferID;
}
