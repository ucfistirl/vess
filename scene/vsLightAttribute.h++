// File vsLightAttribute.h++

#ifndef VS_LIGHT_ATTRIBUTE_HPP
#define VS_LIGHT_ATTRIBUTE_HPP

#include <Performer/pr/pfLight.h>
#include <Performer/pf/pfLightSource.h>
#include "vsAttribute.h++"
#include "vsNode.h++"

enum vsLightAttributeMode
{
    VS_LIGHT_MODE_GLOBAL,
    VS_LIGHT_MODE_LOCAL
};

class vsLightAttribute : public vsAttribute
{
private:

    pfGroup          *lightHookGroup;
    pfLightSource    *lightNode;
    pfLight          *lightObject;
    
    int              lightOn;
    int              lightScope;

VS_INTERNAL:

    int         canAttach();
    void        attach(vsNode *theNode);
    void        detach(vsNode *theNode);

    void        apply();
    void        restoreSaved();

public:

                   vsLightAttribute();
    virtual        ~vsLightAttribute();

    virtual int    getAttributeType();
    virtual int    getAttributeCategory();
    
    void           setAmbientColor(double r, double g, double b);
    void           getAmbientColor(double *r, double *g, double *b);
    void           setDiffuseColor(double r, double g, double b);
    void           getDiffuseColor(double *r, double *g, double *b);
    void           setSpecularColor(double r, double g, double b);
    void           getSpecularColor(double *r, double *g, double *b);
    
    void           setAttenuationVals(double quadratic, double linear,
                                      double constant);
    void           getAttenuationVals(double *quadratic, double *linear,
                                      double *constant);

    void           setPosition(double x, double y, double z, double w);
    void           getPosition(double *x, double *y, double *z, double *w);

    void           setSpotlightDirection(double dx, double dy, double dz);
    void           getSpotlightDirection(double *dx, double *dy, double *dz);
    void           setSpotlightValues(double exponent, double cutoffDegrees);
    void           getSpotlightValues(double *exponent, double *cutoffDegrees);
    
    void           setScope(int scope);
    int            getScope();
    
    void           on();
    void           off();
    int            isOn();
};

#endif
