// File vsFogAttribute.h++

#ifndef VS_FOG_ATTRIBUTE_HPP
#define VS_FOG_ATTRIBUTE_HPP

#include <Performer/pr/pfFog.h>
#include "vsSystem.h++"
#include "vsAttribute.h++"

enum vsFogEquationType
{
    VS_FOG_EQTYPE_LINEAR,
    VS_FOG_EQTYPE_EXP,
    VS_FOG_EQTYPE_EXP2
};

class vsFogAttribute : public vsAttribute
{
private:

    pfFog       *performerFog;
    pfFog       *savedFog;

VS_INTERNAL:

                    vsFogAttribute(pfFog *fogObject);

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();

public:

                   vsFogAttribute();
                   ~vsFogAttribute();

    virtual int    getAttributeType();
    
    void           setEquationType(int equType);
    int            getEquationType();
    
    void           setColor(double r, double g, double b);
    void           getColor(double *r, double *g, double *b);
    
    void           setRanges(double near, double far);
    void           getRanges(double *near, double *far);
};

#endif
