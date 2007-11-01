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
//    VESS Module:  vsSoundSourceAttribute.c++
//
//    Description:  Attribute to maintain the location/orientation of a
//                  source of sound in the VESS scene graph
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include <AL/al.h>
#include <stdio.h>

#ifdef __linux__
    #include <values.h>
#endif

#ifdef WIN32
    #include <float.h>
#endif

#include "vsSoundSourceAttribute.h++"
#include "vsSoundManager.h++"
#include "vsNode.h++"
#include "vsComponent.h++"
#include "vsTimer.h++"

// ------------------------------------------------------------------------
// Constructor -  creates a static sound source.  If the loop parameter
// is set, the sound will start playing immediately.  Otherwise, the 
// play() method can be called to "trigger" the sound.
// ------------------------------------------------------------------------
vsSoundSourceAttribute::vsSoundSourceAttribute(vsSoundSample *buffer, bool loop)
{
    // Keep a handle to the vsSoundSample
    soundBuffer = buffer;
    soundBuffer->ref();

    // Start with no OpenAL source assigned (signified by the -1)
    sourceID = 0;
    sourceValid = false;

    // Remember whether we are looping or not
    loopSource = loop;

    // Remember whether we are streaming or not
    streamingSource = false;

    // Create a timer to measure playback time
    playTimer = new vsTimer();

    // Initialize class members
    offsetMatrix.setIdentity();
    parentComponent = NULL;
    lastPos.set(0.0, 0.0, 0.0);
    lastDir.set(0.0, 0.0, 0.0);
    priority = VS_SSRC_PRIORITY_NORMAL;

    // Initialize all source parameters to their defaults
    gain = 1.0;
    minGain = 0.0;
    maxGain = 1.0;
    refDistance = 1.0;
    maxDistance = FLT_MAX;
    rolloffFactor = 1.0;
    pitch = 1.0;
    baseDirection.set(0.0, 0.0, 0.0);
    innerConeAngle = 360.0;
    outerConeAngle = 360.0;
    outerConeGain = 0.0;

    // Set up a coordinate conversion quaternion
    coordXform.setAxisAngleRotation(1, 0, 0, -90.0);
    coordXformInv = coordXform;
    coordXformInv.conjugate();

    // Start playing if this is a looping sound
    if (loopSource)
        playState = AL_PLAYING;
    else
        playState = AL_STOPPED;

    // Create a mutex to synchronize the source state between voice 
    // management, updates, and stream operations
    pthread_mutex_init(&sourceMutex, NULL);

    // Register with the sound manager
    vsSoundManager::getInstance()->addSoundSource(this);
}

// ------------------------------------------------------------------------
// Constructor -  creates a streaming sound source.  The play() method
// must be explicitly called to start playing because we cannot assume
// that the buffer contains valid data at the end of this constructor.
// ------------------------------------------------------------------------
vsSoundSourceAttribute::vsSoundSourceAttribute(vsSoundStream *buffer)
{
    // Keep a handle to the vsSoundStream
    soundBuffer = buffer;
    soundBuffer->ref();

    // Remember whether we are streaming or not
    streamingSource = true;

    // Assume that the stream has data available to start with
    outOfData = false;

    // Start with no OpenAL source assigned
    sourceID = 0;
    sourceValid = false;

    // Initialize class members
    offsetMatrix.setIdentity();
    parentComponent = NULL;
    lastPos.set(0.0, 0.0, 0.0);
    lastDir.set(0.0, 0.0, 0.0);

    // Create a timer to measure playback time
    playTimer = new vsTimer();

    // Initialize priority to normal
    priority = VS_SSRC_PRIORITY_NORMAL;

    // Initialize all source parameters to their defaults
    gain = 1.0;
    minGain = 0.0;
    maxGain = 1.0;
    refDistance = 1.0;
    maxDistance = FLT_MAX;
    rolloffFactor = 1.0;
    pitch = 1.0;
    baseDirection.set(0.0, 0.0, 0.0);
    innerConeAngle = 360.0;
    outerConeAngle = 360.0;
    outerConeGain = 0.0;
    loopSource = false;

    // Default to stopped (not playing)
    playState = AL_STOPPED;

    // Set up a coordinate conversion quaternion
    coordXform.setAxisAngleRotation(1, 0, 0, -90.0);
    coordXformInv = coordXform;
    coordXformInv.conjugate();

    // Create a mutex to synchronize the source state between voice 
    // management, updates, and stream operations
    pthread_mutex_init(&sourceMutex, NULL);

    // Register with the sound manager
    vsSoundManager::getInstance()->addSoundSource(this);
}

