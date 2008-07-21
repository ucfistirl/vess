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

#include <pthread.h>
#include "vsUpdatable.h++"
#include "vsSoundPipe.h++"
#include "vsSoundSourceAttribute.h++"
#include "vsSoundListenerAttribute.h++"

#define VS_SDM_MAX_SOUNDS          512
#define VS_SDM_DEFAULT_VOICE_LIMIT 32
#define VS_SDM_MAX_VOICES          128
#define VS_SDM_SOURCE_THREAD_HZ    20

struct vsSoundSourceListItem
{
    vsSoundSourceAttribute *source;
    double                 gain;
};

class VESS_SYM vsSoundManager : public vsUpdatable
{
protected:

    static vsSoundManager          *instance;

    pthread_t                      sourceThread;
    pthread_mutex_t                sourceListMutex;
    bool                           sourceThreadDone;
    int                            threadDelay;

    vsSoundPipe                    *soundPipe;

    vsSoundSourceListItem          *soundSources[VS_SDM_MAX_SOUNDS];
    int                            numSoundSources;

    int                            voiceLimit;
    int                            hardwareVoiceLimit;
    int                            numVoices;
    int                            voices[VS_SDM_MAX_VOICES];

    vsSoundListenerAttribute       *soundListener;

                                   vsSoundManager();

    static void                    *sourceThreadFunc(void *arg);

    void                           sortSources();

VS_INTERNAL:

    virtual        ~vsSoundManager();
    static void    deleteInstance();

    void           setSoundPipe(vsSoundPipe *pipe);
    void           removeSoundPipe(vsSoundPipe *pipe);
    void           addSoundSource(vsSoundSourceAttribute *attr);
    void           removeSoundSource(vsSoundSourceAttribute *attr);
    void           setSoundListener(vsSoundListenerAttribute *attr);
    void           removeSoundListener(vsSoundListenerAttribute *attr);

public:

    virtual const char       *getClassName();

    static vsSoundManager    *getInstance();

    void                     setVoiceLimit(int newLimit);
    int                      getVoiceLimit();

    void                     setSourceUpdateRate(int hz);
    int                      getSourceUpdateRate();

    void                     update();
}; 
#endif
