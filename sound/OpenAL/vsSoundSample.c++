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
    
    // See if the buffer was generated properly
    if (alGetError() != AL_NO_ERROR)
    {
        printf("vsSoundSample::vsSoundSample:");
        printf("  Error generating sound buffer!\n");        
    }

    // Load the WAV file, keep track of the data format parameters as well
    alutLoadWAVFile((ALbyte *)fileName, &format, &soundData, &size, &freq, 
        &loop);

    // Make sure the load succeeds
    if (soundData == NULL)
    {
        // Load failed, print an error
        printf("vsSoundSample::vsSoundSample:  Unable to load file: %s\n", 
            fileName);

        // Set all parameters to appropriate values
        bufferSize = 0;
        bufferFrequency = 0;
        bufferFormat = 0;
    }
    else
    {
        // Store the sound parameters for later retrieval
        bufferSize = size;
        bufferFrequency = freq;
        bufferFormat = format;
        
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
// Return the name of this class
// ------------------------------------------------------------------------
const char * vsSoundSample::getClassName()
{
    return "vsSoundSample";
}

// ------------------------------------------------------------------------
// Return the type of buffer
// ------------------------------------------------------------------------
int vsSoundSample::getBufferType()
{
    return VS_SOUND_BUFFER_SAMPLE;
}

// ------------------------------------------------------------------------
// Returns the corresponding base library object (an OpenAL buffer ID
// as an ALuint in this case)
// ------------------------------------------------------------------------
ALuint vsSoundSample::getBaseLibraryObject()
{
    return bufferID;
}
