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
//    VESS Module:  vsWireframeAttribute.h++
//
//    Description:  Attribute that specifies that geometry should be drawn
//                  in wireframe mode rather than filled
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_WIREFRAME_ATTRIBUTE_HPP
#define VS_WIREFRAME_ATTRIBUTE_HPP

#include "vsStateAttribute.h++"

class vsWireframeAttribute : public vsStateAttribute
{
private:

    int         wireValue;

VS_INTERNAL:

    virtual void    attachDuplicate(vsNode *theNode);

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();
    virtual void    setState(pfGeoState *state);

public:

                   vsWireframeAttribute();
    virtual        ~vsWireframeAttribute();

    virtual int    getAttributeType();

    void           enable();
    void           disable();
    int            isEnabled();
    
};

#endif
