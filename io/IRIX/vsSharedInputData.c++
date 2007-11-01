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
//    VESS Module:  vsSharedInputData.c++
//
//    Description:  A class to handle exchange of vsMotionTracker data
//                  between concurrent processes via shared memory
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsSharedInputData.h++"

// ------------------------------------------------------------------------
// Acquire a shared memory segment and semaphore id for the specified 
// number of vsMotionTrackers.  If master is true, this process is 
// responsible for creating the necessary structures.
// ------------------------------------------------------------------------
vsSharedInputData::vsSharedInputData(key_t ipcKey, int trackerCount,
                                     int master)
{
    union semun zero;
    int         i;

    // Initialize variables
    server = master;
    numEntries = trackerCount;

    // Get (or create) shared memory segment
    if (server)
    {
        shmID = shmget(ipcKey, sizeof(vsInputData) * numEntries,
            0666 | IPC_CREAT);
    }
    else
    {
        shmID = shmget(ipcKey, sizeof(vsInputData) * numEntries, 0);

        // Keep trying until successful
        while (shmID == -1)
        {
            shmID = shmget(ipcKey, sizeof(vsInputData) * numEntries, 0);
        }
    }

    // Print an error if we fail to create the segment
    if (shmID == -1)
    {
        printf("vsSharedInputData::vsSharedInputData: "
            "Unable to create shared memory segment\n");
    }

    // Attach the data structure to the shared memory segment
    data = (vsInputData *)shmat(shmID, NULL, 0);

    // Check to see if the data segment we get back is valid
    if ((int)data == -1)
    {
        printf("vsSharedInputData::vsSharedInputData: "
            "Unable to attach to shared memory segment\n");
    }

    // Initialize the data structure
    for (i = 0; i < numEntries; i++)
    {
        data[i].vectData[0] = 0.0;
        data[i].vectData[1] = 0.0;
        data[i].vectData[2] = 0.0;
        data[i].vectData[3] = 0.0;
        data[i].quatData[0] = 0.0;
        data[i].quatData[1] = 0.0;
        data[i].quatData[2] = 0.0;
        data[i].quatData[3] = 1.0;
    }

    // Get (or create) the associated semaphores
    if (server)
    {
        semID = semget(ipcKey, numEntries, 0666 | IPC_CREAT);
    }
    else
    {
        semID = semget(ipcKey, numEntries, 0);
        while (semID == -1)
        {
            semID = semget(ipcKey, numEntries, 0);
        }
    }

    // Print an error if we fail to create the semaphores
    if (semID == -1)
    {
        printf("vsSharedInputData::vsSharedInputData: "
            "Unable to create semaphores\n");
    }

    // Initialize the semaphores to zero
    if (server)
    {
        zero.val = 0;

        for (i = 0; i < numEntries; i++)
        {
            semctl(semID, i, SETVAL, zero);
        }
    }
}

