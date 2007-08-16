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
//     VESS Module:  vsSoundAttributeComponentTuple.h++
//
//     Description:  The class is used to store a vsSoundSourceAttribute
//                   and vsComponent pairing.  It also handles the 
//                   attribute removal and unref deletion by itself
//
//     Author(s):     Michael Whiteley
//
//------------------------------------------------------------------------

#ifndef VS_SOUND_ATTRIBUTE_COMPONENT_TUPLE_HPP
#define VS_SOUND_ATTRIBUTE_COMPONENT_TUPLE_HPP

#include <atItem.h++>
#include "vsComponent.h++"
#include "vsGlobals.h++"
#include "vsSoundSourceAttribute.h++"

class VS_SOUND_DLL vsSoundAttributeComponentTuple : public atItem
{
protected:
    
    vsSoundSourceAttribute    *soundSourceAttribute;
    vsComponent               *component;

public:

                                      vsSoundAttributeComponentTuple(
                                          vsSoundSourceAttribute *soundSource, 
                                          vsComponent *component);
    virtual                           ~vsSoundAttributeComponentTuple();

    virtual vsSoundSourceAttribute    *getSoundSourceAttribute();
};

#endif

