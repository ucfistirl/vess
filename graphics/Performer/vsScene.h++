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
//    VESS Module:  vsScene.c++
//
//    Description:  vsNode subclass that acts as the root of a VESS scene
//                  graph.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_SCENE_HPP
#define VS_SCENE_HPP

class vsLightAttribute;

#include <Performer/pf/pfScene.h>
#include "vsNode.h++"
#include "vsLightAttribute.h++"

class vsScene : public vsNode
{
private:

    vsNode              *child;
    int                 childCount;

    pfScene             *performerScene;

public:

                            vsScene();
    virtual                 ~vsScene();

    virtual const char      *getClassName();

    virtual vsNode          *cloneTree();
    virtual void            deleteTree();

    virtual int             addChild(vsNode *newChild);
    virtual int             insertChild(vsNode *newChild, int index);
    virtual int             removeChild(vsNode *targetChild);
    virtual int             replaceChild(vsNode *targetChild,
                                         vsNode *newChild);

    virtual int             getChildCount();
    virtual vsNode          *getChild(int index);

    virtual int             getNodeType();

    virtual void            getBoundSphere(vsVector *centerPoint,
                                           double *radius);
    virtual vsMatrix        getGlobalXform();

    virtual void            setIntersectValue(unsigned int newValue);
    virtual unsigned int    getIntersectValue();

    virtual void            addAttribute(vsAttribute *newAttribute);

    pfScene                 *getBaseLibraryObject();
};

#endif

