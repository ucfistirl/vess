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
//    VESS Module:  vsMovieReader.h++
//
//    Description:  Class for reading a video file and outputting frames
//                  of image data
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_MOVIEREADER_HPP
#define VS_MOVIEREADER_HPP


#include "vsObject.h++"
#include "vsSoundStream.h++"
#include "vsTimer.h++"
#include <stdio.h>
#include <pthread.h>

extern "C"
{
#include "ffmpeg/avcodec.h"
#include "ffmpeg/avformat.h"
}

#define VS_MOVIE_PACKET_QUEUE_SIZE 8

enum  vsMoviePlayMode
{
    VS_MOVIE_PLAYING,
    VS_MOVIE_STOPPED,
    VS_MOVIE_EOF,
    VS_MOVIE_QUIT
};

struct VS_ENVIRONMENT_DLL vsMoviePacketQueue
{
    AVPacketList          *head;
    AVPacketList          *tail;
    int                   packetCount;
};

class VS_ENVIRONMENT_DLL vsMovieReader : public vsObject
{
private:

    AVFormatContext       *movieFile;

    vsMoviePacketQueue    *videoQueue;
    vsMoviePacketQueue    *audioQueue;

    AVCodecContext        *videoCodecContext;
    AVStream              *videoStream;
    AVCodec               *videoCodec;
    int                   videoStreamIndex;
    AVFrame               *videoFrame;
    AVPicture             *rgbFrame;

    AVCodecContext        *audioCodecContext;
    AVStream              *audioStream;
    AVCodec               *audioCodec;
    int                   audioStreamIndex;
    int                   sampleRate, sampleSize, channelCount;
    int                   samplesPerFrame;
    AVFrame               *audioFrame;
    unsigned char         audioBuffer[AVCODEC_MAX_AUDIO_FRAME_SIZE * 8];
    int                   audioBufferSize;
    vsSoundStream         *soundStream;

    int                   imageWidth;
    int                   imageHeight;
    double                timePerFrame;

    unsigned char         *outputBuffer;
    double                lastTimeStamp;
    double                lastFrameInterval;
    double                currentFrameTime;
    double                totalFileTime;
    int                   playMode;
vsTimer *playTimer;

    void                  readNextFrame();
    void                  copyFrame();

    void                  enqueuePacket(vsMoviePacketQueue *queue, 
                                        AVPacket *packet);
    bool                  dequeuePacket(vsMoviePacketQueue *queue,
                                         AVPacket *packet);
    void                  flushQueue(vsMoviePacketQueue *queue);

    // Thread functions to handle reading from the file and decoding and
    // queueing audio, respectively
    static void           *fileThreadFunc(void *readerObject);
    static void           *audioThreadFunc(void *readerObject);
    pthread_t             fileThread;
    pthread_t             audioThread;
    pthread_mutex_t       fileMutex;
    pthread_mutex_t       queueMutex;
    pthread_mutex_t       audioMutex;

public:

                     vsMovieReader();
    virtual          ~vsMovieReader();

    virtual const char    *getClassName();

    bool             openFile(char *filename);
    void             closeFile();

    int              getWidth();
    int              getHeight();
    int              getDataSize();
    double           getTimePerFrame();

    void             setVideoBuffer(unsigned char *dataOutputBuffer);
    unsigned char    *getVideoBuffer();

    vsSoundStream    *getSoundStream();

    void             advanceFrame();
    void             advanceTime(double seconds);
    double           getTotalTime();
    void             restart();
    int              getPlayMode();
};

#endif
