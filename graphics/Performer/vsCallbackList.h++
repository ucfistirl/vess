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

struct vsCallbackNode
{
    vsCallbackNode    *prev;
    vsCallbackNode    *next;

    pfChanFuncType    func;
    void              *data;
    usema_t           *sema;
};

class VS_GRAPHICS_DLL vsCallbackList
{
private:

    pfChannel         *channel;

    vsCallbackNode    *callbackList;

    vsCallbackNode    **callbackListAddress;
    usema_t           *listSemaphore;

    int               *glClearMask;
    usema_t           *maskSemaphore;

    static void       traverseCallbacks(pfChannel *chan, void *userData);
    static void       drawCallback(pfChannel *chan, void *userData);

public:

                   vsCallbackList(pfChannel *callbackChannel);
    virtual        ~vsCallbackList();

    void           setGLClearMask(int clearMask);
    int            getGLClearMask();

    void           *prependCallback(pfChanFuncType callbackFunction,
                       int sharedMemorySize);
    void           prependCallback(pfChanFuncType callbackFunction,
                       void *sharedMemory);

    void           *appendCallback(pfChanFuncType callbackFunction,
                       int sharedMemorySize);
    void           appendCallback(pfChanFuncType callbackFunction,
                       void *sharedMemory);

    void           removeCallback(pfChanFuncType callbackFunction,
                       void *sharedMemory);

    static void    *getData(void *nodeData);
    static void    nodeRemove(void *nodeData);

    static bool    nodeAcquireData(void *nodeData);
    static void    nodeReleaseData(void *nodeData);

    bool           acquireData(void *sharedMemory);
    void           releaseData(void *sharedMemory);
};

#endif
