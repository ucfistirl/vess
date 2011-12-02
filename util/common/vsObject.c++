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
//    VESS Module:  vsObject.c++
//
//    Description:  Reference counting and object validation base class
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsGlobals.h++"

#include "vsObject.h++"

vsTreeMap *vsObject::currentObjectList = NULL;
pthread_once_t vsObject::initObjectListOnce = PTHREAD_ONCE_INIT;
pthread_mutex_t vsObject::objectListMutex;

//------------------------------------------------------------------------
// Constructor - Initializes the magic number and reference count
//------------------------------------------------------------------------
vsObject::vsObject()
{
    // Copy the magic number into the first four bytes of the object
    magicNumber = VS_OBJ_MAGIC_NUMBER;

    // Start unreferenced
    refCount = 0;

#ifdef VESS_DEBUG

    // Make sure the vsObject list is initialized (we use a pthread_once to
    // make sure the initialization only happens once)
    pthread_once(&initObjectListOnce, initObjectList);

    // Add this object to the object list
    pthread_mutex_lock(&objectListMutex);
    currentObjectList->addEntry(this, NULL);
    pthread_mutex_unlock(&objectListMutex);

#endif
}

//------------------------------------------------------------------------
// Destructor - Complains if the object being deleted is invalid or
// still referenced
//------------------------------------------------------------------------
vsObject::~vsObject()
{
    atPair *objListEntry;

    // Error checking
    if (magicNumber != VS_OBJ_MAGIC_NUMBER)
        notify(AT_WARN, "vsObject::~vsObject: Deletion of invalid object\n");
    else if (refCount != 0)
        notify(AT_WARN, "vsObject::~vsObject: Deletion of referenced object\n");

#ifdef VESS_DEBUG

    // Lock the object list mutex
    pthread_mutex_lock(&objectListMutex);

    // Remove this object from the object list
    if (currentObjectList)
    {
        // Remove the entry for this object from the global object map
        objListEntry = currentObjectList->removeEntry(this);

        // Remove this object from the pair before deleting it, otherwise the
        // pair will try to free its memory again in its destructor
        objListEntry->removeFirst();
        delete objListEntry;
    }

    // Release the object list mutex
    pthread_mutex_unlock(&objectListMutex);

#endif

    // Remove the magic number so VESS knows this isn't a valid object
    // anymore
    magicNumber = 0;
}

//------------------------------------------------------------------------
// Initalize the object list that keeps track of all allocated VESS
// object
//------------------------------------------------------------------------
void vsObject::initObjectList()
{
    // Create a mutex to protect the object list (very necessary if we're
    // running multiple threads)
    pthread_mutex_init(&objectListMutex, NULL);

    // Create a tree map as a list for all allocated objects
    currentObjectList = new vsTreeMap();
}

//------------------------------------------------------------------------
// Informs this object that it is being used by another
//------------------------------------------------------------------------
void vsObject::ref()
{
    // Magic number verify
    if (magicNumber != VS_OBJ_MAGIC_NUMBER)
    {
        notify(AT_WARN, "vsObject::ref: Operation on invalid object\n");
        return;
    }
    
    // Increment the reference count
    refCount++;
}

//------------------------------------------------------------------------
// Informs this object that it is no longer being used by another
//------------------------------------------------------------------------
void vsObject::unref()
{
    // Magic number verify
    if (magicNumber != VS_OBJ_MAGIC_NUMBER)
    {
        notify(AT_WARN, "vsObject::unref: Operation on invalid object\n");
        return;
    }

    // Reference count verify
    if (refCount < 1)
    {
        notify(AT_WARN, "vsObject::unref: Called on unreferenced object\n");
        return;
    }
    
    // Decrement the reference count
    refCount--;
}

//------------------------------------------------------------------------
// Returns the number of objects using this one
//------------------------------------------------------------------------
int vsObject::getRefCount()
{
    return refCount;
}

//------------------------------------------------------------------------
// Checks the value of the magic number and returns true if it is what it
// should be or false if not
//------------------------------------------------------------------------
bool vsObject::isValidObject()
{
    // An object is valid if its magic number is correct
    if (magicNumber == VS_OBJ_MAGIC_NUMBER)
        return true;
    return false;
}

//------------------------------------------------------------------------
// Static function
// Deletes the given object if its reference count is zero
//------------------------------------------------------------------------
void vsObject::checkDelete(vsObject *obj)
{
    if (obj->getRefCount() == 0)
        delete obj;
}

//------------------------------------------------------------------------
// Static function
// Unreferences the given object, then deletes the object if its reference
// count is zero
//------------------------------------------------------------------------
void vsObject::unrefDelete(vsObject *obj)
{
    if (obj->getRefCount() > 0)
        obj->unref();
    if (obj->getRefCount() == 0)
        delete obj;
}

//------------------------------------------------------------------------
// Static function
// Writes a list of currently allocated vsObjects out to the specified
// file
//------------------------------------------------------------------------
void vsObject::printCurrentObjects(FILE *outfile)
{
    atList keyList;
    int listSize;
    vsObject *currentObj;

    // Bail if no object list present
    if (!currentObjectList)
        return;

    // Get the size of the list
    listSize = currentObjectList->getNumEntries();

    // Get a sorted list of keys and values from the tree map
    currentObjectList->getSortedList(&keyList, NULL);

    // Print all objects that are currently allocated to the output file
    fprintf(outfile, "list of allocated objects (%d):\n", listSize);
    currentObj = (vsObject *) keyList.getFirstEntry();
    while (currentObj != NULL)
    {
        // Print the object's characteristics to the file
        fprintf(outfile, "  object: %p   refcount = %d   class = \"%s\"   valid = %s\n",
            currentObj, currentObj->getRefCount(), currentObj->getClassName(),
            (currentObj->isValidObject() ? "TRUE" : "FALSE"));

        // Next object
        currentObj = (vsObject *) keyList.getNextEntry();
    }

    // Remove the keys so they aren't deleted when the list goes out of scope
    keyList.removeAllEntries();
}

//------------------------------------------------------------------------
// Static function
// Deletes any vsObject type items that still exist
//------------------------------------------------------------------------
void vsObject::deleteObjectList()
{
    // Check if the list exists before deleting it
    if (currentObjectList)
    {
        // Remove all items from the map before deleting it
        currentObjectList->removeAllEntries();
        delete currentObjectList;
        currentObjectList = NULL;

        // Get rid of the mutex that protects this object as well
        pthread_mutex_destroy(&objectListMutex);
    }
}

