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
    ALenum  error;
    ALint   bufferChannels;
    ALint   bufferBits;

    // Clear any OpenAL or alut error conditions
    alGetError();
    alutGetError();

    // Set all parameters to initial values.  These will be the values
    // that remain, unless we successfully open and buffer the data
    bufferSize = 0;
    bufferFrequency = 0;
    bufferFormat = 0;

    // Get an OpenAL buffer using the WAV file for data
    bufferID = alutCreateBufferFromFile(fileName);

    // Make sure the load succeeds
    error = alutGetError();
    if ((bufferID == AL_NONE) && (error != ALUT_ERROR_NO_ERROR))
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
        // Query and store the sound parameters for later retrieval
        alGetBufferiv(bufferID, AL_SIZE, &bufferSize);
        alGetBufferiv(bufferID, AL_FREQUENCY, &bufferFrequency);
        alGetBufferiv(bufferID, AL_BITS, &bufferBits);
        alGetBufferiv(bufferID, AL_CHANNELS, &bufferChannels);
        
        // Check for errors
        if (alGetError() != AL_NO_ERROR)
        {
            // Print a warning that we may have bad attributes
            printf("vsSoundSample::vsSoundSample:  Unable to query audio "
                "attributes for file %s\n", fileName);
        }
        
        // Convert the bits/channels attributes to a "format"
        if (bufferChannels == 1)
        {
            // This is a mono sound, figure out the sample width and
            // set the format
            if (bufferBits == 8)
            {
                // 8-bit mono sound
                bufferFormat = VS_SBUF_FORMAT_MONO8;
            }
            else if (bufferBits == 16)
            {
                // 16-bit mono sound
                bufferFormat = VS_SBUF_FORMAT_MONO16;
            }
            else
            {
                // This is either an error or unsupported format
                bufferFormat = VS_SBUF_FORMAT_UNKNOWN;
            }
        }
        else if (bufferChannels == 2)
        {
            // This is a stereo sound, figure out the sample width and
            // set the format
            if (bufferBits == 8)
            {
                // 8-bit stereo sound
                bufferFormat = VS_SBUF_FORMAT_STEREO8;
            }
            else if (bufferBits == 16)
            {
                // 16-bit stereo sound
                bufferFormat = VS_SBUF_FORMAT_STEREO16;
            }
            else
            {
                // This is either an error or an unsupported format
                bufferFormat = VS_SBUF_FORMAT_UNKNOWN;
            }
        }
        else
        {
            // This is either an error or an unsupported format
            bufferFormat = VS_SBUF_FORMAT_UNKNOWN;
        }
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
