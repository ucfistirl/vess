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
//    VESS Module:  vsObjectMap.h++
//
//    Description:  Utility class that implements a list of paired object
//                  pointers
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_OBJECT_MAP_HPP
#define VS_OBJECT_MAP_HPP

#include "vsGrowableArray.h++"

enum vsObjectMapList
{
    VS_OBJMAP_FIRST_LIST,
    VS_OBJMAP_SECOND_LIST,
    VS_OBJMAP_EITHER_LIST
};

class vsObjectMap
{
private:

    vsGrowableArray     firstList;
    vsGrowableArray     secondList;

    int                 objectEntryCount;

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
