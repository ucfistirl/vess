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
//    VESS Module:  vsMPEGReader.h++
//
//    Description:  Class for reading an MPEG file and outputting frames
//                  of image data
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_MPEGREADER_HPP
#define VS_MPEGREADER_HPP

#include <stdio.h>
#include <inttypes.h>

#include "vsObject.h++"

extern "C"
{
#include "mpeg2dec/mpeg2.h"
#include "mpeg2dec/convert.h"
}

#define VS_MPEG_BUFFER_SIZE 4096

enum VS_GRAPHICS_DLL vsMPEGPlayMode
{
    VS_MPEG_PLAYING,
    VS_MPEG_STOPPED
};

class VS_GRAPHICS_DLL vsMPEGReader : public vsObject
{
private:

    mpeg2dec_t            *mpegDecoder;
    const mpeg2_info_t    *mpegInfo;

    FILE                  *mpegFile;
    uint8_t               mpegDataBuffer[VS_MPEG_BUFFER_SIZE];

    int                   imageWidth;
    int                   imageHeight;
    double                timePerFrame;

    unsigned char         *outputBuffer;
    double                currentFrameTime;
    double                totalFileTime;
    int                   playMode;

    void                  readNextFrame();
    void                  copyFrame();

public:

                     vsMPEGReader();
    virtual          ~vsMPEGReader();

    virtual const char    *getClassName();

    bool             openFile(char *filename);
    void             closeFile();

    int              getWidth();
    int              getHeight();
    int              getDataSize();
    double           getTimePerFrame();

    void             setOutputBuffer(unsigned char *dataOutputBuffer);
    unsigned char    *getOutputBuffer();

    void             advanceFrame();
    void             advanceTime(double seconds);
    double           getTotalTime();
    void             restart();
    int              getPlayMode();
};

#endif
