#include <AL/al.h>
#include <AL/alkludge.h>
#include <sys/time.h>
#include <stdio.h>

#include "vsSoundSourceAttribute.h++"
#include "vsNode.h++"
#include "vsComponent.h++"

// ------------------------------------------------------------------------
// Constructor -  creates a static sound source.  If the loop parameter
// is set, the sound will start playing immediately.  Otherwise, the 
// play() method can be called to "trigger" the sound.
// ------------------------------------------------------------------------
vsSoundSourceAttribute::vsSoundSourceAttribute(vsSoundSample *buffer, int loop)
{
    ALfloat zero[3];

    // Keep a handle to the vsSoundSample
    soundBuffer = buffer;

    // Remember whether we are looping or not
    loopSource = loop;

    // Remember whether we are streaming or not
    streamingSource = VS_FALSE;

    // Do some initialization
    offsetMatrix.setIdentity();
    parentComponent = NULL;

    zero[0] = 0.0;
    zero[1] = 0.0;
    zero[2] = 0.0;

    lastPos.clear();
    lastOrn.clear();

    // Call getTimeInterval to initialize the lastTime variable
    getTimeInterval();

    // Initialize the base direction vector
    baseDirection.set(0.0, 0.0, 0.0);

    // Set up a coordinate conversion quaternion
    coordXform.setAxisAngleRotation(1, 0, 0, -90.0);
    coordXformInv = coordXform;
    coordXformInv.conjugate();

    // Create the OpenAL source
    alGenSources(1, &sourceID);

    // Configure
    alSourcefv(sourceID, AL_POSITION, zero);
    alSourcefv(sourceID, AL_DIRECTION, zero);
    alSourcefv(sourceID, AL_VELOCITY, zero);
    alSourcei(sourceID, AL_BUFFER, 
        ((vsSoundSample *)soundBuffer)->getBufferID());
    alSourcei(sourceID, AL_LOOPING, loopSource);

    // Start playing if this is a looping sound
    if (loopSource)
        alSourcePlay(sourceID);
}

/*  
   Not available yet.  The details of streaming sounds are still 
   being worked out in the OpenAL world

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
    streamingSource = VS_TRUE;

    // Do some initialization
    offsetMatrix.setIdentity();
    parentComponent = NULL;

    zero[0] = 0.0;
    zero[1] = 0.0;
    zero[2] = 0.0;

    lastPos.clear();
    lastOrn.clear();

    // Call getTimeInterval to initialize the lastTime variable
    getTimeInterval();

    // Initialize the base direction vector
    baseDirection.set(0.0, 0.0, 0.0);

    // Set up a coordinate conversion quaternion
    coordXform.setAxisAngleRotation(1, 0, 0, -90.0);
    coordXformInv = coordXform;
    coordXformInv.conjugate();

    // Create the OpenAL source
    alGenSources(1, &sourceID);
    ((vsSoundStream *)soundBuffer)->setSourceID(sourceID);

    // Configure
    alSourcefv(sourceID, AL_POSITION, zero);
    alSourcefv(sourceID, AL_DIRECTION, zero);
    alSourcefv(sourceID, AL_VELOCITY, zero);
    alSourcei(sourceID, AL_BUFFER, 0);
}

*/

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsSoundSourceAttribute::~vsSoundSourceAttribute()
{
    alDeleteSources(1, &sourceID);
}

// ------------------------------------------------------------------------
// Returns the interval of time since the last time this function was 
// called (based on the lastTime variable)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getTimeInterval()
{
    struct timeval tv;
    double         currentTime;
    double         deltaTime;

    gettimeofday(&tv, NULL);

    currentTime = tv.tv_sec + tv.tv_usec / 1E6;
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    return deltaTime;
}

// ------------------------------------------------------------------------
// VESS internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::attach(vsNode *theNode)
{
    if (attachedFlag)
    {
        printf("vsSoundSourceAttribute::attach: Attribute is already "
            "attached\n");
        return;
    }

    if (theNode->getNodeType() == VS_NODE_TYPE_GEOMETRY)
    {
        printf("vsSoundSourceAttribute::attach: Can't attach sound source "
            "attributes to geometry nodes\n");
        return;
    }

    parentComponent = ((vsComponent *)theNode);
    
    attachedFlag = 1;
}

