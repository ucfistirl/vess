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
//    VESS Module:  vsSoundManager.c++
//
//    Description:  Singleton class to watch over all sound
//                  operations.  Updates all sources and the listener
//                  automatically.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsSoundManager.h++"

// Static instance variable
vsSoundManager *vsSoundManager::instance = NULL;

// ------------------------------------------------------------------------
// Constructor for the vsSoundManager object.  This is not called directly,
// but by the getInstance() singleton accessor method.  The soundSources 
// growable array is created initially with size zero, so that little space 
// is taken if the application doesn't require any audio functionality.
// ------------------------------------------------------------------------
vsSoundManager::vsSoundManager()
{
    // Initialize the soundPipe pointer to NULL
    soundPipe = NULL;
    
    // Initialize the soundListener pointer to NULL
    soundListener = NULL;

    // Inintialize the sound source counters and array
    numSoundSources = 0;
    memset(soundSources, 0, sizeof(soundSources));
}

// ------------------------------------------------------------------------
// Destrutor.  This is an internal method, as the single instance of
// vsSoundManager will be destroyed by the vsSystem object.
// ------------------------------------------------------------------------
vsSoundManager::~vsSoundManager()
{
    instance = NULL;
}

// ------------------------------------------------------------------------
// Registers a vsSound with the manager so it can be manipulated
// ------------------------------------------------------------------------
void vsSoundManager::setSoundPipe(vsSoundPipe *pipe)
{
    // Ignore the request if there is already a sound pipe present
    if (soundPipe != NULL)
    {
        printf("vsSoundManager::setSoundPipe:  A sound pipe already "
            "exists!\n");
        return;
    }

    // Set the sound pipe pointer to the given object
    soundPipe = pipe;
}

// ------------------------------------------------------------------------
// Removes the vsSoundPipe from the manager
// ------------------------------------------------------------------------
void vsSoundManager::removeSoundPipe(vsSoundPipe *pipe)
{
    // Ignore the request if there is no sound listener registered
    if (soundPipe == NULL)
    {
        printf("vsSoundManager::removeSoundPipe:  No sound pipe "
            "registered!\n");
        return;
    }

    // Also ignore the request if the registered listener attribute doesn't
    // match the given attribute
    if (soundPipe != pipe)
    {
        printf("vsSoundManager::removeSoundListener:  Registered sound "
            "pipe does not match given sound pipe!\n");
        return;
    }

    // Set the sound pipe pointer to NULL
    soundPipe = NULL;
}

// ------------------------------------------------------------------------
// Registers a vsSoundSourceAttribute with the manager so it can be updated
// ------------------------------------------------------------------------
void vsSoundManager::addSoundSource(vsSoundSourceAttribute *attr)
{
    // Add the sound source to the sources array
    soundSources[numSoundSources] = attr;

    // Increment the number of sources
    numSoundSources++;
}

// ------------------------------------------------------------------------
// Removes a vsSoundSourceAttribute from the manager
// ------------------------------------------------------------------------
void vsSoundManager::removeSoundSource(vsSoundSourceAttribute *attr)
{
    int attrIndex, i;

    // Find the sound source in the array
    attrIndex = 0;
    while ((attrIndex < numSoundSources) && 
        (soundSources[attrIndex] != attr))
    {
        attrIndex++;
    }

    // If we found the attribute, slide the remaining attributes down
    // into its place
    if (attrIndex < numSoundSources)
    {
        for (i = attrIndex; i < numSoundSources; i++)
            soundSources[i+1] = soundSources[i];
    }

    // Decrement the number of sources
    numSoundSources--;
}

// ------------------------------------------------------------------------
// Register the sound listener object with the manager so it can be updated
// ------------------------------------------------------------------------
void vsSoundManager::setSoundListener(vsSoundListenerAttribute *attr)
{
    // Ignore the request if there is already a sound listener present
    if (soundListener != NULL)
    {
        printf("vsSoundManager::setSoundListener:  A sound listener already "
            "exists!\n");
        return;
    }

    // Set the sound listener pointer to the given attribute
    soundListener = attr;
}

// ------------------------------------------------------------------------
// Remove the sound listener object from the manager
// ------------------------------------------------------------------------
void vsSoundManager::removeSoundListener(vsSoundListenerAttribute *attr)
{
    // Ignore the request if there is no sound listener registered
    if (soundListener == NULL)
    {
        printf("vsSoundManager::removeSoundListener:  No sound listener "
            "registered!\n");
        return;
    }

    // Also ignore the request if the registered listener attribute doesn't
    // match the given attribute
    if (soundListener != attr)
    {
        printf("vsSoundManager::removeSoundListener:  Registered sound "
            "listener does not match given sound listener!\n");
        return;
    }

    // Set the sound listener pointer to NULL
    soundListener = NULL;
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsSoundManager::getClassName()
{
    return "vsSoundManager";
}

// ------------------------------------------------------------------------
// Return the current instance of this class, creating one if necessary
// ------------------------------------------------------------------------
vsSoundManager *vsSoundManager::getInstance()
{
    // Check to see if an instance exists, and create one if not
    if (instance == NULL)
    {
        instance = new vsSoundManager();
    }

    // Return the singleton instance of this class
    return instance;
}

// ------------------------------------------------------------------------
// Update all sources and the listener
// ------------------------------------------------------------------------
void vsSoundManager::update()
{
    int i;

    // Update all sound sources
    for (i = 0; i < numSoundSources; i++)
    {
        soundSources[i]->update();
    }

    // Update the sound listener
    if (soundListener)
        soundListener->update();
}