// ------------------------------------------------------------------------
// Detach from shared memory.  If this instance is the server, release the 
// shared memory segment and semaphores.
// ------------------------------------------------------------------------
vsSharedInputData::~vsSharedInputData()
{
    // Detach from shared memory
    shmdt((void *)data);
    
    // Clean up shared memory if we're the server process
    if (server)
    {
        // Remove the shared memory and semaphores
        shmctl(shmID, IPC_RMID, NULL);
        semctl(semID, numEntries, IPC_RMID, NULL);
    }
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsSharedInputData::getClassName()
{
    return "vsSharedInputData";
}

// ------------------------------------------------------------------------
// Stores the atVector's data in specified shared memory slot
// ------------------------------------------------------------------------
void vsSharedInputData::storeVectorData(int index, atVector vector)
{
    struct sembuf semOps[2];
    int           i;

    // Validate the index before proceeding
    if (index < numEntries)
    {
        // Set up the semaphore operations
        semOps[0].sem_num = index;
        semOps[0].sem_op = 0;
        semOps[0].sem_flg = 0;
        semOps[1].sem_num = index;
        semOps[1].sem_op = 1;
        semOps[1].sem_flg = 0;

        // Test-and-set the associated semaphore
        semop(semID, semOps, 2);

        // Copy the data
        for (i = 0; i < vector.getSize(); i++)
            data[index].vectData[i] = vector[i];

        // Release the semaphore
        semOps[0].sem_num = index;
        semOps[0].sem_op = -1;
        semOps[0].sem_flg = 0;
        semop(semID, semOps, 1);
    }
}


// ------------------------------------------------------------------------
// Stores the atQuat's data in specified shared memory slot
// ------------------------------------------------------------------------
void vsSharedInputData::storeQuatData(int index, atQuat quat)
{
    struct sembuf semOps[2];

    // Validate the index before proceeding
    if (index < numEntries)
    {
        // Set up the semaphore operations
        semOps[0].sem_num = index;
        semOps[0].sem_op = 0;
        semOps[0].sem_flg = 0;
        semOps[1].sem_num = index;
        semOps[1].sem_op = 1;
        semOps[1].sem_flg = 0;

        // Test-and-set the associated semaphore
        semop(semID, semOps, 2);

        // Copy the data
        data[index].quatData[AT_X] = quat[AT_X];
        data[index].quatData[AT_Y] = quat[AT_Y];
        data[index].quatData[AT_Z] = quat[AT_Z];
        data[index].quatData[AT_W] = quat[AT_W];

        // Release the semaphore
        semOps[0].sem_num = index;
        semOps[0].sem_op = -1;
        semOps[0].sem_flg = 0;
        semop(semID, semOps, 1);
    }
}

// ------------------------------------------------------------------------
// Retrieves the atVector's data from specified shared memory slot
// ------------------------------------------------------------------------
void vsSharedInputData::retrieveVectorData(int index, atVector *vector)
{
    struct sembuf semOps[2];
    int           i;

    // Validate the index before proceeding
    if (index < numEntries)
    {
        // Set up the semaphore operations
        semOps[0].sem_num = index;
        semOps[0].sem_op = 0;
        semOps[0].sem_flg = 0;
        semOps[1].sem_num = index;
        semOps[1].sem_op = 1;
        semOps[1].sem_flg = 0;

        // Test-and-set the associated semaphore
        semop(semID, semOps, 2);

        // Copy the data
        for (i = 0; i < vector->getSize(); i++)
            (*(vector))[i] = data[index].vectData[i];

        // Release the semaphore
        semOps[0].sem_num = index;
        semOps[0].sem_op = -1;
        semOps[0].sem_flg = 0;
        semop(semID, semOps, 1);
    }
}

// ------------------------------------------------------------------------
// Retrieves the atQuat's data from specified shared memory slot
// ------------------------------------------------------------------------
void vsSharedInputData::retrieveQuatData(int index, atQuat *quat)
{
    struct sembuf semOps[2];

    // Validate the index before proceeding
    if (index < numEntries)
    {
        // Set up the semaphore operations
        semOps[0].sem_num = index;
        semOps[0].sem_op = 0;
        semOps[0].sem_flg = 0;
        semOps[1].sem_num = index;
        semOps[1].sem_op = 1;
        semOps[1].sem_flg = 0;

        // Test-and-set the associated semaphore
        semop(semID, semOps, 2);

        // Copy the data
        (*(quat))[AT_X] = data[index].quatData[AT_X];
        (*(quat))[AT_Y] = data[index].quatData[AT_Y];
        (*(quat))[AT_Z] = data[index].quatData[AT_Z];
        (*(quat))[AT_W] = data[index].quatData[AT_W]; 

        // Release the semaphore
        semOps[0].sem_num = index;
        semOps[0].sem_op = -1;
        semOps[0].sem_flg = 0;
        semop(semID, semOps, 1);
    }
}

