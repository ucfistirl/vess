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
//    VESS Module:  vsLightAttribute.h++
//
//    Description:  Specifies that geometry should be drawn as if lit with
//                  the parameters in this object. Multiple lights can
//                  affect the same geometry.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_LIGHT_ATTRIBUTE_HPP
#define VS_LIGHT_ATTRIBUTE_HPP

#define VS_LIGHT_MAX 8

class vsScene;

#include <osg/Light>
#include <osg/LightSource>
#include <osg/State>
#include "vsAttribute.h++"
#include "vsScene.h++"

enum VS_GRAPHICS_DLL vsLightAttributeMode
{
    VS_LIGHT_MODE_GLOBAL,
    VS_LIGHT_MODE_LOCAL
};

class VS_GRAPHICS_DLL vsLightAttribute : public vsAttribute
{
private:

    osg::Group          *lightHookGroup;
    osg::LightSource    *lightNode;
    osg::Light          *lightObject;
 
    bool                lightOn;
    int                 lightScope;

    vsNode              *parentNode;

    vsScene             *scene;

    bool                active;

VS_INTERNAL:

    virtual bool    canAttach();
    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);

    virtual void    attachDuplicate(vsNode *theNode);

    virtual void    apply();
    virtual void    restoreSaved();

    void            setScene(vsScene *newScene);
    vsScene         *getScene();

    bool            addToScene();
    bool            removeFromScene();

    void            enableLocalLight(osg::State *state);
    void            disableLocalLight(osg::State *state);

public:

                          vsLightAttribute();
    virtual               ~vsLightAttribute();

    virtual const char    *getClassName();

    virtual int           getAttributeType();
    virtual int           getAttributeCategory();

    void                  setAmbientColor(double r, double g, double b);
    void                  getAmbientColor(double *r, double *g, double *b);
    void                  setDiffuseColor(double r, double g, double b);
    void                  getDiffuseColor(double *r, double *g, double *b);
    void                  setSpecularColor(double r, double g, double b);
    void                  getSpecularColor(double *r, double *g, double *b);
    
    void                  setAttenuationVals(double quadratic, double linear,
                                             double constant);
    void                  getAttenuationVals(double *quadratic, double *linear,
                                             double *constant);

    void                  setPosition(double x, double y, double z, double w);
    void                  getPosition(double *x, double *y, double *z, double *w);

    void                  setSpotlightDirection(double dx, double dy, double dz);
    void                  getSpotlightDirection(double *dx, double *dy, double *dz);
    void                  setSpotlightValues(double exponent, double cutoffDegrees);
    void                  getSpotlightValues(double *exponent, double *cutoffDegrees);
    
    void                  setScope(int scope);
    int                   getScope();

    void                  on();
    void                  off();
    bool                  isOn();
};

#endif
