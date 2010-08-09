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
//    VESS Module:  vsRenderBin.c++
//
//    Description:  Class to represent a render bin, into which elements
//                  of the scene are sorted after culling and before
//                  drawing.  Render bins have numbers to indicate in
//                  which order they are drawn, and they also have a
//                  sorting mode (depth sort or state sort).  By default
//                  all geometry uses render bin 0, and bin 0 defaults to
//                  state sorting.  
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_RENDER_BIN_HPP
#define VS_RENDER_BIN_HPP

#include "vsObject.h++"
#include "vsArray.h++"

enum vsRenderBinSortMode
{
    VS_RENDER_BIN_SORT_STATE,
    VS_RENDER_BIN_SORT_DEPTH
};

class VESS_SYM vsRenderBin : public vsObject
{
private:

    static vsArray        renderBinList;

    int                   binNumber;
    vsRenderBinSortMode   sortMode;

                          vsRenderBin(int number);
    virtual               ~vsRenderBin();

VS_INTERNAL:

    static bool           binModesChanged;

public:

    static vsRenderBin *          getBin(int number);

    virtual const char            *getClassName();

    virtual int                   getNumber();
    virtual vsRenderBinSortMode   getSortMode();

    virtual void                  setSortMode(vsRenderBinSortMode newMode);
};

#endif

