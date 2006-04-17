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
//    VESS Module:  vsMultiQueue.c++
//
//    Description:  
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsMultiQueue.h++"
#include <stdlib.h>
#include <string.h>

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsMultiQueue::vsMultiQueue()
{
    // Store the queue capacity and create the ring buffer.
    bufferCapacity = 0;
    bufferTail = 0;
    ringBuffer = NULL;

    // Begin with zero references.
    totalRefCount = 0;
    referenceListHead = NULL;

    // Initialize mutexes to protect the buffer and the list.
    pthread_mutex_init(&bufferMutex, NULL);
    pthread_mutex_init(&listMutex, NULL);
}

// ------------------------------------------------------------------------
// Constructor
// This version of the constructor initializes the buffer to some specified
// capacity.
// ------------------------------------------------------------------------
vsMultiQueue::vsMultiQueue(int capacity)
{
    // Store the queue capacity and create the ring buffer.
    bufferCapacity = capacity;
    bufferTail = 0;
    ringBuffer = (unsigned char *)calloc(bufferCapacity, 1);

    // Begin with zero references.
    totalRefCount = 0;
    referenceListHead = NULL;

    // Initialize mutexes to protect the buffer and the list.
    pthread_mutex_init(&bufferMutex, NULL);
    pthread_mutex_init(&listMutex, NULL);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsMultiQueue::~vsMultiQueue()
{
    vsMQRefNode *traversalNode;

    // Acquire exclusive access to the reference list.
    pthread_mutex_lock(&listMutex);

    // Free all of the data stored in the list.
    traversalNode = referenceListHead;
    while (traversalNode)
    {
        // Scoot the list head forward to the next node.
        referenceListHead = traversalNode->next;

        // Delete the current node.
        free(traversalNode);

        // Scoot the traversal forward to the new head of the list.
        traversalNode = referenceListHead;
    }

    // Ensure that all calls accessing the buffer have ceased by locking it.
    pthread_mutex_lock(&bufferMutex);

    // Give up access to the list. Any function calls from this point forward
    // should report that the reference ID was invalid, so no functions should
    // attempt to access the buffer from this point on.
    pthread_mutex_unlock(&listMutex);

    // Free the buffer.
    free(ringBuffer);

    // Free the mutex semaphores.
    pthread_mutex_destroy(&listMutex);
    pthread_mutex_destroy(&bufferMutex);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsMultiQueue::getClassName()
{
    return "vsMultiQueue";
}

// ------------------------------------------------------------------------
// This method attempts to set the size of the ring buffer to the new
// capacity value, maintaining the data itself and the validity of pointers
// to that data.
// ------------------------------------------------------------------------
bool vsMultiQueue::setCapacity(int capacity)
{
fprintf(stdout, "vsMultiQueue::setCapacity: Called\n");
    unsigned char *newBuffer;
    vsMQRefNode *traversalNode;
    int bufferNeeded;
    int bufferRemainder;

    // Acquire access to both the buffer and the list immediately, as this call
    // requires heavy modification of both.
    pthread_mutex_lock(&listMutex);
    pthread_mutex_lock(&bufferMutex);

    // If the size is the same, return true without modifying anything.
    if (capacity == bufferCapacity)
    {
        // Free the list and the buffer and indicate success.
        pthread_mutex_unlock(&listMutex);
        pthread_mutex_unlock(&bufferMutex);
        return true;
    }
    else if (bufferCapacity == 0)
    {
        // If the old buffer didn't exist, attempt to allocate a new buffer.
        ringBuffer = (unsigned char *)malloc(capacity);
        if (ringBuffer)
        {
            // Update the buffer capacity with the new value.
            bufferCapacity = capacity;

            // Free the list and the buffer and indicate success.
            pthread_mutex_unlock(&listMutex);
            pthread_mutex_unlock(&bufferMutex);
            return true;
        }
        else
        {
            // Free the list and the buffer and indicate failure.
            pthread_mutex_unlock(&listMutex);
            pthread_mutex_unlock(&bufferMutex);
            return false;
        }
    }
    else if (capacity == 0)
    {
        // The old buffer contained data but the new buffer does not. Free the
        // memory used by the old buffer.
        free(ringBuffer);
        ringBuffer = NULL;

        // Set some values to prevent unexpected behavior.
        bufferTail = 0;
        bufferCapacity = 0;

        // Free the list and the buffer and indicate success.
        pthread_mutex_unlock(&listMutex);
        pthread_mutex_unlock(&bufferMutex);
        return true;
    }
    else
    {
        // Attempt to allocate the new memory block.
        newBuffer = (unsigned char *)malloc(capacity);
        if (newBuffer)
        {
            // Traverse the list, determining the required quantity of data to
            // be copied from the old buffer into the new one.
            traversalNode = referenceListHead;
            while (traversalNode)
            {
                // Calculate the amount of data that this read head still needs
                // in order to function.
                if (capacity < bufferCapacity)
                {
                    if (bufferTail - traversalNode->bufferHead > capacity)
                    {
                        // This head has lagged behind and needs data that the
                        // new buffer will not be able to store. Simply keep as
                        // much data as possible.
                        bufferNeeded = capacity;
                        traversalNode = NULL;
                    }
                    else
                    {
                        // Keep the new value if it is greater than the old.
                        if (bufferNeeded <
                            bufferTail - traversalNode->bufferHead)
                        {
                            bufferNeeded = 
                                bufferTail - traversalNode->bufferHead;
                        }

                        // Move to the next node in the list.
                        traversalNode = traversalNode->next;
                    }
                }
                else if (bufferTail - traversalNode->bufferHead >
                    bufferCapacity)
                {
                    // This head has lagged behind and needs data that the old
                    // buffer wasn't able to store. Simply keep as much data as
                    // the old buffer had stored.
                    bufferNeeded = bufferCapacity;
                    traversalNode = NULL;
                }
                else
                {
                    // The new buffer is greater in size than the old buffer,
                    // and the read head doesn't need any more data than the
                    // old buffer had stored, so the data remaining for this
                    // reference should be used if it is greater than the
                    // existing data needed.
                    if (bufferNeeded < bufferTail - traversalNode->bufferHead)
                    {
                        bufferNeeded = bufferTail - traversalNode->bufferHead;
                    }

                    // Move to the next node in the list.
                    traversalNode = traversalNode->next;
                }
            }

            // Traverse the list again, updating the read heads to their new
            // values based on the amount of data to be copied.
            traversalNode = referenceListHead;
            while (traversalNode)
            {
                // Update the value based on the new buffer capacity. If the
                // amount of data previously stored exceeds the amount of data
                // that may now be stored, keep as much data as possible.
                if (bufferTail - traversalNode->bufferHead > bufferNeeded)
                {
                    traversalNode->bufferHead = 0;
                }
                else
                {
                    traversalNode->bufferHead = bufferNeeded -
                        (bufferTail - traversalNode->bufferHead);
                }

                // Move to the next node in the list.
                traversalNode = traversalNode->next;
            }

            // Unlock the list mutex, as the list has been updated and
            // exclusive access to it is no longer necessary.
            pthread_mutex_unlock(&listMutex);

            // Now handle rearranging of memory such that the buffer state is
            // preserved.
            if (bufferTail % bufferCapacity >= bufferNeeded)
            {
                // The amount of data needed fits in the space between the
                // start of the buffer and the current tail location. This
                // means the significant 'bufferNeeded' bytes of the buffer are
                // contiguous and do not wrap around the ring. Copy the data
                // from where it began in the old buffer to the beginning of
                // the new buffer.
                memcpy(newBuffer, &(ringBuffer[(bufferTail % bufferCapacity) -
                    bufferNeeded]), bufferNeeded);
            }
            else
            {
                // Figure out the location into the old buffer where the first
                // data to be preserved resides. This is the address into the
                // buffer where the first chunk of memory resides before the
                // buffer wraps around.
                bufferRemainder = (bufferTail + bufferCapacity -
                    bufferNeeded) % bufferCapacity;

                // Copy the bytes from the old head to the previous capacity
                // into the first part of the new buffer.
                memcpy(newBuffer, &(ringBuffer[bufferRemainder]),
                    bufferCapacity - bufferRemainder);

                // Copy the rest of the bytes that wrapped around from the old
                // buffer into the new one.
                memcpy(&(newBuffer[bufferCapacity - bufferRemainder]),
                    ringBuffer, bufferNeeded - (bufferCapacity -
                    bufferRemainder));
            }

            // The new buffer tail is simply the amount of data that needed to
            // be stored from the old buffer, capped at the amount of data that
            // is valid. This value has been calculated already.
            bufferTail = bufferNeeded;

            // Store the new buffer capacity now that the list has been
            // rearranged.
            bufferCapacity = capacity;

            // Free the memory of the old buffer and set the pointer to the
            // new one.
            if (ringBuffer != NULL)
                free(ringBuffer);
            ringBuffer = newBuffer;

            // Release the buffer semaphore and return true indicating success.
            pthread_mutex_unlock(&bufferMutex);
            return true;
        }
        else
        {
            // Free the semaphores and return false, indicating a failure to
            // allocate new buffer space.
            pthread_mutex_unlock(&listMutex);
            pthread_mutex_unlock(&bufferMutex);
            return false;
        }
    }
}

// ------------------------------------------------------------------------
// This method adds a reference to the data in the queue. It initializes
// this reference to the maximum available amount of data in the queue,
// even if that particular data has expired.
// ------------------------------------------------------------------------
int vsMultiQueue::addReference()
{
    vsMQRefNode *addNode;
    int returnID;

    // Create a new reference node.
    addNode = (vsMQRefNode *)malloc(sizeof(vsMQRefNode));

    // Add a new reference and assign it the new ID.
    totalRefCount++;
    addNode->refID = totalRefCount;
    returnID = addNode->refID;

    // This current reference should have the maximum data available, so begin
    // its head at 0. Future calls requesting the data will bump the head
    // forward as necessary.
    addNode->bufferHead = 0;

    // Acquire exclusive access to the list so the node may be inserted.
    pthread_mutex_lock(&listMutex);

    // Add the node arbitrarily to the head of the list.
    addNode->next = referenceListHead;
    referenceListHead = addNode;

    // Yield access to the list.
    pthread_mutex_unlock(&listMutex);

    // Return the new ID.
    return returnID;
}

// ------------------------------------------------------------------------
// This method yields the reference matching the provided ID.
// ------------------------------------------------------------------------
void vsMultiQueue::yieldReference(int id)
{
    vsMQRefNode *traversalNode;
    vsMQRefNode *removalNode;

    // Acquire exclusive access to the list so the node may be inserted.
    pthread_mutex_lock(&listMutex);

    // Find the node with the appropriate ID in the list and delete it.
    if (referenceListHead)
    {
        // A special check is made for the first node.
        traversalNode = referenceListHead;
        if (referenceListHead->refID == id)
        {
            // Free the node and scoot the list head backwards.
            referenceListHead = referenceListHead->next;
            free(traversalNode);
        }
        else
        {
            // Loop until the NEXT node is the one to be deleted.
            while ((traversalNode->next) && (traversalNode->next->refID != id))
            {
                traversalNode = traversalNode->next;
            }

            // See which end condition occurred.
            if (traversalNode->next)
            {
                // The next node still exists, so the other condition must have
                // caused the loop to fail. Remove the next node.
                removalNode = traversalNode->next;
                traversalNode->next = removalNode->next;
                free(removalNode);
            }
        }
    }

    // Yield access to the list.
    pthread_mutex_unlock(&listMutex);
}

// ------------------------------------------------------------------------
// This method will copy 'size' bytes of memory located at 'data' into the
// ring buffer, pushing the tail back. This may require one or two calls to
// memcpy. The new data will be copied even if the space between the tail
// and one or more of the heads cannot hold it, and will fail only if the
// total capacity of the buffer is not sufficient to hold the provided
// data.
// ------------------------------------------------------------------------
void vsMultiQueue::enqueue(void *data, int size)
{
    int tailSpace;

    // Acquire exclusive access to the buffer so the data may be stored.
    pthread_mutex_lock(&bufferMutex);

    // First make sure the data will fit.
    if (size <= bufferCapacity)
    {
        // Calculate the space in between the tail and the end of the buffer in
        // memory.
        tailSpace = bufferCapacity - (bufferTail % bufferCapacity);

        // See if the space needed is bigger than the space between the current
        // tail and the end of the buffer.
        if (size > tailSpace)
        {
            // Fill in the empty space at the end of the buffer first.
            memcpy(&(ringBuffer[bufferTail % bufferCapacity]), data, tailSpace);

            // Now fill the front of the buffer with the rest of the data.
            memcpy(ringBuffer, &(((char *)data)[tailSpace]), size - tailSpace);
        }
        else
        {
            // Fill in the empty space at the end of the buffer.
            memcpy(&(ringBuffer[bufferTail % bufferCapacity]), data, size);
        }

        // Scoot the tail of the list back by the size of the data just added.
        bufferTail += size;
    }

    // Yield exclusive access to the buffer.
    pthread_mutex_unlock(&bufferMutex);
}

// ------------------------------------------------------------------------
// This method will fill 'data' with the first 'size' bytes of the ring
// buffer starting at the head attributed to reference ID 'id'. The call
// will fail if the reference ID is invalid or if there are not at least
// 'size' bytes of data between the head and the tail. On success, this
// method will move the appropriate head pointer.
// ------------------------------------------------------------------------
bool vsMultiQueue::dequeue(void *data, int size, int id)
{
    return readBuffer(data, 0, size, id, true);
}

// ------------------------------------------------------------------------
// This method will fill 'data' with the first 'size' bytes of the ring
// buffer starting at the head attributed to reference ID 'id'. The call
// will fail if the reference ID is invalid or if there are not at least
// 'size' bytes of data between the head and the tail. Even if successful,
// this method will not move the appropriate head pointer.
// ------------------------------------------------------------------------
bool vsMultiQueue::peek(void *data, int size, int id)
{
    return readBuffer(data, 0, size, id, false);
}

// ------------------------------------------------------------------------
// This method 'clears' all of the data stored for the provided reference
// ID by moving the head forward to the common tail.
// ------------------------------------------------------------------------
void vsMultiQueue::clear(int id)
{
    vsMQRefNode *traversalNode;

    // Acquire exclusive access to the list for reference ID lookup.
    pthread_mutex_lock(&listMutex);

    // Look up the reference ID from the list, starting at the head.
    traversalNode = referenceListHead;
    while (traversalNode)
    {
        if (traversalNode->refID == id)
        {
            // Overwrite the sentinel value and break from the loop.
            traversalNode->bufferHead = bufferTail;
            traversalNode = NULL;
        }
        else
        {
            traversalNode = traversalNode->next;
        }
    }

    // Yield list access before returning.
    pthread_mutex_unlock(&listMutex);
}

// ------------------------------------------------------------------------
// Private function
// This method will fill 'data' with the first 'size' bytes of the ring
// buffer starting at the head attributed to reference ID 'id'. The call
// will fail if the reference ID is invalid or if there are not at least
// 'size' bytes of data between the head and the tail. The fourth argument
// determines whether, if the read is successful, the head will be updated.
// ------------------------------------------------------------------------
bool vsMultiQueue::readBuffer(void *data, int skip, int size, int id,
    bool dequeue)
{
    vsMQRefNode *traversalNode;
    vsMQRefNode *referenceNode;
    int bufferHeadValue;
    int bufferHeadSpace;

    // Acquire exclusive access to the list for reference ID lookup.
    pthread_mutex_lock(&listMutex);

    // Initialize the reference node to NULL as a sentinel.
    referenceNode = NULL;

    // Look up the reference ID from the list, starting at the head.
    traversalNode = referenceListHead;
    while (traversalNode)
    {
        if (traversalNode->refID == id)
        {
            // Overwrite the sentinel value and break from the loop.
            referenceNode = traversalNode;
            traversalNode = NULL;
        }
        else
        {
            traversalNode = traversalNode->next;
        }
    }

    // See if the lookup operation failed.
    if (referenceNode == NULL)
    {
        // Yield list access before returning false to indicate that the
        // reference ID could not be found.
        pthread_mutex_unlock(&listMutex);

        // Print a message indicating the failure and return.
        fprintf(stderr, "vsMultiQueue::readBuffer: Invalid reference ID! "
            "(%d)\n", id);
        return false;
    }

    // Sieze access to the buffer before giving up access to the list. This
    // checkpoint operation is safe because and only because the list will
    // never be siezed by a thread already claiming access to the buffer. The
    // checkpoint operation is needed to ensure that threads that were able to
    // successfully look up the head are always allowed access to the buffer
    // before any modifications by other threads can be made.
    pthread_mutex_lock(&bufferMutex);

    // Fix the position of the head if it had previously been left behind.
    if (bufferTail - referenceNode->bufferHead > bufferCapacity)
    {
        referenceNode->bufferHead = bufferTail - bufferCapacity;
    }

    // See if more data is requested than can be returned.
    if (skip + size > bufferTail - referenceNode->bufferHead)
    {
        // Yield list access before returning false to indicate that the
        // requested amount of data could not be returned.
        pthread_mutex_unlock(&listMutex);
        pthread_mutex_unlock(&bufferMutex);
        return false;
    }
    else
    {
        // Store the buffer head value at the start of the operation.
        bufferHeadValue = referenceNode->bufferHead + skip;

        // See if this is a peek or a dequeue operation.
        if (dequeue)
        {
            // Move the head of the reference node backwards. It is safe to
            // perform this operation because no head position checks are made
            // unless the buffer is locked, and the buffer will not be unlocked
            // by this function until the data has actually been read.
            referenceNode->bufferHead += size + skip;
        }

        // Release control of the list.
        pthread_mutex_unlock(&listMutex);
    }

    // If the data pointer is null, then no memory needs to be copied.
    if (data != NULL)
    {
        // Copy the data from the buffer. First check how much data is
        // available between the current head and the end of the buffer.
        bufferHeadSpace = bufferCapacity - (bufferHeadValue % bufferCapacity);

        // See if more data is requested than can be fetched in one operation.
        if (size > bufferHeadSpace)
        {
            // Copy as much data as will fit in the first call.
            memcpy(data, &(ringBuffer[bufferHeadValue % bufferCapacity]),
                bufferHeadSpace);

            // Copy the rest of the data from the beginning of the buffer.
            memcpy(&(((char *)data)[bufferHeadSpace]), (void *)(ringBuffer),
                size - bufferHeadSpace);
        }
        else
        {
            // Copy the appropriate amount of data in one call.
            memcpy(data, &(ringBuffer[bufferHeadValue % bufferCapacity]), size);
        }
    }

    // Yield exclusive access to the buffer.
    pthread_mutex_unlock(&bufferMutex);

    // Return true to indicate that the operation succeeded.
    return true;
}
