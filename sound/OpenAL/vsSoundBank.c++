//------------------------------------------------------------------------
//
//     VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//     Copyright (c) 2007, University of Central Florida
//
//         See the file LICENSE for license information
//
//     E-mail:  vess@ist.ucf.edu
//     WWW:      http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//     VESS Module:  vsSoundBank.c++
//
//     Description:  A class used to store a bank of sounds that can have
//                   multiple copies of the same sounds playing.  It also
//                   manages the removal of the sound source attributes by
//                   way of VESS reference counting
//
//            Note:  This class is meant to be used with sounds that play
//                   multiple times in sucession.  This is not meant to be
//                   used with looping audio
//
//     Author(s):     Michael Whiteley
//
//------------------------------------------------------------------------

#include <atString.h++>
#include "vsSoundAttributeComponentTuple.h++"
#include "vsSoundBank.h++"
#include "vsSoundSampleRef.h++"

//------------------------------------------------------------------------
// Constructor for the vsSoundBank
// Sets default values for sound pause (false), priority 1,
// also sets the root component to null
// and creates the needed list and map for this classes
//------------------------------------------------------------------------
vsSoundBank::vsSoundBank()
{
    // Initialize the soundPaused variable to false to indicate that
    // there are no sounds currently paused
    soundPaused = false;

    // Initialize the priority to 1, so that everything in this bank with
    // have a priority of 1
    soundAttributesPriority = 1;

    // Initialize the root component to NULL, so we aren't accessing random
    // memory
    rootComponent = NULL;

    // Create the list and maps that are required for this classes
    playingSounds = new atList();
    soundCache = new atMap();
}

//------------------------------------------------------------------------
// Constructor for the vsSoundBank
// Sets default values for sound pause (false), priority (passed in value),
// also sets the root component to null
// and creates the needed list and map for this classes
//------------------------------------------------------------------------
vsSoundBank::vsSoundBank(int priority)
{
    // Initialize the soundPaused variable to false to indicate that
    // there are no sounds currently paused
    soundPaused = false;

    // Initialize the priority to whatever the user passed in for this bank
    soundAttributesPriority = priority;

    // Initialize the root component to NULL, so we aren't accessing random
    // memory
    rootComponent = NULL;

    // Create the list and maps that are required for this classes
    playingSounds = new atList();
    soundCache = new atMap();
}

//------------------------------------------------------------------------
// Destructor for the vsSoundBank
// If the rootComponent is not null unrefdelete it
// Clear the banks (which will delete everything in the cache and
// the playing sound stream)
//------------------------------------------------------------------------
vsSoundBank::~vsSoundBank()
{
    // Make sure that there is a rootComponent before trying to unrefDelete
    if (rootComponent != NULL)
    {
        // unrefDelete the root component.  We do this instead of just regular
        // delete because a user may have decided to use the component
        // in more than one place
        vsObject::unrefDelete(rootComponent);
    }

    // Call the clearBanks function to empty out the cache and play list
    clearBanks();

    // Finally clean up the last bit of memory that needs to be deleted
    delete playingSounds;
    delete soundCache;
}

//------------------------------------------------------------------------
// Sets the current root component.  If the rootComponent was already
// set it will unrefDelete the component before referencing the new one
//------------------------------------------------------------------------
void vsSoundBank::setRootComponent(vsComponent *root)
{
    // Check to make sure that the root componenet wasn't already set
    // if it was then unrefDelete the object
    if (rootComponent != NULL)
    {
        // UnrefDelete the object
        vsObject::unrefDelete(rootComponent);
    }

    // Store the new root
    rootComponent = root;

    // If the root isn't null then reference it!
    if (rootComponent != NULL)
    {
        rootComponent->ref();
    }
}

//------------------------------------------------------------------------
// Returns the current rootComponent
//------------------------------------------------------------------------
vsComponent *vsSoundBank::getRootComponent()
{
    // Return the current root component
    return rootComponent;
}

