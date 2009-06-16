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
//    VESS Module:  vsArray.c++
//
//    Description:  vsObject-based container class that uses the ATLAS
//                  atArray container and API.  This class works exactly
//                  like atArray, with the added functionality of properly
//                  maintaining vsObject reference counts.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#include "vsArray.h++"
#include <stdlib.h>
#include <string.h>


// ------------------------------------------------------------------------
// Constructor.  Initialized the ATLAS atArray that we use to store our
// objects
// ------------------------------------------------------------------------
vsArray::vsArray()
{
    // Initialize the array
    objectArray = new atArray();
}

// ------------------------------------------------------------------------
// Destructor.  Destroys the array, unreferencing any objects contained
// ------------------------------------------------------------------------
vsArray::~vsArray()
{
    // Remove all entries from the array (unreferencing them as necessary)
    removeAllEntries();

    // Delete the array
    delete objectArray;
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsArray::getClassName()
{
    return "vsArray";
}

// ------------------------------------------------------------------------
// Return the number of objects contained in the array (including NULL
// objects that lie between valid objects)
// ------------------------------------------------------------------------
u_long vsArray::getNumEntries()
{
    return objectArray->getNumEntries();
}

// ------------------------------------------------------------------------
// Adds an object to the end of the array
// ------------------------------------------------------------------------
bool vsArray::addEntry(vsObject *obj)
{
    bool   result;

    // Try to add the object to the array
    result = objectArray->addEntry(obj);

    // If the object is valid, and it was successfully added, reference it
    if ((obj != NULL) && (result == true))
        obj->ref();

    // Return the result of the add
    return result;
}

// ------------------------------------------------------------------------
// Sets an object into the array at the given index, and unreferences the
// previous object (if any)
// ------------------------------------------------------------------------
bool vsArray::setEntry(long index, vsObject *obj)
{
    vsObject *oldObj;

    // Set the new object on the array, keeping track of any old object
    // that was there
    oldObj = (vsObject *)objectArray->setEntry(index, obj);

    // If the new object made it into the array, reference it, otherwise
    // return false, indicating we couldn't replace the given entry
    if (objectArray->getEntry(index) == obj)
        obj->ref();
    else
        return false;

    // If the old object is valid, unreference it and delete it
    if (oldObj != NULL)
        vsObject::unrefDelete(oldObj);

    // Return true, indicating the replace succeeded
    return true;
}

// ------------------------------------------------------------------------
// Inserts an object into the array at the given index, shifting the
// following objects over as needed
// ------------------------------------------------------------------------
bool vsArray::insertEntry(long index, vsObject *obj)
{
    bool result;

    // Insert the new object into the array
    result = objectArray->insertEntry(index, obj);

    // If the new object is valid, and it made it into the array, reference it
    if ((obj != NULL) && (result == true))
        obj->ref();

    // Return the result of the insert
    return result;
}

// ------------------------------------------------------------------------
// Removes the object at the given index from the array and unreferences
// it.  Any following objects are slid down into the empty space
// ------------------------------------------------------------------------
bool vsArray::removeEntryAtIndex(long index)
{
    vsObject *oldObj;
    bool     result;

    // Get the object at the given index
    oldObj = (vsObject *)objectArray->getEntry(index);

    // Do the remove operation
    result = objectArray->removeEntryAtIndex(index);

    // If the remove succeeded and the object is valid, unreference it
    if ((oldObj != NULL) && (result == true))
        vsObject::unrefDelete(oldObj);

    // Return the result of the remove
    return result;
}

// ------------------------------------------------------------------------
// Removes the given object from the array and unreferences it.  Any
// following objects are slid down into the empty space
// ------------------------------------------------------------------------
bool vsArray::removeEntry(vsObject *obj)
{
    bool result;

    // Try to remove the given object
    result = objectArray->removeEntry(obj);

    // If the remove succeeded, unreference the object
    if (result)
        vsObject::unrefDelete(obj);

    // Return the result of the remove
    return result;
}

// ------------------------------------------------------------------------
// Removes and unreferences all objects from the array
// ------------------------------------------------------------------------
bool vsArray::removeAllEntries()
{
    long     idx;
    vsObject *obj;

    // Go through the array and call unrefDelete on the objects contained
    idx = objectArray->getNumEntries() - 1;
    while (idx >= 0)
    {
        // Get the object at the end of the array
        obj = (vsObject *)objectArray->getEntry(idx);

        // Remove the object from the array (even if it's NULL)
        objectArray->removeEntryAtIndex(idx);

        // If the object is valid, unreference it, and delete it if the
        // reference count drops to zero
        if (obj != NULL)
            vsObject::unrefDelete(obj);

        // Recompute the index of the last element in the array
        idx = objectArray->getNumEntries() - 1;
    }

    // Return true to indicate success
    return true;
}

// ------------------------------------------------------------------------
// Returns the object at the given index
// ------------------------------------------------------------------------
vsObject *vsArray::getEntry(long index)
{
    // Call the corresponding atArray method and return the result
    return (vsObject *)objectArray->getEntry(index);
}

// ------------------------------------------------------------------------
// Returns the index of the given object in the array
// ------------------------------------------------------------------------
long vsArray::getIndexOf(vsObject *obj)
{
    // Call the corresponding atArray method and return the result
    return objectArray->getIndexOf(obj);
}

