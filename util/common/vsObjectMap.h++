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

#include "vsTreeMap.h++"

enum  vsObjectMapList
{
    VS_OBJMAP_FIRST_LIST,
    VS_OBJMAP_SECOND_LIST,
    VS_OBJMAP_EITHER_LIST
};

class VS_UTIL_DLL vsObjectMap
{
private:

    vsTreeMap    *firstList;
    vsTreeMap    *secondList;

public:

                vsObjectMap();
                ~vsObjectMap();

    void        registerLink(void *firstObject, void *secondObject);
    bool        removeLink(void *theObject, int whichList);
    void        removeAllLinks();

    void        *mapFirstToSecond(void *firstObject);
    void        *mapSecondToFirst(void *secondObject);
};

#endif
