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
//    VESS Module:  vsMenuFrame.h++
//
//    Description:  The menu frame class describes a location within a
//                  vsMenuTree.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_MENU_FRAME_HPP
#define VS_MENU_FRAME_HPP

#include "vsGlobals.h++"
#include <stdlib.h>
#include <string>

class VS_MENU_DLL vsMenuFrame
{
private:

    int         maxDepth;
    int         pathDepth;
    int         *pathIndices;

public:

                vsMenuFrame();
                vsMenuFrame(char *path);
                vsMenuFrame(int *indices, int depth);
                vsMenuFrame(vsMenuFrame *frame);
    virtual     ~vsMenuFrame();

    void        setFrame(vsMenuFrame *frame);
    void        setFrame(int *indices, int depth);
    void        setFrame(char *path);

    void        appendIndex(int index);
    void        removeIndex();

    int         getIndex(int depth);
    int         getDepth();

    bool        isEqual(vsMenuFrame *frame);
};

#endif
