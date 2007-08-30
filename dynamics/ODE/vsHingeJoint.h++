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
//    VESS Module:  vsHingeJoint.h++
//
//    Description:  This vsDynamicJoint subclass represents a hinge joint.
//                  It attempts to lock its attached units to the same
//                  relative position and orientation at the time of
//                  attachment, with the exception of a single axis around
//                  which the joint is allowed to rotate.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_HINGE_JOINT_HPP
#define VS_HINGE_JOINT_HPP

#include "vsDynamicJoint.h++"
#include "vsDynamicWorld.h++"

#include <ode/ode.h>

#include "atVector.h++"

#include "vsGlobals.h++"

class VS_DYNAMICS_DLL vsHingeJoint : public vsDynamicJoint
{
public:

                vsHingeJoint(vsDynamicWorld *world, bool feedback);
    virtual     ~vsHingeJoint();

    virtual const char    *getClassName();

    void        setAnchor(const atVector &anchor);
    void        setAxis(const atVector &axis);

    void        setMinimumAngle(double angle);
    void        setMaximumAngle(double angle);
    void        setLimitBounce(double bounce);

    double      getAngle();
    double      getVelocity();
};

#endif

