#ifndef VS_SOUND_LISTENER_ATTRIBUTE_HPP
#define VS_SOUND_LISTENER_ATTRIBUTE_HPP

// Attribute to maintain the location/orientation of the listener
// in the VESS scene graph

#include <Performer/pf/pfGroup.h>
#include "vsAttribute.h++"
#include "vsSoundBuffer.h++"
#include "vsVector.h++"
#include "vsMatrix.h++"
#include "vsQuat.h++"

class vsSoundListenerAttribute : public vsAttribute
{
protected:

    // Offset transform from the component to the sound listener
    vsMatrix    offsetMatrix;

    pfGroup     *componentMiddle;

    // Previous location/orientation
    vsVector    lastPos;
    vsVector    lastOrn;

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

                   vsSoundListenerAttribute();
    virtual        ~vsSoundListenerAttribute();

    virtual int    getAttributeType();
    virtual int    getAttributeCategory();

    void           setOffsetMatrix(vsMatrix newMatrix);
    vsMatrix       getOffsetMatrix();

    double         getGain();
    void           setGain(double gain);
};

#endif
