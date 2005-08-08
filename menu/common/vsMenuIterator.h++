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
//    VESS Module:  vsMenuIterator.h++
//
//    Description:  This object represents a traversal of all of the
//                  children of a given node in a vsMenuTree structure.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_MENU_ITERATOR_HPP
#define VS_MENU_ITERATOR_HPP

#include "vsGlobals.h++"
#include "vsMenuObject.h++"
#include "vsMenuTree.h++"

class VS_MENU_DLL vsMenuIterator
{
private:

    vsMenuTreeNode    *parentNode;
    vsMenuTreeNode    *currentNode;

public:

                    vsMenuIterator(vsMenuTree *tree, vsMenuFrame *frame);
    virtual         ~vsMenuIterator();

    void            advance();
    void            reset();

    int             getLength();

    vsMenuObject    *getObject();
};

#endif

