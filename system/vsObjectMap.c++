// File vsObjectMap.c++

#include <stdlib.h>
#include <stdio.h>
#include "vsObjectMap.h++"

// ------------------------------------------------------------------------
// Constructor - Initializes the object map's internal array
// ------------------------------------------------------------------------
vsObjectMap::vsObjectMap()
{
    objectList = (vsObjectMapEntry *)
        malloc(sizeof(vsObjectMapEntry) * VS_OBJMAP_START_SIZE);
    objectListSize = VS_OBJMAP_START_SIZE;
    objectEntryCount = 0;
}

// ------------------------------------------------------------------------
// Destructor - Deletes the object map's internal array
// ------------------------------------------------------------------------
vsObjectMap::~vsObjectMap()
{
    free(objectList);
}

// ------------------------------------------------------------------------
// Adds a connection between the two given objects to the object map's
// list
// ------------------------------------------------------------------------
void vsObjectMap::registerLink(void *firstObject, void *secondObject)
{
    objectList[objectEntryCount].firstObj = firstObject;
    objectList[objectEntryCount].secondObj = secondObject;

    objectEntryCount++;
    if (objectEntryCount == objectListSize)
        growList();
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
                if (theObject == objectList[loop].firstObj)
                {
                    objectList[loop] = objectList[objectEntryCount - 1];
                    objectEntryCount--;
                    return VS_TRUE;
                }
                break;
            case VS_OBJMAP_SECOND_LIST:
                if (theObject == objectList[loop].secondObj)
                {
                    objectList[loop] = objectList[objectEntryCount - 1];
                    objectEntryCount--;
                    return VS_TRUE;
                }
                break;
            case VS_OBJMAP_EITHER_LIST:
                if ((theObject == objectList[loop].firstObj) ||
                    (theObject == objectList[loop].secondObj))
                {
                    objectList[loop] = objectList[objectEntryCount - 1];
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
}

// ------------------------------------------------------------------------
// Searches for the given object in the first list of objects, and returns
// the corresponding second object if found.
// ------------------------------------------------------------------------
void *vsObjectMap::mapFirstToSecond(void *firstObject)
{
    int loop;
    
    for (loop = 0; loop < objectEntryCount; loop++)
        if (firstObject == objectList[loop].firstObj)
            return (objectList[loop].secondObj);

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
        if (secondObject == objectList[loop].secondObj)
            return (objectList[loop].firstObj);

    return NULL;
}

// ------------------------------------------------------------------------
// Private function
// Increases the size of the object map's internal array
// ------------------------------------------------------------------------
void vsObjectMap::growList()
{
    objectListSize += VS_OBJMAP_SIZE_INCREMENT;
    objectList = (vsObjectMapEntry *)realloc(objectList,
        sizeof(vsObjectMapEntry) * objectListSize);
}
