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
#include "vsGrowableArray.h++"

vsTreeMap *vsObject::currentObjectList = NULL;

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
    // Get or create a tree map as a list for all allocated objects
    if (!currentObjectList)
        currentObjectList = new vsTreeMap();

    // Add this object to the object list
    currentObjectList->addEntry(this, NULL);
#endif
}

//------------------------------------------------------------------------
// Destructor - Complains if the object being deleted is invalid or
// still referenced
//------------------------------------------------------------------------
vsObject::~vsObject()
{
    // Error checking
    if (magicNumber != VS_OBJ_MAGIC_NUMBER)
        printf("vsObject::~vsObject: Deletion of invalid object\n");
    else if (refCount != 0)
        printf("vsObject::~vsObject: Deletion of referenced object\n");

    // Remove the magic number so VESS knows this isn't a valid object
    // anymore
    magicNumber = 0;

#ifdef VESS_DEBUG
    // Remove this object from the object list
    if (currentObjectList)
        currentObjectList->deleteEntry(this);
#endif
}

//------------------------------------------------------------------------
// Informs this object that it is being used by another
//------------------------------------------------------------------------
void vsObject::ref()
{
    // Magic number verify
    if (magicNumber != VS_OBJ_MAGIC_NUMBER)
    {
        printf("vsObject::ref: Operation on invalid object\n");
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
        printf("vsObject::unref: Operation on invalid object\n");
        return;
    }
    // Reference count verify
    if (refCount < 1)
    {
        printf("vsObject::unref: Called on unreferenced object\n");
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
int vsObject::isValidObject()
{
    // An object is valid if its magic number is correct
    if (magicNumber == VS_OBJ_MAGIC_NUMBER)
        return VS_TRUE;
    return VS_FALSE;
}

//------------------------------------------------------------------------
// Static function
// Writes a list of currently allocated vsObjects out to the specified
// file
//------------------------------------------------------------------------
void vsObject::printCurrentObjects(FILE *outfile)
{
    vsGrowableArray keyList(1, 1);
    vsGrowableArray valueList(1, 1);
    int listSize, loop;
    vsObject *currentObj;

    // Bail if no object list present
    if (!currentObjectList)
        return;

    // Get the size of the list
    listSize = currentObjectList->getEntryCount();

    // Get a sorted list of keys and values from the tree map
    currentObjectList->getSortedList(&keyList, &valueList);

    // Print all objects that are currently allocated to the output file
    fprintf(outfile, "list of allocated objects (%d):\n", listSize);
    for (loop = 0; loop < listSize; loop++)
    {
        // Get the next object
        currentObj = (vsObject *)(keyList[loop]);

        // Print the object's characteristics to the file
        fprintf(outfile, "  object: %p   refcount = %d   class = \"%s\"   valid = %s\n",
            currentObj, currentObj->getRefCount(), currentObj->getClassName(),
            (currentObj->isValidObject() ? "TRUE" : "FALSE"));
    }
}

//------------------------------------------------------------------------
// Static function
// Writes a list of currently allocated vsObjects out to the specified
// file
//------------------------------------------------------------------------
void vsObject::deleteObjectList()
{
    // Check if the list exists before deleting it
    if (currentObjectList)
    {
        delete currentObjectList;
        currentObjectList = NULL;
    }
}
