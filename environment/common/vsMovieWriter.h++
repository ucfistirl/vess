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
//    VESS Module:  vsMovieWriter.h++
//
//    Description:  
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_MOVIE_WRITER_HPP
#define VS_MOVIE_WRITER_HPP

#include "vsMultiQueue.h++"
#include "vsObject.h++"
#include "vsVideoQueue.h++"
#include "vsVideoStream.h++"
#include <stdio.h>
#include <pthread.h>

extern "C"
{
#include "ffmpeg/avcodec.h"
#include "ffmpeg/avformat.h"
}

#define VS_MOVIE_WRITER_DEFAULT_WIDTH       640
#define VS_MOVIE_WRITER_DEFAULT_HEIGHT      480
#define VS_MOVIE_WRITER_DEFAULT_FRAMERATE    30
#define VS_MOVIE_WRITER_DEFAULT_BITRATE  400000

enum VS_ENVIRONMENT_DLL vsVideoFrameSize
{
    VS_VIDEO_SIZE_320X240,
    VS_VIDEO_SIZE_400X300,
    VS_VIDEO_SIZE_640X480,
    VS_VIDEO_SIZE_800X600,
    VS_VIDEO_SIZE_1024X768,
    VS_VIDEO_SIZE_1280X960,
    VS_VIDEO_SIZE_1600X1200
};

enum VS_ENVIRONMENT_DLL vsVideoTimingMode
{
    VS_MW_TIMING_FIXED,
    VS_MW_TIMING_REAL
};

class VS_ENVIRONMENT_DLL vsMovieWriter : public vsObject
{
private:

    AVFormatContext       *movieContext;
    AVOutputFormat        *movieFormat;

    vsVideoQueue          *videoQueue;
    int                   videoReferenceID;
    vsVideoTimingMode     videoTimingMode;
    AVStream              *vStream;
    int                   vStreamIndex;
    AVCodecContext        *vCodecContext;
    AVCodec               *vCodec;
    AVFrame               *rgbFrame;
    AVFrame               *outputFrame;
    uint8_t               *vOutputBuffer;
    int                   vOutputBufferSize;

    vsMultiQueue          *audioQueue;
    int                   audioReferenceID;
    AVStream              *aStream;
    int                   aStreamIndex;
    AVCodecContext        *aCodecContext;
    AVCodec               *aCodec;
    uint8_t               *rawSamples;
    uint8_t               *nullSamples;
    int                   rawSampleSize;
    uint8_t               *aOutputBuffer;
    int                   aOutputBufferSize;

    pthread_t             writeThread;
    pthread_mutex_t       signalMutex;
    bool                  ceaseSignal;
    pthread_mutex_t       writeMutex;
    bool                  writePaused;

    bool                  fileOpen;
    double                videoElapsed;
    double                audioElapsed;
    double                curVideoTimestamp;

    AVFrame               *allocFrame(int format, int width, int height);

    static void           *writeLoopFixed(void *userData);
    static void           *writeLoopReal(void *userData);
    void                  writeFrame();
    void                  writeSamples(void *samples);

public:

                         vsMovieWriter(const char *format);
    virtual              ~vsMovieWriter();

    virtual const char    *getClassName();

    void                 addVideoQueue(vsVideoQueue *queue);
    void                 addAudioQueue(vsMultiQueue *queue);

    void                 setFrameSize(vsVideoFrameSize size);

    int                  getWidth();
    int                  getHeight();
    int                  getDataSize();
    double               getTimePerFrame();

/*
  codec
  bit rate
  frame width
  frame heigth
  frame rate

  intra frame rate (gop_size?)
  B frames?
  macroblocks
*/

    void                 setTimingMode(vsVideoTimingMode mode);
    vsVideoTimingMode    getTimingMode();

    bool                 openFile(char *filename);
    void                 closeFile();

    bool                 isPaused();
    void                 startResume();
    void                 pause();

    double               getTimeElapsed();
};

#endif

