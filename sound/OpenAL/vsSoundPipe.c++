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

#include "vsSoundPipe.h++"
#include "vsSoundManager.h++"
#include <stdio.h>

// ------------------------------------------------------------------------
// Creates a vsSoundPipe with the given internal buffer frequency
// ------------------------------------------------------------------------
vsSoundPipe::vsSoundPipe(int freq)
{
    int attrList[3];

    // Open the audio device
    deviceHandle = alcOpenDevice(NULL);

    // Create an attribute list for the frequency argument
    attrList[0] = ALC_FREQUENCY;
    attrList[1] = freq;
    attrList[2] = 0;

    // Create an audio context using the specified mixing frequency
    pipeHandle = alcCreateContext(deviceHandle, attrList);

    // Initialize the audio context
    alcMakeContextCurrent(pipeHandle);

    // Register with the sound manager
    vsSoundManager::getInstance()->setSoundPipe(this);
}

// ------------------------------------------------------------------------
// Creates a vsSoundPipe with the default parameters (depends on the 
// OpenAL implementation)
// ------------------------------------------------------------------------
vsSoundPipe::vsSoundPipe()
{
    int attrList[1];

    // Create an empty attribute list, so OpenAL will use the default
    // settings
    attrList[0] = 0;

    // Open the audio device
    deviceHandle = alcOpenDevice(NULL);

    // Create an audio context
    pipeHandle = alcCreateContext(deviceHandle, attrList);

    // Initialize the audio context
    alcMakeContextCurrent(pipeHandle);

    // Register with the sound manager
    vsSoundManager::getInstance()->setSoundPipe(this);
}

// ------------------------------------------------------------------------
// Destroys a vsSoundPipe
// ------------------------------------------------------------------------
vsSoundPipe::~vsSoundPipe()
{
    // Destroy the OpenAL context
    alcDestroyContext(pipeHandle);

    // Close the device
    if (deviceHandle)
        alcCloseDevice(deviceHandle);

    // Unregister from the sound manager
    vsSoundManager::getInstance()->removeSoundPipe(this);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsSoundPipe::getClassName()
{
    return "vsSoundPipe";
}

// ------------------------------------------------------------------------
// Returns the current scaling factor for Doppler effects (default = 1.0)
// ------------------------------------------------------------------------
double vsSoundPipe::getDopplerScale()
{
    float scale;

    // Get the current Doppler scale factor from OpenAL
    alGetFloatv(AL_DOPPLER_FACTOR, &scale);

    // Return the scale factor
    return (double)scale;
}

// ------------------------------------------------------------------------
// Sets the scaling factor for Doppler effects
// ------------------------------------------------------------------------
void vsSoundPipe::setDopplerScale(double scale)
{
    // Set the Doppler scale factor to the given value
    alDopplerFactor((float)scale);
}

// ------------------------------------------------------------------------
// Returns the current reference velocity (speed of sound) for Doppler 
// effects (default = 1.0)
// ------------------------------------------------------------------------
double vsSoundPipe::getDopplerVelocity()
{
    float speed;

    // Get the current Doppler reference velocity from OpenAL
    alGetFloatv(AL_DOPPLER_VELOCITY, &speed);

    // Return the velocity
    return (double)speed;
}

// ------------------------------------------------------------------------
// Sets the reference velocity for Doppler effects
// ------------------------------------------------------------------------
void vsSoundPipe::setDopplerVelocity(double speed)
{
    // Set the Doppler velocity to the specified value
    alDopplerVelocity((float)speed);
}

// ------------------------------------------------------------------------
// Returns the corresponding base library object (an OpenAL context handle
// as a void * in this case)
// ------------------------------------------------------------------------
void *vsSoundPipe::getBaseLibraryObject()
{
    return pipeHandle;
}
