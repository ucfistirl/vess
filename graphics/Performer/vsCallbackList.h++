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
//    VESS Module:  vsCallbackList.h++
//
//    Description:  This class manages a list of callback functions used
//                  by a Performer process
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_CALLBACK_LIST_HPP
#define VS_CALLBACK_LIST_HPP

#include <Performer/pf/pfChannel.h>
#include <ulocks.h>
#include "vsObject.h++"

#ifndef VS_TRUE
#define VS_TRUE 1
#endif

struct vsCallbackNode
{
    vsCallbackNode *prev;
    vsCallbackNode *next;

    usema_t        *sema;

    pfChanFuncType func;
    void           *data;
};

class VS_GRAPHICS_DLL vsCallbackList : public vsObject
{
private:

    pfChannel       *channel;

    vsCallbackNode  *callbackList;
    vsCallbackNode  **callbackListAddress;

    usema_t         *listSemaphore;

VS_INTERNAL:

    static void     traverseCallbacks(pfChannel *chan, void *userData);
    static void     defaultCallback(pfChannel *chan, void *userData);

public:

                          vsCallbackList(pfChannel *callbackChannel);
    virtual               ~vsCallbackList();

    virtual const char    *getClassName();

    static void           *getData(void *userData);
    static void           removeCallbackNode(void *userData);
    static void           releaseData(void *userData);

    int                   acquireData(void *sharedMemory);

    void                  *prependCallback(pfChanFuncType callbackFunction,
                                           int sharedMemorySize);
    void                  prependCallback(pfChanFuncType callbackFunction,
                                          void *sharedMemory);

    void                  *appendCallback(pfChanFuncType callbackFunction,
                                          int sharedMemorySize);
    void                  appendCallback(pfChanFuncType callbackFunction,
                                         void *sharedMemory);

    void                  removeCallback(pfChanFuncType callbackFunction,
                                         void *sharedMemory);
};

#endif
