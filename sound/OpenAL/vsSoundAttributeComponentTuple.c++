//------------------------------------------------------------------------
//
//     VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//     Copyright (c) 2007, University of Central Florida
//
//         See the file LICENSE for license information
//
//     E-mail:  vess@ist.ucf.edu
//     WWW:      http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//     VESS Module:  vsSoundAttributeComponentTuple.c++
//
//     Description:  The class is used to store a vsSoundSourceAttribute
//                   and vsComponent pairing.  It also handles the 
//                   attribute removal and unref deletion by itself
//
//     Author(s):     Michael Whiteley
//
//------------------------------------------------------------------------

#include "vsSoundAttributeComponentTuple.h++"

//------------------------------------------------------------------------
// Constructor that sets the local variables to those that are passed in
// it also references the variables
//------------------------------------------------------------------------
vsSoundAttributeComponentTuple::vsSoundAttributeComponentTuple(
    vsSoundSourceAttribute *soundSource, vsComponent *component)
{
    // Store the local values and then reference them
    soundSourceAttribute = soundSource;
    soundSourceAttribute->ref();

    this->component = component;
    this->component->ref();
}

//------------------------------------------------------------------------
// Removes the attribute from the component and then unref deletes both
// the component and the sound source attribute
//------------------------------------------------------------------------
vsSoundAttributeComponentTuple::~vsSoundAttributeComponentTuple()
{
    // Remove the attribute from the component
    component->removeAttribute(soundSourceAttribute);

    // Unref delete both the soundSourceAttribute and the component
    vsObject::unrefDelete(soundSourceAttribute);
    vsObject::unrefDelete(component);

    // Since vsSoundManager keeps a reference, check to see if the reference
    // count is one.  If it is that means that the only thing that has
    // a reference to this soundSourceAttribute is the vsSoundManager
    // so we can delete it.  Delete the vsSoundSourceAttribute will remove it
    // from the vsSoundManager along with the reference
    if (soundSourceAttribute->getRefCount() == 1)
    {
        delete soundSourceAttribute;
    }
}

//------------------------------------------------------------------------
// Returns the sound source attribute that is stored
//------------------------------------------------------------------------
vsSoundSourceAttribute *
    vsSoundAttributeComponentTuple::getSoundSourceAttribute()
{
   // Return the stored attribute
   return soundSourceAttribute;
}
