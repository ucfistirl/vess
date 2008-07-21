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
//    VESS Module:  vsSoundSampleRef.h++
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

#ifndef VS_SOUND_SAMPLE_REF_HPP
#define VS_SOUND_SAMPLE_REF_HPP

#include <atItem.h++>
#include "vsGlobals.h++"
#include "vsSoundSample.h++"

class VESS_SYM vsSoundSampleRef : public atItem
{
protected:

    vsSoundSample * sample;

public:

                     vsSoundSampleRef(vsSoundSample * theSample);
    virtual          ~vsSoundSampleRef();

    vsSoundSample    *getSample();
};

#endif

