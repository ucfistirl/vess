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
    ALfloat zero[3];

    // Keep a handle to the vsSoundSample
    soundBuffer = buffer;

    // Remember whether we are looping or not
    loopSource = loop;

    // Remember whether we are streaming or not
    streamingSource = false;

    // Initialize class members
    offsetMatrix.setIdentity();
    parentComponent = NULL;
    lastPos.clear();
    lastOrn.clear();

    // Initialize the base direction vector (direction before coordinate
    // conversion)
    baseDirection.set(0.0, 0.0, 0.0);

    // Set up a coordinate conversion quaternion
    coordXform.setAxisAngleRotation(1, 0, 0, -90.0);
    coordXformInv = coordXform;
    coordXformInv.conjugate();

    // Create the OpenAL source
    alGenSources(1, &sourceID);

    // Set up a three-float buffer with zero values, used to initialize
    // the OpenAL source
    zero[0] = 0.0;
    zero[1] = 0.0;
    zero[2] = 0.0;

    // Initialize the OpenAL source 
    alSourcefv(sourceID, AL_POSITION, zero);
    alSourcefv(sourceID, AL_DIRECTION, zero);
    alSourcefv(sourceID, AL_VELOCITY, zero);
    alSourcei(sourceID, AL_BUFFER, 
        ((vsSoundSample *)soundBuffer)->getBufferID());

    if (loopSource)
        alSourcei(sourceID, AL_LOOPING, AL_TRUE);
    else
        alSourcei(sourceID, AL_LOOPING, AL_FALSE);

    // Start playing if this is a looping sound
    if (loopSource)
        alSourcePlay(sourceID);

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
    ALfloat zero[3];

    // Keep a handle to the vsSoundStream
    soundBuffer = buffer;

    // Remember whether we are streaming or not
    streamingSource = true;

    // Initialize class members
    offsetMatrix.setIdentity();
    parentComponent = NULL;
    lastPos.clear();
    lastOrn.clear();

    // Initialize the base direction vector (direction before coordinate
    // conversion)
    baseDirection.set(0.0, 0.0, 0.0);

    // Set up a coordinate conversion quaternion
    coordXform.setAxisAngleRotation(1, 0, 0, -90.0);
    coordXformInv = coordXform;
    coordXformInv.conjugate();

    // Create the OpenAL source
    alGenSources(1, &sourceID);
    ((vsSoundStream *)soundBuffer)->setSourceID(sourceID);

    // Set up a three-float buffer with zero values, used to initialize
    // the OpenAL source
    zero[0] = 0.0;
    zero[1] = 0.0;
    zero[2] = 0.0;

    // Initialize the OpenAL source
    alSourcefv(sourceID, AL_POSITION, zero);
    alSourcefv(sourceID, AL_DIRECTION, zero);
    alSourcefv(sourceID, AL_VELOCITY, zero);
    alSourcei(sourceID, AL_BUFFER, 0);

    // Register with the sound manager
    vsSoundManager::getInstance()->addSoundSource(this);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsSoundSourceAttribute::~vsSoundSourceAttribute()
{
    // Delete the OpenAL source
    alDeleteSources(1, &sourceID);

    // Unregister from the sound manager
    vsSoundManager::getInstance()->removeSoundSource(this);
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

    // Create a duplicate attribute
    source = new vsSoundSourceAttribute((vsSoundSample *)soundBuffer, 
        loopSource);

    // Attach it to the given node
    theNode->addAttribute(source);
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
    return VS_ATTRIBUTE_CATEGORY_OTHER;
}

// ------------------------------------------------------------------------
// Sets the offset matrix for this attribute. The offset matrix is
// multiplied into the overall transform matrix before it is sent to the 
// OpenAL sound source.
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setOffsetMatrix(vsMatrix newMatrix)
{
    offsetMatrix = newMatrix;
}

// ------------------------------------------------------------------------
// Retrieves the offset matrix for this attribute
// ------------------------------------------------------------------------
vsMatrix vsSoundSourceAttribute::getOffsetMatrix()
{
    return offsetMatrix;
}

