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
//    VESS Module:  vsMultiQueue.h++
//
//    Description:  This class is designed to maintain a single container
//                  of data from which multiple sources must read, keeping
//                  track of which data has already been read by each
//                  source via a unique reference ID. This structure is
//                  implemented as a ring buffer.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_MULTI_QUEUE_HPP
#define VS_MULTI_QUEUE_HPP

#include "vsObject.h++"
#include <pthread.h>

struct VS_UTIL_DLL vsMQRefNode
{
    int            refID;
    int            bufferHead;

    vsMQRefNode    *next;
};

class VS_UTIL_DLL vsMultiQueue : public vsObject
{
protected:

    unsigned char      *ringBuffer;
    int                bufferCapacity;
    int                bufferTail;

    int                totalRefCount;
    vsMQRefNode        *referenceListHead;

    pthread_mutex_t    bufferMutex;
    pthread_mutex_t    listMutex;

    bool               readBuffer(void *data, int skip, int size, int id,
                           bool dequeue);

public:

                     vsMultiQueue();
                     vsMultiQueue(int capacity);
    virtual          ~vsMultiQueue();

    virtual const char    *getClassName();

    bool             setCapacity(int capacity);

    int              addReference();
    void             yieldReference(int id);

    void             enqueue(void *data, int size);
    bool             dequeue(void *data, int size, int id);
    bool             peek(void *data, int size, int id);

    void             clear(int id);

    int              getLength(int id);
};

#endif

