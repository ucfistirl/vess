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

#include "vsGlobals.h++"
#include "vsObject.h++"

// vsSoundSample and vsSoundStream are descendants of this class.
// Does nothing but define the interface for use by vsSoundSourceAttribute
// and other classes that might use sound buffers.

class VS_SOUND_DLL vsSoundBuffer : public vsObject
{
public:

                      vsSoundBuffer();
    virtual           ~vsSoundBuffer();
};

#endif