//------------------------------------------------------------------------
// Sets the priority that will be placed on the attributes that will be
// generated from this point out
//------------------------------------------------------------------------
void vsSoundBank::setPriority(int priority)
{
    // Set the priority
    soundAttributesPriority = priority;
}

//------------------------------------------------------------------------
// Gets the priority that will be placed on the attributes that will be
// created from this point out
//------------------------------------------------------------------------
int vsSoundBank::getPriority()
{
    return soundAttributesPriority;
}

//------------------------------------------------------------------------
// Adds a sound sample to the sound cache for use later
//------------------------------------------------------------------------
void vsSoundBank::addSoundSample(char *key, vsSoundSample *sample)
{
    // Create an atString to be used as the key
    atString *atKey = new atString(key);

    // Place the item in the map
    soundCache->addEntry(atKey,new vsSoundSampleRef(sample));
}

//------------------------------------------------------------------------
// Adds a sound sample to the sound cache for later use
//------------------------------------------------------------------------
void vsSoundBank::addSoundSample(char *key, char *filename)
{
    // Call the other addSoundSample once the vsSoundSample has been created
    addSoundSample(key, new vsSoundSample(filename));
}

//------------------------------------------------------------------------
// Removes the sounds sample with the given key
//------------------------------------------------------------------------
void vsSoundBank::removeSoundSample(char *key)
{
    // Create the key to be looking for
    atString *atKey = new atString(key);

    // Make sure the map contains the key
    if (soundCache->containsKey(atKey))
    {
        // Now that we know it exists, remove the item from the map
        soundCache->deleteEntry(atKey);
    }

    // Delete the key that was used for looking
    delete atKey;
}

//------------------------------------------------------------------------
// Calls the stopAllSound function (which will purge the playing Sounds
// list) and will then delete all of the objects in the map cache
//------------------------------------------------------------------------
void vsSoundBank::clearBanks()
{
    // First stop all sound to clear the playingSounds list
    stopAllSound();

    // Next delete the soundCache, this will dereference all sound samples
    // we have stored
    soundCache->clear();
}

//------------------------------------------------------------------------
// Creates a vsSoundSourceAttribute with the sound sample retrieved
// using the key.  It then either attaches it to the passed in component
// or the default component (if the passed in value is null).  Afterwards
// the component/attribute pair is added to the playing sounds list for
// updating later.  Play is then called on the attribute
//------------------------------------------------------------------------
void vsSoundBank::playSound(char *key, vsComponent *source)
{
    // Make the key to use to look up the object
    atString *atKey = new atString(key);
    vsSoundAttributeComponentTuple *tuple;
    vsSoundSample *sample;
    vsSoundSourceAttribute *soundSourceAttribute;

    // Only proceed if the sound cache has the key
    if (soundCache->containsKey(atKey))
    {
        // Get the sample from the atMap
        sample = ((vsSoundSampleRef *)soundCache->getValue(atKey))->getSample();

        // If the source is null then set the source to be the
        // root component
        if (source == NULL)
        {
            // Set the source to be the predefined component
            source = rootComponent;
        }

        // Make sure the source is not null at this point.  We check
        // because it is not assured that the rootComponent variable
        // will be assigned by this point
        if (source != NULL)
        {
            // Create the vsSoundSourceAttribute for playing purposes
            soundSourceAttribute = new vsSoundSourceAttribute(
                sample, false);

            // Set it's priority to that of this bank
            soundSourceAttribute->setPriority(soundAttributesPriority);

            // Add the attribute to the component
            source->addAttribute(soundSourceAttribute);

            // Create the tuple
            tuple = 
                new vsSoundAttributeComponentTuple(
                    soundSourceAttribute, source);

            // Add the tuple to the list of playing sounds
            playingSounds->addEntry(tuple);

            // Tell the sound attribute to play
            soundSourceAttribute->play();
        }
    }

    // Delete the key that we created
    delete atKey;
}

