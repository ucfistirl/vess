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
//    VESS Module:  vsFixedJoint.h++
//
//    Description:  This vsDynamicJoint subclass represents a fixed joint.
//                  It attempts to lock its attached units to the same
//                  relative position and orientation at the time of
//                  attachment.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_FIXED_JOINT_HPP
#define VS_FIXED_JOINT_HPP

#include "vsDynamicJoint.h++"
#include "vsDynamicWorld.h++"

#include <ode/ode.h>

#include "vsGlobals.h++"

class VS_DYNAMICS_DLL vsFixedJoint : public vsDynamicJoint
{
public:

                    vsFixedJoint(vsDynamicWorld *world, bool feedback);
    virtual         ~vsFixedJoint();

    virtual const char    *getClassName();

    virtual void    attach(vsDynamicUnit *unit1, vsDynamicUnit *unit2);
};

#endif

