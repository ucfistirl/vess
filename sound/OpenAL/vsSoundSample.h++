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
#include <AL/alc.h>
#include "vsGlobals.h++"
#include "vsSoundBuffer.h++"

class VS_SOUND_DLL vsSoundSample : public vsSoundBuffer
{
protected:

    // Handle of the buffer (for OpenAL)
    ALuint      bufferID;

VS_INTERNAL:

    virtual ALuint    getBufferID();

public:

                      vsSoundSample(char *filename);
    virtual           ~vsSoundSample();

    ALuint            getBaseLibraryObject();
};

#endif
