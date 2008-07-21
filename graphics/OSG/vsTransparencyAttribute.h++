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

#include <osg/Depth>
#include "vsStateAttribute.h++"

enum vsTransparencyQuality
{
    VS_TRANSP_QUALITY_DEFAULT,
    VS_TRANSP_QUALITY_FAST,
    VS_TRANSP_QUALITY_HIGH
};

class VESS_SYM vsTransparencyAttribute : public vsStateAttribute
{
private:

    bool            occlusion;
    int             quality;
    bool            transpValue;
    
    osg::Depth      *osgDepth;

    virtual void    setOSGAttrModes(vsNode *node);

VS_INTERNAL:

    virtual void    attach(vsNode *node);
    virtual void    detach(vsNode *node);

    virtual void    attachDuplicate(vsNode *theNode);

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();
    virtual void    setState(osg::StateSet *stateSet);

    virtual bool    isEquivalent(vsAttribute *attribute);

public:

                           vsTransparencyAttribute();
    virtual                ~vsTransparencyAttribute();

    virtual const char     *getClassName();

    virtual int            getAttributeType();
    virtual vsAttribute    *clone();

    void                   enable();
    void                   disable();
    bool                   isEnabled();
    
    void                   setQuality(int newQuality);
    int                    getQuality();

    void                   enableOcclusion();
    void                   disableOcclusion();
    bool                   isOcclusionEnabled();
};

#endif
