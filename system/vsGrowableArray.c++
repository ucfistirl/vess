// File vsGrowableArray.c++

#include "vsGrowableArray.h++"

#include <stdlib.h>
#include <Performer/pr/pfMemory.h>
#include "vsGlobals.h++"
#include "vsSystem.h++"

// ------------------------------------------------------------------------
// Constructor - Initializes the array's internal storage
// ------------------------------------------------------------------------
vsGrowableArray::vsGrowableArray(int initialSize, int sizeIncrement,
    int sharedMemory)
{
    if (initialSize < 0)
    {
        printf("vsGrowableArray::vsGrowableArray: Invalid initial size\n");
        currentSize = 0;
    }
    else
        currentSize = initialSize;

    stepSize = sizeIncrement;

    maxSize = 32767;
    if (currentSize > maxSize)
        maxSize = currentSize;
    
    nowhere = NULL;
    
    shared = sharedMemory;
    if (shared && (!(vsSystem::systemObject)))
    {
        printf("vsGrowableArray::vsGrowableArray: Can't create in shared "
            "memory before vsSystem object is created. Creating unshared "
            "instead.");
        shared = 0;
    }
    
    // Allocate memory here
    if (shared)
        storage = (void **)(pfMemory::malloc(currentSize * sizeof(void *)));
    else
        storage = (void **)(malloc(currentSize * sizeof(void *)));
    
    if (!storage)
    {
        printf("vsGrowableArray::vsGrowableArray: Unable to allocate memory "
            "for internal array\n");
        currentSize = 0;
    }
}

// ------------------------------------------------------------------------
// Destructor - Destroys the array's internal storage
// ------------------------------------------------------------------------
vsGrowableArray::~vsGrowableArray()
{
    if (storage)
    {
        if (shared)
            pfMemory::free(storage);
        else
            free(storage);
    }
}

// ------------------------------------------------------------------------
// Sets the current size of the array
// ------------------------------------------------------------------------
void vsGrowableArray::setSize(int newSize)
{
    if (shared)
        storage = (void **)(pfMemory::realloc(storage,
            newSize * sizeof(void *)));
    else
        storage = (void **)(realloc(storage, newSize * sizeof(void *)));

    if (storage)
        currentSize = newSize;
    else
    {
        currentSize = 0;
        printf("vsGrowableArray::setSize: Unable to allocate memory for "
            "internal array\n");
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
    maxSize = newMax;
    
    if (maxSize > currentSize)
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
// Sets the value at the given index in the array to data
// ------------------------------------------------------------------------
void vsGrowableArray::setData(int index, void *data)
{
    if (!access(index))
        return;

    storage[index] = data;
}

// ------------------------------------------------------------------------
// Retrieves the data value at the given index in the array
// ------------------------------------------------------------------------
void *vsGrowableArray::getData(int index)
{
    if (!access(index))
        return NULL;

    return (storage[index]);
}

// ------------------------------------------------------------------------
// Retrieves one value from the array as a reference to a pointer. Failed
// accesses return a reference to a dummy pointer area.
// ------------------------------------------------------------------------
void *&vsGrowableArray::operator[](int index)
{
    if (!access(index))
        return nowhere;

    return (storage[index]);
}

// ------------------------------------------------------------------------
// Private function
// Determines if an access of the specified index is possible. Determines
// if the index is valid. Also checks if the array is allocated at the
// desired index, and grows the array to cover the index if needed.
// ------------------------------------------------------------------------
int vsGrowableArray::access(int index)
{
    // Array bounds check
    if ((index < 0) || (index >= maxSize))
    {
        printf("vsGrowableArray::access: Array index out of bounds\n");
        return VS_FALSE;
    }

    // Allocated space check
    if (index >= currentSize)
    {
        int newSize;
        newSize = currentSize;
        while (newSize <= index)
        {
            newSize += stepSize;
            if (newSize > maxSize)
                newSize = maxSize;
        }
        setSize(newSize);

        // Check for allocation failure
        if (currentSize == 0)
            return VS_FALSE;
    }

    return VS_TRUE;
}
