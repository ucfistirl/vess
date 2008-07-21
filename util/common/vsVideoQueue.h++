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
//    VESS Module:  vsVideoQueue.h++
//
//    Description:  Class for holding a series of images.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_VIDEO_QUEUE_HPP
#define VS_VIDEO_QUEUE_HPP

#include "vsObject.h++"
#include "vsMultiQueue.h++"
#include <pthread.h>

class VESS_SYM vsVideoQueue : public vsMultiQueue
{
protected:

    int         streamWidth;
    int         streamHeight;
    int         bytesPerPixel;
    int         bytesPerImage;

public:

                vsVideoQueue(int width, int height, int capacity);
    virtual     ~vsVideoQueue();

    virtual const char    *getClassName();

    int         getWidth();
    int         getHeight();
    int         getBytesPerPixel();
    int         getBytesPerImage();

    void        enqueue(char *image, double timestamp);
    bool        dequeue(char *image, double *timestamp, int id);
    bool        peek(char *image, double *timestamp, int id);
};

#endif