// ------------------------------------------------------------------------
// Causes the attribute to calculate the total transform to its parent
// node, and assign that data to the associated alSource object
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::update()
{
    vsMatrix       result;
    vsQuat         tempQuat;
    vsVector       tempVec;
    vsVector       deltaVec;
    double         interval;
    int            buffersProcessed;
    ALuint         bufferID;

    // If we're not attached to a component, we have nothing to do
    if (!attachedCount)
        return;

    // Get the global transform for this attribute's component and
    // apply the source's offset matrix.  This lets us know where in
    // global space the sound source is.
    result = parentComponent->getGlobalXform();
    result = result * offsetMatrix;
    
    // Apply the VESS-to-OpenAL coordinate transformation
    tempVec[VS_X] = result[0][3];
    tempVec[VS_Y] = result[1][3];
    tempVec[VS_Z] = result[2][3];
    tempVec = coordXform.rotatePoint(tempVec);

    // Update the OpenAL source's position
    alSource3f(sourceID, AL_POSITION, (float)tempVec[VS_X],
        (float)tempVec[VS_Y], (float)tempVec[VS_Z]); 

    // Update the velocity (based on the last frame's position)
    deltaVec = tempVec - lastPos;
    interval = vsTimer::getSystemTimer()->getInterval();

    // Make sure time has passed to avoid dividing by zero
    if (interval > 0.0)
    {
        // Scale the position change by the inverse time interval to 
        // compute the velocity
        deltaVec.scale(1/interval);

        // Set the source's velocity
        alSource3f(sourceID, AL_VELOCITY, (float)deltaVec[VS_X],
            (float)deltaVec[VS_Y], (float)deltaVec[VS_Z]);
    }

    // Save the current position for next frame
    lastPos = tempVec;

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
        tempVec = tempQuat.rotatePoint(baseDirection);

        // Apply the direction to the OpenAL source
        alSource3f(sourceID, AL_DIRECTION, tempVec[VS_X], tempVec[VS_Y], 
            tempVec[VS_Z]);
    }

    // For streaming sources, check to see if we need to swap buffers
    // to allow the old buffer to be refilled
    if (streamingSource)
    {
        // Get the number of buffers processed
        alGetSourcei(sourceID, AL_BUFFERS_PROCESSED, &buffersProcessed);

        // Swap buffers if the front buffer is done
        if (buffersProcessed > 0)
        {
            // The current buffer is done, swap buffers
            bufferID = ((vsSoundStream *)soundBuffer)->getFrontBufferID();
            alSourceUnqueueBuffers(sourceID, 1, &bufferID);

            // NOTE:  no provisions are made in case the buffer swap
            // fails (i.e.: if there isn't any new data in the back buffer)
            // The user is responsible for making sure the buffers stay
            // filled and ready
            ((vsSoundStream *)soundBuffer)->swapBuffers();
        }
    }
}

// ------------------------------------------------------------------------
// Begins playback of the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::play()
{
    // Stop the source in case it's already playing
    alSourceStop(sourceID);

    // Start playing the source
    alSourcePlay(sourceID);
}

// ------------------------------------------------------------------------
// Halts playback of the source.  If a streaming source, also unqueues all
// queued buffers.
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::stop()
{
    // Stop the OpenAL source
    alSourceStop(sourceID);

    // If this is a streaming source, we have extra work to do
    if (streamingSource)
    {
        // Flush the data from the streaming buffers
        ((vsSoundStream *)soundBuffer)->flushBuffers();

        // Set the current buffer for this source to the zero (empty)
        // buffer
        alSourcei(sourceID, AL_BUFFER, 0);
    }
}

// ------------------------------------------------------------------------
// Pauses playback of the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::pause()
{
    // Pause the OpenAL source
    alSourcePause(sourceID);
}

// ------------------------------------------------------------------------
// Rewinds the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::rewind()
{
    // Rewind the OpenAL source
    alSourceRewind(sourceID);
}

// ------------------------------------------------------------------------
// Returns true if the source is currently playing
// ------------------------------------------------------------------------
bool vsSoundSourceAttribute::isPlaying()
{
    int srcState;

    // Get the state of the sound source from OpenAL
    alGetSourcei(sourceID, AL_SOURCE_STATE, &srcState);

    // Return true if playing, false otherwise
    if (srcState == AL_PLAYING)
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
    int srcState;

    // Get the state of the sound source from OpenAL
    alGetSourcei(sourceID, AL_SOURCE_STATE, &srcState);

    // Return true if stopped.  OpenAL also has an INITIAL state, which
    // is almost the same as STOPPED, but is slightly different (an
    // INITIAL source has never been played or has not been played since
    // the last call to alSourceRewind()).  We don't distinguish between
    // STOPPED and INITIAL.  Note that rewinding a PAUSED source will
    // make it INITIAL, thus VESS will then consider it STOPPED.
    if ((srcState == AL_STOPPED) || (srcState == AL_INITIAL))
        return true;
    else
        return false;
}

