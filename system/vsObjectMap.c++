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
    firstList = new vsTreeMap();
    secondList = new vsTreeMap();
}

// ------------------------------------------------------------------------
// Destructor - Deletes the object map's internal tree map objects
// ------------------------------------------------------------------------
vsObjectMap::~vsObjectMap()
{
    delete firstList;
    delete secondList;
}

// ------------------------------------------------------------------------
// Adds a connection between the two given objects to the object map's
// list
// ------------------------------------------------------------------------
void vsObjectMap::registerLink(void *firstObject, void *secondObject)
{
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
    
    firstList->addEntry(firstObject, secondObject);
    secondList->addEntry(secondObject, firstObject);
}

// ------------------------------------------------------------------------
// Deletes a connection between two objects from the object map's list. The
// whichList constant specifies which list of objects the function should
// search in for the link to delete.
// ------------------------------------------------------------------------
int vsObjectMap::removeLink(void *theObject, int whichList)
{
    void *otherListObjPtr;

    switch (whichList)
    {
        case VS_OBJMAP_FIRST_LIST:
            if (firstList->containsKey(theObject))
            {
                otherListObjPtr = firstList->getValue(theObject);
                firstList->deleteEntry(theObject);
                secondList->deleteEntry(otherListObjPtr);
                return VS_TRUE;
            }
            break;

        case VS_OBJMAP_SECOND_LIST:
            if (secondList->containsKey(theObject))
            {
                otherListObjPtr = secondList->getValue(theObject);
                secondList->deleteEntry(theObject);
                firstList->deleteEntry(otherListObjPtr);
                return VS_TRUE;
            }
            break;

        case VS_OBJMAP_EITHER_LIST:
            if (firstList->containsKey(theObject))
            {
                otherListObjPtr = firstList->getValue(theObject);
                firstList->deleteEntry(theObject);
                secondList->deleteEntry(otherListObjPtr);
                return VS_TRUE;
            }
            if (secondList->containsKey(theObject))
            {
                otherListObjPtr = secondList->getValue(theObject);
                secondList->deleteEntry(theObject);
                firstList->deleteEntry(otherListObjPtr);
                return VS_TRUE;
            }
            break;
    }

    return VS_FALSE;
}

// ------------------------------------------------------------------------
// Completely clears out the object map's list of links
// ------------------------------------------------------------------------
void vsObjectMap::removeAllLinks()
{
    firstList->clear();
    secondList->clear();
}

// ------------------------------------------------------------------------
// Searches for the given object in the first list of objects, and returns
// the corresponding second object if found.
// ------------------------------------------------------------------------
void *vsObjectMap::mapFirstToSecond(void *firstObject)
{
    return (firstList->getValue(firstObject));
}

// ------------------------------------------------------------------------
// Searches for the given object in the second list of objects, and returns
// the corresponding first object if found.
// ------------------------------------------------------------------------
void *vsObjectMap::mapSecondToFirst(void *secondObject)
{
    return (secondList->getValue(secondObject));
}
