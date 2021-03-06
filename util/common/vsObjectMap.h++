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
//                  pointers.  This class is designed to be thread safe,
//                  so it should be OK for multiple threads to access
//                  and manipulate the map concurrently.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_OBJECT_MAP_HPP
#define VS_OBJECT_MAP_HPP

#include "vsObject.h++"
#include "vsTreeMap.h++"
#include <pthread.h>

enum vsObjectMapList
{
    VS_OBJMAP_FIRST_LIST,
    VS_OBJMAP_SECOND_LIST,
    VS_OBJMAP_EITHER_LIST
};

enum vsObjectMapClearAction
{
    VS_OBJMAP_ACTION_DELETE,
    VS_OBJMAP_ACTION_UNREF_DELETE,
    VS_OBJMAP_ACTION_CHECK_DELETE,
    VS_OBJMAP_ACTION_NONE,
};

class VESS_SYM vsObjectMap : public atNotifier
{
private:

    vsTreeMap         *forwardMap;
    vsTreeMap         *reverseMap;

    pthread_mutex_t   mapLock;


    void              lockMap();
    void              unlockMap();

    bool              validate(vsTreeMap * mapA, vsTreeMap * mapB,
                               char * direction);

public:

                vsObjectMap();
                ~vsObjectMap();

    void        registerLink(vsObject *firstObject, vsObject *secondObject);
    vsObject    *removeLink(vsObject *theObject, int whichList);

    void        removeAllLinks();
    void        removeAllLinks(vsObjectMapClearAction firstListAction,
                               vsObjectMapClearAction secondListAction);

    vsObject    *mapFirstToSecond(vsObject *firstObject);
    vsObject    *mapSecondToFirst(vsObject *secondObject);

    bool        validate(bool printOnError);
};

#endif
