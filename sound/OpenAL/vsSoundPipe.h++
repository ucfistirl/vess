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
//    VESS Module:  vsSoundPipe.h++
//
//    Description:  Constructs and maintains all low-level access to the
//                  audio hardware. Also handles all global sound options
//                  (such as distance attenuation scale).
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_SOUND_PIPE_HPP
#define VS_SOUND_PIPE_HPP

// This implementation uses OpenAL

#include "vsGlobals.h++"
#include <AL/al.h>
#include <AL/alc.h>

class VS_SOUND_DLL vsSoundPipe
{
protected:

    // Handle to the audio hardware (an OpenAL device ID in this case)
    ALCdevice     *deviceHandle;

    // Handle to the audio rendering context
    ALCcontext    *pipeHandle;

public:

                vsSoundPipe(int freq);
                vsSoundPipe();
                ~vsSoundPipe();

    // Global Doppler effect parameters
    double      getDopplerScale();
    void        setDopplerScale(double scale);

    double      getDopplerVelocity();
    void        setDopplerVelocity(double speed);

    void        *getBaseLibraryObject();
};

#endif
