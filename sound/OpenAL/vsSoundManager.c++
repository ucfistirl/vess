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
// but by the getInstance() singleton accessor method.
// ------------------------------------------------------------------------
vsSoundManager::vsSoundManager()
{
    bool alError;
    int i;

    // Initialize the soundPipe pointer to NULL
    soundPipe = NULL;
    
    // Initialize the soundListener pointer to NULL
    soundListener = NULL;

    // Inintialize the sound source counter and array
    numSoundSources = 0;
    memset(soundSources, 0, sizeof(soundSources));

    // Initialize the voice counter and array
    numVoices = 0;
    memset(voices, 0, sizeof(voices));

    // Create the list of sound sources
    for (i = 0; i < VS_SDM_MAX_SOUNDS; i++)
    {
        soundSources[i] = new vsSoundSourceListItem;
        soundSources[i]->source = NULL;
        soundSources[i]->gain = 0.0;
    }
    
    // Determine the hardware voice limit by generating OpenAL voices
    // until OpenAL signals an error
    alError = false;
    while ((!alError) && (numVoices < VS_SDM_MAX_VOICES))
    {
        // Generate an OpenAL source (a new voice)
        alGenSources(1, (ALuint *)&voices[numVoices]);

        // See if we successfully generated the voice
        if (alGetError() == AL_NO_ERROR)
            numVoices++;
        else
            alError = true;
    }
    hardwareVoiceLimit = numVoices;

    // Set the "soft" voice limit to the number of voices generated or to
    // the default,  whichever is less.
    voiceLimit = VS_SDM_DEFAULT_VOICE_LIMIT;
    if (voiceLimit > hardwareVoiceLimit)
        voiceLimit = hardwareVoiceLimit;

    // Release excess voices so that we only have the default number of
    // voices allocated (this limit can be changed by the user)
    if ((numVoices - voiceLimit) > 0)
    {
        alDeleteSources(numVoices - voiceLimit, (ALuint *)&voices[voiceLimit]);
        numVoices = voiceLimit;
    }
}

// ------------------------------------------------------------------------
// Destructor.  Should only be called from the static deleteInstance() 
// method, which should only be called by the vsSystem object.
// ------------------------------------------------------------------------
vsSoundManager::~vsSoundManager()
{
    instance = NULL;
}

// ------------------------------------------------------------------------
// Protected function.  Sort the sound sources by play state, then by
// priority, then by effective gain.  The sources will then be assigned
// voices in list order.
// ------------------------------------------------------------------------
void vsSoundManager::sortSources()
{
    int i, numPass;
    bool exch;
    bool playing1, playing2;
    int priority1, priority2;
    vsSoundSourceListItem *tempSrc;
    vsVector listenerPos;

    // Get the listener's position (for gain computation)
    listenerPos = soundListener->getLastPosition();

    // Clear the source gains array to -1 (sourceGain[i] < 0 indicates
    // that we haven't computed source i's gain yet)
    for (i = 0; i < VS_SDM_MAX_SOUNDS; i++)
    {
        soundSources[i]->gain = -1.0;
    }

    // Bubble sort the sources, using all sort keys simultaneously.  This
    // takes advantage of the fact that the sources' states will change
    // little over each frame.
    numPass = numSoundSources - 1;
    exch = true;

    // Repeat until the list is sorted
    while (exch)
    {
        // First, flag that we haven't made a change on this pass yet
        exch = false;

        // Compare each contiguous pair of sources
        for (i = 0; i < numPass; i++)
        {
            // See if the sources are playing
            playing1 = soundSources[i]->source->isPlaying();
            playing2 = soundSources[i+1]->source->isPlaying();

            // If only one source is playing, make sure it comes before
            // the other source in the list
            if (!playing1 && playing2)
            {
                tempSrc = soundSources[i];
                soundSources[i] = soundSources[i+1];
                soundSources[i+1] = tempSrc;
                exch = true;
            }
            else if (playing1 == playing2)
            {
                // Get the priorities of the two sources
                priority1 = soundSources[i]->source->getPriority();
                priority2 = soundSources[i+1]->source->getPriority();

                // Swap the sources if the priorities are out of order
                if (priority1 < priority2)
                {
                    tempSrc = soundSources[i];
                    soundSources[i] = soundSources[i+1];
                    soundSources[i+1] = tempSrc;
                    exch = true;
                }
                else if (priority1 == priority2)
                {
                    // Get the effective gains of each source.  Use the
                    // gains we've already computed, so we only compute the
                    // gain for each source once.
                    if (soundSources[i]->gain < 0.0)
                        soundSources[i]->gain = soundSources[i]->source->
                            getEffectiveGain(listenerPos);

                    if (soundSources[i+1]->gain < 0.0)
                        soundSources[i+1]->gain = soundSources[i+1]->source->
                            getEffectiveGain(listenerPos);

                    // Swap the sources if the first source is quieter than
                    // the second
                    if (soundSources[i]->gain < soundSources[i+1]->gain)
                    {
                        tempSrc = soundSources[i];
                        soundSources[i] = soundSources[i+1];
                        soundSources[i+1] = tempSrc;
                        exch = true;
                    }
                }
            }
        }

        // Reduce the length of list considered for the next pass, because
        // we know that the numPass'th element is in it's proper place
        numPass--;
    }
}