// ------------------------------------------------------------------------
// Returns true if the source is currently paused (not stopped)
// ------------------------------------------------------------------------
bool vsSoundSourceAttribute::isPaused()
{
    int srcState;

    // Get the state of the sound source from OpenAL
    alGetSourcei(sourceID, AL_SOURCE_STATE, &srcState);

    // Return true if paused, false otherwise
    if (srcState == AL_PAUSED)
        return true;
    else
        return false;
}

// ------------------------------------------------------------------------
// Returns whether or not this source is looping
// ------------------------------------------------------------------------
bool vsSoundSourceAttribute::isLooping()
{
    ALint looping;

    // Get the current looping state for the source
    alGetSourcei(sourceID, AL_LOOPING, &looping);

    // Return the looping state
    if ((int) looping == 0)
        return false;
    else
        return true;
}

// ------------------------------------------------------------------------
// Sets whether or not this source should loop
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setLooping(int looping)
{
    // Set the source's looping state to the given value
    alSourcei(sourceID, AL_LOOPING, looping);
}

// ------------------------------------------------------------------------
// Retrieve the source gain adjustment (range = [0.0, inf), default = 1.0)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getGain()
{
    float gain;

    // Get the source's gain from OpenAL
    alGetSourcefv(sourceID, AL_GAIN, &gain);

    // Return the gain value
    return (double)gain;
}

// ------------------------------------------------------------------------
// Adjust the source gain
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setGain(double gain)
{
    // Set the source's gain to the given value
    alSourcef(sourceID, AL_GAIN, (float)gain);
}

// ------------------------------------------------------------------------
// Returns the minimum gain of the source (range = [0.0, 1.0], 
// default = 0.0)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getMinGain()
{
    float gain;

    // Get the current minimum gain from OpenAL
    alGetSourcefv(sourceID, AL_MIN_GAIN, &gain);

    // Return the minimum gain
    return (double)gain;
}

// ------------------------------------------------------------------------
// Sets the minimum gain of the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setMinGain(double gain)
{
    // Set the minimum gain of the source to the given value
    alSourcef(sourceID, AL_MIN_GAIN, (float)gain);
}

// ------------------------------------------------------------------------
// Returns the maximum gain of the source (range = [0.0, 1.0], 
// default = 1.0)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getMaxGain()
{
    float gain;

    // Get the maximum gain of the source from OpenAL
    alGetSourcefv(sourceID, AL_MAX_GAIN, &gain);

    // Return the maximum gain
    return (double)gain;
}

// ------------------------------------------------------------------------
// Sets the maximum gain of the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setMaxGain(double gain)
{
    // Set the maximum gain of the source to the given value
    alSourcef(sourceID, AL_MIN_GAIN, (float)gain);
}

// ------------------------------------------------------------------------
// Returns the reference distance of the source (the distance at which 
// maximum gain is heard) (range = [0, inf), default = 1.0)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getReferenceDistance()
{
    float dist;

    // Get the current reference distance of the source from OpenAL
    alGetSourcefv(sourceID, AL_REFERENCE_DISTANCE, &dist);

    // Return the reference distance
    return (double)dist;
}

// ------------------------------------------------------------------------
// Sets the reference distance of the source 
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setReferenceDistance(double dist)
{
    // Set the reference distance of the source to the given value
    alSourcef(sourceID, AL_REFERENCE_DISTANCE, (float)dist);
}

// ------------------------------------------------------------------------
// Returns the maximum distance of the source.  A source beyond of this
// distance is either heard at minimum gain.  (range = [0.0, inf), 
// default = MAX_FLOAT)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getMaxDistance()
{
    float dist;

    // Get the maximum distance of the source from OpenAL
    alGetSourcefv(sourceID, AL_MAX_DISTANCE, &dist);

    // Return the maximum distance
    return (double)dist;
}

// ------------------------------------------------------------------------
// Sets the maximum distance of the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setMaxDistance(double dist)
{
    // Set the maximum distance of the source to the given value
    alSourcef(sourceID, AL_MAX_DISTANCE, (float)dist);
}

