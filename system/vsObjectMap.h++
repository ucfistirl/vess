// File vsObjectMap.h++

#ifndef VS_OBJECT_MAP_HPP
#define VS_OBJECT_MAP_HPP

#include "vsGlobals.h++"

#define VS_OBJMAP_START_SIZE     100
#define VS_OBJMAP_SIZE_INCREMENT 50

enum vsObjectMapList
{
    VS_OBJMAP_FIRST_LIST,
    VS_OBJMAP_SECOND_LIST,
    VS_OBJMAP_EITHER_LIST
};

struct vsObjectMapEntry
{
    void        *firstObj;
    void        *secondObj;
};

class vsObjectMap
{
private:

    vsObjectMapEntry    *objectList;
    int                 objectListSize;
    int                 objectEntryCount;
    
    void                growList();

public:

                vsObjectMap();
                ~vsObjectMap();

    void        registerLink(void *firstObject, void *secondObject);
    int         removeLink(void *theObject, int whichList);
    void        removeAllLinks();

    void        *mapFirstToSecond(void *firstObject);
    void        *mapSecondToFirst(void *secondObject);
};

#endif
