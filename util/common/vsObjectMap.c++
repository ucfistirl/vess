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
//    VESS Module:  vsObjectMap.c++
//
//    Description:  Utility class that implements a list of paired object
//                  pointers
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsObjectMap.h++"

#include <stdio.h>
#include "vsGlobals.h++"

// ------------------------------------------------------------------------
// Constructor - Initializes the object map's internal tree map objects
// ------------------------------------------------------------------------
vsObjectMap::vsObjectMap()
{
    // Initialize a mutex to protect the maps from multiple simultaneous
    // changes (this is possible in a multi-threaded environment)
    pthread_mutex_init(&mapLock, NULL);

    // Create a pair of tree maps to hold the object associations
    firstList = new vsTreeMap();
    secondList = new vsTreeMap();
}

// ------------------------------------------------------------------------
// Destructor - Deletes the object map's internal tree map objects
// ------------------------------------------------------------------------
vsObjectMap::~vsObjectMap()
{
    // Lock the map (we don't want to destroy it while someone else is
    // using it
    lockMap();

    // Destroy the tree maps
    delete firstList;
    delete secondList;

    // Release the map lock
    unlockMap();

    // Destroy the map lock
    pthread_mutex_destroy(&mapLock);
}

// ------------------------------------------------------------------------
// Acquires the map lock to ensure that only this thread will be accessing
// the map
// ------------------------------------------------------------------------
void vsObjectMap::lockMap()
{
   // Lock the map lock
   pthread_mutex_lock(&mapLock);
}

// ------------------------------------------------------------------------
// Releases the map lock, allowing other threads to access the map
// ------------------------------------------------------------------------
void vsObjectMap::unlockMap()
{
   // Lock the map lock
   pthread_mutex_unlock(&mapLock);
}

// ------------------------------------------------------------------------
// Adds a connection between the two given objects to the object map's
// list
// ------------------------------------------------------------------------
void vsObjectMap::registerLink(void *firstObject, void *secondObject)
{
    // Lock the map
    lockMap();

    // Check for duplicates
    if (firstList->containsKey(firstObject))
    {
        printf("vsObjectMap::registerLink: firstObject already appears in "
            "first object list\n");
        return;
    }
    if (secondList->containsKey(secondObject))
    {
        printf("vsObjectMap::registerLink: secondObject already appears in "
            "second object list\n");
        return;
    }
    
    // Add a connection from the first object to the second object to
    // the first map, and a connection from the second object to the
    // first object to the second map
    firstList->addEntry(firstObject, secondObject);
    secondList->addEntry(secondObject, firstObject);

    // Unlock the map
    unlockMap();
}

// ------------------------------------------------------------------------
// Deletes a connection between two objects from the object map's list. The
// whichList constant specifies which list of objects the function should
// search in for the link to delete.
// ------------------------------------------------------------------------
bool vsObjectMap::removeLink(void *theObject, int whichList)
{
    void *otherListObjPtr;

    // Lock the map
    lockMap();

    // Interpret the whichList constant
    switch (whichList)
    {
        case VS_OBJMAP_FIRST_LIST:
            // Search the first map for the specified key
            if (firstList->containsKey(theObject))
            {
                // Determine which object in the second map corresponds
		// to the object in the first, and remove each object
		// from its associated map
                otherListObjPtr = firstList->getValue(theObject);
                firstList->deleteEntry(theObject);
                secondList->deleteEntry(otherListObjPtr);

                // Unlock the map
                unlockMap();

		// Return true to indicate success
                return true;
            }
            break;

        case VS_OBJMAP_SECOND_LIST:
            // Search the second map for the specified key
            if (secondList->containsKey(theObject))
            {
                // Determine which object in the first map corresponds
		// to the object in the second, and remove each object
		// from its associated map
                otherListObjPtr = secondList->getValue(theObject);
                secondList->deleteEntry(theObject);
                firstList->deleteEntry(otherListObjPtr);

                // Unlock the map
                unlockMap();

		// Return true to indicate success
                return true;
            }
            break;

        case VS_OBJMAP_EITHER_LIST:
            // Search the first map for the specified key, and the second
	    // map if the key isn't found in the first
            if (firstList->containsKey(theObject))
            {
                // Determine which object in the second map corresponds
		// to the object in the first, and remove each object
		// from its associated map
                otherListObjPtr = firstList->getValue(theObject);
                firstList->deleteEntry(theObject);
                secondList->deleteEntry(otherListObjPtr);

                // Unlock the map
                unlockMap();

		// Return true to indicate success
                return true;
            }
            if (secondList->containsKey(theObject))
            {
                // Determine which object in the first map corresponds
		// to the object in the second, and remove each object
		// from its associated map
                otherListObjPtr = secondList->getValue(theObject);
                secondList->deleteEntry(theObject);
                firstList->deleteEntry(otherListObjPtr);

                // Unlock the map
                unlockMap();

		// Return true to indicate success
                return true;
            }
            break;
    }

    // Unlock the map
    unlockMap();

    // Return false to indicate that we couldn't find the requested object
    // in the requested map(s)
    return false;
}

// ------------------------------------------------------------------------
// Completely clears out the object map's list of links
// ------------------------------------------------------------------------
void vsObjectMap::removeAllLinks()
{
    // Lock the map
    lockMap();

    // Empty both maps
    firstList->clear();
    secondList->clear();

    // Unlock the map
    unlockMap();
}

// ------------------------------------------------------------------------
// Searches for the given object in the first list of objects, and returns
// the corresponding second object if found.
// ------------------------------------------------------------------------
void *vsObjectMap::mapFirstToSecond(void *firstObject)
{
    void *secondObject;

    // Lock the map
    lockMap();

    // Get the object corresponding to the first object from the first map
    secondObject = firstList->getValue(firstObject);

    // Unlock the map
    unlockMap();

    // Return the object we found (if any)
    return secondObject;
}

// ------------------------------------------------------------------------
// Searches for the given object in the second list of objects, and returns
// the corresponding first object if found.
// ------------------------------------------------------------------------
void *vsObjectMap::mapSecondToFirst(void *secondObject)
{
    void *firstObject;

    // Lock the map
    lockMap();

    // Get the object corresponding to the second object from the second map
    firstObject = secondList->getValue(secondObject);

    // Unlock the map
    unlockMap();

    // Return the object we found (if any)
    return firstObject;
}
