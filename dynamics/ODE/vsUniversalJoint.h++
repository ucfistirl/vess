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
//    VESS Module:  vsUniversalJoint.h++
//
//    Description:  This vsDynamicJoint subclass represents a universal
//                  joint. It attempts to lock its attached units to the
//                  same relative position as at the time of attachment,
//                  and allows rotation around two orthogonal axes.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_UNIVERSAL_JOINT_HPP
#define VS_UNIVERSAL_JOINT_HPP

#include "vsDynamicJoint.h++"
#include "vsDynamicWorld.h++"

#include <ode/ode.h>

#include "atVector.h++"

#include "vsGlobals.h++"

class VS_DYNAMICS_DLL vsUniversalJoint : public vsDynamicJoint
{
public:

                vsUniversalJoint(vsDynamicWorld *world, bool feedback);
    virtual     ~vsUniversalJoint();

    virtual const char    *getClassName();

    void        setAnchor(const atVector &anchor);
    void        setAxis1(const atVector &axis);
    void        setAxis2(const atVector &axis);

    void        setMinimumAngle(double angle);
    void        setMaximumAngle(double angle);
    void        setLimitBounce(double bounce);
};

#endif