// ------------------------------------------------------------------------
// VESS internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::detach(vsNode *theNode)
{
    if (!attachedFlag)
    {
        printf("vsSoundSourceAttribute::detach: Attribute is not attached\n");
        return;
    }

    parentComponent = NULL;
    
    attachedFlag = 0;
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
    pfMatrix       xform;
    vsMatrix       result;
    vsQuat         tempQuat;
    vsVector       tempVec;
    vsVector       deltaVec;
    double         interval;
/*
    int            buffersProcessed;
    ALuint         bufferID;
*/

    if (!attachedFlag)
        return;

    xform.makeIdent();
    
    result = parentComponent->getGlobalXform();

    result = result * offsetMatrix;
    
    // Update the position
    tempVec[VS_X] = result[0][3];
    tempVec[VS_Y] = result[1][3];
    tempVec[VS_Z] = result[2][3];
    tempVec = coordXform.rotatePoint(tempVec);

    // Update the OpenAL source's position
    alSource3f(sourceID, AL_POSITION, (float)tempVec[VS_X],
        (float)tempVec[VS_Y], (float)tempVec[VS_Z]); 

    // Update the velocity (based on the last frame's position)
    deltaVec = tempVec - lastPos;
    interval = getTimeInterval();
    deltaVec.scale(1/interval);
    alSource3f(sourceID, AL_VELOCITY, (float)deltaVec[VS_X],
        (float)deltaVec[VS_Y], (float)deltaVec[VS_Z]);

    lastPos = tempVec;

    // Update the orientation
    if (baseDirection.getMagnitude() > 0.0)
    {
        tempQuat.setMatrixRotation(result);
        tempQuat = coordXformInv * tempQuat * coordXform;
        tempVec = tempQuat.rotatePoint(baseDirection);
        alSource3f(sourceID, AL_DIRECTION, tempVec[VS_X], tempVec[VS_Y], 
            tempVec[VS_Z]);
    }

/*
    // For streaming sources, check to see if we need to swap buffers
    // to allow the old buffer to be refilled
    if (streamingSource)
    {
        alGetSourcei(sourceID, AL_BUFFERS_PROCESSED, &buffersProcessed);

        if (buffersProcessed > 0)
        {
            // The current buffer is done, swap buffers
            bufferID = ((vsSoundStream *)soundBuffer)->getFrontBufferID();
            alUnqueue(sourceID, 1, &bufferID);
            ((vsSoundStream *)soundBuffer)->swapBuffers();
        }
    }
*/
}

// ------------------------------------------------------------------------
// Begins playback of the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::play()
{
    alSourcePlay(sourceID);
}

// ------------------------------------------------------------------------
// Halts playback of the source.  If a streaming source, also unqueues all
// queued buffers.
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::stop()
{
/*
    int buffersProcessed;
*/

    alSourceStop(sourceID);

/*
    if (streamingSource)
    {
        ((vsSoundStream *)soundBuffer)->flushBuffers();
        alSourcei(sourceID, AL_BUFFER, 0);
    }
*/
}

// ------------------------------------------------------------------------
// Pauses playback of the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::pause()
{
    alSourcePause(sourceID);
}

/* 
   This is part of the OpenAL spec, but not yet implemented

// ------------------------------------------------------------------------
// Rewinds the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::rewind()
{
    alSourceRewind(sourceID);
}

*/

// ------------------------------------------------------------------------
// Returns whether or not this source is looping
// ------------------------------------------------------------------------
int vsSoundSourceAttribute::isLooping()
{
    ALint looping;

    alGetSourcei(sourceID, AL_LOOPING, &looping);

    return (int)looping;
}

// ------------------------------------------------------------------------
// Sets whether or not this source should loop
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setLooping(int looping)
{
    alSourcei(sourceID, AL_LOOPING, looping);
}

// ------------------------------------------------------------------------
// Retrieve the source gain adjustment (range = [0.0, inf), default = 1.0)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getGain()
{
    float gain;

    alGetListenerf(AL_GAIN, &gain);

    return (double)gain;
}

// ------------------------------------------------------------------------
// Adjust the source gain
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setGain(double gain)
{
    alSourcef(sourceID, AL_GAIN, (float)gain);
}

// ------------------------------------------------------------------------
// Returns the minimum gain of the source (range = [0.0, 1.0], 
// default = 0.0)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getMinGain()
{
    float gain;

    alGetSourcef(sourceID, AL_MIN_GAIN, &gain);

    return (double)gain;
}

// ------------------------------------------------------------------------
// Sets the minimum gain of the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setMinGain(double gain)
{
    alSourcef(sourceID, AL_MIN_GAIN, (float)gain);
}

