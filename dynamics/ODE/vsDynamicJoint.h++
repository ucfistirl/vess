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
//    VESS Module:  vsDynamicJoint.h++
//
//    Description:  This abstract class represents a joint in a dynamic
//                  environment.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_DYNAMIC_JOINT_HPP
#define VS_DYNAMIC_JOINT_HPP

#include <ode/ode.h>

#include "vsDynamicUnit.h++"
#include "vsObject.h++"

#include "atVector.h++"

#include "vsGlobals.h++"

class VS_DYNAMICS_DLL vsDynamicJoint : public vsObject
{
protected:

    dJointID          odeJointID;

    dJointFeedback    *odeJointFeedback;

VS_INTERNAL:

    dJointID    getODEJointID();
    dBodyID     getAttachedODEBodyID(int index);

public:

                    vsDynamicJoint(bool feedback);
    virtual         ~vsDynamicJoint();

    virtual const char    *getClassName() = 0;

    virtual void    attach(vsDynamicUnit *unit1, vsDynamicUnit *unit2);

    void            getFeedback(atVector *force1, atVector *torque1,
                        atVector *force2, atVector *torque2);
};

#endif

