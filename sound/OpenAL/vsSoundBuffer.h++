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
//    VESS Module:  vsSoundBuffer.h++
//
//    Description:  Sound buffer base class
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_SOUND_BUFFER_HPP
#define VS_SOUND_BUFFER_HPP

#include <AL/al.h>
#include "vsGlobals.h++"
#include "vsObject.h++"

// vsSoundSample and vsSoundStream are descendants of this class.
// Does nothing but define the interface for use by vsSoundSourceAttribute
// and other classes that might use sound buffers.

#define VS_SBUF_FORMAT_MONO8    AL_FORMAT_MONO8
#define VS_SBUF_FORMAT_MONO16   AL_FORMAT_MONO16
#define VS_SBUF_FORMAT_STEREO8  AL_FORMAT_STEREO8
#define VS_SBUF_FORMAT_STEREO16 AL_FORMAT_STEREO16
#define VS_SBUF_FORMAT_UNKNOWN  AL_NONE

enum
{
    VS_SOUND_BUFFER_SAMPLE,
    VS_SOUND_BUFFER_STREAM
};

class VS_SOUND_DLL vsSoundBuffer : public vsObject
{
protected:

    // Sound format parameters
    int    bufferFormat;
    int    bufferFrequency;
    int    bufferSize;

public:

                      vsSoundBuffer();
    virtual           ~vsSoundBuffer();

    virtual int       getBufferType() = 0;

    virtual int       getFormat();
    virtual int       getFrequency();
    virtual int       getBufferSize();
    virtual double    getLength();

    // These quantities are derived from the format
    int               getChannelCount();
    int               getBytesPerSample();
};

#endif
