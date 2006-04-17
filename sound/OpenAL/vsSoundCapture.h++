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
//    VESS Module:  vsSoundCapture.h++
//
//    Description:  This class manages a single connection to an OpenAL
//                  sound capture device. When a device is opened and unpaused,
//                  it will place data into a vsMultiQueue for processing.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_SOUND_CAPTURE_HPP
#define VS_SOUND_CAPTURE_HPP

#include "vsGlobals.h++"
#include "vsObject.h++"
#include "vsSoundBuffer.h++"
#include "vsSoundStream.h++"
#include "vsMultiQueue.h++"
#include <AL/al.h>
#include <AL/alc.h>
#include <pthread.h>
#include <stdlib.h>

#define VS_SOUND_CAPTURE_DEFAULT_FORMAT        VS_SBUF_FORMAT_MONO16
#define VS_SOUND_CAPTURE_DEFAULT_RATE          22050
#define VS_SOUND_CAPTURE_DEFAULT_CAPACITY      8820
#define VS_SOUND_CAPTURE_DEFAULT_PACKETSIZE    4410

#define VS_SOUND_CAPTURE_LOOP_SLEEP            5000

class VS_SOUND_DLL vsSoundCapture : public vsObject
{
protected:

    ALCdevice          *captureDevice;

    vsMultiQueue       *soundQueue;

    void               *soundPacketBuffer;
    int                packetSampleCount;
    int                queueSampleCapacity;

    int                captureFormat;
    int                captureRate;
    int                bytesPerSample;

    bool               deviceOpen;

    pthread_t          captureThread;
    pthread_mutex_t    signalMutex;
    bool               ceaseCapture;
    pthread_mutex_t    bufferMutex;
    bool               capturePaused;

    static void        *captureLoop(void *userData);

public:

                          vsSoundCapture();
                          vsSoundCapture(int format, int rate, int capacity,
                              int packetSize);
    virtual               ~vsSoundCapture();

    virtual const char    *getClassName();

    bool                  openDevice(char *device);
    void                  closeDevice();

    int                   getFormat();
    int                   getRate();
    int                   getBytesPerSample();

    bool                  isPaused();
    void                  startResume();
    void                  pause();

    vsMultiQueue          *getSoundQueue();
};

#endif

