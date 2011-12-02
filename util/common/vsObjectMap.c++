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
//                  pointers.  This class is designed to be thread-safe,
//                  so it should be OK for multiple threads to access and
//                  manipulate the map concurrently.
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
    forwardMap = new vsTreeMap();
    reverseMap = new vsTreeMap();
}

// ------------------------------------------------------------------------
// Destructor - Deletes the object map's internal tree map objects
// ------------------------------------------------------------------------
vsObjectMap::~vsObjectMap()
{
    // Lock the map (we don't want to destroy it while someone else is
    // using it
    lockMap();

    // Destroy the maps, but keep the contents intact
    forwardMap->removeAllEntries();
    delete forwardMap;
    reverseMap->removeAllEntries();
    delete reverseMap;

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
// Adds a connection between the two given objects
// ------------------------------------------------------------------------
void vsObjectMap::registerLink(vsObject *firstObject, vsObject *secondObject)
{
    // Lock the map
    lockMap();

    // Check for duplicates
    if (forwardMap->containsKey(firstObject))
    {
        notify(AT_WARN, "vsObjectMap::registerLink: firstObject already "
            "appears in forward object map\n");
        return;
    }
    if (reverseMap->containsKey(secondObject))
    {
        notify(AT_WARN, "vsObjectMap::registerLink: secondObject already "
            "appears in reverse object map\n");
        return;
    }

    // Add a connection from the first object to the second object to
    // the forward map, and a connection from the second object to the
    // first object to the reverse map
    forwardMap->addEntry(firstObject, secondObject);
    reverseMap->addEntry(secondObject, firstObject);

    // Unlock the map
    unlockMap();
}

// ------------------------------------------------------------------------
// Deletes a connection between two objects from the object map's list. The
// whichList constant specifies which list of objects the function should
// search in for the link to delete.
// ------------------------------------------------------------------------
vsObject *vsObjectMap::removeLink(vsObject *theObject, int whichList)
{
    vsObject    *otherListObjPtr;
    atPair      *pair;

    // Lock the map
    lockMap();

    // Test whether we should look in the forward map
    if (whichList != VS_OBJMAP_SECOND_LIST)
    {
        // Search the forward map for the specified key
        if (forwardMap->containsKey(theObject))
        {
            // Determine which object in the reverse map corresponds
            // to the object in the forward, and remove each object
            // from its associated map
            otherListObjPtr = (vsObject *) forwardMap->getValue(theObject);

            // Remove the forward mapping, clearing the pair so its contents
            // are not double-deleted
            pair = forwardMap->removeEntry(theObject);
            pair->removeFirst();
            pair->removeSecond();
            delete pair;

            // Remove the reverse mapping, clearing the pair so its contents
            // are not double-deleted
            pair = reverseMap->removeEntry(otherListObjPtr);
            pair->removeFirst();
            pair->removeSecond();
            delete pair;

            // Unlock the map
            unlockMap();

            // Return the other object pointer
            return otherListObjPtr;
        }
    }

    // Test whether we should look in the reverse map
    if (whichList != VS_OBJMAP_FIRST_LIST)
    {
        // Search the reverse map for the specified key
        if (reverseMap->containsKey(theObject))
        {
            // Determine which object in the forward map corresponds
            // to the object in the reverse, and remove each object
            // from its associated map
            otherListObjPtr = (vsObject *) reverseMap->getValue(theObject);

            // Remove the reverse mapping, clearing the pair so its contents
            // are not double-deleted
            pair = reverseMap->removeEntry(theObject);
            pair->removeFirst();
            pair->removeSecond();
            delete pair;

            // Remove the forward mapping, clearing the pair so its contents
            // are not double-deleted
            pair = forwardMap->removeEntry(otherListObjPtr);
            pair->removeFirst();
            pair->removeSecond();
            delete pair;

            // Unlock the map
            unlockMap();

            // Return the other object pointer
            return otherListObjPtr;
        }
    }

    // Unlock the map
    unlockMap();

    // Indicate that we couldn't find the object in the requested map(s)
    return NULL;
}

// ------------------------------------------------------------------------
// Completely clears out the object map's list of links
// ------------------------------------------------------------------------
void vsObjectMap::removeAllLinks()
{
    // The default behavior is to leave all memory alone
    removeAllLinks(VS_OBJMAP_ACTION_NONE, VS_OBJMAP_ACTION_NONE);
}

// ------------------------------------------------------------------------
// Completely clears out the object map's list of links, handling the keys
// and values based on the specified actions
// ------------------------------------------------------------------------
void vsObjectMap::removeAllLinks(vsObjectMapClearAction firstListAction,
                                 vsObjectMapClearAction secondListAction)
{
    atList firstList;
    atList secondList;
    vsObject *firstObj;
    vsObject *secondObj;

    // Lock the map
    lockMap();

    // Fetch all of the mappings from the forward map
    forwardMap->getSortedList(&firstList, &secondList);

    // Empty both maps using the method that orphans all memory. After
    // this our local lists will have the only references to the values
    forwardMap->removeAllEntries();
    reverseMap->removeAllEntries();

    // Process each list
    firstObj = (vsObject *)firstList.getFirstEntry();
    secondObj = (vsObject *)secondList.getFirstEntry();
    while ((firstObj != NULL) || (secondObj != NULL))
    {
        // Remove the items from the lists so they aren't mistakenly freed
        // when the list goes out of scope
        firstList.removeCurrentEntry();
        secondList.removeCurrentEntry();

        // Handle the first item
        switch (firstListAction)
        {
            case VS_OBJMAP_ACTION_DELETE:
                delete firstObj;
                break;

            case VS_OBJMAP_ACTION_UNREF_DELETE:
                vsObject::unrefDelete(firstObj);
                break;

            case VS_OBJMAP_ACTION_CHECK_DELETE:
                vsObject::checkDelete(firstObj);
                break;

            // Do nothing in either of these cases
            case VS_OBJMAP_ACTION_NONE:
            default:
                break;
        }

        // Handle the second item
        switch (secondListAction)
        {
            case VS_OBJMAP_ACTION_DELETE:
                delete secondObj;
                break;

            case VS_OBJMAP_ACTION_UNREF_DELETE:
                vsObject::unrefDelete(secondObj);
                break;

            case VS_OBJMAP_ACTION_CHECK_DELETE:
                vsObject::checkDelete(secondObj);
                break;

            // Do nothing in either of these cases
            case VS_OBJMAP_ACTION_NONE:
            default:
                break;
        }

        // Move on to the new first entries in the lists
        firstObj = (vsObject *)firstList.getFirstEntry();
        secondObj = (vsObject *)secondList.getFirstEntry();
    }

    // Unlock the map
    unlockMap();
}

// ------------------------------------------------------------------------
// Searches for the given object in the forward list of objects, and
// returns the corresponding second object if found.
// ------------------------------------------------------------------------
vsObject *vsObjectMap::mapFirstToSecond(vsObject *firstObject)
{
    vsObject *secondObject;

    // Lock the map
    lockMap();

    // Get the object corresponding to the first object from the forward map
    secondObject = (vsObject *) forwardMap->getValue(firstObject);

    // Unlock the map
    unlockMap();

    // Return the object we found (if any)
    return secondObject;
}

// ------------------------------------------------------------------------
// Searches for the given object in the reverse map of objects, and returns
// the corresponding first object if found.
// ------------------------------------------------------------------------
vsObject *vsObjectMap::mapSecondToFirst(vsObject *secondObject)
{
    vsObject *firstObject;

    // Lock the map
    lockMap();

    // Get the object corresponding to the second object from the reverse map
    firstObject = (vsObject *) reverseMap->getValue(secondObject);

    // Unlock the map
    unlockMap();

    // Return the object we found (if any)
    return firstObject;
}

// ------------------------------------------------------------------------
// Confirms that all mappings are sane, optionally printing the contents of
// each map if an invalid mapping is found
// ------------------------------------------------------------------------
bool vsObjectMap::validate(bool printOnError)
{
    bool forwardResult;
    bool reverseResult;
    bool mapValid;

    // Indicate our validation
    notify(AT_INFO, "Comparing maps: %d items vs %d items\n",
           forwardMap->getNumEntries(), reverseMap->getNumEntries());

    // Lock the map to prevent modification while we validate its contents
    lockMap();

    // Validate the map in both directions
    forwardResult = validate(forwardMap, reverseMap, "forward");
    reverseResult = validate(reverseMap, forwardMap, "reverse");

    // Determine whether we succeeded
    mapValid = (forwardResult && reverseResult);

    // Test whether we need to print on error
    if ((mapValid == false) && (printOnError == true))
    {
        // Indicate an error
        notify(AT_INFO, "Validation error\n");

        // Print the forward map
        notify(AT_INFO, "Printing forward map\n");
        forwardMap->print();

        // Print the reverse map
        notify(AT_INFO, "Printing reverse map\n");
        reverseMap->print();
    }
    
    // Yield exclusive access to the map
    unlockMap();

    // Return our result
    return mapValid;
}

// ------------------------------------------------------------------------
// Confirms that all mappings from one map to another are valid, that is
// that each key in the first map points to a valid value, that that value
// exists as a key in the second map, and that the value indicated by that
// key in the second map is the original key in the first map. If any of
// these conditions fail for any key, false is returned.
// ------------------------------------------------------------------------
bool vsObjectMap::validate(vsTreeMap * mapA, vsTreeMap * mapB, char * direction)
{
    bool        isValid;
    atList      firstKeys;
    vsObject    *firstItem;
    vsObject    *secondItem;
    vsObject    *firstItemAgain;

    // Begin in the valid state
    isValid = true;

    // Fetch the list of keys in our first map
    mapA->getSortedList(&firstKeys, NULL);

    // Iterate across all entries in the list of keys
    firstItem = (vsObject *)firstKeys.getFirstEntry();
    while (firstItem)
    {
        // Find the item to which our current key maps
        secondItem = (vsObject *)mapA->getValue(firstItem);
        if (secondItem == NULL)
        {
            // Indicate failure
            notify(AT_ERROR, "Invalid %s mapping: %p to NULL\n",
                   direction, firstItem);
            isValid = false;
        }
        else if (mapB->containsKey(secondItem) == false)
        {
            // Indicate failure
            notify(AT_ERROR, "Forward-only %s mapping: %p to %p (no key)\n",
                   direction, firstItem, secondItem);
            isValid = false;
        }
        else
        {
            // Find the item in the first map that the value maps back to
            firstItemAgain = (vsObject *)mapB->getValue(secondItem);
            if (firstItemAgain == NULL)
            {
                // Indicate failure
                notify(AT_ERROR, "Forward-only %s mapping: %p to %p to NULL\n",
                       direction, firstItem, secondItem);
                isValid = false;
            }
            else if (firstItem != firstItemAgain)
            {
                // Indicate failure
                notify(AT_ERROR, "Nonsense %s mapping: %p to %p to %p\n",
                       direction, firstItem, secondItem, firstItemAgain);
                isValid = false;
            }
        }

        // Remove the current (first) entry so it isn't deleted when the list
        // goes out of scope
        firstKeys.removeCurrentEntry();

        // Move on to the new first entry
        firstItem = (vsObject *)firstKeys.getFirstEntry();
    }

    // Return success or failure
    return isValid;
}

