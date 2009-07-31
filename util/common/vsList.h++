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
//    VESS Module:  vsList.h++
//
//    Description:  vsObject-based container class that uses the ATLAS
//                  atList container and API.  This class works exactly
//                  like atList, with the added functionality of properly
//                  maintaining vsObject reference counts.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#ifndef VS_LIST_H
#define VS_LIST_H


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "atList.h++"
#include "vsObject.h++"


class VESS_SYM vsList : public vsObject
{
protected:

      atList     *objectList;
      vsObject   *currentObject;

public:
                             vsList();
      virtual                ~vsList();

      virtual const char     *getClassName();

      virtual u_long         getNumEntries();

      virtual bool           addEntry(vsObject * obj);
      virtual bool           insertEntry(vsObject * obj);
      virtual bool           removeCurrentEntry();
      virtual bool           removeAllEntries();

      virtual vsObject *     getFirstEntry();
      virtual vsObject *     getNextEntry();
      virtual vsObject *     getPreviousEntry();
      virtual vsObject *     getLastEntry();
      virtual vsObject *     getNthEntry(u_long n);

      virtual vsObject       *findEntry(vsObject * obj);
};

#endif