//------------------------------------------------------------------------
// Loop through all of the sounds in the playingSounds list and pause
// them, then toggle the soundPaused boolean
//------------------------------------------------------------------------
void vsSoundBank::pauseAllSound()
{
    vsSoundAttributeComponentTuple *tuple;

    // Start at the head of the list and loop through pausing all samples
    tuple = (vsSoundAttributeComponentTuple *)playingSounds->getFirstEntry();
    while (tuple != NULL)
    {
        // Pause the sound
        tuple->getSoundSourceAttribute()->pause();

        // Move to the next entry in the list
        tuple = (vsSoundAttributeComponentTuple *)playingSounds->getNextEntry();
    }

    // Tell this bank that all sound is paused at the moment
    soundPaused = true;
}

//------------------------------------------------------------------------
// This will loop through all of the sounds in the queue (if sound is paused)
// and call the play command (which should let the sound continue playing.
//------------------------------------------------------------------------
void vsSoundBank::resumeAllSound()
{
    vsSoundAttributeComponentTuple *tuple;

    // If the sound is not paused then exit this function
    if (soundPaused == false)
    {
        // Don't do anything.  Just leave.
        return;
    }

    // Start at the head of the list and loop through unpausing all samples
    tuple = (vsSoundAttributeComponentTuple *)playingSounds->getFirstEntry();
    while (tuple != NULL)
    {
        // Tel the sound to play
        tuple->getSoundSourceAttribute()->play();

        // Move to the next entry in the list
        tuple = (vsSoundAttributeComponentTuple *)playingSounds->getNextEntry();
    }

    // Tell the system that the sound is no longer paused
    soundPaused = false;
}

//------------------------------------------------------------------------
// Stop all sound will go through every thing in the list and stop it
// and then call the update command in order to clear the list of playing
// sounds
//------------------------------------------------------------------------
void vsSoundBank::stopAllSound()
{
    vsSoundAttributeComponentTuple *tuple;

    // Start at the head of the list and loop through stoping all samples
    tuple = (vsSoundAttributeComponentTuple *)playingSounds->getFirstEntry();
    while (tuple != NULL)
    {
        // Tell the sound to stop
        tuple->getSoundSourceAttribute()->stop();

        // Move to the next entry in the list
        tuple = (vsSoundAttributeComponentTuple *)playingSounds->getNextEntry();
    }

    // Make sure that the system is no longer paused since it has no sounds
    soundPaused = false;

    // Call the update function in order to purge sounds
    update();
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char * vsSoundBank::getClassName()
{
    return "vsSoundBank";
}

// ------------------------------------------------------------------------
// If none of the sounds in the sound bank are playing then this should
// return false.  This is done by a check on the number of items in 
// the playing sound list
// ------------------------------------------------------------------------
bool vsSoundBank::isSoundPlaying()
{
    // If there are any sounds in the playingSounds list then return true
    return playingSounds->getNumEntries() > 0;
}

//------------------------------------------------------------------------
// The update call will check all component/attribute pairs in the playing
// sounds list and remove those that are no longer playing.  Removing them
// involves removing the sound source attribute from the component and also
// unref deleting the component
//------------------------------------------------------------------------
void vsSoundBank::update()
{
    vsSoundAttributeComponentTuple *tuple;

    // Start at the head of the list and loop through, for every sample that
    // is not playing remove it from the list
    tuple = (vsSoundAttributeComponentTuple *)playingSounds->getFirstEntry();
    while (tuple != NULL)
    {
        // If the sound is not playing then delete it from the list
        // This must be done by removing the entry from the list and then
        // calling delete on the object manually
        if (tuple->getSoundSourceAttribute()->isPlaying() == false)
        {
            // Remove the entry from the list of playing sounds
            playingSounds->removeCurrentEntry();

            // Then call delete on the tuple.  The touple handles the
            // unref delete and attribute removal from the component
            delete tuple;
        }

        // Move to the next entry in the list
        tuple = (vsSoundAttributeComponentTuple *)playingSounds->getNextEntry();
    }

}
