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
//    VESS Module:  vsGrowableArray.c++
//
//    Description:  Utility class that implements a dynamically-sized
//                  array of pointers
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsGrowableArray.h++"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vsGlobals.h++"

// ------------------------------------------------------------------------
// Constructor - Initializes the array's internal storage
// ------------------------------------------------------------------------
vsGrowableArray::vsGrowableArray(int initialSize, int sizeIncrement)
{
    int startSize;

    // Bounds check on initial size
    if (initialSize < 0)
    {
        printf("vsGrowableArray::vsGrowableArray: Invalid initial size, size = %d is negative\n",initialSize);
        startSize = 0;
    }
    else
        startSize = initialSize;

    // Store the grow step size
    stepSize = sizeIncrement;

    // Maximum size defaults to 32767; clamp the start size to that value
    maxSize = 32767;
    if (startSize > maxSize)
        maxSize = startSize;
    
    // Initialize the 'error result' pointer
    nowhere = NULL;

    // Allocate memory here
    storage = NULL;
    currentSize = 0;
    setSize(startSize);
}

// ------------------------------------------------------------------------
// Destructor - Destroys the array's internal storage
// ------------------------------------------------------------------------
vsGrowableArray::~vsGrowableArray()
{
    // Free the array storage, if it exists
    if (storage)
        free(storage);
}

// ------------------------------------------------------------------------
// Sets the current size of the array
// ------------------------------------------------------------------------
void vsGrowableArray::setSize(int newSize)
{
    // Bounds check
    if (newSize < 0)
    {
        printf("vsGrowableArray::setSize: Invalid size = %d is negative\n",newSize);
        return;
    }

    // If there's no size change, there's no work to do
    if (newSize == currentSize)
        return;

    // Figure out what we need to do based on the requested size
    if (newSize > 0)
    {
        // Check the current size to figure out if we need to make a new
        // storage area or just resize the current one
        if (currentSize > 0)
        {
            // Modify
            storage = (void **)(realloc(storage, newSize * sizeof(void *)));
        }
        else
        {
            // Create
            storage = (void **)(malloc(newSize * sizeof(void *)));
        }

        // Check for failed allocation
        if (storage)
        {
            // If the list has grown, clear the newly-allocated memory
            if (newSize > currentSize)
            {
                memset(&storage[currentSize], 0, 
                    sizeof(void *) * (newSize - currentSize));
            }

            // Set the size
            currentSize = newSize;
        }
        else
        {
            // Set size to zero and signal and error
            currentSize = 0;
            printf("vsGrowableArray::setSize: Unable to allocate memory for "
                "internal array\n");
        }
    }
    else if (currentSize > 0)
    {
        // Destroy
        free(storage);

        // Set size to zero and NULL the storage pointer
        currentSize = 0;
        storage = NULL;
    }
}

// ------------------------------------------------------------------------
// Retrieves the current size of the array
// ------------------------------------------------------------------------
int vsGrowableArray::getSize()
{
    return currentSize;
}

// ------------------------------------------------------------------------
// Sets the size increment for the array. The array is increased by this
// size when an attempted access goes beyone the current size.
// ------------------------------------------------------------------------
void vsGrowableArray::setSizeIncrement(int sizeIncrement)
{
    // Bounds check
    if (sizeIncrement <= 0)
    {
        printf("vsGrowableArray::setSizeIncrement: Invalid size increment = %d is not positive\n",sizeIncrement);
        return;
    }

    // Set the grow step size
    stepSize = sizeIncrement;
}

// ------------------------------------------------------------------------
// Retrieves the size increment for this array
// ------------------------------------------------------------------------
int vsGrowableArray::getSizeIncrement()
{
    return stepSize;
}

// ------------------------------------------------------------------------
// Sets the maximum size for this array. Attempts to access beyond this
// index will fail. If the maximum is less than the current size of the
// array, the array will be reduced in size to match the maximum.
// ------------------------------------------------------------------------
void vsGrowableArray::setMaxSize(int newMax)
{
    // Bounds check
    if (newMax <= 0)
    {
        printf("vsGrowableArray::setMaxSize: Invalid maximum size = %d is not positive\n",newMax);
        return;
    }

    // Set the maximum size
    maxSize = newMax;
    
    // If the maximum size is less than the current size, then reduce the
    // current size to match the new boundary
    if (maxSize < currentSize)
        setSize(newMax);
}

// ------------------------------------------------------------------------
// Retrieves the maximum size for this array
// ------------------------------------------------------------------------
int vsGrowableArray::getMaxSize()
{
    return maxSize;
}

// ------------------------------------------------------------------------
// Private function
// Determines if an access of the specified index is possible. Determines
// if the index is valid. Also checks if the array is allocated at the
// desired index, and grows the array to cover the index if needed.
// ------------------------------------------------------------------------
inline bool vsGrowableArray::access(int index)
{
    // Array bounds check
    if ((index < 0) || (index >= maxSize))
    {
        printf("vsGrowableArray::access: Array index out of bounds, index = %d\n",index);
        return false;
    }

    // Allocated space check
    if (index >= currentSize)
    {
        // If the array is set to not grow (stepSize == 0), we won't
        // be able to access this index
        if (stepSize <= 0)
        {
            printf("vsGrowableArray::access: Array index out of bounds, index = %d\n",index);
            return false;
        }

        // Determine how big the new array should be by repeatedly
        // incrementing the current size by the step size until the
        // size is equal to or larger than the requested array position
        int newSize;
        newSize = currentSize;
        while (newSize <= index)
        {
            newSize += stepSize;
            if (newSize > maxSize)
                newSize = maxSize;
        }

        // Set the size of the array to the new, larger size
        setSize(newSize);

        // Check for allocation failure
        if (currentSize == 0)
            return false;
    }

    // Return success
    return true;
}

// ------------------------------------------------------------------------
// Sets the value at the given index in the array to data
// ------------------------------------------------------------------------
void vsGrowableArray::setData(int index, void *data)
{
    // Verify that the desired position in the array is accessable
    if (!access(index))
        return;

    // Set the entry in the array to the specified data
    storage[index] = data;
}

// ------------------------------------------------------------------------
// Retrieves the data value at the given index in the array
// ------------------------------------------------------------------------
void *vsGrowableArray::getData(int index)
{
    // Verify that the desired position in the array is accessable
    if (!access(index))
        return NULL;

    // Return the specified data
    return (storage[index]);
}

// ------------------------------------------------------------------------
// Retrieves one value from the array as a reference to a pointer. Failed
// accesses return a reference to a dummy pointer area.
// ------------------------------------------------------------------------
void *&vsGrowableArray::operator[](int index)
{
    // Verify that the desired position in the array is accessable; if
    // it's not, return a reference to our dummy data area
    if (!access(index))
        return nowhere;

    // Return a reference to the specified data
    return (storage[index]);
}
