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
//    VESS Module:  vsSoundListenerAttribute.h++
//
//    Description:  Attribute to maintain the location/orientation of the
//                  listener in the VESS scene graph
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_SOUND_LISTENER_ATTRIBUTE_HPP
#define VS_SOUND_LISTENER_ATTRIBUTE_HPP

#include "vsComponent.h++"
#include "vsAttribute.h++"
#include "vsSoundBuffer.h++"
#include "vsVector.h++"
#include "vsMatrix.h++"
#include "vsQuat.h++"

class vsSoundListenerAttribute : public vsAttribute
{
protected:

    // Offset transform from the component to the sound listener
    vsMatrix       offsetMatrix;

    // The component we're attached to
    vsComponent    *parentComponent;

    // Previous location/orientation
    vsVector       lastPos;
    vsVector       lastOrn;

    // Time of last update (in seconds)
    double         lastTime;

    // Coordinate conversion quaternions
    vsQuat         coordXform;
    vsQuat         coordXformInv;

    // Returns the time in seconds since the last time this function was
    // called
    double         getTimeInterval();

VS_INTERNAL:

    void        attach(vsNode *theNode);
    void        detach(vsNode *theNode);

public:

                   vsSoundListenerAttribute();
    virtual        ~vsSoundListenerAttribute();

    // Inherited methods
    virtual int    getAttributeType();
    virtual int    getAttributeCategory();

    // Offsets from the parentComponent's transform
    void           setOffsetMatrix(vsMatrix newMatrix);
    vsMatrix       getOffsetMatrix();

    // Sets the new listener position, velocity, and orientation based on
    // the parentComponent's global transform
    void           update();

    // Listener attributes
    double         getGain();
    void           setGain(double gain);
};

#endif
