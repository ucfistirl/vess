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
//    VESS Module:  vsObject.h++
//
//    Description:  Reference counting and object validation base class
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_OBJECT_HPP
#define VS_OBJECT_HPP

#include "atItem.h++"
#include "atGlobals.h++"
#include <stdio.h>
#include "vsTreeMap.h++"

#define VS_OBJ_MAGIC_NUMBER 0xFEEDF00D

class VESS_SYM vsObject : public atItem
{
private:

    static vsTreeMap    *currentObjectList;

    int                 magicNumber;
    
    int                 refCount;

public:

                          vsObject();
    virtual               ~vsObject();

    virtual const char    *getClassName() = 0;

    void                  ref();
    void                  unref();
    
    int                   getRefCount();
    
    bool                  isValidObject();

    static void           checkDelete(vsObject *obj);
    static void           unrefDelete(vsObject *obj);

    static void           printCurrentObjects(FILE *outfile);
    static void           deleteObjectList();
};

#endif
