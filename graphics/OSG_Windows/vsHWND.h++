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
//    VESS Module:  vsOSGNode.c++
//
//    Description:  vsObject wrapper for Microsoft HWND (window handle)
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsObject.h++"

class vsHWND : public vsObject
{
protected:

    HWND   msWindow;

public:

                         vsHWND(HWND theWindow);
    virtual              ~vsHWND();

    virtual const char   *getClassName();

    HWND                 getHWND();

    bool                 equals(atItem *otherItem);
    int                  compare(atItem *otherItem);
};

