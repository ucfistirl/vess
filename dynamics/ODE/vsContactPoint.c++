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
//    VESS Module:  vsContactPoint.c++
//
//    Description:  A contact point
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsContactPoint.h++"

// ------------------------------------------------------------------------
// Constructor - Default
// The default constructor does nothing. The underlying structure defining
// the contact is left untouched.
// ------------------------------------------------------------------------
vsContactPoint::vsContactPoint()
{
    odeContact.surface.mode = dContactSoftCFM;
    odeContact.surface.mu = VS_CONTACT_DEFAULT_MU;
    odeContact.surface.soft_erp = VS_CONTACT_DEFAULT_SOFT_ERP;
    odeContact.surface.soft_cfm = VS_CONTACT_DEFAULT_SOFT_CFM;
}

// ------------------------------------------------------------------------
// Constructor - VESS Internal
// This constructor builds the contact based on a specified dContactGeom.
// ------------------------------------------------------------------------
vsContactPoint::vsContactPoint(const dContactGeom &geom)
{
    odeContact.surface.mode = dContactSoftCFM;
    odeContact.surface.mu = VS_CONTACT_DEFAULT_MU;
    odeContact.surface.soft_erp = VS_CONTACT_DEFAULT_SOFT_ERP;
    odeContact.surface.soft_cfm = VS_CONTACT_DEFAULT_SOFT_CFM;

    // Store the geometry aspects.
    odeContact.geom = geom;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsContactPoint::~vsContactPoint()
{
}

// ------------------------------------------------------------------------
// This method sets the bounce properties. The factor indicates the
// magnitude of the velocity after the bounce (relative to the initial
// velocity). Any incident velocity below the threshold value will be
// counted as 0 and incur no bounce.
// ------------------------------------------------------------------------
void vsContactPoint::setBounce(bool bounce, double factor, double threshold)
{
    if (bounce)
    {
        odeContact.surface.mode |= dContactBounce;
        odeContact.surface.bounce = factor;
        odeContact.surface.bounce_vel = threshold;
    }
    else if (odeContact.surface.mode & dContactBounce)
    {
        odeContact.surface.mode -= dContactBounce;
    }
}

// ------------------------------------------------------------------------
// This method sets the constraint force mixing parameter associated with
// this collision.
// ------------------------------------------------------------------------
void vsContactPoint::setConstraintForceMixing(double cfm)
{
    odeContact.surface.soft_cfm = cfm;
}

// ------------------------------------------------------------------------
// This method sets the error reduction parameter associated with this
// collision.
// ------------------------------------------------------------------------
void vsContactPoint::setErrorReductionParameter(double erp)
{
    odeContact.surface.soft_erp = erp;
}

// ------------------------------------------------------------------------
// This method sets the friction of the contact.
// ------------------------------------------------------------------------
void vsContactPoint::setFriction(double mu)
{
    odeContact.surface.mu = mu;
}

// ------------------------------------------------------------------------
// VESS Internal Function
// This method sets the contact geom of this contact point, specifying the
// location, normal, and depth of the contact as well as the two geometries
// involved.
// ------------------------------------------------------------------------
void vsContactPoint::setContactGeom(const dContactGeom &geom)
{
    odeContact.geom = geom;
}

// ------------------------------------------------------------------------
// VESS Internal Function
// Returns the dContact associated with this contact point, which may be
// used to build a dContactJoint.
// ------------------------------------------------------------------------
dContact vsContactPoint::getODEContact() const
{
    return odeContact;
}

