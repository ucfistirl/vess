#include <AL/al.h>
#include <AL/alc.h>
#include "vsSoundPipe.h++"

// ------------------------------------------------------------------------
// Creates a vsSoundPipe with the given parameters
// NOTE:  OpenAL currently does not support the channels option, a sound
//        pipe will always have two channels by default
// ------------------------------------------------------------------------
vsSoundPipe::vsSoundPipe(int freq, int width, int channels)
{
     int attrList[7];

     attrList[0] = ALC_FREQUENCY;
     attrList[1] = freq;
     attrList[2] = ALC_RESOLUTION;
     attrList[3] = width;
     attrList[4] = ALC_CHANNELS;
     attrList[5] = channels;
     attrList[6] = 0;

     pipeHandle = alcCreateContext(attrList);
}

// ------------------------------------------------------------------------
// Creates a vsSoundPipe with the default parameters (depends on the 
// OpenAL implementation)
// ------------------------------------------------------------------------
vsSoundPipe::vsSoundPipe()
{
    int attrList[1];

    attrList[0] = 0;

    pipeHandle = alcCreateContext(attrList);
}

// ------------------------------------------------------------------------
// Destroys a vsSoundPipe
// ------------------------------------------------------------------------
vsSoundPipe::~vsSoundPipe()
{
    alcDestroyContext(pipeHandle);
}

// ------------------------------------------------------------------------
// Returns the current scaling factor for Doppler effects (default = 1.0)
// ------------------------------------------------------------------------
double vsSoundPipe::getDopplerScale()
{
    float scale;

    alGetFloatv(AL_DOPPLER_SCALE, &scale);

    return (double)scale;
}

// ------------------------------------------------------------------------
// Sets the scaling factor for Doppler effects
// ------------------------------------------------------------------------
void vsSoundPipe::setDopplerScale(double scale)
{
    alDopplerScale((float)scale);
}

// ------------------------------------------------------------------------
// Returns the current reference velocity (speed of sound) for Doppler 
// effects (default = 1.0)
// ------------------------------------------------------------------------
double vsSoundPipe::getDopplerVelocity()
{
    float speed;

    alGetFloatv(AL_PROPAGATION_SPEED, &speed);

    return (double)speed;
}

// ------------------------------------------------------------------------
// Sets the reference velocity for Doppler effects
// ------------------------------------------------------------------------
void vsSoundPipe::setDopplerVelocity(double speed)
{
    alPropagationSpeed((float)speed);
}
