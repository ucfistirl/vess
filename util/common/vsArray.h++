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
//    VESS Module:  vsArray.h++
//
//    Description:  vsObject-based container class that uses the ATLAS
//                  atArray container and API.  This class works exactly
//                  like atArray, with the added functionality of properly
//                  maintaining vsObject reference counts.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#ifndef VS_ARRAY_H
#define VS_ARRAY_H


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "atArray.h++"
#include "vsObject.h++"


class VESS_SYM vsArray : public vsObject
{
protected:

      atArray   *objectArray;

public:
                             vsArray();
      virtual                ~vsArray();

      virtual const char     *getClassName();

      virtual u_long         getNumEntries();

      virtual bool           addEntry(vsObject * obj);
      virtual bool           setEntry(long index, vsObject * obj);
      virtual bool           insertEntry(long index, vsObject * obj);
      virtual bool           removeEntry(vsObject * obj);
      virtual bool           removeEntryAtIndex(long index);
      virtual bool           removeAllEntries();

      virtual vsObject       *getEntry(long index);
      virtual long           getIndexOf(vsObject * obj);
};

#endif

