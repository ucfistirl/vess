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
//    VESS Module:  vsFogAttribute.h++
//
//    Description:  Specifies that geometry be drawn with fog effects
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_FOG_ATTRIBUTE_HPP
#define VS_FOG_ATTRIBUTE_HPP

#include <Performer/pr/pfFog.h>
#include "vsStateAttribute.h++"

enum vsFogEquationType
{
    VS_FOG_EQTYPE_LINEAR,
    VS_FOG_EQTYPE_EXP,
    VS_FOG_EQTYPE_EXP2
};

class vsFogAttribute : public vsStateAttribute
{
private:

    pfFog              *performerFog;

VS_INTERNAL:

                    vsFogAttribute(pfFog *fogObject);

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();
    virtual void    setState(pfGeoState *state);

public:

                   vsFogAttribute();
    virtual        ~vsFogAttribute();

    virtual int    getAttributeType();

    void           setEquationType(int equType);
    int            getEquationType();
    
    void           setColor(double r, double g, double b);
    void           getColor(double *r, double *g, double *b);
    
    void           setRanges(double near, double far);
    void           getRanges(double *near, double *far);
};

#endif
