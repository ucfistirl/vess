#ifndef VS_SOUND_PIPE_HPP
#define VS_SOUND_PIPE_HPP

// Constructs and maintains all low-level access to the audio hardware
// Also handles all global sound options (such as distance attenuation scale)
// This implementation uses OpenAL

class vsSoundPipe
{
protected:

    // Handle to the audio hardware (an OpenAL context ID in this case)
    void    *pipeHandle;

public:

                // NOTE:  The channels parameter won't do anything at this
                //        time (you'll get two channels by default)
                vsSoundPipe(int freq, int width, int channels);
                vsSoundPipe();
                ~vsSoundPipe();

    // Global Doppler effect parameters
    double      getDopplerScale();
    void        setDopplerScale(double scale);

    double      getDopplerVelocity();
    void        setDopplerVelocity(double speed);
};

#endif
