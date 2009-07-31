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
//    VESS Module:  vsList.c++
//
//    Description:  vsObject-based container class that uses the ATLAS
//                  atList container and API.  This class works exactly
//                  like atList, with the added functionality of properly
//                  maintaining vsObject reference counts.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#include "vsList.h++"
#include <stdlib.h>
#include <string.h>


// ------------------------------------------------------------------------
// Constructor.  Initialized the ATLAS atList that we use to store our
// objects
// ------------------------------------------------------------------------
vsList::vsList()
{
    // Initialize the list
    objectList = new atList();
}

// ------------------------------------------------------------------------
// Destructor.  Destroys the list, unreferencing any objects contained
// ------------------------------------------------------------------------
vsList::~vsList()
{
    // Remove all entries from the list (unreferencing them as necessary)
    removeAllEntries();

    // Delete the list
    delete objectList;
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsList::getClassName()
{
    return "vsList";
}

// ------------------------------------------------------------------------
// Return the number of objects contained in the list
// ------------------------------------------------------------------------
u_long vsList::getNumEntries()
{
    return objectList->getNumEntries();
}

// ------------------------------------------------------------------------
// Adds an object to the end of the list
// ------------------------------------------------------------------------
bool vsList::addEntry(vsObject *obj)
{
    bool   result;

    // Try to add the object to the list
    result = objectList->addEntry(obj);

    // If the object is was successfully added, reference it
    if (result == true)
        obj->ref();

    // Return the result of the add
    return result;
}

// ------------------------------------------------------------------------
// Inserts an object into the list at the current position
// ------------------------------------------------------------------------
bool vsList::insertEntry(vsObject *obj)
{
    bool result;

    // Insert the new object into the list
    result = objectList->insertEntry(obj);

    // If the new object made it into the list, reference it
    if (result == true)
        obj->ref();

    // Return the result of the insert
    return result;
}

// ------------------------------------------------------------------------
// Removes the object at the current list position and unreferences it
// ------------------------------------------------------------------------
bool vsList::removeCurrentEntry()
{
    bool     result;

    // Do the remove operation
    result = objectList->removeCurrentEntry();

    // If the remove succeeded and the object is valid, unreference it
    if ((currentObject != NULL) && (result == true))
        vsObject::unrefDelete(currentObject);

    // Return the result of the remove
    return result;
}

// ------------------------------------------------------------------------
// Removes and unreferences all objects from the list
// ------------------------------------------------------------------------
bool vsList::removeAllEntries()
{
    // Go through the list and call unrefDelete on the objects contained
    currentObject = (vsObject *)objectList->getFirstEntry();
    while (currentObject != NULL)
    {
        // Remove the object from the list
        objectList->removeCurrentEntry();

        // Unreference the object, and delete it if the reference count
        // drops to zero
        vsObject::unrefDelete(currentObject);

        // Get the new head of the list
        currentObject = (vsObject *)objectList->getFirstEntry();
    }

    // Return true to indicate success
    return true;
}

// ------------------------------------------------------------------------
// Returns the first object in the list
// ------------------------------------------------------------------------
vsObject *vsList::getFirstEntry()
{
    // Call the corresponding atList method and keep track of the
    // "current" object
    currentObject = (vsObject *)objectList->getFirstEntry();

    // Return the object (may be NULL)
    return currentObject;
}

// ------------------------------------------------------------------------
// Returns the next object in the list, that is, the object after the
// one previously returned by one of the "get" methods
// ------------------------------------------------------------------------
vsObject *vsList::getNextEntry()
{
    // Call the corresponding atList method and keep track of the
    // "current" object
    currentObject = (vsObject *)objectList->getNextEntry();

    // Return the object (may be NULL)
    return currentObject;
}

// ------------------------------------------------------------------------
// Returns the previous object in the list, that is, the object before the
// one previously returned by one of the "get" methods
// ------------------------------------------------------------------------
vsObject *vsList::getPreviousEntry()
{
    // Call the corresponding atList method and keep track of the
    // "current" object
    currentObject = (vsObject *)objectList->getPreviousEntry();

    // Return the object (may be NULL)
    return currentObject;
}

// ------------------------------------------------------------------------
// Returns the last object in the list
// ------------------------------------------------------------------------
vsObject *vsList::getLastEntry()
{
    // Call the corresponding atList method and keep track of the
    // "current" object
    currentObject = (vsObject *)objectList->getLastEntry();

    // Return the object (may be NULL)
    return currentObject;
}

// ------------------------------------------------------------------------
// Returns the n'th object in the list.  This is equivalent to calling
// getFirstEntry(), then getNextEntry() n times, with the value of the
// last call being returned
// ------------------------------------------------------------------------
vsObject *vsList::getNthEntry(u_long n)
{
    // Call the corresponding atList method and keep track of the
    // "current" object
    currentObject = (vsObject *)objectList->getNthEntry(n);

    // Return the object (may be NULL)
    return currentObject;
}

// ------------------------------------------------------------------------
// Finds the first object equivalent to the given object in the list and
// returns it.  Equivalence is determined using the equals() method of
// atItem (potentially overridden by subclasses).
// ------------------------------------------------------------------------
vsObject * vsList::findEntry(vsObject *obj)
{
    // Call the corresponding atList method and keep track of the
    // "current" object
    currentObject = (vsObject *)objectList->findEntry(obj);

    // Return the object (may be NULL)
    return currentObject;
}

