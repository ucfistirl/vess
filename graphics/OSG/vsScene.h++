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
//    VESS Module:  vsScene.h++
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

enum vsSceneEarthSkyColor
{
    VS_SCENE_ESCOLOR_SKY_NEAR,
    VS_SCENE_ESCOLOR_SKY_FAR,
    VS_SCENE_ESCOLOR_SKY_HORIZON,
    VS_SCENE_ESCOLOR_GROUND_FAR,
    VS_SCENE_ESCOLOR_GROUND_NEAR,
    VS_SCENE_ESCOLOR_UNIFORM
};

class VS_GRAPHICS_DLL vsScene : public vsNode
{
private:

    vsNode              *child;
    int                 childCount;

    vsLightAttribute    *lightList[VS_LIGHT_MAX];
    
    osg::Group          *osgGroup;

    bool                esEnabled;
    atVector            esUniformColor;

VS_INTERNAL:

    int         addLight(vsLightAttribute *light);
    void        removeLight(vsLightAttribute *light);

    virtual void    getAxisAlignedBoxBounds(atVector *minValues, 
                                            atVector *maxValues);

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

    virtual void            getBoundSphere(atVector *centerPoint,
                                           double *radius);
    virtual atMatrix        getGlobalXform();

    virtual void            setIntersectValue(unsigned int newValue);
    virtual unsigned int    getIntersectValue();

    virtual void            addAttribute(vsAttribute *newAttribute);

    virtual void            enableLighting();
    virtual void            disableLighting();

    virtual void            enableEarthSky();
    virtual void            disableEarthSky();
    virtual bool            isEarthSkyEnabled();
    virtual void            setESGroundHeight(double newHeight);
    virtual double          getESGroundHeight();
    virtual void            setESColor(vsSceneEarthSkyColor which, double r,
                                       double g, double b);
    virtual void            getESColor(vsSceneEarthSkyColor which, double *r,
                                       double *g, double *b);

    virtual void            enableCull();
    virtual void            disableCull();
    
    osg::Group              *getBaseLibraryObject();
};

#endif
