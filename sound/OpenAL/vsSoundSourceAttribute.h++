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
//    VESS Module:  vsSoundSourceAttribute.h++
//
//    Description:  Attribute to maintain the location/orientation of a
//                  source of sound in the VESS scene graph
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_SOUND_SOURCE_ATTRIBUTE_HPP
#define VS_SOUND_SOURCE_ATTRIBUTE_HPP

#include <pthread.h>
#include "vsComponent.h++"
#include "vsAttribute.h++"
#include "vsSoundSample.h++"
#include "vsSoundStream.h++"

#include "vsVector.h++"
#include "vsMatrix.h++"
#include "vsQuat.h++"
#include "vsTimer.h++"
 
// Sound source priorities, used for voice management
enum
{
    VS_SSRC_PRIORITY_LOW,
    VS_SSRC_PRIORITY_NORMAL,
    VS_SSRC_PRIORITY_HIGH,
    VS_SSRC_PRIORITY_ALWAYS_ON
};

class VS_SOUND_DLL vsSoundSourceAttribute : public vsAttribute
{
protected:

    // The sound data
    vsSoundBuffer      *soundBuffer;

    // Type of source
    bool               loopSource;
    bool               streamingSource;

    // Pthread mutex for synchronization of operations
    pthread_mutex_t    sourceMutex;

    // Our alSource ID number, and a flag to indicate whether or not
    // the ID number is valid
    ALuint             sourceID;
    bool               sourceValid;

    // Indicates that a streaming source is out of sound data
    bool               outOfData;

    // Offset transform from the component to the sound listener
    vsMatrix           offsetMatrix;

    // Base direction of radiation (prior to transforms)
    vsVector           baseDirection;

    // The vsComponent we're attached to
    vsComponent        *parentComponent;

    // Previous location/direction
    vsVector           lastPos;
    vsVector           lastDir;

    // Coordinate conversion quaternions
    vsQuat             coordXform;
    vsQuat             coordXformInv;

    // Playing state of the source
    int                playState;

    // Playback timer
    vsTimer            *playTimer;

    // Source parameters
    double             gain;
    double             maxGain;
    double             minGain; 
    double             refDistance;
    double             maxDistance;
    double             rolloffFactor;
    double             pitch;
    double             innerConeAngle;
    double             outerConeAngle;
    double             outerConeGain;

    // Priority of this source
    int                priority;

VS_INTERNAL:

    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);

    virtual void    attachDuplicate(vsNode *theNode);

    bool            isActive();
    void            assignVoice(int voiceID);
    void            revokeVoice();
    int             getVoiceID();

    double          getEffectiveGain(vsVector listenerPos);
    vsVector        getLastPosition();

    void            lockSource();
    void            unlockSource();

    void            updateStream();
    void            updatePlayState();

public:

    // Constructor for a static sound source (either a looping or triggered
    // sound)
                          vsSoundSourceAttribute(vsSoundSample *buffer,
                                                 bool loop);

    // Constructor for a streaming sound source
                          vsSoundSourceAttribute(vsSoundStream *buffer);

    // Destructor
    virtual               ~vsSoundSourceAttribute();

    // Inherited methods
    virtual const char    *getClassName();

    virtual int           getAttributeType();
    virtual int           getAttributeCategory();

    // Accessor for the sound buffer
    vsSoundBuffer         *getSoundBuffer();

    // Offset from the component's global transform
    void                  setOffsetMatrix(vsMatrix newMatrix);
    vsMatrix              getOffsetMatrix();

    // Update function.  Sets the new source position, velocity, and direction
    // based on the attached component's global transform
    void                  update();

    // Source control
    void                  play();
    void                  stop();
    void                  pause();
    void                  rewind();
    bool                  isPlaying();
    bool                  isStopped();
    bool                  isPaused();

    // Loop control
    bool                  isLooping();
    void                  setLooping(bool looping);

    bool                  isStreaming();

    // Volume and distance attenuation parameters
    double                getGain();
    void                  setGain(double gain);
    double                getMinGain();
    void                  setMinGain(double gain);
    double                getMaxGain();
    void                  setMaxGain(double gain);

    double                getReferenceDistance();
    void                  setReferenceDistance(double distance);
    double                getMaxDistance();
    void                  setMaxDistance(double distance);
    double                getRolloffFactor();
    void                  setRolloffFactor(double factor);

    // Pitch parameters
    double                getPitchShift();
    void                  setPitchShift(double shift);

    // Directional sound parameters
    vsVector              getDirection();
    void                  setDirection(vsVector direction);
    double                getInnerConeAngle();
    void                  setInnerConeAngle(double angle);
    double                getOuterConeAngle();
    void                  setOuterConeAngle(double angle);
    double                getOuterConeGain();
    void                  setOuterConeGain(double gain);

    // Priority control
    void                  setPriority(int newPriority);
    int                   getPriority();
    
    ALuint                getBaseLibraryObject();
};

#endif
