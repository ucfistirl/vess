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
//    VESS Module:  vsBackfaceAttribute.h++
//
//    Description:  Attribute for specifying the visibility of back-facing
//                  geometry
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_BACKFACE_ATTRIBUTE_HPP
#define VS_BACKFACE_ATTRIBUTE_HPP

#include "vsStateAttribute.h++"

class vsBackfaceAttribute : public vsStateAttribute
{
private:

    pfLightModel    *lightModel;
    int             cullfaceVal;

VS_INTERNAL:

    virtual void    attachDuplicate(vsNode *theNode);

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();
    virtual void    setState(pfGeoState *state);
    
    virtual int     isEquivalent(vsAttribute *attribute);

public:

                          vsBackfaceAttribute();
    virtual               ~vsBackfaceAttribute();

    virtual const char    *getClassName();

    virtual int           getAttributeType();

    void                  enable();
    void                  disable();
    int                   isEnabled();
};

#endif
