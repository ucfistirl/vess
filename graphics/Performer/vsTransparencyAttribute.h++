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

enum VS_GRAPHICS_DLL vsTransparencyQuality
{
    VS_TRANSP_QUALITY_DEFAULT,
    VS_TRANSP_QUALITY_FAST,
    VS_TRANSP_QUALITY_HIGH
};

class VS_GRAPHICS_DLL vsTransparencyAttribute : public vsStateAttribute
{
private:

    int         quality;
    bool        occlusion;
    int         transpValue;

VS_INTERNAL:

    virtual void    attachDuplicate(vsNode *theNode);

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();
    virtual void    setState(pfGeoState *state);

    virtual bool    isEquivalent(vsAttribute *attribute);

public:

                          vsTransparencyAttribute();
    virtual               ~vsTransparencyAttribute();

    virtual const char    *getClassName();

    virtual int           getAttributeType();

    void                  enable();
    void                  disable();
    bool                  isEnabled();
    
    void                  setQuality(int newQuality);
    int                   getQuality();

    void                  enableOcclusion();
    void                  disableOcclusion();
    bool                  isOcclusionEnabled();
};

#endif
