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
    // Create a pair of tree maps to hold the object associations
    firstList = new vsTreeMap();
    secondList = new vsTreeMap();
}

// ------------------------------------------------------------------------
// Destructor - Deletes the object map's internal tree map objects
// ------------------------------------------------------------------------
vsObjectMap::~vsObjectMap()
{
    // Destroy the tree maps
    delete firstList;
    delete secondList;
}

// ------------------------------------------------------------------------
// Adds a connection between the two given objects to the object map's
// list
// ------------------------------------------------------------------------
void vsObjectMap::registerLink(void *firstObject, void *secondObject)
{
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
}

// ------------------------------------------------------------------------
// Deletes a connection between two objects from the object map's list. The
// whichList constant specifies which list of objects the function should
// search in for the link to delete.
// ------------------------------------------------------------------------
bool vsObjectMap::removeLink(void *theObject, int whichList)
{
    void *otherListObjPtr;

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
                return true;
            }
            break;
    }

    return false;
}

// ------------------------------------------------------------------------
// Completely clears out the object map's list of links
// ------------------------------------------------------------------------
void vsObjectMap::removeAllLinks()
{
    // Empty both maps
    firstList->clear();
    secondList->clear();
}

// ------------------------------------------------------------------------
// Searches for the given object in the first list of objects, and returns
// the corresponding second object if found.
// ------------------------------------------------------------------------
void *vsObjectMap::mapFirstToSecond(void *firstObject)
{
    // Get the object corresponding to the first object from the first map
    return (firstList->getValue(firstObject));
}

// ------------------------------------------------------------------------
// Searches for the given object in the second list of objects, and returns
// the corresponding first object if found.
// ------------------------------------------------------------------------
void *vsObjectMap::mapSecondToFirst(void *secondObject)
{
    // Get the object corresponding to the second object from the second map
    return (secondList->getValue(secondObject));
}
