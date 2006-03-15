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
#include "vsVector.h++"
#include "vsMatrix.h++"
#include "vsQuat.h++"

class VS_SOUND_DLL vsSoundListenerAttribute : public vsAttribute
{
protected:

    // Offset transform from the component to the sound listener
    vsMatrix       offsetMatrix;

    // The component we're attached to
    vsComponent    *parentComponent;

    // Previous location/orientation
    vsVector       lastPos;
    vsQuat         lastOrn;

    // Coordinate conversion quaternions
    vsQuat         coordXform;
    vsQuat         coordXformInv;

VS_INTERNAL:

    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);

    virtual void    attachDuplicate(vsNode *theNode);

    vsVector        getLastPosition();
    vsQuat          getLastOrientation();

public:

                          vsSoundListenerAttribute();
    virtual               ~vsSoundListenerAttribute();

    // Inherited methods
    virtual const char    *getClassName();

    virtual int           getAttributeType();
    virtual int           getAttributeCategory();

    // Offsets from the parentComponent's transform
    void                  setOffsetMatrix(vsMatrix newMatrix);
    vsMatrix              getOffsetMatrix();

    // Sets the new listener position, velocity, and orientation based on
    // the parentComponent's global transform
    void                  update();

    // Listener attributes
    double                getGain();
    void                  setGain(double gain);
};

#endif
