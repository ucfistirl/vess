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

    int                soundAttributesPriority;
    double             soundRolloffFactor;
    double             soundReferenceDistance;
    double             soundMaxDistance;

    atList             *playingSounds;
    atMap              *soundCache;

    vsComponent        *rootComponent;

public:

                            vsSoundBank();
                            vsSoundBank(int priority);
                            vsSoundBank(int priority,
                              double rolloff, double referenceDistance,
                              double maxDistance);
    virtual                 ~vsSoundBank();

    virtual void            setRootComponent(vsComponent *root);
    virtual vsComponent     *getRootComponent();

    virtual void            setPriority(int priority);
    virtual int             getPriority();

    virtual void            setRolloffFactor(double rolloff);
    virtual double          getRolloffFactor();

    virtual void            setReferenceDistance(double reference);
    virtual double          getReferenceDistance();

    virtual void            setMaxDistance(double max);
    virtual double          getMaxDistance();


    virtual void            addSoundSample(char *key, vsSoundSample *sample);
    virtual void            addSoundSample(char *key, char *filename);
    virtual void            removeSoundSample(char *key);

    virtual void            clearBanks();
    
    virtual int             playSound(char *key, vsComponent *source);

    virtual void            pauseAllSound();
    virtual void            resumeAllSound();
    virtual void            stopAllSound();

    virtual bool            pauseSound(int id);
    virtual bool            resumeSound(int id);
    virtual bool            stopSound(int id);

    virtual bool            isSoundPlaying(int id = -1);

    const char              *getClassName();

    virtual void            update();
};

#endif

