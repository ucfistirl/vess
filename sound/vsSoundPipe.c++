#include "vsSoundPipe.h++"
#include <stdio.h>

// ------------------------------------------------------------------------
// Creates a vsSoundPipe with the given internal buffer frequency
// ------------------------------------------------------------------------
vsSoundPipe::vsSoundPipe(int freq, int width, int channels)
{
    int attrList[3];

    deviceHandle = alcOpenDevice(NULL);

    attrList[0] = ALC_FREQUENCY;
    attrList[1] = freq;
    attrList[2] = 0;

    pipeHandle = alcCreateContext(deviceHandle, attrList);

    alcMakeContextCurrent(pipeHandle);
}

// ------------------------------------------------------------------------
// Creates a vsSoundPipe with the default parameters (depends on the 
// OpenAL implementation)
// ------------------------------------------------------------------------
vsSoundPipe::vsSoundPipe()
{
    int attrList[1];

    attrList[0] = 0;

    deviceHandle = alcOpenDevice(NULL);
    pipeHandle = alcCreateContext(deviceHandle, attrList);
    alcMakeContextCurrent(pipeHandle);
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

    alGetFloatv(AL_DOPPLER_FACTOR, &scale);

    return (double)scale;
}

// ------------------------------------------------------------------------
// Sets the scaling factor for Doppler effects
// ------------------------------------------------------------------------
void vsSoundPipe::setDopplerScale(double scale)
{
    alDopplerFactor((float)scale);
}

// ------------------------------------------------------------------------
// Returns the current reference velocity (speed of sound) for Doppler 
// effects (default = 1.0)
// ------------------------------------------------------------------------
double vsSoundPipe::getDopplerVelocity()
{
    float speed;

    alGetFloatv(AL_DOPPLER_VELOCITY, &speed);

    return (double)speed;
}

// ------------------------------------------------------------------------
// Sets the reference velocity for Doppler effects
// ------------------------------------------------------------------------
void vsSoundPipe::setDopplerVelocity(double speed)
{
    alDopplerVelocity((float)speed);
}
