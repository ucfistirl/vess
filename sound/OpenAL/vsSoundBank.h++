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
//    VESS Module:  vsSoundBank.h++
//
//    Description:  A class used to store a bank of sounds that can have
//                  multiple copies of the same sounds playing.  It also
//                  manages the removal of the sound source attributes by
//                  way of VESS reference counting.
//                  
//           Note:  This class is meant to be used with sounds that play
//                  multiple times in sucession.  This is not meant to be
//                  used with looping audio
//
//    Author(s):    Michael Whiteley
//
//------------------------------------------------------------------------

#ifndef VS_SOUND_BANK_HPP
#define VS_SOUND_BANK_HPP

#include <atItem.h++>
#include <atList.h++>
#include <atMap.h++>
#include "vsGlobals.h++"
#include "vsObject.h++"
#include "vsComponent.h++"
#include "vsSoundSample.h++"
#include "vsSoundSourceAttribute.h++"


class VS_SOUND_DLL vsSoundBank : public vsObject
{
protected:

    // Start the private variables here make sure to leave a blank line after
    // all declarations
    bool               soundPaused;  

    int                soundAttributesPriority;

    atList             *playingSounds;
    atMap              *soundCache;

    vsComponent        *rootComponent;

public:

                            vsSoundBank();
                            vsSoundBank(int priority);
    virtual                 ~vsSoundBank();

    virtual void            setRootComponent(vsComponent *root);
    virtual vsComponent     *getRootComponent();

    virtual void            setPriority(int priority);
    virtual int             getPriority();

    virtual void            addSoundSample(char *key, vsSoundSample *sample);
    virtual void            addSoundSample(char *key, char *filename);
    virtual void            removeSoundSample(char *key);

    virtual void            clearBanks();
    
    virtual void            playSound(char *key, vsComponent *source);

    virtual void            pauseAllSound();
    virtual void            resumeAllSound();
    virtual void            stopAllSound();

    virtual bool            isSoundPlaying();

    const char              *getClassName();

    virtual void            update();
};

#endif

