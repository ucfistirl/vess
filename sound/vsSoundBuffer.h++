#ifndef VS_SOUND_BUFFER_HPP
#define VS_SOUND_BUFFER_HPP

// Sound buffer base class (vsSoundSample and vsSoundStream are descendants).
// Does nothing but define the interface for use by vsSoundSourceAttribute
// and other classes that might use sound buffers.

class vsSoundBuffer
{
public:

                      vsSoundBuffer();
    virtual           ~vsSoundBuffer();
};

#endif