// ------------------------------------------------------------------------
// Returns the rolloff factor.  The rolloff factor determines how the 
// source is scaled over the range between REFERENCE_DISTANCE and 
// MAX_DISTANCE.  A value of zero means no distance attenuation.  See the 
// OpenAL specification for details.  (range = [0.0, inf), default = 1.0) 
// ------------------------------------------------------------------------ 
double vsSoundSourceAttribute::getRolloffFactor()
{
    float factor;

    // Get the current rolloff factor from OpenAL
    alGetSourcefv(sourceID, AL_ROLLOFF_FACTOR, &factor);

    // Return the rolloff factor
    return (double)factor;
}

// ------------------------------------------------------------------------
// Sets the rolloff factor of the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setRolloffFactor(double factor)
{
    // Set the rolloff factor of the source to the given value
    alSourcef(sourceID, AL_ROLLOFF_FACTOR, (float)factor);
}

// ------------------------------------------------------------------------
// Returns the current pitch shifting of the source.  Every 50% reduction
// or in pitch shift parameter results in the shifting of the source 
// sound's pitch by one octave (12 semitones).  (range = (0.0, 1.0],
// default = 1.0)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getPitchShift()
{
    float shift;

    // Get the current pitch shift factor from OpenAL
    alGetSourcefv(sourceID, AL_PITCH, &shift);

    // Return the pitch shift
    return (double)shift;
}

// ------------------------------------------------------------------------
// Sets the pitch shift of the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setPitchShift(double shift)
{
    // Set the pitch shift of the source to the given value
    alSourcef(sourceID, AL_PITCH, (float)shift);
}

// ------------------------------------------------------------------------
// Returns the source's base direction of sound radiation (in VESS 
// coordinates) (default = (0.0, 0.0, 0.0), i.e. non-directional)
// ------------------------------------------------------------------------
vsVector vsSoundSourceAttribute::getDirection()
{
    vsVector direction;

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
void vsSoundSourceAttribute::setDirection(vsVector direction)
{
    // We need to maintain the direction of the source locally, because
    // the direction set on the OpenAL source will also include any 
    // transformations that apply to the component where this attribute is 
    // attached.  The base direction is maintained in OpenAL coordinates
    // for ease of computation in the update() method.

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
    float angle;

    // Get the current inner cone angle of the source from OpenAL
    alGetSourcefv(sourceID, AL_CONE_INNER_ANGLE, &angle);

    // Return the inner cone angle
    return (double)angle;
}

// ------------------------------------------------------------------------
// Sets the directional source's inner cone angle
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setInnerConeAngle(double angle)
{
    // Set the inner cone angle of the source to the given value
    alSourcef(sourceID, AL_CONE_INNER_ANGLE, (float)angle);
}

// ------------------------------------------------------------------------
// Sets the angular width of a directional source's outer cone of 
// radiation.  The outer cone is the area in which the sound is still heard
// but at a reduced gain. (range = [0.0, inf), default = 360.0)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getOuterConeAngle()
{
    float angle;

    // Get the current outer cone angle of the source from OpenAL
    alGetSourcefv(sourceID, AL_CONE_OUTER_ANGLE, &angle);

    // Return the outer cone angle
    return (double)angle;
}

// ------------------------------------------------------------------------
// Sets the directional source's outer cone angle
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setOuterConeAngle(double angle)
{
    // Set the outer cone angle of the source to the given value
    alSourcef(sourceID, AL_CONE_OUTER_ANGLE, (float)angle);
}

// ------------------------------------------------------------------------
// Sets the gain reduction of a directional source's outer cone of 
// radiation.  A gain of 1.0 means no reduction (same as the inner cone)
// while a gain of 0.0 means no sound is heard at all in the outer cone.
// (range = [0.0, 1.0], default = 0.0)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getOuterConeGain()
{
    float gain;

    // Get the current outer cone gain of the source from OpenAL
    alGetSourcefv(sourceID, AL_CONE_OUTER_GAIN, &gain);

    // Return the outer cone gain
    return (double)gain;
}

// ------------------------------------------------------------------------
// Sets the directional source's outer cone gain
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setOuterConeGain(double gain)
{
    // Set the outer cone gain to the given value
    alSourcef(sourceID, AL_CONE_OUTER_GAIN, (float)gain);
}

// ------------------------------------------------------------------------
// Returns the corresponding base library object (an OpenAL source ID
// as an ALuint in this case)
// ------------------------------------------------------------------------
ALuint vsSoundSourceAttribute::getBaseLibraryObject()
{
    return sourceID;
}