// ------------------------------------------------------------------------
// Static internal function.  Should only be called by the vsSystem object.
// Deletes the current instance of the vsSoundManager, if one exists
// ------------------------------------------------------------------------
void vsSoundManager::deleteInstance()
{
    if (instance != NULL)
        delete instance;
}

// ------------------------------------------------------------------------
// Registers a vsSoundPipe with the manager so it can be manipulated
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

    // Set the sound pipe pointer to the given object and reference it
    soundPipe = pipe;
    soundPipe->ref();
}

// ------------------------------------------------------------------------
// Removes the vsSoundPipe from the manager
// ------------------------------------------------------------------------
void vsSoundManager::removeSoundPipe(vsSoundPipe *pipe)
{
    // Ignore the request if there is no sound pipe registered
    if (soundPipe == NULL)
    {
        printf("vsSoundManager::removeSoundPipe:  No sound pipe "
            "registered!\n");
        return;
    }

    // Also ignore the request if the registered sound pipe doesn't
    // match the given one
    if (soundPipe != pipe)
    {
        printf("vsSoundManager::removeSoundListener:  Registered sound "
            "pipe does not match given sound pipe!\n");
        return;
    }

    // Unreference the sound pipe and set the pointer to NULL
    soundPipe->unref();
    soundPipe = NULL;
}

// ------------------------------------------------------------------------
// Registers a vsSoundSourceAttribute with the manager so it can be updated
// ------------------------------------------------------------------------
void vsSoundManager::addSoundSource(vsSoundSourceAttribute *attr)
{
    // Add the sound source to the sources array and reference it
    soundSources[numSoundSources]->source = attr;
    soundSources[numSoundSources]->source->ref();

    // If we have a free voice available for the source, go ahead and
    // assign it one
    if (numVoices > 0)
    {
        // Reduce the number of available voices
        numVoices--;
       
        // Assign the voice to the sound source
        soundSources[numSoundSources]->source->
            assignVoice(voices[numVoices]);
    }

    // Increment the number of sources
    numSoundSources++;
}

