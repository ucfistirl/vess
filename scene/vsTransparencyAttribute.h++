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
//    VESS Module:  vsTransparencyAttribute.h++
//
//    Description:  Attribute that specifies that geometry contains
//                  transparent or translucent parts and should be drawn
//                  accordingly
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_TRANSPARENCY_ATTRIBUTE_HPP
#define VS_TRANSPARENCY_ATTRIBUTE_HPP

#include "vsStateAttribute.h++"

class vsTransparencyAttribute : public vsStateAttribute
{
private:

    int         transpValue;

VS_INTERNAL:

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();
    virtual void    setState(pfGeoState *state);

public:

                   vsTransparencyAttribute();
    virtual        ~vsTransparencyAttribute();

    virtual int    getAttributeType();

    void           enable();
    void           disable();
    int            isEnabled();
};

#endif
