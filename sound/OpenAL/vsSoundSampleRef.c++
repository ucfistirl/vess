//------------------------------------------------------------------------
//
//    VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//    Copyright (c) 2007, University of Central Florida
//
//       See the file LICENSE for license information
//
//    E-mail:  vess@ist.ucf.edu
//    WWW:     http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//    VESS Module:  vsSoundSampleRef.c++
//
//    Description:  This class is a temporary solution to a problem caused
//                  by the way atMap is currently functioning.  Since the
//                  atMap deletes the values it has we can't directly
//                  store a vsSoundSample in an atMap (especially since
//                  the vsSoundSample could be referenced in multiple places)
//                  So the solution is to make this class and have it
//                  store the sound sample and ref/unrefdelete when needed
//
//    Author(s):    Jason Daly, Michael Whiteley
//
//------------------------------------------------------------------------

#include "vsSoundSampleRef.h++"

//------------------------------------------------------------------------
// Constructor that refs and sets the local sound sample to what has
// been passed in by the user
//------------------------------------------------------------------------
vsSoundSampleRef::vsSoundSampleRef(vsSoundSample *theSample)
{
   // Store the sample
   sample = theSample;

   // Reference the sample
   sample->ref();
}

//------------------------------------------------------------------------
// Unref Delete the object when this object is deleted
//------------------------------------------------------------------------
vsSoundSampleRef::~vsSoundSampleRef()
{
   // Unref delete the sample
   vsObject::unrefDelete(sample);
}

vsSoundSample *vsSoundSampleRef::getSample()
{
   // Return the stored sample
   return sample;
}
