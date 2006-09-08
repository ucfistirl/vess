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
#include <AL/alut.h>
#include <stdio.h>
#include <stdlib.h>
#include "vsSoundSample.h++"

// ------------------------------------------------------------------------
// Create a vsSoundSample from the given sound file
// ------------------------------------------------------------------------
vsSoundSample::vsSoundSample(char *fileName)
             : vsSoundBuffer()
{
    ALsizei   size;
    ALfloat   freq;
    ALenum    format;
    ALenum    error;

    void      *soundData;

    // Clear any OpenAL or alut error conditions
    alGetError();
    alutGetError();

    // Set all parameters to initial values.  These will be the values
    // that remain, unless we successfully open and buffer the data
    bufferSize = 0;
    bufferFrequency = 0;
    bufferFormat = 0;

    // Load the WAV file, keep track of the data format parameters as well
    soundData = alutLoadMemoryFromFile(fileName, &format, &size, &freq);

    // Make sure the load succeeds
    error = alutGetError();
    if ((soundData == NULL) && (error != ALUT_ERROR_NO_ERROR))
    {
        // Load failed, print an error
        printf("vsSoundSample::vsSoundSample:  Unable to load file: %s\n", 
            fileName);

        // Print out more detailed info if we can
        switch(error)
        {
            case ALUT_ERROR_OUT_OF_MEMORY:
                printf("    Not enough memory to load file\n");
                break;

            case ALUT_ERROR_IO_ERROR:
                printf("    I/O error.  Could not find or open file\n");
                break;

            case ALUT_ERROR_UNSUPPORTED_FILE_TYPE:
                printf("    Unsupported file type\n");
                break;

            case ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE:
                printf("    File type is supported, but uses an unsupported "
                    "sub-type\n");
                break;

            case ALUT_ERROR_CORRUPT_OR_TRUNCATED_DATA:
                printf("    File contains corrupt or truncated data\n");
                break;

            default:
                printf("    Unknown error\n");
                break;
        }
    }
    else
    {
        // Generate an OpenAL buffer
        alGenBuffers(1, &bufferID);

        // Check for error
        if (alGetError() != AL_NO_ERROR)
        {
            printf("vsSoundSample::vsSoundSample:  Unable to generate a buffer"
                " to store audio data.");
            printf("    Make sure a valid vsSoundPipe has been created.\n");

            // Free the audio data we loaded before bailing out
            if (soundData != NULL)
                free(soundData);

            // Now, bail out
            return;
        }
        
        // Pass the WAV file data and parameters to the buffer
        alBufferData(bufferID, format, soundData, size, (ALuint)freq);

        // Check for error
        if (alGetError() != AL_NO_ERROR)
        {
            printf("vsSoundSample::vsSoundSample:  Unable to fill the audio "
                "buffer with data.");
            printf("    Make sure a valid vsSoundPipe has been created.\n");

            // Free the audio data we loaded before bailing out
            if (soundData != NULL)
                free(soundData);

            // Now, bail out
            return;
        }
        
        // OpenAL makes a local copy of the sound data once we fill an 
        // alBuffer so we can discard the local copy of the sound data.
        free(soundData);

        // Store the sound parameters for later retrieval
        bufferSize = size;
        bufferFrequency = (int)freq;
        bufferFormat = format;
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
