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
// Constructor - Initializes the object map's internal arrays
// ------------------------------------------------------------------------
vsObjectMap::vsObjectMap() : firstList(100, 50), secondList(100, 50)
{
    objectEntryCount = 0;
}

// ------------------------------------------------------------------------
// Destructor - Deletes the object map's internal array
// ------------------------------------------------------------------------
vsObjectMap::~vsObjectMap()
{
}

// ------------------------------------------------------------------------
// Adds a connection between the two given objects to the object map's
// list
// ------------------------------------------------------------------------
void vsObjectMap::registerLink(void *firstObject, void *secondObject)
{
    firstList[objectEntryCount] = firstObject;
    secondList[objectEntryCount] = secondObject;

    objectEntryCount++;
}

// ------------------------------------------------------------------------
// Deletes a connection between two objects from the object map's list. The
// whichList constant specifies which list of objects the function should
// search in for the link to delete.
// ------------------------------------------------------------------------
int vsObjectMap::removeLink(void *theObject, int whichList)
{
    int loop;
    
    for (loop = 0; loop < objectEntryCount; loop++)
        switch (whichList)
        {
            case VS_OBJMAP_FIRST_LIST:
                if (theObject == firstList[loop])
                {
                    firstList[loop] = firstList[objectEntryCount - 1];
                    secondList[loop] = secondList[objectEntryCount - 1];
                    objectEntryCount--;
                    return VS_TRUE;
                }
                break;
            case VS_OBJMAP_SECOND_LIST:
                if (theObject == secondList[loop])
                {
                    firstList[loop] = firstList[objectEntryCount - 1];
                    secondList[loop] = secondList[objectEntryCount - 1];
                    objectEntryCount--;
                    return VS_TRUE;
                }
                break;
            case VS_OBJMAP_EITHER_LIST:
                if ((theObject == firstList[loop]) ||
                    (theObject == secondList[loop]))
                {
                    firstList[loop] = firstList[objectEntryCount - 1];
                    secondList[loop] = secondList[objectEntryCount - 1];
                    objectEntryCount--;
                    return VS_TRUE;
                }
                break;
            default:
                printf("vsObjectMap::removeList: Bad list specifier\n");
                return VS_FALSE;
        }

    return VS_FALSE;
}

// ------------------------------------------------------------------------
// Completely clears out the object map's list of links
// ------------------------------------------------------------------------
void vsObjectMap::removeAllLinks()
{
    objectEntryCount = 0;
    firstList.setSize(100);
    secondList.setSize(100);
}

// ------------------------------------------------------------------------
// Searches for the given object in the first list of objects, and returns
// the corresponding second object if found.
// ------------------------------------------------------------------------
void *vsObjectMap::mapFirstToSecond(void *firstObject)
{
    int loop;
    
    for (loop = 0; loop < objectEntryCount; loop++)
        if (firstObject == firstList[loop])
            return (secondList[loop]);

    return NULL;
}

// ------------------------------------------------------------------------
// Searches for the given object in the second list of objects, and returns
// the corresponding first object if found.
// ------------------------------------------------------------------------
void *vsObjectMap::mapSecondToFirst(void *secondObject)
{
    int loop;
    
    for (loop = 0; loop < objectEntryCount; loop++)
        if (secondObject == secondList[loop])
            return (firstList[loop]);

    return NULL;
}
