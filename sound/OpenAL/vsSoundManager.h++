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
//    VESS Module:  vsSoundManager.h++
//
//    Description:  Singleton class to watch over all audio 
//                  operations.  
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_SOUND_MANAGER_HPP
#define VS_SOUND_MANAGER_HPP

#include "vsObject.h++"
#include "vsSoundPipe.h++"
#include "vsSoundSourceAttribute.h++"
#include "vsSoundListenerAttribute.h++"

#define VS_SDM_MAX_SOUNDS 512

class vsSoundManager : public vsObject
{
protected:

    static vsSoundManager       *instance;

    vsSoundPipe                 *soundPipe;

    vsSoundSourceAttribute      *soundSources[VS_SDM_MAX_SOUNDS];
    int                         numSoundSources;

    vsSoundListenerAttribute    *soundListener;

                                vsSoundManager(); 

VS_INTERNAL:

    virtual     ~vsSoundManager();

    void        setSoundPipe(vsSoundPipe *pipe);
    void        removeSoundPipe(vsSoundPipe *pipe);
    void        addSoundSource(vsSoundSourceAttribute *attr);
    void        removeSoundSource(vsSoundSourceAttribute *attr);
    void        setSoundListener(vsSoundListenerAttribute *attr);
    void        removeSoundListener(vsSoundListenerAttribute *attr);

public:

    virtual const char       *getClassName();

    static vsSoundManager    *getInstance();

    void                     update();
}; 
#endif
