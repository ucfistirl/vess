#ifndef VS_SOUND_SOURCE_ATTRIBUTE_HPP
#define VS_SOUND_SOURCE_ATTRIBUTE_HPP

// Attribute to maintain the location/orientation of a source of sound
// in the VESS scene graph

#include <Performer/pf/pfGroup.h>
#include "vsAttribute.h++"
#include "vsSoundBuffer.h++"
#include "vsVector.h++"
#include "vsMatrix.h++"
#include "vsQuat.h++"

class vsSoundSourceAttribute : public vsAttribute
{
protected:

    // The sound data
    vsSoundBuffer    *soundBuffer;
    int              loopSource;

    // Our alSource ID number
    ALuint           sourceID;

    // Offset transform from the component to the sound listener
    vsMatrix         offsetMatrix;

    // Base direction of radiation (prior to transforms)
    vsVector         baseDirection;

    pfGroup          *componentMiddle;

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

    void        attach(vsNode *theNode);
    void        detach(vsNode *theNode);

    void        update();

public:

                   vsSoundSourceAttribute(vsSoundBuffer *buffer, int loop);
    virtual        ~vsSoundSourceAttribute();

    virtual int    getAttributeType();
    virtual int    getAttributeCategory();

    // Offset from the component's global transform
    void           setOffsetMatrix(vsMatrix newMatrix);
    vsMatrix       getOffsetMatrix();

    // Source control
    void           play();
    void           stop();
    void           pause();

/*  
    Not yet implemented

    void           rewind();
*/

    // Loop control
    int            isLooping();
    void           setLooping(int looping);

    // Volume and distance attenuation parameters
    double         getGain();
    void           setGain(double gain);
    double         getMinGain();
    void           setMinGain(double gain);
    double         getMaxGain();
    void           setMaxGain(double gain);

/*  In the OpenAL spec, but not yet implemented

    double         getReferenceDistance();
    void           setReferenceDistance(double distance);
    double         getMaxDistance();
    void           setMaxDistance(double distance);
    double         getRolloffFactor();
    void           setRolloffFactor(double factor);
*/

    // Pitch parameters
    double         getPitchShift();
    void           setPitchShift(double shift);

    // Directional sound parameters
    vsVector       getDirection();
    void           setDirection(vsVector direction);
    double         getInnerConeAngle();
    void           setInnerConeAngle(double angle);
    double         getOuterConeAngle();
    void           setOuterConeAngle(double angle);

/*  In the OpenAL spec, but not yet implemented

    double         getOuterConeGain();
    void           setOuterConeGain(double gain);
*/
    
};

#endif
