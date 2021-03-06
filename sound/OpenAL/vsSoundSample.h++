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
//    VESS Module:  vsSoundSample.h++
//
//    Description:  Loads and maintains a static buffer of audio data from
//                  a given sound file
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_SOUND_SAMPLE_HPP
#define VS_SOUND_SAMPLE_HPP

// This implementation uses OpenAL

#include <AL/al.h>
#include "vsGlobals.h++"
#include "vsSoundBuffer.h++"

class VESS_SYM vsSoundSample : public vsSoundBuffer
{
protected:

    // Handle of the buffer (for OpenAL)
    ALuint           bufferID;

    // Sample parameters
    unsigned long    dataSize;
    unsigned long    frequency;
    int              numChannels;
    int              bytesPerSample;

VS_INTERNAL:

    virtual ALuint    getBufferID();

public:

                     vsSoundSample(char *filename);
    virtual          ~vsSoundSample();

    const char *     getClassName();

    virtual int      getBufferType();

    ALuint           getBaseLibraryObject();
};

#endif
