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

#include "vsComponent.h++"
#include "vsAttribute.h++"
#include "vsSoundSample.h++"
#include "vsSoundStream.h++"

#include "vsVector.h++"
#include "vsMatrix.h++"
#include "vsQuat.h++"

class vsSoundSourceAttribute : public vsAttribute
{
protected:

    // The sound data
    vsSoundBuffer    *soundBuffer;
    int              loopSource;
    int              streamingSource;

    // Our alSource ID number
    ALuint           sourceID;

    // Offset transform from the component to the sound listener
    vsMatrix         offsetMatrix;

    // Base direction of radiation (prior to transforms)
    vsVector         baseDirection;

    // The vsComponent we're attached to
    vsComponent      *parentComponent;

    // Previous location/orientation
    vsVector         lastPos;
    vsVector         lastOrn;

    // Time of last update (in seconds)
    double           lastTime;

    // Coordinate conversion quaternions
    vsQuat           coordXform;
    vsQuat           coordXformInv;

    // Returns the time in seconds since the last time this function was
    // called
    double           getTimeInterval();

VS_INTERNAL:

    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);

    virtual void    attachDuplicate(vsNode *theNode);

public:

    // Constructor for a static sound source (either a looping or triggered
    // sound)
                          vsSoundSourceAttribute(vsSoundSample *buffer, int loop);

    // Constructor for a streaming sound source
                          vsSoundSourceAttribute(vsSoundStream *buffer);

    // Destructor
    virtual               ~vsSoundSourceAttribute();

    // Inherited methods
    virtual const char    *getClassName();

    virtual int           getAttributeType();
    virtual int           getAttributeCategory();

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

    // Loop control
    int                   isLooping();
    void                  setLooping(int looping);

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
    
    ALuint                getBaseLibraryObject();
};

#endif