// ------------------------------------------------------------------------
// Constructor -  creates a packet streaming sound source.  The play()
// method must be explicitly called to start playing because we cannot
// assume that the buffer contains valid data at the end of this
// constructor.
// ------------------------------------------------------------------------
vsSoundSourceAttribute::vsSoundSourceAttribute(vsSoundPacketStream *buffer)
{
    // Keep a handle to the vsSoundPacketStream
    soundBuffer = buffer;
    soundBuffer->ref();

    // Remember whether we are streaming or not
    streamingSource = true;

    // Assume that the stream has no data available to start with
    outOfData = false;

    // Start with no OpenAL source assigned
    sourceID = 0;
    sourceValid = false;

    // Initialize class members
    offsetMatrix.setIdentity();
    parentComponent = NULL;
    lastPos.set(0.0, 0.0, 0.0);
    lastDir.set(0.0, 0.0, 0.0);

    // Create a timer to measure playback time
    playTimer = new vsTimer();

    // Initialize priority to always on (playback emulation is not yet
    // supported for this buffer type)
    priority = VS_SSRC_PRIORITY_ALWAYS_ON;

    // Initialize all source parameters to their defaults
    gain = 1.0;
    minGain = 0.0;
    maxGain = 1.0;
    refDistance = 1.0;
    maxDistance = FLT_MAX;
    rolloffFactor = 1.0;
    pitch = 1.0;
    baseDirection.set(0.0, 0.0, 0.0);
    innerConeAngle = 360.0;
    outerConeAngle = 360.0;
    outerConeGain = 0.0;
    loopSource = false;

    // Default to stopped (not playing)
    playState = AL_STOPPED;

    // Set up a coordinate conversion quaternion
    coordXform.setAxisAngleRotation(1, 0, 0, -90.0);
    coordXformInv = coordXform;
    coordXformInv.conjugate();

    // Create a mutex to synchronize the source state between voice 
    // management, updates, and stream operations
    pthread_mutex_init(&sourceMutex, NULL);

    // Register with the sound manager
    vsSoundManager::getInstance()->addSoundSource(this);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsSoundSourceAttribute::~vsSoundSourceAttribute()
{
    // Unregister from the sound manager, to prevent it from trying to
    // manipulate or update this sound source any longer
    vsSoundManager::getInstance()->removeSoundSource(this);

    // Unreference our sound buffer
    soundBuffer->unref();

    // Delete the playback timer
    delete playTimer;

    // Destroy the source's mutex
    pthread_mutex_destroy(&sourceMutex);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsSoundSourceAttribute::getClassName()
{
    return "vsSoundSourceAttribute";
}

// ------------------------------------------------------------------------
// VESS internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::attach(vsNode *theNode)
{
    // Make sure the attribute isn't attached elsewhere
    if (attachedCount)
    {
        printf("vsSoundSourceAttribute::attach: Attribute is already "
            "attached\n");
        return;
    }

    if (theNode->getNodeType() != VS_NODE_TYPE_COMPONENT)
    {
        printf("vsSoundSourceAttribute::attach: Can only attach sound source "
            "attributes to vsComponents\n");
        return;
    }

    // Attach to the given component
    parentComponent = ((vsComponent *)theNode);

    // Flag this attribute as attached to a component
    attachedCount = 1;
}

// ------------------------------------------------------------------------
// VESS internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::detach(vsNode *theNode)
{
    // Make sure the attribute is actually attached to the node
    if (!attachedCount)
    {
        printf("vsSoundSourceAttribute::detach: Attribute is not attached\n");
        return;
    }

    // Detach from the node
    parentComponent = NULL;

    // Flag this attribute as not attached to a component
    attachedCount = 0;
}

// ------------------------------------------------------------------------
// VESS internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::attachDuplicate(vsNode *theNode)
{
    vsSoundSourceAttribute *source;

    // If this is a streaming source, we can't duplicate it.  The user
    // must duplicate the attribute and the vsSoundStream manually.  A
    // vsSoundStream can only be attached to one source (for synchronization
    // reasons).
    if (streamingSource)
    {
        printf("vsSoundSourceAttribute::attachDuplicate:\n");
        printf("    Cannot automatically duplicate streaming sound source "
            "attributes!\n");
        return;
    }

    // Create a duplicate attribute
    source = new vsSoundSourceAttribute((vsSoundSample *)soundBuffer, 
        loopSource);

    // Attach it to the given node
    theNode->addAttribute(source);
}

// ------------------------------------------------------------------------
// VESS internal function
// Returns the active state of this source.  A source is active if it has
// been given a valid OpenAL source ID by the sound manager
// ------------------------------------------------------------------------
bool vsSoundSourceAttribute::isActive()
{
    return sourceValid;
}

// ------------------------------------------------------------------------
// VESS internal function
// Assigns the source attribute the given OpenAL source ID (hardware voice)
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::assignVoice(int voiceID)
{
    // Remember the voice ID
    sourceID = voiceID;
    sourceValid = true;

    // Attach the sound sample or stream to the OpenAL source
    if (soundBuffer->getBufferType() == VS_SOUND_BUFFER_STREAM)
        ((vsSoundStream *)soundBuffer)->assignSource(sourceID);
    else if (soundBuffer->getBufferType() == VS_SOUND_BUFFER_PACKET_STREAM)
        ((vsSoundPacketStream *)soundBuffer)->assignSource(sourceID);
    else
        alSourcei(sourceID, AL_BUFFER, 
            ((vsSoundSample *)soundBuffer)->getBufferID());

    // Set all the source parameters
    alSourcef(sourceID, AL_GAIN, (ALfloat)gain);
    alSourcef(sourceID, AL_MIN_GAIN, (ALfloat)minGain);
    alSourcef(sourceID, AL_MAX_GAIN, (ALfloat)maxGain);
    alSourcef(sourceID, AL_REFERENCE_DISTANCE, (ALfloat)refDistance);
    alSourcef(sourceID, AL_MAX_DISTANCE, (ALfloat)maxDistance);
    alSourcef(sourceID, AL_ROLLOFF_FACTOR, (ALfloat)rolloffFactor);
    alSourcef(sourceID, AL_PITCH, (ALfloat)pitch);
    alSourcef(sourceID, AL_CONE_INNER_ANGLE, (ALfloat)innerConeAngle);
    alSourcef(sourceID, AL_CONE_OUTER_ANGLE, (ALfloat)outerConeAngle);
    alSourcef(sourceID, AL_CONE_OUTER_GAIN, (ALfloat)outerConeGain);
    alSourcei(sourceID, AL_LOOPING, (ALuint)loopSource);
    alSource3f(sourceID, AL_POSITION, (ALfloat)lastPos[AT_X],
        (ALfloat)lastPos[AT_Y], (ALfloat)lastPos[AT_Z]);
    alSource3f(sourceID, AL_DIRECTION, (ALfloat)lastDir[AT_X],
        (ALfloat)lastDir[AT_Y], (ALfloat)lastDir[AT_Z]);

    // If the source should be playing, start playing now.  With OpenAL
    // 1.1, we'll be able to start playback at the right time (using
    // the current time in the playTimer()); however, the 1.1 implementation
    // isn't ready yet.
    if (playState == AL_PLAYING)
        play();
}

// ------------------------------------------------------------------------
// VESS internal function
// Revoke's the source's OpenAL source ID (voice), making the source
// inactive
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::revokeVoice()
{
    // Stop the OpenAL source (this function isn't called unless the sourceID
    // is valid, so we don't need to check it)
    alSourceStop(sourceID);

    // If streaming, detach the stream from the OpenAL source, but keep
    // the data in the stream intact, so we can emulate playback while
    // the source is swapped out.
    if (soundBuffer->getBufferType() == VS_SOUND_BUFFER_STREAM)
        ((vsSoundStream *)soundBuffer)->revokeSource();
    else if (soundBuffer->getBufferType() == VS_SOUND_BUFFER_PACKET_STREAM)
        ((vsSoundPacketStream *)soundBuffer)->revokeSource();

    // Flush the source's buffer queue
    alSourcei(sourceID, AL_BUFFER, 0);

    // Invalidate the sourceID
    sourceID = 0;
    sourceValid = false;
}

// ------------------------------------------------------------------------
// VESS internal function
// Return the current OpenAL source ID (voice) for this source
// ------------------------------------------------------------------------
int vsSoundSourceAttribute::getVoiceID()
{
    return sourceID;
}

// ------------------------------------------------------------------------
// Return the effective gain for this source, computed in the last call
// to update()
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getEffectiveGain(atVector listenerPos)
{
    double distance;
    double distScale;
    double effectiveGain;
    atVector listenerDir;
    double angle;
    double proportion;

    // Calculate the distance to the listener
    distance = (lastPos - listenerPos).getMagnitude();

    // Apply the inverse distance attenuation formula (see OpenAL 1.0 spec)
    distScale = (distance - refDistance) / refDistance;
    effectiveGain = gain / (1.0 + rolloffFactor * distScale);

    // Apply additional attenuation due to directionality (if any)
    if ((baseDirection.getMagnitude() > 1.0E-6) && 
        (innerConeAngle < 360.0) &&
        (outerConeGain < gain))
    {
        // Get the direction from the source to the listener
        listenerDir = listenerPos - lastPos;
        listenerDir.normalize();

        // Compute the angle between the source's direction and the direction
        // to the listener
        angle = lastDir.getAngleBetween(listenerDir);

        // Apply an additional attenuation if the angle is greater than
        // the inner cone angle
        if (angle > innerConeAngle)
        {
            // If the angle is greater than the outer cone angle, apply
            // the outer cone gain
            if (angle > outerConeAngle)
            {
                effectiveGain *= outerConeGain;
            }
            else
            {
                // Interpolate the attenuation between the inner and outer
                // cone gains
                proportion = (angle - innerConeAngle) / 
                    (outerConeAngle - innerConeAngle);
                effectiveGain *= 
                    ((1 - proportion) * gain) + (proportion * outerConeGain);
            }
        }
    }
    
    // Clamp the effective gain to [minGain, maxGain]
    if (effectiveGain < minGain)
        effectiveGain = minGain;
    if (effectiveGain > maxGain)
        effectiveGain = maxGain;

    return effectiveGain;
}

// ------------------------------------------------------------------------
// Return the last computed position of the source
// ------------------------------------------------------------------------
atVector vsSoundSourceAttribute::getLastPosition()
{
    return lastPos;
}

// ------------------------------------------------------------------------
// Locks this source's mutual exclusion object.  This should only be 
// called by this object or the vsSoundManager.
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::lockSource()
{
    pthread_mutex_lock(&sourceMutex);
}

// ------------------------------------------------------------------------
// Unlocks this source's mutual exclusion object.  This should only be 
// called by this object or the vsSoundManager.
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::unlockSource()
{
    pthread_mutex_unlock(&sourceMutex);
}

// ------------------------------------------------------------------------
// VESS internal function
// Handles the necessary data movement for sound streams.  If a streaming
// source is swapped out, this will try to emulate playback and keep 
// consuming audio as normal.  This function also implicitly updates the
// source's playback state, so the updatePlayState() function need not be
// called for streaming sources.  This function should only be called by 
// the vsSoundManager's worker thread.
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::updateStream()
{
    int            buffersProcessed;
    ALuint         bufferID;
    int            state, queued;

    // Lock the source to keep it from being swapped in or out by voice
    // management during the running of this function
    lockSource();

    // Check to see if the source is active or swapped out, also making sure
    // that OpenAL considers the source ID valid
    if ((sourceValid) && (alIsSource(sourceID)))
    {
        // See what kind of stream this is
        if (soundBuffer->getBufferType() == VS_SOUND_BUFFER_STREAM)
        {
            // Get the number of buffers processed
            alGetSourcei(sourceID, AL_BUFFERS_PROCESSED, &buffersProcessed);

            // Swap buffers if the front buffer is done
            if (buffersProcessed > 0)
            {
                // The current buffer is done, swap buffers.  NOTE:
                // The user is responsible for making sure the buffers stay
                // filled and ready
                bufferID = ((vsSoundStream *)soundBuffer)->getFrontBufferID();
                alSourceUnqueueBuffers(sourceID, 1, &bufferID);
                ((vsSoundStream *)soundBuffer)->swapBuffers();
            }
        }
        else if (soundBuffer->getBufferType() == VS_SOUND_BUFFER_PACKET_STREAM)
        {
            // Just update the stream
            ((vsSoundPacketStream *)soundBuffer)->update();
        }

        // Compare the vsSourceAttribute's play state with the real
        // OpenAL source's state.  If the source should be playing but 
        // isn't, try to start playing again.
        alGetSourcei(sourceID, AL_SOURCE_STATE, &state);
        alGetSourcei(sourceID, AL_BUFFERS_QUEUED, &queued);
        if ((playState == AL_PLAYING) && (state != AL_PLAYING) && 
            (queued > 0))
        {
             // We have at least one buffer queued, so start playing
             // again.
             play();
        }
    }
    else
    {
        // Playback emulation is not yet supported on packet streams, so
        // skip this part if this is a packet stream
        if (soundBuffer->getBufferType() == VS_SOUND_BUFFER_PACKET_STREAM)
        {
            // Unlock the source mutex and bail
            unlockSource();

            return;
        }

        // The source is swapped out, so we need to emulate playback and
        // still consume audio.  Check the play time of the source 
        // and see if it has exceeded the length of the buffer.
        if ((playState == AL_PLAYING) && 
            (playTimer->getElapsed() > soundBuffer->getLength()))
        {
            // See if the stream had previously run out of data, and
            // try to recover
            if (outOfData) 
            {
                // See if the stream now has data available
                if (!((vsSoundStream *)soundBuffer)->isEmpty())
                {
                    // Mark the playTimer immediately, to emulate the 
                    // resuming of playback
                    playTimer->mark();

                    // Reset the out of data flag
                    outOfData = false;
                }
            }
            else
            {
                // Swap the stream buffers and check the result
                if (!((vsSoundStream *)soundBuffer)->swapBuffers())
                {
                    // The swapBuffers() call failed, so the stream is out 
                    // of data.  We'll try to recover from this on 
                    // subsequent updates
                    outOfData = true;
                }
                else
                {
                    // If we still have data, mark the playTimer at the
                    // point where the last stream buffer finished.  This
                    // keeps the buffer consumption timing accurate.
                    if (!outOfData)
                        playTimer->markAtInterval(soundBuffer->getLength());
                }
            }
        }
    }

    // Unlock the source mutex
    unlockSource();
}

// ------------------------------------------------------------------------
// VESS internal function
// Checks the state of the OpenAL source to see if it has changed, and
// updates the VESS internal play state appropriately.  This function 
// should only be called by the vsSoundManager's worker thread.
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::updatePlayState()
{
    ALint state;

    // Lock the source to keep it from being swapped in or out by voice
    // management during the running of this function
    lockSource();

    // If the source is valid and isn't streaming, check the play state of 
    // the OpenAL source and see if it has stopped.  Update the 
    // attribute's play state if so.  If it is a looping source, it 
    // should always be considered playing, unless stopped by the 
    // user.
    if ((sourceValid) && (alIsSource(sourceID)))
    {
        alGetSourcei(sourceID, AL_SOURCE_STATE, &state);
        if ((playState == AL_PLAYING) && (state != AL_PLAYING))
        {
            // Update the play state
            playState = AL_STOPPED;
        }
    }
    else 
    {
        // The source is swapped out.  Check the playback timer to see if
        // the buffer length has been exceeded.
        if ((playState == AL_PLAYING) && 
            (playTimer->getElapsed() > soundBuffer->getLength()))
        {
            // The buffer has finished playing.  However, we don't want to
            // change the play state if this is a looping source (they 
            // never stop unless stopped explicitly)
            if (!loopSource)
            {
                // The sound is done playing, so indicate the source is
                // stopped
                playState = AL_STOPPED;
            }
        }
    }

    // Unlock the source mutex
    unlockSource();
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsSoundSourceAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_SOUND_SOURCE;
}

// ------------------------------------------------------------------------
// Retrieves the category of this attribute
// ------------------------------------------------------------------------
int vsSoundSourceAttribute::getAttributeCategory()
{
    return VS_ATTRIBUTE_CATEGORY_CONTAINER;
}

// ------------------------------------------------------------------------
// Returns the sound buffer passed into the constructor
// ------------------------------------------------------------------------
vsSoundBuffer *vsSoundSourceAttribute::getSoundBuffer()
{
    return soundBuffer;
}

// ------------------------------------------------------------------------
// Sets the offset matrix for this attribute. The offset matrix is
// multiplied into the overall transform matrix before it is sent to the 
// OpenAL sound source.
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setOffsetMatrix(atMatrix newMatrix)
{
    offsetMatrix = newMatrix;
}

// ------------------------------------------------------------------------
// Retrieves the offset matrix for this attribute
// ------------------------------------------------------------------------
atMatrix vsSoundSourceAttribute::getOffsetMatrix()
{
    return offsetMatrix;
}

// ------------------------------------------------------------------------
// Causes the attribute to calculate the total transform to its parent
// node, and assign that data to the associated alSource object
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::update()
{
    atMatrix       result;
    atQuat         tempQuat;
    atVector       posVec;
    atVector       deltaVec;
    atVector       dirVec;
    double         interval;

    // If we're not attached to a component, we have nothing to do
    if (!attachedCount)
        return;

    // Get the global transform for this attribute's component and
    // apply the source's offset matrix.  This lets us know where in
    // global space the sound source is.
    result = parentComponent->getGlobalXform();
    result = result * offsetMatrix;
    
    // Apply the VESS-to-OpenAL coordinate transformation
    posVec.setSize(3);
    result.getTranslation(&posVec[AT_X], &posVec[AT_Y], &posVec[AT_Z]);
    posVec = coordXform.rotatePoint(posVec);

    // Update the velocity (based on the last frame's position)
    deltaVec.setSize(3);
    deltaVec = posVec - lastPos;
    interval = vsTimer::getSystemTimer()->getInterval();

    // Make sure time has passed to avoid dividing by zero
    if (interval > 0.0)
    {
        // Scale the position change by the inverse time interval to 
        // compute the velocity
        deltaVec.scale(1.0/interval);
    }
    else
    {
        // Assume zero for the delta (no velocity)
        deltaVec.clear();
    }

    // Save the current position for next frame
    lastPos = posVec;

    // Update the orientation.  Check the base direction to see if it
    // has a valid direction.
    if (baseDirection.getMagnitude() > 0.0)
    {
        // Base direction is valid, so we need to convert from VESS to
        // OpenAL orientation.  Set up a quaternion with the global
        // transform.
        tempQuat.setMatrixRotation(result);

        // Convert to OpenAL orientation
        tempQuat = coordXformInv * tempQuat * coordXform;

        // Rotate the base direction vector by the converted global transform
        dirVec = tempQuat.rotatePoint(baseDirection);

        // Remember the direction, in case the sound manager needs it
        lastDir = dirVec;
    }
    else
    {
        // Clear the vector that holds the latest direction (this indicates
        // that the source is not directional)
        lastDir.clear();
    }

    // Make sure we have a valid OpenAL source ID before trying to update it
    if ((sourceValid) && (alIsSource(sourceID)))
    {
        // Update the OpenAL source's position
        alSource3f(sourceID, AL_POSITION, (float)posVec[AT_X],
            (float)posVec[AT_Y], (float)posVec[AT_Z]); 

        // Set the source's velocity
        alSource3f(sourceID, AL_VELOCITY, (float)deltaVec[AT_X],
            (float)deltaVec[AT_Y], (float)deltaVec[AT_Z]);

        // Apply the direction to the OpenAL source
        alSource3f(sourceID, AL_DIRECTION, dirVec[AT_X], dirVec[AT_Y], 
            dirVec[AT_Z]);
    }

    // Check the validity of the OpenAL source
    if (sourceValid && !alIsSource(sourceID))
    {
        // Complain that VESS thinks it has a valid source, but OpenAL 
        // disagrees
        printf("vsSoundSourceAttribute::update:\n");
        printf("    Source is active, but has an invalid source ID (%d)!\n", 
            sourceID);
    }
}

// ------------------------------------------------------------------------
// Begins playback of the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::play()
{
    // Set the play state to playing
    playState = AL_PLAYING;

    // Mark the playback timer, so we can stop the source at the right
    // time when it gets swapped out
    playTimer->mark();

    // Make sure we have a source
    if (sourceValid)
    {
        // Stop the source in case it's already playing
        alSourceStop(sourceID);

        // Start playing the source
        alSourcePlay(sourceID);
    }
}

// ------------------------------------------------------------------------
// Halts playback of the source.  If a streaming source, also unqueues all
// queued buffers.
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::stop()
{
    // Set the play state to playing
    playState = AL_STOPPED;

    // Make sure we have a source
    if (sourceValid)
    {
        // Stop the OpenAL source
        alSourceStop(sourceID);

        // If this is a streaming source, we have extra work to do
        if (streamingSource)
        {
            // Flush the data from the streaming buffers
            if (soundBuffer->getBufferType() == VS_SOUND_BUFFER_STREAM)
                ((vsSoundStream *)soundBuffer)->flushBuffers();
            else if (soundBuffer->getBufferType() == 
                         VS_SOUND_BUFFER_PACKET_STREAM)
                ((vsSoundPacketStream *)soundBuffer)->flushBuffers();

            // Set the current buffer for this source to the zero (empty)
            // buffer
            alSourcei(sourceID, AL_BUFFER, 0);
        }
    }
}

// ------------------------------------------------------------------------
// Pauses playback of the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::pause()
{
    // Set the play state to playing
    playState = AL_PAUSED;

    // Make sure we have a source
    if (sourceValid)
    {
        // Pause the OpenAL source
        alSourcePause(sourceID);
    }
}

// ------------------------------------------------------------------------
// Rewinds the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::rewind()
{
    // Make sure we have a source
    if (sourceValid)
    {
        // Rewind the OpenAL source
        alSourceRewind(sourceID);
    }
}

// ------------------------------------------------------------------------
// Returns true if the source is currently playing
// ------------------------------------------------------------------------
bool vsSoundSourceAttribute::isPlaying()
{
    // Return true if playing, false otherwise
    if (playState == AL_PLAYING)
        return true;
    else
        return false;
}

// ------------------------------------------------------------------------
// Returns true if the source is currently stopped (or if it has been
// paused and then rewound)
// ------------------------------------------------------------------------
bool vsSoundSourceAttribute::isStopped()
{
    // Return true if stopped
    if (playState == AL_STOPPED)
        return true;
    else
        return false;
}

// ------------------------------------------------------------------------
// Returns true if the source is currently paused (not stopped)
// ------------------------------------------------------------------------
bool vsSoundSourceAttribute::isPaused()
{
    // Return true if paused, false otherwise
    if (playState == AL_PAUSED)
        return true;
    else
        return false;
}

// ------------------------------------------------------------------------
// Returns whether or not this source is looping
// ------------------------------------------------------------------------
bool vsSoundSourceAttribute::isLooping()
{
    // Return the looping state
    return loopSource;
}

// ------------------------------------------------------------------------
// Sets whether or not this source should loop
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setLooping(bool looping)
{
    // Change the looping state
    loopSource = looping;

    // Make sure we have a valid source ID
    if (sourceValid)
    {
        // Set the source's looping state to the given value
        alSourcei(sourceID, AL_LOOPING, looping);
    }
}

// ------------------------------------------------------------------------
// Returns whether or not this source is attached to a sound stream
// ------------------------------------------------------------------------
bool vsSoundSourceAttribute::isStreaming()
{
    return streamingSource;
}

// ------------------------------------------------------------------------
// Retrieve the source gain adjustment (range = [0.0, inf), default = 1.0)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getGain()
{
    // Return the gain value
    return gain;
}

// ------------------------------------------------------------------------
// Adjust the source gain
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setGain(double newGain)
{
    // Validate the new value
    if ((newGain < 0.0) || (newGain > 1.0))
    {
        printf("vsSoundSourceAttribute::setGain:  Value %0.2lf out of range\n",
            newGain);
    }

    // Set the new gain value
    gain = newGain;

    // Update the OpenAL source, if we have one
    if (sourceValid)
    {
        alSourcef(sourceID, AL_GAIN, (float)gain);
    }
}

// ------------------------------------------------------------------------
// Returns the minimum gain of the source (range = [0.0, 1.0], 
// default = 0.0)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getMinGain()
{
    // Return the minimum gain
    return minGain;
}

// ------------------------------------------------------------------------
// Sets the minimum gain of the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setMinGain(double newGain)
{
    // Validate the new value
    if ((newGain < 0.0) || (newGain > 1.0))
    {
        printf("vsSoundSourceAttribute::setMinGain:  "
            "Value %0.2lf out of range\n", newGain);
    }

    // Set the new minimum gain value
    minGain = newGain;

    // Update the OpenAL source, if we have one
    if (sourceValid)
    {
        alSourcef(sourceID, AL_MIN_GAIN, (float)minGain);
    }
}

// ------------------------------------------------------------------------
// Returns the maximum gain of the source (range = [0.0, 1.0], 
// default = 1.0)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getMaxGain()
{
    // Return the maximum gain
    return maxGain;
}

// ------------------------------------------------------------------------
// Sets the maximum gain of the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setMaxGain(double newGain)
{
    // Validate the new value
    if ((newGain < 0.0) || (newGain > 1.0))
    {
        printf("vsSoundSourceAttribute::setMaxGain:  "
            "Value %0.2lf out of range\n", newGain);
    }

    // Set the new gain value
    maxGain = newGain;

    // Update the OpenAL source, if we have one
    if (sourceValid)
    {
        alSourcef(sourceID, AL_MAX_GAIN, (float)maxGain);
    }
}

// ------------------------------------------------------------------------
// Returns the reference distance of the source (the distance at which 
// maximum gain is heard) (range = [0, inf), default = 1.0)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getReferenceDistance()
{
    // Return the reference distance
    return refDistance;
}

// ------------------------------------------------------------------------
// Sets the reference distance of the source 
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setReferenceDistance(double dist)
{
    // Validate the new value
    if (dist < 0.0)
    {
        printf("vsSoundSourceAttribute::setReferenceDistance:  "
            "Value %0.2lf out of range\n", dist);
    }

    // Set the new reference distance
    refDistance = dist;

    // Update the OpenAL source, if we have one
    if (sourceValid)
    {
        alSourcef(sourceID, AL_REFERENCE_DISTANCE, (float)refDistance);
    }
}

// ------------------------------------------------------------------------
// Returns the maximum distance of the source.  A source beyond this
// distance is heard at minimum gain.  (range = [0.0, inf), 
// default = MAX_FLOAT)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getMaxDistance()
{
    // Return the maximum distance
    return maxDistance;
}

// ------------------------------------------------------------------------
// Sets the maximum distance of the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setMaxDistance(double dist)
{
    // Validate the new value
    if (dist < 0.0)
    {
        printf("vsSoundSourceAttribute::setMaxDistance:  "
            "Value %0.2lf out of range\n", dist);
    }

    // Set the new gain value
    maxDistance = dist;

    // Update the OpenAL source, if we have one
    if (sourceValid)
    {
        alSourcef(sourceID, AL_MAX_DISTANCE, (float)maxDistance);
    }
}

// ------------------------------------------------------------------------
// Returns the rolloff factor.  The rolloff factor determines how the 
// source is scaled over the range between REFERENCE_DISTANCE and 
// MAX_DISTANCE.  A value of zero means no distance attenuation.  See the 
// OpenAL specification for details.  (range = [0.0, inf), default = 1.0) 
// ------------------------------------------------------------------------ 
double vsSoundSourceAttribute::getRolloffFactor()
{
    // Return the rolloff factor
    return rolloffFactor;
}

// ------------------------------------------------------------------------
// Sets the rolloff factor of the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setRolloffFactor(double factor)
{
    // Validate the new value
    if (factor < 0.0)
    {
        printf("vsSoundSourceAttribute::setRolloffFactor:  "
            "Value %0.2lf out of range\n", factor);
    }

    // Set the new gain value
    rolloffFactor = factor;

    // Update the OpenAL source, if we have one
    if (sourceValid)
    {
        alSourcef(sourceID, AL_ROLLOFF_FACTOR, (float)rolloffFactor);
    }
}

// ------------------------------------------------------------------------
// Returns the current pitch shifting of the source.  Every 50% reduction
// in pitch shift parameter results in the shifting of the source sound's 
// pitch by one octave (12 semitones).  (range = (0.0, 1.0], default = 1.0)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getPitchShift()
{
    return pitch;
}

// ------------------------------------------------------------------------
// Sets the pitch shift of the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setPitchShift(double shift)
{
    // Validate the new value
    if ((shift <= 0.0) || (shift > 1.0))
    {
        printf("vsSoundSourceAttribute::setPitchShift:  "
            "Value %0.2lf out of range\n", shift);
    }

    // Set the new gain value
    pitch = shift;

    // Update the OpenAL source, if we have one
    if (sourceValid)
    {
        alSourcef(sourceID, AL_PITCH, (float)pitch);
    }
}

// ------------------------------------------------------------------------
// Returns the source's base direction of sound radiation (in VESS 
// coordinates) (default = (0.0, 0.0, 0.0), i.e. non-directional)
// ------------------------------------------------------------------------
atVector vsSoundSourceAttribute::getDirection()
{
    atVector direction;

    // Initialize the direction vector with the current base direction
    // (see the setDirection() method below for more about the locally-
    // maintained base direction).
    direction = baseDirection;

    // Normalize the base direction and convert to VESS coordinates
    if (direction.getMagnitude() != 0.0)
    {
        direction.normalize();
        direction = coordXformInv.rotatePoint(direction);
    }

    // Return the converted direction 
    return direction;
}

// ------------------------------------------------------------------------
// Sets the source's base direction of sound radiation
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setDirection(atVector direction)
{
    // The direction set on the OpenAL source will also include any 
    // transformations that apply to the component where this attribute is 
    // attached.  However, the base direction, (the direction of the source
    // in it's local coordinate system) is kept locally.  It base 
    // direction is maintained in OpenAL coordinates for ease of 
    // computation in the update() method.

    // Normalize and convert to OpenAL coordinates
    if (direction.getMagnitude() != 0.0)
    {
        direction.normalize();
        direction = coordXform.rotatePoint(direction);
    }

    // Set the base direction vector to the given direction
    baseDirection = direction;
}

// ------------------------------------------------------------------------
// Sets the angular width of a directional source's inner cone of 
// radiation.  The inner cone is the area in which the normal volume of
// sound is heard. (range = [0.0, inf), default = 360.0)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getInnerConeAngle()
{
    // Return the inner cone angle
    return innerConeAngle;
}

// ------------------------------------------------------------------------
// Sets the directional source's inner cone angle
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setInnerConeAngle(double angle)
{
    // Validate the new value
    if (angle < 0.0)
    {
        printf("vsSoundSourceAttribute::setInnerConeAngle:  "
            "Value %0.2lf out of range\n", angle);
    }

    // Set the new gain value
    innerConeAngle = angle;

    // Update the OpenAL source, if we have one
    if (sourceValid)
    {
        alSourcef(sourceID, AL_CONE_INNER_ANGLE, (float)innerConeAngle);
    }
}

// ------------------------------------------------------------------------
// Sets the angular width of a directional source's outer cone of 
// radiation.  The outer cone is the area in which the sound is still heard
// but at a reduced gain. (range = [0.0, inf), default = 360.0)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getOuterConeAngle()
{
    // Return the outer cone angle
    return outerConeAngle;
}

// ------------------------------------------------------------------------
// Sets the directional source's outer cone angle
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setOuterConeAngle(double angle)
{
    // Validate the new value
    if (angle < 0.0)
    {
        printf("vsSoundSourceAttribute::setOuterConeAngle:  "
            "Value %0.2lf out of range\n", angle);
    }

    // Set the new gain value
    outerConeAngle = angle;

    // Update the OpenAL source, if we have one
    if (sourceValid)
    {
        alSourcef(sourceID, AL_CONE_OUTER_ANGLE, (float)outerConeAngle);
    }
}

// ------------------------------------------------------------------------
// Sets the gain reduction of a directional source's outer cone of 
// radiation.  A gain of 1.0 means no reduction (same as the inner cone)
// while a gain of 0.0 means no sound is heard at all in the outer cone.
// (range = [0.0, 1.0], default = 0.0)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getOuterConeGain()
{
    // Return the outer cone gain
    return outerConeGain;
}

// ------------------------------------------------------------------------
// Sets the directional source's outer cone gain
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setOuterConeGain(double newGain)
{
    // Validate the new value
    if ((newGain < 0.0) || (newGain > 1.0))
    {
        printf("vsSoundSourceAttribute::setOuterConeGain:  "
            "Value %0.2lf out of range\n", newGain);
    }

    // Set the new gain value
    outerConeGain = newGain;

    // Update the OpenAL source, if we have one
    if (sourceValid)
    {
        alSourcef(sourceID, AL_CONE_OUTER_GAIN, (float)outerConeGain);
    }
}

// ------------------------------------------------------------------------
// Change the priority of this sound source.  When a large number of sound
// sources are used, some sources may be stopped to conserve hardware
// resources.  Higher-priority sounds remain playing longer than lower-
// priority ones.
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setPriority(int newPriority)
{
    priority = newPriority;
}

// ------------------------------------------------------------------------
// Return the current priority of this sound source
// ------------------------------------------------------------------------
int vsSoundSourceAttribute::getPriority()
{
    return priority;
}

// ------------------------------------------------------------------------
// Returns the corresponding base library object (an OpenAL source ID
// as an ALuint in this case)
// ------------------------------------------------------------------------
ALuint vsSoundSourceAttribute::getBaseLibraryObject()
{
    return sourceID;
}
