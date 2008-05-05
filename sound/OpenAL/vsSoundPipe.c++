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
#include <AL/alut.h>

// ------------------------------------------------------------------------
// Creates a vsSoundPipe using the specified audio device (from .openalrc
// [Linux] or the device enumeration extension [Windows]) and the given
// internal buffer frequency
// ------------------------------------------------------------------------
vsSoundPipe::vsSoundPipe(char *deviceSpec, int freq)
{
    int attrList[3];

    // Initialize ALUT without a context (we'll do the context ourselves, but
    // we still need to initialize ALUT to be able to load files)
    alutInitWithoutContext(0, NULL);

    // Open the audio device
    deviceHandle = alcOpenDevice((ALCchar *)deviceSpec);
    
    // Check the handle returned, if it's NULL, print an error and try the
    // default device instead
    if (deviceHandle == NULL)
    {
        printf("vsSoundPipe::vsSoundPipe:  Unable to open the specified\n");
        printf("    device (%s).  Switching to default device\n", deviceSpec);
        
        deviceHandle = alcOpenDevice(NULL);
    }

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
// Creates a vsSoundPipe on the default device with the given internal 
// buffer frequency
// ------------------------------------------------------------------------
vsSoundPipe::vsSoundPipe(int freq)
{
    int attrList[3];

    // Initialize ALUT without a context (we'll do the context ourselves, but
    // we still need to initialize ALUT to be able to load files)
    alutInitWithoutContext(0, NULL);

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

    // Initialize ALUT without a context (we'll do the context ourselves, but
    // we still need to initialize ALUT to be able to load files)
    alutInitWithoutContext(0, NULL);

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
    // Unregister from the sound manager
    vsSoundManager::getInstance()->removeSoundPipe(this);

    // Destroy the OpenAL context
    alcDestroyContext(pipeHandle);

    // Close the device
    if (deviceHandle)
        alcCloseDevice(deviceHandle);

    // Clean up ALUT
    alutExit();
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
