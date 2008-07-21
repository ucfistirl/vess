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
//    VESS Module:  vsBallJoint.h++
//
//    Description:  This vsDynamicJoint subclass represents a ball joint.
//                  It takes an anchor position and attempts to lock its
//                  attached units to the same relative radius around that
//                  position.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_BALL_JOINT_HPP
#define VS_BALL_JOINT_HPP

#include "vsDynamicJoint.h++"
#include "vsDynamicWorld.h++"

#include <ode/ode.h>

#include "atVector.h++"

#include "vsGlobals.h++"

class VESS_SYM vsBallJoint : public vsDynamicJoint
{
public:

                vsBallJoint(vsDynamicWorld *world, bool feedback);
    virtual     ~vsBallJoint();

    virtual const char    *getClassName();

    void        setAnchor(const atVector &anchor);
};

#endif

