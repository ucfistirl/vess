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
//    VESS Module:  vsGrowableArray.h++
//
//    Description:  Utility class that implements a dynamically-sized
//                  array of pointers
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_GROWABLE_ARRAY_HPP
#define VS_GROWABLE_ARRAY_HPP

#include "vsGlobals.h++"

class VESS_SYM vsGrowableArray
{
private:

    void        **storage;
    int         currentSize, stepSize, maxSize;
    void        *nowhere;
    
    bool        access(int index);

public:

                vsGrowableArray(int initialSize, int sizeIncrement);
                ~vsGrowableArray();
    
    void        setSize(int newSize);
    int         getSize();
    
    void        setSizeIncrement(int sizeIncrement);
    int         getSizeIncrement();
    
    void        setMaxSize(int newMax);
    int         getMaxSize();
    
    void        setData(int index, void *data);
    void        *getData(int index);
    
    void        *&operator[](int index);
};

#endif
