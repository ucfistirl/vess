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
//    VESS Module:  vsMenuTree.h++
//
//    Description:  The menu tree class describes a menu structure that is
//                  used by the vsMenuSystem for navigation. The tree is
//                  stored in first-child-next-sibling format, with the
//                  parent node pointer stored for convenience.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_MENU_TREE_HPP
#define VS_MENU_TREE_HPP

#include "vsGlobals.h++"
#include "vsMenuFrame.h++"
#include "vsMenuObject.h++"

struct VESS_SYM vsMenuTreeNode
{
    vsMenuObject      *object;

    vsMenuTreeNode    *parent;
    vsMenuTreeNode    *child;
    vsMenuTreeNode    *sibling;
};

class VESS_SYM vsMenuTree
{
private:

    vsMenuTreeNode    *rootNode;

    void              destroyTree(vsMenuTreeNode *node);

VS_INTERNAL:

    vsMenuTreeNode    *getNode(vsMenuFrame *frame);

public:

                    vsMenuTree();
    virtual         ~vsMenuTree();

    bool            addObject(vsMenuFrame *frame, vsMenuObject *object);
    bool            removeObject(vsMenuFrame *frame);

    int             getChildCount(vsMenuFrame *frame);
    vsMenuObject    *getObject(vsMenuFrame *frame);
};

#endif