// ------------------------------------------------------------------------
// Removes a vsSoundSourceAttribute from the manager
// ------------------------------------------------------------------------
void vsSoundManager::removeSoundSource(vsSoundSourceAttribute *attr)
{
    int attrIndex, i;
    vsSoundSourceListItem *deletedItem;

    // Find the sound source in the array
    attrIndex = 0;
    while ((attrIndex < numSoundSources) && 
        (soundSources[attrIndex]->source != attr))
    {
        attrIndex++;
    }

    // If we found the attribute, remove it from the array
    if (attrIndex < numSoundSources)
    {
        // If the source is active, free up it's voice so other sources
        // can use it
        if (soundSources[attrIndex]->source->isActive())
        {
            // Get the source's voice ID and return it to the list of
            // available voices
            voices[numVoices] = soundSources[attrIndex]->source->getVoiceID();
            numVoices++;

            // Revoke the source's voice ID
            soundSources[attrIndex]->source->revokeVoice();
        }

        // Unreference the attribute
        soundSources[attrIndex]->source->unref();

        // Clear the data in the deleted list item
        deletedItem = soundSources[attrIndex];
        deletedItem->source = NULL;
        deletedItem->gain = 0.0;
        
        // Decrement the number of sound sources in the list
        numSoundSources--;
        
        // Slide the remaining list members down into the deleted item's
        // place
        for (i = attrIndex; i < numSoundSources; i++)
        {
            soundSources[i] = soundSources[i+1];
        }
        
        // Finally, recycle the deleted sound source list item, by
        // putting the deleted item back at the end of the list.
        soundSources[numSoundSources] = deletedItem;
    }
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

    // Set the sound listener pointer to the given attribute and reference
    // the attribute
    soundListener = attr;
    soundListener->ref();
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

    // Unreference the sound listener and set the pointer to NULL
    soundListener->unref();
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
// Return the current voice limit (the maximum number of sounds playing
// at the same time)
// ------------------------------------------------------------------------
int vsSoundManager::getVoiceLimit()
{
   return voiceLimit;
}

// ------------------------------------------------------------------------
// Sets the maximum number of hardware or software voices used by the
// sound manager
// ------------------------------------------------------------------------
void vsSoundManager::setVoiceLimit(int newLimit)
{
    bool alError;
    int difference;
    int i;
    ALuint nextVoice;

    // Make sure we don't try to exceed the hardware's capabilities
    if (newLimit > hardwareVoiceLimit)
    {
        printf("vsSoundManager::setVoiceLimit:  Maximum voice limit is %d "
            "on this platform!\n", hardwareVoiceLimit);
        newLimit = hardwareVoiceLimit;
    }

    // Also make sure the new limit is nonnegative (zero voices is OK)
    if (newLimit < 0)
    {
        printf("vsSoundManager::setVoiceLimit:  Voice limit must be 0 or "
            "greater!\n");
        newLimit = 0;
    }

    // Calculate the difference between the new limit and the previous one
    difference = newLimit - voiceLimit;

    // If we're increasing the limit, generate new voices
    if (difference > 0)
    {
        // Generate voices until we hit the limit, or an error from OpenAL
        alError = false;
        i = 0;
        while ((!alError) && (i < difference))
        {
            // Generate an OpenAL source (a new voice)
            alGenSources(1, (ALuint *)&voices[numVoices]);

            // See if we successfully generated the voice
            if (alGetError() == AL_NO_ERROR)
            {
                numVoices++;
                i++;
            }
            else
                alError = true;
        }

        // Adjust the difference by i so we can compute an accurate voice
        // limit (in case we couldn't generate all the voices we needed)
        difference -= i;
    }
    else if (difference < 0)
    {
        // We need to free some voices.  Start by removing voices that aren't
        // assigned to sources (if any)
        while ((numVoices > 0) && (difference < 0))
        {
            // Delete the next voice
            numVoices--;
            alDeleteSources(1, (ALuint *)&voices[numVoices]);

            // Make sure it was really deleted
            if (alGetError() == AL_NO_ERROR)
                difference++;
        }

        // See if we need to release more voices
        if (difference < 0)
        {
            // Go through the array of sources, and revoke the voices
            // from the first active sources found (these are the ones
            // with the lowest priority)
            i = 0;
            while ((difference < 0) && (i < numSoundSources))
            {
                // See if the source is active (has a voice we can
                // take)
                if (soundSources[i]->source->isActive())
                {
                    // Revoke the source's voice and delete it
                    nextVoice = soundSources[i]->source->getVoiceID();
                    soundSources[i]->source->revokeVoice();
                    alDeleteSources(1, &nextVoice);

                    // Make sure it was really deleted
                    if (alGetError() == AL_NO_ERROR)
                        difference++;
                }

                // Move to the next source
                i++;
            }

            // The difference now represents the number of voices
            // we couldn't free to reach the new limit (this should
            // be zero in most cases).  Take the absolute value to
            // allow an accurate new voice limit to be computed.
            difference = abs(difference);
        }
    }

    // Set the new voice limit
    voiceLimit = newLimit - difference;
}

// ------------------------------------------------------------------------
// Update all sources and the listener
// ------------------------------------------------------------------------
void vsSoundManager::update()
{
    int i;

    // Update the sound listener
    if (soundListener)
    {
        soundListener->update();
    }

    // Update all sound sources
    for (i = 0; i < numSoundSources; i++)
    {
        // Update the source (position, velocity, orientation)
        soundSources[i]->source->update();
    }

    // Perform voice management if we have more sources in the
    // scene than voices to process them with
    if ((soundListener) && (numSoundSources > 0))
    {
        // Sort the sources by priority, then by effective gain
        sortSources();

        // Now, traverse the list, making sure that only the sources
        // that should be active are active.  First, deactivate the
        // sources that shouldn't be active (so we have the voices we
        // need for the active ones.
        for (i = voiceLimit; i < numSoundSources; i++)
        {
            // Check if the source is active
            if (soundSources[i]->source->isActive())
            {
                // See if this source is set to ALWAYS_ON priority.  If
                // it is (and it's currently playing), then we have too many
                // ALWAYS_ON sources in the scene.  Print an error (but swap
                // it out anyway)
                if ((soundSources[i]->source->isPlaying()) && 
                   (soundSources[i]->source->getPriority() == 
                        VS_SSRC_PRIORITY_ALWAYS_ON))
                {
                    printf("vsSoundManager::update:  Too many ALWAYS_ON"
                        " sources in the scene.\n");
                }

                // Get the voice ID from the source, and add it to the
                // list of available voices
                voices[numVoices] = soundSources[i]->source->getVoiceID();
                numVoices++;

                // Revoke the source's voice, making the source inactive
                soundSources[i]->source->revokeVoice();
            }
        }

        // Now, traverse the list of sources that should be active and
        // assign the inactive ones a voice
        i = 0;
        while ((i < voiceLimit) && (i < numSoundSources))
        {
            // Check if the source is inactive, but playing
            if ((!soundSources[i]->source->isActive()) && 
                (soundSources[i]->source->isPlaying()))
            {
                // Sanity check:  make sure we have a voice to give
                // the source (this should always be the case)
                if (numVoices > 0)
                {
                    // Assign an available voice to the source
                    numVoices--;
                    soundSources[i]->source->assignVoice(voices[numVoices]);
                }
            }

            // Move on to the next source
            i++;
        }
    }
}
