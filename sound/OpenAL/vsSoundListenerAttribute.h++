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

#include "vsSoundBuffer.h++"
#include "vsComponent.h++"
#include "vsAttribute.h++"
#include "atVector.h++"
#include "atMatrix.h++"
#include "atQuat.h++"

class VS_SOUND_DLL vsSoundListenerAttribute : public vsAttribute
{
protected:

    // Offset transform from the component to the sound listener
    atMatrix       offsetMatrix;

    // The component we're attached to
    vsComponent    *parentComponent;

    // Previous location/orientation
    atVector       lastPos;
    atQuat         lastOrn;

    // Coordinate conversion quaternions
    atQuat         coordXform;
    atQuat         coordXformInv;

VS_INTERNAL:

    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);

    virtual void    attachDuplicate(vsNode *theNode);

    atVector        getLastPosition();
    atQuat          getLastOrientation();

public:

                          vsSoundListenerAttribute();
    virtual               ~vsSoundListenerAttribute();

    // Inherited methods
    virtual const char    *getClassName();

    virtual int           getAttributeType();
    virtual int           getAttributeCategory();

    // Offsets from the parentComponent's transform
    void                  setOffsetMatrix(atMatrix newMatrix);
    atMatrix              getOffsetMatrix();

    // Sets the new listener position, velocity, and orientation based on
    // the parentComponent's global transform
    void                  update();

    // Listener attributes
    double                getGain();
    void                  setGain(double gain);
};

#endif
