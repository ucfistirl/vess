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
//    VESS Module:  vsSharedInputData.h++
//
//    Description:  A class to handle exchange of vsMotionTracker data
//                  between concurrent processes via shared memory
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_SHARED_INPUT_DATA_HPP
#define VS_SHARED_INPUT_DATA_HPP

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "vsVector.h++"
#include "vsQuat.h++"
#include "vsObject.h++"

#ifdef _XOPEN_SOURCE
union semun {
    int             val;
    struct semid_ds *buf;
    unsigned short  *array;
};
#endif

typedef struct
{
    double vectData[4];
    double quatData[4];
} vsInputData;

class vsSharedInputData : public vsObject
{
protected:

    vsInputData    *data;

    // Shared memory and semaphore ID's
    int            shmID;
    int            semID;

    // Number of DeviceData entries in shared memory segment
    // (also the number of semaphores)
    int            numEntries;

    // Indicates whether this process is the data server
    bool           server;

public:

                 vsSharedInputData(key_t key, int entryCount, bool master);
    virtual      ~vsSharedInputData();

    virtual const char    *getClassName();

    void         storeVectorData(int index, vsVector vector);
    void         storeQuatData(int index, vsQuat quat);

    void         retrieveVectorData(int index, vsVector *vector);
    void         retrieveQuatData(int index, vsQuat *quat);
};


#endif
