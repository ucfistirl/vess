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
        while (shmID == -1)
        {
            shmID = shmget(ipcKey, sizeof(vsInputData) * numEntries, 0);
        }
    }

    if (shmID == -1)
    {
        printf("vsSharedInputData::vsSharedInputData: "
            "Unable to create shared memory segment\n");
    }

    // Attach the data structure to the shared memory segment
    data = (vsInputData *)shmat(shmID, NULL, 0);

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

    if ((int)data == -1)
    {
        printf("vsSharedInputData::vsSharedInputData: "
            "Unable to attach to shared memory segment\n");
    }

    // Get semaphores
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

    if (semID == -1)
    {
        printf("vsSharedInputData::vsSharedInputData: "
            "Unable to create semaphores\n");
    }

    // Initialize the semaphores
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
    shmdt((void *)data);
    
    if (server)
    {
        shmctl(shmID, IPC_RMID, NULL);
        semctl(semID, numEntries, IPC_RMID, NULL);
    }
}

// ------------------------------------------------------------------------
// Stores the vsVector's data in specified shared memory slot
// ------------------------------------------------------------------------
void vsSharedInputData::storeVectorData(int index, vsVector vector)
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
        semop(semID, semOps, 1);

        // Copy the data
        for (i = 0; i < vector.getSize(); i++)
            data[index].vectData[i] = vector[i];
    }
}


// ------------------------------------------------------------------------
// Stores the vsQuat's data in specified shared memory slot
// ------------------------------------------------------------------------
void vsSharedInputData::storeQuatData(int index, vsQuat quat)
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
        semop(semID, semOps, 1);

        // Copy the data
        data[index].quatData[VS_X] = quat[VS_X];
        data[index].quatData[VS_Y] = quat[VS_Y];
        data[index].quatData[VS_Z] = quat[VS_Z];
        data[index].quatData[VS_W] = quat[VS_W];
    }
}

// ------------------------------------------------------------------------
// Retrieves the vsVector's data from specified shared memory slot
// ------------------------------------------------------------------------
void vsSharedInputData::retrieveVectorData(int index, vsVector *vector)
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
        semop(semID, semOps, 1);

        // Copy the data
        for (i = 0; i < vector->getSize(); i++)
            (*(vector))[i] = data[index].vectData[i];
    }
}

// ------------------------------------------------------------------------
// Retrieves the vsQuat's data from specified shared memory slot
// ------------------------------------------------------------------------
void vsSharedInputData::retrieveQuatData(int index, vsQuat *quat)
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
        semop(semID, semOps, 1);

        // Copy the data
        (*(quat))[VS_X] = data[index].quatData[VS_X];
        (*(quat))[VS_Y] = data[index].quatData[VS_Y];
        (*(quat))[VS_Z] = data[index].quatData[VS_Z];
        (*(quat))[VS_W] = data[index].quatData[VS_W]; 
    }
}

