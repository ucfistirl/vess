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
//    VESS Module:  vsComponent.c++
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

#include <osg/Group>
#include "vsNode.h++"
#include "vsLightAttribute.h++"

class VS_GRAPHICS_DLL vsScene : public vsNode
{
private:

    vsNode              *child;
    int                 childCount;

    vsLightAttribute    *lightList[VS_LIGHT_MAX];
    
    osg::Group          *osgGroup;

VS_INTERNAL:
                
    int         addLight(vsLightAttribute *light);
    void        removeLight(vsLightAttribute *light);

public:

                            vsScene();
    virtual                 ~vsScene();
    
    virtual const char      *getClassName();

    virtual vsNode          *cloneTree();

    virtual bool            addChild(vsNode *newChild);
    virtual bool            insertChild(vsNode *newChild, int index);
    virtual bool            removeChild(vsNode *targetChild);
    virtual bool            replaceChild(vsNode *targetChild, 
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

    virtual void            enableCull();
    virtual void            disableCull();
    
    osg::Group              *getBaseLibraryObject();
};

#endif
