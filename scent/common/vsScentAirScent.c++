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
//    VESS Module:  vsScent.h++
//
//    Description:  Descendant of the vsScent class providing support for
//                  the ScentAir olfactory device.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsScentAirScent.h++"

// ------------------------------------------------------------------------
// Constructor.  Creates a vsScent using the given ScentAir system and
// scent channel.
// ------------------------------------------------------------------------
vsScentAirScent::vsScentAirScent(vsScentAirSystem *system, int channel)
{
    // Save the system and scent channel
    scentAir = system;
    scentChannel = channel;
}

// ------------------------------------------------------------------------
// Destructor.  Does nothing
// ------------------------------------------------------------------------
vsScentAirScent::~vsScentAirScent()
{
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsScentAirScent::getClassName()
{
    return "vsScentAirScent";
}

// ------------------------------------------------------------------------
// Returns the current scent strength
// ------------------------------------------------------------------------
double vsScentAirScent::getStrength()
{
    return strength;
}

// ------------------------------------------------------------------------
// Adjusts the current scent strength
// ------------------------------------------------------------------------
void vsScentAirScent::setStrength(double newStrength)
{
    // Store the new strength (in case someone asks for it)
    strength = newStrength;
    
    // Clamp the value to 0.0-1.0 range.  A strength of less than 0.0 or 
    // greater than 1.0 is not valid.
    if (strength < 0.0)
    {
        strength = 0.0;
    }
    else if (strength > 1.0)
    {
        strength = 1.0;
    }

    // Turn the channel on if the strength is not zero
    if (fabs(strength) > 1.0e-6)
    {
        // Adjust the pulse rate of the ScentAir to match the strength.
        // Note that most scents are much too strong at the maximum strength.
        // The scent scale on the vsScentSourceAttribute is a good place to
        // adjust this.  
        scentAir->setPulseRate(scentChannel, strength*10.0,
            10.0 - strength*10.0);
	scentAir->setChannelState(scentChannel, true);
    }
    else
    {
        // Turn the channel off
	scentAir->setPulseRate(scentChannel, 0.0, 0.0);
	scentAir->setChannelState(scentChannel, false);
    }
}