// ------------------------------------------------------------------------
// Returns the maximum gain of the source (range = [0.0, 1.0], 
// default = 1.0)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getMaxGain()
{
    float gain;

    alGetSourcef(sourceID, AL_MAX_GAIN, &gain);

    return (double)gain;
}

// ------------------------------------------------------------------------
// Sets the maximum gain of the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setMaxGain(double gain)
{
    alSourcef(sourceID, AL_MIN_GAIN, (float)gain);
}

/*
   Not yet implemented

// ------------------------------------------------------------------------
// Returns the reference distance of the source (the distance at which 
// maximum gain is heard) (range = [0, inf), default = 1.0)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getReferenceDistance()
{
    float dist;

    alGetSourcef(sourceID, AL_REFERENCE_DISTANCE, &dist);

    return (double)dist;
}

// ------------------------------------------------------------------------
// Sets the reference distance of the source 
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setReferenceDistance(double dist)
{
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

    alGetSourcef(sourceID, AL_MAX_DISTANCE, &dist);

    return (double)dist;
}

// ------------------------------------------------------------------------
// Sets the maximum distance of the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setMaxDistance(double dist)
{
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

    alGetSourcef(sourceID, AL_ROLLOFF_FACTOR, &factor);

    return (double)factor;
}

// ------------------------------------------------------------------------
// Sets the rolloff factor of the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setRolloffFactor(double factor)
{
    alSourcef(sourceID, AL_ROLLOFF_FACTOR, (float)gain);
}

*/

// ------------------------------------------------------------------------
// Returns the current pitch shifting of the source.  Every 50% reduction
// or in pitch shift parameter results in the shifting of the source 
// sound's pitch by one octave (12 semitones).  (range = (0.0, 1.0],
// default = 1.0)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getPitchShift()
{
    float shift;

    alGetSourcef(sourceID, AL_PITCH, &shift);

    return (double)shift;
}

// ------------------------------------------------------------------------
// Sets the pitch shift of the source
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setPitchShift(double shift)
{
    alSourcef(sourceID, AL_PITCH, (float)shift);
}

// ------------------------------------------------------------------------
// Returns the source's base direction of sound radiation (in VESS 
// coordinates) (default = (0.0, 0.0, 0.0), i.e. non-directional)
// ------------------------------------------------------------------------
vsVector vsSoundSourceAttribute::getDirection()
{
    vsVector direction;

    direction = baseDirection;

    if (direction.getMagnitude() != 0.0)
    {
        // Normalize and convert to VESS coordinates
        direction.normalize();
        direction = coordXformInv.rotatePoint(direction);
    }

    return direction;
}

// ------------------------------------------------------------------------
// Sets the source's base direction of sound radiation
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setDirection(vsVector direction)
{
    if (direction.getMagnitude() != 0.0)
    {
        // Normalize and convert to OpenAL coordinates
        direction.normalize();
        direction = coordXform.rotatePoint(direction);
    }

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

    alGetSourcef(sourceID, AL_CONE_INNER_ANGLE, &angle);

    return (double)angle;
}

// ------------------------------------------------------------------------
// Sets the directional source's inner cone angle
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setInnerConeAngle(double angle)
{
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

    alGetSourcef(sourceID, AL_CONE_OUTER_ANGLE, &angle);

    return (double)angle;
}

// ------------------------------------------------------------------------
// Sets the directional source's outer cone angle
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setOuterConeAngle(double angle)
{
    alSourcef(sourceID, AL_CONE_OUTER_ANGLE, (float)angle);
}

/* 
   In the spec, but not yet implemented

// ------------------------------------------------------------------------
// Sets the gain reduction of a directional source's outer cone of 
// radiation.  A gain of 1.0 means no reduction (same as the inner cone)
// while a gain of 0.0 means no sound is heard at all in the outer cone.
// (range = [0.0, 1.0], default = 0.0)
// ------------------------------------------------------------------------
double vsSoundSourceAttribute::getOuterConeGain()
{
    float gain;

    alGetSourcef(sourceID, AL_CONE_OUTER_GAIN, &gain);

    return (double)gain;
}

// ------------------------------------------------------------------------
// Sets the directional source's outer cone gain
// ------------------------------------------------------------------------
void vsSoundSourceAttribute::setOuterConeGain(double gain)
{
    alSourcef(sourceID, AL_CONE_OUTER_GAIN, (float)gain);
}

*/
